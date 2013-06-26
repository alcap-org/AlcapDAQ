#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <time.h>

#include "midas.h"

#include "crate.h"
#include "vme.h"
#include "odb_wrapper.h"
#include "diag.h"

INT caenV767_bor();
INT caenV767_eor();
INT caenV767_poll_live();
INT caenV767_read(char *pevent);

struct readout_module caenV767_module = {
  NULL,                 // init
  NULL,                 // exit
  NULL,                 // pre_bor
  caenV767_bor,          // bor
  caenV767_eor,          // eor
  caenV767_poll_live,    // poll_live
  NULL,                 // poll_dead
  NULL,                 // start_cycle
  NULL,                 // stop_cycle
  caenV767_read,         // read
};

#define CAENV767_MAX_DMA_SIZE        256
#define CAENV767_MAX_SIZE            (32*1024*4)
#define ACTIVE_READOUT_MAX_PER_POLL (2*1024)
#define CAENV767_NUM_CHANNELS       128

struct caenV767 {
  bool enabled;
  bool do_active_readout;
  bool record_trailing_edges;
  struct vme_handle *vme_handle;
  unsigned long vme_base;
  char odb_name[20];
  char bank_name[20];
  int active_readout_size;
  unsigned char *active_readout_buffer;
  int active_readout_buffer_size;
  int half_full_level;
  double bytes_per_second;
};

struct timeval block_start_time;

#define MAX_CAENV767 4
struct caenV767 caenV767[MAX_CAENV767];

// Register offsets
#define CAENV767_OUTBUF        0x00
#define CAENV767_CTR1          0x10
#define CAENV767_STR2          0x48
#define CAENV767_EVT_COUNT     0x4C
#define CAENV767_OPHAND        0x50
#define CAENV767_OPCODE        0x52
#define CAENV767_CLEAR         0x54

// Reserved value returned in output buffer when the FIFO is empty
#define CAENV767_NOT_VALID     0x00600000

// Bits in status register 2
#define CAENV767_GLOBAL_TDC_ERROR  0x0008

/*
  Complete function listing

  void caenV767_opcode_write(struct caenV767 *caen, short code);
  short caenV767_opcode_read(struct caenV767 *caen);
  INT caenV767_bor1(struct caenV767 *caen)
  INT caenV767_bor()
  INT caenV767_eor1(struct caenV767 *caen)
  INT caenV767_eor()
  INT caenV767_fifo_read(struct caenV767 *caen, unsigned char *buffer, 
	  	         int max_size, bool in_active_readout)
  INT caenV767_poll1(struct caenV767 *caen)
  INT caenV767_poll_live()
  INT caenV767_read1(struct caenV767 *caen, char *pevent)
  INT caenV767_read(char *pevent)
*/

/* ********************************************************************* */
void caenV767_opcode_write(struct caenV767 *caen, short code)
{
  // Wait for the microcontroller to become ready
  while(!(vme_read_d16(caen->vme_handle, caen->vme_base | CAENV767_OPHAND) & 
         0x0002)) {
    ss_sleep(10);
  }

  // Wait a little longer
  ss_sleep(10);

  // Write the opcode value
  vme_write_d16(caen->vme_handle, caen->vme_base | CAENV767_OPCODE, code); 
}

/* ********************************************************************* */
short caenV767_opcode_read(struct caenV767 *caen) 
{
  // Wait for the microcontroller to become ready
  while(!(vme_read_d16(caen->vme_handle, caen->vme_base | CAENV767_OPHAND) & 
         0x1)) {
    ss_sleep(10);
  }

  // Wait a little longer
  ss_sleep(10);

  // Read the opcode value
  return vme_read_d16(caen->vme_handle, caen->vme_base | CAENV767_OPCODE);
}

/* ********************************************************************* */
/*
 * caenV767_bor1
 *
 * Initialize one CAENV767 module.
 */
