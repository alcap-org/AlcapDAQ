/********************************************************************

  Name:         esone.c
  Created by:   Pierre-Andre Amaudruz & Stefan Ritt

  Contents:     CAMAC interface for ESONE standard using
                MCSTD (Midas Camac Standard)

  $Log: esone.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.1  2001/08/21 14:28:34  midas
  Initial revision

\********************************************************************/

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

/*-- initialization ------------------------------------------------*/

/** ccinit
    CAMAC initialization

    CAMAC initialization must be called before any other ESONE
    subroutine call

    @memo CAMAC initialization.
    @return void
*/
INLINE void ccinit(void)
{
   cam_init();
}

/*------------------------------------------------------------------*/

/** fccinit
    CAMAC initialization with return status

    fccinit can be called instead of ccinit to determine if the
    initialization was successful

    @memo CAMAC initialization.
    @return 1 for success, 0 for failure
*/
INLINE int fccinit(void)
{
   if (cam_init() == SUCCESS)
      return 1;

   return 0;
}

/*-- external representation ---------------------------------------*/

/** cdreg
    Control Declaration REGister.

    Compose an external address from BCNA for later use.
    Accessing CAMAC through ext could be faster if the external address is
    memory mapped to the processor (hardware dependent). Some CAMAC controller
    do not have this option see \Ref{appendix D: Supported hardware}. In this case
    @memo External Address register.
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

/*-- 16bit functions -----------------------------------------------*/

/** cssa
    Control Short Operation.

    16 bit operation on a given external CAMAC address.

    The range of the f is hardware dependent. The number indicated below are for
    standard ANSI/IEEE Std (758-1979) \\
    Execute cam16i for f<8, cam16o for f>15, camc\_q for (f>7 or f>23)

    @memo 16 bit function.
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

/*-- 24bit functions -----------------------------------------------*/

/** cfsa
    Control Full Operation.

    24 bit operation on a given external CAMAC address.

    The range of the f is hardware dependent. The number indicated below are for
    standard ANSI/IEEE Std (758-1979) \\
    Execute cam24i for f<8, cam24o for f>15, camc\_q for (f>7 or f>23)

    @memo 24 bit function.
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

/*-- general functions----------------------------------------------*/

/** cccc
    Control Crate Clear.

    Generate Crate Clear function. Execute cam\_crate\_clear()

    @memo Crate Clear.
    @param ext external address
    @return void
*/
INLINE void cccc(const int ext)
{
   int b, c, n, a;

   came_ext(ext, &b, &c, &n, &a);
   cam_crate_clear(c);
}

/*------------------------------------------------------------------*/

/** cccz
    Control Crate Z.

    Generate Dataway Initialize. Execute cam\_crate\_zinit()

    @memo Crate Z.
    @param ext external address
    @return void
*/
INLINE void cccz(const int ext)
{
   int b, c, n, a;

   came_ext(ext, &b, &c, &n, &a);
   cam_crate_zinit(c);
}

/*------------------------------------------------------------------*/

/** ccci
    Control Crate I.

    Set or Clear Dataway Inhibit, Execute cam\_inhinit\_set() /clear()

    @memo Crate I.
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

/*------------------------------------------------------------------*/

/** ctci
    Test Crate I.

    Test Crate Inhibit, Execute cam\_inhibit\_test()

    @memo Crate I.
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

/*------------------------------------------------------------------*/

/** cccd
    Control Crate D.

    Enable or Disable Crate Demand.

    @memo Crate D.
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

/*------------------------------------------------------------------*/

/** ctcd
    Control Test Crate D.

    Test Crate Demand.

    @memo Test Crate D.
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

/*------------------------------------------------------------------*/

/** cdlam
    Control Declare LAM.

    Declare LAM, Identical to cdreg.

    @memo Declare LAM.
    @param lam external LAM address
    @param b branch number (0..7)
    @param c crate number (0..)
    @param n station number (0..30)
    @param a sub-address (0..15)
    @param inta[2] implementation dependent
    @return void
*/
INLINE void cdlam(int *lam, const int b, const int c, const int n,
                  const int a, const int inta[2])
{
   /* inta[2] ignored */
   cdreg(lam, b, c, n, a);
}

/*------------------------------------------------------------------*/

/** ctgl
    Control Test Demand Present.

    Test the LAM register.

    @memo Test GL.
    @param lam external LAM register address
    @param l  l !=0 if any LAM is set.
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

/*------------------------------------------------------------------*/

/** cclm
    Control Crate LAM.

    Enable or Disable LAM. Execute F24 for disable, F26 for enable.

    @memo dis/enable LAM.
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

/*------------------------------------------------------------------*/

/** cclnk
    Link LAM to service procedure

    Link a specific service routine to a LAM. Since this routine
    is executed asynchronously, care must be taken on re-entrancy.

    @memo Link LAM to service procedure
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

/*------------------------------------------------------------------*/

