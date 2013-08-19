#ifndef __PLX_API_H
#define __PLX_API_H

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
 *      PlxApi.h
 *
 * Description:
 *
 *      The main PLX API include file
 *
 * Revision:
 *
 *      01-30-01 : PCI SDK v3.20
 *
 ******************************************************************************/




/**********************************************
*               Error Checks
**********************************************/
#if !defined(IOP_CODE) && !defined(PCI_CODE)
    #error ERROR: Either IOP_CODE or PCI_CODE must be defined.
#endif

#if !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
    #error ERROR: Either LITTLE_ENDIAN or BIG_ENDIAN must be defined.
#endif




/**********************************************
*              Include Files
**********************************************/
#include "Plx.h"
#include "PlxTypes.h"

#if defined(IOP_CODE)
    #include "IopApi.h"
#endif

#if defined(PCI_CODE)
    #include "PciApi.h"
#endif



#endif
