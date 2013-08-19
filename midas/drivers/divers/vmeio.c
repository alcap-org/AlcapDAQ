/*-----------------------------------------------------------------------------
 *  Copyright (c) 1996      TRIUMF Data Acquistion Group
 *  Please leave this header in any reproduction of that distribution
 * 
 *  TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 *  Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * -----------------------------------------------------------------------------
 *  
 *  Description	: Support function library for the vmeIO board developed at TRIUMF
 *                by Mr. T. ambardar & P.A. Amaudruz
 *  Requires 	  : 
 *  Application : VME
 *  Author      :  Pierre-Andre Amaudruz Data Acquisition Group
 *  Revision 1.0:  1998/Mar/20 Pierre	   Inline function
    $Log: vmeio.c,v $
    Revision 1.1.1.1  2005/06/20 23:37:09  mucap
    Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
    (Next, I'll commit our local customizations.)

    Revision 1.2  2004/01/08 07:51:21  midas
    Changed indentation

    Revision 1.1  1999/12/20 10:18:23  midas
    Reorganized driver directory structure

    Revision 1.4  1999/12/08 00:52:24  pierre
    - fix () to (void *) declaration.

    Revision 1.3  1999/09/29 21:04:19  pierre
    - include vmeio.h

    Revision 1.2  1999/05/06 18:36:20  pierre
    - PPCxxx support
 *
 *---------------------------------------------------------------------------*/

#include "stdio.h"
#include "string.h"

#include "vmeio.h"

void myStub(void)
{
#ifdef OS_VXWORKS
   logMsg("myStub\n", 0, 0, 0, 0, 0, 0);
#endif
}

/************ INLINE function for General command ***********/
INLINE void vmeio_intsync_set(DWORD base_adr, DWORD pattern)
/**************************************************************\
 Purpose: Set VMEIO interrupt source to be sync mode.
          Each bit refers to an input (0:7)
          If not set the interrupt source is Async
 Input:
  DWORD * base_addr     : base address of the VMEIO
  DWORD pattern         : data to be applied to board
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr) + (SET_INT_SYNC);
   *spec_adr = (pattern & 0xf);
}

/************ INLINE function for General command **********/
INLINE void vmeio_pulse_set(const DWORD base_adr, DWORD pattern)
/**************************************************************\
 Purpose: Set VMEIO output channel pattern to be pulse mode
          Each bit refers to an output (0:23)
          If not set the output is latch mode
 Input:
  DWORD * base_addr     : base address of the VMEIO
  DWORD pattern         : data to be applied to board
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr) + (SET_PULSE);
   *spec_adr = (pattern & 0xffffff);
}

/************ INLINE function for General command **********/
INLINE void vmeio_sync_read(const DWORD base_adr, DWORD * data)
/**************************************************************\
 Purpose: read VMEIO sync input data if gate received
 Input:
  DWORD * base_addr       : base address of the VMEIO
  DWORD * data            : data to be read
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;
   DWORD status;

   /* status : Check first if trigger has been received */
   spec_adr = (DWORD *) (A32D24 + base_adr) + (READ_STATUS);
   status = *spec_adr;
   status &= 0xf;
   if (status != 0) {
      spec_adr = (DWORD *) (A32D24 + base_adr) + (READ_SYNC);
      *data = (*spec_adr & 0xffffff);
   } else
      *data = 0x80000000;
}

/************ INLINE function for General command **********/
INLINE void vmeio_async_read(const DWORD base_adr, DWORD * data)
/**************************************************************\
 Purpose: read VMEIO async input data
 Input:
  DWORD * base_addr       : base address of the VMEIO
  DWORD * data            : data to be read
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr) + (READ_ASYNC);
   *data = (*spec_adr & 0xffffff);
}

/************ INLINE function for General command **********/
INLINE void vmeio_pulse_write(const DWORD base_adr, DWORD data)
/**************************************************************\
 Purpose: write to VMEIO pulse output.
          only the pulse enable channels will be affected
          Each bit correspond to a output (0:23)
 Input:
  DWORD * base_addr       : base address of the VMEIO
  DWORD data              : data to write
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   DWORD *spec_adr;

   /* status : Check first if trigger has been received */
   spec_adr = (DWORD *) (A32D24 | base_adr) + (WRITE_PULSE);
   *spec_adr = (data & 0xffffff);
}

