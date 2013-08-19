/********************************************************************

  Name:         wecc32.c
  Created by:   Pierre-Andre Amaudruz

  Contents:     Device driver for the Wiener CC32 PCI-CAMAC controller
                following the MIDAS CAMAC Standard
		include the 2000 ARW Elektronik source code
		pcicc32.h and libcc32.h for simplicity.

		Requires module pcicc32.o to be loaded prior the use of
		this driver. For info contact midas@triumf.ca
		
  $Log: wecc32.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.5  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.4  2002/02/19 21:56:31  pierre
  comments+verified for Kernel 2.4

  Revision 1.3  2001/10/16 20:47:51  pierre
  Update cam_interrupt_() args

  Revision 1.2  2000/09/07 17:17:33  pierre
  -Add fe_name

  Revision 1.1  2000/04/18 19:07:36  pierre
  - W-IE-NE-R PCI/CAMAC interface CC32 uses pcicc32.o module, doesn't use dio.


/*--PAA--- ---------------------------------------------------------------
   cc32lib.c -- a simple access library for the PCICC32 PCI to CAMAC
   Interface from ARW Elektronik

   (c) 2000 ARW Elektronik

   this source code is published under GPL (Open Source). You can use,
   redistrubute and modify it unless this header is not modified or
   deleted. No warranty is given that this software will work like
   expected.
   This product is not authorized for use as critical component in
   life support systems wihout the express written approval of ARW
   Elektronik Germany.
  
   Please announce changes and hints to ARW Elektronik

   first steps AR   25.02.2000
---------------------------------------------------------------------
\********************************************************************/

#include <stdio.h>
#include "midas.h"
#include "mcstd.h"
#include <sys/mman.h>
#include <errno.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>

/*--PAA--- inclusion of pcicc32.h for simplicity----------------------*/
#ifndef __PCICC32_H__
#define __PCICC32_H__
/*
   pcicc32.h -- the common header for driver and applications for the
   PCICC32 PCI to CAMAC Interface from ARW Elektronik

   (c) 2000 ARW Elektronik

   this source code is published under GPL (Open Source). You can use,
   redistrubute and modify it unless this header is not modified or deleted.
   No warranty is given that this software will work like expected.
   This product is not authorized for use as critical component in life
   support systems wihout the express written approval of ARW
   Elektronik Germany.
  
   Please announce changes and hints to ARW Elektronik

   first steps                                       AR   25.02.2000
*/

#define PCICC32_MAGIC ' '
typedef struct {
   char bConnected;             /* the interface seems to be connected */
   char bFail;                  /* a timeout failure is registered     */
   char bIrq;                   /* a LAM wants to generate a IRQ       */
   char bReserve;               /* not yet used                        */
} PCICC32_STATUS;

#define PCICC32_IOSTATE _IOR(PCICC32_MAGIC, 1, sizeof(PCICC32_STATUS))  /* get the status of PCICC32 */
#define PCICC32_IOCNTRL _IO(PCICC32_MAGIC, 2)   /* reset a timeout failure, no parameters */

#endif                          /* __PCICC32_H__ */

#define pwCC32_ADR(adr, N, A, F) (WORD *)((N << 10) + (A << 6) + ((F & 0xF) << 2) + adr)
#define plCC32_ADR(adr, N, A, F) (DWORD *)((N << 10) + (A << 6) + ((F & 0xF) << 2) + adr)
#define WINDOW_SIZE 32768

typedef struct {
   FILE *f;                     /* the handle of this device */
   int fileNo;                  /* equals fileno(f)          */
   char *base;                  /* base of range, got with mmap */
} CC32_DEVICE;
/*--PAA--- END of inclusion of pcicc32.h for simplicity----------------------*/

typedef void *CC32_HANDLE;      /* type of the device handle */

#define MAX_PCI_ADD 4

CC32_HANDLE handle[MAX_PCI_ADD] = { NULL, NULL, NULL, NULL };
/*--PAA--- inclusion of libcc32.h for simplicity-------------------*/
int cc32_open(char *cszPath, CC32_HANDLE * handle)
{
   CC32_DEVICE *dev;

   *handle = (CC32_HANDLE) 0;

   dev = (CC32_DEVICE *) malloc(sizeof(CC32_DEVICE));
   if (!dev)
      return errno;

   dev->fileNo = 0;
   dev->base = (char *) 0;

   if (!(dev->f = fopen(cszPath, "r"))) {
      int error = errno;

      free(dev);
      return error;
   }

   dev->fileNo = fileno(dev->f);

   dev->base =
       (char *) mmap(0, WINDOW_SIZE, PROT_READ, MAP_FILE | MAP_PRIVATE, dev->fileNo, 0);
   if (dev->base == (char *) -1) {
      int error = errno;

      fclose(dev->f);
      free(dev);
      return error;
   }

   *handle = (CC32_HANDLE) dev;

   return 0;
}

