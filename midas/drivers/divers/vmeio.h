/*-----------------------------------------------------------------------------
 * Copyright (c) 1996      TRIUMF Data Acquistion Group
 * Please leave this header in any reproduction of that distribution
 * 
 * TRIUMF Data Acquisition Group, 4004 Wesbrook Mall, Vancouver, B.C. V6T 2A3
 * Email: online@triumf.ca           Tel: (604) 222-1047  Fax: (604) 222-1074
 *        amaudruz@triumf.ca
 * ----------------------------------------------------------------------------
 *  
 * Description	: Header file for VMEIO triumf board
 *
 * Author:  Pierre-Andre Amaudruz Data Acquisition Group
   $Log: vmeio.h,v $
   Revision 1.1.1.1  2005/06/20 23:37:09  mucap
   Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
   (Next, I'll commit our local customizations.)

   Revision 1.2  2004/01/08 07:51:21  midas
   Changed indentation

   Revision 1.1  1999/12/20 10:18:23  midas
   Reorganized driver directory structure

   Revision 1.3  1999/09/29 21:06:33  pierre
   - was missing the cvs header

 *---------------------------------------------------------------------------*/
#ifndef VMEIO_INCLUDE
#define VMEIO_INCLUDE
#include "stdio.h"
#include "string.h"

#ifdef OS_VXWORKS
#include "vme.h"
#include "vxWorks.h"
#include "intLib.h"
#include "sys/fcntlcom.h"
#ifdef PPCxxx
#include "arch/ppc/ivPpc.h"
#else
#include "arch/mc68k/ivMc68k.h"
#endif
#include "taskLib.h"
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

#ifdef PPCxxx
#define A32D24	       0xfa000000       /* A32D24 camac base address */
#else
#define A32D24	       0xf0000000       /* A32D24 camac base address */
#endif

#define ENABLE_INT      0x00
#define SET_INT_SYNC    0x01
#define SET_PULSE       0x02
#define WRITE_PULSE     0x03
#define WRITE_LATCH     0x04
#define READ_SYNC       0x05
#define READ_ASYNC      0x06
#define READ_STATUS     0x07
#define CLEAR_VMEIO     0x07

#define VMEIO_VECT_BASE 0x7f

void myStub(void);
void vmeio(void);
void vmeio_intsync_set(DWORD base_adr, DWORD pattern);
void vmeio_pulse_set(const DWORD base_adr, DWORD pattern);
void vmeio_sync_read(const DWORD base_adr, DWORD * data);
void vmeio_async_read(const DWORD base_adr, DWORD * data);
void vmeio_pulse_write(const DWORD base_adr, DWORD data);
void vmeio_latch_write(const DWORD base_adr, DWORD data);
void vmeio_clear(const DWORD base_adr);
void vmeio_status_read(const DWORD base_adr, DWORD * data);
void vmeio_int_clear(const DWORD base_adr);
void vmeio_int_enable(const DWORD base_adr, int intnum);
void vmeio_int_disable(const DWORD base_adr, int intnum);
void vmeio_int_attach(const DWORD base_adr, DWORD base_vect, int intnum,
                      void (*isr) (void));
void vmeio_int_detach(const DWORD base_adr, DWORD base_vect, int intnum);

#endif