/************ INLINE function for General command **********/
INLINE void vmeio_latch_write(const DWORD base_adr, DWORD data)
/**************************************************************\
 Purpose: write to VMEIO latch output.
          only the latch enable channels will be affected
          Each bit correspond to a output (0:23)
 Input:
  DWORD * base_addr       : base address of the VMEIO
  DWORD data              : data to write
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   /* status : Check first if trigger has been received */
   spec_adr = (DWORD *) (A32D24 + base_adr) + (WRITE_LATCH);
   *spec_adr = (data & 0xffffff);
}

/************ INLINE function for General command **********/
INLINE void vmeio_clear(const DWORD base_adr)
/**************************************************************\
 Purpose: clear the status reg (strobe)
 Input:
  DWORD * base_addr       : base address of the VMEIO
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   /* status : Check first if trigger has been received */
   spec_adr = (DWORD *) (A32D24 + base_adr) + (CLEAR_VMEIO);
   *spec_adr = 0x0;
}

/************ INLINE function for General command **********/
INLINE void vmeio_status_read(const DWORD base_adr, DWORD * data)
/**************************************************************\
 Purpose: read the status register.
          contains the strobe bit (0x1), the interrupt level (0x70)
 Input:
  DWORD * base_addr       : base address of the VMEIO
  DWORD * data            : status
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr) + (READ_STATUS);
   *data = (*spec_adr & 0xff);
}

/************ INLINE function for Interrupt handling **********/
INLINE void vmeio_int_clear(const DWORD base_adr)
{
   volatile DWORD *spec_adr;

   /* status : Check first if trigger has been received */
   spec_adr = (DWORD *) (A32D24 + base_adr) + (CLEAR_VMEIO);
   *spec_adr = 0x0;
}

INLINE void vmeio_int_enable(const DWORD base_adr, int intnum)
/**************************************************************\
 Purpose: Enable the interrupt for the bitwise input intnum (0xff).
          The interrupt vector is then the VECTOR_BASE+intnum
 Input:
  DWORD * base_addr       : base address of the VMEIO
  DWORD * intnum          : interrupt number (input 0:7)
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr) + (ENABLE_INT);
   *spec_adr = (intnum & 0xff);
#ifdef OS_VXWORKS
   sysIntEnable(7);             /* interrupt level 7 */
#endif
}

INLINE void vmeio_int_disable(const DWORD base_adr, int intnum)
/**************************************************************\
 Purpose: Disable the interrupt for the bitwise input intnum (0xff).
          The interrupt vector is then the VECTOR_BASE+intnum
 Input:
  DWORD * base_addr       : base address of the VMEIO
  DWORD * intnum          : interrupt number (input 0:7)
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

#ifdef OS_VXWORKS
   sysIntDisable(7);            /* interrupt level 7 */
#endif
   spec_adr = (DWORD *) (A32D24 + base_adr) + (ENABLE_INT);
   *spec_adr = ~(intnum & 0xff);
}

INLINE void vmeio_int_attach(const DWORD base_adr, DWORD base_vect, int intnum,
                             void (*isr) (void))
/**************************************************************\
 Purpose: Book an ISR for a bitwise set of interrupt input (0xff).
          The interrupt vector is then the VECTOR_BASE+intnum
 Input:
  DWORD * base_addr      : base address of the VMEIO
  DWORD base_vect        : base vector of the module
  DWORD intnum           : interrupt number (0:7)
  DWORD isr_routine      : interrupt routine pointer
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   vmeio_int_clear(base_adr);   /* clear any pending front panel interrupts */
   vmeio_int_disable(base_adr, intnum); /* bitwise input */
#ifdef OS_VXWORKS
   if (intnum < 8)
      intConnect(INUM_TO_IVEC(base_vect + intnum), (VOIDFUNCPTR) isr, intnum);
