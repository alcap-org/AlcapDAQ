#ifndef __PLX_ERROR_H
#define __PLX_ERROR_H

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
 *      PlxError.h
 *
 * Description:
 *
 *      This file defines all the error codes for PLX PCI SDK.
 *
 * Revision:
 *
 *      01-30-01 : PCI SDK v3.20
 *
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif




/******************************************
*             Definitions
******************************************/
#define API_RETURN_CODE_STARTS              0x200   /* Starting return code */


/* API Return Code Values */
typedef enum _RETURN_CODE
{
    ApiSuccess = API_RETURN_CODE_STARTS,
    ApiFailed,
    ApiAccessDenied,
    ApiDmaChannelUnavailable,
    ApiDmaChannelInvalid,
    ApiDmaChannelTypeError,
    ApiDmaInProgress,
    ApiDmaDone,
    ApiDmaPaused,
    ApiDmaNotPaused,
    ApiDmaCommandInvalid,
    ApiDmaManReady,
    ApiDmaManNotReady,
    ApiDmaInvalidChannelPriority,
    ApiDmaManCorrupted,
    ApiDmaInvalidElementIndex,
    ApiDmaNoMoreElements,
    ApiDmaSglInvalid,
    ApiDmaSglQueueFull,
    ApiNullParam,
    ApiInvalidBusIndex,
    ApiUnsupportedFunction,
    ApiInvalidPciSpace,
    ApiInvalidIopSpace,
    ApiInvalidSize,
    ApiInvalidAddress,
    ApiInvalidAccessType,
    ApiInvalidIndex,
    ApiMuNotReady,
    ApiMuFifoEmpty,
    ApiMuFifoFull,
    ApiInvalidRegister,
    ApiDoorbellClearFailed,
    ApiInvalidUserPin,
    ApiInvalidUserState,
    ApiEepromNotPresent,
    ApiEepromTypeNotSupported,
    ApiEepromBlank,
    ApiConfigAccessFailed,
    ApiInvalidDeviceInfo,
    ApiNoActiveDriver,
    ApiInsufficientResources,
    ApiObjectAlreadyAllocated,
    ApiAlreadyInitialized,
    ApiNotInitialized,
    ApiBadConfigRegEndianMode,
    ApiInvalidPowerState,
    ApiPowerDown,
    ApiFlybyNotSupported,
    ApiNotSupportThisChannel,
    ApiNoAction,
    ApiHSNotSupported,
    ApiVPDNotSupported,
    ApiVpdNotEnabled,
    ApiNoMoreCap,
    ApiInvalidOffset,
    ApiBadPinDirection,
    ApiPciTimeout,
    ApiDmaChannelClosed,
    ApiDmaChannelError,
    ApiInvalidHandle,
    ApiBufferNotReady,
    ApiInvalidData,
    ApiDoNothing,
    ApiDmaSglBuildFailed,
    ApiPMNotSupported,
    ApiLastError            /* do not add API errors below this line */
} RETURN_CODE, *PRETURN_CODE;



#ifdef __cplusplus
}
#endif

#endif