INT caenV767_bor1(struct caenV767 *caen)
{
  // Get the VME base address of the module
  caen->vme_base = odb_get_dword("/Equipment/Crate %d/Settings/%s/vme address", 
		    crate_number, caen->odb_name);

  // Open a VME handle for memory-mapped access
  struct vme_mapping_ctrl mapping = { 
	  VMEMAP_DWIDTH_32, 
	  VMEMAP_ASPACE_A32, 
	  VMEMAP_SUPUSERAM_SUPER, 
	  VMEMAP_PRGDATAAM_DATA 
  };

  struct vme_handle *handle = 
	  vme_open(caen->vme_base, mapping, 0x200, CAENV767_MAX_DMA_SIZE);
  caen->vme_handle = handle;
 
  // Determine whether we're supposed to do active readout
  caen->do_active_readout =
    odb_get_bool("/Equipment/Crate %d/Settings/%s/Active Readout mode",
		    crate_number, caen->odb_name);

  // Determine whether we should record trailing edges, or only leading edges
  caen->record_trailing_edges  =
    odb_get_bool("/Equipment/Crate %d/Settings/%s/Record trailing edges",
		    crate_number, caen->odb_name);

  // Request a bus error when DMA transfer reaches end of data
  vme_write_d16(caen->vme_handle, caen->vme_base | CAENV767_CTR1, 0x0020);

  // Read the half-full mark and the active readout buffer size
  caen->half_full_level = 
    odb_get_int("/Equipment/Crate %d/Settings/%s/Half-full level",
		    crate_number, caen->odb_name);
  caen->active_readout_buffer_size = 
    odb_get_int("/Equipment/Crate %d/Settings/%s/Active readout buffer size",
		    crate_number, caen->odb_name);
 
  // Allocate the active readout buffer
  caen->active_readout_buffer = new unsigned char[caen->active_readout_buffer_size];

  // Set various TDC options
  caenV767_opcode_write(caen, 0x1200); // set start gating mode
  caenV767_opcode_write(caen, 0x7100); // set DRDY = almost full
  caenV767_opcode_write(caen, 0x7400); // prepare to set almost full level...
  caenV767_opcode_write(caen, caen->half_full_level); // ...set almost full level 
  caenV767_opcode_write(caen, 0x4000); // enable readout of start time
  caenV767_opcode_write(caen, 0x4300); // enable subtraction of start time 
                                       // from hit times

  if(caen->record_trailing_edges) {
    caenV767_opcode_write(caen, 0x6600); // enable both edges on all channels 
                                         // and start
  } else {
    caenV767_opcode_write(caen, 0x6000); // enable only leading edge on all
                                         // channels and start
  }

  // Enable the CAEN channels according to the ODB settings
  diag_print(1, "Enabling all channels on %s.\n", caen->odb_name);
  caenV767_opcode_write(caen, 0x2300); // enable all channels first 
  BOOL channel_enable_array[CAENV767_NUM_CHANNELS], channelstatus;
  odb_get_boolarray(&channel_enable_array[0], CAENV767_NUM_CHANNELS,
		    "/Equipment/Crate %d/Settings/%s/channels/channel enabled",
		    crate_number, caen->odb_name);
  // disable each masked channel 
  for (int channel=0; channel<=(CAENV767_NUM_CHANNELS-1); channel++) {
    channelstatus = channel_enable_array[channel];
    if (channelstatus == 0) {
      diag_print(1, "Disabling %s channel %d.\n", caen->odb_name, channel);
      caenV767_opcode_write(caen, 0x2100 | channel);
    }
  }

  // Reset the TDC
  vme_write_d16(caen->vme_handle, caen->vme_base | CAENV767_CLEAR, 1);

  // Clear for the next block
  caen->active_readout_size = 0;

  return SUCCESS;
}

/* ********************************************************************* */
/*
 * caenV767_bor
 *
 * Called at the beginning of the run to discover CAENV767 modules
 * and initialize them.
 */
