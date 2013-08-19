#ifndef __PLXTYPES_H
#define __PLXTYPES_H

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
 *      PlxTypes.h
 *
 * Description:
 *
 *      This file defines the basic types available to the PCI SDK.
 *
 * Revision:
 *
 *      01-30-01 : PCI SDK v3.20
 *
 ******************************************************************************/


#if defined(IOP_CODE)
    #include "IopTypes.h"
#elif defined(PCI_CODE)
    #include "PciTypes.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif




/* Assertion constants */
#define AssertBLAST          0
#define EOTAsserted          1

/* BLINK duration values. The larger the duration, the slower it blinks */
#define BLINK_SLOW           0x80000
#define BLINK_MEDIUM         0x50000
#define BLINK_FAST           0x30000

/* Hot swap status definitions */
#define HS_LED_ON            0x08
#define HS_BOARD_REMOVED     0x40
#define HS_BOARD_INSERTED    0x80


/* Access Size Type */
typedef enum _ACCESS_TYPE
{
    BitSize8,
    BitSize16,
    BitSize32,
    BitSize64
} ACCESS_TYPE, *PACCESS_TYPE;


/* Various DMA states */
typedef enum _DMA_STATE
{
    DmaStateClosed,
    DmaStateBlock,
    DmaStateSgl,
    DmaStateShuttle,
} DMA_STATE;


/* DMA Channel Definitions */
typedef enum _DMA_CHANNEL
{
    IopChannel0,
    IopChannel1,
    IopChannel2,
    PrimaryPciChannel0,
    PrimaryPciChannel1,
    SecondaryPciChannel0,
    SecondaryPciChannel1
} DMA_CHANNEL, *PDMA_CHANNEL;


/* DMA Transfer Direction Type */
typedef enum _DMA_DIRECTION
{
    IopToIop,
    IopToPrimaryPci,
    PrimaryPciToIop,
    IopToSecondaryPci,
    SecondaryPciToIop,
    PrimaryPciToSecondaryPci,
    SecondaryPciToPrimaryPci
} DMA_DIRECTION, *PDMA_DIRECTION;


/* DMA Command Definitions */
typedef enum _DMA_COMMAND
{
    DmaStart,
    DmaPause,
    DmaResume,
    DmaAbort,
    DmaStatus
} DMA_COMMAND, *PDMA_COMMAND;


/* DMA Channel Priority Definitions */
typedef enum _DMA_CHANNEL_PRIORITY
{
    Channel0Highest,
    Channel1Highest,
    Channel2Highest,
    Channel3Highest,
    Rotational
} DMA_CHANNEL_PRIORITY;


/* Power State Definitions */
typedef enum _PLX_POWER_LEVEL
{
    D0Uninitialized,
    D0,
    D1,
    D2,
    D3Hot,
    D3Cold
} PLX_POWER_LEVEL, *PPLX_POWER_LEVEL;


/* EEPROM Type Definitions */
typedef enum _EEPROM_TYPE
{
    Eeprom93CS46,
    Eeprom93CS56,
    Eeprom93CS66,
    EepromX24012,
    EepromX24022,
    EepromX24042,
    EepromX24162,
    EEPROM_UNSUPPORTED
} EEPROM_TYPE, *PEEPROM_TYPE;


/* FLASH Device Definitions */
typedef enum _FLASH_TYPE
{
    AM29F040,
    AM29LV040B,
    AT49LV040,
    MBM29LV160,
    FLASH_UNSUPPORTED
} FLASH_TYPE, *PFALSH_TYPE;


/* USER Pin Definitions */
typedef enum _USER_PIN
{
    USER0,
    USER1,
    USER2,
    USER3,
    USER4,
    USER5
} USER_PIN, *PUSER_PIN;


/* USER Pin Values */
typedef enum _PLX_PIN_STATE
{
    Inactive,
    Active
} PLX_PIN_STATE, *PPLX_PIN_STATE;


/* Pin Direction Values */
typedef enum _PLX_PIN_DIRECTION
{
    PLX_PIN_INPUT,
    PLX_PIN_OUTPUT
} PLX_PIN_DIRECTION;


/* PCI Space Definitions */
typedef enum _PCI_SPACE
{
    PciMemSpace,
    PciIoSpace
} PCI_SPACE, *PPCI_SPACE;


/* Base Address Register Definitions */
typedef enum _BAR_SPACE
{
    Bar0,
    Bar1,
    Bar2,
    Bar3,
    Bar4,
    Bar5,
    IopExpansionRom
} BAR_SPACE, *PBAR_SPACE;


