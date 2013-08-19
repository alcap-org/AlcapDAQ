/*****************************************************************************
**
**      Filename:   btdef.h
**
**      Purpose:    SBS Bit 3 shared definitions
**
**      $Revision: 1.1.1.1 $
**
******************************************************************************/
/*****************************************************************************
**
**        Copyright (c) 1997-2000 by SBS Technologies, Inc.
**        Copyright (c) 1996-1997 by Bit 3 Computer Corporation.
**                     All Rights Reserved.
**              License governs use and distribution.
**
*****************************************************************************/

#ifndef _BTDEF_H
#define _BTDEF_H

#include        <stddef.h>

/*****************************************************************************
**
**  Definitions to identify the operating system.
**
*****************************************************************************/
#if     defined(BT973) && !defined(BT_WIN95)
#define BT_WIN95
#endif  /* defined(BT973) && !defined(BT_WIN95) */

#if     (defined(BT983) || defined(BT984)) && !defined(BT_WINNT)
#define BT_WINNT
#endif  /* (defined(BT983) || defined(BT984)) && !defined(BT_WINNT) */

#if     defined(BT15901)    /* Broadcast Memory     */
#define BT_WINNT
#endif  /* defined(BT15901) */

#if     defined(BT15991)    /* Shared Memory        */
#define BT_WINNT
#endif  /* defined(BT15991) */

#if     defined(BT17903)    /* NanoBridge           */
#define BT_WINNT
#endif  /* defined(BT17903) */

#if     defined(BT2345)     /* Embedded 1394 to VME */
#define BT_uCOS
#endif  /* defined(BT2345) */

/*****************************************************************************
**
**  VMEbus macros.
**
*****************************************************************************/
/*
**  VMEbus address modifiers
*/
#define BT_AMOD_A32_SB      (0x0F)  /* A32 Extended Supervisory Block       */
#define BT_AMOD_A32_SP      (0x0E)  /* A32 Extended Supervisory Program     */
#define BT_AMOD_A32_SD      (0x0D)  /* A32 Extended Supervisory Data Access */
#define BT_AMOD_A32_NB      (0x0B)  /* A32 Extended Non-Privileged Block    */
#define BT_AMOD_A32_NP      (0x0A)  /* A32 Extended Non-Privileged Program  */
#define BT_AMOD_A32_ND      (0x09)  /* A32 Extended Non-Privileged Data     */
#define BT_AMOD_A32_SMBLT   (0x0C)  /* A32 Multiplexed Block Transfer (D64) */
#define BT_AMOD_A32_NMBLT   (0x08)  /* A32 Multiplexed Block Transfer (D64) */

#define BT_AMOD_A32     BT_AMOD_A32_SD
#define BT_AMOD_A32_D64 BT_AMOD_A32_SMBLT

#define BT_AMOD_A24_SB      (0x3F)  /* A24 Standard Supervisory Block       */
#define BT_AMOD_A24_SP      (0x3E)  /* A24 Standard Supervisory Program     */
#define BT_AMOD_A24_SD      (0x3D)  /* A24 Standard Supervisory Data Access */
#define BT_AMOD_A24_NB      (0x3B)  /* A24 Standard Non-Privileged Block    */
#define BT_AMOD_A24_NP      (0x3A)  /* A24 Standard Non-Privileged Program  */
#define BT_AMOD_A24_ND      (0x39)  /* A24 Standard Non-Privileged Data     */
#define BT_AMOD_A24_SMBLT   (0x3C)  /* A24 Multiplexed Block Transfer (D64) */
#define BT_AMOD_A24_NMBLT   (0x38)  /* A24 Multiplexed Block Transfer (D64) */

#define BT_AMOD_A24     BT_AMOD_A24_SD
#define BT_AMOD_A24_D64 BT_AMOD_A24_SMBLT
#define BT_AMOD_A24_MAX 0x1000000

