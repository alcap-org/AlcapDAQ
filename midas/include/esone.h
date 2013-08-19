/********************************************************************

  Name:         esone.h
  Created by:   Pierre-Andre Amaudruz & Stefan Ritt

  Contents:     ESONE declarations

  $Log: esone.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:14  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.5  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.4  2001/08/21 09:34:08  midas
  Added a few functions

\********************************************************************/

#ifndef INLINE
#if defined( _MSC_VER )
#define INLINE __inline
#elif defined(__GNUC__)
#define INLINE __inline__
#else
#define INLINE
#endif
#endif

#define EXTERNAL extern

/* make functions under WinNT dll exportable */
#if defined(_MSC_VER) && defined(MIDAS_DLL)
#define EXPRT __declspec(dllexport)
#else
#define EXPRT
#endif

/*------------------------------------------------------------------*/

/* make functions callable from a C++ program */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------*/

   EXTERNAL INLINE void ccinit(void);
   EXTERNAL INLINE void cdreg(int *ext, const int b, const int c, const int n,
                              const int a);
   EXTERNAL INLINE void cssa(const int f, int ext, unsigned short *d, int *q);
   EXTERNAL INLINE void cfsa(const int f, const int ext, unsigned long *d, int *q);
   EXTERNAL INLINE void cccc(const int ext);
   EXTERNAL INLINE void cccz(const int ext);
   EXTERNAL INLINE void ccci(const int ext, int l);
   EXTERNAL INLINE void ctci(const int ext, int *l);
   EXTERNAL INLINE void cccd(const int ext, int l);
   EXTERNAL INLINE void ctcd(const int ext, int *l);
   EXTERNAL INLINE void cdlam(int *lam, const int b, const int c, const int n,
                              const int a, const int inta[2]);
   EXTERNAL INLINE void ctgl(const int lam, int *l);
   EXTERNAL INLINE void cclm(const int lam, int l);
   EXTERNAL INLINE void cclc(const int lam);
   EXTERNAL INLINE void ctlm(const int lam, int *l);
   EXTERNAL INLINE void cfga(int f[], int exta[], int intc[], int qa[], int cb[]);
   EXTERNAL INLINE void csga(int f[], int exta[], int intc[], int qa[], int cb[]);
   EXTERNAL INLINE void cfmad(int f, int extb[], int intc[], int cb[]);
   EXTERNAL INLINE void csmad(int f, int extb[], int intc[], int cb[]);
   EXTERNAL INLINE void cfubc(const int f, int ext, int intc[], int cb[]);
   EXTERNAL INLINE void csubc(const int f, int ext, int intc[], int cb[]);
   EXTERNAL INLINE void cfubr(const int f, int ext, int intc[], int cb[]);
   EXTERNAL INLINE void csubr(const int f, int ext, int intc[], int cb[]);

/*------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif
