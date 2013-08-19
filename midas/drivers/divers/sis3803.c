/*-----------------------------------------------------------------------------
 *  Copyright (c) 1996      TRIUMF Data Acquistion Group
 *  Please leave this header in any reproduction of that distribution
 * 
 *  TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 *  Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * -----------------------------------------------------------------------------
 *  
 *  Description	: Support function library for the SIS3803 200MHz 16ch. scalers
 *                P.A. Amaudruz
 *  Requires 	: sis3803.h
 *  Application : VME
 *  Author      : Pierre-Andre Amaudruz Data Acquisition Group
    $Log: sis3803.c,v $
    Revision 1.1.1.1  2005/06/20 23:37:09  mucap
    Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
    (Next, I'll commit our local customizations.)

    Revision 1.2  2004/01/08 07:51:21  midas
    Changed indentation

    Revision 1.1  2000/07/21 18:46:09  pierre
    - SIS3803 16ch 32-bit 200MHz scalers (www.struck.de)

 *
 *---------------------------------------------------------------------------*/
#include "sis3803.h"

void myStub_sis3803(void)
{
   static int sis3803_private_counter = 0;
#ifdef OS_VXWORKS
   sis3803_int_source_disable(0, 0);
   logMsg("sis3803 IRQ#%i served\n", sis3803_private_counter++, 0, 0, 0, 0, 0);
   sis3803_int_source_enable(0, 0);
#endif
}

/************ INLINE function for General command ***********/
INLINE DWORD sis3803_module_ID(const DWORD base_adr)
/**************************************************************\
 Purpose: return the Module ID number (I) version (V)
          IRQ level (L) IRQ vector# (BB)
 Input: 
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 DWORD                : 0xIIIIVLBB
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + MODULE_ID);
   return *spec_adr;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_module_reset(const DWORD base_adr)
/**************************************************************\
 Purpose: General Module reset
 Input: 
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + MODULE_RESET);
   *spec_adr = 0x0;
}

/************ INLINE function for General command ***********/
INLINE DWORD sis3803_IRQ_REG_read(const DWORD base_adr)
/**************************************************************\
 Purpose: return the lower 12 bits corresponding to
          VME IRQ enable, levels, vector(lower 8bits)
 Input: 
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 DWORD                : 0xE(1)L(3)V(8)
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + IRQ_REG);
   return (*spec_adr & 0xFFF);
}

/************ INLINE function for General command ***********/
INLINE DWORD sis3803_IRQ_REG_write(const DWORD base_adr, DWORD irq)
/**************************************************************\
 Purpose: write irq (ELV) to the register and read back 
 Input: 
  DWORD base_addr     : base address of the SIS3803
  DWORD irq           : reg 
 Output:
    none
 Function value:
 DWORD                : 0xE(1)L(3)V(8)
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + IRQ_REG);
   *spec_adr = irq;
   return (*spec_adr & 0xFFF);
}

/************ INLINE function for General command ***********/
INLINE DWORD sis3803_input_mode(const DWORD base_adr, DWORD mode)
/**************************************************************\
 Purpose: Set input configuration mode (only mode 1 available)
 Input: 
  DWORD base_addr     : base address of the SIS3803
  DWORD mode          : Mode 0-3
 Output:
    none
 Function value:
    DWORD             : current mode
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + CSR_WRITE);
   if (mode < 4)
      mode <<= 2;
   *spec_adr = mode;
   spec_adr = (DWORD *) (A32D24 + base_adr + CSR_READ);
   return ((*spec_adr & GET_MODE) >> 2);
}

/************ INLINE function for General command ***********/
INLINE DWORD sis3803_ref1(const DWORD base_adr, DWORD endis)
/**************************************************************\
 Purpose: Enable/Disable Reference on Channel 1
 Input: 
  DWORD base_addr     : base address of the SIS3803
  DWORD endis         : action either ENABLE_REF_CH1
                                      DISABLE_REF_CH1
 Output:
    none
 Function value:
               : status of the ref1 1=enable, 0=disable
\**************************************************************/
{
   volatile DWORD *spec_adr;

   if ((endis == ENABLE_REF_CH1) || (endis == DISABLE_REF_CH1)) {
      spec_adr = (DWORD *) (A32D24 + base_adr + endis);
      *spec_adr = 0x0;
   } else
      printf("sis3803_ref1: unknown command %d\n", endis);

   /* read back the status */
   spec_adr = (DWORD *) (A32D24 + base_adr + CSR_READ);
   return ((*spec_adr & IS_REF1) ? 1 : 0);
}