int cc32_close(CC32_HANDLE handle)
{
   CC32_DEVICE *dev = (CC32_DEVICE *) handle;
   int error = 0;

   if (dev) {
      munmap(dev->base, WINDOW_SIZE);

      if (dev->f)
         fclose(dev->f);
      else
         error = -EINVAL;

      free(dev);
   } else
      error = -EINVAL;

   return error;
}

/*--PAA--- END of inclusion of libcc32.h for simplicity-------------------*/


/*------------------------------------------------------------------*/
INLINE void cami(const int c, const int n, const int a, const int f, WORD * d)
{
   cam16i(c, n, a, f, d);
}

/*------------------------------------------------------------------*/
INLINE void cam16i(const int c, const int n, const int a, const int f, WORD * d)
{
   if (handle[c])
      *d = *pwCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f);
}

/*------------------------------------------------------------------*/
INLINE void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   if (handle[c])
      *d = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f);
   *d &= 0xFFFFFF;
}

/*------------------------------------------------------------------*/
INLINE void cam16i_q(const int c, const int n, const int a, const int f,
                     WORD * d, int *x, int *q)
{
   DWORD erg;
   if (handle[c]) {
      erg = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f);
      *q = (erg & 0x80000000) ? 1 : 0;
      *x = (erg & 0x40000000) ? 1 : 0;
      *d = (WORD) (erg & 0x0000ffff);
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24i_q(const int c, const int n, const int a, const int f,
                     DWORD * d, int *x, int *q)
{
   if (handle[c]) {
      *d = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f);
      *q = (*d & 0x80000000) ? 1 : 0;
      *x = (*d & 0x40000000) ? 1 : 0;
      *d &= 0x00ffffff;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam16i_r(const int c, const int n, const int a, const int f,
                     WORD ** d, const int r)
{
   int i;
   if (handle[c])
      for (i = 0; i < r; i++)
         *(*d)++ = *pwCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f);
}

/*------------------------------------------------------------------*/
INLINE void cam24i_r(const int c, const int n, const int a, const int f,
                     DWORD ** d, const int r)
{
   INT i;
   if (handle[c])
      for (i = 0; i < r; i++)
         *(*d)++ = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f);
}

/*------------------------------------------------------------------*/
INLINE void cam16i_rq(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   WORD erg;
   INT i, q;

   if (handle[c])
      for (i = 0; i < r; i++) {
         erg = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f);
         q = (erg & 0x80000000) ? 1 : 0;
         if (q)
            *(*d)++ = (WORD) (erg & 0x0000ffff);
         else
            break;
      }
}

/*------------------------------------------------------------------*/
INLINE void cam24i_rq(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   DWORD erg;
   INT i, q;

   if (handle[c])
      for (i = 0; i < r; i++) {
         erg = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f);
         q = (erg & 0x80000000) ? 1 : 0;
         if (q)
            *(*d)++ = (erg & 0x00ffffff);
         else
            break;
      }
}

/*------------------------------------------------------------------*/
INLINE void cam16i_sa(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   INT i;

   if (handle[c])
      for (i = 0; i < r; i++)
         *(*d)++ = *pwCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a + i, f);
}

/*------------------------------------------------------------------*/
INLINE void cam24i_sa(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   INT i;

   if (handle[c])
      for (i = 0; i < r; i++)
         *(*d)++ = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a + i, f);
}

/*------------------------------------------------------------------*/
INLINE void cam16i_sn(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   INT i;

   for (i = 0; i < r; i++)
      cam16i(c, n + i, a, f, (*d)++);
}

/*------------------------------------------------------------------*/
INLINE void cam24i_sn(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   INT i;

   for (i = 0; i < r; i++)
      cam24i(c, n + i, a, f, (*d)++);
}

/*------------------------------------------------------------------*/
INLINE void camo(const int c, const int n, const int a, const int f, WORD d)
{
   cam16o(c, n, a, f, d);
}

/*------------------------------------------------------------------*/
INLINE void cam16o(const int c, const int n, const int a, const int f, WORD d)
{
   if (handle[c])
      *pwCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f) = d;
}

/*------------------------------------------------------------------*/
INLINE void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
   if (handle[c])
      *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f) = d;
}

/*------------------------------------------------------------------*/
INLINE void cam16o_q(const int c, const int n, const int a, const int f,
                     WORD d, int *x, int *q)
{
   if (handle[c]) {
      *pwCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f) = d;
      *x = 1;
      *q = 1;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24o_q(const int c, const int n, const int a, const int f,
                     DWORD d, int *x, int *q)
{
   if (handle[c]) {
      *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f) = d;
      *x = 1;
      *q = 1;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam16o_r(const int c, const int n, const int a, const int f,
                     WORD * d, const int r)
{
   INT i;
   if (handle[c])
      for (i = 0; i < r; i++)
         *pwCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f) = d[i];
}

/*------------------------------------------------------------------*/
INLINE void cam24o_r(const int c, const int n, const int a, const int f,
                     DWORD * d, const int r)
{
   INT i;
   if (handle[c])
      for (i = 0; i < r; i++)
         *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f) = d[i];
}

