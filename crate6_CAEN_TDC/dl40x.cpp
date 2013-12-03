/*-----------------------------------------------------------------------------
  STRUCK DL40x modules interface 
  F. Gray (U.C. Berkeley)
  based on a previous version by T. Gorringe/P. Zolnierczuk (U. Kentucky)
  ----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "midas.h"

#include "crate.h"
#include "vme.h"

#include "dl40x.h"
#include "diag.h"
#include "odb_wrapper.h"

INT dl40x_bor ();
INT dl40x_eor ();
INT dl40x_read (char *pevent);

struct readout_module dl40x_module = {
  NULL,				// init
  NULL,				// exit
  NULL,                         // pre_bor
  dl40x_bor,			// bor
  dl40x_eor,			// eor
  NULL,				// poll_live
  NULL,				// poll_dead
  NULL,				// start_cycle
  NULL,				// stop_cycle
  dl40x_read,			// read
};

static struct dl401
{
  struct vme_handle *a16_handle;
  struct vme_handle *a32_handle;
  unsigned long a16_base;
  unsigned long a32_base;
  BOOL enabled;
  char odb_name[20];

} DL401[DL401_MAXMODULES];

static struct dl403
{
  struct vme_handle *vme_handle;
  unsigned long a16_base;
  BOOL enabled;
} clk;

INT dl401_bor1 (struct dl401 *d);

INT
dl40x_bor ()
{
  /*
   * Initialize DL403 clock generator.
   */
  if (dl403_init (&clk) != SUCCESS) {
    return FE_ERR_HW;
  }

  /*
   * Initialize DL401 FADC modules.
   */
  for (int j = 0; j < DL401_MAXMODULES; j++)
    {
      sprintf (DL401[j].odb_name, "FADC %d", j);
      DL401[j].enabled = FALSE;

      if (odb_find_key ("/Equipment/Crate %d/Settings/FADC %d", crate_number, j))
	{
	  diag_print (1, "ODB says FADC #%d is present in crate 3, ", j);
	  DL401[j].enabled =
	    odb_get_bool ("/Equipment/Crate %d/Settings/FADC %d/enabled status",
			  crate_number, j);
	  if (DL401[j].enabled)
	    {
	      diag_print (1, "and is enabled.  Initializing...\n");
	      dl401_bor1 (&DL401[j]);
	    }
	  else
	    {
	      diag_print (1, "but is disabled.\n");
	    }
	}

    }

  return SUCCESS;
}

INT
dl40x_eor ()
{
  // release VME handles
  if(clk.enabled) {
    vme_close(clk.vme_handle);
  }

  for (int j = 0; j < DL401_MAXMODULES; j++) {
    if(DL401[j].enabled) {
      vme_close(DL401[j].a16_handle);
      vme_close(DL401[j].a32_handle);
    }
  }

  return SUCCESS;
}

INT
dl40x_read (char *pevent)
{
  if(!clk.enabled) {
    return SUCCESS;
  }

  // loop over FADC banks if the clock has stopped.  Otherwise, they
  // aren't part of the event, because they didn't get a trigger.
  if (!dl403_running (&clk))
    {
      for (int j = 0; j < DL401_MAXMODULES; j++)
	{
	  if (DL401[j].enabled)
	    {
#if 0
	      // wait for the ADCs to finish converting
	      while (!dl401_finished (&DL401[j]))
		{
		  cm_yield(0); 
		}
#endif

	      // perform readout
	      long *pdata;
	      char bk_name[8];
	      int size = DL401_CNTMAX;

	      sprintf (bk_name, "ADC%d", j);
	      bk_create (pevent, bk_name, TID_DWORD, &pdata);
	      size = dl401_read (&DL401[j], (u_int *) pdata);
	      pdata += size;
	      bk_close (pevent, pdata);

              // Reset for next event 
   	      dl401_reset (&DL401[j]);
	      dl401_clear (&DL401[j]);
	    }
	}
    }

  // Restart FADCs for next event
  dl403_start (&clk);

  return SUCCESS;
}

/* =================================================
   STRUCK DL401 VME FLASH ADC
   ================================================= */