INT caenV767_bor()
{
  // Use the ODB to find any CAENV767 modules
  for(int j = 0; j < MAX_CAENV767; j++) {

    bool enabled = false;

    if (odb_find_key("/Equipment/Crate %d/Settings/CAEN %d", crate_number, j)) {
      diag_print(1, "ODB says CAEN %d is present, ", j);
      enabled = 
	odb_get_bool("/Equipment/Crate %d/Settings/CAEN %d/enabled status", 
            crate_number, j);
      if (enabled) {
        diag_print(1, "and is enabled.  Initializing...\n");
      } else {
        diag_print(1, "but is disabled.\n");
      }
    }

    caenV767[j].enabled = enabled;

    // Set up the name of the MIDAS bank associated with the module
    sprintf(caenV767[j].bank_name, "CAE%d", j);
    sprintf(caenV767[j].odb_name, "CAEN %d", j);

    if(enabled) {
      caenV767_bor1(&caenV767[j]);
    }
  }

  return SUCCESS;
}

/* ********************************************************************* */
INT caenV767_eor1(struct caenV767 *caen)
{
  vme_close(caen->vme_handle);
  delete[] caen->active_readout_buffer;
  return SUCCESS;
}

/* ********************************************************************* */
/*
 * caenV767_eor
 *
 * Called at the end of the run to release any resources that may have
 * been allocated.
 */
INT caenV767_eor()
{

  for(int i = 0; i < MAX_CAENV767; i++) {
    if(caenV767[i].enabled) {
      int status = caenV767_eor1(&caenV767[i]);
      if(status != SUCCESS) {
         return status;
      }
    }
  }

  return SUCCESS;
}

/* ********************************************************************* */
INT caenV767_fifo_read(struct caenV767 *caen, unsigned char *buffer, 
		      int max_size, bool in_active_readout)
{
  if(!in_active_readout) {
    int status = vme_dma_read(caen->vme_handle,
			      caen->vme_base | CAENV767_OUTBUF,
			      buffer,
			      max_size);
    return status;
  } else {
    int size = 0;

    for(int i = 0; i < max_size / sizeof(DWORD); i++) {
      DWORD word = 
        vme_read_d32(caen->vme_handle, caen->vme_base | CAENV767_OUTBUF);

      if((word & CAENV767_NOT_VALID) == CAENV767_NOT_VALID) {
        break;
      }

      *((DWORD *) buffer) = word;
      buffer += sizeof(DWORD);
      size += sizeof(DWORD);
    }

    return size;
  }
}

/* ********************************************************************* */
/*
 * caenV767_poll1
 *
 * Performs active readout for a single CAENV767 FIFO.
 */
INT caenV767_poll1(struct caenV767 *caen)
{
  // If active readout is not enabled, skip it.
  if(!caen->do_active_readout) {
    return SUCCESS;
  }

  // Check whether there's anything at all in the buffer.
 
  // How many words do we expect?  
  struct timeval now;
  gettimeofday(&now, NULL);
  double block_seconds = (now.tv_sec - block_start_time.tv_sec) + 
                           1e-6*(now.tv_usec - block_start_time.tv_usec);
  int expected_words = 
    (int) (caen->bytes_per_second / block_seconds  / sizeof(DWORD));

  // First check how much space we have available in the active
  // readout buffer.
  int size_left = caen->active_readout_buffer_size - caen->active_readout_size;
  int size = MIN(ACTIVE_READOUT_MAX_PER_POLL, size_left);
  size = MIN(size, expected_words);

  // Now try to read up to that amount, without using block transfers,
  // which can potentially drop the last data word.
  int status = 
     caenV767_fifo_read(caen, 
  		        caen->active_readout_buffer + caen->active_readout_size,
		        size,
                        TRUE);
			    
  // Check the status
  if(status >= 0) {
    caen->active_readout_size += status;
  } else {
    diag_print(0, "Status from caenV767_fifo_read is %d for %s\n", status,
      caen->odb_name);
    return FE_ERR_HW;
  }

  if(caen->active_readout_size == caen->active_readout_buffer_size) {
    return FE_NEED_STOP; 
  } else {
    return SUCCESS;
  }
}

