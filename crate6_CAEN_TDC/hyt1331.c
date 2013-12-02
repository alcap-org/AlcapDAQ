/********************************************************************

  Name:         hyt1331.c
  Created by:   Stefan Ritt

  Contents:     Device driver for HYTEC 1331 Turbo CAMAC controller
                either via IS PC card or via 5331 PCI interface card
                following the MIDAS CAMAC Standard under DIRECTIO

  27-May-2008 Volodya: 
       added #include <sys/io.h>

  $Log: hyt1331.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.24  2004/04/20 13:27:22  midas
  Fixed bug with PCI interface

  Revision 1.23  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.22  2003/10/30 12:21:45  midas
  Fixed bug with pci_scan under linux

  Revision 1.21  2003/10/29 13:08:15  midas
  Stop after one PCI device found

  Revision 1.20  2003/10/06 15:01:33  midas
  Removed tabs

  Revision 1.19  2002/03/12 22:28:50  pierre
  fix gbl_sw1d, cam16i_sa for sw1d

  Revision 1.18  2002/02/12 14:56:04  midas
  Opened port 0x80 for delayed write

  Revision 1.17  2002/01/16 14:54:52  midas
  Removed debugging printf()

  Revision 1.16  2002/01/16 14:40:45  midas
  Fixed bug with gbl_sw1d

  Revision 1.15  2001/09/04 12:53:41  midas
  Fixed small bug

  Revision 1.14  2001/08/22 13:53:32  midas
  Fixed small bug

  Revision 1.13  2001/08/16 09:39:12  midas
  Changed comments

  Revision 1.12  2001/08/14 10:27:40  midas
  Restore signal handler on error

  Revision 1.11  2001/08/14 10:15:43  midas
  Removed debugging statements

  Revision 1.10  2001/08/14 09:42:42  midas
  Added autodetection of ISA and PCI cards


\********************************************************************/

#include <stdio.h>
#include <string.h>
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
#include <signal.h>
#define OUTP(_p, _d) _outp((WORD) (_p), (BYTE) (_d))
#define OUTPW(_p, _d) _outpw((WORD) (_p), (WORD) (_d))
#define OUTPD(_p, _d) _outpd((WORD) (_p), (unsigned int) (_d))
#define INP(_p) _inp((WORD) (_p))
#define INPW(_p) _inpw((WORD) (_p))
#define INPD(_p) _inpd((WORD) (_p))
#define OUTP_P(_p, _d) {_outp((WORD) (_p), (int) (_d)); _outp((WORD)0x80,0);}
#define OUTPW_P(_p, _d) {_outpw((WORD) (_p), (WORD) (_d)); _outp((WORD)0x80,0);}
#define INP_P(_p) _inp((WORD) (_p)); _outp((WORD)0x80,0);
#define INPW_P(_p) _inpw((WORD) (_p));_outp((WORD)0x80,0);
#elif defined( __linux__ )
#if !defined(__OPTIMIZE__)
#error Please compile hyt1331.c with the -O flag to make port access possible
#endif
//#include <asm/io.h>
#include <sys/io.h>
#include <unistd.h>
#include <signal.h>
#define OUTP(_p, _d) outb(_d, _p)
#define OUTPW(_p, _d) outw(_d, _p)
#define INP(_p) inb(_p)
#define INPW(_p) inw(_p)
#define OUTP_P(_p, _d) outb_p(_d, _p)
#define OUTPW_P(_p, _d) outw_p(_d, _p)
#define INP_P(_p) inb_p(_p)
#define INPW_P(_p) inw_p(_p)
#endif

/*---- global variables  -------------------------------------------*/

#define MAX_DEVICES 4           /* Maximal 4 PC cards                   */

WORD io_base[MAX_DEVICES];      /* IO base addrress for PC interfaces   */
BYTE irq[MAX_DEVICES];          /* IRQ for PC interfaces                */

int gbl_sw1d[MAX_DEVICES];      /* Switch 1D on HYT1331 controlelr      */

/*------------------------------------------------------------------*/

INLINE void cam8i(const int c, const int n, const int a, const int f, BYTE * d)
{
   WORD adr, status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   status = (BYTE) INP(adr + 6);
   *((BYTE *) d) = INP(adr);
}

