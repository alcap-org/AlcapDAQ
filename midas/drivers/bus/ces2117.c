/********************************************************************\

  Name:         ces2117.c
  Created by:   Stefan Ritt

  Cotents:      CAMAC functions for CES VCC 2117 controller

  $Log: ces2117.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:06  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/01/08 08:39:19  midas
  Removed garbage at end of file

  Revision 1.1  1999/12/20 10:18:11  midas
  Reorganized driver directory structure

  Revision 1.3  1998/10/23 14:21:49  midas
  - Modified version scheme from 1.06 to 1.6.0
  - cm_get_version() now returns versino as string

  Revision 1.2  1998/10/12 12:18:56  midas
  Added Log tag in header


\********************************************************************/

#include <mcstd.h>

/*---- input -------------------------------------------------------*/

INLINE void cami(int c, int n, int a, int f, unsigned short *d)
{
   *d = (unsigned short)
       *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2)) & 0xFFFFFF;
}

INLINE void cam16i(int c, int n, int a, int f, unsigned short *d)
{
   *d = (unsigned short)
       *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2)) & 0xFFFFFF;
}


INLINE void cam24i(int c, int n, int a, int f, unsigned long *d)
{
   *d = *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2)) & 0xFFFFFF;
}

INLINE void cam16i_q(int c, int n, int a, int f, unsigned short *d, int *x, int *q);
{
   unsigned long tmp;

   tmp = *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2));
   *x = (tmp & 0x40000000) != 0;
   *q = (tmp & 0x80000000) != 0;
   *d = (unsigned short) tmp;
}

INLINE void cam24i_q(int c, int n, int a, int f, unsigned long *d, int *x, int *q);
{
   unsigned long tmp;

   tmp = *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2));
   *x = (tmp & 0x40000000) != 0;
   *q = (tmp & 0x80000000) != 0;
   *d = tmp & 0xFFFFFF;
}

INLINE void cam16i_r(int c, int n, int a, int f, unsigned short *d, int r);
{
   unsigned long tmp;
   unsigned short q;

   do {
      tmp = *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2));
      *q = (tmp & 0x80000000) != 0;
      *(d++) = (unsigned short) tmp;
   } while (q);
}

INLINE void cam24i_r(int c, int n, int a, int f, unsigned long *d, int r);
{
   unsigned long tmp;
   unsigned short q;

   do {
      tmp = *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2));
      *q = (tmp & 0x80000000) != 0;
      if (!q)
         break;

      *(d++) = tmp & 0xFFFFFF;
   } while (1);
}

INLINE void cam16i_sa(int c, int n, int a, int f, unsigned short *d, int r);
{
   unsigned long tmp;
   unsigned short q;

   for (i = 0; i < r; i++) {
      tmp = *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2));
      *q = (tmp & 0x80000000) != 0;
      if (!q)
         break;

      *(d++) = (unsigned short) tmp;
      a++;
   }
}

INLINE void cam24i_sa(int c, int n, int a, int f, unsigned long *d, int r);
{
   unsigned long tmp;
   unsigned short q;

   for (i = 0; i < r; i++) {
      tmp = *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2));
      *q = (tmp & 0x80000000) != 0;
      if (!q)
         break;

      *(d++) = tmp & 0xFFFFFF;
      a++;
   }
}

INLINE void cam16i_sn(int c, int n, int a, int f, unsigned short *d, int r)
{
   unsigned long tmp;
   unsigned short q;

   for (i = 0; i < r; i++) {
      tmp = *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2));
      *q = (tmp & 0x80000000) != 0;
      if (!q)
         break;

      *(d++) = (unsigned short) tmp;
      n++;
   }
}

INLINE void cam24i_sn(int c, int n, int a, int f, unsigned long *d, int r)
{
   unsigned long tmp;
   unsigned short q;

   for (i = 0; i < r; i++) {
      tmp = *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2));
      *q = (tmp & 0x80000000) != 0;
      if (!q)
         break;

      *(d++) = tmp & 0xFFFFFF;
      n++;
   }
}

/*---- output ------------------------------------------------------*/

INLINE void camo(int c, int n, int a, int f, unsigned short d)
{
   *((unsigned long *) (0xC90000 | ((((f << 5) | n) << 4) | a) << 2)) = (d);
}

INLINE void cam16o(int c, int n, int a, int f, unsigned short d)
{
   *((unsigned long *) (0xC90000 | ((((f << 5) | n) << 4) | a) << 2)) = (d);
}

INLINE void cam24o(int c, int n, int a, int f, unsigned long d)
{
   *((unsigned long *) (0xC90000 | ((((f << 5) | n) << 4) | a) << 2)) = (d);
}

INLINE void cam16o_q(int c, int n, int a, int f, unsigned short d, int *x, int *q)
{
   unsigned long status;

   *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2)) = (d);
   status = *((unsigned long *) (0xC00780));
   *x = (status & 0x40000000) != 0;
   *q = (status & 0x80000000) != 0;
}

INLINE void cam24o_q(int c, int n, int a, int f, unsigned long d, int *x, int *q)
{
   unsigned long status;
   *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2)) = (d);
   status = *((unsigned long *) (0xC00780));
   *x = (status & 0x40000000) != 0;
   *q = (status & 0x80000000) != 0;
}

/*---- control -----------------------------------------------------*/

INLINE void camc(int c, int n, int a, int f)
{
   *((unsigned long *) (0xC90000 | ((((f << 5) | n) << 4) | a) << 2))
       = 0;
}


INLINE void camc_sa(int c, int n, int a, int f, int r)
{
   int i;
   for (i = 0; i < r; i++) {
      *((unsigned long *) (0xC90000 | ((((f << 5) | n) << 4) | a) << 2)) = 0;
      a++;
   }
}