INT
dl401_bor1 (struct dl401 *d)
{
  // Find base addresses.  This module has separate A16 (for configuration) 
  // and A32 (for data memory) address regions.  The A16 address is set by
  // jumpers while the A32 is programmed by a register accessed through the
  // A16 space.
  d->a16_base = odb_get_word ("/Equipment/Crate %d/Settings/%s/a16 base address",
	   	              crate_number, d->odb_name);
  d->a32_base = odb_get_dword ("/Equipment/Crate %d/Settings/%s/a32 base address",
		              crate_number, d->odb_name);

  // Get VME handles to access module
  struct vme_mapping_ctrl a16_mapping = {
          VMEMAP_DWIDTH_32,
          VMEMAP_ASPACE_A16,
          VMEMAP_SUPUSERAM_SUPER,
          VMEMAP_PRGDATAAM_DATA
  };
  d->a16_handle = vme_open(d->a16_base, a16_mapping, 0x100, 0);

  struct vme_mapping_ctrl a32_mapping = {
          VMEMAP_DWIDTH_32,
          VMEMAP_ASPACE_A32,
          VMEMAP_SUPUSERAM_USER,
          VMEMAP_PRGDATAAM_DATA
  };
  d->a32_handle = vme_open(d->a32_base, a32_mapping, 4096, 0);

  // Set up the A32 base address
  vme_write_d16 (d->a16_handle, d->a16_base | DL401_BASE, d->a32_base >> 16);

  // Set up the address modifiers
  vme_write_d16 (d->a16_handle, d->a16_base | DL401_MASK, 
                 DL401_LWIOENA | DL401_AM_EXT_MODE);

  // Set the default mode bits
  vme_write_d16 (d->a16_handle, d->a16_base | DL401_MODE, DL401_DEFMODE);

  // find and set pedestals
  WORD ped[DL401_NCHAN];
  for (int k = 0; k < DL401_NCHAN; k++)
    {
      ped[k] = odb_get_word ("/Equipment/Crate %d/Settings/%s/Pedestal",
	  	             crate_number, d->odb_name);
    }
  dl401_setPedestals (d, ped, DL401_NCHAN);

  // standard reset and clear
  dl401_reset (d);
  dl401_clear (d);

  diag_print(2, "dl401_bor1  a32_base=0x%08lx a16_base=0x%08lx\n",
	  d->a32_base, d->a16_base);

  return SUCCESS;
}

INLINE int
dl401_setPedestals (struct dl401 *d, u_short p[], int np)
{
  if (np != DL401_NCHAN)
    return (-1);

  // compute word to write to pedestal DAC 
  DWORD parPed = 0;
  for (int i = 0; i < np; i++) {
      parPed |= (u_long) (p[i] & DL401_PEDCHANMASK) << (i * DL401_PEDCHANBITS);
  }
  parPed &= DL401_PEDMASK;
  diag_print (2, "dl401_setPedestal: [%d] 0%08lo (", d, parPed);

  // enable writing to DAC
  vme_write_d16 (d->a16_handle, d->a16_base | DL401_MODE,
                 DL401_DEFMODE & DL401_DACWENA);

  // shift bits into DAC one at a time; it's a serial interface
  for (int i = 0; i < DL401_PEDBITS; i++)
    {
      u_short bit =
	((parPed << i) & DL401_PEDBITMASK) >> (DL401_PEDBITS - 1);
      vme_write_d16 (d->a16_handle, d->a16_base | DL401_WDAC, bit & 0x0001);
      diag_print (2, "%1x", bit);
    }

  // disable writing to DAC
  vme_write_d16 (d->a16_handle, d->a16_base | DL401_MODE,
		 DL401_DEFMODE | DL401_DACWDIS);
  diag_print (2, ")\n");

  return SUCCESS;
}

INLINE u_short
dl401_getCounts (struct dl401 * d)
{
  WORD rmad = vme_read_d16 (d->a16_handle, d->a16_base | DL401_RMAD);
  return rmad & DL401_CNTMASK;
}

INLINE u_short
dl401_getMode (struct dl401 * d)
{
  return vme_read_d16 (d->a16_handle, d->a16_base | DL401_MODE);
}

INLINE void
dl401_reset (struct dl401 *d)
{
  vme_write_d16 (d->a16_handle, d->a16_base | DL401_MRST, 0xffff);
}

INLINE void
dl401_clear (struct dl401 *d)
{
  vme_write_d16 (d->a16_handle, d->a16_base | DL401_CLIF, 0xffff);
}

INLINE void
dl401_convert (struct dl401 *d)
{
  vme_read_d16 (d->a16_handle, d->a16_base | DL401_ICNV);
}

INLINE int
dl401_finished (struct dl401 *d)
{
  WORD mode = vme_read_d16 (d->a16_handle, d->a16_base | DL401_MODE);
  return (mode & DL401_INTREQ);
}


int
dl401_read (struct dl401 *d, u_int * data)
{
  register u_int i = 0;
  register u_int *addr;
  int counts;

  counts = dl401_getCounts (d);
  diag_print (2, "dl401_read: [%d] base=%p counts=%d\n", d, addr, counts);

  while (i < DL401_CNTMAX - counts)
    {
      data[i++] = vme_read_d32 (d->a32_handle, d->a32_base + (i + counts)*4);
    }

  i = 0;
  while (i < counts)
    {
      data[DL401_CNTMAX - counts + i] =
	vme_read_d32 (d->a32_handle, d->a32_base + i*4);
      i++;
    }
  diag_print (2, "--> done\n");
  return (DL401_CNTMAX);
}

#if 0
int
dl401_dmaread (DL401 * d, u_int * data)
{
  long transfersize1, transfersize2;
  int counts;

  counts = dl401_getCounts (d);
  printf ("dl401 counts = %d\n", counts);

  transfersize1 = pread (fd_a32d32_noblt,
			 data,
			 (DL401_CNTMAX - counts) * sizeof (*data),
			 d->dataAddr - a32_space_start + counts * 4);

  transfersize2 = pread (fd_a32d32_noblt,
			 data + DL401_CNTMAX - counts,
			 counts * sizeof (*data),
			 d->dataAddr - a32_space_start);

  return ((transfersize1 + transfersize2) / sizeof (*data));
}
#endif