/*------------------------------------------------------------------*/

INLINE void cami(const int c, const int n, const int a, const int f, WORD * d)
{
   WORD adr, status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   status = (BYTE) INP(adr + 6);
   *((BYTE *) d) = INP(adr);
   *(((BYTE *) d) + 1) = INP(adr + 2);
}

/*------------------------------------------------------------------*/

INLINE void cam16i(const int c, const int n, const int a, const int f, WORD * d)
{
   cami(c, n, a, f, d);
}

/*------------------------------------------------------------------*/

INLINE void cam24i(const int c, const int n, const int a, const int f, DWORD * d)
{
   WORD adr;
   BYTE status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   status = (BYTE) INP(adr + 6);
   *((BYTE *) d) = INP(adr);
   *(((BYTE *) d) + 1) = INP(adr + 2);
   *(((BYTE *) d) + 2) = INP(adr + 4);
   *(((BYTE *) d) + 3) = 0;
}

/*------------------------------------------------------------------*/

INLINE void cam8i_q(const int c, const int n, const int a, const int f,
                    BYTE * d, int *x, int *q)
{
   WORD adr;
   BYTE status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   status = (BYTE) INP(adr + 6);
   *((BYTE *) d) = INP(adr);

   *q = status & 1;
   *x = status >> 7;
}

/*------------------------------------------------------------------*/

INLINE void cam16i_q(const int c, const int n, const int a, const int f,
                     WORD * d, int *x, int *q)
{
   WORD adr;
   BYTE status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   status = (BYTE) INP(adr + 6);
   *((BYTE *) d) = INP(adr);
   *(((BYTE *) d) + 1) = INP(adr + 2);

   *q = status & 1;
   *x = status >> 7;
}

/*------------------------------------------------------------------*/

INLINE void cam24i_q(const int c, const int n, const int a, const int f,
                     DWORD * d, int *x, int *q)
{
   WORD adr, status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   status = (BYTE) INP(adr + 6);
   *((BYTE *) d) = INP(adr);
   *(((BYTE *) d) + 1) = INP(adr + 2);
   *(((BYTE *) d) + 2) = INP(adr + 4);
   *(((BYTE *) d) + 3) = 0;

   *q = status & 1;
   *x = status >> 7;
}

/*------------------------------------------------------------------*/

INLINE void cam16i_r(const int c, const int n, const int a, const int f,
                     WORD ** d, const int r)
{
   WORD adr, i, status;

   adr = io_base[c >> 2] + ((c % 3) << 4);

   OUTP(adr + 8, n);
   OUTP(adr + 6, a);
   OUTP_P(adr + 10, f);

   status = (BYTE) INP(adr + 6);
   *((BYTE *) * d) = INP(adr);
   *(((BYTE *) * d) + 1) = INP(adr + 2);        /* read the first word */
   (*d)++;

   INPW_P(adr + 12);            /* trigger first cycle */

   for (i = 0; i < (r - 1); i++)
      *((*d)++) = INPW(adr + 12);       /* read data and trigger next cycle */
}

/*------------------------------------------------------------------*/

INLINE void cam24i_r(const int c, const int n, const int a, const int f,
                     DWORD ** d, const int r)
{
   WORD adr, i, status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr + 6, a);

   for (i = 0; i < r; i++) {
      OUTP(adr + 10, f);
      status = (BYTE) INP(adr + 6);
      *((BYTE *) * d) = INP(adr);
      *(((BYTE *) * d) + 1) = INP(adr + 2);
      *(((BYTE *) * d) + 2) = INP(adr + 4);
      *(((BYTE *) * d) + 3) = 0;
      (*d)++;
   }

   /*
      gives unrealiable results, SR 6.4.00

      adr = io_base[c >> 2]+((c % 3)<<4);

      OUTP(adr+8, n);
      OUTP(adr+6, a);
      OUTP_P(adr+10,f);

      status = (BYTE) INP(adr+6);
      *((BYTE *) *d) = INP(adr);
      *(((BYTE *) *d)+1) = INP(adr+2); // read the first word
      *(((BYTE *) *d)+2) = INP(adr+4);
      *(((BYTE *) *d)+3) = 0;
      (*d)++;

      INPW_P(adr+12); // trigger first cycle

      for (i=0 ; i<(r-1) ; i++)
      {
      *(((BYTE *) *d)+2) = INP_P(adr+4);
      *(((BYTE *) *d)+3) = 0;
      *((WORD *) *d) = INPW_P(adr+12);
      (*d)++;
      }
    */
}

