#ifndef __PLX_H
#define __PLX_H

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
 *      Plx.h
 *
 * Description:
 *
 *      This file contains definitions that are common to all PCI SDK code.
 *
 * Revision:
 *
 *      01-30-01 : PCI SDK v3.20
 *
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif




/**********************************************
*               Definitions
**********************************************/
/* Define the max number of PCI buses and slots */
#define MAX_PCI_BUS                      32
#define MAX_PCI_DEV                      32

/* Define the valid vendor IDs */
#define NO_VENDOR_ID                     0xFFFF
#define PLX_VENDOR_ID                    0x10B5

/* Define the valid device IDs */
#define NO_DEVICE_ID                     0xFFFF
#define PLX_9050_DEVICE_ID               0x9050
#define PLX_9030_DEVICE_ID               0x9030
#define PLX_9060_DEVICE_ID               0x9060
#define PLX_9080_DEVICE_ID               0x9080
#define PLX_9054_DEVICE_ID               0x9054
#define PLX_480_DEVICE_ID                0x0480

/* Define the evaluation board device IDs */
#define PLX_9080RDK_960_DEVICE_ID        0x0960
#define PLX_9080RDK_401B_DEVICE_ID       0x0401
#define PLX_9080RDK_860_DEVICE_ID        0x0860
#define PLX_9054RDK_860_DEVICE_ID        0x1860
#define PLX_9054RDK_LITE_DEVICE_ID       0x5406
#define PLX_CPCI9054RDK_860_DEVICE_ID    0xC860
#define PLX_9656RDK_LITE_DEVICE_ID       0x5601
#define PLX_9656RDK_860_DEVICE_ID        0x5602
#define PLX_IOP480RDK_DEVICE_ID          0x0480
#define PLX_9030RDK_LITE_DEVICE_ID       0x3001
#define PLX_CPCI9030RDK_LITE_DEVICE_ID   0x30c1
#define PLX_9050RDK_LITE_DEVICE_ID       0x9050

// New Capabilities flags
#define CAPABILITY_POWER_MANAGEMENT      (1 << 0)
#define CAPABILITY_HOT_SWAP              (1 << 1)
#define CAPABILITY_VPD                   (1 << 2)

/* The local CPU will reset when this value is written to the doorbell */
#define PLX_RESET_EMBED_INT              ((unsigned long)(1 << 31))

/* Define a large value for a signal to the driver */
#define FIND_AMOUNT_MATCHED              80001

/* Define -1 values for the different types */
#define MINUS_ONE_LONG                   0xFFFFFFFFL
#define MINUS_ONE_SHORT                  0xFFFF
#define MINUS_ONE_CHAR                   0xFF

/*
 * The value for BLOCK_SIZE is standard for IBM image files and therfore 
 * should not be modified.
 */
#define BLOCK_SIZE                       512



#ifdef __cplusplus
}
#endif

#endif