/************ INLINE function for General command ***********/
INLINE void sis3803_channel_enable(const DWORD base_adr, DWORD pat)
/**************************************************************\
 Purpose: Enable pat channel for acquistion.
          blind command! 0x0..0xffff
 Input: 
  DWORD base_addr     : base address of the SIS3803
  DWORD pat           : channel pattern
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + SELECT_COUNT_DIS_REG);
   *spec_adr = (~pat & 0xffff);
}


/************ INLINE function for General command ***********/
INLINE void sis3803_channel_disable(const DWORD base_adr, DWORD pat)
/**************************************************************\
 Purpose: Disable pat channel for acquistion.
          blind command! 0x0..0xffff
 Input: 
  DWORD base_addr     : base address of the SIS3803
  DWORD pat           : channel pattern
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + SELECT_COUNT_DIS_REG);
   *spec_adr = pat & 0xffff;
}

/************ INLINE function for General command ***********/
INLINE DWORD sis3803_CSR_read(const DWORD base_adr, const DWORD what)
/**************************************************************\
 Purpose: Read the CSR and return 1/0 based on what.
          except for what == CSR_FULL where it returns current CSR
 Input: 
  DWORD base_addr     : base address of the SIS3803
  DWORD what          : bitwise field (see sis3803.h)
 Output:
    none
 Function value:
 DWORD             : CSR 32bit current content
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + CSR_READ);
   if (what == CSR_FULL)
      return *spec_adr;
   else
      return ((*spec_adr & what) ? 1 : 0);
}

/************ INLINE function for General command ***********/
INLINE DWORD sis3803_CSR_write(const DWORD base_adr, const DWORD what)
/**************************************************************\
 Purpose: Write to the CSR and return CSR_FULL.
 Input: 
  DWORD base_addr     : base address of the SIS3803
  DWORD what          : bitwise field 
 Output:
    none
 Function value:
 DWORD             : CSR 32bit current content
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + CSR_WRITE);
   *spec_adr = what;
   return sis3803_CSR_read(base_adr, CSR_FULL);
}

/************ INLINE function for General command ***********/
INLINE void sis3803_all_clear(const DWORD base_adr)
/**************************************************************\
 Purpose: Clear all module + overflow bits
 Input:
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + ALL_CLEAR);
   *spec_adr = 0x0;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_test_enable(const DWORD base_adr)
/**************************************************************\
 Purpose: Enable test pulse and input test
 Input:
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + CSR_WRITE);
   *spec_adr = ENABLE_TEST_PULSE;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_test_disable(const DWORD base_adr)
/**************************************************************\
 Purpose: Disable test pulse and input test
 Input:
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + CSR_WRITE);
   *spec_adr = DISABLE_TEST_PULSE;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_all_enable(const DWORD base_adr)
/**************************************************************\
 Purpose: Enable global counting
 Input:
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + ENABLE_GBL_COUNT);
   *spec_adr = 0x0;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_all_disable(const DWORD base_adr)
/**************************************************************\
 Purpose: Disable global counting
 Input:
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + DISABLE_GBL_COUNT);
   *spec_adr = 0x0;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_grp1_clear(const DWORD base_adr)
/**************************************************************\
 Purpose: Clear group 1 (1-8) channels + overflow bits
 Input:
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + COUNTER_GRP1_CLEAR);
   *spec_adr = 0x0;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_grp2_clear(const DWORD base_adr)
/**************************************************************\
 Purpose: Clear group 2 (9-16) channels + overflow bits
 Input:
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + COUNTER_GRP2_CLEAR);
   *spec_adr = 0x0;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_single_clear(const DWORD base_adr, const INT ch)
/**************************************************************\
 Purpose: Clear single scaler channels + overflow bits
 Input:
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   if (ch < 16) {
      spec_adr = (DWORD *) (A32D24 + base_adr + COUNTER_CLEAR + (ch << 2));
      *spec_adr = 0x0;
   }
}

/************ INLINE function for General command ***********/
INLINE void sis3803_single_OVFL_clear(const DWORD base_adr, INT ch)
/**************************************************************\
 Purpose: Clear single scaler channels + overflow bits
 Input:
  DWORD base_addr     : base address of the SIS3803
 Output:
    none
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   if (ch < 16) {
      spec_adr = (DWORD *) (A32D24 + base_adr + OVERFLOW_CLEAR + (ch << 2));
      *spec_adr = 0x0;
   }
}

/*********** INLINE function for General command ***********/
INLINE void sis3803_counter_read(const DWORD base_adr, INT ch, DWORD * pvalue)
/**************************************************************\
 Purpose: Reads one individual scaler
 Input:
  DWORD base_addr     : base address of the SIS3803
  INT   ch            : Channel to be read
  DWORD *pvalue       : destination pointer
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   if (ch < 16) {
      spec_adr = (DWORD *) (A32D24 + base_adr + COUNTER_READ + (ch << 2));
      *pvalue = *spec_adr;
   }
}

/*********** INLINE function for General command ***********/
INLINE void sis3803_all_read(const DWORD base_adr, DWORD * pvalue)
/**************************************************************\
 Purpose: Reads all 16 channels scaler
 Input:
  DWORD base_addr     : base address of the SIS3803
  DWORD *pvalue       : destination pointer
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   INT i;
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + COUNTER_READ);
   for (i = 0; i < 16; i++) {
      *pvalue++ = *spec_adr++;
   }
}

/*********** INLINE function for General command ***********/
INLINE void sis3803_grp1_read(const DWORD base_adr, DWORD * pvalue)
/**************************************************************\
 Purpose: Reads group 1 (1..8) channels scaler
 Input:
  DWORD base_addr     : base address of the SIS3803
  DWORD *pvalue       : destination pointer
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   INT i;
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + COUNTER_READ);
   for (i = 0; i < 8; i++) {
      *pvalue++ = *spec_adr++;
   }
}

/*********** INLINE function for General command ***********/
INLINE void sis3803_grp2_read(const DWORD base_adr, DWORD * pvalue)
/**************************************************************\
 Purpose: Reads group 1 (1..8) channels scaler
 Input:
  DWORD base_addr     : base address of the SIS3803
  DWORD *pvalue       : destination pointer
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   INT i;
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + COUNTER_GRP2_READ);
   for (i = 0; i < 8; i++) {
      *pvalue++ = *spec_adr++;
   }
}

/*********** INLINE function for General command ***********/
INLINE void sis3803_all_read_clear(const DWORD base_adr, DWORD * pvalue)
/**************************************************************\
 Purpose: Reads & clear all 16 channels scaler
 Input:
  DWORD base_addr     : base address of the SIS3803
  DWORD *pvalue       : destination pointer
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   INT i;
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + COUNTER_READ_CLEAR);
   for (i = 0; i < 16; i++) {
      *pvalue++ = *spec_adr++;
   }
}

/************ INLINE function for General command ***********/
INLINE void sis3803_OVFL_grp1_read(const DWORD base_adr, DWORD * reg)
/**************************************************************\
 Purpose: Read Overflow reg for group 1 (1-8) 
 Input:
  DWORD base_addr     : base address of the SIS3803
 Output:
    reg               : pointer to overflow reg 
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + OVERFLOW_REG1_8);
   *reg = *spec_adr;
   *reg >>= 24;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_OVFL_grp2_read(const DWORD base_adr, DWORD * reg)
/**************************************************************\
 Purpose: Read Overflow reg for group 2 (9-16) 
 Input:
  DWORD base_addr     : base address of the SIS3803
 Output:
    reg               : pointer to overflow reg 
 Function value:
 none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + OVERFLOW_REG9_16);
   *reg = *spec_adr;
   *reg >>= 24;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_int_source_enable(const DWORD base_adr, const int intnum)
/**************************************************************\
 Purpose: Enable the interrupt for the bitwise input intnum (0xff).
          The interrupt vector is then the VECTOR_BASE
 Input:
  DWORD * base_addr       : base address of the sis3803
  DWORD * intnum          : interrupt number (input 0:7)
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;
   DWORD int_source;

   spec_adr = (DWORD *) (A32D24 + base_adr + CSR_WRITE);
   switch (intnum) {
   case 0:
      int_source = ENABLE_IRQ_EN_BS0;
      break;
   case 1:
      int_source = ENABLE_IRQ_EN_BS1;
      break;
   case 2:
      int_source = ENABLE_IRQ_EN_BS2;
      break;
   default:
      logMsg("Unknown interrupt source (%d)\n", int_source, 0, 0, 0, 0, 0);
   }
   *spec_adr = int_source;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_int_source_disable(const DWORD base_adr, const int intnum)
/**************************************************************\
 Purpose: Disable the interrupt for the bitwise input intnum (0xff).
          The interrupt vector is then the VECTOR_BASE+intnum
 Input:
  DWORD * base_addr       : base address of the sis3803
  int     level           : IRQ level (1..7)
  int   * intnum          : interrupt number (input 0..3)
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;
   DWORD int_source;

   spec_adr = (DWORD *) (A32D24 + base_adr + CSR_WRITE);
   switch (intnum) {
   case 0:
      int_source = DISABLE_IRQ_DI_BS0;
      break;
   case 1:
      int_source = DISABLE_IRQ_DI_BS1;
      break;
   case 2:
      int_source = DISABLE_IRQ_DI_BS2;
      break;
   default:
      logMsg("Unknown interrupt source (%d)\n", int_source, 0, 0, 0, 0, 0);
   }
   *spec_adr = int_source;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_int_source(const DWORD base_adr, DWORD int_source)
/**************************************************************\
 Purpose: Enable the one of the 4 interrupt using the
          predefined parameters (see sis3803.h)
 Input:
  DWORD * base_addr       : base address of the sis3803
  DWORD * intnum          : interrupt number (input 0..3)
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   spec_adr = (DWORD *) (A32D24 + base_adr + CSR_WRITE);

   int_source &= (ENABLE_IRQ_EN_BS0 | ENABLE_IRQ_EN_BS1
                  | ENABLE_IRQ_EN_BS2
                  | DISABLE_IRQ_DI_BS0 | DISABLE_IRQ_DI_BS1 | DISABLE_IRQ_DI_BS2);
   *spec_adr = int_source;
}

/************ INLINE function for General command ***********/
INLINE void sis3803_int_attach(const DWORD base_adr, DWORD base_vect, int level,
                               void (*isr) (void))