/*------------------------------------------------------------------*/

INLINE void cam16i_rq(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   WORD adr, i;
   int fail;

   /* following code is disabled by above code */
   adr = io_base[c >> 2] + ((c % 3) << 4);

   OUTP(adr + 8, n);
   OUTP(adr + 6, a);
   /* Turn on the Q-mode for repeat until Q=1 in the INPW(adr+12) */
   OUTP(adr + 1, 0x10);
   OUTP_P(adr + 10, f);
   INPW_P(adr + 12);            /* trigger first cycle */

   for (i = 0; i < r; i++) {
      /* read data and trigger next cycle fail = no valid Q within 12usec */
      **d = INPW_P(adr + 12);
      fail = ((BYTE) INP(adr + 6)) & 0x20;      // going to test!
      if (fail)
         break;
      (*d)++;
   }
   /* Turn off the Q-mode for repeat until Q=1 in the INPW(adr+12) */
   OUTP(adr + 1, 0x0);
}

/*------------------------------------------------------------------*/

INLINE void cam24i_rq(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   WORD adr;
   int i, fail;

   adr = io_base[c >> 2] + ((c % 3) << 4);

   OUTP(adr + 8, n);
   OUTP(adr + 6, a);
   /* Turn on the Q-mode for repeat until Q=1 in the INPW(adr+12) */
   OUTP(adr + 1, 0x10);
   OUTP_P(adr + 10, f);
   INPW_P(adr + 12);            /* trigger first cycle */

   for (i = 0; i < r; i++) {
      /* read data and trigger next cycle fail = no valid Q within 12usec */
      *(((BYTE *) * d) + 2) = INP(adr + 4);
      *(((BYTE *) * d) + 3) = 0;
      *((WORD *) * d) = INPW_P(adr + 12);
      fail = ((BYTE) INP(adr + 6)) & 0x20;      // going to test!
      if (fail)
         break;
      (*d)++;
   }
   /* Turn off the Q-mode for repeat until Q=1 in the INPW(adr+12) */
   OUTP(adr + 1, 0x0);
}

/*------------------------------------------------------------------*/

INLINE void cam16i_sa(const int c, const int n, const int a, const int f,
                      WORD ** d, const int r)
{
   WORD adr, i;

   if (gbl_sw1d[c]) {
      adr = io_base[c >> 2] + ((c % 3) << 4);
      /* enable auto increment */
      OUTP_P(adr + 10, 49);
      OUTP(adr + 8, n);
      OUTP(adr + 6, a - 1);
      OUTP_P(adr + 10, f);
      INPW_P(adr + 12);

      for (i = 0; i < r; i++)
         *((*d)++) = INPW_P(adr + 12);  /* read data and trigger next cycle */

      /* disable auto increment */
      OUTP_P(adr + 10, 48);
   } else
      for (i = 0; i < r; i++)
         cami(c, n, a + i, f, (*d)++);
}

/*------------------------------------------------------------------*/

INLINE void cam24i_sa(const int c, const int n, const int a, const int f,
                      DWORD ** d, const int r)
{
   WORD adr, i;

   if (gbl_sw1d[c]) {
      adr = io_base[c >> 2] + ((c % 3) << 4);

      /* enable auto increment */
      OUTP_P(adr + 10, 49);
      OUTP(adr + 8, n);
      OUTP(adr + 6, a - 1);
      OUTP_P(adr + 10, f);
      INPW_P(adr + 12);
      for (i = 0; i < r; i++) {
         /* read data and trigger next cycle */
         *(((BYTE *) * d) + 2) = INP(adr + 4);
         *(((BYTE *) * d) + 3) = 0;
         *((WORD *) * d) = INPW_P(adr + 12);
         (*d)++;
      }

      /* disable auto increment */
      OUTP_P(adr + 10, 48);
   } else
      for (i = 0; i < r; i++)
         cam24i(c, n, a + i, f, (*d)++);
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

INLINE void cam8o(const int c, const int n, const int a, const int f, BYTE d)
{
   unsigned int adr;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr, d);
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);
}

