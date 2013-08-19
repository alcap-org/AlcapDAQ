/*****************************************************************************
**
**      Filename:   btwdef.h
**
**      Purpose:    SBS Windows NT PCI NanoBus/dataBlizzard driver header file.
**
**                  Mirror API types and defines specific to the Model 983 and 984
**
**
******************************************************************************/
/*****************************************************************************
**
**        Copyright (c) 2000 by SBS Technologies, Inc.
**                     All Rights Reserved.
**              License governs use and distribution.
**
*****************************************************************************/

#if !defined(_BTPDEF_H)
#define _BTPDEF_H

/*
** Various types used by the Mirror API
*/
typedef unsigned long bt_msec_t;
typedef DWORD bt_devdata_t;
typedef DWORD bt_devaddr_t;
typedef DWORD bt_addr_mod_t;

/*
**  Define Mirror API constants that vary based on OS
*/
#define BT_FOREVER  (~0ul)
#define BT_NO_WAIT  (0ul)

/*
** Define the various flags for memory mapping
*/
#define	BT_MAP_WRITE		BT_WR
#define	BT_MAP_READ		    BT_RD
#define	BT_MAP_SHARE		(1<<2)

#define	BT_MAP_RW	(BT_MAP_READ | BT_MAP_WRITE)

/*
** Constants shared between driver and Mirror API
*/
#define BT_MIN_IRQ_Q_SIZE       0x10    /* Interrupt Q sizes */
#define DEFAULT_Q_SIZE          0x400
#define BT_MAX_IRQ_Q_SIZE       0x4000

#define BT_MIN_DMA_THRESHOLD    0x4     /* DMA threshold */
#define DEFAULT_DMA_THRESHOLD   0x80

#define BT_MIN_DMA_TIMEOUT      200     /* DMA timeout in mSec */
#define DEFAULT_DMA_TIMEOUT     5000
#define BT_MAX_DMA_TIMEOUT      60000

#define BT_MIN_RESET_DELAY      10      /* Remote reset delay in mSec */
#define DEFAULT_RESET_TIMER     200 
#define BT_MAX_RESET_DELAY      60000

#define BT_MIN_TRACE_Q_LEN      50      /* Number of trace queue messages */
#define DEFAULT_TRACE_Q_LEN     200 
#define BT_MAX_TRACE_Q_LEN      500

#define BT_LEN_TR_MSG           80      /* Number of characters for each trace message */

#define BT_MIN_DMA_CEILING          0x4     /* DMA threshold */
#define DEFAULT_DMA_POLL_CEILING    4096

#define DEFAULT_LMEM_SIZE       0x10000     /* Default size of the local memory device */

#define FILE_DEVICE_BTBRIDGE ((DEVICE_TYPE)32768)

/*
** Define interrupt handling information
*/
typedef enum {
   BT_IRQ_OVERFLOW,	    /* Overflowed ability to deliver IRQ notifications */
   BT_IRQ_ERROR,	    /* Error interrupt */
   BT_IRQ_PRG,          /* Programmed interrupt PT/PR */
   BT_IRQ_IACK,         /* VMEbus interrupt */
   BT_MAX_IRQ
} bt_irq_t;

#define	BT_VECTOR_ALL	(0)

typedef  void bt_icbr_t(void * param_p, bt_irq_t irq_type, 
	bt_data32_t vector);

/*
** Macro to help align pointers
*/
#define BT_ALIGN_PTR(buf_p, align) ((((unsigned long)(buf_p))&((align) - 1))?\
                                    ((ptrdiff_t)((align)-(((unsigned long) \
                                     (buf_p))&((align)-1)))):((ptrdiff_t)0))
#endif /* !defined(_BTPDEF_H) */


