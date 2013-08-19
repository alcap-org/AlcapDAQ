/*-----------------------------------------------------------------------------
 *  Copyright (c) 1998      TRIUMF Data Acquistion Group
 *  Please leave this header in any reproduction of that distribution
 * 
 *  TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 *  Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * -----------------------------------------------------------------------------
 *  
 *  Description : CAMAC interface for CES8210 CAMAC controller using
 *      Midas Camac Standard calls.
 *  Requires  : a VME driver:
 *                              VxVME.c-> VxWorks/PPC
 *                              btxxx  -> Linux,NT/bit3
 *  Application :
 *  Author      : Pierre-Andre Amaudruz Data Acquisition Group
 *  $Log: ces8210.c,v $
 *  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
 *  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
 *  (Next, I'll commit our local customizations.)
 *
 *  Revision 1.11  2004/10/06 18:56:55  pierre
 *  fix definition MVME
 *
 *  Revision 1.10  2004/01/08 08:40:08  midas
 *  Implemented standard indentation
 *
 *  Revision 1.9  2003/12/01 18:31:51  pierre
 *  adapted for new mvmestd
 *
 *  Revision 1.8  2002/06/17 23:46:01  pierre
 *  fix camc R/W mode
 *
 *  Revision 1.7  2002/05/08 22:30:07  pierre
 *  24bit access arithmetic bug
 *
 *  Revision 1.6  2002/03/02 01:16:26  pierre
 *  fix _sx  loop
 *
 *  Revision 1.5  2002/02/08 06:52:10  pierre
 *  add inhibit_test, interrupt_test
 *
 *  Revision 1.4  2002/01/28 20:14:16  pierre
 *  added macros, fix camc_chk
 *
 *  Revision 1.3  2001/09/07 18:09:44  pierre
 *  Fix args for interrupts
 *
 *  Revision 1.2  2001/06/18 11:11:54  midas
 *  Major modifications (SR) to work with Bit3 MOdel 617 interface
 *
 *---------------------------------------------------------------------------*/

#ifndef INLINE
#if defined( _MSC_VER )
#define INLINE __inline
#elif defined(__GNUC__)
#define INLINE __inline__
#else
#define INLINE
#endif
#endif

#include <stdio.h>
#include <string.h>

#include "mcstd.h"              /* Provides CAMAC access */
#include "mvmestd.h"            /* using the generic Midas VME interface */

/* VME Base address for each crate */
typedef struct {
   int baseCrate;               /* translated VME crate address */
} CBD_DESC_T;

CBD_DESC_T cbd[8];              /* crate 0 ==>system CC mapping */

/* VME CAMAC CBD8210 base address */
#define CBD8210_BASE   0x800000 /* camac base address */
#define CBDWL_D16      0x000002
#define CBDWL_D24      0x000000
#define CSR_OFFSET     0x0000e800       /* camac control and status reg */
#define CAR_OFFSET     0x0000e820       /* Crate Address Register */
#define BTB_OFFSET     0x0000e824       /* on-line status (R) */
#define CSR            CBD8210_BASE | CSR_OFFSET | CBDWL_D16
#define CAR            CBD8210_BASE | CAR_OFFSET | CBDWL_D16
#define BTB            CBD8210_BASE | BTB_OFFSET | CBDWL_D16

#define CAM_CSRCHK(_w){\
       DWORD cbd_csr = cbd[0].baseCrate | CSR;\
       *_w = *(WORD *) cbd_csr;}

#define CAM_CARCHK(_w){\
       DWORD cbd_car = cbd[0].baseCrate | CAR;\
       *_w = *(WORD *) cbd_car;}

#define CAM_BTBCHK(_w){\
       DWORD cbd_btb = cbd[0].baseCrate | BTB;\
                         *_w = *(WORD *) cbd_btb;}

#define CAM_QXCHK(_q,_x){\
       DWORD cbd_csr = cbd[0].baseCrate | CSR;\
       WORD csr = *((WORD *) cbd_csr);\
       *_q = ((csr & 0x8000)>>15);\
       *_x = ((csr & 0x4000)>>14);}

#define CAM_QCHK(_q){\
       DWORD cbd_csr = cbd[0].baseCrate | CSR;\
       WORD csr = *((WORD *) cbd_csr);\
       *_q = ((csr & 0x8000)>>15);}