/*------------------------------------------------------------------*/

INLINE void camo(const int c, const int n, const int a, const int f, WORD d)
{
   unsigned int adr;

   adr = io_base[c >> 2] + ((c % 3) << 4);

   OUTP(adr + 8, n);
   OUTP(adr, (BYTE) d);
   OUTP(adr + 2, *(((BYTE *) & d) + 1));
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);
}

/*------------------------------------------------------------------*/

INLINE void cam16o(const int c, const int n, const int a, const int f, WORD d)
{
   camo(c, n, a, f, d);
}

/*------------------------------------------------------------------*/

INLINE void cam24o(const int c, const int n, const int a, const int f, DWORD d)
{
   unsigned int adr;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr, (BYTE) d);
   OUTP(adr + 2, *(((BYTE *) & d) + 1));
   OUTP(adr + 4, *(((BYTE *) & d) + 2));
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);
}

/*------------------------------------------------------------------*/

INLINE void cam16o_q(const int c, const int n, const int a, const int f,
                     WORD d, int *x, int *q)
{
   unsigned int adr, status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr, (BYTE) d);
   OUTP(adr + 2, *(((BYTE *) & d) + 1));
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   status = (BYTE) INP(adr + 6);
   *q = status & 1;
   *x = status >> 7;
}

/*------------------------------------------------------------------*/

INLINE void cam24o_q(const int c, const int n, const int a, const int f,
                     DWORD d, int *x, int *q)
{
   unsigned int adr, status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr, (BYTE) d);
   OUTP(adr + 2, *(((BYTE *) & d) + 1));
   OUTP(adr + 4, *(((BYTE *) & d) + 2));
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   status = (BYTE) INP(adr + 6);
   *q = status & 1;
   *x = status >> 7;
}

/*------------------------------------------------------------------*/

INLINE void cam8o_r(const int c, const int n, const int a, const int f,
                    BYTE * d, const int r)
{
   WORD adr, i;

   adr = io_base[c >> 2] + ((c % 3) << 4);

   /* trigger first cycle */
   OUTP(adr + 8, n);
   OUTP(adr, *((BYTE *) d));
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   for (i = 0; i < r - 1; i++)
      OUTPW(adr + 12, (BYTE) * (++d));  /* write data and trigger next cycle */
}

/*------------------------------------------------------------------*/

INLINE void cam16o_r(const int c, const int n, const int a, const int f,
                     WORD * d, const int r)
{
   WORD adr, i;

   adr = io_base[c >> 2] + ((c % 3) << 4);

   /* trigger first cycle */
   OUTP(adr + 8, n);
   OUTP(adr, *((BYTE *) d));
   OUTP(adr + 2, *(((BYTE *) d) + 1));
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   for (i = 0; i < r - 1; i++)
      OUTPW(adr + 12, *(++d));  /* write data and trigger next cycle */
}

/*------------------------------------------------------------------*/

INLINE void cam24o_r(const int c, const int n, const int a, const int f,
                     DWORD * d, const int r)
{
   WORD adr, i;

   adr = io_base[c >> 2] + ((c % 3) << 4);

   /* trigger first cycle */
   OUTP(adr + 8, n);
   OUTP(adr, *((BYTE *) d));
   OUTP(adr + 2, *(((BYTE *) d) + 1));
   OUTP(adr + 4, *(((BYTE *) d) + 2));
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   for (i = 0; i < r - 1; i++) {
      d++;
      OUTP(adr + 4, *(((BYTE *) d) + 2));
      OUTPW(adr + 12, (WORD) * d);      /* write data and trigger next cycle */
   }
}

/*------------------------------------------------------------------*/

INLINE int camc_chk(const int c)
{
   unsigned int adr, n, a, f;

   /* clear inhibit */
   camc(c, 1, 2, 32);

   /* read back naf */
   adr = io_base[c >> 2] + ((c % 3) << 4);
   a = (BYTE) INP(adr + 10);
   n = (BYTE) INP(adr + 10);
   f = (BYTE) INP(adr + 10);

   if (n != 1 || a != 2 || f != 32)
      return -1;

   return 0;
}