#define BT_AMOD_A16_SD      (0x2D)  /* A16 Short Supervisory Data Access    */
#define BT_AMOD_A16_ND      (0x29)  /* A16 Short Non-Privileged Data Access */

#define BT_AMOD_A16     BT_AMOD_A16_SD
#define BT_AMOD_A16_MAX (0x10000)
#define BT_AMOD_DP_MAX  (0x800000)

/*****************************************************************************
**
**  List all SBS Bit 3 adaptor part numbers registered to date.
**  Used to track local and remote identification.
**
*****************************************************************************/

#define BT_PN_UNKNOWN       0               /* Unknown bus                  */
#define BT_PN_MCA5          84502220        /* MicroChannel type 5  (477)   */
#define BT_PN_MCA5_RPQ      600433          /* MicroChannel type 5  (477)   */
#define BT_PN_MCA3_DMA      85184230        /* MicroChannel type 3  (477-1) */
#define BT_PN_VME_NOINC     600908          /* VME RPQ w/ DMA Inc Inhibit   */
#define BT_PN_VME_NODMA     85154557        /* VME w/o DMA  (616)           */
#define BT_PN_VME_SDMA      85154550        /* VME w/slave DMA  (467,487-1) */
#define BT_PN_VME_SDMA_64   85642852        /* VME w/slave DMA & D64 (619)  */
#define BT_PN_VME2_DMA      84302037        /* VMEbus Ver 2 w/DMA   (487)   */
#define BT_PN_VME_DMA       82902270        /* VMEbus w/DMA         (413)   */
#define BT_PN_VME           82902250        /* VMEbus               (412)   */
#define BT_PN_VME2          85154557        /* VMEbus w/o DMA               */
#define BT_PN_VME_FIBER     85853432        /* VMEbus Fiber optic (618-202) */
#define BT_PN_QBUS          82602125        /* Qbus                 (465)   */
#define BT_PN_MB            82402025        /* Multibus I           (464)   */
#define BT_PN_VME_A24       82002065        /* VMEbus A24           (411)   */
#define BT_PN_EISA          84801115        /* EISA bus             (487)   */
#define BT_PN_EISA_RPQ      600440          /* EISA 32Mb window     (487)   */
#define BT_PN_EISA2         84801610        /* EISA w/slave DMA     (487-1) */
#define BT_PN_EISA2_RPQ     600650          /* EISA w/slave DMA     (487-1) */
#define BT_PN_SB            84202320        /* SBus slave only      (466)   */
#define BT_PN_SB2           84202322        /* SBus slave only (new layout) */
#define BT_PN_SB_DMA        84903600        /* SBus w/DMA           (467)   */
#define BT_PN_SB_SDMA       84903810        /* SBus w/ Slave DMA    (467-1) */
#define BT_PN_MCA3          82801120        /* MicroChannel type 3  (446)   */
#define BT_PN_TC            85001010        /* TURBOchannel (VME)   (497)   */
#define BT_PN_TC_QBUS       85001015        /* TURBOchannel (Qbus)  (495)   */
#define BT_PN_GIO32         85201240        /* GIO32                (607)   */
#define BT_PN_GIO64         85201650        /* GIO64                (608)   */
#define BT_PN_PCI_DMA       85221510        /* PCI                  (617)   */
#define BT_PN_PCI           85221515        /* PCI wo/DMA         (614/615) */
#define BT_PN_PCI_NODMA     85221517        /* PCI to VME w/o DMA   (616)   */
#define BT_PN_PCI2          BT_PN_PCI_NODMA /* don't use!                   */
#define BT_PN_PCI_FIBER     85851090        /* PCI Fiber Optic      (618)   */
#define BT_PN_VME_PCI       85302110        /* VMEbus to PCI Expansion Unit */
#define BT_PN_PCI_LC        85302115        /* PCI to VMEbus Low Cost       */
#define BT_PN_VME64         85253010        /* VME64 NanoPort      (2866)   */
#define BT_PN_PCI64         85257040        /* PCI64 2.1 NanoPort  (xxxx)   */
#define BT_PN_PCI_BMEM_2500 85601915        /* Broadcast Mem PCI (2500-xx)  */
#define BT_PN_PCI_BMEM_2510 85601914        /* Broadcast Mem PCI (2510-xx)  */
#define BT_PN_PCI_BMEM_2500_1 85601916      /* Broadcast Mem PCI (2500-xx-1)*/
#define BT_PN_PCI_BMEM_2510_1 85601917      /* Broadcast Mem PCI (2510-xx-1)*/
#define BT_PN_PCI_BMEM_2590_1 85601918      /* Shared Mem PCI (2590-xx-1)   */
#define BT_PN_PCI_BMEM_2591_1 85601919      /* Shared Mem PCI (2591-xx-1)   */
#define BT_PN_PCI_NBDG      85752420        /* NanoBridge PCI card          */
#define BT_PN_VME_NBDG      85804030        /* NanoBridge VMEbus card       */
#define BT_PN_1394_EMBEDDED 85901080        /* embedded 1394 to VME card    */
#define BT_PN_PCI_DB        85911020        /* dataBLIZZARD PCI card    */
#define BT_PN_VME_DB        85913153        /* dataBLIZZARD VME */
/* EAS A64 */           
#define BT_PN_PCI_DBA64_RPQ 601332          /* Next Gen NanoBus PCI card RPQ 601332 (with A64) */

