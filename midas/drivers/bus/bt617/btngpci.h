/*****************************************************************************
**
**      Filename:   btngpci.h
**
**      Purpose:    Bit 3 Next Generation (Data Blizzard) Adaptor include 
**                  file used only by Data Blizzard Device Drivers.
**
**      $Revision: 1.1.1.1 $
**
******************************************************************************/
/*****************************************************************************
**
**        Copyright (c) 2000-02 by SBS Technologies, Inc.
**                     All Rights Reserved.
**              License governs use and distribution.
**
*****************************************************************************/

#ifndef _BTNGPCI_H
#define _BTNGPCI_H

#if defined(__vxworks)
#include <stdio.h>
#endif /* defined(__vxworks) */

/*****************************************************************************
**
**  Macro indicating that this product is from the NanoBus family
**
*****************************************************************************/
#define BT_NBUS_FAMILY

/* 
** Interrupt queue for storing a list of interrupts that have occured
*/
typedef struct {
    volatile bt_data64_t    head;       /* queue index at which to add next message */
    bt_data32_t             length;     /* number of elements in queue buffer array */
    volatile bt_data32_t    queue[1];   /* queue buffer, is array of vectors */
} bt_irq_q_t;

/*****************************************************************************
**
**  Logical Devices available. 
**
**  Not all avalible on all adapters.
**
*****************************************************************************/
typedef enum BT_AXSTYPS {
    /*
    ** Original Unix definitions
    */
    BT_AXSIO,                   /* Access I/O registers */
#define	BT_MIN_DEV BT_AXSIO	/* Used for parameter checking */
    BT_AXSRDP,                  /* Access Remote Dual Port RAM */
#define	BT_AXSDP BT_AXSRDP	/* Old DP name references remote DP */
    BT_AXSRI,                   /* Access Remote IO */
    BT_AXSRR,                   /* Access Remote RAM */
    BT_AXSRE,                   /* Access Remote RAM Extended */
    BT_AXS24,                   /* Access Remote RAM 24-bit space */
    BT_AXSLM,                   /* Access Local Memory (remote window) */
    BT_AXSLDP,                  /* Access Local Dual Port RAM */
    BT_MAX_AXSTYPS,             /* Maximum number of defined access types */

    /* 
    ** The following definition is used internally by some drivers 
    */
    BT_DEV_RE = BT_AXSRE,	/* Not to be used by application programs! */

    /*
    ** Mirror API definitions
    */
    BT_DEV_DP = BT_AXSRDP,	/* Dual Port RAM */
    BT_DEV_LDP = BT_AXSLDP,	/* Local Dual Port RAM */
    BT_DEV_RDP = BT_AXSRDP,	/* Remote Dual Port RAM */
    BT_DEV_IO = BT_AXSRI,	/* Remote bus A16 (I/O) space */
    BT_DEV_A24 = BT_AXS24,	/* Remote bus A24 (short) space */
    BT_DEV_RR = BT_AXSRR,	/* Remote bus A32 (standard) space */
    BT_DEV_LM = BT_AXSLM,	/* Local Memory device */
    BT_DEV_NIO = BT_AXSIO,	/* Adapter Node I/O registers */
    BT_DEV_DIAG = BT_AXSIO,	/* For Internal Use Only! */
    
    /*
    ** Mirror API aliases
    */
    BT_DEV_A16 = BT_DEV_IO,
    BT_DEV_A32 = BT_DEV_RR,
    BT_DEV_MEM = BT_DEV_RR,
    BT_DEV_DEFAULT = BT_DEV_DP,

    BT_MAX_DEV = BT_MAX_AXSTYPS
    
} bt_dev_t;

/*****************************************************************************
**
**  Structure for parameter set or acquire 
**
**  BIOC_PARAM & BIOC_DEV_ATTRIB
**
*****************************************************************************/
typedef struct {
    bt_data32_t    error;               /* Error code */
    bt_data32_t    vector;              /* Vector to send with irq */
                                        /* Must be below BT_DRV_VECTOR_BASE */
} bt_send_vector_t;

/*****************************************************************************
**
**  Structure for parameter set or acquire 
**
**  BIOC_PARAM & BIOC_DEV_ATTRIB
**
*****************************************************************************/
typedef struct {
    bt_data32_t    error;               /* Error code */
    bt_data32_t    param;               /* Parameter read or write - bt_info_t */
    bt_devdata_t   value;               /* Value to write or value read */
} bt_param_t;

/*****************************************************************************
**
**  Test and Set IOCTL structure 
**
**  BIOC_TAS
**
*****************************************************************************/
typedef struct {
    bt_data32_t  error;              /* Error code */
    bt_devaddr_t addr;               /* Address of byte to access */
    bt_data8_t   result;             /* Result:     0 = bit was clear */
} bt_tas_t;                          /*             1 = bit was set */

#define  BT_TAS_MSBIT 0x80

/*****************************************************************************
**
**  Compare and swap IOCTL structure 
**
**  BIOC_CAS
**
*****************************************************************************/
typedef struct {
    bt_data32_t  error;          /* Error code */
    bt_devaddr_t addr;           /* Address to check */
    bt_data32_t  cmp;            /* Compare value */
    bt_data32_t  swap;           /* Swap value */
    bt_data32_t  len;            /* Length of access (sizeof) parameter */
                                 /* Valid values are BT_WIDTH_D?, not 64 */
    bt_data32_t  result;         /* Result of operation */
} bt_cas_t;

/*****************************************************************************
**
**  ICBR thread register/unregister structure 
**
**  BIOC_THREAD_REG & BIOC_THREAD_UNREG
**
*****************************************************************************/
typedef struct {
    bt_data32_t  error;             /* Error code */
    
    /* Returned from driver */
    bt_data32_t  unit;              /* Unit number, library requires this */
                                    /*   to do mmap of diag device */
    bt_data64_t  thread_id;         /* Driver level thread id */
    
} bt_thread_reg_t;

/*****************************************************************************
**
**  ICBR thread add/delete structure 
**
**  BIOC_THREAD_ADD & BIOC_THREAD_DELETE
**
*****************************************************************************/
typedef struct {
    bt_data32_t  error;             /* Error code */
    bt_data32_t  irq_type;          /* Irq type interrested in - bt_irq_t */
    bt_data32_t  vector;            /* Vector - not currently used by driver */
    bt_data64_t  thread_id;         /* Driver level thread id */
    
} bt_thread_add_t;

/*****************************************************************************
**
**  ICBR thread wait structure 
**
**  BIOC_THREAD_WAIT
**
*****************************************************************************/
typedef struct {
    bt_data32_t  error;             /* Error code */
    bt_data64_t  thread_id;         /* Driver level thread id */
    bt_data32_t  wait_msec;         /* Number of mSec to wait for irq */
                                    /* First release only supports BT_NO_WAIT */
    
} bt_thread_wait_t;

/*****************************************************************************
**
**  HW Address Transfer 
**
**  BIOC_HW_READ & BIOC_HW_WRITE
**
*****************************************************************************/
typedef struct {
    bt_data32_t  error;             /* Error code */
    bt_data32_t  local_hw_addr;     /* Local bus address to xfer to/from */
    bt_data32_t  ldev_addr;         /* Logical device address to xfer to/from */
    bt_data32_t  xfer_len;          /* Number of bytes to transfer */
                                    /* On return, number of bytes transferred */
    
} bt_hw_xfer_t;

/*****************************************************************************
**
**  Semaphore Take/Give 
**
**  BIOC_SEMA_TAKE/BIOC_SEMA_GIVE
**
*****************************************************************************/
typedef struct {
    bt_data32_t  error;             /* Error code */
    bt_data32_t  sema_reg;          /* Which semaphore to take/give - bt_reg_t */
    bt_data32_t  wait_msec;         /* Number of mSec to wait for semaphore */
                                    /* First release only supports BT_NO_WAIT */
    
} bt_sema_access_t;

/*****************************************************************************
**
**  Hardware Diagnostics 
**
**  BIOC_LCARD_DIAG/BIOC_CABLE_DIAG/BIOC_RCARD_DIAG/BIOC_PAIR_DIAG
**
*****************************************************************************/
#define BT_DIAG_MAX_REV_INFO    30
typedef struct {
    bt_data32_t  error;             /* Error code */
    bt_data32_t  line_number;      /* Line number failure was discovered on */
    bt_data32_t  r_ldev;            /* Remote logical device to use during testing */
    bt_data32_t  r_addr;            /* Remote address to use during testing */
    bt_data32_t  r_len;             /* Size in bytes of remote resource to test */
    bt_data8_t   rev_info[BT_DIAG_MAX_REV_INFO];      /* Text string with file and revision number */
    
} bt_hw_diag_t;


/*****************************************************************************
**
**  Status structure 
**
**  BIOC_RESET, BIOC_STATUS, BIOC_CLR_STATUS, BIOC_SETUP
**
*****************************************************************************/
typedef bt_data32_t bt_status_t;
#define BT_STATUS_ERR_SHFT    (24)

    /* interface parity error */
#define BT_STATUS_PARITY      (LSR_PARITY_ERR<<BT_STATUS_ERR_SHFT)   

    /* remote bus error */
#define BT_STATUS_REMBUS      (LSR_REMBUS_ERR<<BT_STATUS_ERR_SHFT)   

    /* interface timeout */
#define BT_STATUS_TIMEOUT     (LSR_TIMEOUT_ERR<<BT_STATUS_ERR_SHFT)

    /* remote bus power or cable missing */
#define BT_STATUS_POWER       (LSR_NO_CONNECT<<BT_STATUS_ERR_SHFT)

#define BT_STATUS_RRESET      (1<<19)   /* remote bus was reset */

#define BT_STATUS_MASK        (BT_STATUS_TIMEOUT|BT_STATUS_POWER|BT_STATUS_PARITY|BT_STATUS_REMBUS)

/* Old names to be obsoleted, use above names for new development */
#define BT_INTR_XMITR      (1<<17)      /* Not used anymore */
#define BT_INTR_OVRRUN     (1<<16)      /* Not used anymore */
#define BT_INTR_RRESET     BT_STATUS_RRESET  
#define BT_INTR_POWER      BT_STATUS_POWER   
#define BT_INTR_TIMEOUT    BT_STATUS_TIMEOUT 
#define BT_INTR_REMBUS     BT_STATUS_REMBUS  
#define BT_INTR_PARITY     BT_STATUS_PARITY  
#define BT_INTR_ERR_SHFT   BT_STATUS_ERR_SHFT

/*****************************************************************************
**
**  Bind structure 
**
**  BIOC_BIND, BIOC_UNBIND
**  BIOC_HW_BIND, BIOC_HW_UNBIND
**
*****************************************************************************/
typedef struct bt_bind_d {
    bt_data32_t   error;        /* Error code  */
    bt_devaddr_t  host_addr;    /* Virtual address of user buffer to bind */
                                /*    or physical bus address for HW_BIND */
    bt_data32_t   length;       /* Length of buffer to bind in bytes */
    bt_data8_t    nowait;       /* FALSE: wait if resources unavailable */
                                /* TRUE: return errno = EBUSY, don't wait */

    /* 
    **  Fields below are written by the driver if binding succeeds.
    */ 
    bt_data32_t  phys_addr;     /* 16 Mbyte offset in VMEbus remote RAM */
                                /*   window to access to reach buffer */
                                /*   Should be BT_BIND_NO_CARE if specific */
                                /*   alignment is not required */
#define BT_BIND_NO_CARE         (0xffffffff)

    /* 
    ** Following information is for internal device driver use only!
    */ 
    bt_devaddr_t sysinfo_p;      /* Pointer to internal system information */
     
} bt_bind_t;

/*****************************************************************************
**
**  Adaptor register access stucture 
**
**  BIOC_IOREG
**
*****************************************************************************/
typedef struct {
    bt_data32_t        error;                /* Error code */
    bt_data32_t        operation;            /* one of enum BT_ACCESS */ 
                                             /* use enum BT_ACCESS values */
    bt_data32_t        offset;               /* Appropriate register offset */
                                             /* use enum BT_REGISTERS values*/
    bt_data32_t        value;                /* Value to write or one-shot */
} bt_ioaccess_t;