/*------------------------------------------------------------------*/

INLINE void camc(const int c, const int n, const int a, const int f)
{
   unsigned int adr;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);
}

/*------------------------------------------------------------------*/

INLINE void camc_q(const int c, const int n, const int a, const int f, int *q)
{
   unsigned int adr, status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 8, n);
   OUTP(adr + 6, a);
   OUTP(adr + 10, f);

   status = (BYTE) INP(adr + 6);
   *q = status & 1;
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

INLINE void cam_inhibit_set(const int c)
{
   unsigned int adr;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 10, 33);
}

/*------------------------------------------------------------------*/

INLINE void cam_inhibit_clear(const int c)
{
   unsigned int adr;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 10, 32);
}

/*------------------------------------------------------------------*/

INLINE int cam_inhibit_test(const int c)
{
   unsigned int adr;
   BYTE status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   status = (BYTE) INP(adr + 6);
   return (status & 1) > 0;
}

/*------------------------------------------------------------------*/

INLINE void cam_crate_clear(const int c)
{
   unsigned int adr;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 10, 36);
}

/*------------------------------------------------------------------*/

INLINE void cam_crate_zinit(const int c)
{
   unsigned int adr;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 10, 34);
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_enable(const int c, const int n)
{
   /* enable the station number */
   unsigned int adr;

   /* enable LAM in controller */
   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 10, 64 + n);
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_disable(const int c, const int n)
{
   /* disable the station number */
   unsigned int adr;

   /* disable LAM in controller */
   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 10, 128 + n);
}

/*------------------------------------------------------------------*/

INLINE void cam_interrupt_enable(const int c)
{
   unsigned int adr;

   /* enable interrupts in controller */
   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 10, 41);
}

/*------------------------------------------------------------------*/

INLINE void cam_interrupt_disable(const int c)
{
   unsigned int adr;

   /* disable interrupts in controller */
   adr = io_base[c >> 2] + ((c % 3) << 4);
   OUTP(adr + 10, 40);
}

/*------------------------------------------------------------------*/

INLINE int cam_interrupt_test(const int c)
{
   unsigned int adr;
   BYTE status;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   status = (BYTE) INP(adr + 6);
   return (status & (1 << 2)) > 0;
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_read(const int c, DWORD * lam)
{
   /*
      return a BITWISE coded station NOT the station number
      i.e.: n = 5  ==> lam = 0x10
    */
   unsigned int adr, csr;

   adr = io_base[c >> 2] + ((c % 3) << 4);
   csr = (BYTE) INP(adr + 6);
   if (csr & (1 << 3)) {
      *lam = ((BYTE) INP(adr + 8)) & 0x1F;      // mask upper 3 bits
      *lam = 1 << (*lam - 1);
   } else
      *lam = 0;
}

/*------------------------------------------------------------------*/

INLINE void cam_lam_clear(const int c, const int n)
{
   unsigned int adr;

   /*
      note that the LAM flip-flop in unit must be cleared via

      camc(c, n, 0, 10);

      in the user code prior to the call of cam_lam_clear()
    */

   /* restart LAM scanner in controller */
   adr = io_base[c >> 2] + ((c % 3) << 4);
   INP(adr + 8);
}

/*------------------------------------------------------------------*/

INLINE int cam_init_rpc(char *host_name, char *exp_name, char *fe_name,
                        char *client_name, char *rpc_server)
{
   /* dummy routine for compatibility */
   return 1;
}

/*---- DirectIO functions ------------------------------------------*/

int directio_give_port(DWORD start, DWORD end)
{
#ifdef _MSC_VER

   /* under Windows NT, use DirectIO driver to open ports */

   OSVERSIONINFO vi;
   HANDLE hdio = 0;
   DWORD buffer[] = { 6, 0, 0, 0 };
   DWORD size;

   vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&vi);

   /* use DirectIO driver under NT to gain port access */
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
      hdio = CreateFile("\\\\.\\directio", GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, 0, NULL);
      if (hdio == INVALID_HANDLE_VALUE) {
         printf("hyt1331.c: Cannot access IO ports (No DirectIO driver installed)\n");
         return -1;
      }

      /* open ports */
      buffer[1] = start;
      buffer[2] = end;
      if (!DeviceIoControl(hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer),
                           NULL, 0, &size, NULL))
         return -1;
   }

   return 0;