/** cculk
    Unlink LAM from service procedure

    Performs complementary operation to cclnk.

    @memo Detach LAM from service procedure
    @param lam external address
    @return void
*/
INLINE void cculk(const int lam)
{
   int b, c, n, a;

   came_ext(lam, &b, &c, &n, &a);
   cam_interrupt_detach(c, n);
}

/*------------------------------------------------------------------*/

/** ccrgl
    Relink LAM

    Re-enable LAM in the controller

    @memo Re-enable LAM
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

/*------------------------------------------------------------------*/

/** cclc
    Control Clear LAM.
    Clear the LAM of the staion pointer by the lam address.

    @memo Clear LAM.
    @param lam external address
    @return void
*/
INLINE void cclc(const int lam)
{
   int b, c, n, a;

   came_ext(lam, &b, &c, &n, &a);
   camc(c, n, 0, 10);
}

/*------------------------------------------------------------------*/

/** ctlm
    Test LAM.

    Test the LAM of the station pointed by lam. Performs an F8

    @memo Test LAM.
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

/*------------------------------------------------------------------*/

/** cfga
    Control Full (24bit) word General Action.

    @memo General external address scan function.
    @param f function code
    @param exta[] external address array
    @param intc[] data array
    @param qa[] Q response array
    @param cb[] control block array \\
    cb[0] : number of function to perform \\
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

/*------------------------------------------------------------------*/

/** csga
    Control (16bit) word General Action.

    @memo General external address scan function.
    @param f function code
    @param exta[] external address array
    @param intc[] data array
    @param qa[] Q response array
    @param cb[] control block array \\
    cb[0] : number of function to perform \\
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

/*------------------------------------------------------------------*/

/** cfmad
    Control Full (24bit) Address Q scan.

    Scan all sub-address while Q=1 from a0..a15 max from address extb[0] and store
    corresponding data in intc[]. If Q=0 while A<15 or A=15 then cross station boundary is applied
    (n-> n+1) and sub-address is reset (a=0). Perform action until either cb[0] action are performed
    or current external address exceeds extb[1].

    {\it implementation of cb[2] for LAM recognition is not implemented.}

    @memo Address scan function.
    @param f function code
    @param extb[] external address array \\
    extb[0] : first valid external address \\
    extb[1] : last valid external address
    @param intc[] data array
    @param qa[] Q response array
    @param cb[] control block array \\
    cb[0] : number of function to perform \\
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

/*------------------------------------------------------------------*/

/** csmad
    Control (16bit) Address Q scan.

    Scan all sub-address while Q=1 from a0..a15 max from address extb[0] and store
    corresponding data in intc[]. If Q=0 while A<15 or A=15 then cross station boundary is applied
    (n-> n+1) and sub-address is reset (a=0). Perform action until either cb[0] action are performed
    or current external address exceeds extb[1].

    {\it implementation of cb[2] for LAM recognition is not implemented.}

    @memo Address scan function.
    @param f function code
    @param extb[] external address array \\
    extb[0] : first valid external address \\
    extb[1] : last valid external address
    @param intc[] data array
    @param qa[] Q response array
    @param cb[] control block array \\
    cb[0] : number of function to perform \\
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

/*------------------------------------------------------------------*/

/** cfubc
    Control Full (24bit) Block Repeat with Q-stop.

    Execute function f on address ext with data intc[] while Q.

    @memo Repeat function Q-stop.
    @param f function code
    @param ext external address array
    @param intc[] data array
    @param cb[] control block array \\
    cb[0] : number of function to perform \\
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

/*------------------------------------------------------------------*/

/** csubc
    Control (16bit) Block Repeat with Q-stop.

    Execute function f on address ext with data intc[] while Q.

    @memo Repeat function Q-stop.
    @param f function code
    @param ext external address array
    @param intc[] data array
    @param cb[] control block array \\
    cb[0] : number of function to perform \\
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

/*------------------------------------------------------------------*/

/** cfubr
    Repeat Mode Block Transfer (24bit).

    Execute function f on address ext with data intc[] if Q.
    If noQ keep current intc[] data. Repeat cb[0] times.

    @memo  Repeat function.
    @param f function code
    @param ext external address array
    @param intc[] data array
    @param cb[] control block array \\
    cb[0] : number of function to perform \\
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

/*------------------------------------------------------------------*/

/** csubr
    Repeat Mode Block Transfer (16bit).

    Execute function f on address ext with data intc[] if Q.
    If noQ keep current intc[] data. Repeat cb[0] times.

    @memo  Repeat function.
    @param f function code
    @param ext external address array
    @param intc[] data array
    @param cb[] control block array \\
    cb[0] : number of function to perform \\
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
