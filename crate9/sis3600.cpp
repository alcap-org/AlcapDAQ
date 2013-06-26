#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "midas.h"

#include "crate.h"
#include "vme.h"
#include "odb_wrapper.h"
#include "diag.h"

INT sis3600_bor();
INT sis3600_eor();
INT sis3600_poll_live();
INT sis3600_read(char *pevent);

struct readout_module sis3600_module = {
  NULL,                 // init
  NULL,                 // exit
  NULL,                 // pre_bor
  sis3600_bor,          // bor
  sis3600_eor,          // eor
  sis3600_poll_live,    // poll_live
  NULL,                 // poll_dead
  NULL,                 // start_cycle
  NULL,                 // stop_cycle
  sis3600_read,         // read
};



#define SIS3600_MAX_DMA_SIZE        256
#define SIS3600_MAX_SIZE            (32*1024*4)
#define ACTIVE_READOUT_BUFFER_SIZE  (32*1024*4)
#define ACTIVE_READOUT_MAX_PER_POLL (2*1024)

struct sis3600 {
  bool enabled;
  bool do_active_readout;
  struct vme_handle *vme_handle;
  unsigned long vme_base;
  char odb_name[20];
  char bank_name[20];
  int active_readout_size;
  unsigned char active_readout_buffer[ACTIVE_READOUT_BUFFER_SIZE];
};

#define MAX_SIS3600 4
struct sis3600 sis3600[MAX_SIS3600];

#define SIS3600_CSR           0x000
#define SIS3600_KA_CLEAR      0x020
#define SIS3600_KA_ENABLENEXT 0x028
#define SIS3600_KA_RESET      0x060
#define SIS3600_OUTBUF        0x100

#define SIS3600_CSR_FIFO_EMPTY_MASK        0x00000100
#define SIS3600_CSR_FIFO_ALMOST_EMPTY_MASK 0x00000200

/*
 * sis3600_bor1
 *
 * Initialize one SIS3600 module.
 */
INT sis3600_bor1(struct sis3600 *sis)
{
  // Get the VME base address of the module
  sis->vme_base = odb_get_dword("/Equipment/Crate %d/Settings/%s/vme address", 
		    crate_number, sis->odb_name);

  // Open a VME handle for memory-mapped access
  struct vme_mapping_ctrl mapping = { 
	  VMEMAP_DWIDTH_32, 
	  VMEMAP_ASPACE_A32, 
	  VMEMAP_SUPUSERAM_SUPER, 
	  VMEMAP_PRGDATAAM_DATA 
  };

  struct vme_handle *handle = 
	  vme_open(sis->vme_base, mapping, 0x200, SIS3600_MAX_DMA_SIZE);
  sis->vme_handle = handle;

  // Reset
  vme_write_d32(handle, sis->vme_base | SIS3600_KA_RESET, 0x1);

  // Clear for the next block
  sis->active_readout_size = 0;

  // Read back current status register and check it
  DWORD csr = vme_read_d32(handle, sis->vme_base | SIS3600_CSR);
  if(csr != 0x00000300) {
    diag_print(0, "Unexpected status 0x%08x on %s after reset, disabling\n", 
      csr, sis->odb_name);
    sis->enabled = false;
    return FE_ERR_HW;
  }

  // Continue setup
  vme_write_d32(handle, sis->vme_base | SIS3600_CSR, 0x08010000);
  vme_write_d32(handle, sis->vme_base | SIS3600_KA_CLEAR, 0x1);
  vme_write_d32(handle, sis->vme_base | SIS3600_KA_ENABLENEXT, 0x1);

  // Again check the status register value
  csr = vme_read_d32(handle, sis->vme_base | SIS3600_CSR);
  if(csr != 0x00018300) {
    diag_print(0, "Unexpected status 0x%08x on %s after setup, disabling\n", 
      csr, sis->odb_name);
    sis->enabled = false;
    return FE_ERR_HW;
  }

  // Determine whether we're supposed to do active readout
  sis->do_active_readout =
    odb_get_bool("/Equipment/Crate %d/Settings/%s/Active Readout mode",
		    crate_number, sis->odb_name);

  return SUCCESS;
}

/*
 * sis3600_bor
 *
 * Called at the beginning of the run to discover SIS3600 modules
 * and initialize them.
 */
INT sis3600_bor()
{
  // Use the ODB to find any SIS3600 modules
  for(int j = 0; j < MAX_SIS3600; j++) {

    bool enabled = false;

    if (odb_find_key("/Equipment/Crate %d/Settings/COMP %d", crate_number, j)) {
      diag_print(1, "ODB says COMP %d is present, ", j);
      enabled = 
	odb_get_bool("/Equipment/Crate %d/Settings/COMP %d/enabled status", 
            crate_number, j);
      if (enabled) {
        diag_print(1, "and is enabled.  Initializing...\n");
      } else {
        diag_print(1, "but is disabled.\n");
      }
    }

    sis3600[j].enabled = enabled;

    // Set up the name of the MIDAS bank associated with the module
    sprintf(sis3600[j].bank_name, "CMP%d", j);
    sprintf(sis3600[j].odb_name, "COMP %d", j);

    if(enabled) {
      sis3600_bor1(&sis3600[j]);
    }
  }

  return SUCCESS;
}

