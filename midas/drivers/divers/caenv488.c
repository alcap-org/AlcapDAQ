/*-----------------------------------------------------------------------------
 *  Copyright (c) 1996      TRIUMF Data Acquistion Group
 *  Please leave this header in any reproduction of that distribution
 *
 *  TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 *  Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * -----------------------------------------------------------------------------
 *
 *  Description	: function library for the CAEN V488 *TDC chan.
 *                no interrupt support yet
 *  Requires 	:
 *  Author:  Pierre-Andre Amaudruz Data Acquisition Group
 *  Revision History:
 *  Revision 1.0  1998/Jun02 Pierre	 Initial revision
 *
 *---------------------------------------------------------------------------*/

#ifdef OS_VXWORKS
#include "vxWorks.h"
#include "vme.h"
#endif

#define A24D16_V488  0xf0000000 /* A24D16 access */

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

#ifdef __alpha
typedef unsigned int DWORD;
#else
typedef unsigned long int DWORD;
#endif

#endif                          /* MIDAS_TYPE_DEFINED */

#define VER_SER     0xFE        /* version _ serial# */
#define MAN_MOD     0xFC        /* manufactor ID _ mode type */
#define FIXED_CODE  0xFA        /* fix code ID */
#define HF_REG      0x1E        /* Half full reg */
#define RESET_REG   0x1C        /* reset reg */
#define CTL_REG     0x1A        /* control reg */
#define OUTB_REG    0x18        /* Output buffer reg */
#define FF_REG      0x16        /* Fifo full reg */
#define RR_REG      0x14        /* Range reg */
#define THRH_REG    0x12        /* High threshold reg */
#define THRL_REG    0x10        /* Low threshold reg */
#define INT_REG     0x00        /* interrupt reg */
#define V488_CSTART 0x8000      /* Common start */
#define V488_IE1    0x1
#define V488_IE2    0x2
#define V488_IE3    0x4
#define V488_IE4    0x8
#define V488_IE5    0x10
#define V488_IE6    0x20
#define V488_IE7    0x40
#define V488_IE8    0x80
#define V488_IE14   0xf
#define V488_IE58   0xf0
#define V488_IE18   0xff

INLINE caenv488_HF_set(const DWORD base, const WORD mode)
{
   volatile WORD *spec_adr;

   spec_adr = (WORD *) (A24D16_V488 | base | HF_REG);
   if (mode)
      *spec_adr = 0x1 << 12;
   else
      *spec_adr = 0x0;
}

INLINE caenv488_HF_get(const DWORD base, WORD * mode)
{
   volatile WORD *spec_adr;

   spec_adr = (WORD *) (A24D16_V488 | base | HF_REG);
   *mode = *spec_adr;
   *mode = (*mode >> 12) & 0x1;
}

INLINE caenv488_reset(const DWORD base)
{
   volatile WORD *spec_adr;

   spec_adr = (WORD *) (A24D16_V488 | base | RESET_REG);
   *spec_adr = 0x0;
}

INLINE caenv488_CTL_write(const DWORD base, const WORD mode)
{
   volatile WORD *spec_adr;
   volatile WORD ctl;

   spec_adr = (WORD *) (A24D16_V488 | base | CTL_REG);
   ctl = *spec_adr;
   ctl |= mode & 0x80ff;
   *spec_adr = ctl & 0x80ff;
}

INLINE caenv488_CTL_read(const DWORD base, WORD * mode)
{
   volatile WORD *spec_adr;

   spec_adr = (WORD *) (A24D16_V488 | base | CTL_REG);
   *mode = *spec_adr;
}

INLINE caenv488_RR_write(const DWORD base, const WORD mode)
{
   volatile WORD *spec_adr;

   spec_adr = (WORD *) (A24D16_V488 | base | RR_REG);
   *spec_adr = mode & 0x11ff;
}

INLINE caenv488_THRH_write(const DWORD base, const WORD mode)
{
   volatile WORD *spec_adr;

   spec_adr = (WORD *) (A24D16_V488 | base | THRH_REG);
   *spec_adr = mode & 0xff;
}

INLINE caenv488_THRL_write(const DWORD base, const WORD mode)
{
   volatile WORD *spec_adr;

   spec_adr = (WORD *) (A24D16_V488 | base | THRL_REG);
   *spec_adr = mode & 0xff;
}

INLINE caenv488_FF_set(const DWORD base)
{
   volatile WORD *spec_adr;

   spec_adr = (WORD *) (A24D16_V488 | base | FF_REG);
   *spec_adr = 0x0;
}

INLINE caenv488_read(const DWORD base, WORD ** data)
{
   volatile WORD *spec_adr;
   int header, mult;

   spec_adr = (WORD *) (A24D16_V488 | base | OUTB_REG);
   header = *spec_adr;
   if (header & 0x8000) {
      if ((header & 0xfff) != 1)
         logMsg("Event counter in V488 base:0x%x = %d", base, (header & 0xfff), 0, 0, 0,
                0);
      else {
         mult = (header >> 12) & 0x7;
         while (mult > 0) {
            *((*data)++) = *spec_adr;
         }
      }
   } else
      logMsg("Event out of sequence in V488 base 0x%x", base, 0, 0, 0, 0, 0);
}