/*------------------------------------------------------------------*/
INLINE int camc_chk(const int c)
{
   printf("not implemented\n");
   return 0;
}

/*------------------------------------------------------------------*/
INLINE void camc(const int c, const int n, const int a, const int f)
{
   DWORD temp;
   if (handle[c])
      temp = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f);
}

/*------------------------------------------------------------------*/
INLINE void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   DWORD erg;
   if (handle[c])
      erg = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, n, a, f);
   *q = (erg & 0x80000000) ? 1 : 0;
}

/*------------------------------------------------------------------*/
INLINE void camc_sa(const int c, const int n, const int a, const int f, const int r)
{
   INT i;

   for (i = 0; i < r; i++)
      camc(c, n, a + i, f);
}

/*------------------------------------------------------------------*/
INLINE void camc_sn(const int c, const int n, const int a, const int f, const int r)
{
   INT i;

   for (i = 0; i < r; i++)
      camc(c, n + i, a, f);
}

/*------------------------------------------------------------------*/
INLINE int cam_init(void)
{
   unsigned char status;
   unsigned int adr, error;
   int i;
   char fname[16];

#ifdef _MSC_VER
   printf(" Board not supported under this OS\n")
#endif                          // _MSC_VER
#ifdef OS_LINUX
       for (i = 0; i < MAX_PCI_ADD; i++) {
      if (handle[i]) {
         printf("Crate[%d] already in use\n", i);
         continue;
      }
      sprintf(fname, "/dev/cc32_%1d", i);
      if ((error = cc32_open(fname, &handle[i]))) {
         cm_msg(MERROR, "cam_init", "wecc32: Crate[%d] not found (%s)", i, fname);
         handle[i] = NULL;
      } else
         printf("cam_init: wecc32: open device %s handle:0x%x \n", fname, handle[i]);
   }
#endif
   for (i = 0; i < MAX_PCI_ADD; i++)
      if (handle[i])
         return SUCCESS;
   return FE_ERR_HW;
}

/*------------------------------------------------------------------*/
INLINE void cam_exit(void)
{
   INT i;

   for (i = 0; i < MAX_PCI_ADD; i++)
      cc32_close(handle[i]);
}

/*------------------------------------------------------------------*/
INLINE void cam_inhibit_set(const int c)
{
   if (handle[c])
      *pwCC32_ADR(((CC32_DEVICE *) handle[c])->base, 27, 0, 16) = 0;
}

/*------------------------------------------------------------------*/
INLINE void cam_inhibit_clear(const int c)
{
   if (handle[c])
      *pwCC32_ADR(((CC32_DEVICE *) handle[c])->base, 27, 1, 16) = 0;
}

/*------------------------------------------------------------------*/
INLINE void cam_crate_clear(const int c)
{
   if (handle[c])
      *pwCC32_ADR(((CC32_DEVICE *) handle[c])->base, 0, 0, 16) = 0;
}

/*------------------------------------------------------------------*/
INLINE void cam_crate_zinit(const int c)
{
   if (handle[c])
      *pwCC32_ADR(((CC32_DEVICE *) handle[c])->base, 0, 1, 16) = 0;
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_enable(const int c, const int n)
{
   DWORD mask;
   if (handle[c]) {
//    printf("c%d n%d ", c, n);
      mask = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, 28, 1, 0);
      mask &= 0xffffff;
//    printf("mask 0x%x ", mask);
      mask |= (1 << (n - 1));
//    printf("Enable mask:0x%x ",mask);
      *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, 28, 1, 16) = mask;
      mask = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, 28, 1, 0);
//    printf("Final Enable mask:0x%x\n", mask);
   }
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_disable(const int c, const int n)
{
   DWORD mask;
   if (handle[c]) {
      mask = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, 28, 1, 0);
      mask &= ~(1 << (n - 1));
      *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, 28, 1, 16) = mask;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_read(const int c, DWORD * lam)
{
   if (handle[c]) {
      *lam = *plCC32_ADR(((CC32_DEVICE *) handle[c])->base, 28, 2, 0);
      *lam &= 0xffffff;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_clear(const int c, const int n)
{
   if (handle[c]) {
      /* station F10 */
//      *pwCC32_ADR(((CC32_DEVICE *)handle[c])->base, n, 0, 10) = 0;
      /* CC32 */
      *pwCC32_ADR(((CC32_DEVICE *) handle[c])->base, 28, 0, 16) = 0;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam_interrupt_enable(const int c)
{
   printf("not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE void cam_interrupt_disable(const int c)
{
   printf("not implemented\n");
}

/*------------------------------------------------------------------*/
static void (*old_handler) (void) = NULL;

INLINE void cam_interrupt_attach(const int c, const int n, void (*isr) (void))
{
   printf("not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE void cam_interrupt_detach(const int c, const int n)
{
   printf("not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE int cam_init_rpc(char *host_name, char *exp_name, char *fe_name, char *client_name,
                        char *rpc_server)
{
   return 1;
}

/*------------------------------------------------------------------*/
