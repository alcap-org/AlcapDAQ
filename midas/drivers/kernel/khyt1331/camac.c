/********************************************************************

  Name:         libcamac.c
  Created by:   Stefan Ritt

  Contents:     Linux CAMAC library wrapping calls around /dev/camac
                driver

  $Log: camac.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  2001/10/03 14:45:11  midas
  Separated IOCTL functions code into header file

  Revision 1.1  2001/08/21 14:28:34  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "mcstd.h"

/*---- definitions -------------------------------------------------*/

#include "khyt1331.h"

/*---- global variables  -------------------------------------------*/

int hcamac;                     /* handle for device driver             */

/*---- general CNAF routine ----------------------------------------*/

INLINE void cnaf8(const int c, const int n, const int a, const int f,
                  BYTE * d, int *x, int *q)
{
   CNAF_BUF cnaf_buf;

   cnaf_buf.c = c;
   cnaf_buf.n = n;
   cnaf_buf.a = a;
   cnaf_buf.f = f;
   cnaf_buf.d = *d;

   ioctl(hcamac, OP_CNAF8, &cnaf_buf);

   *d = (BYTE) cnaf_buf.d;
   if (x) {
      *x = cnaf_buf.x;
      *q = cnaf_buf.q;
   }
}

INLINE void cnaf8_r(const int c, const int n, const int a, const int f,
                    BYTE ** d, int r, int m)
{
   CNAF_BUF cnaf_buf;

   cnaf_buf.c = c;
   cnaf_buf.n = n;
   cnaf_buf.a = a;
   cnaf_buf.f = f;
   cnaf_buf.d = (unsigned long) *d;
   cnaf_buf.r = r;
   cnaf_buf.m = m;

   ioctl(hcamac, OP_CNAF8R, &cnaf_buf);

   /* increment pointer */
   *d += r;
}

/*------------------------------------------------------------------*/

INLINE void cnaf16(const int c, const int n, const int a, const int f,
                   WORD * d, int *x, int *q)
{
   CNAF_BUF cnaf_buf;

   cnaf_buf.c = c;
   cnaf_buf.n = n;
   cnaf_buf.a = a;
   cnaf_buf.f = f;
   cnaf_buf.d = *d;

   ioctl(hcamac, OP_CNAF16, &cnaf_buf);

   *d = (WORD) cnaf_buf.d;
   if (x) {
      *x = cnaf_buf.x;
      *q = cnaf_buf.q;
   }
}

INLINE void cnaf16_r(const int c, const int n, const int a, const int f,
                     WORD ** d, int r, int m)
{
   CNAF_BUF cnaf_buf;

   cnaf_buf.c = c;
   cnaf_buf.n = n;
   cnaf_buf.a = a;
   cnaf_buf.f = f;
   cnaf_buf.d = (unsigned long) *d;
   cnaf_buf.r = r;
   cnaf_buf.m = m;

   ioctl(hcamac, OP_CNAF16R, &cnaf_buf);

   /* increment pointer */
   *d += r;
}

/*------------------------------------------------------------------*/

INLINE void cnaf24(const int c, const int n, const int a, const int f,
                   DWORD * d, int *x, int *q)
{
   CNAF_BUF cnaf_buf;

   cnaf_buf.c = c;
   cnaf_buf.n = n;
   cnaf_buf.a = a;
   cnaf_buf.f = f;
   cnaf_buf.d = *d;

   ioctl(hcamac, OP_CNAF24, &cnaf_buf);

   *d = cnaf_buf.d;
   if (x) {
      *x = cnaf_buf.x;
      *q = cnaf_buf.q;
   }
}

INLINE void cnaf24_r(const int c, const int n, const int a, const int f,
                     DWORD ** d, int r, int m)
{
   CNAF_BUF cnaf_buf;

   cnaf_buf.c = c;
   cnaf_buf.n = n;
   cnaf_buf.a = a;
   cnaf_buf.f = f;
   cnaf_buf.d = (unsigned long) *d;
   cnaf_buf.r = r;
   cnaf_buf.m = m;

   ioctl(hcamac, OP_CNAF24R, &cnaf_buf);

   /* increment pointer */
   *d += r;
}