#define BT_PN_PCI_FIBER_D64 BT_PN_PCI_DB    /* Obsolote name do not use any more */
#define BT_PN_VME_FIBER_D64 BT_PN_VME_DB    /* Obsolote name do not use any more */
 


/*****************************************************************************
**
**  IEEE Organization Unique Identifier
**
*****************************************************************************/

#define BT_OUI_SBS_CP	(0x00d01cUL)	/* 24 bit company ID */

/*****************************************************************************
**
**  PCI Configuration Register definitions
**
**  All PCI Device IDs registered to date are listed here.  Each device ID
**  symbolic constant ends with the hardware model number.
**
*****************************************************************************/
#define BT_PCI_VENDOR_BIT3      0x108a  /* PCI sig assigned Bit 3 Vendor Id */
#define BT_PCI_DEVICE_617       0x0001  /* NanoBus with DMA                 */
#define BT_PCI_DEVICE_614       0x0002  /* NanoBus without DMA              */
#define BT_PCI_DEVICE_616       0x0003  /* NanoBus low cost                 */
#define BT_PCI_DEVICE_RPQ890    0x0004  /* NanoBus SGI RPQ w/different swap */
#define BT_PCI_DEVICE_618       0x0010  /* NanoBus Fiber Interface          */
#define BT_PCI_DEVICE_619       0x0011  /* NanoBus Fiber Interface D64      */
#define BT_PCI_DEVICE_704       0x0040  /* NanoBus II Fiber Interface D64   */
/* EAS A64 */           
#define BT_PCI_DEVICE_RPQ601332 0x0041  /* PCI A64 hardware */



#define BT_PCI_DEVICE_2500      0x0100  /* Broadcast Memory (PN 85601915)   */
#define BT_PCI_DEVICE_2510      0x0101  /* Broadcast Memory (PN 85601914)   */
#define BT_PCI_DEVICE_2500_1    0x0102  /* Broadcast Memory (PN 85601916)   */
#define BT_PCI_DEVICE_2510_1    0x0103  /* Broadcast Memory (PN 85601917)   */
#define BT_PCI_DEVICE_2590_1    0x0104  /* Shared Memory (PN 85601918)      */
#define BT_PCI_DEVICE_2591_1    0x0105  /* Shared Memory (PN 85601919)      */
#define	BT_PCI_DEVICE_2506      0x0110  /* Broadcast Memory VME (PN 85631360*/
#define	BT_PCI_DEVICE_2516      0x0111  /* Broadcast Memory VME (PN 85631355*/

#define BT_PCI_DEVICE_xxxx      0x0200  /* NanoPort card                    */