/*****************************************************************************
**
**  PCI configuration register access stucture 
**
**  BIOC_PCFG
**
*****************************************************************************/
typedef struct {
    bt_data32_t         error;      /* Error code */
    bt_data32_t         operation;  /* one of enum BT_ACCESS */ 
                                    /* use enum BT_ACCESS values */
    bt_data32_t         bus;        /* Bus number (0 - 255) NOT supported yet */
    bt_data32_t         device;     /* Device number (0 - 15) */
    bt_data32_t         function;   /* Function number (0 - 7) */
    bt_data32_t         size;       /* Access width (1, 2 or 4) */
    bt_data32_t         offset;     /* Configuration register offset */
    bt_data32_t         value;      /* Value to write or value read */
    bt_data32_t         type0;      /* TRUE or FALSE, NOT supported yet */
} bt_pcfg_access_t;

/*****************************************************************************
**
**  Adjustable driver parameters.  
**
**  Not all are supported on each platform.
**
**  Use with bt_param_t structure (BIOC_PARAM & BIOC_DEV_ATTRIB)
**
**  #defined names are obsolete and should not be used.  They are for
**  compatability only.  Use BT_INFO_ names for new development.
**
*****************************************************************************/
typedef enum param_valus {
    /*
    ** Mirror API info parameters 
    */
    BT_MIN_INFO,	        /* Range checking only */
#define INVALID                 BT_MIN_INFO

    BT_INFO_BLOCK,	        /* Can use BLT mode: TRUE/FALSE */
#define DMA_BLOCK               BT_INFO_BLOCK

    BT_INFO_PAUSE,	        /* Set pause mode for DMA: TRUE/FALSE */
#define DMA_PAUSE               BT_INFO_PAUSE

    BT_INFO_DATAWIDTH,	        /* Maximum access width allowed on a transfer */
#define DATA_SIZE               BT_INFO_DATAWIDTH

    BT_INFO_DMA_AMOD,	        /* Address Modifier to use for DMA */
#define DMA_ADDR_MOD            BT_INFO_DMA_AMOD

    BT_INFO_PIO_AMOD,	        /* Address Modifier to use for PIO */
#define PIO_ADDR_MOD            BT_INFO_PIO_AMOD

    BT_INFO_MMAP_AMOD,	        /* Address Modifier to use for MMAP */
#define MMAP_ADDR_MOD           BT_INFO_MMAP_AMOD

    BT_INFO_SWAP,               /* Swapping value to use for logical dev */
#define SWAP_BITS               BT_INFO_SWAP

    BT_INFO_DMA_THRESHOLD,      /* Minimum length before we attempt DMA */
#define THRESHOLD               BT_INFO_DMA_THRESHOLD

    BT_INFO_DMA_POLL_CEILING,   /* Maximum length for polled mode DMA. */
#define DMA_POLL_SIZE           BT_INFO_DMA_POLL_CEILING

    BT_INFO_DMA_WATCHDOG,          /* Watchdog timer for DMA, default 5 seconds */
#define DMA_TIMEOUT             BT_INFO_DMA_WATCHDOG

    BT_INFO_TRACE,	            /* Software tracing level */
#define TRACE_LVL               BT_INFO_TRACE

    BT_INFO_RESET_DELAY,    /* Wait time for remote reset to complete */
#define RESET_TIMER         BT_INFO_RESET_DELAY

    BT_INFO_USE_PT,         /* Send PT programmed irqs instead of PR */

    BT_INFO_INC_INHIB,	    /* Set increment inhibit for DMA: TRUE/FALSE */
#define DMA_INC_INHIB       BT_INFO_INC_INHIB

    BT_INFO_ICBR_PRIO,     /* Task priority of interrupt dispatch, vxWorks only */
    
    BT_INFO_ICBR_STACK,     /* Task stack size for interrupt dispatch, vxWorks only */

    BT_INFO_GEMS_SWAP,          /* Enable GEMS swapping, GEMS Only */
    
    BT_INFO_LOC_PN,         /* Part number of local card */
#define BT_INFO_GET_BASE    BT_INFO_LOC_PN
#define INF_LDEV_TYPE       BT_INFO_LOC_PN

    BT_INFO_REM_PN,	    /* Part number of remote card */
#define INF_RDEV_TYPE       BT_INFO_REM_PN

    BT_INFO_LM_SIZE,	    /* Size of local memory device */
#define INF_LM_SIZE         BT_INFO_LM_SIZE

    BT_INFO_ICBR_Q_SIZE,    /* Size of the interrupt dispatch queue */
#define INF_ICBR_Q_SIZE     BT_INFO_ICBR_Q_SIZE

    BT_INFO_BIND_ALIGN,	    /* Alignment required for bt_bind() buffers */
#define INF_BIND_ALIGN      BT_INFO_BIND_ALIGN

    BT_INFO_BIND_SIZE,	    /* Maximum size of a single bound buffer */
#define INF_BIND_SIZE       BT_INFO_BIND_SIZE

    BT_INFO_BIND_COUNT,	    /* Number of concurrent bind requests allowed */
#define INF_BIND_COUNT      BT_INFO_BIND_COUNT

    BT_INFO_TRANSMITTER,    /* TRUE = transmitter mode; FALSE = receiver mode */
#define INF_TRANSMITTER     BT_INFO_TRANSMITTER

    BT_INFO_UNIT_NUM,	    /* Unit number (0..15) */
#define INF_UNIT            BT_INFO_UNIT_NUM

    BT_INFO_KMEM_SIZE,      /* Total kernel memory used in bytes */
#define INF_TOTAL_KMEM      BT_INFO_KMEM_SIZE

    BT_INFO_ERROR_COUNT,    /* Number of error interrupts received */
#define BT_INFO_SIG_ERR     BT_INFO_ERROR_COUNT
#define INF_SIG_ERR         BT_INFO_ERROR_COUNT

    BT_INFO_EVENT_COUNT,    /* Number of events (prog irqs) received */
#define BT_INFO_SIG_PRG     BT_INFO_EVENT_COUNT
#define INF_SIG_PRG         BT_INFO_EVENT_COUNT

    BT_INFO_TOTAL_COUNT,    /* Total number of intterunts */
#define BT_INFO_SIG_TOT     BT_INFO_TOTAL_COUNT
#define INF_SIG_TOT         BT_INFO_TOTAL_COUNT

    BT_INFO_IACK_COUNT,     /* Total number of IACK intterunts */
#define BT_INFO_SIG_IACK    BT_INFO_IACK_COUNT
#define INF_SIG_IACK        BT_INFO_IACK_COUNT

    BT_INFO_ICBR_Q_START,   /* starting memory address of icbr_q, vxWorks only */

    INF_LOG_STAT,
#define BT_INFO_LOG_STAT    INF_LOG_STAT

    BT_INFO_BOARD_REV,      /* PCI Rev ID of board, firmware revision */

#if defined (BT1003)
/* EAS A64 */
    BT_INFO_A64_OFFSET,    /* A64 mode bits A32-A63 and mode flag MSB */
/* EAS TMP CODE */
    BT_INFO_KMALLOC_BUF,    /* return kmalloc buffer from driver */ 
    BT_INFO_KFREE_BUF,      /* free kmalloc buffer allocated by driver */
    BT_INFO_KMALLOC_SIZ,    /* size of buffer to kmalloc from driver */
#endif

    BT_MAX_INFO		    /* Last valid BT_INFO_ value, internal use only */
} bt_info_t;

/* 
** Obsolte parameter values -- to be removed! 
*/

#define INTR_FREEQ_COUNT    BT_MAX_INFO
#define REM_ADDR            BT_MAX_INFO
#define ADDR_MOD            BT_MAX_INFO
#define HNDSHK_DR_COUNT     BT_MAX_INFO
#define HNDSHK_USEC_TIME    BT_MAX_INFO

#define INF_SLOT            BT_MAX_INFO
#define INF_INTR_LVL        BT_MAX_INFO
#define INF_INTR_PRIO       BT_MAX_INFO
#define INF_PT_INTR_LVL     BT_MAX_INFO
#define INF_REMLOC          BT_MAX_INFO
#define INF_REMLEN          BT_MAX_INFO
#define INF_INTR_REGS       BT_MAX_INFO
#define INF_INTR_PICS       BT_MAX_INFO
#define INF_PIO_ERROR       BT_MAX_INFO
#define INF_MEM_PTR         BT_MAX_INFO
#define INF_INTR_QCNT       BT_MAX_INFO

/******************************************************************************
**
**  Trace values.
**
**  Use with TRACE_LVL parameter (BIOC_PARAM & BIOC_DEV_ATTRIB)
**
**  For simple informational tracing use one of the first 4 levels defined
**  below.
**
**  For detailed tracing the trace control value is made up of oring the 
**  various bits together to enable the trace macros that are of interest 
**  to you.  This type of tracing destroys performance and should only be
**  used by SBS Technical Support.
**
******************************************************************************/
/* 
**  Trace flags controlling error output 
*/
#define BT_TRC_NONE     0       /* no tracing is enabled whatsoever */
#define BT_TRC_ERROR    (1<< 0) /* Fatal system errors, shouldn't happen */
#define BT_TRC_WARN     (1<< 1) /* driver errors that may happen due to  */
                                /* external conditions */
#define BT_TRC_INFO     (1<< 2) /* messages for errors returned to the user */

/* 
**  Trace flags controlling which major components to trace
*/
#define BT_TRC_CFG      (1<< 4) /* driver configuration and initialization */
#define BT_TRC_RD_WR    (1<< 5) /* generic read/write (outside of DMA & PIO) */
#define BT_TRC_MMAP     (1<< 6) /* memory mapping and related code */
#define BT_TRC_BIND     (1<< 7) /* bt_bind() and related code */

#define BT_TRC_CTRL     (1<< 8) /* All ioctl() functions */
#define BT_TRC_OPEN     (1<< 9) /* Driver open and close */

#define BT_TRC_ISR      (1<<12) /* Interrupt Service Routine */
#define BT_TRC_IRQ_DISP (1<<13) /* Interrupt Request Processing/Dispatch */
#define BT_TRC_ICBR     (1<<14) /* user ICBR and the vector returned by */
                                /* applicable kernel interrupt handlers */
#define BT_TRC_ISR_ERR  (1<<15) /* ISR errors only */


/* 
**  Trace flags controlling which subsubsystems in the driver 
*/
#define BT_TRC_DMA      (1<<16) /* DMA, each transfer */
#define BT_TRC_PIO      (1<<17) /* PIO, each transfer */
#define BT_TRC_FUNC     (1<<18) /* function entry and exit */
#define BT_TRC_ALLOC    (1<<19) /* Allocation/release of resources */

#define BT_TRC_LOCK     (1<<20) /* all locks (user & driver internal) */
#define BT_TRC_MAPREG   (1<<21) /* details of each map register load */
#define BT_TRC_RAWIO    (1<<22) /* Raw I/O register access */
#define BT_TRC_KLIB     (1<<23) /* Misc. shared kernel routines */

#define BT_TRC_KDDI     (1<<24) /* All 3rd party driver rouitnes KDDI */
#define BT_TRC_LIO      (1<<25) /* Low level I/O register rd/wr */

/* 
**  Modifiers on tracing.
**
**  These bits are always used in combination with the other trace bits,
**  to provide more information. Combine them with any of the functional
**  tracing bits (4-27) above.
*/
#define BT_TRC_PROFILE  (1<<28) /* enable H/W timing marks for profiling */
#define BT_TRC_DETAIL   (1<<29) /* Detailed trace of enabled function */
#define BT_TRC_HW_DEBUG (1<<30) /* enable HW trace messages instead of console */

/* 
**  We don't use bit 31, the sign bit, because it would make some comparison
** operations messy
*/

/* 
**  Default trace level 
*/
#define BT_TRC_DEFAULT  (BT_TRC_ERROR|BT_TRC_WARN)

/*****************************************************************************
**
**  File numbers for hardware tracing
**
**  Each file should have a unique for number, lower ones for shared, higher
**  ones for private.  These are assigned to trace_file_number_g at the 
**  beginning of each file.
**
*****************************************************************************/
#define BT_FILE_NUMBER(fnumber)  static int trace_file_number_g = fnumber

#define TRACE_BT_BIT_C              0x1
#define TRACE_BT_DELAY_C            0x2
#define TRACE_BT_EVENT_C            0x3
#define TRACE_BT_ICBR_THREAD_C      0x4
#define TRACE_BT_ISR_C              0x5
#define TRACE_BT_LLIST_C            0x6
#define TRACE_BT_MEM_C              0x7
#define TRACE_BT_MUTEX_C            0x8
#define TRACE_BT_NBUS_C             0x9
#define TRACE_BT_PARAM_C            0xa
#define TRACE_BT_RWLCK_C            0xb
#define TRACE_BT_SETUP_C            0xc
#define TRACE_BT_XFER_C             0xd
#define TRACE_BT_DIAG_C             0xe
#define TRACE_BT_BSP_UNIQUE_C       0xf

