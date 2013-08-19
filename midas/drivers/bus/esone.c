/********************************************************************

  Name:         esone.c
  Created by:   Pierre-Andre Amaudruz & Stefan Ritt

  Contents:     CAMAC interface for ESONE standard using 
                MCSTD (Midas Camac Standard)

  $Log: esone.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.8  2004/01/08 06:51:40  pierre
  Doxygen the file

  Revision 1.7  2002/02/01 19:11:33  pierre
  doc++ cleanup

  Revision 1.6  2001/12/17 18:25:05  pierre
  include cclnk, cculk, ccrgl from khy1331

  Revision 1.5  2001/08/14 10:27:40  midas
  Restore signal handler on error

  Revision 1.3  2001/08/14 09:43:30  midas
  Initial revision

\********************************************************************/

/**dox***************************************************************/
/** @file esone.c
The ESONE CAMAC standard call file
*/

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <stdio.h>
#include "mcstd.h"
#include "esone.h"

#ifndef INLINE
#if defined( _MSC_VER )
#define INLINE __inline
#elif defined(__GNUC__)
#define INLINE __inline__
#else
#define INLINE
#endif
#endif

/*-- external representation added to MCSTD ------------------------*/

INLINE void came_cn(int *ext, const int b, const int c, const int n, const int a)
{
   *ext = (b << 24 | (c << 16) | (n << 8) | a);
}

/*------------------------------------------------------------------*/
INLINE void came_ext(const int ext, int *b, int *c, int *n, int *a)
{
   *b = (ext >> 24) & 0x7;
   *c = (ext >> 16) & 0x7;
   *n = (ext >> 8) & 0x1f;
   *a = (ext >> 0) & 0xf;
}

/*********************************************************************
*  ESONE functions                                                   *
*********************************************************************/

/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/********************************************************************/
/**
CAMAC initialization

CAMAC initialization must be called before any other ESONE
subroutine call.

@return void
*/
INLINE void ccinit(void)
{
   cam_init();
}

/********************************************************************/
/**
CAMAC initialization with return status

fccinit can be called instead of ccinit to determine if the
initialization was successful

@return 1 for success, 0 for failure
*/
INLINE int fccinit(void)
{
   if (cam_init() == SUCCESS)
      return 1;

   return 0;
}

/********************************************************************/
/**
Control Declaration REGister.

Compose an external address from BCNA for later use.
Accessing CAMAC through ext could be faster if the external address is
memory mapped to the processor (hardware dependent). Some CAMAC controller
do not have this option see @ref AppendixB.
@param ext external address
@param b branch number (0..7)
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@return void
*/
INLINE void cdreg(int *ext, const int b, const int c, const int n, const int a)
{
   came_cn(ext, b, c, n, a);
}

/********************************************************************/
/**
Control Short Operation.

16 bit operation on a given external CAMAC address.

The range of the f is hardware dependent. The number indicated below are for
standard ANSI/IEEE Std (758-1979) 
Execute cam16i for f<8, cam16o for f>15, camc_q for (f>7 or f>23)

@param f function code (0..31)
@param ext external address
@param d data word
@param q Q response
@return void
*/
INLINE void cssa(const int f, int ext, unsigned short *d, int *q)
{
   int b, c, n, a, x;

   if (f < 8) {
      /* read */
      came_ext(ext, &b, &c, &n, &a);
      cam16i_q(c, n, a, f, d, &x, q);
   } else if (f > 15) {
      /* write */
      came_ext(ext, &b, &c, &n, &a);
      cam16o_q(c, n, a, f, *d, &x, q);
   } else if ((f > 7) || (f > 23)) {
      /* command */
      came_ext(ext, &b, &c, &n, &a);
      camc_q(c, n, a, f, q);
   }
}

/********************************************************************/
/**
Control Full Operation.

24 bit operation on a given external CAMAC address.

The range of the f is hardware dependent. The number indicated below are for
standard ANSI/IEEE Std (758-1979) 
Execute cam24i for f<8, cam24o for f>15, camc_q for (f>7 or f>23)

@param f function code (0..31)
@param ext external address
@param d data long word
@param q Q response
@return void
*/
INLINE void cfsa(const int f, const int ext, unsigned long *d, int *q)
{
   int b, c, n, a, x;

   if (f < 8) {
      /* read */
      came_ext(ext, &b, &c, &n, &a);
      cam24i_q(c, n, a, f, d, &x, q);
   } else if (f > 15) {
      /* write */
      came_ext(ext, &b, &c, &n, &a);
      cam24o_q(c, n, a, f, *d, &x, q);
   } else if ((f > 7) || (f > 23)) {
      /* command */
      came_ext(ext, &b, &c, &n, &a);
      camc_q(c, n, a, f, q);
   }
}

/********************************************************************/
/**
Control Crate Clear.

Generate Crate Clear function. Execute cam_crate_clear()

@param ext external address
@return void
*/
INLINE void cccc(const int ext)
{
   int b, c, n, a;

   came_ext(ext, &b, &c, &n, &a);
   cam_crate_clear(c);
}

/********************************************************************/
/**
Control Crate Z.

Generate Dataway Initialize. Execute cam_crate_zinit()

@param ext external address
@return void
*/
INLINE void cccz(const int ext)
{
   int b, c, n, a;

   came_ext(ext, &b, &c, &n, &a);
   cam_crate_zinit(c);
}

/********************************************************************/
/**
Control Crate I.

Set or Clear Dataway Inhibit, Execute cam_inhinit_set() /clear()

@param ext external address
@param l action l=0 -> Clear I, l=1 -> Set I
@return void
*/
INLINE void ccci(const int ext, int l)
{
   int b, c, n, a;

   came_ext(ext, &b, &c, &n, &a);
   if (l)
      cam_inhibit_set(c);
   else
      cam_inhibit_clear(c);
}

/********************************************************************/
/**
Test Crate I.

Test Crate Inhibit, Execute cam_inhibit_test()

@param ext external address
@param l action l=0 -> Clear I, l=1 -> Set I
@return void
*/
INLINE void ctci(const int ext, int *l)
{
   int b, c, n, a;

   came_ext(ext, &b, &c, &n, &a);
   *l = cam_inhibit_test(c);
}

/********************************************************************/
/**
Control Crate D.

Enable or Disable Crate Demand.

@param ext external address
@param l action l=0 -> Clear D, l=1 -> Set D
@return void
*/
INLINE void cccd(const int ext, int l)
{
   int b, c, n, a;

   came_ext(ext, &b, &c, &n, &a);

   if (l)
      cam_interrupt_enable(c);
   else
      cam_interrupt_disable(c);
}

/********************************************************************/
/**
Control Test Crate D.

Test Crate Demand.

@param ext external address
@param l D cleared -> l=0, D set -> l=1
@return void
*/
INLINE void ctcd(const int ext, int *l)
{
   int b, c, n, a;

   came_ext(ext, &b, &c, &n, &a);
   *l = cam_interrupt_test(c);
}

/********************************************************************/
/**
Control Declare LAM.

Declare LAM, Identical to cdreg.

@param lam external LAM address
@param b branch number (0..7)
@param c crate number (0..)
@param n station number (0..30)
@param a sub-address (0..15)
@param inta implementation dependent
@return void
*/
INLINE void cdlam(int *lam, const int b, const int c, const int n,
                  const int a, const int inta[2])
{
   /* inta[2] ignored */
   cdreg(lam, b, c, n, a);
}

/********************************************************************/
/**
Control Test Demand Present.

Test the LAM register.

@param ext      external LAM register address
@param l        l !=0 if any LAM is set.
@return void
*/
INLINE void ctgl(const int ext, int *l)
{
   int b, c, n, a;
   unsigned long lam;

   came_ext(ext, &b, &c, &n, &a);
   cam_lam_read(c, &lam);
   *l = (lam > 0);
}

/********************************************************************/
/**
Control Crate LAM.

Enable or Disable LAM. Execute F24 for disable, F26 for enable.

@param lam external address
@param l action l=0 -> disable LAM , l=1 -> enable LAM
@return void
*/
INLINE void cclm(const int lam, int l)
{
   int b, c, n, a;

   came_ext(lam, &b, &c, &n, &a);

   if (l)
      camc(c, n, 0, 26);
   else
      camc(c, n, 0, 24);
}

/********************************************************************/
/**
Link LAM to service procedure

Link a specific service routine to a LAM. Since this routine
is executed asynchronously, care must be taken on re-entrancy.

@param lam external address
@param isr name of service procedure
@return void
*/
INLINE void cclnk(const int lam, void (*isr) (void))
{
   int b, c, n, a;

   came_ext(lam, &b, &c, &n, &a);

   cam_interrupt_attach(c, n, isr);
   cam_lam_enable(c, n);
   cam_lam_clear(c, n);
}