INLINE void camc_sn(int c, int n, int a, int f, int r)
{
   int i;
   for (i = 0; i < r; i++) {
      *((unsigned long *) (0xC90000 | ((((f << 5) | n) << 4) | a) << 2)) = 0;
      n++;
   }
}

INLINE void camq(int c, int n, int a, int f, int *q)
{
   unsigned long tmp;
   tmp = *((unsigned long *) (0xC00000 | ((((f << 5) | n) << 4) | a) << 2));
   *q = (tmp & 0x80000000) != 0;
}

/*---- general functions -------------------------------------------*/

void cam_init(void)
{
};
void cam_exit(void)
{
};
INLINE void cam_set_inhibit(int c)
{
   *((unsigned long *) 0xC9E7A4) = 0;
}

INLINE void cam_clear_inhibit(int c)
{
   *((unsigned long *) 0xC9C7A4) = 0;
}

INLINE void cam_c_cycle(int c)
{
   *((unsigned long *) 0xC9D724) = 0;
}

INLINE void cam_c_cycle(int c)
{
   *((unsigned long *) 0xC9D720) = 0;
}

/*---- LAM functions -----------------------------------------------*/

/* addresses of the CIO data/ctrl ports                                   */
#define CIO1_BASE_ADRS          ((char *)0x00e00100)
#define CIO1_DATA_C_ADRS        ((char *)(CIO1_BASE_ADRS + 0x00))
#define CIO1_DATA_B_ADRS        ((char *)(CIO1_BASE_ADRS + 0x04))
#define CIO1_DATA_A_ADRS        ((char *)(CIO1_BASE_ADRS + 0x08))
#define CIO1_CNTRL_ADRS         ((char *)(CIO1_BASE_ADRS + 0x0c))
#define CIO2_BASE_ADRS          ((char *)0x00e00200)
#define CIO2_DATA_C_ADRS        ((char *)(CIO2_BASE_ADRS + 0x00))
#define CIO2_DATA_B_ADRS        ((char *)(CIO2_BASE_ADRS + 0x04))
#define CIO2_DATA_A_ADRS        ((char *)(CIO2_BASE_ADRS + 0x08))
#define CIO2_CNTRL_ADRS         ((char *)(CIO2_BASE_ADRS + 0x0c))
#define CIO3_BASE_ADRS          ((char *)0x00e00300)
#define CIO3_DATA_C_ADRS        ((char *)(CIO3_BASE_ADRS + 0x00))
#define CIO3_DATA_B_ADRS        ((char *)(CIO3_BASE_ADRS + 0x04))
#define CIO3_DATA_A_ADRS        ((char *)(CIO3_BASE_ADRS + 0x08))
#define CIO3_CNTRL_ADRS         ((char *)(CIO3_BASE_ADRS + 0x0c))

/* interupt base vectors used                                             */
#define CIO1_IVA                0xD0
#define CIO1_IVB                0xE0
#define CIO2_IVA                0xF0

#define wait __asm__("nop; nop;")       /* wait two NOPs between CIO accesses */


INLINE void cam_lam_enable(int c, int n)
{
   char lam_mask;
   char *cioCtrl;
   int i;
   /* enable CIO-interrupt */
   cioCtrl = ((n <= 16) ? CIO1_CNTRL_ADRS : CIO2_CNTRL_ADRS);
   lam_mask = *cio_ADRS;
   wait;
   *cio_ADRS = (n <= 8 || n > 16) ? ZCIO_PAPM : ZCIO_PBPM;
   wait;
   lam_mask = *cio_ADRS;
   if (l) {
      actLM |= lam->lm;
   } else {
      actLM &= ~lam->lm;
   };
   i = intLock();
   /* Locking out IRQs necesarry at this point. Otherwise setting the    */
   /* CIOs pattern mask bit correponding to the LAM will generate one    */
   /* extra interrupt                                                    */
   wait;
   *lam->cc = lam->pm;
   wait;
   *lam->cc = actLM;
   wait;
   *lam->cc = lam->cs;
   wait;
   *lam->cc = ZCIO_CS_CLIPIUS;
   wait;
   intUnlock(i);
   /* enable the modules LAM */
   camc(c, n, 0, 26);
}

INLINE void cam_lam_disable(int c, int n)
{
   char actLM;
   int i;
   if (lam->cc != NULL) {       /* enable/disable CIO-interrupt   */
      actLM = *lam->cc;
      wait;
      *lam->cc = lam->pm;
      wait;
      actLM = *lam->cc;
      if (l) {
         actLM |= lam->lm;
      } else {
         actLM &= ~lam->lm;
      };
      i = intLock();
      /* Locking out IRQs necesarry at this point. Otherwise setting the    */
      /* CIOs pattern mask bit correponding to the LAM will generate one    */
      /* extra interrupt                                                    */
      wait;
      *lam->cc = lam->pm;
      wait;
      *lam->cc = actLM;
      wait;
      *lam->cc = lam->cs;
      wait;
      *lam->cc = ZCIO_CS_CLIPIUS;
      wait;
      intUnlock(i);
   }
   /* enable/disable the modules LAM */
   if (l)
      return ((((*(int *) (lam->ext | (26 << VCCshiftF))) & VCCmaskX)
               != 0) ? 0 : -1);
   return ((((*(int *) (lam->ext | (24 << VCCshiftF))) & VCCmaskX)
            != 0) ? 0 : -1);
}


INLINE void cam_lam_read(int c, int *lam)
{
   *lam = *((unsigned long *) (0xC00780));
}