/* IOP Space Types */
typedef enum _IOP_SPACE
{
    IopSpace0,
    IopSpace1,
    IopSpace2,
    IopSpace3,
    MsLcs0,
    MsLcs1,
    MsLcs2,
    MsLcs3,
    MsDram,
    MsDefault,
    ExpansionRom
} IOP_SPACE, *PIOP_SPACE;


/* Bus Index Types */
typedef enum _BUS_INDEX
{
    PrimaryPciBus,
    SecondaryPciBus
} BUS_INDEX, *PBUS_INDEX;


/* Mailbox ID Definitions */
typedef enum _MAILBOX_ID
{
    MailBox0,
    MailBox1,
    MailBox2,
    MailBox3,
    MailBox4,
    MailBox5,
    MailBox6,
    MailBox7
} MAILBOX_ID, *PMAILBOX_ID;


/* Power Management Data Definitions */
typedef enum _POWER_DATA_SELECT
{
    D0PowerConsumed,
    D1PowerConsumed,
    D2PowerConsumed,
    D3HotPowerConsumed,
    D0PowerDissipated,
    D1PowerDissipated,
    D2PowerDissipated,
    D3PowerDissipated
} POWER_DATA_SELECT, *PPOWER_DATA_SELECT;


/* Power Scale definitions */
typedef enum _POWER_DATA_SCALE
{
    PowerScaleUnknown,
    PowerScaleOneTenth,
    PowerScaleOneHundredth,
    PowerScaleOneThousandth
} POWER_DATA_SCALE, *PPOWER_DATA_SCALE;


/* DMA Channel Descriptor Structure */  
typedef struct _DMA_CHANNEL_DESC 
{
    U32 EnableReadyInput         :1;
    U32 EnableBTERMInput         :1;
    U32 EnableIopBurst           :1;
    U32 EnableWriteInvalidMode   :1;
    U32 EnableDmaEOTPin          :1;
    U32 DmaStopTransferMode      :1;
    U32 HoldIopAddrConst         :1;
    U32 HoldIopSourceAddrConst   :1;
    U32 HoldIopDestAddrConst     :1;
    U32 DemandMode               :1;
    U32 SrcDemandMode            :1;
    U32 DestDemandMode           :1;
    U32 EnableTransferCountClear :1;
    U32 WaitStates               :4;
    U32 IopBusWidth              :2;
    U32 EOTEndLink               :1;
    U32 ValidStopControl         :1;
    U32 ValidModeEnable          :1;
    U32 EnableDualAddressCycles  :1;
    U32 Reserved1                :9;
    U32 TholdForIopWrites        :4;
    U32 TholdForIopReads         :4;
    U32 TholdForPciWrites        :4;
    U32 TholdForPciReads         :4;
    U32 EnableFlybyMode          :1;
    U32 FlybyDirection           :1;
    U32 EnableDoneInt            :1;
    U32 Reserved2                :13;
    DMA_CHANNEL_PRIORITY DmaChannelPriority;
} DMA_CHANNEL_DESC, *PDMA_CHANNEL_DESC;


/* DMA Transfer Element Union Structure */
typedef union _DMA_TRANSFER_ELEMENT
{
    struct
    {
    #if defined(PCI_CODE)
        union
        {
            U32 LowPciAddr;
            U32 UserAddr;
        };
    #else
        U32 LowPciAddr;
    #endif
        U32 IopAddr;
        U32 TransferCount;
    #if defined(LITTLE_ENDIAN)
        U32 PciSglLoc         :1;
        U32 LastSglElement    :1;
        U32 TerminalCountIntr :1;
        U32 IopToPciDma       :1;
        U32 NextSglPtr        :28;
    #elif defined(BIG_ENDIAN)
        U32 NextSglPtr        :28;
        U32 IopToPciDma       :1;
        U32 TerminalCountIntr :1;
        U32 LastSglElement    :1;
        U32 PciSglLoc         :1;
    #endif
    } Pci9080Dma;

    struct
    {
    #if defined(PCI_CODE)
        union
        {
            U32 LowPciAddr;
            U32 UserAddr;
        };
    #else
        U32 LowPciAddr;
    #endif
        U32 IopAddr;
        U32 TransferCount;
    #if defined(LITTLE_ENDIAN)
        U32 PciSglLoc         :1;
        U32 LastSglElement    :1;
        U32 TerminalCountIntr :1;
        U32 IopToPciDma       :1;
        U32 NextSglPtr        :28;
    #elif defined(BIG_ENDIAN)
        U32 NextSglPtr        :28;
        U32 IopToPciDma       :1;
        U32 TerminalCountIntr :1;
        U32 LastSglElement    :1;
        U32 PciSglLoc         :1;
    #endif
        U32 HighPciAddr;
    } Pci9054Dma;

    struct
    {
        U32 TransferCount;
    #if defined(PCI_CODE)
        union
        {
            U32 LowPciAddr;
            U32 UserAddr;
            U32 SourceAddr;
        };
    #else
        union loc1
        {
            U32 LowPciAddr; /* DMA channel 0, 1 */
            U32 SourceAddr; /* DMA channel 2 */
        } Loc1;
    #endif

    #if defined(PCI_CODE)
        union loc2
        {
            U32 IopAddr;    /* DMA channel 0, 1 */
            U32 DestAddr;   /* DMA channel 2 */
        };
    #else
        union loc2
        {
            U32 IopAddr;    /* DMA channel 0, 1 */
            U32 DestAddr;   /* DMA channel 2 */
        } Loc2;
    #endif

    #if defined(LITTLE_ENDIAN)
        U32 PciSglLoc         :1;
        U32 LastSglElement    :1;
        U32 TerminalCountIntr :1;
        U32 IopToPciDma       :1;
        U32 NextSglPtr        :28;
    #elif defined(BIG_ENDIAN)
        U32 NextSglPtr        :28;
        U32 IopToPciDma       :1;
        U32 TerminalCountIntr :1;
        U32 LastSglElement    :1;
        U32 PciSglLoc         :1;
    #endif
        U32 HighPciAddr;
    } Iop480Dma;

    /*
     * The DMA Transfer Element must always start on a 16 byte
     * boundary so the following reserve field ensures this. Total size = 0x30.
     */
    U32 Reserved[12];
} DMA_TRANSFER_ELEMENT, *PDMA_TRANSFER_ELEMENT;


/* DMA SGL Address Data Type */
typedef PDMA_TRANSFER_ELEMENT          SGL_ADDR, *PSGL_ADDR;


/* DMA Manager initialization Parameters Structure */
typedef struct _DMA_PARMS
{
    DMA_CHANNEL           DmaChannel;
    PDMA_TRANSFER_ELEMENT FirstSglElement; 
    PU32                  WaitQueueBase; 
    U32                   NumberOfElements;
} DMA_PARMS, *PDMA_PARMS;


/* PLX Interrupt Structure */   
typedef struct _PLX_INTR
{
    U32 InboundPost      :1;
    U32 OutboundPost     :1;
    U32 OutboundOverflow :1;
    U32 OutboundOption   :1;
    U32 IopDmaChannel0   :1;
    U32 PciDmaChannel0   :1;
    U32 IopDmaChannel1   :1;
    U32 PciDmaChannel1   :1;
    U32 IopDmaChannel2   :1;
    U32 PciDmaChannel2   :1;
    U32 Mailbox0         :1;
    U32 Mailbox1         :1;
    U32 Mailbox2         :1;
    U32 Mailbox3         :1;
    U32 Mailbox4         :1;
    U32 Mailbox5         :1;
    U32 Mailbox6         :1;
    U32 Mailbox7         :1;
    U32 IopDoorbell      :1;
    U32 PciDoorbell      :1;
    U32 SerialPort1      :1;
    U32 SerialPort2      :1;
    U32 BIST             :1;
    U32 PowerManagement  :1;
    U32 PciMainInt       :1;
    U32 IopToPciInt      :1;
    U32 IopMainInt       :1;
    U32 PciAbort         :1;
    U32 PciReset         :1;
    U32 PciPME           :1;
    U32 Enum             :1;
    U32 PciENUM          :1;
    U32 IopBusTimeout    :1;
    U32 AbortLSERR       :1;
    U32 ParityLSERR      :1;
    U32 RetryAbort       :1;
    U32 LocalParityLSERR :1;
    U32 PciSERR          :1;
    U32 IopRefresh       :1;
    U32 PciINTApin       :1;
    U32 IopINTIpin       :1;
    U32 TargetAbort      :1;
    U32 Ch1Abort         :1;
    U32 Ch0Abort         :1;
    U32 DMAbort          :1;
    U32 IopToPciInt_2    :1;
    U32 Reserved         :18;
} PLX_INTR, *PPLX_INTR;