/********************************************************************/
/**
Unlink LAM from service procedure

Performs complementary operation to cclnk.

@param lam external address
@return void
*/
INLINE void cculk(const int lam)
{
   int b, c, n, a;

   came_ext(lam, &b, &c, &n, &a);
   cam_interrupt_detach(c, n);
}

/********************************************************************/
/**
Relink LAM

Re-enable LAM in the controller

@param lam external address
@return void
*/
INLINE void ccrgl(const int lam)
{
   int b, c, n, a;

   came_ext(lam, &b, &c, &n, &a);

   cam_lam_enable(c, n);
   cam_lam_clear(c, n);
   cam_interrupt_enable(c);
}

/********************************************************************/
/**
Control Clear LAM.

Clear the LAM of the station pointer by the lam address.

@param lam external address
@return void
*/
INLINE void cclc(const int lam)
{
   int b, c, n, a;

   came_ext(lam, &b, &c, &n, &a);
   camc(c, n, 0, 10);
}

/********************************************************************/
/**
Test LAM.

Test the LAM of the station pointed by lam. Performs an F8

@param lam external address
@param l No LAM-> l=0, LAM present-> l=1
@return void
*/
INLINE void ctlm(const int lam, int *l)
{
   int b, c, n, a;

   came_ext(lam, &b, &c, &n, &a);
   camc_q(c, n, a, 8, l);
}

/********************************************************************/
/**
Control Full (24bit) word General Action.

@param f function code
@param exta[] external address array
@param intc[] data array
@param qa[] Q response array
@param cb[] control block array<br>
cb[0] : number of function to perform<br>
cb[1] : returned number of function performed
@return void
*/
INLINE void cfga(int f[], int exta[], int intc[], int qa[], int cb[])
{
   int i;

   for (i = 0; i < cb[0]; i++)
      cfsa(f[i], exta[i], (unsigned long *) (&(intc[i])), &(qa[i]));

   cb[1] = cb[0];
}

/********************************************************************/
/**
Control (16bit) word General Action.

@param f function code
@param exta[] external address array
@param intc[] data array
@param qa[] Q response array
@param cb[] control block array <br>
cb[0] : number of function to perform<br>
cb[1] : returned number of function performed
@return void
*/
INLINE void csga(int f[], int exta[], int intc[], int qa[], int cb[])
{
   int i;

   for (i = 0; i < cb[0]; i++)
      cssa(f[i], exta[i], (unsigned short *) (&(intc[i])), &(qa[i]));

   cb[1] = cb[0];
}

/********************************************************************/
/** 
Control Full (24bit) Address Q scan.

Scan all sub-address while Q=1 from a0..a15 max from address extb[0] and store
corresponding data in intc[]. If Q=0 while A<15 or A=15 then cross station boundary is applied
(n-> n+1) and sub-address is reset (a=0). Perform action until either cb[0] action are performed
or current external address exceeds extb[1].

<b>implementation of cb[2] for LAM recognition is not implemented.</b>

@param f function code
@param extb[] external address array<br>
extb[0] : first valid external address <br>
extb[1] : last valid external address
@param intc[] data array
@param cb[] control block array <br>
cb[0] : number of function to perform <br>
cb[1] : returned number of function performed
@return void
*/
INLINE void cfmad(int f, int extb[], int intc[], int cb[])
{
   int j, count;
   int x, q, b, c, n, a;
   unsigned long exts, extc, exte;

   exts = extb[0];
   exte = extb[1];
   count = cb[0];
   j = 0;
   came_ext(exts, &b, &c, &n, &a);
   do {
      cam24i_q(c, n, a, f, (unsigned long *) &intc[j], &x, &q);
      if (q == 0) {
         a = 0;                 /* set subaddress to zero */
         n++;                   /* select next slot */
         j++;
      } else {
         a++;                   /* increment address */
         ++cb[1];               /* increment tally count */
         ++intc;                /* next data array */
         --count;
      }
      came_cn((int *) &extc, b, c, n, a);

      if (extc > exte)
         count = 0;             /* force exit */

   } while (count);
}