#else
   printf("vector : 0x%x\n", base_vect + intnum);
#endif
   vmeio_int_enable(base_adr, intnum);  /* bitwise input */
}

INLINE void vmeio_int_detach(const DWORD base_adr, DWORD base_vect, int intnum)
/**************************************************************\
 Purpose: Unbook an ISR for a bitwise set of interrupt input (0xff).
          The interrupt vector is then the VECTOR_BASE+intnum
 Input:
  DWORD * base_addr       : base address of the VMEIO
  DWORD base_vect        : base vector of the module
  DWORD intnum           : interrupt number (0:7)
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   vmeio_int_clear(base_adr);   /* clear any pending front panel interrupts */
   vmeio_int_disable(base_adr, intnum); /* mask off external interrupt x */
#ifdef OS_VXWORKS
   if (intnum < 8)
      intConnect(INUM_TO_IVEC(base_vect + intnum), (VOIDFUNCPTR) myStub, intnum);
#else
   printf("vector : 0x%x\n", base_vect + intnum);
#endif
}

/*-Tests---------------------------------------------------------*/
void vmeio(void)
{
   printf("\n---> vmeIO Triumf board 24bit in/out latch/pulse + 8 interrupts <---\n");
   printf("Inline: vmeio_intsync_set (base, pattern)  : set interrupt source\n");
   printf("Inline: vmeio_pulse_set   (base, pattern)  : set output pulse\n");
   printf("Inline: vmeio_sync_read   (base, *data)    : read sync input\n");
   printf("Inline: vmeio_async_read  (base, *data)    : read Async input\n");
   printf("Inline: vmeio_pulse_write (base, data)     : write pulse output\n");
   printf("Inline: vmeio_latch_write (base, data)     : write latch output\n");
   printf("Inline: vmeio_clear       (base)           : clear strobe\n");
   printf("Inline: vmeio_status_read (base, *status)  : read status register\n");
   printf("Inline: vmeio_int_clear   (base)           : clear interrupts\n");
   printf("Inline: vmeio_int_enable  (base, int#)     : enable interrupt (0:7)\n");
   printf("Inline: vmeio_int_disable (base, int#)     : disable interrupt (0:7)\n");
   printf("Inline: vmeio_int_enable  (base, int#)     : enable interrupt (0:7)\n");
   printf("Inline: vmeio_int_attach  (base, vec, int#, isr) : interrupt (0:7)\n");
   printf("Inline: vmeio_int_detach  (base, vect, int#) : interrupt (0:7)\n");
   printf("Test  : vmeio_Status_read (0x780000)       : display status\n");
   printf("Test  : vmeio_Async_read  (0x780000)       : read Async\n");
   printf("Test  : vmeio_Sync_read   (0x780000)       : read sync\n");
   printf("Test  : vmeio_Pulse_write (0x780000,       : write pulse\n");
   printf("Test  : vmeio_Latch_write (0x780000,       : write latch\n");
   printf("Test  : vmeio_Clear       (0x780000)       : clear VMEIO\n");
   printf("Test  : Interrupt : VMEIO_BASE: 0x780000, VMEIO_VECT_BASE: 0x7F\n");
   printf("Test  : The interrupt test is based on the following configuration:\n");
   printf("        Output 0 -> strobe\n");
   printf("        Output 1 -> Input 0\n");
   printf("        Output 2 -> Input 1\n");
   printf("        Output 3 -> Input 2\n");
   printf("        vmeio_attach_async_interrupt(1)  will enable I1\n");
   printf("        vmeio_gen_async(1)               will generate int in I1\n");
   printf("  or    vmeio_attach_sync_interrupt(1)   will enable I1\n");
   printf("        vmeio_gen_sync(1)                will generate int in I1\n");
   printf("Test  : vmeio_attach_async_interrupt( x...): attach, enable Int#\n");
   printf("Test  : vmeio_attach_sync_interrupt( x...) : attach, enable Int#\n");
   printf("Test  : vmeio_detach_interrupt( x...)      : detach, enable Int#\n");
   printf("Test  : vmeio_gen_async( x)                : generate a Async int\n");
   printf("Test  : vmeio_gen_sync( x)                 : generate a sync int\n");
}