/**************************************************************\
 Purpose: Book an ISR for a bitwise set of interrupt input (0xff).
          The interrupt vector is then the VECTOR_BASE+intnum
 Input:
  DWORD * base_addr      : base address of the sis3803
  DWORD base_vect        : base vector of the module
  int   level            : IRQ level (1..7)
  DWORD isr_routine      : interrupt routine pointer
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;

   /* disable all IRQ sources */
/*  Should be done but not in attach 
  sis3803_int_source(base_adr
			     , DISABLE_IRQ_DI_BS0
			     | DISABLE_IRQ_DI_BS1
			     | DISABLE_IRQ_DI_BS2);
*/

#ifdef OS_VXWORKS
   if ((level < 8) && (level > 0) && (base_vect < 0x100)) {
      spec_adr = (DWORD *) (A32D24 + base_adr + IRQ_REG);
      *spec_adr = (level << 8) | VME_IRQ_ENABLE | base_vect;
      sysIntEnable(level);      /* interrupt level */
   }
   intConnect(INUM_TO_IVEC(base_vect), (VOIDFUNCPTR) isr, 0);
#else
   printf("Not implemented for this OS\n");
#endif
}

/************ INLINE function for General command ***********/
INLINE void sis3803_int_detach(const DWORD base_adr, DWORD base_vect, int level)
/**************************************************************\
 Purpose: Unbook an ISR for a bitwise set of interrupt input (0xff).
          The interrupt vector is then the VECTOR_BASE+intnum
 Input:
  DWORD * base_addr       : base address of the sis3803
  DWORD base_vect        : base vector of the module
  int   level            : IRQ level (1..7)
 Output:
    none
 Function value:
    none
\**************************************************************/
{

   /* disable all IRQ sources */
/*  Should be done but not in detach 
  sis3803_int_source(base_adr   , DISABLE_IRQ_DI_BS0
		    | DISABLE_IRQ_DI_BS1
		    | DISABLE_IRQ_DI_BS2);
*/

#ifdef OS_VXWORKS
/* Suppose to save the default isr before attach but would
 * required isr table in case of multiple IRQ
 * for now restore with myStub()
 */
   intConnect(INUM_TO_IVEC(base_vect), (VOIDFUNCPTR) myStub_sis3803, 0);
   sysIntDisable(level);        /* interrupt level */
#else
   printf("vector : 0x%x\n", base_vect + intnum);
#endif
}

