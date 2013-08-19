/*-----------------------------------------------------------------------------
 * Copyright (c) 1996      TRIUMF Data Acquistion Group
 * Please leave this header in any reproduction of that distribution
 *
 * TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 * Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * ----------------------------------------------------------------------------
 *
 * Description	: VME function for the LRS1151 using Macros or INLINE
 *		  No reference to Midas.
 *
 * Application : VME
 *
 * Author:  Pierre-Andre Amaudruz Data Acquisition Group
 *
  $Log: lrs1151.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/01/08 07:51:21  midas
  Changed indentation

  Revision 1.1  1999/12/20 10:18:23  midas
  Reorganized driver directory structure

  Revision 1.3  1999/05/06 18:39:01  pierre
  - PPCxxx support

  Revision 1.2  1998/10/12 12:18:56  midas
  Added Log tag in header

 * Revision 1.0  1996/ Pierre	 Initial revision
 * Revision 1.1  1998/ Pierre	 Split include for specific hardware.
 * ---------------------------------------------------------------------------- */
#ifdef OS_VXWORKS
#include "vxWorks.h"
#include "vme.h"
#endif

#ifdef PPCxxx
#define A32D24_1151	       0xfa000000       /* A32D24_1151 access */
#else
#define A32D24_1151	       0xf0000000       /* A32D24_1151 access */
#endif

#if defined( _MSC_VER )
#define INLINE __inline
#elif defined(__GNUC__)
#define INLINE __inline__
#else
#define INLINE
#endif

#define EXTERNAL extern

#ifndef MIDAS_TYPE_DEFINED
#define MIDAS_TYPE_DEFINED

typedef unsigned short int WORD;

#ifdef __alpha
typedef unsigned int DWORD;
#else
typedef unsigned long int DWORD;
#endif

#endif                          /* MIDAS_TYPE_DEFINED */

/*-Macros--------------------------------------------------------*/
#define LRS1151_READ(_vmebase,_d,_r){\
  {\
     volatile DWORD _ll, *_local, __r;\
	_ll = _vmebase;\
	_local = (DWORD *) (((_ll << 8) | 0x80) | A32D24_1151);\
	__r =_r;\
	if (__r > 16) __r = 16;\
	while (__r > 0) {\
	           *_d++ = *_local++;\
		   __r--;}\
	}\
  }

#define LRS115_CLEAR(_vmebase){\
  {\
     volatile DWORD _ll, _dummy, *_local, __r=16;\
	_ll = _vmebase;\
	_local =(DWORD *)( ((_ll << 8) | 0x40) | A32D24_1151);\
	while (__r > 0) {\
		   _dummy = *_local;\
		   _local++;\
		   __r--;}\
       	}\
  }

/*-input---------------------------------------------------------*/
INLINE void lrs1151_read(DWORD vmeBase, DWORD ** d, int r)
{
   volatile DWORD *local;

   if (r > 16)
      r = 16;
   local = (DWORD *) (((vmeBase << 8) | 0x80) | A32D24_1151);
   while (r > 0) {
      *((*d)++) = *((DWORD *) local);
      local++;
      r--;
   }
}

/*-command-------------------------------------------------------*/
INLINE void lrs1151_clear(DWORD vmeBase)
{
   volatile DWORD *local, dummy, r;

   local = (DWORD *) (((vmeBase << 8) | 0x40) | A32D24_1151);
   for (r = 0; r < 16; r++) {
      dummy = *local++;
   }
}

void lrs1151(void)
{
   printf("\n---> LeCroy 1151 16ch. NIM/ECL scalers <---\n");
   printf("Macro  : LRS1151_READ (DWORD base, DWORD *data, int repeat);\n");
   printf("Macro  : LRS1151_CLEAR (DWORD base);\n");
   printf("Inline : lrs1151_read  (DWORD base, DWORD *data, int repeat);\n");
   printf("Inline : lrs1151_clear (DWORD base);\n");
   printf("Test   : rd1151   (0x7a00)    <--- read    VME scaler\n");
   printf("Test   : clr1151  (0x7a00)    <--- clear   VME scaler\n");
}

void rd1151(DWORD vmeBase)
{
   DWORD i, dd[16], *pdd;
   for (i = 0; i < 16; i++)
      dd[i] = 0;
   pdd = &dd[0];
   lrs1151_read(vmeBase, &pdd, 16);
   for (i = 0; i < 16; i++)
      printf("%i-> %8.8x\n", i, dd[i]);
}

void clr1151(DWORD vmeBase)
{
   lrs1151_clear(vmeBase);
}
