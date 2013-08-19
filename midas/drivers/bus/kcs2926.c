/********************************************************************

  Name:         kcs2926.c
  Created by:   Pierre-André Amaudruz

  Contents:     Device driver for Kinetics System KCS2926 8bit CAMAC controller
                following the MIDAS CAMAC Standard for DirectIO

  $Log: kcs2926.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.6  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.5  2002/05/08 22:19:46  pierre
  remove warnings

  Revision 1.4  2001/10/16 20:47:20  pierre
  Update cam_interrupt_() args

  Revision 1.3  2000/09/26 07:10:50  midas
  Added DO_IOPERM for debugging as root

  Revision 1.2  2000/09/07 17:17:03  pierre
  -Add fe_name

  Revision 1.1  1999/12/20 10:18:11  midas
  Reorganized driver directory structure

  Revision 1.5  1999/12/13 17:47:12  pierre
  - Fix cam26/24_r/sa pointers

  Revision 1.4  1999/02/22 19:10:22  pierre
  - Remove came_xxx

  Revision 1.3  1998/11/19 22:28:36  pierre
  Remove Q-stop for _sa and_sn

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

  ADDRESS SA9-SA4
  c=close=on=0
  o=open=off=1

  0x380 -> oo occc (11 1000)
  0x2f0 -> oc oooo (10 1111)
  0x280 -> oc occc (10 1000)
*/

#define CAMAC_BASE 0x280

#define CSR1 CAMAC_BASE+0
#define CSR2 CAMAC_BASE+0x1
#define MCR  CAMAC_BASE+0x2
#define C    CAMAC_BASE+0x3
#define NAF1 CAMAC_BASE+0x4
#define NAF2 CAMAC_BASE+0x5
#define DL   CAMAC_BASE+0x6
#define DM   CAMAC_BASE+0x7
#define DH   CAMAC_BASE+0x8
#define PP   CAMAC_BASE+0x9

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
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(CSR1, 1);
   while (!(INP(CSR1) & 128));
   *((char *) d) = (unsigned char) INP(DL);
   *((char *) d + 1) = (unsigned char) INP(DM);
}

/*------------------------------------------------------------------*/
INLINE void cam16i(const int c, const int n, const int a, const int f, WORD * d)
{
   cami(c, n, a, f, d);
}

/*------------------------------------------------------------------*/
INLINE void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(CSR1, 1);
   while (!(INP(CSR1) & 128));
   *((char *) d) = (unsigned char) INP(DL);
   *((char *) d + 1) = (unsigned char) INP(DM);
   *((char *) d + 2) = (unsigned char) INP(DH);
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
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(CSR1, 1);
   while (!(INP(CSR1) & 128));
   *((char *) d) = (unsigned char) INP(DL);
   *((char *) d + 1) = (unsigned char) INP(DM);
   *((char *) d + 2) = (unsigned char) INP(DH);
   *q = (((INP(CSR1) >> 1) & 1) ^ 1);
   *x = (((INP(CSR1) >> 2) & 1) ^ 1);
}

/*------------------------------------------------------------------*/
INLINE void cam24i_q(const int c, const int n, const int a, const int f,
                     DWORD * d, int *x, int *q)
{
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(CSR1, 1);
   while (!(INP(CSR1) & 128));
   *((char *) d) = (unsigned char) INP(DL);
   *((char *) d + 1) = (unsigned char) INP(DM);
   *((char *) d + 2) = (unsigned char) INP(DH);
   *q = (((INP(CSR1) >> 1) & 1) ^ 1);
   *x = (((INP(CSR1) >> 2) & 1) ^ 1);
}