/********************************************************************/
/**
Control (16bit) Address Q scan.

Scan all sub-address while Q=1 from a0..a15 max from address extb[0] and store
corresponding data in intc[]. If Q=0 while A<15 or A=15 then cross station boundary is applied
(n-> n+1) and sub-address is reset (a=0). Perform action until either cb[0] action are performed
or current external address exceeds extb[1].

<b>implementation of cb[2] for LAM recognition is not implemented.</b>

@param f function code
@param extb[] external address array<br>
extb[0] : first valid external address <br>
extb[1] : last valid external address
@param intc[] data array
@param cb[] control block array <br>
cb[0] : number of function to perform <br>
cb[1] : returned number of function performed
@return void
*/
INLINE void csmad(int f, int extb[], int intc[], int cb[])
{
   int j, count;
   int x, q, b, c, n, a;
   unsigned long exts, extc, exte;

   exts = extb[0];
   exte = extb[1];
   count = cb[0];
   j = 0;
   came_ext(exts, &b, &c, &n, &a);
   do {
      cam16i_q(c, n, a, f, (unsigned short *) &intc[j], &x, &q);
      if (q == 0) {
         a = 0;                 /* set subaddress to zero */
         n++;                   /* select next slot */
         j++;
      } else {
         a++;                   /* increment address */
         ++cb[1];               /* increment tally count */
         ++intc;                /* next data array */
         --count;
      }
      came_cn((int *) &extc, b, c, n, a);

      if (extc > exte)
         count = 0;             /* force exit */

   } while (count);
}

/********************************************************************/
/**
Control Full (24bit) Block Repeat with Q-stop.

Execute function f on address ext with data intc[] while Q.

@param f function code
@param ext external address array
@param intc[] data array
@param cb[] control block array <br>
cb[0] : number of function to perform <br>
cb[1] : returned number of function performed
@return void
*/
INLINE void cfubc(const int f, int ext, int intc[], int cb[])
{
   int count, q;

   count = cb[0];
   do {
      cfsa(f, ext, (unsigned long *) intc, &q);
      if (q == 0)
         count = 0;             /* stop on no q */
      else {
         ++cb[1];               /* increment tally count */
         ++intc;                /* next data array */
         --count;
      }
   } while (count);
}

/********************************************************************/
/**
Control (16bit) Block Repeat with Q-stop.

Execute function f on address ext with data intc[] while Q.

@param f function code
@param ext external address array
@param intc[] data array
@param cb[] control block array <br>
cb[0] : number of function to perform <br>
cb[1] : returned number of function performed
@return void
*/
INLINE void csubc(const int f, int ext, int intc[], int cb[])
{
   int count, q;

   count = cb[0];
   do {
      cssa(f, ext, (unsigned short *) intc, &q);
      if (q == 0)
         count = 0;             /* stop on no q */
      else {
         ++cb[1];               /* increment tally count */
         ++intc;                /* next data array */
         --count;
      }
   } while (count);
}

/********************************************************************/
/**
Repeat Mode Block Transfer (24bit).

Execute function f on address ext with data intc[] if Q.
If noQ keep current intc[] data. Repeat cb[0] times. 

@param f function code
@param ext external address array
@param intc[] data array
@param cb[] control block array <br>
cb[0] : number of function to perform <br>
cb[1] : returned number of function performed
@return void
*/
INLINE void cfubr(const int f, int ext, int intc[], int cb[])
{
   int q, count;

   count = cb[0];
   do {
      do {
         cfsa(f, ext, (unsigned long *) intc, &q);
      } while (q == 0);

      ++cb[1];                  /* increment tally count */
      ++intc;                   /* next data array */
      --count;
   } while (count);
}

/********************************************************************/
/** 
Repeat Mode Block Transfer (16bit).

Execute function f on address ext with data intc[] if Q.
If noQ keep current intc[] data. Repeat cb[0] times. 

@param f function code
@param ext external address array
@param intc[] data array
@param cb[] control block array <br>
cb[0] : number of function to perform <br>
cb[1] : returned number of function performed
@return void
*/
INLINE void csubr(const int f, int ext, int intc[], int cb[])
{
   int q, count;

   count = cb[0];
   do {
      do {
         cssa(f, ext, (unsigned short *) intc, &q);
      } while (q == 0);

      ++cb[1];                  /* increment tally count */
      ++intc;                   /* next data array */
      --count;
   } while (count);
}
