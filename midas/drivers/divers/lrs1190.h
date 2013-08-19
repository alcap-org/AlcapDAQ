/*-----------------------------------------------------------------------------
 * Copyright (c) 1996      TRIUMF Data Acquistion Group
 * Please leave this header in any reproduction of that distribution
 * 
 * TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 * Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * ----------------------------------------------------------------------------
 *  
 * Description	: Header file for Macros support to the LeCroy 1190
 *		  dual port memory buffer.
 *
 * Author:  Pierre-Andre Amaudruz Data Acquisition Group
 
   $Log: lrs1190.h,v $
   Revision 1.1.1.1  2005/06/20 23:37:09  mucap
   Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
   (Next, I'll commit our local customizations.)

   Revision 1.2  2004/01/08 07:51:21  midas
   Changed indentation

   Revision 1.1  1999/12/20 10:18:23  midas
   Reorganized driver directory structure

   Revision 1.2  1998/10/12 12:18:57  midas
   Added Log tag in header


 * Revision 1.0  1996/ Pierre	 Initial revision
 *
 *  VME_RESET_1190 (base);		! Reset 
 *  VME_ENABLE_1190 (base);		! Enable Front Panel Port (FPP)
 *  VME_DISABLE_1190 (base);		! Enable VME port (disable FPP)
 *  VME_COUNT_1190 (base, d);		! Read entry counter
 *  VME_READI4_1190 (base, b, r);	! Read r entries of 32bit word size
 *  VME_READL2_1190 (base, d, r);	! Read r entries of the low port in 32bit data
 *  VME_READH2_1190 (base, d, r);	! Read r entries of the high port in 32 bit data
 *  VME_READL2_TONY_1190 (base, d, r);	! Read r entries of the low port in 16bit data 
 *---------------------------------------------------------------------------*/
#ifndef _vme_h_
#define _vme_h_
#include "vxWorks.h"
#include "vme.h"
#endif

#define A32D24	       0xf0000000       /* A32D24 access */

/*------------------------------------------------------------------------------------*/

#define VME_ENABLE_1190(_vmebase){\
				    {\
				       DWORD _ll, _local;\
				       _ll = _vmebase;\
				       _local = ((_ll) | 0x8002) | A32D24;\
				       *(WORD *)_local = 0x1;\
				   }\
				}

#define VME_DISABLE_1190(_vmebase){\
				    {\
				       DWORD _ll, _local;\
				      _ll = _vmebase;\
				       _local = ((_ll) | 0x8002) | A32D24;\
				       *(WORD *)_local = 0x0;\
				   }\
				}

#define VME_RESET_1190(_vmebase){\
				   {\
				      static DWORD _ll, _dummy, _local;\
				      _ll = _vmebase;\
				      _local = (_ll) | A32D24;\
				      _dummy = *(WORD *) _local;\
				  }\
			       }

#define VME_COUNT_1190(_vmebase, _d){\
					{\
					 DWORD _ll, *_pcount;\
					 _ll = _vmebase;\
					 _pcount = (DWORD *) (((_ll) | 0x8000) | A32D24);\
					 *((WORD *) _d) = *(WORD *) _pcount;\
				       }\
				    }


#define VME_READI4_1190(_vmebase,_d,_r){\
					{\
					 DWORD _ll, *_local, __r, _count;\
					 DWORD *_pcount;\
					 __r = _r;\
					 _ll = _vmebase;\
					 _pcount = (DWORD *) (((_ll) | 0x8000) | A32D24);\
					 _count = *(WORD *) _pcount;\
					 if (_count <= __r)\
					   __r = _count;\
					 _local = (DWORD *)((_ll)  | A32D24);\
					 while (__r > 0) {\
						*_d++ = *_local++;\
						__r--;}\
					 }\
				      }

#define VME_READL2_1190(_vmebase,_d,_r){\
					{\
					 DWORD _ll, *_pcount, *_local;\
					 WORD _count, __r;\
					 __r = _r;\
					 _ll = _vmebase;\
					 _pcount = (DWORD *) (((_ll) | 0x8000) | A32D24);\
					 _count = *(WORD *) _pcount;\
					 if (_count <= __r)\
					   __r = _count;\
					 _local = (DWORD *)((_ll)  | A32D24);\
					 while (__r > 0) {\
						*(WORD *) _d = *(WORD *)_local;\
						((WORD *)_d)++;\
						(_local)++;\
						__r--;}\
					 }\
				      }

#define VME_READH2_1190(_vmebase,_d,_r){\
					{\
					 DWORD _ll, *_pcount, *_local;\
					 WORD _count, __r;\
					 __r = _r;\
					 _ll = _vmebase;\
					 _pcount = (DWORD *) (((_ll) | 0x8000) | A32D24);\
					 _count = *(WORD *) _pcount;\
					 if (_count <= __r)\
					   __r = _count;\
					 _local = (DWORD *)((_ll)  | A32D24);\
					 while (__r > 0) {\
						*(WORD *) _d = *(++((WORD *)_local));\
						((WORD *)_d)++;\
						((WORD *)_local)++;\
						__r--;}\
					 }\
				      }

#define VME_READL2_TONY_1190(_vmebase,_d,_r){\
					{\
					 DWORD _ll, *_pcount, *_local, __r, _count;\
					 __r = _r;\
					 _ll = _vmebase;\
					 _pcount = (DWORD *) (((_ll) | 0x8000) | A32D24);\
					 _count = *(WORD *) _pcount;\
					 if (_count <= __r)\
					   __r = _count;\
					 _local = (DWORD *)((_ll)  | A32D24);\
 					 *(WORD *) _d = __r;\
					 ((WORD *)_d)++;\
					 while (__r > 0) {\
						*(WORD *) _d = *(WORD *)_local;\
						((WORD *)_d)++;\
						(_local)++;\
						__r--;}\
					 }\
				      }
/*------------------------------------------------------------------------------------*/
