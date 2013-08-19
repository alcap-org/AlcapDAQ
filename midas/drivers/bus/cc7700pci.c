/********************************************************************

  Name:         cc7700pci.c
  Created by:   Stefan Ritt, adapted from camdrv.c by E. Sanshiro

  Contents:     Device driver for TOYO CC/7700 PCI CAMAC controller
                following the MIDAS CAMAC Standard under DIRECTIO

  $Log: cc7700pci.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:06  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  2002/01/31 09:11:03  midas
  Fixed bugs with directio driver

  Revision 1.1  2000/12/05 00:49:24  midas
  Initial revision


\********************************************************************/

#include <stdio.h>
#include "mcstd.h"

/*------------------------------------------------------------------*/

#if defined( __MSDOS__ )
#include <dos.h>
#define OUTP(_p, _d) outportb(_p, _d)
#define OUTPW(_p, _d) outport(_p, _d)
#define INP(_p) inportb(_p)
#define INPW(_p) inport(_p)
#define OUTP_P(_p, _d) outportb(_p, _d)
#define OUTPW_P(_p, _d) outport(_p, _d)
#define INP_P(_p) inportb(_p)
#define INPW_P(_p) inport(_p)
#elif defined( _MSC_VER )
#include <windows.h>
#include <conio.h>
#define OUTP(_p, _d) _outp((unsigned short) (_p), (int) (_d))
#define OUTPW(_p, _d) _outpw((unsigned short) (_p), (unsigned short) (_d))
#define OUTPL(_p, _d) _outpd((unsigned short) (_p), (unsigned long) (_d))
#define INP(_p) _inp((unsigned short) (_p))
#define INPW(_p) _inpw((unsigned short) (_p))
#define INPL(_p) _inpd((unsigned short) (_p))
#define OUTP_P(_p, _d) {_outp((unsigned short) (_p), (int) (_d)); _outp((unsigned short)0x80,0);}
#define OUTPW_P(_p, _d) {_outpw((unsigned short) (_p), (unsigned short) (_d)); _outp((unsigned short)0x80,0);}
#define INP_P(_p) _inp((unsigned short) (_p)); _outp((unsigned short)0x80,0);
#define INPW_P(_p) _inpw((unsigned short) (_p));_outp((unsigned short)0x80,0);
#elif defined( OS_LINUX )
#if !defined(__OPTIMIZE__)
#error Please compile cc7700.c with the -O flag to make port access possible
#endif
#include <asm/io.h>
//#include <unistd.h>
#define OUTP(_p, _d) outb(_d, _p)
#define OUTPW(_p, _d) outw(_d, _p)
#define OUTPL(_p, _d) outl(_d, _p)
#define INP(_p) inb(_p)
#define INPW(_p) inw(_p)
#define INPL(_p) inl(_p)
#define OUTP_P(_p, _d) outb_p(_d, _p)
#define OUTPW_P(_p, _d) outw_p(_d, _p)
#define INP_P(_p) inb_p(_p)
#define INPW_P(_p) inw_p(_p)
#endif

#if defined( _MSC_VER )
#define INLINE __inline
#elif defined(__GNUC__)
#define INLINE __inline__
#else
#define INLINE
#endif

/*- Global var -----------------------------------------------------*/

static unsigned int iobase;

/* Register mapping */

enum cc7700_regs {
   regCONTROL = 0x00,
   regSTATUS = 0x00,
   regCOMMAND = 0x04,
   regLAM = 0x04,
   regDATA = 0x08,
   regGO = 0x0C
};

enum cc7700_ctrlbits {
   ctrlCLEAR = 0x01,
   ctrlINITIALIZE = 0x02,
   ctrlINHIBIT = 0x04,
   ctrlENABLEINTERRUPT = 0x08,
   ctrlRESET = 0x40,
   ctrlLAMINTERNAL = 0x80
};

enum cc7700_statbits {
   statNOQ = 0x01,
   statNOX = 0x02,
   statINHIBIT = 0x04,
   statENABLEINTERRUPT = 0x08,
   statDONE = 0x10,
   statONLINE = 0x20,
   statLAMSUM = 0x40,
   statLAMINTERNAL = 0x80
};

#define CONTROL_KEPT_BIT_MASK (0x04 | 0x08 | 0x80)
#define STATUS_BIT_MASK 0x00ff

/*------------------------------------------------------------------*/

int wait_done()
{
   int i, status;

   for (i = 0; i < 100; i++) {
      status = INP(iobase + regSTATUS);
      if (status & statDONE)
         break;
   }

   return status;
}