#elif defined(__linux__)

   /*
      In order to access the IO ports of the CAMAC interface, one needs
      to call the ioperm() function for those ports. This requires root
      privileges. For normal operation, this is performed by the "dio"
      program, which is a "setuid" program having temporarily root privi-
      lege. So the user program is started with "dio <program>". Since the
      program cannot be debugged through the dio program, we suplly here
      the direct ioperm call which requires the program to be run as root,
      making it possible to debug it. The program has then to be compiled
      with "gcc -DDO_IOPERM -o <program> <program>.c hyt1331.c ..."
    */

#ifdef DO_IOPERM

   if (end <= 0x3FF) {
      if (ioperm(start, end - start + 1, 1) < 0) {
         printf("hyt1331.c: Cannot call ioperm() (no root privileges)\n");
         return -1;
      }
   } else {
      if (iopl(3) < 0) {
         printf("hyt1331.c: Cannot call iopl() (no root privileges)\n");
         return -1;
      }
   }

#endif

   return 0;

#else
#error This driver cannot be compiled under this operating system
#endif
}

/*------------------------------------------------------------------*/

int directio_lock_port(DWORD start, DWORD end)
{
#ifdef _MSC_VER

   /* under Windows NT, use DirectIO driver to lock ports */

   OSVERSIONINFO vi;
   HANDLE hdio;
   DWORD buffer[] = { 7, 0, 0, 0 };
   DWORD size;

   vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&vi);

   /* use DirectIO driver under NT to gain port access */
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
      hdio = CreateFile("\\\\.\\directio", GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, 0, NULL);
      if (hdio == INVALID_HANDLE_VALUE) {
         printf("hyt1331.c: Cannot access IO ports (No DirectIO driver installed)\n");
         return -1;
      }

      /* lock ports */
      buffer[1] = start;
      buffer[2] = end;
      if (!DeviceIoControl(hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer),
                           NULL, 0, &size, NULL))
         return -1;
   }

   return 0;

#elif defined(__linux__)

#ifdef DO_IOPERM

   if (end <= 0x3FF) {
      if (ioperm(start, end - start + 0, 0) < 0) {
         printf("hyt1331.c: Cannot call ioperm() (no root privileges)\n");
         return -1;
      }
   } else {
      if (iopl(0) < 0) {
         printf("hyt1331.c: Cannot call iopl() (no root privileges)\n");
         return -1;
      }
   }

#endif

   return 0;

#endif
}

/*---- PCI Scan ----------------------------------------------------*/

int pci_scan(int vendor_id, int device_id, int n_dev, BYTE * pirq, DWORD * ba)
{
#ifdef __linux__

   FILE *f;
   char line[1000];
   int n;
   DWORD base_addr[6], irq, dfn, vend, vend_id, dev_id;

   f = fopen("/proc/bus/pci/devices", "r");
   if (f == NULL)
      return 0;

   n = 0;
   while (fgets(line, sizeof(line), f)) {
      sscanf(line, "%x %x %x %lx %lx %lx %lx %lx %lx",
             &dfn,
             &vend,
             &irq,
             &base_addr[0],
             &base_addr[1], &base_addr[2], &base_addr[3], &base_addr[4], &base_addr[5]);

      vend_id = vend >> 16U;
      dev_id = vend & 0xFFFF;

      /*
         printf("%x:%x %d %x %x %x %x\n", vend_id, dev_id, *irq, base_addr[0], base_addr[1],
         base_addr[2], base_addr[3]);
       */

      if (vend_id == vendor_id && dev_id == device_id)
         n++;

      if (n == n_dev)
         break;
   }

   fclose(f);

   if (n == n_dev) {
      *pirq = irq;
      memcpy(ba, base_addr, sizeof(base_addr));
      return 1;
   }

   return 0;

#elif defined(_MSC_VER)

   DWORD buffer[] = { 8, 0, 0, 0 };
   DWORD retbuf[7];
   HANDLE hdio;
   int size;

   hdio = CreateFile("\\\\.\\directio", GENERIC_READ, FILE_SHARE_READ, NULL,
                     OPEN_EXISTING, 0, NULL);
   if (hdio == INVALID_HANDLE_VALUE) {
      printf("hyt1331.c: Cannot access DirectIO driver\n");
      return -1;
   }

   buffer[1] = vendor_id;
   buffer[2] = device_id;
   buffer[3] = n_dev + 1;
   if (!DeviceIoControl(hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer),
                        retbuf, sizeof(retbuf), &size, NULL))
      return 0;

   if (size == 0)
      return 0;

   *pirq = (BYTE) retbuf[0];
   memcpy(ba, retbuf + 1, sizeof(DWORD) * 6);
   return 1;