/*------------------------------------------------------------------*/
INLINE void cam16i_r(const int c, const int n, const int a, const int f,
                     WORD ** d, const int r)
{
   WORD i;

   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   for (i = 0; i < r; i++) {
      OUTP(CSR1, 1);
      while (!(INP(CSR1) & 128));
      *((char *) (*d)) = (unsigned char) INP(DL);
      *((char *) (*d) + 1) = (unsigned char) INP(DM);
      (*d)++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24i_r(const int c, const int n, const int a, const int f,
                     DWORD ** d, const int r)
{
   WORD i;

   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   for (i = 0; i < r; i++) {
      OUTP(CSR1, 1);
      while (!(INP(CSR1) & 128));
      *((char *) (*d)) = (unsigned char) INP(DL);
      *((char *) (*d) + 1) = (unsigned char) INP(DM);
      *((char *) (*d) + 2) = (unsigned char) INP(DH);
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
      if (!q) {
         (*d)--;
         break;
      }
   }
}

/*------------------------------------------------------------------*/
INLINE void cam16i_sa(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   int i, aa;

   aa = a;
   OUTP(C, c);
   for (i = 0; i < r; i++) {
      OUTP(NAF1, (aa << 5) | f);
      OUTP(NAF2, (n << 1) | (a >> 3));
      OUTP(CSR1, 1);
      while (!(INP(CSR1) & 128));
      *((char *) (*d)) = (unsigned char) INP(DL);
      *((char *) (*d) + 1) = (unsigned char) INP(DM);
      aa++;
      (*d)++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24i_sa(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   int i, aa;

   aa = a;
   OUTP(C, c);
   for (i = 0; i < r; i++) {
      OUTP(NAF1, (aa << 5) | f);
      OUTP(NAF2, (n << 1) | (a >> 3));
      OUTP(CSR1, 1);
      while (!(INP(CSR1) & 128));
      *((char *) (*d)) = (unsigned char) INP(DL);
      *((char *) (*d) + 1) = (unsigned char) INP(DM);
      *((char *) (*d) + 2) = (unsigned char) INP(DH);
      aa++;
      (*d)++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam16i_sn(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   int i;
   for (i = 0; i < r; i++)
      cam16i(c, n + i, a, f, (*d)++);
}

/*------------------------------------------------------------------*/
INLINE void cam24i_sn(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   int i;
   for (i = 0; i < r; i++)
      cam24i(c, n + i, a, f, (*d)++);
}

/*------------------------------------------------------------------*/
INLINE void cam8o(const int c, const int n, const int a, const int f, unsigned char d)
{
   printf("cam_8o not implemented\n");

}

/*------------------------------------------------------------------*/
INLINE void camo(const int c, const int n, const int a, const int f, WORD d)
{
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(DL, (unsigned char) d);
   OUTP(DM, *(((unsigned char *) &d) + 1));
   OUTP(DH, 0);
   OUTP(CSR1, 1);
}

/*------------------------------------------------------------------*/
INLINE void cam16o(const int c, const int n, const int a, const int f, WORD d)
{
   camo(c, n, a, f, d);
}

/*------------------------------------------------------------------*/
INLINE void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(DL, (unsigned char) d);
   OUTP(DM, *(((unsigned char *) &d) + 1));
   OUTP(DH, *(((unsigned char *) &d) + 2));
   OUTP(CSR1, 1);
}

/*------------------------------------------------------------------*/
INLINE void cam16o_q(const int c, const int n, const int a, const int f,
                     WORD d, int *x, int *q)
{
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(DL, (unsigned char) d);
   OUTP(DM, *(((unsigned char *) &d) + 1));
   OUTP(DH, 0);
   OUTP(CSR1, 1);
   while (!(INP(CSR1) & 128));
   *q = (((INP(CSR1) >> 1) & 1) ^ 1);
   *x = (((INP(CSR1) >> 2) & 1) ^ 1);
}

/*------------------------------------------------------------------*/
INLINE void cam24o_q(const int c, const int n, const int a, const int f,
                     DWORD d, int *x, int *q)
{
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(DL, (unsigned char) d);
   OUTP(DM, *(((unsigned char *) &d) + 1));
   OUTP(DH, *(((unsigned char *) &d) + 2));
   OUTP(CSR1, 1);
   while (!(INP(CSR1) & 128));
   *q = (((INP(CSR1) >> 1) & 1) ^ 1);
   *x = (((INP(CSR1) >> 2) & 1) ^ 1);
}

/*------------------------------------------------------------------*/
INLINE void cam8o_r(const int c, const int n, const int a, const int f,
                    BYTE * d, const int r)
{
   printf("cam8o_r not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE void cam16o_r(const int c, const int n, const int a, const int f,
                     WORD * d, const int r)
{
   WORD i;
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(DH, 0);
   for (i = 0; i < r; i++) {
      OUTP(DL, *((unsigned char *) d));
      OUTP(DM, *(((unsigned char *) d) + 1));
      OUTP(CSR1, 1);
      d++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24o_r(const int c, const int n, const int a, const int f,
                     DWORD * d, const int r)
{
   WORD i;
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(DH, 0);
   for (i = 0; i < r; i++) {
      OUTP(DL, *((unsigned char *) d));
      OUTP(DM, *(((unsigned char *) d) + 1));
      OUTP(DH, *(((unsigned char *) d) + 2));
      OUTP(CSR1, 1);
      d++;
   }
}

/*------------------------------------------------------------------*/
INLINE int camc_chk(const int c)
{
   unsigned int n, a, f;

   /* clear inhibit */
   camc(c, 1, 2, 32);

   /* read back naf */
   a = (unsigned char) INP(NAF1);
   f = a & 0x1f;
   a = a >> 5;
   n = (unsigned char) INP(NAF2);
   a |= (n & 0x1) << 4;
   n = n >> 1;

   if (n != 1 || a != 2 || f != 32)
      return -1;

   return 0;
}

/*------------------------------------------------------------------*/
INLINE void camc(const int c, const int n, const int a, const int f)
{
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(CSR1, 1);
}

/*------------------------------------------------------------------*/
INLINE void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   OUTP(C, c);
   OUTP(NAF1, (a << 5) | f);
   OUTP(NAF2, (n << 1) | (a >> 3));
   OUTP(CSR1, 1);
   while (!(INP(CSR1) & 128));
   *q = (((INP(CSR1) >> 1) & 1) ^ 1);
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
      with "gcc -DDO_IOPERM -o frontend frontend.c kcs2926.c ..."
    */

#ifdef DO_IOPERM
   ioperm(0x80, 1, 1);
   if (ioperm(CAMAC_BASE, 4 * 0x10, 1) < 0)
      printf("hyt1331.c: Cannot call ioperm() (no root privileges)\n");
#endif

#endif
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
   OUTP(C, c);
   OUTP(DL, 4);
   OUTP(DM, 0);
   OUTP(DH, 0);
   OUTP(NAF2, 60);
   OUTP(NAF1, 17);
   OUTP(CSR1, 1);
}

/*------------------------------------------------------------------*/
INLINE void cam_inhibit_clear(const int c)
{
   OUTP(DL, 0);
   OUTP(DM, 0);
   OUTP(DH, 0);
   OUTP(NAF2, 60);
   OUTP(NAF1, 17);
   OUTP(CSR1, 1);
}

/*------------------------------------------------------------------*/
INLINE void cam_crate_clear(const int c)
{
   OUTP(C, c);
   OUTP(DL, 2);
   OUTP(DM, 0);
   OUTP(DH, 0);
   OUTP(NAF2, 60);
   OUTP(NAF1, 17);
   OUTP(CSR1, 1);
}

/*------------------------------------------------------------------*/
INLINE void cam_crate_zinit(const int c)
{
   OUTP(C, c);
   OUTP(DL, 1);
   OUTP(DM, 0);
   OUTP(DH, 0);
   OUTP(NAF2, 60);
   OUTP(NAF1, 17);
   OUTP(CSR1, 1);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_enable(const int c, const int n)
{
   DWORD mask;

   /* append slot n LAM(bitwise) to LAM mask in controller */
   cam24i(c, 30, 13, 1, &mask);
   mask |= (1 << (n - 1));
   cam24o(c, 30, 13, 17, mask);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_disable(const int c, const int n)
{
   DWORD mask;
   /* disable LAM mask for slot n in controller */
   cam24i(c, 30, 13, 1, &mask);
   mask &= ~(1 << (n - 1));
   cam24o(c, 30, 13, 17, mask);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_read(const int c, DWORD * lam)
{
   cam24i(c, 30, 12, 1, lam);
}

/*------------------------------------------------------------------*/
INLINE void cam_lam_clear(const int c, const int n)
{
}

/*------------------------------------------------------------------*/
INLINE void cam_interrupt_enable(const int c)
{
   printf("cam_interrupt_enable not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE int cam_interrupt_test(const int c)
{
   return 1;
}

/*------------------------------------------------------------------*/
INLINE void cam_interrupt_disable(const int c)
{
   printf("cam_interrupt_disable not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE int cam_init_rpc(char *host_name, char *exp_name, char *fe_name, char *client_name,
                        char *rpc_server)
{
   return 1;
}

/*------------------------------------------------------------------*/