/************ INLINE function for General command ***********/
INLINE void sis3803_int_clear(const DWORD base_adr, const int intnum)
/**************************************************************\
 Purpose: Disable the interrupt for the bitwise input intnum (0xff).
          The interrupt vector is then the VECTOR_BASE+intnum
 Input:
  DWORD * base_addr       : base address of the sis3803
  int   * intnum          : interrupt number (input 0..3)
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   volatile DWORD *spec_adr;
   DWORD int_source;

   spec_adr = (DWORD *) (A32D24 + base_adr + CSR_WRITE);
   switch (intnum) {
   case 0:
      int_source = DISABLE_IRQ_DI_BS0;
      *spec_adr = int_source;
      int_source = ENABLE_IRQ_EN_BS0;
      *spec_adr = int_source;
      break;
   case 1:
      int_source = DISABLE_IRQ_DI_BS1;
      *spec_adr = int_source;
      int_source = ENABLE_IRQ_EN_BS1;
      *spec_adr = int_source;
      break;
   case 2:
      int_source = DISABLE_IRQ_DI_BS2;
      *spec_adr = int_source;
      int_source = ENABLE_IRQ_EN_BS2;
      *spec_adr = int_source;
      break;
   default:
      logMsg("Unknown interrupt source (%d)\n", int_source, 0, 0, 0, 0, 0);
   }
}

/************ INLINE function for General command ***********/
void sis3803_setup(const DWORD base_adr, int mode, int dsp)
{
   switch (mode) {
   case 1:
      /* */
      if (dsp) {
         printf("Setting          : %d \t", mode);
         printf("Info             : PAA/June 2000\n");
      }
      sis3803_module_reset(base_adr);
      sis3803_input_mode(base_adr, 1);  /* mode 1 ONLY */
      sis3803_all_enable(base_adr);
      sis3803_channel_enable(base_adr, 0xf);    /* 4 channels */
      if (dsp)
         SIS3803_CSR_read(base_adr);
      if (dsp)
         printf("# of active ch.  : 4\n");
      break;
   default:
      printf("Undefined mode %d (known mode :1, 2, 3, 4, 5)\n", mode);
      break;
   }
}

