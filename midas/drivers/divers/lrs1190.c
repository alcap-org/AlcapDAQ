/*-----------------------------------------------------------------------------
 * Copyright (c) 1996      TRIUMF Data Acquistion Group
 * Please leave this header in any reproduction of that distribution
 * 
 * TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 * Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * ----------------------------------------------------------------------------
 *  
 * Description	: VME Macros and functions for the LRS1190 Dual port buffer
 *		  Display result on the VME console screen.
 *		  No reference to Midas.
 *
 * Application : VME + Vxworks
 *
 * Author:  Pierre-Andre Amaudruz Data Acquisition Group
 * 

   $Log: lrs1190.c,v $
   Revision 1.1.1.1  2005/06/20 23:37:09  mucap
   Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
   (Next, I'll commit our local customizations.)

   Revision 1.2  2004/01/08 07:51:21  midas
   Changed indentation

   Revision 1.1  1999/12/20 10:18:23  midas
   Reorganized driver directory structure

   Revision 1.3  1999/05/06 18:39:34  pierre
   - PPCxxx support

   Revision 1.2  1998/10/12 12:18:56  midas
   Added Log tag in header


 * Revision 1.0  1996/ Pierre	 Initial revision
 * Revision 1.1  1998/ Pierre	 Split include for specific hardware.
 * ---------------------------------------------------------------------------- */
/*------------------------------------------------------------------------------------*/
#include <stdio.h>

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

typedef short int INT;
typedef unsigned short int WORD;

#ifdef __alpha
typedef unsigned int DWORD;
#else
typedef unsigned long int DWORD;
#endif

#endif                          /* MIDAS_TYPE_DEFINED */

#ifdef PPCxxx
#define A32D24_1190	       0xfa000000       /* A32D24_1151 access */
#else
#define A32D24_1190	       0xf0000000       /* A32D24_1151 access */
#endif

/*-Macros--------------------------------------------------------*/
/*-command-------------------------------------------------------*/
#define LRS1190_ENABLE(_vmebase){\
				    {\
				       volatile DWORD _ll, _local;\
				       _ll = _vmebase;\
				       _local = ((_ll) | 0x8002) | A32D24_1190;\
				       *(WORD *)_local = 0x1;\
				   }\
				}

/*-command-------------------------------------------------------*/
#define LRS1190_DISABLE(_vmebase){\
				    {\
				       volatile DWORD _ll, _local;\
				      _ll = _vmebase;\
				       _local = ((_ll) | 0x8002) | A32D24_1190;\
				       *(WORD *)_local = 0x0;\
				   }\
				}

/*-command-------------------------------------------------------*/
#define LRS1190_RESET(_vmebase){\
				   {\
				      volatile DWORD _ll, _dummy, _local;\
				      _ll = _vmebase;\
				      _local = (_ll) | A32D24_1190;\
				      _dummy = *(WORD *) _local;\
				  }\
			       }

/*-command-------------------------------------------------------*/
#define LRS1190_COUNT(_vmebase, _d){\
					{\
					 volatile DWORD _ll, *_pcount;\
					 _ll = _vmebase;\
					 _pcount = (DWORD *) (((_ll) | 0x8000) | A32D24_1190);\
					 *((WORD *) _d) = *(WORD *) _pcount;\
				       }\
				    }


/*-input---------------------------------------------------------*/
#define LRS1190_READI4(_vmebase,_d,_r){\
					{\
					 volatile DWORD _ll, *_local, __r, _count;\
					 DWORD *_pcount;\
					 __r = _r;\
					 _ll = _vmebase;\
					 _pcount = (DWORD *) (((_ll) | 0x8000) | A32D24_1190);\
					 _count = *(WORD *) _pcount;\
					 if (_count <= __r)\
					   __r = _count;\
					 _local = (DWORD *)((_ll)  | A32D24_1190);\
					 while (__r > 0) {\
						*_d++ = *_local++;\
						__r--;}\
					 }\
				      }

/*-input---------------------------------------------------------*/
#define LRS1190_READL2(_vmebase,_d,_r){\
					{\
					 volatile DWORD _ll, *_pcount, *_local, __r, _count;\
					 __r = _r;\
					 _ll = _vmebase;\
					 _pcount = (DWORD *) (((_ll) | 0x8000) | A32D24_1190);\
					 _count = *(WORD *) _pcount;\
					 if (_count <= __r)\
					   __r = _count;\
					 _local = (DWORD *)((_ll)  | A32D24_1190);\
					 while (__r > 0) {\
						*(WORD *) _d = *(WORD *)_local;\
						((WORD *)_d)++;\
						(_local)++;\
						__r--;}\
					 }\
				      }

