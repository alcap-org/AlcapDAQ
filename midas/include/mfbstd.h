/*********************************************************************

  Name:         mfbstr.h
  Created by:   Stefan Ritt

  Cotents:      MIDAS FASTBUS standard routines. Has to be combined
                with either LRS1821.C or STR340.C

  *** muCap patches ***
  - choose DWORD type appropriately for x86-64.
                
  $Log: mfbstd.h,v $
  Revision 1.2  2005/06/21 00:14:14  mucap
  (Fred)
  Choose DWORD type appropriately for x86-64.

  Revision 1.4  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.3  2000/09/22 07:34:19  midas
  Moved to main include directory

  Revision 1.1  1999/12/20 10:18:11  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:57  midas
  Added Log tag in header


*********************************************************************/

/*---- replacements if not running under MIDAS ---------------------*/

#ifndef MIDAS_TYPE_DEFINED
#define MIDAS_TYPE_DEFINED

typedef unsigned short int WORD;

#if defined(__alpha) || defined(__x86_64__)
typedef unsigned int DWORD;
#else
typedef unsigned long int DWORD;
#endif

#define SUCCESS  1

#endif                          /* MIDAS_TYPE_DEFINED */

/* make functions under WinNT dll exportable */
#if defined(_MSC_VER) && defined(MIDAS_DLL)
#define EXPRT __declspec(dllexport)
#else
#define EXPRT
#endif


/*---- function declarations ---------------------------------------*/

/* make functions callable from a C++ program */
#ifdef __cplusplus
extern "C" {
#endif

   int EXPRT fb_init();
   void EXPRT fb_exit();
   int EXPRT fb_reset(void);

   int EXPRT fb_frd(int paddr, int saddr, DWORD * data);
   int EXPRT fb_frc(int paddr, int saddr, DWORD * data);
   int EXPRT fb_fwd(int paddr, int saddr, DWORD data);
   int EXPRT fb_fwc(int paddr, int saddr, DWORD data);
   int EXPRT fb_fwdm(int paddr, int saddr, DWORD data);
   int EXPRT fb_fwcm(int b_case, int paddr, int saddr, DWORD data);
   int EXPRT fb_frcm(int b_case, DWORD * data);
   int EXPRT fb_frdb(int paddr, int saddr, DWORD * data, int *count);
   int EXPRT fb_out(DWORD data);
   int EXPRT fb_in(void);
   void EXPRT fb_frdba(int paddr, int saddr, int count);
   int EXPRT fb_load_begin(int addr);
   int EXPRT fb_load_end(void);
   int EXPRT fb_execute(int addr, void *buffer, int *count);

#ifdef __cplusplus
}
#endif
