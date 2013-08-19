#ifndef __PCIAPI_H
#define __PCIAPI_H

/*******************************************************************************
 * Copyright (c) 2001 PLX Technology, Inc.
 * 
 * PLX Technology Inc. licenses this software under specific terms and
 * conditions.  Use of any of the software or derviatives thereof any
 * product witha PLX Technology chip is strictly prohibited. 
 * 
 * PLX Technology, Inc. provides this software AS IS, WITHANY WARRANTY,
 * EXPRESS OR IMPLIED, INCLUDING, WITHLIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  PLX makes no guarantee
 * or representations regarding the use of, or the results of the use of,
 * the software and documentation terms of correctness, accuracy,
 * reliability, currentness, or otherwise; and you rely on the software,
 * documentation and results solely at your own risk.
 * 
 * NO EVENT SHALL PLX BE LIABLE FOR ANY LOSS OF USE, LOSS OF BUSINESS,
 * LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES
 * OF ANY KIND.  NO EVENT SHALL PLX'S TOTAL LIABILITY EXCEED THE SUM
 * PAID TO PLX FOR THE PRODUCT LICENSED HEREUNDER.
 * 
 ******************************************************************************/

/******************************************************************************
 *
 * File Name:
 *
 *     PciApi.h
 *
 * Description:
 *
 *     This file contains all the PCI API function prototypes.
 *
 * Revision:
 *
 *     01-30-01 : PCI SDK v3.20
 *
 ******************************************************************************/


#include <wtypes.h>
#include "PlxTypes.h"