#define CAM_XCHK(_x){\
       DWORD cbd_csr = cbd[0].baseCrate | CSR;\
       WORD csr = *((WORD *) cbd_csr);\
       *_x = ((csr & 0x4000)>>14);}

/* VME handle */
int vh;

/*****************************************************************\
*                                                                 *
* Input functions                                                 *
*                                                                 *
\*****************************************************************/

INLINE void cam16i(const int c, const int n, const int a, const int f, WORD * d)
{
   int ext;

   ext = (cbd[c].baseCrate + CBDWL_D16 + (n << 11) + (a << 7) + (f << 2));
   *d = *((unsigned short *) ext);
/*  printf("cam16i: c%dn%da%df%d = ext[0x%x] -> 0x%x \n", c,n,a,f,ext, *d); */
}

/*---------------------------------------------------------------*/

INLINE void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   int ext;
   WORD dh, dl;

   ext = (cbd[c].baseCrate + CBDWL_D24 + (n << 11) + (a << 7) + (f << 2));
   dh = *((unsigned short *) ext);
/*  printf("cam24i: c%dn%da%df%d = ext[0x%x - ", c,n,a,f,ext); */
   ext += 2;
   dl = *((unsigned short *) ext);

   *d = ((dh << 16) | dl) & 0xffffff;
/*  printf("0x%x] -> (0x%x) - (0x%x) -> 0x%x \n", ext,dh,dl,*d); */
}

/*---------------------------------------------------------------*/

INLINE void cam16i_q(const int c, const int n, const int a, const int f,
                     WORD * d, int *x, int *q)
{
   WORD csr;

   cam16i(c, n, a, f, d);
   cami(0, 29, 0, 0, &csr);
   *x = ((csr & 0x4000) != 0) ? 1 : 0;
   *q = ((csr & 0x8000) != 0) ? 1 : 0;
}

/*---------------------------------------------------------------*/

INLINE void cam24i_q(const int c, const int n, const int a, const int f,
                     DWORD * d, int *x, int *q)
{
   WORD csr;

   cam24i(c, n, a, f, d);
   cami(0, 29, 0, 0, &csr);
   *x = ((csr & 0x4000) != 0) ? 1 : 0;
   *q = ((csr & 0x8000) != 0) ? 1 : 0;
}

/*---------------------------------------------------------------*/

INLINE void cam16i_r(const int c, const int n, const int a, const int f,
                     WORD ** d, const int r)
{
   int i;

   for (i = 0; i < r; i++)
      cam16i(c, n, a, f, (*d)++);
}

/*---------------------------------------------------------------*/

INLINE void cam24i_r(const int c, const int n, const int a, const int f,
                     DWORD ** d, const int r)
{
   int i;

   for (i = 0; i < r; i++)
      cam24i(c, n, a, f, (*d)++);
}

/*---------------------------------------------------------------*/

INLINE void cam16i_rq(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   WORD dtemp;
   WORD csr, i;

   for (i = 0; i < r; i++) {
      cam16i(c, n, a, f, &dtemp);
      cami(0, 29, 0, 0, &csr);
      if ((csr & 0x8000) != 0)
         *((*d)++) = dtemp;
      else
         break;
   }
}

/*---------------------------------------------------------------*/

INLINE void cam24i_rq(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   DWORD i, dtemp;
   WORD csr;

   for (i = 0; i < (DWORD) r; i++) {
      cam24i(c, n, a, f, &dtemp);
      cami(0, 29, 0, 0, &csr);
      if ((csr & 0x8000) != 0)
         *((*d)++) = dtemp;
      else
         break;
   }
}

/*---------------------------------------------------------------*/

INLINE void cam16i_sa(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   int aa;

   for (aa = a; aa < a + r; aa++)
      cam16i(c, n, aa, f, (*d)++);
}

/*--input--------------------------------------------------------*/

INLINE void cam24i_sa(const int c, const int n, const int a, const int f,
                      unsigned long **d, const int r)
{
   int aa;

   for (aa = a; aa < a + r; aa++)
      cam24i(c, n, aa, f, (*d)++);
}

/*---------------------------------------------------------------*/