/* PCI bus properties structure */
typedef struct _PCI_BUS_PROP
{
    U32 PciRequestMode           :1;
    U32 DmPciReadMode            :1;
    U32 EnablePciArbiter         :1;
    U32 EnableWriteInvalidMode   :1;
    U32 DmPrefetchLimit          :1;
    U32 PciReadNoWriteMode       :1;
    U32 PciReadWriteFlushMode    :1;
    U32 PciReadNoFlushMode       :1;
    U32 EnableRetryAbort         :1;
    U32 WFifoAlmostFullFlagCount :5;
    U32 DmWriteDelay             :2;
    U32 ReadPrefetchMode         :2;
    U32 IoRemapSelect            :1;
    U32 EnablePciBusMastering    :1;
    U32 EnableMemorySpaceAccess  :1;
    U32 EnableIoSpaceAccess      :1;
    U32 Reserved1                :10;
    U32 ReservedForFutureUse;
} PCI_BUS_PROP, *PPCI_BUS_PROP;


/* PCI Abritration Descriptor Structure */
typedef struct _PCI_ARBIT_DESC
{
    U32 PciHighPriority;
} PCI_ARBIT_DESC, *PPCI_ARBIT_DESC;


/* IOP Bus Properties Structure */
typedef struct _IOP_BUS_PROP
{
    U32 EnableReadyRecover       :1;
    U32 EnableReadyInput         :1;
    U32 EnableBTERMInput         :1;
    U32 DisableReadPrefetch      :1;
    U32 EnableReadPrefetchCount  :1;
    U32 ReadPrefetchCounter      :4;
    U32 EnableBursting           :1;
    U32 EnableIopBusTimeoutTimer :1;
    U32 BREQoTimerResolution     :1;
    U32 EnableIopBREQo           :1;
    U32 BREQoDelayClockCount     :4;
    U32 MapInMemorySpace         :1;
    U32 OddParitySelect          :1;
    U32 EnableParityCheck        :1;
    U32 MemoryWriteProtect       :1;
    U32 InternalWaitStates       :4;
    U32 PciRev2_1Mode            :1;
    U32 IopBusWidth              :2;
    U32 Reserved1                :4;
    U32 Iop480WADWaitStates      :4;
    U32 Iop480WDDWaitStates      :4;
    U32 Iop480WDLYDelayStates    :3;
    U32 Iop480WHLDHoldStates     :3;
    U32 Iop480WRCVRecoverStates  :3;
    U32 Reserved2                :15;
    U32 Iop480RADWaitStates      :4;
    U32 Iop480RDDWaitStates      :4;
    U32 Iop480RDLYADelayStates   :3;
    U32 Iop480RDLYDDelayStates   :3;
    U32 Iop480RRCVRecoverStates  :3;
    U32 Reserved3                :15;
    U32 DramRefreshEnable        :1;
    U32 DramRefreshInterval      :11;
    U32 Iop480TWRdelay           :2;
    U32 Iop480W2Wdelay           :2;
    U32 Iop480A2Cdelay           :2;
    U32 Iop480RRCVdelay          :2;
    U32 Iop480PRCGdelay          :2;
    U32 Iop480WCWdelay           :2;
    U32 Iop480RCWdelay           :2;
    U32 Iop480C2Cdelay           :2;
    U32 Iop480R2Cdelay           :2;
    U32 Iop480R2Rdelay           :2;
} IOP_BUS_PROP, *PIOP_BUS_PROP;


/* IOP Arbitration Descriptor Structure */
typedef struct _IOP_ARBIT_DESC
{
    U32 IopBusDSGiveUpBusMode    :1;
    U32 EnableDSLockedSequence   :1;
    U32 GateIopLatencyTimerBREQo :1;
    U32 EnableWAITInput          :1;
    U32 EnableBOFF               :1;
    U32 BOFFTimerResolution      :1;
    U32 EnableIopBusLatencyTimer :1;
    U32 EnableIopBusPauseTimer   :1;
    U32 EnableIopArbiter         :1;
    U32 IopArbitrationPriority   :3;
    U32 BOFFDelayClocks          :4;
    U32 IopBusLatencyTimer       :8;
    U32 IopBusPauseTimer         :8;
    U32 EnableIopBusTimeout      :1;
    U32 IopBusTimeout            :15;
    U32 Reserved                 :16;
} IOP_ARBIT_DESC, *PIOP_ARBIT_DESC;