/* ********************************************************************* */
/*
 * caenV767_poll_live
 *
 * Called periodically while a block is active; performs active readout.
 *
 * Returns:
 * - ordinarily 0,
 * - a request for a "soft stop" end-of-block, or
 * - an error code
 */
INT caenV767_poll_live()
{
  // If this is our first active readout pass, just remember the time.
  if(block_start_time.tv_sec == 0 && block_start_time.tv_usec == 0) {
    return SUCCESS;
  }

  for(int i = 0; i < MAX_CAENV767; i++) {
    if(caenV767[i].enabled) {
      int status = caenV767_poll1(&caenV767[i]);
      if(status != SUCCESS) {
        return status; 
      }
    }
  }

  return SUCCESS;
}

/* ********************************************************************* */
/*
 * caenV767_read1
 *
 * Constructs the MIDAS bank for a single CAENV767.
 */
INT caenV767_read1(struct caenV767 *caen, char *pevent)
{
  // Create the MIDAS bank
  DWORD *pdata;
  bk_create(pevent, caen->bank_name, TID_DWORD, &pdata);

  // Copy data that was read during the active readout
  int active_size = caen->active_readout_size;
  memcpy(pdata, caen->active_readout_buffer, active_size);

  // Read any data remaining in the module
  int status = 
    caenV767_fifo_read(caen,  ((unsigned char *) pdata) + active_size,
	 	      CAENV767_MAX_SIZE, FALSE);

  // Check the status
  int final_size = 0;
  if(status >= 0) {
    final_size = status;
  } else {
    // handle the error
  }
 
  // Check the TDC status for error codes (typically "DLL unlocked" problems)
  // and add an error flag word if a problem is found.
  short str2 = vme_read_d16(caen->vme_handle, caen->vme_base | CAENV767_STR2);
  if(str2 & CAENV767_GLOBAL_TDC_ERROR) {
    DWORD flag_word = CAENV767_NOT_VALID;
    for(int tdc = 0; tdc < 4; tdc++) {
      short ind_tdc_error_mask = (1 << (12+tdc));
      if(str2 & (1 << (12+tdc))) {
        flag_word |= (1 << tdc); 
      }
    }

    pdata[(active_size + final_size)/sizeof(DWORD)] = flag_word;
    final_size += sizeof(DWORD);

    diag_print(0, "TDC error in %s: 0x%08x\n", caen->odb_name, flag_word);
  }

  // Close the bank
  bk_close(pevent, pdata + (active_size + final_size)/sizeof(DWORD));

  // Clear for the next block
  caen->active_readout_size = 0;

  // Reset the TDC
  vme_write_d16(caen->vme_handle, caen->vme_base | CAENV767_CLEAR, 1);

  // Update the expected rate.
  if(block_start_time.tv_sec != 0) {  
    struct timeval now;
    gettimeofday(&now, NULL);
    double block_seconds = (now.tv_sec - block_start_time.tv_sec) + 
                             1e-6*(now.tv_usec - block_start_time.tv_usec);
    caen->bytes_per_second = (active_size + final_size)/block_seconds;
  }
  block_start_time.tv_sec = block_start_time.tv_usec = 0;

  return SUCCESS;
}

/* ********************************************************************* */
/*
 * caenV767_read
 *
 * Called at the end of a block to assemble data from that block into a 
 * MIDAS event.
 */
INT caenV767_read(char *pevent)
{
  for(int i = 0; i < MAX_CAENV767; i++) {
    if(caenV767[i].enabled) {
      int status = caenV767_read1(&caenV767[i], pevent);
       if(status != SUCCESS) {
         return status;
       }
    }
  }

  return SUCCESS;
}

/* ********************************************************************* */
