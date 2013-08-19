/********************************************************************

  Name:         kcs2927.c
  Created by:   Pierre-André Amaudruz

  Contents:     Device driver for Kinetics System KCS2927 16bit CAMAC controller
                following the MIDAS CAMAC Standard for DirectIO

  $Log: kcs2927.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.7  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.6  2002/05/08 22:19:57  pierre
  remove warnings

  Revision 1.5  2001/10/16 20:47:29  pierre
  Update cam_interrupt_() args

  Revision 1.4  2001/04/20 21:56:51  pierre
  - Zeroed upper byte in 24i, sa, rq

  Revision 1.3  2000/09/26 07:10:50  midas
  Added DO_IOPERM for debugging as root

  Revision 1.2  2000/09/07 17:17:14  pierre
  -Add fe_name

  Revision 1.1  1999/12/20 10:18:11  midas
  Reorganized driver directory structure

  Revision 1.5  1999/02/22 19:10:37  pierre
  - Remove came_xxx

  Revision 1.4  1998/11/19 22:31:43  pierre
  correct (*d)-- for the _rq in case of noQ
  Remove Q-stop for _sa and_sn
  Fix 24x_ data access

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

#define CSR CAMAC_BASE+0
#define MCR CAMAC_BASE+0x2
#define CCR CAMAC_BASE+0x4
#define NAF CAMAC_BASE+0x6
#define DLR CAMAC_BASE+0x8
#define DHR CAMAC_BASE+0xa
#define SRR CAMAC_BASE+0xc
#define TCR CAMAC_BASE+0xe

/* Control/Status Register (CSR) Bit Fields */
#define GO		0x01    /* Start a data transfer        */
#define NOQ		0x02    /* Q status of last operation   */
#define NOX		0x04    /* X status of last operation   */
#define ID1		0x08    /* Interface ID1 + ID 2 Ro      */
#define ID2		0x10
#define DONE_ENABLE     0x40    /* Done interrupt enable RW */
#define DONE            0x80    /* Done Ro */
#define RFS_ENABLE      0x100   /* Request For Service Int Enable RW */
#define RFS             0x200   /* LAM is Pending Ro */
#define PP              0x400   /* Parallel Poll Wo */
#define RESET           0x1000  /* Reset Interface Wo */
#define TMO             0x2000  /* Timeout info Ro */
#define ABT             0x4000  /* Abort info Ro */
#define CERROR          0x8000  /* Error = TMO + ABT Ro */
#define CSR_RESET_VALUE	(DONE | NOX | NOQ)
#define MCR_RESET_VALUE	0x00

/* Mode Control Register (MCR) Bit Fields 15-6 not used */
#define AD		0x01    /* Disable Abort on DMA tfer    */
#define WS1		0x02    /* Word size                    */
#define WS2		0x04    /* 00=24bit 01=16bit 10=8bit    */
#define TM1		0x08    /* DMA transfer mode            */
#define	TM2		0x10    /* 00:Q-stop,   01:Q-ignore
                                   10:Q-repeat, 11:Q-scan       */
#define BM		0x20    /* Block transfer mode          */

#define WS_24BIT	0x00
#define WS_16BIT	WS1
#define WS_8BIT		WS2

#define Q_STOP_BITS	0x00
#define Q_IGNORE_BITS	TM1
#define Q_REPEAT_BITS	TM2
#define Q_SCAN_BITS	(TM1 | TM2)


/* CAMAC Crate Address Register (CCR) field 0-2 for crate 0-7*/
#define CRATE_FIELD     0x03

/* CAMAC Command Register (NAF) */
/* 32 functions 
   F16 F8 Op
    0   0 Read
    0   1 Control
    1   0 Write
    1   1 Control */

#define A_FIELD         0x01E0  /* 16 sub addresses */
#define A_SHIFT         5
#define N_FIELD         0x3E00  /* 32 slots */
#define N_SHIFT         9
#define F_FIELD         0x1F    /* 32 functions */
#define RDNAF           0x8000  /* READ NAF Wo Initiate K-bus NAF */

/* Bit fields in the 3922 Status Register */
#define ZINIT	       0x0001
#define CLEAR		     0x0002
#define INHIBIT		   0x0004
#define D_INHIBIT	   0x0040
#define BUFFER_MODE	 0x0080
#define ENABLE_SRQ	 0x0100
#define INT_L24		   0x0200
#define OFFLINE		   0x2000
#define LAM_PRESENT  0x8000