#define TRACE_BTP_BIND_C            0x81
#define TRACE_BTP_CDEV_C            0x82
#define TRACE_BTP_CFG_C             0x83
#define TRACE_BTP_CTLS_C            0x84
#define TRACE_BTP_INTR_C            0x85
#define TRACE_BTP_KERN_C            0x86
#define TRACE_BTP_LWIO_C            0x87
#define TRACE_BTP_MMAP_C            0x88
#define TRACE_BTP_RDWR_C            0x89
#define TRACE_BTP_OPEN_C            0x8a

#define TRACE_BTP_IOCTL_C           TRACE_BTP_CTLS_C
#define TRACE_BTP_KERNEL_C          TRACE_BTP_KERN_C
#define TRACE_BTP_LOWIO_C           TRACE_BTP_LWIO_C

/*****************************************************************************
**
**  Swapping values.
**
**  Not all are supported on each platform.
**
**  Use with SWAP_BITS parameter (BIOC_PARAM & BIOC_DEV_ATTRIB)
**
*****************************************************************************/
typedef enum BT_SWAP_BITS_VALUE {
    BT_SWAP_NONE = 0,   /* No swapping */
    BT_SWAP_BSNBD,      /* Byte swap, non byte data */
    BT_SWAP_WS,         /* Word swap */
    BT_SWAP_WS_BSNBD,   /* Word swap and byte swap, non btye data */
    BT_SWAP_BSBD,       /* Byte swap, btye data */
    BT_SWAP_BSBD_BSNBD, /* Btye swap, byte data and byte swap, non byte data */
    BT_SWAP_BSBD_WS,    /* Byte swap, byte data and word swap */
    BT_SWAP_BSBD_WS_BSNBD,  /* All possible swapping */
    BT_SWAP_DEFAULT,    /* Driver default swapping */
    BT_MAX_SWAP
} bt_swap_t;


/*****************************************************************************
**
**  Register access types
**
**  Use with bt_ioaccess_t & bt_pcfg_access_t structure 
**  (BIOC_IOREG & BIOC_PCFG)
**
*****************************************************************************/
enum BT_ACCESS {
    BT_IOREAD,  /* Register read */
    BT_IOWRITE, /* Register write */
    BT_IOSHOT   /* Register bit set */
};

/*****************************************************************************
**
**  Valid registers
**
**  Use with bt_ioaccess_t structure (BIOC_IOREG)
**
*****************************************************************************/
#define BT_LOC_START    0
#define BT_REM_START    80
#define BT_RPQ_START    192

typedef enum BT_REGISTERS {
    BT_LOC_CMD1 = BT_LOC_START,         /* Local Command register 1 */
    BT_LOC_INT_CTRL,                    /* Local Interrupt Control */
    BT_LOC_INT_STATUS,                  /* Local Interrupt Status */
    BT_LOC_STATUS,                      /* Local Status register */
    BT_LOC_HANDSHAKE,                   /* Local Handshake register */
    BT_LOC_CONFIG_CTRL,                 /* Local Configuration Register */
    BT_LOC_RCMD2,                       /* Local access to remote command 2 */
    BT_LOC_RPAGE,                       /* Local access to remote page reg  */
    BT_LOC_AMOD,                        /* Local address modifier */
    BT_LDMA_CMD,                        /* Local DMA Command register */
    BT_LDMA_RMD_CNT,                    /* Local DMA Remainder Cnt register */
    BT_LDMA_ADDR,                       /* Local DMA Address register */
    BT_LDMA_PKT_CNT,                    /* Local DMA Packet count register */
    BT_LOC_TCSIZE,                      /* TURBOChannel size register */
    BT_LOC_256MB_PAGE,                  /* Local 256MB window remote page */
    BT_LOC_RCMD1,                       /* Local access to remote command 1 */
    BT_LOC_BUS_CTRL,                    /* Local bus control reg (PCI) */
    BT_LOC_LOOPBACK,                    /* Local loopback control (PCI Fiber) */
    BT_LOC_MREG_CTRL,                   /* Local mapping register control */
    BT_LOC_SEMA0,                       /* Local semaphore register 0 */
    BT_LOC_SEMA1,                       /* Local semaphore register 1 */
    BT_LOC_SEMA2,                       /* Local semaphore register 2 */
    BT_LOC_SEMA3,                       /* Local semaphore register 3 */
    BT_LOC_SEMA4,                       /* Local semaphore register 4 */
    BT_LOC_SEMA5,                       /* Local semaphore register 5 */
    BT_LOC_SEMA6,                       /* Local semaphore register 6 */
    BT_LOC_SEMA7,                       /* Local semaphore register 7 */

    BT_REM_CMD1 = BT_REM_START,         /* Remote Command register 1 */
    BT_REM_STATUS,                      /* Remote Status register */
    BT_REM_CMD2,                        /* Remote Command register 2 */
    BT_REM_PAGE,                        /* Remote Page Register */
    BT_REM_AMOD,                        /* Remote Address Modifier */
    BT_REM_IACK,                        /* Remote IACK register */
    BT_REM_CARD_ID,                     /* Remote Card ID Register */
    BT_RDMA_ADDR,                       /* Remote DMA Address */
    BT_RDMA_RMD_CNT,                    /* Remote DMA Remainder Count */
    BT_REM_MREG_CTRL,                   /* Remote mapping register control */
    BT_REM_SLAVE_STATUS,                /* Remote slave status */
    BT_REM_SLAVE_CLRERR,                /* Remote slave clear error */
    BT_REM_SEMA0,                       /* Remote semaphore register 0 */
    BT_REM_SEMA1,                       /* Remote semaphore register 1 */
    BT_REM_SEMA2,                       /* Remote semaphore register 2 */
    BT_REM_SEMA3,                       /* Remote semaphore register 3 */
    BT_REM_SEMA4,                       /* Remote semaphore register 4 */
    BT_REM_SEMA5,                       /* Remote semaphore register 5 */
    BT_REM_SEMA6,                       /* Remote semaphore register 6 */
    BT_REM_SEMA7,                       /* Remote semaphore register 7 */

    /*
    ** Do not access these registers unless you have RPQ #601107 
    ** These registers allow direct access to all registers located on
    ** the remote card
    **
    ** For non RPQ #601107 cards these registers will return garbage
    */
    BT_RPQ_REM_LCMD1 = BT_RPQ_START,
    BT_RPQ_REM_LINT_CTRL,
    BT_RPQ_REM_LSTATUS,
    BT_RPQ_REM_LINT_STATUS,
    BT_RPQ_REM_LBUS_CTRL,
    BT_RPQ_REM_LLOOPBACK,
    
/* EAS A64 */
    /*
    ** Do not access these registers unless you have RPQ #601332
    ** These registers allow direct access to all registers located on
    ** the remote card
    **
    ** For non RPQ #601332 cards these registers will return garbage
    */
    BT_RPQ_REM_A64PIO,                  /* Remote card Upper address A64 - PIO */
    BT_RPQ_REM_A64DMA,                  /* Remote card Upper address A64 - DMA */
    
    BT_MAX_REGISTER                     /* Maximum number of registers */
} bt_reg_t;

/* The following member names have been obsoleted:
    BT_REM_LSTATUS    replaced by BT_REM_SLAVE_STATUS
    BT_REM_LCLRERR    replaced by BT_REM_SLAVE_CLRERR
    BT_MAX_REGISTERS  replaced by BT_MAX_REGISTER
*/
#define MAX_IOREGS              (0x50)  /* maximum number of I/O registers */ 