/*------------------------------------------------------------------*/

INLINE void cam8i(const int c, const int n, const int a, const int f, unsigned char *d)
{
   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));

   OUTPW(iobase + regGO, 0);
   wait_done();

   *((unsigned char *) d) = INP(iobase + regDATA);
}

/*------------------------------------------------------------------*/

INLINE void cami(const int c, const int n, const int a, const int f, unsigned short *d)
{
   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));

   OUTPW(iobase + regGO, 0);
   wait_done();

   *d = INPW(iobase + regDATA);
}

/*------------------------------------------------------------------*/

INLINE void cam16i(const int c, const int n, const int a, const int f, unsigned short *d)
{
   cami(c, n, a, f, d);
}

/*------------------------------------------------------------------*/

INLINE void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));

   OUTPW(iobase + regGO, 0);
   wait_done();

   *d = INPL(iobase + regDATA) & 0xFFFFFF;
}

/*------------------------------------------------------------------*/

INLINE void cam8i_q(const int c, const int n, const int a, const int f,
                    unsigned char *d, int *x, int *q)
{
   WORD status;

   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));

   OUTPW(iobase + regGO, 0);
   status = wait_done();

   *((unsigned char *) d) = INP(iobase + regDATA);

   *q = ((status & 1) == 0);
   *x = ((status & 2) == 0);
}

/*------------------------------------------------------------------*/

INLINE void cam16i_q(const int c, const int n, const int a, const int f,
                     unsigned short *d, int *x, int *q)
{
   WORD status;

   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));

   OUTPW(iobase + regGO, 0);
   status = wait_done();

   *d = INPW(iobase + regDATA);

   *q = ((status & 1) == 0);
   *x = ((status & 2) == 0);
}

/*------------------------------------------------------------------*/
INLINE void cam24i_q(const int c, const int n, const int a, const int f,
                     DWORD * d, int *x, int *q)
{
   WORD status;

   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));

   OUTPW(iobase + regGO, 0);
   status = wait_done();

   *d = INPL(iobase + regDATA) & 0xFFFFFF;

   *q = ((status & 1) == 0);
   *x = ((status & 2) == 0);
}

/*------------------------------------------------------------------*/

INLINE void cam16i_r(const int c, const int n, const int a, const int f,
                     unsigned short **d, const int r)
{
   unsigned short i;

   for (i = 0; i < r; i++)
      cam16i(c, n, a, f, (*d)++);
}

/*------------------------------------------------------------------*/
INLINE void cam24i_r(const int c, const int n, const int a, const int f,
                     DWORD ** d, const int r)
{
   unsigned short i;

   for (i = 0; i < r; i++)
      cam24i(c, n, a, f, (*d)++);
}

/*------------------------------------------------------------------*/

INLINE void cam16i_rq(const int c, const int n, const int a, const int f,
                      unsigned short **d, const int r)
{
   unsigned short i;
   int q, x;

   for (i = 0; i < r; i++) {
      cam16i_q(c, n, a, f, (*d)++, &x, &q);
      if (!q)
         break;
   }
}

/*------------------------------------------------------------------*/

INLINE void cam24i_rq(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   unsigned short i;
   int q, x;

   for (i = 0; i < r; i++) {
      cam24i_q(c, n, a, f, (*d)++, &x, &q);
      if (!q)
         break;
   }
}

/*------------------------------------------------------------------*/

INLINE void cam16i_sa(const int c, const int n, const int a, const int f,
                      unsigned short **d, const int r)
{
   unsigned short i;

   for (i = 0; i < r; i++)
      cami(c, n, a + i, f, (*d)++);
}

/*------------------------------------------------------------------*/

INLINE void cam24i_sa(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   unsigned short i;

   for (i = 0; i < r; i++)
      cam24i(c, n, a + i, f, (*d)++);
}

/*------------------------------------------------------------------*/

INLINE void cam16i_sn(const int c, const int n, const int a, const int f,
                      unsigned short **d, const int r)
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
   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));

   OUTP(iobase + regDATA, d);
   OUTPW(iobase + regGO, 0);

   wait_done();
}

/*------------------------------------------------------------------*/

INLINE void camo(const int c, const int n, const int a, const int f, unsigned short d)
{
   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));

   OUTPW(iobase + regDATA, d);
   OUTPW(iobase + regGO, 0);

   wait_done();
}

/*------------------------------------------------------------------*/

INLINE void cam16o(const int c, const int n, const int a, const int f, unsigned short d)
{
   camo(c, n, a, f, d);
}