/************ INLINE function for General command ***********/
void sis3803(void)
/**************************************************************\
 Purpose: Display SIS3803 build-in commands
 Input:
    none
 Output:
    none
 Function value:
    none
\**************************************************************/
{
   printf("\n---> SIS 3803 32 MultiScalers (sis3803.c) <---\n");
   printf("Inline : sis3803_module_ID (vmeBase);\n");
   printf("Inline : sis3803_IRQ_REG_read(vmeBase);\n");
   printf("Inline : sis3803_IRQ_REG_write(vmeBase, irq);\n");
   printf("Inline : sis3803_module_reset (vmeBase);\n");
   printf("Inline : sis3803_ref1 (vmeBase, enable/disable);\n");
   printf("Inline : sis3803_channel_enable (vmeBase, pattern);\n");
   printf("Inline : sis3803_channel_disable (vmeBase, pattern);\n");
   printf("Inline : sis3803_CSR_read (vmeBase, what);\n");
   printf("Inline : sis3803_CSR_write (vmeBase, what);\n");
   printf("Inline : sis3803_test_enable(vmeBase);\n");
   printf("Inline : sis3803_test_disable(vmeBase);\n");
   printf("Inline : sis3803_all_clear(vmeBase);\n");
   printf("Inline : sis3803_all_enable(vmeBase);\n");
   printf("Inline : sis3803_all_disable(vmeBase);\n");
   printf("Inline : sis3803_grp1_clear(vmeBase);\n");
   printf("Inline : sis3803_grp2_clear(vmeBase);\n");
   printf("Inline : sis3803_single_clear(vmeBase, ch);\n");
   printf("Inline : sis3803_single_OVFL_clear(vmeBase, ch);\n");
   printf("Inline : sis3803_counter_read(vmeBase, ch, *p);\n");
   printf("Inline : sis3803_all_read(vmeBase, *p);\n");
   printf("Inline : sis3803_grp1_read(vmeBase, *p);\n");
   printf("Inline : sis3803_grp2_read(vmeBase, *p);\n");
   printf("Inline : sis3803_all_read_clear(vmeBase, *p);\n");
   printf("Inline : sis3803_OVFL_grp1_read(vmeBase, *p);\n");
   printf("Inline : sis3803_OVFL_grp2_read(vmeBase, *p);\n");
   printf("Inline : sis3803_int_source (vmeBase, enable/disable);\n");
   printf("Inline : sis3803_int_source_enable (vmeBase, source (0..3));\n");
   printf("Inline : sis3803_int_source_disable (vmeBase, source (0..3));\n");
   printf("Inline : sis3803_int_attach (vmeBase, vect, level, void (*isr)(void));\n");
   printf("Inline : sis3803_int_detach (vmeBase, vect, level);\n");
   printf("Inline : sis3803_int_clear (vmeBase, source (0..3));\n");
   printf("         sis3803_setup(vmeBase, mode, display);\n");
   printf("calls  : SIS3803_setup( base_adr, mode)\n");
   printf("calls  : SIS3803_CSR_read( base_adr)\n");
   printf("calls  : SIS3803_all_read( base_adr) 'sis3803_counter_read()'\n");
   printf("calls  : SIS3803_all_read_clear( base_adr) 'sis3803_single_clear'\n");
   printf("calls  : SIS3803_all_bread( base_adr) 'sis3803_all_read'\n");
   printf("calls  : SIS3803_all_bcread( base_adr) 'sis3803_all_read_clear'\n");
}