/*-input---------------------------------------------------------*/
#define LRS1190_READH2(_vmebase,_d,_r){\
					{\
					 volatile DWORD _ll, *_pcount, *_local, _count, __r;\
					 __r = _r;\
					 _ll = _vmebase;\
					 _pcount = (DWORD *) (((_ll) | 0x8000) | A32D24_1190);\
					 _count = *(WORD *) _pcount;\
					 if (_count <= __r)\
					   __r = _count;\
					 _local = (DWORD *)((_ll)  | A32D24_1190);\
					 while (__r > 0) {\
						*(WORD *) _d = *(++((WORD *)_local));\
						((WORD *)_d)++;\
						((WORD *)_local)++;\
						__r--;}\
					 }\
				      }

/*-input---------------------------------------------------------*/
#define LRS_1190_READL2_TONY(_vmebase,_d,_r){\
					{\
					 volatile DWORD _ll, *_pcount, *_local, __r, _count;\
					 __r = _r;\
					 _ll = _vmebase;\
					 _pcount = (DWORD *) (((_ll) | 0x8000) | A32D24_1190);\
					 _count = *(WORD *) _pcount;\
					 if (_count <= __r)\
					   __r = _count;\
					 _local = (DWORD *)((_ll)  | A32D24_1190);\
 					 *(WORD *) _d = __r;\
					 ((WORD *)_d)++;\
					 while (__r > 0) {\
						*(WORD *) _d = *(WORD *)_local;\
						((WORD *)_d)++;\
						(_local)++;\
						__r--;}\
					 }\
				      }
/*---------------------------------------------------------------*/

/*-Functions-----------------------------------------------------*/
INLINE void lrs1190_enable(DWORD vmeBase)
{
   volatile DWORD local;

   local = ((vmeBase) | 0x8002) | A32D24_1190;
   *(WORD *) local = 0x1;
}

/*-Functions-----------------------------------------------------*/
INLINE void lrs1190_disable(DWORD vmeBase)
{
   volatile DWORD local;

   local = ((vmeBase) | 0x8002) | A32D24_1190;
   *(WORD *) local = 0x0;
}

/*-Functions-----------------------------------------------------*/
INLINE void lrs1190_reset(DWORD vmeBase)
{
   volatile DWORD local, dummy;

   local = vmeBase | A32D24_1190;
   dummy = *(WORD *) local;
}

/*-Functions-----------------------------------------------------*/
INLINE void lrs1190_count(DWORD vmeBase, INT * count)
{
   volatile DWORD local;

   local = (vmeBase | 0x8000 | A32D24_1190);
   *count = *(WORD *) local;
}

/*-input---------------------------------------------------------*/
INLINE void lrs1190_readi4(DWORD vmeBase, DWORD ** d, INT r)
{
   volatile DWORD local, count;

   local = (vmeBase | 0x8000 | A32D24_1190);
   count = *(WORD *) local;
   if ((int) count <= r)
      r = count;
   local = vmeBase | A32D24_1190;
   while (r > 0) {
      *((*d)++) = *((DWORD *) local);
      ((DWORD *) local)++;
      r--;
   }
}

/*-input---------------------------------------------------------*/
INLINE void lrs1190_readl2(DWORD vmeBase, WORD ** d, INT r)
{
   volatile DWORD local, count;

   local = (vmeBase | 0x8000 | A32D24_1190);
   count = *(WORD *) local;
   if ((int) count <= r)
      r = count;
   local = vmeBase | A32D24_1190;
   while (r > 0) {
      *((*d)++) = *((WORD *) local);
      ((DWORD *) local)++;
      r--;
   }
}

/*-input---------------------------------------------------------*/
INLINE void lrs1190_readh2(DWORD vmeBase, WORD ** d, INT r)
{
   volatile DWORD local, count;

   local = (vmeBase | 0x8000 | A32D24_1190);
   count = *(WORD *) local;
   if ((int) count <= r)
      r = count;
   local = vmeBase | A32D24_1190;
   while (r > 0) {
      *((*d)++) = *(++((WORD *) local));
      ((WORD *) local)++;
      r--;
   }
}

/*-input---------------------------------------------------------*/
INLINE void lrs1190_readl2z(DWORD vmeBase, WORD ** d, INT r)
{
   volatile DWORD *local, count;

   local = (DWORD *) (vmeBase | 0x8000 | A32D24_1190);
   count = *(WORD *) local;
   if ((int) count <= r)
      r = count;
   local = (DWORD *) (vmeBase | A32D24_1190);

   *((*d)++) = r;
   while (r > 0) {
      *((*d)++) = *((WORD *) local);
      (local)++;
      r--;
   }
}