/*------------------------------------------------------------------*/

INLINE void cam8i(const int c, const int n, const int a, const int f, BYTE * d)
{
   cnaf8(c, n, a, f, d, NULL, NULL);
}

/*------------------------------------------------------------------*/

INLINE void cami(const int c, const int n, const int a, const int f, WORD * d)
{
   cnaf16(c, n, a, f, d, NULL, NULL);
}

/*------------------------------------------------------------------*/

INLINE void cam16i(const int c, const int n, const int a, const int f, WORD * d)
{
   cnaf16(c, n, a, f, d, NULL, NULL);
}

/*------------------------------------------------------------------*/

INLINE void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   cnaf24(c, n, a, f, d, NULL, NULL);
}

/*------------------------------------------------------------------*/

INLINE void cam8i_q(const int c, const int n, const int a, const int f,
                    BYTE * d, int *x, int *q)
{
   cnaf8(c, n, a, f, d, x, q);
}

/*------------------------------------------------------------------*/

INLINE void cam16i_q(const int c, const int n, const int a, const int f,
                     WORD * d, int *x, int *q)
{
   cnaf16(c, n, a, f, d, x, q);
}

/*------------------------------------------------------------------*/

INLINE void cam24i_q(const int c, const int n, const int a, const int f,
                     DWORD * d, int *x, int *q)
{
   cnaf24(c, n, a, f, d, x, q);
}

/*------------------------------------------------------------------*/

INLINE void cam16i_r(const int c, const int n, const int a, const int f,
                     WORD ** d, const int r)
{
   cnaf16_r(c, n, a, f, d, r, RM_COUNT);
}

/*------------------------------------------------------------------*/

INLINE void cam24i_r(const int c, const int n, const int a, const int f,
                     DWORD ** d, const int r)
{
   cnaf24_r(c, n, a, f, d, r, RM_COUNT);
}

/*------------------------------------------------------------------*/

INLINE void cam16i_rq(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   cnaf16_r(c, n, a, f, d, r, RM_QMODE);
}

/*------------------------------------------------------------------*/

INLINE void cam24i_rq(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   cnaf24_r(c, n, a, f, d, r, RM_QMODE);
}

/*------------------------------------------------------------------*/

INLINE void cam16i_sa(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   cnaf16_r(c, n, a, f, d, r, RM_ASCAN);
}

/*------------------------------------------------------------------*/

INLINE void cam24i_sa(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   cnaf24_r(c, n, a, f, d, r, RM_ASCAN);
}

/*------------------------------------------------------------------*/

INLINE void cam16i_sn(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   cnaf16_r(c, n, a, f, d, r, RM_NSCAN);
}

/*------------------------------------------------------------------*/

INLINE void cam24i_sn(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   cnaf24_r(c, n, a, f, d, r, RM_NSCAN);
}

/*------------------------------------------------------------------*/

INLINE void cam8o(const int c, const int n, const int a, const int f, BYTE d)
{
   cnaf8(c, n, a, f, &d, NULL, NULL);
}

/*------------------------------------------------------------------*/

INLINE void camo(const int c, const int n, const int a, const int f, WORD d)
{
   cnaf16(c, n, a, f, &d, NULL, NULL);
}

/*------------------------------------------------------------------*/

INLINE void cam16o(const int c, const int n, const int a, const int f, WORD d)
{
   camo(c, n, a, f, d);
}

/*------------------------------------------------------------------*/

INLINE void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
   cnaf24(c, n, a, f, &d, NULL, NULL);
}

/*------------------------------------------------------------------*/

INLINE void cam16o_q(const int c, const int n, const int a, const int f,
                     WORD d, int *x, int *q)
{
   cnaf16(c, n, a, f, &d, x, q);
}

