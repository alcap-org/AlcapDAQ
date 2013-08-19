/********************************************************************

  Name:         dsp004.c
  Created by:   Pierre-André Amaudruz

  Contents:     Device driver for DSP6002 and PC004 8bit CAMAC controller
                following the MIDAS CAMAC Standard for DirectIO

  $Log: dsp004.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.10  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.9  2002/05/16 22:20:54  pierre
  add cam_inhibit_test

  Revision 1.8  2002/05/08 22:19:27  pierre
  remove warnings

  Revision 1.7  2001/11/22 18:26:25  pierre
  fix cam24o_q upper byte

  Revision 1.6  2001/10/16 20:47:40  pierre
  Update cam_interrupt_() args

  Revision 1.5  2001/04/20 21:58:29  pierre
  - decrement *d if no Q in _rq calls

  Revision 1.4  2000/09/26 07:10:50  midas
  Added DO_IOPERM for debugging as root

  Revision 1.3  2000/09/07 17:15:47  pierre
  -Add fe_name

  Revision 1.2  2000/07/11 16:39:38  pierre
  - Fix bug for i_sa, i_r functions

  Revision 1.1  1999/12/20 10:18:11  midas
  Reorganized driver directory structure

  Revision 1.5  1999/07/22 01:24:27  pierre
  - Zero 4th bytes on R24 calls

  Revision 1.4  1999/02/22 19:07:10  pierre
  - Remove came_xxx

  Revision 1.3  1999/01/18 17:22:42  pierre
  - fix cam24i_r for DH access

  Revision 1.2  1998/10/12 12:18:56  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include "midas.h"
#include "mcstd.h"

/*------------------------------------------------------------------*/

/*
  Base address of PC card. Must match jumper setting:
  ===================================================
*/

#define CAMAC_BASE 0x280
#define WH   CAMAC_BASE + 0
#define WM   CAMAC_BASE + 1
#define WL   CAMAC_BASE + 2
#define ADD  CAMAC_BASE + 3
#define FUN  CAMAC_BASE + 4
#define STA  CAMAC_BASE + 5
#define ZCI  CAMAC_BASE + 6
#define CACY CAMAC_BASE + 7
#define LXQ  CAMAC_BASE + 8
#define RH   CAMAC_BASE + 9
#define RM   CAMAC_BASE + 10
#define RL   CAMAC_BASE + 11
#define OPST CAMAC_BASE + 12
#define CDMA CAMAC_BASE + 15

/*
  Interrupt request selection. Must match jumper setting:
  =======================================================

Switch IRQ2-7
*/

/*------------------------------------------------------------------*/
#if defined( __MSDOS__ )
#include <dos.h>
#define OUTP(_p, _d) outportb(_p, _d)
#define OUTPW(_p, _d) outport(_p, _d)
#define INP(_p) inportb(_p)
#define INPW(_p) inport(_p)
#elif defined( _MSC_VER )
#include <windows.h>
#include <conio.h>
#define OUTP(_p, _d) _outp((unsigned short) (_p), (int) (_d))
#define OUTPW(_p, _d) _outpw((unsigned short) (_p), (unsigned short) (_d))
#define INP(_p) _inp((unsigned short) (_p))
#define INPW(_p) _inpw((unsigned short) (_p))
#elif defined(OS_LINUX)
#include <asm/io.h>
#include <unistd.h>
#define OUTP(_p, _d) outb(_d, _p)
#define OUTPW(_p, _d) outw(_d, _p)
#define INP(_p) inb(_p)
#define INPW(_p) inw(_p)
#endif
/*------------------------------------------------------------------*/
INLINE void cam8i(const int c, const int n, const int a, const int f, unsigned char *d)
{
}

/*------------------------------------------------------------------*/
INLINE void cami(const int c, const int n, const int a, const int f, WORD * d)
{
   WORD loop = 1000;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   OUTP(CACY, 1);
   while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
      loop--;
   if (loop == 0)
      printf("cami: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
   *((char *) d) = (unsigned char) INP(RL);
   *((char *) d + 1) = (unsigned char) INP(RM);
}

/*------------------------------------------------------------------*/
INLINE void cam16i(const int c, const int n, const int a, const int f, WORD * d)
{
   cami(c, n, a, f, d);
}

/*------------------------------------------------------------------*/
INLINE void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   WORD loop = 1000;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   OUTP(CACY, 1);
   while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
      loop--;
   if (loop == 0)
      printf("cami: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
   *((char *) d) = (unsigned char) INP(RL);
   *((char *) d + 1) = (unsigned char) INP(RM);
   *((char *) d + 2) = (unsigned char) INP(RH);
   *((char *) d + 3) = 0;
}

/*------------------------------------------------------------------*/
INLINE void cam8i_q(const int c, const int n, const int a, const int f,
                    unsigned char *d, int *x, int *q)
{
}

/*------------------------------------------------------------------*/
INLINE void cam16i_q(const int c, const int n, const int a, const int f,
                     WORD * d, int *x, int *q)
{
   WORD loop = 1000;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   OUTP(CACY, 1);
   while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
      loop--;
   if (loop == 0)
      printf("cami: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
   *((char *) d) = (unsigned char) INP(RL);
   *((char *) d + 1) = (unsigned char) INP(RM);
   *q = INP(LXQ);
   *x = *q >> 1 & 1;
   *q &= 1;
}

/*------------------------------------------------------------------*/
INLINE void cam24i_q(const int c, const int n, const int a, const int f,
                     DWORD * d, int *x, int *q)
{
   WORD loop = 1000;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   OUTP(CACY, 1);
   while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
      loop--;
   if (loop == 0)
      printf("cami: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
   *((char *) d) = (unsigned char) INP(RL);
   *((char *) d + 1) = (unsigned char) INP(RM);
   *((char *) d + 2) = (unsigned char) INP(RH);
   *((char *) d + 3) = 0;
   *q = INP(LXQ);
   *x = *q >> 1 & 1;
   *q &= 1;
}

/*------------------------------------------------------------------*/
INLINE void cam16i_r(const int c, const int n, const int a, const int f,
                     WORD ** d, const int r)
{
   WORD i, loop;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   for (i = 0; i < r; i++) {
      OUTP(CACY, 1);
      loop = 1000;
      while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
         loop--;
      if (loop == 0) {
         printf("cam16i_r: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
         return;
      }
      *((char *) (*d)) = (unsigned char) INP(RL);
      *((char *) (*d) + 1) = (unsigned char) INP(RM);
      (*d)++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24i_r(const int c, const int n, const int a, const int f,
                     DWORD ** d, const int r)
{
   WORD i, loop;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   for (i = 0; i < r; i++) {
      OUTP(CACY, 1);
      loop = 1000;
      while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
         loop--;
      if (loop == 0) {
         printf("cam24i_r: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
         return;
      }
      *((char *) (*d)) = (unsigned char) INP(RL);
      *((char *) (*d) + 1) = (unsigned char) INP(RM);
      *((char *) (*d) + 2) = (unsigned char) INP(RH);
      *((char *) (*d) + 3) = 0;
      (*d)++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam16i_rq(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   int i, x, q;

   for (i = 0; i < r; i++) {
      cam16i_q(c, n, a, f, (*d)++, &x, &q);
      if (!q) {
         (*d)--;
         break;
      }
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24i_rq(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   int i, x, q;

   for (i = 0; i < r; i++) {
      cam24i_q(c, n, a, f, (*d)++, &x, &q);
      if (!q)
         break;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam16i_sa(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   int i, aa, loop;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(FUN, f);
   aa = a;
   for (i = 0; i < r; i++) {
      OUTP(ADD, aa);
      OUTP(CACY, 1);
      loop = 10;
      while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
         loop--;
      if (loop == 0) {
         printf("cam16i_sa: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
         return;
      }
      *((char *) (*d)) = (unsigned char) INP(RL);
      *((char *) (*d) + 1) = (unsigned char) INP(RM);
      (*d)++;
      aa++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24i_sa(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   int i, aa, loop;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(FUN, f);
   aa = a;
   for (i = 0; i < r; i++) {
      OUTP(ADD, aa);
      OUTP(CACY, 1);
      loop = 1000;
      while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
         loop--;
      if (loop == 0) {
         printf("cam24i_sa: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
         return;
      }
      *((char *) (*d)) = (unsigned char) INP(RL);
      *((char *) (*d) + 1) = (unsigned char) INP(RM);
      *((char *) (*d) + 2) = (unsigned char) INP(RH);
      *((char *) (*d) + 3) = 0;
      (*d)++;
      aa++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam16i_sn(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   int i, x, q;

   for (i = 0; i < r; i++) {
      cam16i_q(c, n + i, a, f, (*d)++, &x, &q);
      if (!q)
         break;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24i_sn(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   int i, x, q;

   for (i = 0; i < r; i++) {
      cam24i_q(c, n + i, a, f, (*d)++, &x, &q);
      if (!q)
         break;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam8o(const int c, const int n, const int a, const int f, unsigned char d)
{
}

/*------------------------------------------------------------------*/
INLINE void camo(const int c, const int n, const int a, const int f, WORD d)
{
   WORD loop = 1000;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   OUTP(WL, (unsigned char) (d));
   OUTP(WM, *(((unsigned char *) &d) + 1));
   OUTP(CACY, 1);
   while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
      loop--;
   if (loop == 0)
      printf("cami: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
}

/*------------------------------------------------------------------*/
INLINE void cam16o(const int c, const int n, const int a, const int f, WORD d)
{
   camo(c, n, a, f, d);
}

/*------------------------------------------------------------------*/
INLINE void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
   WORD loop = 1000;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   OUTP(WL, (unsigned char) (d));
   OUTP(WM, *(((unsigned char *) &d) + 1));
   OUTP(WH, *(((unsigned char *) &d) + 2));
   OUTP(CACY, 1);
   while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
      loop--;
   if (loop == 0)
      printf("cami: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
}

/*------------------------------------------------------------------*/
INLINE void cam16o_q(const int c, const int n, const int a, const int f,
                     WORD d, int *x, int *q)
{
   WORD loop = 1000;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   OUTP(WL, (unsigned char) (d));
   OUTP(WM, *(((unsigned char *) &d) + 1));
   OUTP(CACY, 1);
   while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
      loop--;
   if (loop == 0)
      printf("cami: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
}

/*------------------------------------------------------------------*/
INLINE void cam24o_q(const int c, const int n, const int a, const int f,
                     DWORD d, int *x, int *q)
{
   WORD loop = 1000;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   OUTP(WL, (unsigned char) (d));
   OUTP(WM, *(((unsigned char *) &d) + 1));
   OUTP(WH, *(((unsigned char *) &d) + 2));
   OUTP(CACY, 1);
   while (((INT) ((INP(OPST)) & 0x7) != 5) && loop > 0)
      loop--;
   if (loop == 0)
      printf("cami: status:0x%x\n", (INT) ((INP(OPST)) & 0x7));
   *q = INP(LXQ);
   *x = *q >> 1 & 1;
   *q &= 1;
}

/*------------------------------------------------------------------*/
INLINE void cam8o_r(const int c, const int n, const int a, const int f,
                    BYTE * d, const int r)
{
}

/*------------------------------------------------------------------*/
INLINE void cam16o_r(const int c, const int n, const int a, const int f,
                     WORD * d, const int r)
{
   WORD i;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   for (i = 0; i < r; i++) {
      OUTP(WL, *((unsigned char *) d));
      OUTP(WM, *(((unsigned char *) d) + 1));
      OUTP(CACY, 1);
      d++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24o_r(const int c, const int n, const int a, const int f,
                     DWORD * d, const int r)
{
   WORD i;

   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   for (i = 0; i < r; i++) {
      OUTP(WL, *((unsigned char *) d));
      OUTP(WM, *(((unsigned char *) d) + 1));
      OUTP(WH, *(((unsigned char *) d) + 2));
      OUTP(CACY, 1);
      d++;
   }
}

/*------------------------------------------------------------------*/
INLINE int camc_chk(const int c)
{
   unsigned int n, a, f;

   /* check if crate controller is online */
   a = INP(CDMA);
   if ((a & 0x4) == 0)
      return -1;

   /* clear inhibit */
   camc(c, 1, 2, 32);

   /* read back naf */
   a = (unsigned char) INP(ADD);
   f = (unsigned char) INP(FUN);
   n = (unsigned char) INP(STA);

   if (n != 1 || a != 2 || f != 32)
      return -1;

   return 0;
}

/*------------------------------------------------------------------*/
INLINE void camc(const int c, const int n, const int a, const int f)
{
   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   OUTP(CACY, 1);
}

/*------------------------------------------------------------------*/
INLINE void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(STA, n);
   OUTP(ADD, a);
   OUTP(FUN, f);
   OUTP(CACY, 1);
   *q = INP(LXQ);
   *q &= 1;
}

/*------------------------------------------------------------------*/
INLINE void camc_sa(const int c, const int n, const int a, const int f, const int r)
{
   int i;

   for (i = 0; i < r; i++)
      camc(c, n, a + i, f);
}

/*------------------------------------------------------------------*/
INLINE void camc_sn(const int c, const int n, const int a, const int f, const int r)
{
   int i;

   for (i = 0; i < r; i++)
      camc(c, n + i, a, f);
}

/*------------------------------------------------------------------*/
#ifdef _MSC_VER
static HANDLE _hdio = 0;
#endif

INLINE int cam_init(void)
{

#ifdef _MSC_VER
   OSVERSIONINFO vi;
   DWORD buffer[] = { 6, CAMAC_BASE, CAMAC_BASE + 4 * 0x10, 0 };
   DWORD size;

   vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&vi);

   /* use DirectIO driver under NT to gain port access */
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
      _hdio = CreateFile("\\\\.\\directio", GENERIC_READ, FILE_SHARE_READ, NULL,
                         OPEN_EXISTING, 0, NULL);
      if (_hdio == INVALID_HANDLE_VALUE)
         return -1;
   }

   if (!DeviceIoControl(_hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer),
                        NULL, 0, &size, NULL))
      return -1;
#endif                          // _MSC_VER
#ifdef OS_LINUX
   /* 
      In order to access the IO ports of the CAMAC interface, one needs
      to call the ioperm() function for those ports. This requires root
      privileges. For normal operation, this is performed by the "dio"
      program, which is a "setuid" program having temporarily root privi-
      lege. So the frontend is started with "dio frontend". Since the
      frontend cannot be debugged through the dio program, we suplly here
      the direct ioperm call which requires the program to be run as root,
      making it possible to debug it. The program has then to be compiled
      with "gcc -DDO_IOPERM -o frontend frontend.c dsp004.c ..."
    */

#ifdef DO_IOPERM
   ioperm(0x80, 1, 1);
   if (ioperm(CAMAC_BASE, 4 * 0x10, 1) < 0)
      printf("hyt1331.c: Cannot call ioperm() (no root privileges)\n");
#endif

#endif
   OUTP(ZCI, (1 << 6));
   OUTP(CACY, 1);
   return SUCCESS;
}

/*------------------------------------------------------------------*/
INLINE void cam_exit(void)
{
#ifdef _MSC_VER
   DWORD buffer[] = { 6, CAMAC_BASE, CAMAC_BASE + 4 * 0x10, 0 };
   DWORD size;

   if (_hdio <= 0)
      return;

   DeviceIoControl(_hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer),
                   NULL, 0, &size, NULL);
#endif
}

/*------------------------------------------------------------------*/
INLINE void cam_inhibit_set(const int c)
{
   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(ZCI, 0x4);
   OUTP(CACY, 1);
}

/*------------------------------------------------------------------*/
INLINE int cam_inhibit_test(const int c)
{
   return 0;
}

/*------------------------------------------------------------------*/
INLINE void cam_inhibit_clear(const int c)
{
   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(ZCI, 0x0);
   OUTP(CACY, 1);
}

/*------------------------------------------------------------------*/
INLINE void cam_crate_clear(const int c)
{
   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(ZCI, 0x2);
   OUTP(CACY, 1);
}

/*------------------------------------------------------------------*/
INLINE void cam_crate_zinit(const int c)
{
   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   OUTP(ZCI, 0x1);
   OUTP(CACY, 1);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_enable(const int c, const int n)
{
   /* enable LAM flip-flop in unit */
   camc(c, n, 0, 26);

   /* clear LAM flip-flop in unit */
   camc(c, n, 0, 10);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_disable(const int c, const int n)
{

/* enable LAM flip-flop in unit */
   camc(c, n, 0, 24);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_read(const int c, DWORD * lam)
{
   OUTP(CDMA, (((c & 0x3) - 1) << 4));
   *lam = INP(LXQ);
   *lam = (*lam >> 2) & 0x1f;
   *lam = 1 << (*lam - 1);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_clear(const int c, const int n)
{
   camc(c, n, 0, 9);
}

/*------------------------------------------------------------------*/
INLINE void cam_interrupt_enable(const int c)
{
}

/*------------------------------------------------------------------*/
INLINE void cam_interrupt_disable(const int c)
{
}

/*------------------------------------------------------------------*/
INLINE int cam_interrupt_test(const int c)
{
   return 1;
}

/*------------------------------------------------------------------*/
INLINE int cam_init_rpc(char *host_name, char *exp_name, char *fe_name, char *client_name,
                        char *rpc_server)
{
   return 1;
}

/*------------------------------------------------------------------*/
