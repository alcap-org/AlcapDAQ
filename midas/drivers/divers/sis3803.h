/*-----------------------------------------------------------------------------
 * Copyright (c) 1996      TRIUMF Data Acquistion Group
 * Please leave this header in any reproduction of that distribution
 * 
 * TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 * Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *        amaudruz@triumf.ca
 * ----------------------------------------------------------------------------
 *  
 * Description	: Header file for SIS3803 Standard 200MHz 16 channels scaler
 *
 * Author:  Pierre-Andre Amaudruz Data Acquisition Group
   $Log: sis3803.h,v $
   Revision 1.1.1.1  2005/06/20 23:37:09  mucap
   Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
   (Next, I'll commit our local customizations.)

   Revision 1.2  2004/01/08 07:51:21  midas
   Changed indentation

   Revision 1.1  2000/07/21 18:46:39  pierre
   - SIS3803 16ch 32-bit 200MHz scalers (www.struck.de)

 *---------------------------------------------------------------------------*/
#ifndef SIS3803_INCLUDE
#define SIS3803_INCLUDE

#include "stdio.h"
#include "string.h"

#ifdef OS_VXWORKS
#include "vme.h"
#include "vxWorks.h"
#include "intLib.h"
#include "sys/fcntlcom.h"
#ifdef PPCxxx
#include "arch/ppc/ivPpc.h"
#else
#include "arch/mc68k/ivMc68k.h"
#endif
#include "taskLib.h"
#endif

#if defined( _MSC_VER )
#define INLINE __inline
#elif defined(__GNUC__)
#define INLINE __inline__
#else
#define INLINE
#endif

#define EXTERNAL extern

#ifndef MIDAS_TYPE_DEFINED
#define MIDAS_TYPE_DEFINED

typedef unsigned short int WORD;
typedef long int INT;

#ifdef __alpha
typedef unsigned int DWORD;
#else
typedef unsigned long int DWORD;
#endif

#endif                          /* MIDAS_TYPE_DEFINED */

#ifdef PPCxxx
#define A32D24	       0xfa000000       /* A32D24 camac base address */
#else
#define A32D24	       0xf0000000       /* A32D24 camac base address */
#endif

#define CSR_READ         0x0
#define CSR_FULL         0xffffffff     /* CSR Read */
#define CSR_WRITE        0x0
#define GET_MODE         0x0000000C     /* CSR read */

#define IS_REF1                0x00002000       /* CSR read */

#define IS_LED                 0x00000001       /* CSR read */
#define IS_IRQ_EN_BS0          0x00100000       /* CSR read */
#define IS_IRQ_EN_BS1          0x00200000       /* CSR read */
#define IS_IRQ_EN_BS2          0x00400000       /* CSR read */
#define IS_IRQ_BS0             0x10000000       /* CSR read */
#define IS_IRQ_BS1             0x20000000       /* CSR read */
#define IS_IRQ_BS2             0x40000000       /* CSR read */
#define IS_GBL_ENABLE          0x00008000       /* CSR read */
#define IS_GBL_OVFL            0x00004000       /* CSR read */
#define ENABLE_TEST_PULSE      0x00000030       /* CSR write */
#define DISABLE_TEST_PULSE     0x00003000       /* CSR write */
#define ENABLE_IRQ_EN_BS0      0x00100000       /* CSR write */
#define ENABLE_IRQ_EN_BS1      0x00200000       /* CSR write */
#define ENABLE_IRQ_EN_BS2      0x00400000       /* CSR write */
#define DISABLE_IRQ_DI_BS0     0x10000000       /* CSR write */
#define DISABLE_IRQ_DI_BS1     0x20000000       /* CSR write */
#define DISABLE_IRQ_DI_BS2     0x40000000       /* CSR write */
#define VME_IRQ_ENABLE         0x00000800       /* CSR write */