/*****************************************************************************
**
**  CSR Layout - PCI to VMEbus or PCI to PCI Products Only
**
**  Register bit definitions
**
**  Use with bt_ioaccess_t structure (BIOC_IOREG)
**
*****************************************************************************/
typedef struct {

/*
** Local Node Command Register (read/write, 8 bits)
*/
        bt_data8_t         loc_cmd1;
#define LOC_CMD1        (0x00)  /* Local Command Register */

#define LC1_CLR_ERROR   (1<<7)  /* w = 1, Clear status error bits */
#define LC1_CLR_PR_INT  (1<<6)  /* Clear the PR interrupt */
#define LC1_SND_PT_INT  (1<<5)  /* Send a PT interrupt */
#define LC1_PT_STATUS   LC1_SND_PT_INT  /* Keep consistent with PR name */
/* Bits 4-0 must always be set to 0. */

/*
** Local Interrupt Control Register (read/write, 8 bits)
*/
	bt_data8_t		loc_int_ctrl;
#define LOC_INT_CTRL            (0x01)  /* local interrupt control register */
#define LOC_INT_CTRL_SIZE       (BT_WIDTH_D8)
#define LIC_INT_PENDING         (1<<7)  /* r  , set when generating irq     */
#define LIC_INT_ENABLE          (1<<6)  /* r/w, non-error interrupt enable  */
#define LIC_ERROR_INT_ENABLE    (1<<5)  /* r/w, error interrupt enable      */
#define LIC_PT_CBL_MASK         (0x7)   /* mask for PT cable select line    */
#define LIC_PT_CBL_NONE         (0x0)   /* send PT interrupt disabled       */
#define LIC_PT_CBL_1            (0x1)   /* PT interrupt sent on cable irq 1 */
#define LIC_PT_CBL_2            (0x2)   /* PT interrupt sent on cable irq 2 */
#define LIC_PT_CBL_3            (0x3)   /* PT interrupt sent on cable irq 3 */
#define LIC_PT_CBL_4            (0x4)   /* PT interrupt sent on cable irq 4 */
#define LIC_PT_CBL_5            (0x5)   /* PT interrupt sent on cable irq 5 */
#define LIC_PT_CBL_6            (0x6)   /* PT interrupt sent on cable irq 6 */
#define LIC_PT_CBL_7            (0x7)   /* PT interrupt sent on cable irq 7 */

#define LIC_EN_INTR     (LIC_INT_ENABLE | LIC_ERROR_INT_ENABLE | LIC_PT_CBL_1)
#define LIC_DIS_INTR    (LIC_PT_CBL_1)

/*
** This bit definition only works with RPQ #601107 
*/
#define LIC_INT_D_SET           (1<<4)  /* r/w, Set/Clear INTD# RPQ #601107 only */

/*
** Local Node Status Register (read only, 8 bits)
*/
        bt_data8_t         loc_status;
#define LOC_STATUS      (0x02)  /* Local Status Register */

#define LSR_PARITY_ERR          (1<<7)
#define LSR_REMBUS_ERR          (1<<6)
#define LSR_PR_STATUS           (1<<5)
#define LSR_TIMEOUT_ERR         (1<<2)
#define LSR_LRC_ERR             (1<<1)  /* DMA only */
#define LSR_NO_CONNECT          (1<<0)

#define LSR_ERROR_MASK  (LSR_PARITY_ERR|LSR_LRC_ERR|LSR_REMBUS_ERR|LSR_TIMEOUT_ERR)
#define LSR_CERROR_MASK (LSR_NO_CONNECT|LSR_ERROR_MASK)

/*
** Local Interrupt Status Register (read only, 8 bits)
*/
        bt_data8_t		loc_int_status;
#define LOC_INT_STATUS          (0x03)  /* interrupt status register */
#define LOC_INT_STATUS_SIZE     (BT_WIDTH_D8)
#define LIS_CINT7               (1<<7)  /* r  , cable interrupt 7 */
#define LIS_CINT6               (1<<6)  /* r  , cable interrupt 6 */
#define LIS_CINT5               (1<<5)  /* r  , cable interrupt 5 */
#define LIS_CINT4               (1<<4)  /* r  , cable interrupt 4 */
#define LIS_CINT3               (1<<3)  /* r  , cable interrupt 3 */
#define LIS_CINT2               (1<<2)  /* r  , cable interrupt 2 */
#define LIS_CINT1               (1<<1)  /* r  , cable interrupt 1 */
#define LIS_CINT_MASK (LIS_CINT1 | LIS_CINT2 | LIS_CINT3 | LIS_CINT4 | LIS_CINT5 | LIS_CINT6 | LIS_CINT7)

/*
** Local Bus Control Register (read/write, 8 bits)
*/
        bt_data8_t		loc_bus_ctrl;
#define LOC_BUS_CTRL            (0x04)  /* bus control register         */
#define LOC_BUS_CTRL_SIZE       (BT_WIDTH_D8)
#define LBC_GEMS_SWAP_ENABLE    (1<<3)  /* enable lword swap for GEMS   */
#define LBC_WS_ENABLE           (1<<2)  /* enable word swap             */
#define LBC_PREEMPT_DISABLE     (1<<1)  /* disable local preempt        */
#define LBC_TRG_ABORT_ENABLE    (1<<0)  /* target abort enable          */
#if defined(BT965)
#define LBC_NORM_OPER           (LBC_WS_ENABLE)
#else
#define LBC_NORM_OPER           (0x0)
#endif

/*
** Loopback Control Register (read/write, 8 bits)
** Only avaliable on special adapter models, check HW manual
*/
        bt_data8_t		loc_loopback;
#define LOC_LOOPBACK            (0x05)  /* loopback control register    */
#define LOC_LOOPBACK_SIZE       (BT_WIDTH_D8)
#define LLB_LINK_UP             (1<<7)  /* r, link up                   */
#define LLB_REM_LB_ENABLE       (1<<1)  /* enable local loopback */
#define LLB_LOC_LB_ENABLE       (1<<0)  /* enable local loopback */

/*
** Mapping RAM Control Register (read/write, 8 bits)
** Only avaliable on certain adapter models, check HW manual
*/
        bt_data8_t		loc_mreg_ctrl;
#define LOC_MREG_CTRL           (0x06)  /* map reg control register */
#define LOC_MREG_CTRL_SIZE      (BT_WIDTH_D8)
#define LMC_DRIVER_MASTER       (1<<3)  /* local driver master */
#define LMC_DRIVER_ACTIVE       (1<<2)  /* local driver loaded */
#define LMC_DMA_BYPASS_ENABLE   (1<<1)  /* DMA to local bus bypass */
#define LMC_PIO_BYPASS_ENABLE   (1<<0)  /* cable to local bus bypass */

/*
** Undefined area of CSR register space
*/
        bt_data8_t         reserve07;

/*
** Remote Node Command Register 1 (read/write, 8 bits)
*/
        bt_data8_t         rem_cmd1;
#define REM_CMD1        (0x08)
#define RC1_RESET       (1<<7)  /* Reset remote bus */
#define RC1_CLR_PT_INT  (1<<6)  /* Clear PT interrupt */
#define RC1_SND_PR_INT  (1<<5)  /* Send PR interrupt */
#define RC1_LOCK_SEL    (1<<4)  /* Lock Remote bus */
/* Bit 3 must always be set to "0" */
#define RC1_IACK_MASK   (0x7)   /* IACK read level select */

/*
** Remote Node Status Register (read, 8 bits)
** Same ofset as Remote Note command register 1
*/
#define rem_status	rem_cmd1
#define REM_STATUS      (0x08)  /* remote status register */
#define RSR_WAS_RESET   (1<<7)  /* Remote bus was reset */
                                /* Valid only for A32 VMEbus adaptors */
#define RSR_IACK1	(1<<6)  /* IACK read mode address bit 1 */
#define RSR_PR_STATUS   (1<<5)  /* PR Interrupt is set */
#define RSR_NOT_LOCK_STATUS (1<<4)      /* Remote bus is *NOT* locked */
#define RSR_IACK2	(1<<2)  /* IACK read mode address bit 2 */
#define RSR_PT_STATUS   (1<<1)  /* PT interrupt is set */
#define RSR_IACK0	(1<<0)  /* IACK read mode address bit 0 */


/*
** Remote Node Command Register 2 (read/write, 8 bits)
** Nothing of interest in this register for PCI to PCI applications
*/
        bt_data8_t         rem_cmd2;
#define REM_CMD2        (0x09)

#define RC2_DMA_PAUSE   (1<<7)  /* DMA remote pause after 16 xfers     */
/* Bit 6 must always be set to "0". */
#define RC2_AMOD_SEL    (1<<6)  /* Use Address modifier register       */
#define RC2_DMA_BLK_SEL (1<<5)  /* DMA remote use Block Mode           */
#define RC2_CINT_DISABLE (1<<4) /* Disable passing of remote cable intr */


/*
** Undefined area of CSR register space
** This is normally where the VME page register is.  However,
** this unused for PCI to ? application because of mapping RAM
*/
        bt_data16_t        reserve0a;

/*
** Remote Node Card ID Register (read, 8 bits)
*/
        bt_data8_t         rem_card_id;
#define REM_CARD_ID     (0x0c)

/*
** Remote Node Address Modifier Register (read/write, 8 bits)
** Nothing of interest in this register for PCI to PCI applications
*/
        bt_data8_t         rem_amod;
#define REM_AMOD        (0x0d)

/*
** Remote Node Interrupt Acknowledge Register (read, 16 bits)
*/
        bt_data16_t        rem_iack;
#define REM_IACK        (0x0e)

/*
** Remote Node Mapping RAM Control Register (read/write, 8 bits)
** PCI to PCI only, same offset as the Remote Node Interrupt Acknowledge
**
** Use bit definitions for the Local Node Mapping RAM Control Register
*/
#define rem_mreg_ctrl      rem_iack
#define REM_MREG_CTRL   (0x0e)

/*
** DMA Registers
*/
/*
** Local Node DMA Command Register (read/write, 8 bits)
*/
        bt_data8_t         ldma_cmd;
#define LDMA_CMD        (0x10)

#define LDC_START       (1<<7)          /* Start DMA transfer */
#define LDC_DP_SEL      (1<<6)          /* Transfer to/from dual port RAM */
#define LDC_WRITE_SEL   (1<<5)          /* Transfer from local to remote */
#define LDC_DMA_D32_SEL (1<<4)          /* Transfer as longwords */
#define LDC_DMA_INT_ENABLE (1<<2)       /* Enable DMA DONE interrupt */
#define LDC_DMA_DONE    (1<<1)          /* r=1, DMA is complete */
#define LDC_DMA_BLK_SEL (1<<0)          /* Perform burst DMA transfers */

/*
** Local Node DMA Packet Remainder Count (read/write, 8 bits)
*/
        bt_data8_t         ldma_rmd_cnt;
#define LDMA_RMD_CNT    (0x11)

/*
** Local Node DMA Packet Count (read/write, 16 bits)
*/
        bt_data16_t        ldma_pkt_cnt;
#define LDMA_PKT_CNT    (0x12)

/*
** Local Node DMA Address Register (read/write, 32 bits)
** Certain cards only support 24 bits of local DMA address. See HW manual
** Can only be access by bytes or words
*/
        bt_data16_t        ldma_addr_lo;
        bt_data16_t        ldma_addr_hi;
#define LDMA_ADDR       (0x14)
#define LDMA_ADDR_MASK  (0x00ffffff)

/*
** Remote Node DMA Packet Remainder Count (read/write, 8 bits)
*/
        bt_data8_t         rdma_rmd_cnt;
#define RDMA_RMD_CNT    (0x18)

/*
** Undefined area of CSR register space
*/
        bt_data8_t         reserve19;

/*
** Remote Node DMA Address Register (read/write, 32 bits)
** Certain cards only support 24 bits of remote DMA address. See HW manual
** Can only be access by bytes or words
*/
        bt_data16_t        rdma_addr_hi;
        bt_data16_t        rdma_addr_lo;
#define RDMA_ADDR       (0x1a)

/*
** Remote Node Slave Status Register (read, 8 bits)
*/
        bt_data8_t	rem_slave_status;
#define REM_SLAVE_STATUS    (0x1e)  /* remote node slave status register  */
#define RSS_PARITY_ERR      (1<<7)  /* r=1, interface parity error 	  */
#define RSS_REMBUS_ERR      (1<<6)  /* r=1, remote bus error 	          */
#define RSS_PR_STATUS       (1<<5)  /* r=1, pr interrupt (from remote)    */
#define RSS_LRC_ERR         (1<<3)  /* r=1, LRC DMA packet error 	  */
#define RSS_TIMEOUT_ERR     (1<<2)  /* r=1, interface timeout error       */
#define RSS_PT_STATUS       (1<<1)  /* r=1, pt interrupt (to remote)      */
#define RSS_NO_CONNECT      (1<<0)  /* r=1, no connection to remote       */
#define RSS_ERROR_MASK	    (RSS_PARITY_ERR | RSS_REMBUS_ERR \
			        | RSS_LRC_ERR | RSS_TIMEOUT_ERR)

#define RSS_CERROR_MASK	    (RSS_NO_CONNECT | RSS_ERROR_MASK)

/*
** Remote Node Slave Clear Register (write, 8 bits)
** Same ofset as Remote Node Slave Status Register
*/
#define rem_slave_clrerr    rem_slave_status
#define REM_SLAVE_CLRERR    (0x1e)  /* remote node slave clear error reg. */
#define RSC_CLR_ERROR       (1<<7)  /* w=1, clear status error bits       */

/*
** Undefined area of CSR register space
*/
        bt_data8_t          reserve1f;
/*
** Semaphore Registers
*/
/*
** Local Semaphore Registers (read/write, 8 bits)
** All semaphore registers have the same bit definitions
*/
        bt_data8_t          loc_sema0;
        bt_data8_t          loc_sema1;
        bt_data8_t          loc_sema2;
        bt_data8_t          loc_sema3;
        bt_data8_t          loc_sema4;
        bt_data8_t          loc_sema5;
        bt_data8_t          loc_sema6;
        bt_data8_t          loc_sema7;
        
        bt_data8_t          rem_sema0;
        bt_data8_t          rem_sema1;
        bt_data8_t          rem_sema2;
        bt_data8_t          rem_sema3;
        bt_data8_t          rem_sema4;
        bt_data8_t          rem_sema5;
        bt_data8_t          rem_sema6;
        bt_data8_t          rem_sema7;
#define BT_SEMA_TAKE        (1<<0)
#define BT_SEMA_GIVE        (0)

        bt_data8_t          reserved30;
        bt_data8_t          reserved31;
        bt_data8_t          reserved32;
        bt_data8_t          reserved33;
        bt_data8_t          reserved34;
        bt_data8_t          reserved35;
        bt_data8_t          reserved36;
        bt_data8_t          reserved37;
        
        /*
        ** Do not access these registers unless you have RPQ #601107 
        ** These registers allow direct access to all local registers 
        ** located on the remote card
        **
        ** For non RPQ #601107 cards these registers will return garbage
        **
        ** For bit definitions please use the defines for the 
        ** corresponding local registers.
        */
        bt_data8_t          rpq_rem_lloc_cmd1;
        bt_data8_t          rpq_rem_lint_ctrl;
        bt_data8_t          rpq_rem_lstatus;
        bt_data8_t          rpq_rem_lint_status;
        bt_data8_t          rpq_rem_lbus_ctrl;
        bt_data8_t          rpq_rem_lloopback;
        bt_data8_t          reserved3e;
        bt_data8_t          reserved3f;

/* EAS A64 */
        /*
        ** Do not access these registers unless you have RPQ #601332
        ** These registers allow direct access to all local registers 
        ** located on the remote card
        **
        ** For non RPQ #601332 cards these registers will return garbage
        **
        ** For bit definitions please use the defines for the 
        ** corresponding local registers.
        */
        
        bt_data16_t        reserved40;
        bt_data16_t        reserved42;
        bt_data16_t        reserved44;
        bt_data16_t        reserved46;
        /* Remote Node Upper Address PIO Register (read/write, 32 bits) offset 0x48 */
        bt_data16_t        rem_a64pio_addr_lo;
        bt_data16_t        rem_a64pio_addr_hi;
        /* Remote Node Upper Address DMA Register (read/write, 32 bits) offset 0x4c */
        bt_data16_t        rem_a64dma_addr_lo;
        bt_data16_t        rem_a64dma_addr_hi;

} bt_pci_reg_t;

/*****************************************************************************
**
**  Define all possible cable interrupts -- not currently useful
**
*****************************************************************************/

enum BT_CABLE_INTR {
    BT_CINT_1 = 1,              /* Cable interrupt one                      */
    BT_CINT_2,                  /* Cable interrupt two                      */
    BT_CINT_3,                  /* Cable interrupt three                    */
    BT_CINT_4,                  /* Cable interrupt four                     */
    BT_CINT_5,                  /* Cable interrupt five                     */
    BT_CINT_6,                  /* Cable interrupt six                      */
    BT_CINT_7,                  /* Cable interrupt seven                    */
    BT_CINT_MAX                 /* Actually more than the maximum           */
};