/*-Tests---------------------------------------------------------*/
void lrs1190(void)
{
   printf("\n---> LRS 1190 Dual port memory buffer (lrs1190.c) <---\n");
   printf("Macro  : LRS1190_ENABLE (DWORD vmeBase);\n");
   printf("Macro  : LRS1190_DISABLE (DWORD vmeBase);\n");
   printf("Macro  : LRS1190_CLEAR (DWORD vmeBase);\n");
   printf("Macro  : LRS1190_COUNT (DWORD vmeBase, int * count);\n");
   printf("Macro  : LRS1190_READI4 (DWORD vmeBase, DWORD *d , int repeat);\n");
   printf("Macro  : LRS1190_READL2 (DWORD vmeBase, WORD *d , int repeat);\n");
   printf("Macro  : LRS1190_READH2 (DWORD vmeBase, WORD *d , int repeat);\n");
   printf("Macro  : LRS1190_READL2_TONY (DWORD vmeBase, DWORD *d , int repeat);\n");
   printf("Inline : lrs1190_enable (DWORD vmeBase);\n");
   printf("Inline : lrs1190_disable (DWORD vmeBase);\n");
   printf("Inline : lrs1190_clear (DWORD vmeBase);\n");
   printf("Inline : lrs1190_count (DWORD vmeBase, int * count);\n");
   printf("Inline : lrs1190_readi4 (DWORD vmeBase, DWORD *d , int repeat);\n");
   printf("Inline : lrs1190_readl2 (DWORD vmeBase, DWORD *d , int repeat);\n");
   printf("Inline : lrs1190_readh2 (DWORD vmeBase, DWORD *d , int repeat);\n");
   printf("Inline : lrs1190_readl2_tony (DWORD vmeBase, DWORD *d , int repeat);\n");
   printf("Test : ena1190  (0x7f0000)         <--- enable  VME buffer\n");
   printf("Test : dis1190  (0x7f0000)         <--- disable VME buffer\n");
   printf("Test : res1190  (0x7f0000)         <--- reset   VME buffer\n");
   printf("Test : cn1190   (0x7f0000)         <--- read    WORD count register\n");
   printf("Test : rd1190   (0x7f0000)         <--- read             VME buffer\n");
   printf("Test : rd1190l  (0x7f0000)         <--- read low  WORD   VME buffer\n");
   printf("Test : rd1190h  (0x7f0000)         <--- read high WORD   VME buffer\n");
}

void ena1190(DWORD vmeBase)
{
   lrs1190_enable(vmeBase);
}

void dis1190(DWORD vmeBase)
{
   lrs1190_disable(vmeBase);
}

void res1190(DWORD vmeBase)
{
   lrs1190_reset(vmeBase);
}

void cn1190(DWORD vmeBase)
{
   INT count;
   lrs1190_count(vmeBase, &count);
   printf("Inline counter %i\n", count);
   LRS1190_COUNT(vmeBase, &count);
   printf("Macro  counter %i\n", count);
}

void rd1190l(DWORD vmeBase, DWORD r)
{
   DWORD dd[1024], *pdd;
   INT i, buf_counter;

   for (i = 0; i < 1024; i++)
      dd[i] = 0;
   pdd = &dd[0];
   lrs1190_disable(vmeBase);
   lrs1190_count(vmeBase, &buf_counter);
   printf("counter %i\n", buf_counter);
   lrs1190_readl2(vmeBase, (WORD **) & pdd, r);
   lrs1190_enable(vmeBase);
   for (i = 0; i < buf_counter; i++)
      printf("%i-> %8.8x\n", i, dd[i]);
}

void rd1190h(DWORD vmeBase, DWORD r)
{
   DWORD dd[1024], *pdd;
   INT i, buf_counter;

   for (i = 0; i < 1024; i++)
      dd[i] = 0;
   pdd = &dd[0];
   lrs1190_disable(vmeBase);
   lrs1190_count(vmeBase, &buf_counter);
   printf("counter %i\n", buf_counter);
   lrs1190_readh2(vmeBase, (WORD **) & pdd, r);
   lrs1190_enable(vmeBase);
   for (i = 0; i < buf_counter; i++)
      printf("%i-> %8.8x\n", i, dd[i]);
}

void rd1190(DWORD vmeBase, DWORD r)
{
   DWORD dd[1024], *pdd;
   INT i, buf_counter;

   for (i = 0; i < 1024; i++)
      dd[i] = 0;
   pdd = &dd[0];
   lrs1190_disable(vmeBase);
   lrs1190_count(vmeBase, &buf_counter);
   printf("counter %i\n", buf_counter);
   lrs1190_readi4(vmeBase, &pdd, r);
   lrs1190_enable(vmeBase);
   for (i = 0; i < buf_counter; i++)
      printf("%i-> %8.8x\n", i, dd[i]);
}