/* IOP Endian Descriptor Structure*/
typedef struct _IOP_ENDIAN_DESC
{
    U32 BigEIopSpace0            :1;
    U32 BigEIopSpace1            :1;
    U32 BigEExpansionRom         :1;
    U32 BigEMaster480LCS0        :1;
    U32 BigEMaster480LCS1        :1;
    U32 BigEMaster480LCS2        :1;
    U32 BigEMaster480LCS3        :1;
    U32 BigEMaster480Dram        :1;
    U32 BigEMaster480Default     :1;
    U32 BigEIopBusRegion0        :1; 
    U32 BigEIopBusRegion1        :1;
    U32 BigEIopBusRegion2        :1;
    U32 BigEIopBusRegion3        :1;
    U32 BigEDramBusRegion        :1;
    U32 BigEDefaultBusRegion     :1;
    U32 BigEDmaChannel0          :1;
    U32 BigEDmaChannel1          :1;
    U32 BigEIopConfigRegAccess   :1;
    U32 BigEDirectMasterAccess   :1;
    U32 BigEIopConfigRegInternal :1;
    U32 BigEDirectMasterInternal :1;
    U32 BigEByteLaneMode         :1;
    U32 BigEByteLaneModeLBR0     :1;
    U32 BigEByteLaneModeLBR1     :1;
    U32 BigEByteLaneModeLBR2     :1;
    U32 BigEByteLaneModeLBR3     :1;
    U32 BigEByteLaneModeDRAMBR   :1;
    U32 BigEByteLaneModeDefBR    :1;
    U32 Reserved1                :4;
    U32 ReservedForFutureUse;
} IOP_ENDIAN_DESC, *PIOP_ENDIAN_DESC;


/* Power Management Properties */
typedef struct _PM_PROP
{
    U32 Version                   :3;
    U32 PMEClockNeeded            :1;
    U32 DeviceSpecialInit         :1;
    U32 D1Supported               :1;
    U32 D2Supported               :1;
    U32 AssertPMEfromD0           :1;
    U32 AssertPMEfromD1           :1;
    U32 AssertPMEfromD2           :1;
    U32 AssertPMEfromD3Hot        :1;
    U32 Read_Set_State            :2;
    U32 PME_Enable                :1;
    U32 PME_Status                :1;  
    U32 PowerDataSelect           :3;
    U32 PowerDataScale            :2;
    U32 PowerDataValue            :8;
    U32 Reserved                  :4;
} PM_PROP, *PPM_PROP;


/* IOP API Initialization Parameters Structure */
typedef struct _API_PARMS
{
    VOID            *PlxIcIopBaseAddr;
    VOID            *SpuMemBaseAddr;
    PPCI_BUS_PROP    PtrPciBusProp;
    PPCI_ARBIT_DESC  PtrPciArbitDesc;
    PIOP_BUS_PROP    PtrIopBus0Prop;
    PIOP_BUS_PROP    PtrIopBus1Prop;
    PIOP_BUS_PROP    PtrIopBus2Prop;
    PIOP_BUS_PROP    PtrIopBus3Prop;
    PIOP_BUS_PROP    PtrLcs0Prop;
    PIOP_BUS_PROP    PtrLcs1Prop;
    PIOP_BUS_PROP    PtrLcs2Prop;
    PIOP_BUS_PROP    PtrLcs3Prop;
    PIOP_BUS_PROP    PtrDramProp;
    PIOP_BUS_PROP    PtrDefaultProp;
    PIOP_BUS_PROP    PtrExpRomBusProp;
    PIOP_ARBIT_DESC  PtrIopArbitDesc;
    PIOP_ENDIAN_DESC PtrIopEndianDesc;
    PPM_PROP         PtrPMProp;
    PU32             PtrVPDBaseAddress;
} API_PARMS, *PAPI_PARMS;


typedef struct _SPU_STATUS
{
    U32 DSRInputInactive          :1;
    U32 CTSInputInactive          :1;
    U32 ReceiveBufferReady        :1;
    U32 FramingError              :1;
    U32 OverrunError              :1;
    U32 ParityError               :1;
    U32 LineBreak                 :1;
    U32 TransmitBufferReady       :1;
    U32 TransmitterShiftRegReady  :1;
    U32 Reserve                   :23;
} SPU_STATUS, *PSPU_STATUS;


typedef struct _SPU_DESC
{
    U32 BaudRate;
    U32 LclkFreq;                         /* LCLK Frequency in Hz */
    U32 LM                        :2;     /* LoopBack Modes */
    U32 DTR                       :1;     /* Data Terminal */
    U32 RTS                       :1;     /* Request to Send */
    U32 DB                        :1;     /* Data bits */
    U32 PE                        :1;     /* Parity Enable */
    U32 PTY                       :1;     /* Parity */
    U32 SB                        :1;     /* Stop bits */
    U32 Reserved                  :24;
} SPU_DESC, *PSPU_DESC;



#ifdef __cplusplus
}
#endif

#endif