/*------------------------------------------------------------------*/

INLINE void cam24o_q(const int c, const int n, const int a, const int f,
                     DWORD d, int *x, int *q)
{
   cnaf24(c, n, a, f, &d, x, q);
}

/*------------------------------------------------------------------*/

INLINE void cam8o_r(const int c, const int n, const int a, const int f,
                    BYTE * d, const int r)
{
   cnaf8_r(c, n, a, f, &d, r, RM_COUNT);
}

/*------------------------------------------------------------------*/

INLINE void cam16o_r(const int c, const int n, const int a, const int f,
                     WORD * d, const int r)
{
   cnaf16_r(c, n, a, f, &d, r, RM_COUNT);
}

/*------------------------------------------------------------------*/

INLINE void cam24o_r(const int c, const int n, const int a, const int f,
                     DWORD * d, const int r)
{
   cnaf24_r(c, n, a, f, &d, r, RM_COUNT);
}

/*------------------------------------------------------------------*/

INLINE int camc_chk(const int c)
{
   return ioctl(hcamac, OP_CRATE_CHECK, c);
}

/*------------------------------------------------------------------*/

INLINE void camc(const int c, const int n, const int a, const int f)
{
   BYTE d = 0;

   cnaf8(c, n, a, f, &d, NULL, NULL);
}

/*------------------------------------------------------------------*/

INLINE void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   BYTE d = 0;

   cnaf8(c, n, a, f, &d, NULL, q);
}

/*------------------------------------------------------------------*/

INLINE void camc_sa(const int c, const int n, const int a, const int f, const int r)
{
   cnaf8_r(c, n, a, f, NULL, r, RM_ASCAN);
}

/*------------------------------------------------------------------*/

INLINE void camc_sn(const int c, const int n, const int a, const int f, const int r)
{
   cnaf8_r(c, n, a, f, NULL, r, RM_NSCAN);
}

/*------------------------------------------------------------------*/

INLINE void cam_inhibit_set(const int c)
{
   ioctl(hcamac, OP_INHIBIT_SET, c);
}

/*------------------------------------------------------------------*/

INLINE void cam_inhibit_clear(const int c)
{
   ioctl(hcamac, OP_INHIBIT_CLEAR, c);
}

/*------------------------------------------------------------------*/

INLINE int cam_inhibit_test(const int c)
{
   return ioctl(hcamac, OP_INHIBIT_TEST, c);
}

/*------------------------------------------------------------------*/

INLINE void cam_crate_zinit(const int c)
{
   ioctl(hcamac, OP_CRATE_ZINIT, c);
}

/*------------------------------------------------------------------*/