/* CAMAC DATA LOW  Register (DL)  lower 16bit data */
/* CAMAC DATA HIGH Register (DH)  upper 8bit data */
/* SERVICE Request Register (SRR) bit wise crate selection bit 0-7*/
/* Transfer Count Register (TCR) 0-15 Number of 16bit word to be transfered */

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
   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_16BIT);
   OUTPW(CSR, GO);
   while (!(INPW(CSR) & DONE));
   *d = INPW(DLR);
}

/*------------------------------------------------------------------*/
INLINE void cam16i(const int c, const int n, const int a, const int f, WORD * d)
{
   cami(c, n, a, f, d);
}

/*------------------------------------------------------------------*/
INLINE void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_24BIT);
   OUTPW(CSR, GO);
   while (!(INPW(CSR) & DONE));
   *((unsigned short *) d) = INPW(DLR);
   *((unsigned char *) d + 2) = INP(DHR);
}

/*------------------------------------------------------------------*/
INLINE void cam8i_q(const int c, const int n, const int a, const int f,
                    unsigned char *d, int *x, int *q)
{
   printf("cam8i_q not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE void cam16i_q(const int c, const int n, const int a, const int f,
                     WORD * d, int *x, int *q)
{
   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_16BIT);
   OUTPW(CSR, GO);
   while (!(INPW(CSR) & DONE));
   *((unsigned short *) d) = INPW(DLR);
   *q = (((INPW(CSR) >> 1) & 1) ^ 1);
   *x = (((INPW(CSR) >> 2) & 1) ^ 1);
}

/*------------------------------------------------------------------*/
INLINE void cam24i_q(const int c, const int n, const int a, const int f,
                     DWORD * d, int *x, int *q)
{
   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_24BIT);
   OUTPW(CSR, GO);
   while (!(INPW(CSR) & DONE));
   *((unsigned short *) d) = INPW(DLR);
   *((unsigned char *) d + 2) = INP(DHR);
   *d &= 0x00ffffff;
   *q = (((INPW(CSR) >> 1) & 1) ^ 1);
   *x = (((INPW(CSR) >> 2) & 1) ^ 1);
}

/*------------------------------------------------------------------*/
INLINE void cam16i_r(const int c, const int n, const int a, const int f,
                     WORD ** d, const int r)
{
   WORD i;

   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_16BIT);
   for (i = 0; i < r; i++) {
      OUTPW(CSR, GO);
      while (!(INPW(CSR) & DONE));
      *((unsigned short *) (*d)) = INPW(DLR);
      (*d)++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24i_r(const int c, const int n, const int a, const int f,
                     DWORD ** d, const int r)
{
   WORD i;

   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_24BIT);
   for (i = 0; i < r; i++) {
      OUTPW(CSR, GO);
      while (!(INPW(CSR) & DONE));
      *((unsigned short *) (*d)) = INPW(DLR);
      *(((unsigned char *) *d) + 2) = INP(DHR);
      **(d) &= 0x00ffffff;
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
   WORD i, aa;

   aa = a;
   OUTPW(CCR, c);
   OUTPW(MCR, WS_16BIT);
   for (i = 0; i < r; i++) {
      OUTPW(NAF, (n << N_SHIFT) | (aa << A_SHIFT) | f);
      while (!(INPW(CSR) & DONE));
      OUTPW(CSR, GO);
      while (!(INPW(CSR) & DONE));
      *((unsigned short *) (*d)) = INPW(DLR);
      aa++;
      (*d)++;
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24i_sa(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   WORD i, aa;

   aa = a;
   OUTPW(CCR, c);
   OUTPW(MCR, WS_24BIT);
   for (i = 0; i < r; i++) {
      OUTPW(NAF, (n << N_SHIFT) | (aa << A_SHIFT) | f);
      while (!(INPW(CSR) & DONE));
      OUTPW(CSR, GO);
      while (!(INPW(CSR) & DONE));
      *((unsigned short *) (*d)) = INPW(DLR);
      *(((unsigned char *) *d) + 2) = INP(DHR);
      **(d) &= 0x00ffffff;
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
   printf("cam8o not implemented\n");
}

/*------------------------------------------------------------------*/
INLINE void camo(const int c, const int n, const int a, const int f, WORD d)
{
   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_16BIT);
   OUTPW(DLR, d);
   OUTPW(CSR, GO);
   while (!(INPW(CSR) & DONE));
}

/*------------------------------------------------------------------*/
INLINE void cam16o(const int c, const int n, const int a, const int f, WORD d)
{
   camo(c, n, a, f, d);
}

/*------------------------------------------------------------------*/
INLINE void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_24BIT);
   OUTPW(DLR, (unsigned short) d);
   OUTP(DHR, *(((unsigned char *) (&d)) + 2));
   OUTPW(CSR, GO);
   while (!(INPW(CSR) & DONE));
}

/*------------------------------------------------------------------*/
INLINE void cam16o_q(const int c, const int n, const int a, const int f,
                     WORD d, int *x, int *q)
{
   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_16BIT);
   OUTPW(DLR, (unsigned short) d);
   OUTPW(CSR, GO);
   while (!(INPW(CSR) & DONE));
   *q = (((INPW(CSR) >> 1) & 1) ^ 1);
   *x = (((INPW(CSR) >> 2) & 1) ^ 1);
}