#ifdef __cplusplus
extern "C" {
#endif


// DLL support
#ifndef EXPORT
    #define EXPORT __declspec(dllexport)
#endif



/******************************************
* Miscellaneous Functions
******************************************/
RETURN_CODE EXPORT
PlxSdkVersion(
    U8 *VersionMajor,
    U8 *VersionMinor,
    U8 *VersionRevision
    );

RETURN_CODE EXPORT
PlxDriverVersion(
    HANDLE  hDevice,
    U8     *VersionMajor,
    U8     *VersionMinor,
    U8     *VersionRevision
    );

RETURN_CODE EXPORT
PlxChipTypeGet(
    HANDLE  hDevice,
    U32    *pChipType,
    U8     *pRevision
    );

VOID EXPORT
PlxPciBoardReset(
    HANDLE hDevice
    );

RETURN_CODE EXPORT
PlxPciCommonBufferGet(
    HANDLE      hDevice,
    PCI_MEMORY *pMemoryInfo
    );


/******************************************
* PLX Device Management Functions
******************************************/
RETURN_CODE EXPORT
PlxPciDeviceOpen(
    DEVICE_LOCATION *pDevice,
    HANDLE          *pDrvHandle
    );

RETURN_CODE EXPORT
PlxPciDeviceClose(
    HANDLE hDevice
    );

RETURN_CODE EXPORT
PlxPciDeviceFind(
    DEVICE_LOCATION *device,
    U32             *requestLimit
    );

RETURN_CODE EXPORT 
PlxPciBusSearch(
    DEVICE_LOCATION *pDevData
    );

RETURN_CODE EXPORT
PlxPciBaseAddressesGet(
    HANDLE             hDevice,
    VIRTUAL_ADDRESSES *virtAddr
    );

RETURN_CODE EXPORT
PlxPciBarRangeGet(
    HANDLE  hDevice,
    U32     barRegisterNumber,
    U32    *data
    );

    
/******************************************
* Register Access Functions
******************************************/
U32 EXPORT
PlxPciConfigRegisterRead(
    U32          bus,
    U32          slot,
    U32          registerNumber,
    RETURN_CODE *pReturnCode
    );

RETURN_CODE EXPORT
PlxPciConfigRegisterWrite(
    U32  bus,
    U32  slot,
    U32  registerNumber,
    U32 *data
    );

RETURN_CODE EXPORT
PlxPciConfigRegisterReadAll(
    U32  bus,
    U32  slot,
    U32 *buffer
    );

U32 EXPORT
PlxRegisterRead(
    HANDLE       hDevice,
    U32          registerOffset,
    RETURN_CODE *pReturnCode
    );

RETURN_CODE EXPORT
PlxRegisterWrite(
    HANDLE hDevice,
    U32    registerOffset,
    U32    data
    );

RETURN_CODE EXPORT
PlxRegisterReadAll(
    HANDLE  hDevice,
    U32     startOffset,
    U32     registerCount,
    U32    *buffer
    );

U32 EXPORT
PlxRegisterMailboxRead(
    HANDLE       hDevice,
    MAILBOX_ID   mailboxId,
    RETURN_CODE *pReturnCode
    );

RETURN_CODE EXPORT
PlxRegisterMailboxWrite(
    HANDLE     hDevice,
    MAILBOX_ID mailboxId,
    U32        data
    );

U32 EXPORT 
PlxRegisterDoorbellRead(
    HANDLE       hDevice,
    RETURN_CODE *pReturnCode
    );
        
RETURN_CODE EXPORT
PlxRegisterDoorbellSet(
    HANDLE hDevice,
    U32    data
    );


/******************************************
* Interrupt Support Functions
******************************************/
RETURN_CODE EXPORT
PlxIntrEnable(
    HANDLE    hDevice,
    PLX_INTR *plxIntr
    );

RETURN_CODE EXPORT
PlxIntrDisable(
    HANDLE    hDevice,
    PLX_INTR *plxIntr
    );

RETURN_CODE EXPORT
PlxIntrAttach(
    HANDLE    hDevice,
    PLX_INTR  intrTypes, 
    HANDLE   *pEventHdl
    );

RETURN_CODE EXPORT
PlxIntrStatusGet(
    HANDLE    hDevice,
    PLX_INTR *pPlxIntr
    );

U32 EXPORT
PlxPciAbortAddrRead(
    HANDLE        hDevice,
    RETURN_CODE *pReturnCode
    );


/******************************************
* Bus Memory and I/O Functions
******************************************/
RETURN_CODE EXPORT
PlxBusIopRead(
    HANDLE       hDevice,
    IOP_SPACE    iopSpace,
    U32          address,
    BOOLEAN      remapAddress,
    U32         *destination,
    U32          transferSize,
    ACCESS_TYPE  accessType
    );

RETURN_CODE EXPORT
PlxBusIopWrite(
    HANDLE       hDevice,
    IOP_SPACE    iopSpace,
    U32          address,
    BOOLEAN      remapAddress,
    U32         *source,
    U32          transferSize,
    ACCESS_TYPE  accessType
    );

RETURN_CODE EXPORT 
PlxIoPortRead(
    HANDLE      hDevice,
    U32         address,
    ACCESS_TYPE bits,
    PVOID       pOutData
    );

RETURN_CODE EXPORT
PlxIoPortWrite(
    HANDLE      hDevice,
    U32         address,
    ACCESS_TYPE bits,
    PVOID       pValue
    );


/******************************************
* Serial EEPROM Access Functions
******************************************/
BOOLEAN EXPORT 
PlxSerialEepromPresent(
    HANDLE       hDevice,
    RETURN_CODE *pReturnCode
    );

RETURN_CODE EXPORT
PlxSerialEepromRead(
    HANDLE       hDevice,
    EEPROM_TYPE  eepromType,
    U32         *buffer,
    U32          size
    );

RETURN_CODE EXPORT
PlxSerialEepromWrite(
    HANDLE       hDevice,
    EEPROM_TYPE  eepromType,
    U32         *buffer,
    U32          size
    );


/******************************************
* DMA Functions
******************************************/
RETURN_CODE EXPORT
PlxDmaControl(
    HANDLE      hDevice,
    DMA_CHANNEL dmaChannel, 
    DMA_COMMAND dmaCommand
    );

RETURN_CODE EXPORT
PlxDmaStatus(
    HANDLE      hDevice,
    DMA_CHANNEL dmaChannel
    );


/******************************************
* Block DMA Functions
******************************************/
RETURN_CODE EXPORT
PlxDmaBlockChannelOpen(
    HANDLE            hDevice,
    DMA_CHANNEL       dmaChannel, 
    DMA_CHANNEL_DESC *dmaChannelDesc
    );

RETURN_CODE EXPORT
PlxDmaBlockTransfer(
    HANDLE                hDevice,
    DMA_CHANNEL           dmaChannel,
    DMA_TRANSFER_ELEMENT *dmaData,
    BOOLEAN               returnImmediate
    );

RETURN_CODE EXPORT
PlxDmaBlockTransferRestart(
    HANDLE      hDevice,
    DMA_CHANNEL dmaChannel,
    U32         transferSize,
    BOOLEAN     returnImmediate
    );

RETURN_CODE EXPORT 
PlxDmaBlockChannelClose(
    HANDLE      hDevice,
    DMA_CHANNEL dmaChannel
    );


/******************************************
* SGL DMA Functions
******************************************/
RETURN_CODE EXPORT
PlxDmaSglChannelOpen(
    HANDLE            hDevice,
    DMA_CHANNEL       dmaChannel, 
    DMA_CHANNEL_DESC *dmaChannelDesc
    );

RETURN_CODE EXPORT
PlxDmaSglTransfer(
    HANDLE                hDevice,
    DMA_CHANNEL           dmaChannel,
    DMA_TRANSFER_ELEMENT *dmaData,
    BOOLEAN               returnImmediate
    );

RETURN_CODE EXPORT 
PlxDmaSglChannelClose(
    HANDLE      hDevice,
    DMA_CHANNEL dmaChannel
    );


/******************************************
* Shuttle DMA Functions
******************************************/
RETURN_CODE EXPORT
PlxDmaShuttleChannelOpen(
    HANDLE            hDevice,
    DMA_CHANNEL       dmaChannel, 
    DMA_CHANNEL_DESC *dmaChannelDesc
    );

RETURN_CODE EXPORT
PlxDmaShuttleTransfer(
    HANDLE                hDevice,
    DMA_CHANNEL           dmaChannel,
    DMA_TRANSFER_ELEMENT *dmaData
    );

RETURN_CODE EXPORT 
PlxDmaShuttleChannelClose(
    HANDLE      hDevice,
    DMA_CHANNEL dmaChannel
    );


/******************************************
* Messaging Unit Functions
******************************************/
RETURN_CODE EXPORT
PlxMuInboundPortRead(
    HANDLE  hDevice,
    U32    *framePointer
    );

RETURN_CODE EXPORT
PlxMuInboundPortWrite(
    HANDLE  hDevice,
    U32    *framePointer
    );

RETURN_CODE EXPORT
PlxMuOutboundPortRead(
    HANDLE  hDevice,
    U32    *framePointer
    );

RETURN_CODE EXPORT
PlxMuOutboundPortWrite(
    HANDLE  hDevice,
    U32    *framePointer
    );

U32 EXPORT
PlxMuHostOutboundIndexRead(
    HANDLE       hDevice,
    RETURN_CODE *pReturnCode
    );

RETURN_CODE EXPORT
PlxMuHostOutboundIndexWrite(
    HANDLE hDevice,
    U32    indexValue
    );


/******************************************
* Power Management Functions
******************************************/
PLX_POWER_LEVEL EXPORT
PlxPowerLevelGet(
    HANDLE       hDevice,
    RETURN_CODE *pReturnCode
    );

RETURN_CODE EXPORT
PlxPowerLevelSet(
    HANDLE          hDevice,
    PLX_POWER_LEVEL plxPowerLevel
    );

U8 EXPORT
PlxPmIdRead(
    HANDLE       hDevice,
    RETURN_CODE *pReturnCode
    );

U8 EXPORT 
PlxPmNcpRead(
    HANDLE       hDevice,
    RETURN_CODE *pReturnCode
    );


/******************************************
* Hot Swap Functions
******************************************/
U8 EXPORT
PlxHotSwapIdRead(
    HANDLE       hDevice,
    RETURN_CODE *pReturnCode
    );

U8 EXPORT
PlxHotSwapNcpRead(
    HANDLE       hDevice,
    RETURN_CODE *pReturnCode
    );

U8 EXPORT 
PlxHotSwapStatus(
    HANDLE       hDevice,
    RETURN_CODE *pReturnCode
    );


/******************************************
* VPD Functions
******************************************/
U8 EXPORT
PlxVpdIdRead(
    HANDLE       hDevice,
    RETURN_CODE *pReturnCode
    );

U8 EXPORT
PlxVpdNcpRead(
    HANDLE       hDevice,
    RETURN_CODE *pReturnCode
    );

U32 EXPORT 
PlxVpdRead(
    HANDLE       hDevice,
    U32          offset,
    RETURN_CODE *pReturnCode
    );

RETURN_CODE EXPORT
PlxVpdWrite(
    HANDLE hDevice,
    U32    offset,
    U32    vpdData
    );


/******************************************
* USER Pins Fuctions
******************************************/
PLX_PIN_STATE EXPORT
PlxUserRead(
    HANDLE       hDevice,
    USER_PIN     userPin,
    RETURN_CODE *pReturnCode
    );

RETURN_CODE EXPORT
PlxUserWrite(
    HANDLE        hDevice,
    USER_PIN      userPin,
    PLX_PIN_STATE pinState
    );



#ifdef __cplusplus
}
#endif

#endif
