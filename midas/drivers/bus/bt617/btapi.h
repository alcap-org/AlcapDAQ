/*****************************************************************************
**
**      Filename:   btapi.h
**
**      Purpose:    SBS Bit 3 Application Programming Interface header file.
**
**      $Revision: 1.1.1.1 $
**
******************************************************************************/
/*****************************************************************************
**
**        Copyright (c) 1997-2000 by SBS Technologies, Inc.
**        Copyright (c) 1996 by Bit 3 Computer Corporation.
**                     All Rights Reserved.
**              License governs use and distribution.
**
*****************************************************************************/

#ifndef _BTAPI_H
#define _BTAPI_H

#include <stddef.h> /* gives us size_t and NULL */
#include "btdef.h"

/*
** Define BT_EXTERN
*/
#ifdef __cplusplus
#define BT_EXTERN extern "C"
#else /* __cplusplus */
#define BT_EXTERN extern
#endif /* __cplusplus */

/*
** Define BT_METHOD (Calling Method)
*/

#if !defined(BT973) && !defined(WINVER) && !defined(BT15901) \
    && !defined(BT15991) && !defined(BT17903) && !defined(BT13908)

#define BT_METHOD

#else  /* !defined(BT973) && !defined(WINVER) && !defined(BT15901) \
          && !defined(BT15991) && !defined(BT17903) && !defined(BT13908) */

/* Windows requires __stdcall between the return type and the function
   name- so we can't just hide it in BT_EXTERN. */

#define BT_METHOD __stdcall

#endif /* !defined(BT973) && !defined(WINVER) && !defined(BT15901) \
          && !defined(BT15991) && !defined(BT17903) && !defined(BT13908) */

/*****************************************************************************
**
**  Product specific include files
**
*****************************************************************************/

#if defined(BT25801)
#include "btuapi.h"     /* VMEbus broadcast memory specific include file */
/* defined(BT25801) */

#elif defined(BT21805)
#include "btuapi.h"     /* Universe specific include file */
/* defined(BT21805) */

#elif defined(BT28801)
#include "btvapi.h"
/* defined(BT28801) */

#elif defined(BT973) || defined(WINVER)
#include "btwapi.h"
#if !defined(BT_NTDRIVER)
#define BT_NTDRIVER
#endif /* !defined(BT_NTDRIVER) */
/* defined(BT973) || defined(WINVER) */

#elif defined(BT993)
#include "btpapi.h"
/* defined(BT993) */

#elif defined(BT965)
#include "btpapi.h"
/* defined(BT965) */

#elif defined(BT946)
#include "btpapi.h"
/* defined(BT946) */

#elif defined(BT18901)
#include "btvapi.h"
/* defined(BT18901) */

#elif defined(BT18902)
#include "btqapi.h"
/* defined(BT18902) */

#elif defined(BT15901) || defined(BT15991)
#include "btmapi.h"     /* Broadcast/Shared Memory specific include file */
#if !defined(BT_NTDRIVER)
#define BT_NTDRIVER
#endif /* !defined(BT_NTDRIVER) */
/* defined(BT15901) */

#elif defined(BT15904)
#include "btmapi.h"     /* SGI Broadcast Memory specific include file */
/* defined(BT15904) */

#elif defined(BT15906)
#include "btmapi.h"     /* Solaris 7 Broadcast Memory specific include file */
/* defined(BT15906) */

#elif defined(BT17903)
#include "btuapi.h"     /* NanoBridge specific include file */
#if !defined(BT_NTDRIVER)
#define BT_NTDRIVER
#endif /* !defined(BT_NTDRIVER) */
/* defined(BT17903) */

#elif defined(BT13908)
#include "btfapi.h"
#if !defined(BT_NTDRIVER)
#define BT_NTDRIVER
#endif /* !define(BT_NTDRIVER) */
/* defined(BT13908) */

#elif defined(BT1003)
#include "btpapi.h"    /* Nanobus specific include file */

#else
#error Undefined project!

/*
** If you've got this error, you need to define one of the project
** identifiers (i.e. BT973, etc.).  Please examine the example source
** provided with your driver for which identifier to define, and how
** to define it.
**
** In addition, the various Windows drivers should also define
** BT_NTDRIVER.
*/

#endif

/*****************************************************************************
**
**  Macros and types for the SBS Bit 3 Application Programming Inteface.
**
*****************************************************************************/