#define MODULE_ID             0x004
#define IRQ_REG               0x004
#define SELECT_COUNT_DIS_REG  0x00C
#define ALL_CLEAR             0x020
#define SHADOW_CLK_REG        0x024
#define ENABLE_GBL_COUNT      0x028
#define DISABLE_GBL_COUNT     0x02C
#define COUNTER_GRP1_CLEAR    0x040
#define COUNTER_GRP2_CLEAR    0x044
#define ENABLE_REF_CH1        0x050
#define DISABLE_REF_CH1       0x054
#define MODULE_RESET          0x060
#define SINGLE_TST_PULSE      0x068
#define PRESCALE_REG          0x080
#define COUNTER_CLEAR         0x100
#define OVERFLOW_CLEAR        0x180
#define SHADOW_READ           0x200
#define COUNTER_READ          0x280
#define COUNTER_GRP2_READ     0x2A0
#define COUNTER_READ_CLEAR    0x300
#define OVERFLOW_REG1_8       0x380
#define OVERFLOW_REG9_16      0x3A0

#define SOURCE_CIP           0
#define SOURCE_FIFO_FULL     1
#define SOURCE_FIFO_HFULL    2
#define SOURCE_FIFO_ALFULL   3

#define SIS3803_VECT_BASE 0x6f

INLINE DWORD sis3803_module_ID(const DWORD b);
INLINE void sis3803_module_reset(const DWORD b);
INLINE DWORD sis3803_IRQ_REG_read(const DWORD base_adr);
INLINE DWORD sis3803_IRQ_REG_write(const DWORD base_adr, DWORD irq);
INLINE DWORD sis3803_input_mode(const DWORD base_adr, DWORD mode);
INLINE DWORD sis3803_ref1(const DWORD b, DWORD endis);
INLINE void sis3803_channel_enable(const DWORD b, DWORD pat);
INLINE void sis3803_channel_disable(const DWORD b, DWORD pat);
INLINE DWORD sis3803_CSR_read(const DWORD b, const DWORD what);
INLINE DWORD sis3803_CSR_write(const DWORD b, const DWORD what);
INLINE void sis3803_test_enable(const DWORD b);
INLINE void sis3803_test_disable(const DWORD b);
INLINE void sis3803_all_clear(const DWORD b);
INLINE void sis3803_all_enable(const DWORD b);
INLINE void sis3803_all_disable(const DWORD b);
INLINE void sis3803_grp1_clear(const DWORD b);
INLINE void sis3803_grp2_clear(const DWORD b);
INLINE void sis3803_single_clear(const DWORD b, const INT ch);
INLINE void sis3803_single_OVFL_clear(const DWORD b, const INT ch);
INLINE void sis3803_counter_read(const DWORD b, INT ch, DWORD * p);
INLINE void sis3803_all_read(const DWORD b, DWORD * p);
INLINE void sis3803_grp1_read(const DWORD base_adr, DWORD * pvalue);
INLINE void sis3803_grp2_read(const DWORD base_adr, DWORD * pvalue);
INLINE void sis3803_all_read_clear(const DWORD b, DWORD * p);
INLINE void sis3803_OVFL_grp1_read(const DWORD b, DWORD * p);
INLINE void sis3803_OVFL_grp2_read(const DWORD b, DWORD * p);
INLINE void sis3803_int_source(const DWORD base_adr, DWORD int_source);
INLINE void sis3803_int_source_enable(const DWORD base_adr, const int intnum);
INLINE void sis3803_int_source_disable(const DWORD base_adr, const int intnum);
INLINE void sis3803_int_clear(const DWORD base_adr, const int intnum);
INLINE void sis3803_int_attach(const DWORD base_adr, DWORD base_vect, int level,
                               void (*isr) (void));
INLINE void sis3803_int_detach(const DWORD base_adr, DWORD base_vect, int level);
void sis3803_setup(const DWORD base_adr, int mode, int dsp);
void myStub_sis3803(void);

void SIS3803_setup(const DWORD base_adr, int mode);
void SIS3803_CSR_read(const DWORD base_adr);

#endif