/*****************************************************************************
**
**  DEFINE GENERIC INTERRUPT HANDLER MACROS:
**
**  Define LOAD_MAP_REG macro here so loading a mapping register is illegal.
**
*****************************************************************************/

#define LOAD_MAP_REG(map_reg_ptr, rem_addr, addr_mod) \
    /* LOAD_MAP_REG(caddr_t map_reg_ptr, u_long rem_addr, u_char addr_mod) */ \
    /* This macro may be used only in a kernel extension interrupt handler */ \
    panic("LOAD_MAP_REG() is illegal - mappable windows are not supported.")


#define BT_FAIL   (-1)
#define BT_SUCC   0


/*****************************************************************************
**
**  List generic macros below
**
*****************************************************************************/

#define  SET_BIT(base, bits)  ((base) |= (bits))
#define  CLR_BIT(base, bits)  ((base) &= ~(bits))
#define  TST_BIT(base, bits)  (((base) & (bits)) != 0)
#define  IS_CLR(base, bits)   (((base) & (bits)) == 0)
#define  IS_SET(base, bits)   TST_BIT(base, bits)

#define IS_D16_ALIGNED(addr)  (((bt_devaddr_t)(addr) & 0x01) == 0)
#define IS_D32_ALIGNED(addr)  (((bt_devaddr_t)(addr) & 0x03) == 0)
#define IS_D64_ALIGNED(addr)  (((bt_devaddr_t)(addr) & 0x07) == 0)


/*****************************************************************************
**
**  List diagnostic and Mirror API macros below
**
**  For Internal Use Only!
**
*****************************************************************************/
#define BT_DIAG_MREG_OFFSET         (0x0)
#define BT_DIAG_NIO_OFFSET          (0x40000000)
#define BT_DIAG_ISR_Q_OFFSET        (0x40100000)

/* 
** These macros make it easier to update the code below for adding 
** interrupt types, keeping track of which irq queue was
** use to do the malloc, and each q size in bytes.  
**
** Both the library and the driver assume that the interrupt queues
** are assigned in the following order from the malloced or mmaped
** buffer: error, programmed, and then VME/IACK.  
**
** BTK_Q_USE and BTK_Q_NUM are used only by the driver, but included
** here to make them easier to find.
**
** BTK_Q_SIZE stands for the size of each queue including the controlling
** bt_irq_q_t structure.  BTK_Q_NUM * BTK_Q_SIZE is the number of bytes 
** malloced and the number of bytes that should be mmap by the library.
**
** BTK_Q_ASSIGN assigns each queue structure to the proper portion of the
** malloced or mmapped region described by q_start_p.
**
** Note:
** To get the q_size (qz) the library should call bt_get_info with
** BT_INFO_ICBR_Q_SIZE
*/
#define BTK_Q_NUM               (BT_MAX_IRQ - 1)    /* # of irq thread queues */
#define BTK_Q_USE               (unit_p->error_irq_q_p) /* Which queue as used for malloc */
#define BTK_Q_BYTES(q_size)     (sizeof(bt_irq_q_t) + sizeof(bt_data32_t) * (q_size - 1))
#define BTK_Q_SIZE(qz)          ((BTK_Q_BYTES(qz) % sizeof(bt_data64_t)) ? BTK_Q_BYTES(qz) + sizeof(bt_data32_t) : BTK_Q_BYTES(qz))
#define BTK_Q_ASSIGN(q_start_p, err_q_p, prog_q_p, vme_q_p, q_size)  \
        err_q_p = (bt_irq_q_t *) q_start_p; \
        prog_q_p = (bt_irq_q_t *) (q_start_p + BTK_Q_SIZE(q_size)); \
        vme_q_p = (bt_irq_q_t *) (q_start_p + 2 * BTK_Q_SIZE(q_size)); 

/*
**  List all Bit 3 adaptor IDs registered to date.
**  Used in process of determining remote identification.
*/
#define BT_ID_EISA      0x62                /* EISA ID Value                */
#define BT_ID_EISA2     0x41                /* New EISA ID Value            */
#define BT_ID_MCA3_DMA  0x76                /* MCA type 3 w/slave DMA       */
#define BT_ID_SB_DMA    0xDF                /* SBus ID Value                */
#define BT_ID_TC        0x4D                /* TURBOchannel ID Value        */
#define BT_ID_VME_SDMA  0x80                /* VMEbus w/ Slave DMA          */
#define BT_ID_GIO       0x47                /* GIO ID value - ASCII "G"     */
#define BT_ID_PCI_DMA   0xAB                /* PCI card ID value            */
#define BT_ID_PCI       0xAC                /* PCI wo/DMA card ID value     */
#define BT_ID_VME_NODMA 0x81                /* VME wo/DMA card ID value     */
#define	BT_ID_VME_NOINC	0x82		    /* VME RPQ w/ DMA Inc Inhibit   */
#define BT_ID_VME_FIBER 0x83                /* VME Fiberoptic card          */
#define BT_ID_VME_FIBER_D64 0x84            /* VME Fiberoptic card w/D64    */
#define BT_ID_VME_DB    0x85            /* VME dataBLIZZARD w/D64 */
#define BT_ID_PCI_FIBER 0xAD                /* PCI Fiberoptic card          */
#define BT_ID_PCI_FIBER_D64 0xAE            /* PCI Fiberoptic card w/D64    */
#define BT_ID_PCI_DBA64_RPQ 0xAF            /* PCI Fiberoptic card w/A64 RPQ 601332    */

/*****************************************************************************
**
**  Include old type and macros names for backwards compatiblity only.
**
**  Do Not Use These Names For New Development.  
**  These Names May Be Obsoleted At Any Time.
**
*****************************************************************************/
/*******************************************************************************
**
**  Lock device structure (for BIOC_LOCK)
**
*******************************************************************************/
typedef struct bt_lock_d {
    bt_data32_t  error;                 /* Error code                        */
    bt_msec_t    timeout;               /* Maximum lock time in milliseconds */
    bt_data32_t  signum;                /* Signal number to send on timeout  */
    bt_data32_t  nowait;                /* FALSE = wait; TRUE = don't wait   */
} bt_lock_t;

#if defined(__osf__)

typedef u_char  data8_t;
typedef u_short data16_t;
typedef uint    data32_t; /* uint = unsigned int = 32 bits */
typedef u_long  data64_t; /* u_long = 64 bits for Alpha */

#define DATA8_SIZ    sizeof(data8_t)
#define DATA16_SIZ   sizeof(data16_t)
#define DATA32_SIZ   sizeof(data32_t)
#define DATA64_SIZ   sizeof(data64_t)
#define DATA_ANY_SIZ 0

#elif defined(_sun) /* _sun */

typedef uint8_t         data8_t;
typedef uint16_t        data16_t;
typedef uint32_t        data32_t;
typedef uint64_t        data64_t;

#define DATA8_SIZ    sizeof(data8_t)
#define DATA16_SIZ   sizeof(data16_t)
#define DATA32_SIZ   sizeof(data32_t)
#define DATA64_SIZ   sizeof(data64_t)
#define DATA_ANY_SIZ 0

#else /* __osf__ */

typedef unsigned char  data8_t;
typedef unsigned short data16_t;
typedef unsigned long  data32_t;

#define DATA8_SIZ    sizeof(data8_t)
#define DATA16_SIZ   sizeof(data16_t)
#define DATA32_SIZ   sizeof(data32_t)
#define DATA_ANY_SIZ 0

#endif /* __osf__ */

#define NIBL_MASK 0xF
#define NIBL_SHFT 4
#define BYTE_MASK 0xFF
#define BYTE_SHFT 8
#define WORD_MASK 0xFFFF
#define WORD_SHFT 16
/* #define LONG_MASK 0xFFFFFFFF  conflict with ntddk.h */

#define WORD_ALIGN(addr)      (((bt_data32_t)(addr) & 0x01) == 0)
#define LONG_ALIGN(addr)      (((bt_data32_t)(addr) & 0x03) == 0)
#define LONGLONG_ALIGN(addr)  (((bt_data32_t)(addr) & 0x07) == 0)

#define ISBTCLR(base, bits)  (((base) & (bits)) == 0)
#define ISBTSET(base, bits)  TST_BIT(base, bits)

#define VME_A32_SB  0x0F    /* A32 Extended Supervisory Block Transfer      */
#define VME_A32_SP  0x0E    /* A32 Extended Supervisory Program Access      */
#define VME_A32_SD  0x0D    /* A32 Extended Supervisory Data Access         */
#define VME_A32_NB  0x0B    /* A32 Extended Non-Privileged Block Transfer   */
#define VME_A32_NP  0x0A    /* A32 Extended Non-Privileged Program Access   */
#define VME_A32_ND  0x09    /* A32 Extended Non-Privileged Data Access      */

#define VME_A32     VME_A32_SD

#define VME_A24_SB  0x3F    /* A24 Standard Supervisory Block Transfer      */
#define VME_A24_SP  0x3E    /* A24 Standard Supervisory Program Access      */
#define VME_A24_SD  0x3D    /* A24 Standard Supervisory Data Access         */
#define VME_A24_NB  0x3B    /* A24 Standard Non-Privileged Block Transfer   */
#define VME_A24_NP  0x3A    /* A24 Standard Non-Privileged Program Access   */
#define VME_A24_ND  0x39    /* A24 Standard Non-Privileged Data Access      */

#define VME_A24     VME_A24_SD
#define VME_A24_MAX 0x1000000

#define VME_A16_SD  0x2D    /* A16 Short Supervisory Data Access            */
#define VME_A16_ND  0x29    /* A16 Short Non-Privileged Data Access         */

#define VME_A16     VME_A16_SD

/*
**  List of any card specific values here.
*/
#define BT_QB_PG_MAX    0x3F                /* Qbus max page register value */

/*****************************************************************************
**
**  Kernel Level Information
**
**  NOT Useful or Visable To Applications
**
**  Do not use because these things change often and are not supporred
**  at the application level.
**
*****************************************************************************/

/*
**  Logical devices and unit macros
*/
#define BT_MOCK_UNIT   99               /* When real physical unit is unknown */

#define	BT_DEV_SHFT	(5)
#define	BT_AXS_SHFT	BT_DEV_SHFT	/* Older name */

/*
** Linux requires that BT_MAX_UNITS be a simple integer with no parathesis
** or operators. It MUST NOT be an expression.
*/
#define	BT_MAX_UNITS	31	/* Don't change or you will break Linux */
#if	BT_MAX_UNITS != ((1 << BT_DEV_SHFT) - 1)
#error	"Shift and maximum values out of sync."
#endif	/* BT_MAX_UNITS */

#if defined(_BTDD_H)
#ifdef __sgi
extern minor_t btp_get_minor_no (dev_t dev);
#define BT_UNIT(dev)   ((int)(((long) btp_get_minor_no(dev)) & BT_MAX_UNITS))
#define BT_AXSTYP(dev) ((int)(((long) btp_get_minor_no(dev)) >> BT_DEV_SHFT))

#elif defined(__sun)
#define BT_UNIT(dev)   (int)(getminor(dev) & BT_MAX_UNITS)
#define BT_AXSTYP(dev) (int)(getminor(dev) >> BT_DEV_SHFT)

#elif defined(__linux__)

#if     LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
#define BT_UNIT(dev)   (int)(MINOR(dev) & BT_MAX_UNITS)
#define BT_AXSTYP(dev) (int)(MINOR(dev) >> BT_DEV_SHFT)
#else
#define BT_UNIT(dev)   (int)(minor(dev) & BT_MAX_UNITS)
#define BT_AXSTYP(dev) (int)(minor(dev) >> BT_DEV_SHFT)
#endif  /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0) */

#else

#define BT_UNIT(dev)   (int)(minor(dev) & BT_MAX_UNITS)
#define BT_AXSTYP(dev) (int)(minor(dev) >> BT_DEV_SHFT)

#endif

/* 
** Used to circumvent lseek() deficiency of using signed 32bit values 
*/
#define RE_ADJUST (0x80000000UL)

/*
** Types of mapping registers
*/
typedef enum {
    BT_LMREG_PCI_2_CABLE,
    BT_LMREG_CABLE_2_PCI,
    BT_LMREG_DMA_2_PCI,
    BT_RMREG_PCI_2_CABLE,
    BT_RMREG_CABLE_2_PCI,
    BT_RMREG_DMA_2_PCI,
    BT_MREG_PRG_VECTOR = 0x1ffe,
    BT_MREG_HW_TRACE = 0x1fff
} bt_mreg_t;