#endif
}

/*------------------------------------------------------------------*/

void catch_sigsegv(int signo)
{
   printf
       ("Cannot access ports. Please run with \"dio <prog>\" or compile with \"-DDO_IOPERM\"\n");
   signal(SIGSEGV, SIG_DFL);
}

/*------------------------------------------------------------------*/

INLINE int cam_init(void)
{
   BYTE status, n_dev, i, n, a, f;
   WORD isa_io_base[] = { 0x200, 0x280, 0x300, 0x380 };
   WORD base_test;
   DWORD base_addr[6];

   /* set signal handler for segmet violation */
   signal(SIGSEGV, catch_sigsegv);

   /* scan PCI cards */
   for (n_dev = 0;; n_dev++) {
      if (!pci_scan(0x1196, 0x5331, n_dev + 1, irq + n_dev, base_addr))
         break;

      io_base[n_dev] = (WORD) (base_addr[3] & (~0x3UL));

      printf("hyt1331.c: Found PCI card at 0x%X, IRQ %d\n", io_base[n_dev], irq[n_dev]);

      if (directio_give_port(io_base[n_dev], io_base[n_dev] + 4 * 0x10) < 0) {
         signal(SIGSEGV, SIG_DFL);
         return 0;
      } else {
         n_dev++;
         break;                 // currently only supports one PCI interface
      }
   }

   /* scan ISA cards */
   for (i = 0; i < 4; i++) {
      base_test = isa_io_base[i];

      if (directio_give_port(base_test, base_test + 4 * 0x10) < 0) {
         signal(SIGSEGV, SIG_DFL);
         return 0;
      }

      /* Test if address is writable */
      OUTP(base_test, 0);
      status = INP(base_test);
      if (status != 0) {
         directio_lock_port(base_test, base_test + 4 * 0x10);
         continue;
      }

      /* Test A,N,F readback of ISA card */
      OUTP(base_test + 8, 1);
      OUTP(base_test + 6, 2);
      OUTP(base_test + 10, 32);

      a = (BYTE) INP(base_test + 10);
      n = (BYTE) INP(base_test + 10);
      f = (BYTE) INP(base_test + 10);

      if (n != 1 || a != 2 || f != 32) {
         directio_lock_port(base_test, base_test + 4 * 0x10);
         continue;
      }

      /* ISA card found */
      printf("hyt1331.c: Found ISA card at 0x%X\n", base_test);

      io_base[n_dev++] = base_test;
   }

   if (n_dev == 0) {
      printf("hyt1331.c: No PCI or ISA cards found\n");
      signal(SIGSEGV, SIG_DFL);
      return 0;
   }

   /* open port 80 for delayed write */
   directio_give_port(0x80, 0x80);

   /* check if we have access */
   OUTP(io_base[0], 0);
   signal(SIGSEGV, SIG_DFL);

   /* test auto increment switch SW1D */
   for (i = 0; i < n_dev; i++) {
      status = INP(io_base[i] + 6);
      if (!(status & (1 << 6))) {
         gbl_sw1d[i] = 0;
      } else {
         gbl_sw1d[i] = 1;
      }
   }

   return SUCCESS;
}

/*------------------------------------------------------------------*/

INLINE void cam_exit(void)
{
   int i;

   /* lock IO ports */
   for (i = 0; i < MAX_DEVICES; i++)
      if (io_base[i])
         directio_lock_port(io_base[i], io_base[i] + 4 * 0x10);
}

/*------------------------------------------------------------------*/