/*------------------------------------------------------------------*/

INLINE void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));

   OUTPL(iobase + regDATA, d);
   OUTPW(iobase + regGO, 0);

   wait_done();
}

/*------------------------------------------------------------------*/

INLINE void cam16o_q(const int c, const int n, const int a, const int f,
                     unsigned short d, int *x, int *q)
{
   int status;

   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));

   OUTPL(iobase + regDATA, d);
   OUTPW(iobase + regGO, 0);

   status = wait_done();

   *q = ((status & 1) == 0);
   *x = ((status & 2) == 0);
}

/*------------------------------------------------------------------*/

INLINE void cam24o_q(const int c, const int n, const int a, const int f,
                     DWORD d, int *x, int *q)
{
   int status;

   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));

   OUTPL(iobase + regDATA, d);
   OUTPW(iobase + regGO, 0);

   status = wait_done();

   *q = ((status & 1) == 0);
   *x = ((status & 2) == 0);
}

/*------------------------------------------------------------------*/

INLINE void cam8o_r(const int c, const int n, const int a, const int f,
                    BYTE * d, const int r)
{
   unsigned short i;

   for (i = 0; i < r; i++)
      cam8o(c, n, a, f, *(d++));
}

/*------------------------------------------------------------------*/

INLINE void cam16o_r(const int c, const int n, const int a, const int f,
                     unsigned short *d, const int r)
{
   unsigned short i;

   for (i = 0; i < r; i++)
      cam16o(c, n, a, f, *(d++));
}

/*------------------------------------------------------------------*/
INLINE void cam24o_r(const int c, const int n, const int a, const int f,
                     DWORD * d, const int r)
{
   unsigned short i;

   for (i = 0; i < r; i++)
      cam24o(c, n, a, f, *(d++));
}

/*------------------------------------------------------------------*/

INLINE int camc_chk(const int c)
{
   unsigned int status;

   /* check status, don't know how to check controller */
   status = INP(iobase + regSTATUS);
   if ((status & 0xF0) != 0x30)
      return -1;

   return 0;
}

/*------------------------------------------------------------------*/

INLINE void camc(const int c, const int n, const int a, const int f)
{
   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));
   OUTPL(iobase + regGO, 0);
   wait_done();
}

/*------------------------------------------------------------------*/

INLINE void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   unsigned status;

   OUTPL(iobase + regCOMMAND, ((c & 0x7) << 16) |
         (((n - 1) & 0x1F) << 9) | ((a & 0xF) << 5) | (f & 0x1F));
   OUTPL(iobase + regGO, 0);
   status = wait_done();

   *q = ((status & 1) == 0);
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
   DWORD buffer[] = { 8, 0x0001, 0xcc77, 0 };
   DWORD retbuf[7];
   int size;

   vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&vi);

   /* use DirectIO driver under NT to gain port access */
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
      _hdio = CreateFile("\\\\.\\directio", GENERIC_READ, FILE_SHARE_READ, NULL,
                         OPEN_EXISTING, 0, NULL);
      if (_hdio == INVALID_HANDLE_VALUE) {
         printf("cc7700pci.c: Cannot access IO ports (No DirectIO driver installed)\n");
         return -1;
      }
   }

   /* get IO base from ProbePCI function in DirectIO driver */
   if (!DeviceIoControl(_hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer),
                        retbuf, sizeof(retbuf), &size, NULL))
      return -1;

   /* stip last four bits */
   iobase = (retbuf[1] & 0xFFF0);

   /* open ports for direct access */
   buffer[0] = 6;
   buffer[1] = iobase;
   buffer[2] = iobase + 0x10;

   if (!DeviceIoControl(_hdio, (DWORD) 0x9c406000, buffer, sizeof(buffer),
                        NULL, 0, &size, NULL))
      return -1;

#endif                          /* _MSC_VER */
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
      with "gcc -DDO_IOPERM -o frontend frontend.c hyt1331.c ..."
    */

#ifdef DO_IOPERM
   if (iobase < 0x3FF) {
      ioperm(0x80, 1, 1);
      if (ioperm(iobase, 0x10, 1) < 0) {
         printf("cc7700pci.c: Cannot call ioperm() (no root privileges)\n");
         return -1;
      }
   } else {
      if (iopl(3) < 0) {
         printf("cc7700pci.c: Cannot call iopl() (no root privileges)\n");
         return -1;
      }
   }
#endif

#endif

   /* reset controller */
   OUTPW(iobase + regCONTROL, ctrlRESET);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

