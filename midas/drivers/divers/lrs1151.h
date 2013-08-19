/*-----------------------------------------------------------------------------
 * Copyright (c) 1996      TRIUMF Data Acquistion Group
 * Please leave this header in any reproduction of that distribution
 * 
 * TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 * Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *         amaudruz@triumf.ca
 * ----------------------------------------------------------------------------
 *  
 * Description	: Header file for Macro support to the LeCroy 1151
 *                VME 16 channels scalers 
 *
 * Author:  Pierre-Andre Amaudruz Data Acquisition Group
 * 
  $Log: lrs1151.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/01/08 07:51:21  midas
  Changed indentation

  Revision 1.1  1999/12/20 10:18:23  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:56  midas
  Added Log tag in header

 * Revision 1.0  1996/ Pierre	 Initial revision
 *
 *  VME_CLEAR_1151 (base);		Clear all 16 channels
 *  VME_READ_1151 (base, d, r); 	Read r channels starting from channel 0
  *---------------------------------------------------------------------------*/
#ifndef _vme_h_
#define _vme_h_
#include "vxWorks.h"
#include "vme.h"
#endif

#define A32D24	       0xf0000000       /* A32D16 access */

/*------------------------------------------------------------------------------------*/
#define VME_CLEAR_1151(_vmebase){\
				   {\
				      static DWORD _ll, _dummy, *_local, __r=16;\
				      _ll = _vmebase;\
				      _local =(DWORD *)( ((_ll << 8) | 0x40) | A32D24);\
				      while (__r > 0) {\
						_dummy = *_local;\
						_local++;\
						__r--;}\
				  }\
			       }

#define VME_READ_1151(_vmebase,_d,_r){\
					{\
					   DWORD _ll, *_local, __r;\
					   _ll = _vmebase;\
					   _local = (DWORD *) (((_ll << 8) | 0x80) | A32D24);\
					   __r =_r;\
					   if (__r > 16) __r = 16;\
					   while (__r > 0) {\
						*_d++ = *_local++;\
						__r--;}\
				       }\
				    }
/*------------------------------------------------------------------------------------*/