/*-Tests---------------------------------------------------------*/
void caenv488(void)
{
   printf("\n CAEN V488 8ch. TDC Tool Box\n");
   printf("Inline : caenv488_HF_set(DWORD vmeBase);\n");
   printf("Inline : caenv488_HF_get(DWORD vmeBase, INT * mode);\n");
   printf("Inline : caenv488_reset(DWORD vmeBase);\n");
   printf("Inline : caenv488_CTL_write(DWORD vmeBase, WORD mode);\n");
   printf("Inline : caenv488_CTL_read(DWORD vmeBase, WORD * mode);\n");
   printf("Inline : caenv488_RR_write(DWORD vmeBase, WORD mode);\n");
   printf("Inline : caenv488_THRH_write(DWORD vmeBase, WORD value);\n");
   printf("Inline : caenv488_FF_set(DWORD vmeBase, WORD value);\n");
   printf("Inline : caenv488_read(DWORD vmeBase, WORD ** data);\n");
   printf("Test   : caenv488_MIW(DWORDS base)\n");
   printf("Test   : caenv488_status(DWORD base)\n");
   printf("Test   : caenv488_cstop_set(DWORDS base)\n");
   printf("Test   : caenv488_cstart_set(DWORDS base)\n");
   printf("Test   : caenv488_all_enable(DWORDS base)\n");
   printf("Test   : caenv488_all_disable(DWORDS base)\n");
   printf("Test   : caenv488_std_init(DWORDS base)\n");
   printf("Test   : caenv488_outb_read(DWORDS base)\n");
}

INLINE void caenv488_MIW(DWORD base)
{
   volatile WORD *spec_adr;
   volatile WORD data;

   /* module */
   spec_adr = (WORD *) (A24D16_V488 | base | VER_SER);
   data = *spec_adr;
   printf("Version  : 0x%x - Serial#   : 0x%x\n", (data >> 12) & 0xf, data & 0xfff);
   spec_adr = (WORD *) (A24D16_V488 | base | MAN_MOD);
   data = *spec_adr;
   printf("Manuf.#  : 0x%x - Mod. type#: 0x%x\n", (data >> 10) & 0x3f, data & 0x3ff);
   spec_adr = (WORD *) (A24D16_V488 | base | FIXED_CODE);
   data = *spec_adr;
   printf("FA code  : 0x%x - F5 code  : 0x%x\n", (data >> 8) & 0xff, data & 0xff);

   /* RR */
   spec_adr = (WORD *) (A24D16_V488 | base | RR_REG);
   data = *spec_adr;
   if (data & 0x1000)
      printf("Mode FIFO full      - ");
   else
      printf("Mode FIFO half full - ");

   /* CTL */
   spec_adr = (WORD *) (A24D16_V488 | base | CTL_REG);
   data = *spec_adr;
   printf("Ch.enable: 0x%x \n", data & 0xff);
   if (data & 0x8000)
      printf("Common Start  - ");
   else
      printf("Common Stop   - ");

   if (data & 0x4000)
      printf("FIFO empty\n");
   else if (data & 0x2000)
      printf("FIFO Full\n");
   else if (data & 0x1000)
      printf("FIFO half full\n");
}

INLINE void caenv488_status(DWORD base)
{
   volatile WORD *spec_adr;
   volatile WORD data;

   /* CTL */
   spec_adr = (WORD *) (A24D16_V488 | base | CTL_REG);
   data = *spec_adr;
   printf("CTL status: 0x%x - Ch.enable: 0x%x \n", data, data & 0xff);
   if (data & 0x8000)
      printf("Common Start       - ");
   else
      printf("Common Stop        - ");

   if (!(data & 0x4000))
      printf("FIFO empty\n");
   else if (!(data & 0x2000))
      printf("FIFO Full\n");
   else if (!(data & 0x1000))
      printf("FIFO half full\n");

   /* RR */
   spec_adr = (WORD *) (A24D16_V488 | base | RR_REG);
   data = *spec_adr;
   if (data & 0x1000)
      printf("Mode FIFO full     \n");
   else
      printf("Mode FIFO half full\n");
}

INLINE void caenv488_cstop_set(DWORD base)
{
   WORD ctl;

   caenv488_CTL_read(base, &ctl);
   ctl &= 0x70ff;
   caenv488_CTL_write(base, ctl);
}

INLINE void caenv488_cstart_set(DWORD base)
{
   caenv488_CTL_write(base, V488_CSTART);
}

INLINE void caenv488_all_enable(DWORD base)
{
   caenv488_CTL_write(base, V488_IE18);
}

INLINE void caenv488_all_disable(DWORD base)
{
   caenv488_CTL_write(base, 0x0);
}

INLINE void caenv488_std_init(DWORD base)
{
   caenv488_reset(base);
   caenv488_THRH_write(base, 0xc7);
   caenv488_THRL_write(base, 0x00);
   caenv488_RR_write(base, 0xe0);
   caenv488_all_enable(base);
}

void caenv488_outb_read(DWORD base)
{
   volatile WORD *spec_adr;
   volatile WORD data, header = 0;

   /* CTL */
   spec_adr = (WORD *) (A24D16_V488 | base | OUTB_REG);
   data = *spec_adr;
   for (;;) {
      if (data & 0x8000) {
         header = 1;
         printf("OutB header: 0x%x - ", data);
         printf("OutB multi : %d - event# : %d\n", ((data >> 12) & 0x7) + 1,
                data & 0xfff);
         if (((data >> 12) & 0x7) == 0)
            break;
      } else {
         if (header) {
            header = 0;
            printf("OutB data  : 0x%x - ", data);
            printf("OutB chan. : %d - data# : %d\n", (data >> 12) & 0x7, data & 0xfff);
            if (((data >> 12) & 0x7) == 0)
               break;
         } else {
            printf("OutB data  : 0x%x - No header\n", data);
            break;
         }
      }
   }
}