#define VMEIO_BASE  0x780000    /* VMEIO base address */
#define VMEIO_VECT_BASE 0x7f

void vmeio_Status_read(DWORD base)
{
   char str[5];
   DWORD status, pattern;
   vmeio_status_read(base, &status);
   printf("vmeio status :0x%x\n", status);
   pattern = status & 0xff;
   printf("Status vmeio ==> intsrc:%s / Strobe:%1.1x\n",
          (((pattern & 0xf0) >> 4) == 0 ? strcpy(str, "ASync") : strcpy(str, "Sync"))
          , (pattern & 0x1));
}

void vmeio_Async_read(DWORD base)
{
   DWORD status;
   vmeio_async_read(base, &status);
   printf("vmeio Async data :0x%x\n", status);
}

void vmeio_Sync_read(DWORD base)
{
   DWORD status;
   vmeio_sync_read(base, &status);
   printf("vmeio Sync data :0x%x\n", status);
}

void vmeio_Pulse_write(DWORD base, DWORD pat)
{
   vmeio_pulse_write(base, pat);
}

void vmeio_Latch_write(DWORD base, DWORD pat)
{
   vmeio_latch_write(base, pat);
}

void vmeio_Clear(DWORD base)
{
   vmeio_clear(base);
}

/* the interrupt test is based on the following configuration:
   O0 -> strobe
   O1 -> I0
   O2 -> I1
   O3 -> I2
   
   vmeio_attach_async_interrupt(1)
             will enable I1
   vmeio_gen_async(1)
             will generate int in I1

   vmeio_attach_sync_interrupt(1)
             will enable I1
   vmeio_gen_async(1)
             will generate int in I1
*/


void vmeiotint(void)
{
   static int vmeio_private_counter = 0;
#ifdef OS_VXWORKS
   logMsg("%vmeiotint external interrupt #%i received and served\n",
          vmeio_private_counter++, 0, 0, 0, 0, 0);
#endif
}

void vmeio_attach_async_interrupt(DWORD vmeioint)
{
   vmeio_int_disable(VMEIO_BASE, vmeioint);
   vmeio_intsync_set(VMEIO_BASE, 0x0);
   vmeio_pulse_set(VMEIO_BASE, 0x0);
   vmeio_int_attach(VMEIO_BASE, VMEIO_VECT_BASE, vmeioint, vmeiotint);
   vmeio_int_enable(VMEIO_BASE, vmeioint);
   printf("Please generate an interrupt on line %d\n", vmeioint);
}

void vmeio_gen_async(DWORD intnum)
{
   vmeio_Latch_write(VMEIO_BASE, (intnum << 1));
   vmeio_Latch_write(VMEIO_BASE, 0);
   vmeio_intsync_set(VMEIO_BASE, 0x0);  /* reset interrupt */
}

void vmeio_gen_sync(DWORD intnum)
{
   vmeio_Pulse_write(VMEIO_BASE, (0x1 | (intnum << 1)));
   vmeio_clear(VMEIO_BASE);     /* reset interrupt */
}

void vmeio_attach_sync_interrupt(DWORD vmeioint)
{
   vmeio_intsync_set(VMEIO_BASE, (0x1 | (vmeioint << 1)));
   vmeio_pulse_set(VMEIO_BASE, (0x1 | (vmeioint << 1)));        /* strobe done by intput 0 */
   vmeio_int_disable(VMEIO_BASE, vmeioint);
   vmeio_int_attach(VMEIO_BASE, VMEIO_VECT_BASE, vmeioint, vmeiotint);
   vmeio_int_enable(VMEIO_BASE, vmeioint);
   printf("Please generate an interrupt on line %d\n", vmeioint);
}

void vmeio_detach_interrupt(DWORD vmeioint)
{
   vmeio_int_disable(VMEIO_BASE, vmeioint);
   vmeio_int_detach(VMEIO_BASE, VMEIO_VECT_BASE, vmeioint);
   printf("Interrupt on line %d detached \n", vmeioint);
}