INLINE void cam16i_sn(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   int nn;

   for (nn = n; nn < n + r; nn++)
      cam16i(c, nn, a, f, (*d)++);
}

/*---------------------------------------------------------------*/

INLINE void cam24i_sn(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   int nn;

   for (nn = n; nn < n + r; nn++)
      cam24i(c, nn, a, f, (*d)++);
}

/*---------------------------------------------------------------*/

INLINE void cami(const int c, const int n, const int a, const int f, WORD * d)
{
   cam16i(c, n, a, f, d);
}

/*****************************************************************\
*                                                                 *
* Output functions                                                *
*                                                                 *
\*****************************************************************/

INLINE void cam16o(const int c, const int n, const int a, const int f, WORD d)
{
   int ext;

   ext = (cbd[c].baseCrate + CBDWL_D16 + (n << 11) + (a << 7) + (f << 2));
   *((unsigned short *) ext) = d;
}

/*---------------------------------------------------------------*/

INLINE void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
   int ext;
   unsigned short dtmp;

   ext = (cbd[c].baseCrate + CBDWL_D24 + (n << 11) + (a << 7) + (f << 2));
   dtmp = (unsigned short) ((d >> 16) & 0xff);
   *((unsigned short *) ext) = dtmp;
   dtmp = (unsigned short) (d & 0xffff);
   ext += 2;
   *((unsigned short *) ext) = dtmp;
}

/*---------------------------------------------------------------*/

INLINE void cam16o_q(const int c, const int n, const int a, const int f,
                     WORD d, int *x, int *q)
{
   WORD csr;

   cam16o(c, n, a, f, d);
   cami(0, 29, 0, 0, &csr);
   *x = ((csr & 0x4000) != 0) ? 1 : 0;
   *q = ((csr & 0x8000) != 0) ? 1 : 0;
}

/*---------------------------------------------------------------*/

INLINE void cam24o_q(const int c, const int n, const int a, const int f,
                     DWORD d, int *x, int *q)
{
   WORD csr;

   cam24o(c, n, a, f, d);
   cami(0, 29, 0, 0, &csr);
   *x = ((csr & 0x4000) != 0) ? 1 : 0;
   *q = ((csr & 0x8000) != 0) ? 1 : 0;
}

/*---------------------------------------------------------------*/

INLINE void camo(const int c, const int n, const int a, const int f, WORD d)
{
   cam16o(c, n, a, f, d);
}

/*****************************************************************\
*                                                                 *
* Command functions                                               *
*                                                                 *
\*****************************************************************/

INLINE int camc_chk(const int c)
{
   int crate_status;

   CAM_BTBCHK(&crate_status);
   if (((crate_status >> c) & 0x1) != 1)
      return -1;
   return 0;
}

/*---------------------------------------------------------------*/

INLINE void camc(const int c, const int n, const int a, const int f)
{
   /* Following the CBD8210 manual */
   WORD dtmp;

   if (f < 16)
      cam16i(c, n, a, f, &dtmp);
   else if (f < 24)
      cam16o(c, n, a, f, 0);
   else
      cam16i(c, n, a, f, &dtmp);
}

/*---------------------------------------------------------------*/

INLINE void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   WORD csr;

   camc(c, n, a, f);
   cami(0, 29, 0, 0, &csr);
   *q = ((csr & 0x8000) != 0) ? 1 : 0;
}

/*---------------------------------------------------------------*/

INLINE void camc_sa(const int c, const int n, const int a, const int f, const int r)
{
   int aa;

   for (aa = a; aa < a + r; aa++)
      camc(c, n, aa, f);
}

/*---------------------------------------------------------------*/

INLINE void camc_sn(const int c, const int n, const int a, const int f, const int r)
{
   int nn;

   for (nn = n; nn < n + r; nn++)
      camc(c, nn, a, f);
}

/*****************************************************************\
*                                                                 *
* General functions                                               *
*                                                                 *
\*****************************************************************/