/*****************************************************************************
**
**  Common types and defines for SBS Bit 3 Application Programming Interface
**
*****************************************************************************/


/*****************************************************************************
**
**  Prototypes for the SBS Bit 3 Application Programming Inteface.
**
*****************************************************************************/
/*
**  Function prototypes for SBS Bit 3 Mirror API
*/

BT_EXTERN bt_dev_t
BT_METHOD bt_str2dev(
    const char     *name_p);

BT_EXTERN char *
BT_METHOD bt_gen_name(
    int             unit,
    bt_dev_t        logdev,
    char           *devname_p,
    size_t          max_devname);

BT_EXTERN bt_error_t
BT_METHOD bt_open(
    bt_desc_t      *btd_p,
    const char     *devname_p,
    bt_accessflag_t flags);

BT_EXTERN bt_error_t
BT_METHOD bt_close(
    bt_desc_t       btd_p);

BT_EXTERN bt_error_t
BT_METHOD bt_chkerr(
    bt_desc_t       btd_p);

BT_EXTERN bt_error_t
BT_METHOD bt_clrerr(
    bt_desc_t       btd_p);

BT_EXTERN bt_error_t
BT_METHOD bt_perror(
    bt_desc_t       btd_p,
    bt_error_t      errval,
    const char     *msg_p);

BT_EXTERN char *
BT_METHOD bt_strerror(
    bt_desc_t       btd_p,
    bt_error_t      status,
    const char     *msg_p,
    char           *buf_p,
    size_t          buf_len);

BT_EXTERN bt_error_t
BT_METHOD bt_init(
    bt_desc_t       btd_p);

BT_EXTERN bt_error_t
BT_METHOD bt_read(
    bt_desc_t       btd_p,
    void           *buf_p,
    bt_devaddr_t    xfer_off,
    size_t          xfer_len,
    size_t         *xfer_done_p);

BT_EXTERN bt_error_t
BT_METHOD bt_write(
    bt_desc_t       btd_p,
    void           *buf_p,
    bt_devaddr_t    xfer_off,
    size_t          xfer_len,
    size_t         *xfer_done_p);

BT_EXTERN bt_error_t
BT_METHOD bt_get_info(
    bt_desc_t       btd_p,
    bt_info_t       param,
    bt_devdata_t   *value_p);

BT_EXTERN bt_error_t
BT_METHOD bt_set_info(
    bt_desc_t       btd_p,
    bt_info_t       param,
    bt_devdata_t    value);

BT_EXTERN bt_error_t
BT_METHOD bt_icbr_install(
    bt_desc_t       btd_p, 
    bt_irq_t        irq_type, 
    bt_icbr_t      *icbr_p,
    void           *param_p,
    bt_data32_t vector);

BT_EXTERN bt_error_t
BT_METHOD bt_icbr_remove(
    bt_desc_t       btd_p, 
    bt_irq_t        irq_type,
    bt_icbr_t      *icbr_p);

BT_EXTERN bt_error_t
BT_METHOD bt_lock(
    bt_desc_t       btd_p,
    bt_msec_t       wait_len);

BT_EXTERN bt_error_t
BT_METHOD bt_unlock(
    bt_desc_t       btd_p);

BT_EXTERN bt_error_t
BT_METHOD bt_mmap(
    bt_desc_t       btd_p,
    void          **map_p, 
    bt_devaddr_t    map_off,
    size_t          map_len, 
    bt_accessflag_t flags, 
    bt_swap_t       swapping);

BT_EXTERN bt_error_t
BT_METHOD bt_unmmap(
    bt_desc_t       btd_p,
    void           *map_p, 
    size_t map_len);

BT_EXTERN bt_error_t
BT_METHOD bt_bind(
    bt_desc_t       btd_p,
    bt_binddesc_t  *bind_p,
    bt_devaddr_t   *offset_p,
    void           *buf_p,
    size_t          buf_len,
    bt_accessflag_t flags,
    bt_swap_t       swapping);

BT_EXTERN bt_error_t
BT_METHOD bt_unbind(
    bt_desc_t       btd_p,
    bt_binddesc_t   desc);

BT_EXTERN const char *
BT_METHOD bt_dev2str(
    bt_dev_t        type);

BT_EXTERN bt_error_t
BT_METHOD bt_ctrl(
    bt_desc_t       btd_p,
    int             ctrl, 
    void           *param_p);

#endif  /* _BTAPI_H */