/**************************************************************/
void SIS3803_setup(const DWORD base_adr, int mode)
{
   sis3803_setup(base_adr, mode, 1);
}

void SIS3803_CSR_read(const DWORD base_adr)
{
   DWORD csr;

   csr = sis3803_CSR_read(base_adr, CSR_FULL);

   printf("Module Version   : %d\t", ((sis3803_module_ID(base_adr) & 0xf000) >> 12));
   printf("Module ID        : %4.4x\n", (sis3803_module_ID(base_adr) >> 16));
   printf("LED              : %s \t", (csr & IS_LED) ? "Y" : "N");
   printf("Input mode       : %d \n", sis3803_input_mode(base_adr, 1));
   printf("Reference pulse 1: %s \t", (csr & IS_REF1) ? "Y" : "N");
   printf("Global Enable    : %s \t", (csr & IS_GBL_ENABLE) ? "Y" : "N");
   printf("Global OVFL bit  : %s \n", (csr & IS_GBL_OVFL) ? "Y" : "N");
   printf("IRQ enable BS0   : %s \t", (csr & IS_IRQ_EN_BS0) ? "Y" : "N");
   printf("IRQ enable BS1   : %s \t", (csr & IS_IRQ_EN_BS1) ? "Y" : "N");
   printf("IRQ enable BS2   : %s \n", (csr & IS_IRQ_EN_BS2) ? "Y" : "N");
   printf("IRQ Bit Source 0 : %s \t", (csr & IS_IRQ_BS0) ? "Y" : "N");
   printf("IRQ Bit Source 1 : %s \t", (csr & IS_IRQ_BS1) ? "Y" : "N");
   printf("IRQ Bit Source 2 : %s \n", (csr & IS_IRQ_BS2) ? "Y" : "N");
   printf("internal VME IRQ : %s \t", (csr & 0x4000000) ? "Y" : "N");
   printf("VME IRQ          : %s \n", (csr & 0x8000000) ? "Y" : "N");
}

void SIS3803_all_read(const DWORD base_adr)
{
   DWORD count;
   INT i;

   for (i = 0; i < 16; i++) {
      sis3803_counter_read(base_adr, i, &count);
      printf("Counter[%i] : %u\n", i, count);
   }
}

void SIS3803_all_read_clear(const DWORD base_adr)
{
   DWORD count;
   INT i;

   for (i = 0; i < 16; i++) {
      sis3803_counter_read(base_adr, i, &count);
      sis3803_single_clear(base_adr, i);
      printf("Counter[%i] : %u\n", i, count);
   }
}

void SIS3803_all_bread(const DWORD base_adr)
{
   DWORD count[16];
   INT i;

   sis3803_all_read(base_adr, &(count[0]));
   for (i = 0; i < 16; i++) {
      printf("Counter[%i] : %u 0x%x\n", i, count[i], count[i]);
   }
}

void SIS3803_all_bcread(const DWORD base_adr)
{
   DWORD count[16];
   INT i;

   sis3803_all_read_clear(base_adr, &(count[0]));
   for (i = 0; i < 16; i++) {
      printf("Counter[%i] : %u\n", i, count[i]);
   }
}

DWORD *p = NULL;

void test(DWORD mem)
{
   PART_ID part_id;

   if (p != NULL)
      free(p);
   (char *) p = malloc(mem);
}