INT sis3600_eor1(struct sis3600 *sis)
{
  vme_close(sis->vme_handle);
  return SUCCESS;
}

/*
 * sis3600_eor
 *
 * Called at the end of the run to release any resources that may have
 * been allocated.
 */
INT sis3600_eor()
{

  for(int i = 0; i < MAX_SIS3600; i++) {
    if(sis3600[i].enabled) {
      int status = sis3600_eor1(&sis3600[i]);
      if(status != SUCCESS) {
         return status;
      }
    }
  }

  return SUCCESS;
}

INT sis3600_fifo_read(struct sis3600 *sis, unsigned char *buffer, 
		      int max_size, bool in_active_readout)
{
  // If we're not in the active readout phase, then we simply do a big DMA.
  // There is a 50% chance of losing one word at the end of the block.
  if(!in_active_readout) {
    int status = vme_dma_read(sis->vme_handle,
			      sis->vme_base | SIS3600_OUTBUF, 
			      buffer,
			      max_size);
    return status;
  } 

  // If we are in the active readout phase, then we have to be more careful.
  // Before initiating a block transfer, we check whether the CSR 
  // indicates "almost empty."  If it does not, a 256 byte (64 word)
  // block transfer is safe.  If it does, then we return nothing; we'll 
  // be back when there is enough data to bother with.
  int size = 0;
  while(size < max_size) {

    DWORD csr = vme_read_d32(sis->vme_handle, sis->vme_base | SIS3600_CSR);
    if(csr & SIS3600_CSR_FIFO_ALMOST_EMPTY_MASK) {
      return size;  
    }

    int size_this_time = MIN(max_size - size, 256);

    int status = vme_dma_read(sis->vme_handle,
			      sis->vme_base | SIS3600_OUTBUF, 
			      buffer + size,
			      size_this_time);

    if(status >= 0) {
      size += status;
    }

    if(status != size_this_time) {
      diag_print(0, "Retrieved only %d bytes from %s", status, sis->odb_name);
    }
  }

  return size;
}

/*
 * sis3600_poll1
 *
 * Performs active readout for a single SIS3600 FIFO.
 */
INT sis3600_poll1(struct sis3600 *sis)
{
  // If active readout is not enabled, skip it.
  if(!sis->do_active_readout) {
    return SUCCESS;
  }

  // First check how much space we have available in the active
  // readout buffer.
  int size_left = ACTIVE_READOUT_BUFFER_SIZE - sis->active_readout_size;
  int size = MIN(ACTIVE_READOUT_MAX_PER_POLL, size_left);
  
  // Now try to read up to that amount.
  int status = sis3600_fifo_read(sis, 
		      sis->active_readout_buffer + sis->active_readout_size,
		      size, TRUE);
			    
  // Check the status
  if(status >= 0) {
    sis->active_readout_size += status;
  } else {
    diag_print(0, "Status from sis3600_fifo_read is %d for %s.\n", status,
      sis->odb_name);
    return FE_ERR_HW;
  }

  if(sis->active_readout_size == ACTIVE_READOUT_BUFFER_SIZE) {
    return FE_NEED_STOP;
  } else {
    return SUCCESS;
  }
}

/*
 * sis3600_poll_live
 *
 * Called periodically while a block is active; performs active readout.
 *
 * Returns:
 * - ordinarily 0,
 * - a request for a "soft stop" end-of-block, or
 * - an error code
 */
INT sis3600_poll_live()
{
  for(int i = 0; i < MAX_SIS3600; i++) {
    if(sis3600[i].enabled) {
      int status = sis3600_poll1(&sis3600[i]);
      if(status != SUCCESS) {
        return status; 
      }
    }
  }

  return SUCCESS;
}

/*
 * sis3600_read1
 *
 * Constructs the MIDAS bank for a single SIS3600.
 */
INT sis3600_read1(struct sis3600 *sis, char *pevent)
{
  // Create the MIDAS bank
  DWORD *pdata;
  bk_create(pevent, sis->bank_name, TID_DWORD, &pdata);

  // Copy data that was read during the active readout
  int active_size = sis->active_readout_size;
  memcpy(pdata, sis->active_readout_buffer, active_size);

  // Read any data remaining in the module
  int status = 
    sis3600_fifo_read(sis,  ((unsigned char *) pdata) + active_size,
	 	      SIS3600_MAX_SIZE, FALSE);
  
  // Check the status
  int final_size = 0;
  if(status >= 0) {
    final_size = status;
  } else {
    // handle the error
  }

  // Close the bank
  bk_close(pevent, pdata + (active_size + final_size)/sizeof(DWORD));

  // Clear for the next block
  sis->active_readout_size = 0;

  // Clear FIFO
  vme_write_d32(sis->vme_handle, sis->vme_base | SIS3600_KA_CLEAR, 0x1);

  return SUCCESS;
}

/*
 * sis3600_read
 *
 * Called at the end of a block to assemble data from that block into a 
 * MIDAS event.
 */
INT sis3600_read(char *pevent)
{
  for(int i = 0; i < MAX_SIS3600; i++) {
    if(sis3600[i].enabled) {
      int status = sis3600_read1(&sis3600[i], pevent);
       if(status != SUCCESS) {
         return status;
       }
    }
  }

  return SUCCESS;
}