/*
** Vector values used for intra-driver communications
**
** All values above BT_DRV_VECTOR_BASE are reserved
** for SBS use only.
*/
#define BT_DRV_VECTOR_BASE              0xff000000
#define BT_DRV_VECTOR_PEER2PEER         BT_DRV_VECTOR_BASE + 1
#define BT_DRV_VECTOR_SEMA_RELEASE      BT_DRV_VECTOR_BASE + 2

/*
** Paging structure for the driver
*/
typedef struct {
    size_t          page_size;
    size_t          page_offset;
    bt_data32_t     page_number;
    unsigned int    mreg_start;
    unsigned int    mreg_need;
    caddr_t         bus_base_p;
} btk_page_t;

/*
** ICBR thread tracking data
*/
typedef struct {
    bt_event_t      thread_event;
    bt_data64_t     thread_id;
    int             error_count;
    int             prog_count;
    int             vme_count;
} bt_ttrack_t;

/*
** Various types of device operations
*/
typedef enum {
    BT_OP_OPEN,     /* Only used with btk_invalid_op() */
    BT_OP_CLOSE,    /* Only used with btk_invalid_op() */
    BT_OP_READ,     /* Only used with btk_invalid_op() */
    BT_OP_WRITE,    /* Only used with btk_invalid_op() */
    BT_OP_MMAP,
    BT_OP_BIND,
    BT_OP_DMA,      /* Only used with btk_setup_mreg() */
    BT_OP_PIO,      /* Only used with btk_setup_mreg() */
    BT_MAX_OP
} bt_operation_t;

/*
** Mapping register constants
*/
#define BT_MREG_SWAP_SHIFT      (1)             /* Swap shift */
#define BT_MREG_SWAP_MASK       (0xe)           /* Swap mask */
#define BT_MREG_FUNCT_SHIFT     (4)             /* Function shift */
#define BT_MREG_FUNCT_MASK      (0x30)          /* Function mask */
#define BT_MREG_AMOD_SHIFT      (6)             /* Amod shift */
#define BT_MREG_AMOD_MASK       (0xfc0)         /* Amod mask */
#define BT_MREG_ADDR_SHIFT      (12)            /* Address shift */
#define BT_MREG_ADDR_MASK       (0xfffff000)    /* Address mask */

#define BT_MREG_INVALID         (1<<0)  /* Invalidate mapping register */
#define BT_MREG_FUNCT_PCFG      (0)     /* Remote configurationfunction code */
#define BT_MREG_FUNCT_RIO       (1)     /* Remote I/O function code */
#define BT_MREG_FUNCT_RRAM      (2)     /* Remote RAM  function code */
#define BT_MREG_FUNCT_DPORT     (3)     /* Dual Port RAM  function code */

/*
** PCI card constants
*/
#define BT_CCR_DISABLE_DEV      (0x0)           /* disables the 617 */
#define BT_CCR_NORM_OPER        (0x46)          /* normal operation mode */

#define BT_CSR_RESET_ERR        (0xffff)        /* reset any errors */

#define BT_CLT_MAX_VALUE        (0xe0)          /* Max. latency timer */

/*
** PCI BAR 2, mapping registers, sizes
** _P2P indicates sizes or offsets for PCI to PCI applications
** _NGEN indicates sizes for next generation adapters
*/
#define BT_CMR_SPACE_SIZE       (0x10000)       /* 64k of mapping regs */
#define BT_CMR_SPACE_SIZE_NGEN  (0x80000)       /* 512k of mapping regs NEXT_GEN*/
#define BT_CMR_REG_SIZE         (BT_WIDTH_D32)  /* size of a mapping reg */
#define BT_617_PAGE_SIZE        (0x1000)    /* 4K page size */

/*
** Offsets and sizes in to the mapping register windows for the various
** adapter windows PCI to cable, local dual port, etc.
*/
/*
** Local PCI to cable mapping register window constants (Remote memory)
** _P2P indicates sizes or offsets for PCI to PCI applications
** _NGEN indicates sizes for next generation adapters
*/
#define BT_CMR_RMEM_OFFSET      (0x0) 
#define BT_CMR_RMEM_SIZE        (0x8000)
#define BT_CMR_RMEM_SIZE_P2P    (0x4000) 

/*
** Local Cable to PCI mapping register window constants (Local memory)
** _P2P indicates sizes or offsets for PCI to PCI applications
** _NGEN indicates sizes for next generation adapters
*/
#define BT_CMR_LMEM_OFFSET      (BT_CMR_RMEM_SIZE)
#define BT_CMR_LMEM_SIZE        (0x4000)
#define BT_MAX_SDMA_LEN         (BT_CMR_LMEM_SIZE*BT_617_PAGE_SIZE/BT_CMR_REG_SIZE)

/*
** Local DMA to PCI mapping register window constants (DMA memory)
** _P2P indicates sizes or offsets for PCI to PCI applications
** _NGEN indicates sizes for next generation adapters
*/
#define BT_CMR_LDMA_OFFSET      (BT_CMR_LMEM_OFFSET + BT_CMR_LMEM_SIZE)
#define BT_CMR_LDMA_SIZE        (0x4000) 
#define BT_MAX_DMA_LEN          BT_MAX_SDMA_LEN

/*
** Local Dual Port memory portion of mapping registers (LDP memory)
** _P2P indicates sizes or offsets for PCI to PCI applications
** This only exists on next generation products so _NGEN has been dropped
*/
#define BT_CMR_DP_OFFSET        (BT_CMR_LDMA_OFFSET + BT_CMR_LDMA_SIZE)
#define BT_CMR_DP_SIZE          ((BT_CMR_SPACE_SIZE_NGEN/2)-BT_CMR_DP_OFFSET)

/*
** Remote mapping registers are identical to local ones just add
** half the total mapping registers
*/
#define BT_CMR_REMOTE_OFFSET    (BT_CMR_SPACE_SIZE_NGEN/2)

/*
** PCI Bar 3, remote memory window sizes
*/
#define BT_CRM_SPACE_SIZE       (0x2000000)
#define BT_CRM_SPACE_SIZE_P2P   (0x1000000)

#define BT_MAX_MMAP_BIT  (BT_CMR_RMEM_SIZE/BT_CMR_REG_SIZE) /* Max bit in mmap bit map */
#define BT_MAX_SDMA_BIT  (BT_CMR_LMEM_SIZE/BT_CMR_REG_SIZE) /* Max bit in sdma bit map */

#if defined(__vxworks)

/*
**  OS specific unit structure map structure for accessing remote resouces
*/
typedef struct { 
    
    caddr_t     nio_p;         /* Ptr to node registers */
    caddr_t     mreg_p;        /* ptr to allocated mapping register */
    caddr_t     rmem_p;        /* ptr to remote memory to use */
    bt_data32_t loc_id;        /* Id of local card */
    bt_data32_t rem_id;        /* Id of remote card */
    bt_data32_t nio_len;       /* Lenght of node regs */
    bt_data32_t mreg_start;    /* Starting mapping register allocated */
    bt_data32_t mreg_len;      /* Number of isr mapping regs */
    bt_data32_t rmem_len;      /* Length of isr remote memory */
    bt_swap_t   default_swap;  /* Default swapping for adaptor */
} bt_kmap_t;                   /*   May be wrong if no remote power */
                               /*   during load or if remote */
                               /*   adaptor is changed without */
                               /*   rebooting */


/*
** Prototypes for kernel functions
*/
extern int bt_kmap(unsigned int unit, bt_kmap_t *kmap_p);
extern int bt_kunmap(unsigned int unit, bt_kmap_t *kmap_p);


#endif /* defined __vxworks */

#if (defined(BT983) || defined(BT984))
    /* additions for XP */
#define PCI_NUM_BARS                    7      // Total number of PCI BAR registers
#define MAX_NAME_LENGTH                 0x20   // Max length of registered device name

// Device Location Structure
typedef struct _DEVICE_LOCATION
{
    u_long DeviceId;
    u_long VendorId;
    u_long BusNumber;
    u_long SlotNumber;
    UCHAR SerialNumber[16];
} DEVICE_LOCATION;

// PCI BAR Space information 
typedef struct _PCI_BAR_INFO
{
    u_long             *pVa;                    // BAR Kernel Addresses
    PHYSICAL_ADDRESS  Physical;                // BAR Physical Addresses
    u_long             Size;                    // BAR size
    BOOLEAN           IsIoMapped;              // Memory or I/O mapped?
    BOOLEAN           IsUserMappable;          // Can this BAR be mapped?
    PMDL              pMdl;                    // MDL for the BAR space
} PCI_BAR_INFO;
#endif


