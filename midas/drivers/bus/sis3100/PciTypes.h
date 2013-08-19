#ifndef __PCITYPES_H
#define __PCITYPES_H

/*******************************************************************************
 * Copyright (c) 2001 PLX Technology, Inc.
 * 
 * PLX Technology Inc. licenses this software under specific terms and
 * conditions.  Use of any of the software or derviatives thereof in any
 * product without a PLX Technology chip is strictly prohibited. 
 * 
 * PLX Technology, Inc. provides this software AS IS, WITHOUT ANY WARRANTY,
 * EXPRESS OR IMPLIED, INCLUDING, WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  PLX makes no guarantee
 * or representations regarding the use of, or the results of the use of,
 * the software and documentation in terms of correctness, accuracy,
 * reliability, currentness, or otherwise; and you rely on the software,
 * documentation and results solely at your own risk.
 * 
 * IN NO EVENT SHALL PLX BE LIABLE FOR ANY LOSS OF USE, LOSS OF BUSINESS,
 * LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES
 * OF ANY KIND.  IN NO EVENT SHALL PLX'S TOTAL LIABILITY EXCEED THE SUM
 * PAID TO PLX FOR THE PRODUCT LICENSED HEREUNDER.
 * 
 ******************************************************************************/

/******************************************************************************
 *
 * File Name:
 *
 *      PciTypes.h
 *
 * Description:
 *
 *      This file defines the basic types available to the PCI code.
 *
 * Revision:
 *
 *      01-30-01 : PCI SDK v3.20
 *
 ******************************************************************************/


#include "PlxError.h"


#if defined(PLX_DRIVER)
    #if defined(WDM_DRIVER)
        #include <wdm.h>        /* cannot be used in API */
    #else
        #include <ntddk.h>      /* cannot be used in API */
    #endif
#else
    #include <wtypes.h>         /* access same basic types used in Win32 */
#endif

#ifdef __cplusplus
extern "C" {
#endif


/******************************************
 * Basic Type Definitions
 ******************************************/
#if !defined(VOID)
    typedef void                    VOID, *PVOID;
#endif

#if !defined(S8)
    typedef signed char             S8, *PS8;
#endif

#if !defined(U8)
    typedef unsigned char           U8, *PU8;
#endif

#if !defined(S16)
    typedef signed short            S16, *PS16;
#endif

#if !defined(U16)
    typedef unsigned short          U16, *PU16;
#endif

#if !defined(S32)
    typedef signed long             S32, *PS32;
#endif

#if !defined(U32)
    typedef unsigned long           U32, *PU32;
#endif
/* mki, 16.01.02
#if !defined(LONGLONG)
    typedef signed _int64           LONGLONG;
#endif

#if !defined(ULONGLONG)
    typedef unsigned _int64         ULONGLONG;
#endif
*/
#if !defined(S64)
    typedef union _S64
    {
        struct
        {
            U32  LowPart;
            S32  HighPart;
        }u;

        LONGLONG QuadPart;
    } S64;
#endif

#if !defined(U64)
    typedef union _U64
    {
        struct
        {
            U32  LowPart;
            U32  HighPart;
        }u;

        ULONGLONG QuadPart;
    } U64;
#endif




/******************************************
 * PCI SDK Defined Structures
 ******************************************/
/* Device Location Structure */
typedef struct _DEVICE_LOCATION
{
    U32 DeviceId;
    U32 VendorId;
    U32 BusNumber;
    U32 SlotNumber;
    U8  SerialNumber[16];
} DEVICE_LOCATION, *PDEVICE_LOCATION;

/* Virtual Addresses Structure */
typedef struct _VIRTUAL_ADDRESSES
{
    U32 Va0;
    U32 Va1;
    U32 Va2;
    U32 Va3;
    U32 Va4;
    U32 Va5;
    U32 VaRom;
} VIRTUAL_ADDRESSES, *PVIRTUAL_ADDRESSES;

/* PCI Memory  Structure*/
typedef struct _PCI_MEMORY
{
    U32 UserAddr;
    U32 PhysicalAddr;
    U32 Size;
} PCI_MEMORY, *PPCI_MEMORY;



#ifdef __cplusplus
}
#endif

#endif