#define BT_PCI_DEVICE_NBDG      0x0300  /* NanoBridge card                  */

#define BT_PCI_DEVICE_1394      0x0400  /* 1394 to VMEbus (PN 85901080)     */
#define BT_PCI_DEVICE_1394_OHCI 0x0401  /* 1394 OHCI cards (PN unknown)     */

#define BT_PCI_DEVICE_PPB       0x0500  /* SBS PCI-to-PCI Bridge Expansion  */

#define BT_PCI_VENDOR_NEWBDG    0x10E3

#define BT_PCI_VENDOR_TI        0x104c  /* Texas Instrument's Vendor Id     */
#define BT_PCI_DEVICE_1222      0x8009  /* TI's TSB12lv22 OHCI-lynx DeviceId*/
#define BT_PCI_DEVICE_1223      0x8019  /* TI's TSB12lv23 OHCI-lynx DeviceId*/

/*
**  PCI Device IDs
*/
#define BT_PCI_DEV_617      0x1
#define BT_PCI_DEV_NEW_PVB  0x0
#define BT_PCI_DEV_IBM_PPB 0x22


/*****************************************************************************
**
**  Define general purpose macros and type definitions here.
**
*****************************************************************************/

/*
**  POSIX type definitions needed
*/

#if	defined(__linux__)

#if	defined(__KERNEL__)
#include	<linux/types.h>

/* Copy from <sys/unistd.h> */

#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2

#else	/* defined(__KERNEL__) */

#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>

#endif	/* defined(__KERNEL__) */

#elif     defined(POSIX_SOURCE) || defined(SYS_V) || defined(BSD_4_2) \
        || defined(__sun)     || defined(__sgi) || defined(__AIX)   \
        || defined(__hpux)    || defined(__vxworks)

#if     !(defined(_KERNEL) && (defined(__sgi) || defined(__sun)))
#include        <unistd.h>
#endif  /* !(defined(_KERNEL) && (defined(__sgi) || defined(__sun))) */

#include        <sys/types.h>

#else   /* defined(POSIX_SOURCE) || defined(SYS_V) || defined(BSD_4_2) \
           || defined(__sun)     || defined(__sgi) || defined(__AIX)   \
           || defined(__hpux)    || defined(__vxworks)               */

typedef long    off_t;

#endif  /* defined(POSIX_SOURCE) || defined(SYS_V) || defined(BSD_4_2) \
           || defined(__sun)     || defined(__sgi) || defined(__AIX)   \
           || defined(__hpux)    || defined(__vxworks)               */

/*
**  Types for forcing certain size storage
*/

/* Assume there is a natural 64-bit type */

#define BT_MAKE_DATA64(d64, hi, lo) ((d64)=((((bt_data64_t)(hi))<<32)|(lo)))
#define BT_HI_DATA32(d64) ((bt_data32_t) ((d64) >> 32))
#define BT_LO_DATA32(d64) ((bt_data32_t) ((d64) & 0xFFFFFFFFul))

#if     defined(__sun)

#if     !defined(bool_t)
#define bool_t  int
#endif  /* !defined(bool_t) */

#if	defined(SUNOS5_5)
#define	uint8_t		unsigned char
#define	uint16_t	unsigned short
#define	uint32_t	unsigned long
#define uint64_t	unsigned long long
#define	int8_t		char
#define	int16_t         short
#define	int32_t         long
#define int64_t         longlong_t
#define intptr_t        int
#define timeout_id_t    int
#endif	/* defined(SUNOS5_5) */

#if	defined(SUNOS5_6)
#define timeout_id_t    int
#endif	/* defined(SUNOS5_6) */

typedef uint8_t         bt_data8_t;
typedef uint16_t        bt_data16_t;
#define BT_FMT_D16      "%04hx"
typedef uint32_t        bt_data32_t;
#define BT_FMT_D32      "%08x"
typedef uint64_t        bt_data64_t;

#else   /* defined(__sun) */