/******************************************************************************
**
** Information held by driver on each unit
**
** Unit Structure
**
******************************************************************************/
typedef struct bt_unit_d {

/*****
****** START OF OS UNIQUE UNIT STUCTURE INFORMATION 
*****/

    /*
    ** SGI 965 specific infomation
    */
#if defined(BT965)
    vertex_hdl_t dev_vertex;            /* Vertex to the device */
    pciio_piomap_t cfg_map;             /* Mappings for various windows */
    pciio_piomap_t nio_map;
    pciio_piomap_t mreg_map;
    pciio_piomap_t rmem_map;
    struct buf      *dmabuf_p;          /* Current buf structure pointer */
    pciio_dmamap_t  dma_map;            /* DMA map pointer */
    alenlist_t      p_list;             /* Address list- PCI physical */
    alenlist_t      v_list;             /* Address list- kernel virtual */
    struct bt_kmap_s *window_p;         /* Adaptor window information */
    pciio_intr_t  intr_hdl;             /* Handle for isr attach routine */
    device_desc_t device_desc;          /* device descriptor */
    lock_t  isr_lock;                   /* Lock for interrupt service rouitne */
    struct buf  *lm_buf_p;              /* Pointer to local memory device buf */
    caddr_t     lm_raw_p;               /* Original local memory pointer to free */
    int         lm_raw_len;             /* Original local memory allocated */
    pciio_dmamap_t lm_map_p;            /* DMA map used for local memory */
    vertex_hdl_t logdev_node[BT_MAX_AXSTYPS];   /* The verticies created for the
                                                   logical devices. */
    int         first_dma;
    unsigned int        lm_start;       /* Starting mreg of local memory */
    unsigned int        lm_need;        /* Number of mregs need for local memory */
#elif defined(BT993)

    /*
    ** The VxWorks device header must be the first thing in its unit 
    ** sturcture! DO NOT MOVE vxw_hdr. 
    */
    DEV_HDR       vxw_hdr;         /* VxWorks device header */

    /*
    **    pci device information 
    */
    int           pci_bus;         /* PCI Bus number */
    int           pci_device;      /* PCI device number */
    int           pci_func;        /* PCI function number */
    int           pci_bus_type;    /* PCI bus type (internal, pri, sec) */
    char         *name_p;          /* Base name of the device */

    /*
    **    Mapping register information 
    */
    char         *cfg_map;         /* Physical PCI addr of config registers  */
    char         *nio_map;         /* Physical PCI addr of CSR registers  */
    char         *mreg_map;        /* Physical PCI addr of map registers  */
    char         *rmem_map;        /* Physical PCI addr of remote windows */


    /*
    **  Interrupt related variables
    */
    bt_kmap_t     window;          /* Adapter window information */
    int           disp_stack;      /* Stack size of dispatch task */
    int           disp_prio;       /* Priority of dispatch task */

    /*
    **  needed for VMEbus to PCI slave DMA accesses
    */
    unsigned int  mr_page;         /* Mapping regs per page */


    /*
    ** Keep track of open()s, close()s, and logical devices.
    */
    unsigned long ref_count;                /* Current total number of open()s */
    long          open_total;               /* Opens across all logical devs */

    
    /*
    **  Local memory information
    */
    int           lm_raw_len;       /* Original local memory allocated */
    caddr_t       lm_raw_p;         /* Original local memory allocated */


    /*
    ** Local Memory (BT_DEV_LM) device information 
    */
    char           *lm_phys_addr;   /* VMEbus A24/A32 address of BT_DEV_LM */
    bt_addr_width_t lm_space;	    /* Address Width: BT_AW_A32 or BT_AW_A24 */
    char           *lm_base_p;      /* Local bus address of BT_DEV_LM */
    unsigned int    lm_b_resid;     /* need to release resources for lm device */
    unsigned int    lm_b_flags;
    
    /*
    ** Windows NT 983 984 specific infomation
    */
#elif (defined(BT983) || defined(BT984))
    bt_pci_config_t     pci_config;     /* PCI configuration from HAL */
    PADAPTER_OBJECT     adapter_object; /* DMA adapter object from HAL */
    PHYSICAL_ADDRESS    q_paddr;        /* Physical bus address of interrupt queue */
    bt_data32_t         open_cnt;       /* Number of opens */
    unsigned int        uisr_mreg;      /* Which mapping reg is for user ISRs */
    KSPIN_LOCK          isr_lock;       /* Lock for interrupt service rouitne */
    PHYSICAL_ADDRESS    lm_paddr;       /* Physical bus address of local memory */
    unsigned int        lm_start;       /* Starting mreg of local memory */
    unsigned int        lm_need;        /* Number of mregs need for local memory */
    UNICODE_STRING      params_path;    /* registry path for unit parameters */

    /* additions for XP */
    int                 dpc_busy_flag;  /* set to TRUE if the DPC is running */
    KDPC                irq_dpc;
    KSPIN_LOCK          irq_sl;         /* Lock for interrupt service rouitne */
    KEVENT              evRemove;
    bt_data32_t         usage;
    PDEVICE_OBJECT      pPhysicalDeviceObject;
    PDEVICE_OBJECT      pDeviceObject;            // Device object this extension belongs to
    PDEVICE_OBJECT      pLowerDeviceObject;
    DEVICE_LOCATION     Device;
    WCHAR               DeviceLinkNameBuffer[MAX_NAME_LENGTH];
    PCI_BAR_INFO        PciBar[PCI_NUM_BARS];
    // Hardware locking parameters
    KSPIN_LOCK          HwAccessLock;
    LIST_ENTRY          InterruptEventList;
    KSPIN_LOCK          InterruptEventListLock;
    LIST_ENTRY          UserAddressMappingList;
    KSPIN_LOCK          UserAddressMappingListLock;
    LIST_ENTRY          UserCommonBufferMappingList;
    KSPIN_LOCK          UserCommonBufferMappingListLock;
    DEVICE_POWER_STATE  Power;
    bt_data32_t *       PowerIdleCounter;
    // Interrupt handling variables
    PKINTERRUPT         pInterruptObject;
    BOOLEAN             removing;

    /*
    ** Solaris Model 946 specific infomation
    */
#elif defined(BT946)
    dev_info_t  *devinfo_p;             /* Device information */
    
    ddi_acc_handle_t  nio_regs_hdl;     /* Node register handle */
    ddi_acc_handle_t  mregs_hdl;        /* mapping register handle */
    ddi_acc_handle_t  rmem_hdl;         /* Remote memory window handle  */

    ddi_dma_handle_t    dma_handle;     /* Put here for btp_intr().    */
    ddi_iblock_cookie_t iblock_cookie;  /* Returned by ddi_add_intr(). */
    
    struct buf  *lm_buf_p;              /* Pointer to buf structure for local memory */
    int         lm_start;               /* Starting mapping reg for local memory */
    int         lm_need;                /* Number of mapping regs needed for local memory */
    bt_data32_t lm_offset;              /* Offset to align local memory to 4k page */
    kmutex_t    isr_lock;               /* Lock for interrupt service rouitne */
    kmutex_t    kddi_mutex;             /* Lock for interrupt service rouitne */
    
    bt_data32_t buf_offset;             /* uio_offset from read/write for DMA */
    kcondvar_t  buf_condvar;            /* Buf condvar per unit.       */

    bt_data32_t rem_ram_addr;           /* Starting address of REM RAM window on VME */
    
#elif defined(BT1003)

    struct pci_dev *dev_p;		/* Location of device in PCI space */

    /* 
    ** PCI configuration information  -- NOT FROM PCI CONFIG REGISTERS 
    */
    unsigned long csr_phys_addr;    /* physical address of Node I/O */
    unsigned long rr_phys_addr;     /* physical address of remote ram */
    unsigned long mr_phys_addr;     /* physical address of mapping ram */
    unsigned int irq;               /* System IRQ level */

    bt_cookie_t hirq_cookie;	    /* Hardware IRQ cookie */
    bt_cookie_t sirq_cookie;	    /* Software IRQ cookie */
    bt_cookie_t task_cookie;	    /* Task level cookie, no special context */

    unsigned int    mr_page;               /* Mapping regs per page */

    bt_mutex_t  llist_mutex;        /* Protect non-isr linked lists */
    /* Related shared members are: llist_bind_mutex and llist_map_mutex. */

    /*  [Used only by btk_mutex_init() and btk_mutex_fini() in btp_attach().]
    **  Locking variables that do inhibit the interrupt service routine
    */
    bt_mutex_t      isr_lock;       /* Lock for interrupt service rouitne */

    /*
    **  DMA related parameters
    */
    void *          dma_buf_p;      /* Kernel intermediate data buffer */
    size_t          dma_buf_size;   /* Size of buffer */
#if     LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    unsigned int    linux_dma_lock; /* actual dma lock as can no longer hold a spinlock through an interrupt */
#endif

    /* 
    **  Logical device variables
    */
    long        open_cnt[BT_MAX_AXSTYPS];       /* Opens on this logical dev */


    /*
    **  Local memory information
    */
    caddr_t         lm_kaddr;       /* Kernel address of local memory */
    unsigned int    lm_start;       /* Starting mapping reg */
    unsigned int    lm_need;        /* Number mapping regs needed */

    /* 
    **  Misc variables
    */
    long        open_total;     /* Opens across all logical devs */

#else /* By Product */
#error Unknown product
#endif /* By Product */


/*****
****** START OF COMMON UNIT STUCTURE INFORMATION
*****/

    volatile bt_data32_t   bt_status;  /* Status word for this unit */
#define BT_OPEN_INHIBIT     (1<< 0)     /* Inhibit device open */
#define BT_IS_OPEN          (1<< 1)     /* At least one unit is open */
#define BT_ONLINE           (1<< 2)     /* Set if Device is online */
#define BT_PRIV             (1<< 3)     /* Set if in privileged mode */
#define BT_LOGERR           (1<< 4)     /* Set if status interrupt errlog EN */
#define BT_LM_EN            (1<< 5)     /* Set if Local Memory is ENabled */
#define BT_INT              (1<< 6)     /* Set if interrupt handler registrd */
#define BT_DMA_FIFO         (1<< 7)     /* Set if DMA to FIFO */
#define BT_RESET            (1<< 8)     /* Set if a reset is pending */
#define BT_DMA_ERROR        (1<< 9)     /* Set if error during DMA */
#define BT_POWER            (1<<10)     /* Set if NO remote connection */
#define BT_ERROR            (1<<11)     /* Set if an Error has occurred */
#define BT_DMA_LOCAL        (1<<12)     /* Set if local adaptor does DMA */
#define BT_DMA_AVAIL        (1<<13)     /* Set if adaptor pair can does DMA */
#define BT_DMA_BLOCK        (1<<14)     /* Set if use block mode for DMAs */
#define BT_DMA_WAIT         (1<<15)     /* Set if DMA is in PAUSE mode */
#define BT_PCI2PCI          (1<<16)     /* Set if remote card is PCI based */
#define BT_NEXT_GEN         (1<<17)     /* Set if adapteris next gen design */
/*                          (1<<18)        Not Used */
#define BT_DMA_ACTIVE       (1<<19)     /* A DMA is in progress */
#define BT_RB_INTR          (1<<20)     /* Rembus intr handler is registered */
#define BT_PT_INTR          (1<<21)     /* Prog intr handler is registered */
#define BT_ER_INTR          (1<<22)     /* Error intr handler is registered */
#define BT_SEND_PT          (1<<23)     /* When set, use PT when sending irq */
#define BT_PEER2PEER        (1<<24)     /* Set if remote driver is installed */
#define BT_MASTER           (1<<25)     /* Set if use local sem7 to control */
                                        /* DMA otherwise use remote sema7 */ 
#define BT_LOOPBACK         (1<<26)     /* Set if loopback connector is installed */
#define BT_GEMS_SWAP        (1<<27)     /* Set if GEMS swapping is in effect */
/*                          (1<<28)        Not Used */
#define BT_BUF_WAIT         (1<<29)     /* Another is waiting to enter strategy, Solaris only */
#define BT_BUF_LOCK         (1<<30)     /* Process currently entering stratedy, Solaris only */
/*                          (1<<31)        Not Used */

/* 
**  Format string for %b printf() of bt_status flags 
*/
#define BT_STATUS_FORMAT "\20\40NOTUSED\37BUFLOCK\36BUFWAIT\35NOTUSED\34NOTUSED\33NOTUSED\32MASTER\31PEER2PEER\30NOTUSED\27ER_INTR\26PT_INTR\25RB_INTR\24DMA_ACTIVE\23NOTUSED\22NEXT_GEN\21PCI2PCI\20DMA_WAIT\17NOTUSED\16DMA_AVAIL\15DMA_LOCAL\14ERROR\13POWER\12TRANSMIT\11RESET\10HANDSHAKE\7INT\6DPLOC\5LOGERR\4PRIV\3ONLINE\2IS_OPEN\1OPEN_INHIBIT"

    /*
    **  Physical card information
    */
    bt_data8_t  unit_number;            /* Physical unit number */
    bt_data32_t loc_id;                 /* Part number of local card */
    bt_data32_t rem_id;                 /* Part number of remote card */
    bt_data8_t  board_revision;         /* PCI config rev id register value */
    bt_data8_t  driver_version[BT_DIAG_MAX_REV_INFO];   /* Driver version string */

    /*
    **  Mapping register information
    */
    caddr_t cfg_p;                        /* Pointers for various windows */
    caddr_t nio_p;
    caddr_t mreg_p;
    caddr_t rmem_p;
    bt_pci_reg_t *csr_p;                /* Pointer to CSRs */

    /*
    **  DMA related parameters
    */
    int             dma_threshold;      /* DMA transfer threshold value */
    int             dma_poll_size;      /* Size at which < to use polled DMA */
    bt_msec_t       dma_timeout;        /* DMA watchdog timer value (1 msec) */
    bt_event_t      dma_event;          /* Synchronization for DMA completion */
    bt_mutex_t      dma_mutex;          /* Prevents two DMAs from occurring */
    btk_timeout_t   watchdog_id;        /* DMA watchdog timeout id */

    /* 
    **  queue heads for various stages of interrupt processing 
    */
    btk_llist_t qh_err_fn;             /* Error interrupt handlers */
    btk_llist_t qh_prg_fn;             /* Programmed interrupt handlers */
    btk_llist_t qh_iack_fn;            /* Remote bus interrupt handlers */
    btk_llist_t icbr_thread_list;      /* List of ICBR threads to wake */
    bt_irq_q_t  *error_irq_q_p;        /* Error interrupt queue */
    bt_irq_q_t  *prog_irq_q_p;         /* Programmed  interrupt queue */
    bt_irq_q_t  *vme_irq_q_p;          /* VME interrupt queue */
    bt_data32_t q_size;                /* Interrupt q size */

    /* 
    **  Interrupt servicing and status information
    */
    volatile int   sig_count;          /* Number of signals sent to processes */
    volatile int   sig_prg_cnt;        /* ... from programmed interrupts. */
    volatile int   sig_err_cnt;        /* ... from error interrupts. */
    volatile int   sig_other_cnt;      /* ... for any other reason. */

    /*
    **  Interrupt related variables
    */
    btk_timeout_t wakeup_id;            /* Remote bus reset timeout id */

    /* 
    **  Logical device variables
    */
    caddr_t     kern_addr[BT_MAX_AXSTYPS];      /* Kernel addr of windows */
    bt_data32_t kern_length[BT_MAX_AXSTYPS];    /* Kernel size of windows */
    int         pio_addr_mod[BT_MAX_AXSTYPS];   /* Addr modifiers used for PIO */
    int         dma_addr_mod[BT_MAX_AXSTYPS];   /* Addr modifiers used for DMA */
    int         mmap_addr_mod[BT_MAX_AXSTYPS];  /* Addr modifiers used for MMAP */
    int         swap_bits[BT_MAX_AXSTYPS];      /* Swap bit each logical dev */
    int         data_size[BT_MAX_AXSTYPS];      /* Data transfer size */
    int         open[BT_MAX_AXSTYPS];           /* Device currently open */
    bt_data32_t logstat[BT_MAX_AXSTYPS];        /* Device status */
#define  STAT_ONLINE     (1<<0)         /* Set if logical device is online */
#define  STAT_READ       (1<<2)         /* Set if can read logical device */
#define  STAT_WRITE      (1<<3)         /* Set if can write logical device */
#define  STAT_BIND       (1<<4)         /* Set if can bind logical device */
#define  STAT_DMA        (1<<5)         /* Set if can use DMA to go to dev */
#define  STAT_LOCAL      (1<<6)         /* set if this device is local */
#define  STAT_MMAP       (1<<7)         /* Can logical device use mmap */

/* 
**  Format string for %b printf() of logstat flags 
*/
#define BT_LOGSTAT_FORMAT "\20\10MMAP\7LOCAL\6DMA\5NOTUSED\4WRITE\3READ\2OPEN\1ONLINE"


    /* 
    **  Resources necessary for tracking mmap and slave DMA requests.
    **  Pointers are to bit maps of available mapping registers, 
    **  One bit map is needed for PCI to VMEbus mmap requests and the second is
    **  needed for VMEbus to PCI slave DMA accesses.
    */
    void        *mmap_aval_p;
    void        *sdma_aval_p;
    btk_llist_t qh_mmap_requests;      /* Used to track the mmap requests */
    btk_llist_t qh_bind_requests;      /* Used to track the bind requests */

    /*
    **  Locking variables that do not effect the interrupt service routine
    */
    bt_mutex_t  unit_lock;          /* General unit locking */
    bt_mutex_t  open_mutex;         /* Prevents open and close from clashing */
    bt_mutex_t  mreg_mutex;         /* Protects the mreg bit maps while alloc or free */
    bt_mutex_t  llist_bind_mutex;   /* Protect bind linked lists */
    bt_mutex_t  llist_map_mutex;    /* Protect map linked lists */
    bt_mutex_t  prg_irq_mutex;      /* Protect against sending two prog interrupt at same time */
    bt_rwlock_t hw_rwlock;          /* Prevents TAS/CAS & PIO from clashing */

    /*
    **  Local memory information
    */
    int         lm_size;                /* User requested local memory size */

    /* 
    **  Misc variables
    */
    bt_msec_t           reset_timer;    /* Time in 10ms to wait for reset */
    volatile bt_data8_t last_error;     /* ORed value of last error(s) */

    int                 pio_count;      /* track active pio's             */
/* EAS A64 */
    unsigned int        a64_offset;     /* RPQ 601332 A32-A63 and mode bit */
/* EAS BIND CODE */
    unsigned int    bt_kmalloc_size;    /* size of kernel buffer to kmalloc for bind */
    unsigned int    *bt_kmalloc_buf;    /* aligned kernel buffer kmalloced for bind */
    unsigned int    *bt_kmalloc_ptr;    /* kernel buffer kmalloced for bind (save for free) */

} bt_unit_t;