INLINE void cam_crate_clear(const int c)
{
   ioctl(hcamac, OP_CRATE_CLEAR, c);
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_enable(const int c, const int n)
{
   ioctl(hcamac, OP_LAM_ENABLE, (c << 16) | n);
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_disable(const int c, const int n)
{
   ioctl(hcamac, OP_LAM_DISABLE, (c << 16) | n);
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_read(const int c, DWORD * lam)
{
   *lam = ioctl(hcamac, OP_LAM_READ, c);
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_clear(const int c, const int n)
{
   ioctl(hcamac, OP_LAM_CLEAR, (c << 16) | n);
}

/*------------------------------------------------------------------*/

INLINE int cam_lam_wait(int *c, DWORD * lam, int millisec)
{
   unsigned int d;
   fd_set readfds;
   struct timeval timeout;

   FD_ZERO(&readfds);
   FD_SET(hcamac, &readfds);

   timeout.tv_sec = millisec / 1000;
   timeout.tv_usec = (millisec % 1000) * 1000;

   select(FD_SETSIZE, (void *) &readfds, NULL, NULL, (void *) &timeout);

   if (FD_ISSET(hcamac, &readfds)) {
      read(hcamac, &d, 4);
      *lam = (1 << (d & 0xFFFF));
      *c = (d >> 16);

      return 1;
   }

   return 0;
}

/*------------------------------------------------------------------*/

INLINE void cam_interrupt_enable(const int c)
{
   ioctl(hcamac, OP_INTERRUPT_ENABLE, c);
}

/*------------------------------------------------------------------*/

INLINE void cam_interrupt_disable(const int c)
{
   ioctl(hcamac, OP_INTERRUPT_DISABLE, c);
}

/*------------------------------------------------------------------*/

INLINE int cam_interrupt_test(const int c)
{
   return ioctl(hcamac, OP_INTERRUPT_TEST, c);
}

/*------------------------------------------------------------------*/

#define LAM_TABLE_SIZE 10

struct {
   int c;
   int n;
   void (*isr) (void);
} lam_table[LAM_TABLE_SIZE];

static int n_lam_registered = 0;

static void lam_dispatch(int s)
{
   int i, n, c;
   unsigned int d;

   if (n_lam_registered == 0)
      return;                   /* should never happen */

   /* if only one service routine registered, call immediately */
   if (n_lam_registered == 1) {
      for (i = 0; i < LAM_TABLE_SIZE; i++)
         if (lam_table[i].n != 0) {
            lam_table[i].isr();
            return;
         }
   }

   /* get C and N from driver */
   read(hcamac, &d, 4);
   n = (d & 0xFFFF);
   c = (d >> 16);

   for (i = 0; i < LAM_TABLE_SIZE; i++)
      if (lam_table[i].c == c && lam_table[i].n == n && lam_table[i].isr) {
         lam_table[i].isr();
      }
}

/*------------------------------------------------------------------*/

INLINE void cam_interrupt_attach(int c, int n, void (*isr) (void))
{
   int i;

   /* find free entry in lam_table */
   for (i = 0; i < LAM_TABLE_SIZE; i++)
      if (lam_table[i].isr == NULL)
         break;

   if (i == LAM_TABLE_SIZE) {
      printf("cam_interrupt_attach: too many service routines registered\n");
      return;
   }

   lam_table[i].isr = isr;
   lam_table[i].c = c;
   lam_table[i].n = n;

   n_lam_registered++;

   /* register lam dispatcher to receive asynchronous signal SIGIO on
      first call of cam_interrupt_attach */
   if (n_lam_registered == 1) {
      signal(SIGIO, lam_dispatch);
      fcntl(hcamac, F_SETOWN, getpid());
      fcntl(hcamac, F_SETFL, fcntl(hcamac, F_GETFL) | FASYNC);
   }
}

/*------------------------------------------------------------------*/

INLINE void cam_interrupt_detach(int c, int n)
{
   int i;

   /* find entry in lam_table */
   for (i = 0; i < LAM_TABLE_SIZE; i++)
      if (lam_table[i].c == c && lam_table[i].n == n) {
         lam_table[i].c = 0;
         lam_table[i].n = 0;
         lam_table[i].isr = NULL;
         n_lam_registered--;
      }

   if (n_lam_registered == 0) {
      /* remove signal handler */
      signal(SIGIO, SIG_DFL);
      fcntl(hcamac, F_SETFL, fcntl(hcamac, F_GETFL) & ~FASYNC);
   }
}

/*------------------------------------------------------------------*/

INLINE int cam_init_rpc(char *host_name, char *exp_name, char *fe_name,
                        char *client_name, char *rpc_server)
{
   /* dummy routine for compatibility */
   return 1;
}

/*------------------------------------------------------------------*/

INLINE int cam_init(void)
{
   hcamac = open("/dev/camac", 0);
   if (hcamac < 0) {
      perror("cam_init");
      return -1;
   }

   return SUCCESS;
}

/*------------------------------------------------------------------*/

INLINE void cam_exit(void)
{
   close(hcamac);
}

/*------------------------------------------------------------------*/