INLINE void cam_exit(void)
{
#ifdef _MSC_VER
   DWORD buffer[] = { 6, iobase, iobase + 0x10, 0 };
   DWORD size;

   if (_hdio <= 0)
      return;

   DeviceIoControl(_hdio, (unsigned short) 0x9c406000, &buffer, sizeof(buffer),
                   NULL, 0, &size, NULL);
#endif
#ifdef OS_LINUX
#ifdef DO_IOPERM
   if (iobase < 0x3FF) {
      ioperm(0x80, 1, 0);
      ioperm(iobase, 0x10, 0);
   } else
      iopl(0);
#endif
#endif
}

/*------------------------------------------------------------------*/

INLINE void cam_inhibit_set(const int c)
{
   unsigned int reg;

   reg = INPW(iobase + regCONTROL) & CONTROL_KEPT_BIT_MASK;
   reg |= ctrlINHIBIT;
   OUTPL(iobase + regCONTROL, reg | (c << 16));
}

/*------------------------------------------------------------------*/

INLINE void cam_inhibit_clear(const int c)
{
   unsigned int reg;

   reg = INPW(iobase + regCONTROL) & CONTROL_KEPT_BIT_MASK;
   reg &= ~ctrlINHIBIT;
   OUTPL(iobase + regCONTROL, reg | (c << 16));
}

/*------------------------------------------------------------------*/

INLINE void cam_crate_clear(const int c)
{
   unsigned int reg;

   reg = INPW(iobase + regCONTROL) & CONTROL_KEPT_BIT_MASK;
   reg |= ctrlCLEAR;
   OUTPL(iobase + regCONTROL, reg | (c << 16));
}

/*------------------------------------------------------------------*/

INLINE void cam_crate_zinit(const int c)
{
   unsigned int reg;

   reg = INPW(iobase + regCONTROL) & CONTROL_KEPT_BIT_MASK;
   reg &= ctrlINITIALIZE;
   OUTPL(iobase + regCONTROL, reg | (c << 16));
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_enable(const int c, const int n)
{
   unsigned int reg;

   /* enable interrupts in controller */
   reg = INPW(iobase + regCONTROL) & CONTROL_KEPT_BIT_MASK;
   reg |= ctrlENABLEINTERRUPT;
   OUTPL(iobase + regCONTROL, reg | (c << 16));
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_disable(const int c, const int n)
{
   unsigned int reg;

   /* disenable interrupts in controller */
   reg = INPW(iobase + regCONTROL) & CONTROL_KEPT_BIT_MASK;
   reg &= ~ctrlENABLEINTERRUPT;
   OUTPL(iobase + regCONTROL, reg | (c << 16));
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_read(const int c, DWORD * lam)
{
   *lam = INPL(iobase + regLAM) & 0xFFFFFF;
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_clear(const int c, const int n)
{
   /* signal End-Of-Interrupt */
#ifdef __MSDOS__
   OUTP(0x20, 0x20);
#endif

   /* clear LAM flip-flop in unit */
   camc(c, n, 0, 10);
}

/*------------------------------------------------------------------*/

INLINE void cam_interrupt_enable(const int c)
{
#ifdef __MSDOS__
   unsigned char mask;

   mask = INP(0x21);
   mask &= (~(1 << LAM_IRQ));
   OUTP(0x21, mask);
#endif
}

/*------------------------------------------------------------------*/

INLINE void cam_interrupt_disable(const int c)
{
#ifdef __MSDOS__
   unsigned char mask;

   mask = INP(0x21);
   mask |= 1 << LAM_IRQ;
   OUTP(0x21, mask);
#endif
}

/*------------------------------------------------------------------*/

static void (*old_handler) (void) = (void *) 0;

INLINE void cam_interrupt_attach(const int c, const int n, void (*isr) (void))
{
#ifdef __MSDOS__
   old_handler = (void (*)()) getvect(LAM_INT);
   setvect(LAM_INT, (void interrupt(*)()) isr);
#endif
}

/*------------------------------------------------------------------*/

INLINE void cam_interrupt_detach(const int c, const int n)
{
#ifdef __MSDOS__
   if (old_handler)
      setvect(LAM_INT, (void interrupt(*)()) old_handler);
#endif
}

/*------------------------------------------------------------------*/

INLINE int cam_init_rpc(char *host_name, char *exp_name, char *fe_name, char *client_name,
                        char *rpc_server)
{
   return 1;
}

/*------------------------------------------------------------------*/