#if defined(__vxworks)

/* The lseek_off must be tracked with each open call */
typedef struct {
    /* Stuff to keep track of for each file descriptor openned */
    bt_unit_t    *unit_p;
    int           unit;
    bt_dev_t      logdev;
    long          lseek_off;
} bt_fd_t;

typedef bt_fd_t *bt_dev_id_t;

#endif /* defined __vxworks */

#endif /* _BTDD_H */


/******************************************************************************
**
**  PCI configuration register defines
**
******************************************************************************/
#define BT_MAX_CFGREG   64

/*
**  Vendor ID Register
*/
#define CFG_VENDOR_ID           (0x0)           /* vendor ID register */
#define CFG_VENDOR_ID_SIZE      (BT_WIDTH_D16)

#define CVI_BIT3_ID             (0x108A)        /* Bit 3 Computer Corp ID */  
#define BT_MAX_POSREG 7

/*
**  Device ID Register
*/
#define CFG_DEVICE_ID           (0x2)           /* device ID register */
#define CFG_DEVICE_ID_SIZE      (BT_WIDTH_D16)

/*
**  Command Register
*/
#define CFG_COMMAND             (0x4)           /* command register */
#define CFG_COMMAND_SIZE        (BT_WIDTH_D16)

/*
**  Status Register
*/
#define CFG_STATUS              (0x6)           /* status register */
#define CFG_STATUS_SIZE         (BT_WIDTH_D16)

/*
**  Revision ID Register
*/
#define CFG_REVISION_ID         (0x8)           /* revision ID register */
#define CFG_REVISION_ID_SIZE    (BT_WIDTH_D8)

/*
**  Register Level Programming Register (not used)
*/
#define CFG_REGISTER_PROG       (0x9)           /* low byte of class code */
#define CFG_REGISTER_PROG_SIZE  (BT_WIDTH_D8)

/*
**  Class Code Register
*/
#define CFG_CLASS_CODE          (0xa)           /* upper bytes of class code */
#define CFG_CLASS_CODE_SIZE     (BT_WIDTH_D16)

/*
**  Cache Line Register (not used)
*/
#define CFG_CACHE_LINE          (0xc)           /* cache line size register */
#define CFG_CACHE_LINE_SIZE     (BT_WIDTH_D8)

/*
**  Latency Timer Register
*/
#define CFG_LATENCY_TIMER       (0xd)           /* latency timer register */
#define CFG_LATENCY_TIMER_SIZE  (BT_WIDTH_D8)

/*
**  Header Type Register (not used)
*/
#define CFG_HEADER_TYPE         (0xe)           /* header type register */
#define CFG_HEADER_TYPE_SIZE    (BT_WIDTH_D8)

/*
**  Built-In Self Test Register (not used)
*/
#define CFG_SELF_TEST           (0xf)           /* self test register */
#define CFG_SELF_TEST_SIZE      (BT_WIDTH_D8)

/*
**  I/O Mapped Node I/O Base Address Register (not used in AIX)
*/
#define CFG_IO_IO_BASE          (0x10)          /* I/O mapped node I/O */
#define CFG_IO_IO_BASE_SIZE     (BT_WIDTH_D32)

/*
**  Memory Mapped Node I/O Base Address Register
*/
#define CFG_MM_IO_BASE          (0x14)          /* memory mapped node I/O */
#define CFG_MM_IO_BASE_SIZE     (BT_WIDTH_D32)

/*
**  Mapping Register Base Address Register
*/
#define CFG_MAP_REG_BASE        (0x18)          /* mapping registers base addr */
#define CFG_MAP_REG_BASE_SIZE   (BT_WIDTH_D32)


/*
**  Remote Memory Base Address Register
*/
#define CFG_REM_MEM_BASE        (0x1c)          /* remote memory base addr */
#define CFG_REM_MEM_BASE_SIZE   (BT_WIDTH_D32)

/*
**  Interrupt Line Register 
*/
#define CFG_IRQ_LINE            (0x3c)          /* interrupt line register */
#define CFG_IRQ_LINE_SIZE       (BT_WIDTH_D8)

/*
**  Interrupt Pin Register (not used)
*/
#define CFG_IRQ_PIN             (0x3d)          /* interrupt pin register */
#define CFG_IRQ_PIN_SIZE        (BT_WIDTH_D8)

/*
**  Min_Gnt Register (not used)
*/
#define CFG_MIN_GNT             (0x3e)          /* Min_Gnt register */
#define CFG_MIN_GNT_SIZE        (BT_WIDTH_D8)

/*
**  Max_Lat Register (not used)
*/
#define CFG_MAX_LAT             (0x3f)          /* Max_Lat register */
#define CFG_MAX_LAT_SIZE        (BT_WIDTH_D8)

/*
**  Other defines
*/
#define BT_WRITE   (1<<0)
#define BT_READ    (1<<1)

/*
** Size definitions
*/
#define SIZE_1KB            0x00000400  /* 1KByte */
#define SIZE_4KB            0x00001000  /* 4Kbytes */
#define SIZE_8KB            0x00002000  /* 8Kbytes */
#define SIZE_16KB           0x00004000  /* 16Kbytes */
#define SIZE_32KB           0x00008000  /* 32Kbytes */
#define SIZE_64KB           0x00010000  /* 64Kbytes */
#define SIZE_128KB          0x00020000  /* 128Kbytes */
#define SIZE_256KB          0x00040000  /* 256Kbytes */
#define SIZE_512KB          0x00080000  /* 512Kbytes */
#define SIZE_1MB            0x00100000  /* 1Mbytes */
#define SIZE_2MB            0x00200000  /* 1Mbytes */
#define SIZE_4MB            0x00400000  /* 4Mbytes */
#define SIZE_8MB            0x00800000  /* 8Mbytes */
#define SIZE_16MB           0x01000000  /* 16Mbytes */
#define SIZE_32MB           0x02000000  /* 32Mbytes */
#define SIZE_4GB            0x100000000 /* 4Gbytes */

/*
** Structure for VxWork trace message streams 
*/

#if defined(__vxworks)

typedef struct {
    bt_data32_t  error;
    FILE        *stream1;
    FILE        *stream2;
} bt_trace_init_t;

#endif /* defined(__vxworks) */


/******************************************************************************
**
**	Define error numbers
**
** In the cases where there is an errno defined by Standard C or POSIX that
** is the equivilent to our error return, we would like to use it.
******************************************************************************/

#if	defined(__vxworks)
#define	POSIX_EQUIV(BT_ENO, ENO)	BT_ENO = ENO
#define	BT_ESTART	M_btp

#elif   defined(BT_WINNT)
#define POSIX_EQUIV(BT_ENO, ENO)        BT_ENO

#elif   defined(__sun)
#define	POSIX_EQUIV(BT_ENO, ENO)	BT_ENO
#define	BT_ESTART	0x1000

#elif   defined(__sgi)
#define	POSIX_EQUIV(BT_ENO, ENO)	BT_ENO
#define	BT_ESTART	0x1000

#elif   defined(__linux__)
#define	POSIX_EQUIV(BT_ENO, ENO)	BT_ENO
#define	BT_ESTART	0x1000

#else	/* unknown operating system */
#error Unknown Operating System!
#endif	/* defined(__vxworks) */

typedef enum {
    BT_SUCCESS = 0,			/* Everything is just swell */
    POSIX_EQUIV(BT_EIO, EIO),		/* Input or output error */
    POSIX_EQUIV(BT_ENXIO, ENXIO),	/* Non-existent device */
    POSIX_EQUIV(BT_ENOMEM, ENOMEM),	/* Out of memory or other resource */
    POSIX_EQUIV(BT_EINVAL, EINVAL),	/* Invalid parameter */
    POSIX_EQUIV(BT_EACCESS, EACCES),	/* Access failed: permission denied */
    POSIX_EQUIV(BT_EDESC, EBADF),	/* Invalid descriptor */
    POSIX_EQUIV(BT_ENOSUP, ENOSYS),	/* Option not supported on this */
    					/* particular implementation. */

    POSIX_EQUIV(BT_EABORT, EINTR),	/* Interrupted a system call */

#define BT_EINTR BT_EABORT              /* Obsolete form */

    POSIX_EQUIV(BT_ESYSMAX, BT_ESTART), /* past last POSIX defined error */

    BT_EFAIL,		/* Something went wrong. Generic error. */
    BT_ENORD,           /* Device does not support reads */
    BT_ENOWR,           /* Device does not support writes */
    BT_ESTATUS,		/* Status register error value */
    BT_ENOPWR,          /* Power is off or cable is disconnected */
    BT_EPWRCYC,         /* Power was cycled */
    BT_EBUSY,		/* Timer expired before a resource became available. */
    BT_ELCARD,          /* Local card failed diagnostics */
    BT_ECABLE,          /* Interconnection cable failed diagnostics */
    BT_ERCARD,          /* Remote card failed diagnostics */
    BT_EPAIR,           /* The pair of adapter cards failed diagnostics */
    BT_EEXCEPT,         /* OS exception error */
    BT_EEVT_NOT_REGISTERED, /* enviorment not registered */
    BT_ENOT_FOUND,      /* Attempt to acquire or remove an unknown event */
    BT_ENOT_LOCKED,     /* attempt to unlock a unlocked unit */ 
    BT_ENOT_TRANSMITTER, /* Remote access requested on a unit that is not a transmitter */
    BT_EPCI_CONFIG,     /* Error reading or writting PCI configuration registers */
    BT_ELOCAL,          /* Attempt to perform TAS or CAS on local device */
    BT_EUNKNOWN_REMID,  /* Remote adaptor not supported */
    BT_ENO_UNIT,        /* Unit does not exist */
    BT_ENO_MMAP,        /* Logical device does not support bt_mmap() */
    BT_EHANDLE,         /* Attempt to get unique signal handle failed */

    BT_MAX_ERR		/* Last Error number */
} bt_error_t;


#undef	POSIX_EQUIV

#endif /* !_BTNGPCI_H */