/*------------------------------------------------------------------*/
INLINE void cam24o_q(const int c, const int n, const int a, const int f,
                     DWORD d, int *x, int *q)
{
   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_24BIT);
   OUTPW(DLR, (unsigned short) d);
   OUTP(DHR, *(((unsigned char *) (&d)) + 2));
   OUTPW(CSR, GO);
   while (!(INPW(CSR) & DONE));
   *q = (((INPW(CSR) >> 1) & 1) ^ 1);
   *x = (((INPW(CSR) >> 2) & 1) ^ 1);
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
   int i;

   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_16BIT);
   for (i = 0; i < r; i++) {
      OUTPW(DLR, *((unsigned short *) d));
      OUTPW(CSR, GO);
      while (!(INPW(CSR) & DONE));
   }
}

/*------------------------------------------------------------------*/
INLINE void cam24o_r(const int c, const int n, const int a, const int f,
                     DWORD * d, const int r)
{
   WORD i;

   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_24BIT);
   for (i = 0; i < r; i++) {
      OUTPW(DLR, *((unsigned short *) d));
      OUTP(DHR, *(((unsigned char *) (&d)) + 2));
      OUTPW(CSR, GO);
      while (!(INPW(CSR) & DONE));
   }
}

/*------------------------------------------------------------------*/
INLINE int camc_chk(const int c)
{
   unsigned int naf, n, a, f;

   /* clear inhibit */
   camc(c, 1, 2, 32);

   /* read back naf */
   OUTPW(NAF, RDNAF);
   while (!(INPW(CSR) & DONE));
   naf = INPW(NAF);
   n = ((naf >> N_SHIFT) & N_FIELD);
   a = ((naf >> A_SHIFT) & A_FIELD);
   f = naf & F_FIELD;
   if (n != 1 || a != 2 || f != 32)
      return -1;

   return 0;
}

/*------------------------------------------------------------------*/
INLINE void camc(const int c, const int n, const int a, const int f)
{
   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_16BIT);
   OUTPW(CSR, GO);
   while (!(INPW(CSR) & DONE));
}

/*------------------------------------------------------------------*/
INLINE void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   OUTPW(CCR, c);
   OUTPW(NAF, (n << N_SHIFT) | (a << A_SHIFT) | f);
   while (!(INPW(CSR) & DONE));
   OUTPW(MCR, WS_16BIT);
   OUTPW(CSR, GO);
   while (!(INPW(CSR) & DONE));
   *q = (((INPW(CSR) >> 1) & 1) ^ 1);
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
      with "gcc -DDO_IOPERM -o frontend frontend.c kcs2927.c ..."
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
   WORD ccsr;

   cam16i(c, 30, 0, 1, &ccsr);
   ccsr |= INHIBIT;
   cam16o(c, 30, 0, 17, ccsr);
}

/*------------------------------------------------------------------*/
INLINE void cam_inhibit_clear(const int c)
{
   WORD ccsr;

   cam16i(c, 30, 0, 1, &ccsr);
   ccsr &= ~INHIBIT;
   cam16o(c, 30, 0, 17, ccsr);
}

/*------------------------------------------------------------------*/
INLINE void cam_crate_clear(const int c)
{
   WORD ccsr;

   cam16i(c, 30, 0, 1, &ccsr);
   ccsr |= CLEAR;
   cam16o(c, 30, 0, 17, ccsr);
}

/*------------------------------------------------------------------*/
INLINE void cam_crate_zinit(const int c)
{
   WORD ccsr;

   cam16i(c, 30, 0, 1, &ccsr);
   ccsr |= ZINIT;
   cam16o(c, 30, 0, 17, ccsr);
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