/* =================================================
   STRUCK DL403 VME CLOCK MODULE
   ================================================= */
INT dl403_init (struct dl403 *d)
{
  // Do we have an FADC clock?
  if (odb_find_key("/Equipment/Crate %d/Settings/FADC CLOCK", crate_number)) {
    d->enabled = TRUE;
  } else {
    d->enabled = FALSE;
    return SUCCESS;
  }

  // find base address
  d->a16_base =
    odb_get_word ("/Equipment/Crate %d/Settings/FADC CLOCK/a16 base address",
                  crate_number);

  // Open a VME handle
  struct vme_mapping_ctrl a16_mapping = {
          VMEMAP_DWIDTH_32,
          VMEMAP_ASPACE_A16,
          VMEMAP_SUPUSERAM_SUPER,
          VMEMAP_PRGDATAAM_DATA
  };
  d->vme_handle = vme_open(d->a16_base, a16_mapping, 0x100, 0);

  // Set DL403 clock frequency
  INT fadc_frequency =
    odb_get_int ("/Equipment/Crate %d/Settings/FADC CLOCK/frequency", crate_number);
  switch (fadc_frequency)
    {
    case 100:
      dl403_setClockRate (d, DL403_100MHZ, 0x0, 0x0);
      break;
    case 50:
      dl403_setClockRate (d, DL403_50MHZ, 0x0, 0x0);
      break;
    case 25:
      dl403_setClockRate (d, DL403_SELECTED, 0x0, 0x0);
      break;

    default:
      cm_msg (MERROR, frontend_name,
	      "FADC clock frequency must be 25, 50, or 100 MHz (currently %d)\n",
	      fadc_frequency);
      dl403_setClockRate (d, DL403_50MHZ, 0x0, 0x0);
    }

  dl403_setClockCycle (d, 0x800, 0x800, 0x800);
  dl403_steadyMode (d);
  dl403_start (d);

  diag_print (2, "dl403_init 0x%08lx\n", d->a16_base);
  return SUCCESS;
}

void
dl403_setClockRate (struct dl403 *d, u_short clk1, u_short clk2, u_short clk3)
{
  diag_print (2, "dl403_setClockRate: [%d] rate=0x%02hx\n", d,
	      (clk1 | (clk2 << 2) | (clk3 << 4)));

  vme_write_d16 (d->vme_handle, d->a16_base | DL403_RATE,
		 clk1 | (clk2 << 2) | (clk3 << 4));
}

void
dl403_setClockCycle (struct dl403 *d, u_short c1, u_short c2, u_short c3)
{
  vme_write_d16 (d->vme_handle, d->a16_base | DL403_CYCLE1, c1);
  vme_write_d16 (d->vme_handle, d->a16_base | DL403_CYCLE2, c2);
  vme_write_d16 (d->vme_handle, d->a16_base | DL403_CYCLE3, c3);

  diag_print (2,
	      "dl403_setClockCycles: [%d] cyc1=0x%04hx cyc2=0x%04hx cyc3=0x%04hx\n",
	      d, c1, c2, c3);

}

INLINE short
dl403_getMode (struct dl403 *d)
{
  return vme_read_d16 (d->vme_handle, d->a16_base | DL403_MODE);
}

INLINE void
dl403_asyncMode (struct dl403 *d)
{
  vme_write_d16 (d->vme_handle, d->a16_base | DL403_MODE, DL403_ASYNC);
}

INLINE void
dl403_contMode (struct dl403 *d)
{
  vme_write_d16 (d->vme_handle, d->a16_base | DL403_MODE, DL403_CONT);
}

INLINE void
dl403_steadyMode (struct dl403 *d)
{
  vme_write_d16 (d->vme_handle, d->a16_base | DL403_MODE, DL403_STEADY);
}

INLINE void
dl403_start (struct dl403 *d)
{
  vme_write_d16 (d->vme_handle, d->a16_base | DL403_START, DL403_GO);
}

INLINE void
dl403_stop (struct dl403 *d)
{
  vme_write_d16 (d->vme_handle, d->a16_base | DL403_STOP, DL403_GO);
}

INLINE void
dl403_reset (struct dl403 *d)
{
  vme_write_d16 (d->vme_handle, d->a16_base | DL403_RESET, DL403_GO);
}

INLINE short
dl403_runBit (struct dl403 *d)
{
  WORD mode = vme_read_d16 (d->vme_handle, d->a16_base | DL403_MODE);
  return (mode & DL403_RUN);
}

INLINE short
dl403_running (struct dl403 *d)
{
  WORD mode = vme_read_d16 (d->vme_handle, d->a16_base | DL403_MODE);
  return !(mode & DL403_RUN);
}