INLINE int cam_init(void)
{
   int branch = 0, crate, vmecrate, err, am;

   /* Open in A24D16 */
   vh = vme_open(0, MVME_A24D16);
   if (vh >= 0) {
      am = MVME_AMOD_A24_ND;
      vme_ioctl(vh, MVME_IOCTL_AMOD_SET, &am);
      vme_ioctl(vh, MVME_IOCTL_AMOD_GET, &am);

      /* mapping all crates */
      for (crate = 0; crate < 8; crate++) {
         vmecrate = (CBD8210_BASE | branch << 19 | crate << 16);
         vme_mmap(vh, (void **) &(cbd[crate].baseCrate), vmecrate, 0x10000);
         /*
            printf("am 0x%x - baseCrate[%i] = 0x%x <- 0x%x\n", am, crate, 
            cbd[crate].baseCrate, vmecrate);
          */
      }
   } else {
      printf("cam_init: No more device space\n");
      return 0;
   }

   return SUCCESS;
}

/*---------------------------------------------------------------*/

INLINE void cam_exit(void)
{
   int crate;

   for (crate = 0; crate < 8; crate++)
      vme_unmap(vh, &cbd[crate].baseCrate, 0x10000);
   vme_close(vh);
}

/*---------------------------------------------------------------*/

INLINE void cam_inhibit_set(const int c)
{
   camc(c, 30, 9, 26);
}

/*---------------------------------------------------------------*/

INLINE int cam_inhibit_test(const int c)
{
   return 1;
}

/*---------------------------------------------------------------*/

INLINE void cam_inhibit_clear(const int c)
{
   camc(c, 30, 9, 24);
}

/*---------------------------------------------------------------*/

INLINE void cam_crate_clear(const int c)
{
   camc(c, 28, 9, 26);
}

/*---------------------------------------------------------------*/

INLINE void cam_crate_zinit(const int c)
{
   camc(c, 28, 8, 26);
}

/*---------------------------------------------------------------*/

INLINE void cam_lam_enable(const int c, const int n)
{
   camc(c, n, 0, 26);
}

/*---------------------------------------------------------------*/

INLINE void cam_lam_disable(const int c, const int n)
{
   camc(c, n, 0, 24);
}

/*---------------------------------------------------------------*/

INLINE void cam_lam_read(const int c, DWORD * lam)
{
   cam24i(0, 29, 0, 10, lam);
}

/*---------------------------------------------------------------*/

INLINE void cam_lam_clear(const int c, const int n)
{
/* Depend on the hardware LAM implementation
as this cmd should talk to the controller
but can include the LAM source module LAM clear
  camc (c,n,0,10); */
}

/*****************************************************************\
*                                                                 *
* Interrupt functions                                             *
*                                                                 *
\*****************************************************************/

INLINE void cam_interrupt_enable(const int c)
{
   printf("cam_interrupt_enable - Not yet implemented\n");
}

/*---------------------------------------------------------------*/

INLINE void cam_interrupt_disable(const int c)
{
   printf("cam_interrupt_disable - Not yet implemented\n");
}

/*---------------------------------------------------------------*/

INLINE int cam_interrupt_test(const int c)
{
   printf("cam_interrupt_test - Not yet implemented\n");
   return 0;
}

/*---------------------------------------------------------------*/

INLINE void cam_interrupt_attach(const int c, const int n, void (*isr) (void))
{
   printf("cam_interrupt_attach - Not yet implemented\n");
}

/*---------------------------------------------------------------*/

INLINE void cam_interrupt_detach(const int c, const int n)
{
   printf("cam_interrupt_detach - Not yet implemented\n");
}

/*---------------------------------------------------------------*/

INLINE void cam_glint_enable(void)
{
   printf("cam_glint_enable - Not yet implemented\n");
}

/*---------------------------------------------------------------*/

INLINE void cam_glint_disable(void)
{
   printf("cam_glint_disable - Not yet implemented\n");
}

/*---------------------------------------------------------------*/

INLINE void cam_glint_attach(int lam, void (*isr) (void))
{
   printf("cam_glint_attach - Not yet implemented\n");
}

void csr(void)
{
   WORD online;

   CAM_CSRCHK(&online);
   printf("CSR Camac  status 0x%4.4x\n", online);
}

void car(void)
{
   WORD online;

   CAM_CARCHK(&online);
   printf("CAR Online status 0x%4.4x\n", online);
}

void btb(void)
{
   WORD online;

   CAM_BTBCHK(&online);
   printf("BTB Online status 0x%4.4x\n", online);
}

void camop(void)
{
   csr();
   car();
   btb();
}