#if     !defined(bool_t)
#define bool_t  int
#endif  /* !defined(bool_t) */

#if     !defined(CHAR_BIT) || !defined(UINT_MAX)
#include <limits.h>
#endif  /* !defined(CHAR_BIT) || !defined(UINT_MAX) */


#if     CHAR_BIT != 8
#error  This code assumes the character type is eight bits in size.
#endif

typedef unsigned char   bt_data8_t;
typedef unsigned short  bt_data16_t;

#define BT_FMT_D16      "%04hx"

#if     UINT_MAX < 4294967295ul
typedef unsigned long   bt_data32_t;
#define BT_FMT_D32      "%08lx"
#else   /* Can use unsigned int */
typedef unsigned int    bt_data32_t;
#define BT_FMT_D32      "%08x"
#endif  /* UINT_MAX */

#if     defined(__osf__)

typedef unsigned long   bt_data64_t;

#elif   defined(__sgi)

typedef uint64_t bt_data64_t;

#elif   defined(__GNUC__)

typedef unsigned long long      bt_data64_t;

#elif   defined(_MSC_VER) && (_MSC_VER >= 1000)

typedef unsigned __int64 bt_data64_t;

#else   /* default- use a structure */

#define BT_DATA64_T_IS_STRUCTURE (1)  /* Some optimizations are easier
                                         if we know this. */
typedef struct {
    bt_data32_t hi;
    bt_data32_t lo;
} bt_data64_t;

/* Redefine the default macros for 64 bit data handling */
#undef  BT_MAKE_DATA64
#undef  BT_HI_DATA32
#undef  BT_LO_DATA32

#define BT_MAKE_DATA64(d64, up, low) (((d64).hi=(up),(d64).lo=(low),(d64)))
#define BT_HI_DATA32(d64) ((d64).hi)
#define BT_LO_DATA32(d64) ((d64).lo)

#endif  /* defined(__osf__); defined(sgi); defined(__GNUC__);
           defined(_MSC_VER) && (_MSC_VER >= 1000); default */

#endif  /* defined(__sun) */

/*
**  Defines and types for specifing access widths
*/
typedef size_t bt_width_t;

#define BT_WIDTH_D8     sizeof(bt_data8_t)
#define BT_WIDTH_D16    sizeof(bt_data16_t)
#define BT_WIDTH_D32    sizeof(bt_data32_t)
#define BT_WIDTH_D64    sizeof(bt_data64_t)
#define BT_WIDTH_ANY    0

/*
**  Masks and shift values
*/
#define BT_D4_MASK      0xf
#define BT_D4_SHFT      4
#define BT_D8_MASK      0xff
#define BT_D8_SHFT      8
#define BT_D16_MASK     0xffffUL
#define BT_D16_SHFT     16
#define BT_D32_MASK     0xffffffffUL
#define BT_D32_SHFT     32

/*
**  If TRUE has not been defined, define both TRUE and FALSE here
*/
#if     !defined(TRUE)
#if     defined(LINT)
#define TRUE    ((bool_t) 1)
#define FALSE   ((bool_t) 0)
#else   /* defined(LINT) */
#define TRUE    (0==0)
#define FALSE   (0!=0)
#endif  /* defined(LINT) */
#endif  /* !defined(TRUE) */

/*
**  Access flags type and access defines for bt_open() and bt_mmap()
**  This used to be in btapi.h, SGM moved it here because bt_accessflag_t
**  is also used the mmap call which is often just passed to the driver
**  so the driver wants the BT_RD... definitions so they need to be in
**  btdef.h file.
*/

#if	defined(__vxworks)

typedef int bt_accessflag_t;	/* Want to match type used for open() */

#else	/* defined(__vxworks) */

typedef unsigned long bt_accessflag_t;

#endif	/* defined(__vxworks) */

#define BT_RD   (1<<0)
#define BT_WR   (1<<1)
#define BT_RDWR (BT_RD | BT_WR)

#endif  /* _BTDEF_H */



