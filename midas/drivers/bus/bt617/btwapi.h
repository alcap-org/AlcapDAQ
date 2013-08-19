/*****************************************************************************
**
**      Filename:   btwapi.h
**
**      Purpose:    SBS Bit 3 Application Programming Interface header file
**                  for 973, 983, and 984.
**
**      $Revision: 1.1.1.1 $
**
******************************************************************************/
/*****************************************************************************
**
**             Copyright (c) 1998 by SBS Technologies Inc.
**        Copyright (c) 1996-1997 by Bit 3 Computer Corporation.
**                     All Rights Reserved.
**              License governs use and distribution.
**
*****************************************************************************/

#ifndef _BTWAPI_H
#define _BTWAPI_H

#ifndef BTDRIVER
#include <windows.h>
#endif /* !BTDRIVER */

/*
**  Include files
*/
#include "btwdef.h"
#include "btngpci.h"

#define BT_NBUS_FAMILY

#define FILE_DEVICE_BTBRIDGE ((DEVICE_TYPE)32768)


typedef struct bt_lib_data_s * bt_desc_t;
typedef bt_devaddr_t bt_binddesc_t;

#define BT_MAX_DEV_NAME 20


typedef enum {
    BT_MIN_CTRL,
    BT_MAX_CTRL
} bt_ctrl_t;

#undef BT_EXTERN

#ifndef BTDRIVER

#ifndef BT_APIDLL

#ifdef __cplusplus
#define BT_EXTERN extern "C" __declspec(dllimport)
#else /* __cplusplus */
#define BT_EXTERN extern __declspec(dllimport)
#endif /* __cplusplus */

#else /* !BT_APIDLL */

#ifdef __cplusplus
#define BT_EXTERN extern "C" __declspec(dllexport)
#else /* __cplusplus */
#define BT_EXTERN extern __declspec(dllexport)
#endif /* __cplusplus */

#endif /* !BT_APIDLL */

#else /* !BTDRIVER */

#ifdef __cplusplus
#define BT_EXTERN extern "C"
#else /* __cplusplus */
#define BT_EXTERN extern
#endif /* __cplusplus */

#endif /* !BTDRIVER */

/*
** Nanobus extension functions
*/
BT_EXTERN bt_error_t   __stdcall bt_cas(bt_desc_t btd, bt_devaddr_t rem_off, 
                                        bt_data32_t cmpval, bt_data32_t swapval, 
                                        bt_data32_t * prevval_p, size_t data_size);
BT_EXTERN bt_error_t   __stdcall bt_tas(bt_desc_t btd, bt_devaddr_t rem_off, 
                                        bt_data8_t * prevval_p);
BT_EXTERN bt_error_t   __stdcall bt_get_io(bt_desc_t btd, bt_reg_t reg, 
                                           bt_data32_t * result_p);
BT_EXTERN bt_error_t   __stdcall bt_put_io(bt_desc_t btd, bt_reg_t reg, 
                                           bt_data32_t value);
BT_EXTERN bt_error_t   __stdcall bt_or_io(bt_desc_t btd, bt_reg_t reg, 
                                          bt_data32_t value);
BT_EXTERN const char * __stdcall bt_reg2str(bt_reg_t reg);
BT_EXTERN bt_error_t   __stdcall bt_reset(bt_desc_t btd);
BT_EXTERN bt_error_t   __stdcall bt_send_irq(bt_desc_t btd);
BT_EXTERN bt_error_t   __stdcall bt_send_vector(bt_desc_t btd, bt_data32_t vector);
BT_EXTERN bt_error_t   __stdcall bt_give_sema(bt_desc_t btd, bt_reg_t sema);
BT_EXTERN bt_error_t   __stdcall bt_take_sema(bt_desc_t btd, bt_reg_t sema, bt_msec_t timeout);
BT_EXTERN  bt_error_t __stdcall bt_hw_bind(bt_desc_t btd, bt_binddesc_t *bind_p, bt_devaddr_t *offset_p,
                              bt_devaddr_t loc_addr, size_t buf_len, bt_accessflag_t flags,
                              bt_swap_t swapping);
BT_EXTERN  bt_error_t __stdcall bt_hw_unbind(bt_desc_t btd, bt_binddesc_t desc);
BT_EXTERN  bt_error_t __stdcall bt_hw_write(bt_desc_t btd, bt_devaddr_t bus_addr, bt_devaddr_t xfer_addr, size_t req_len, size_t *xfer_len_p);
BT_EXTERN  bt_error_t __stdcall bt_hw_read(bt_desc_t btd, bt_devaddr_t bus_addr, bt_devaddr_t xfer_addr, size_t req_len, size_t *xfer_len_p);
BT_EXTERN bt_error_t __stdcall bt_driver_version(bt_desc_t btd, char *rev_info_p, int *line_p);
BT_EXTERN bt_error_t __stdcall bt_lcard_diag(bt_desc_t btd, char *rev_info_p, int *line_p);
BT_EXTERN bt_error_t __stdcall bt_cable_diag(bt_desc_t btd, char *rev_info_p, int *line_p);
BT_EXTERN bt_error_t __stdcall bt_rcard_diag(bt_desc_t btd, bt_dev_t rldev, bt_devaddr_t raddr, bt_data32_t rlen, char *rev_info_p, int *line_p);
BT_EXTERN bt_error_t __stdcall bt_pair_diag(bt_desc_t btd, bt_dev_t rldev, bt_devaddr_t raddr, bt_data32_t rlen, char *rev_info_p, int *line_p);

/*
** Windows implementation-specific functions
*/
BT_EXTERN bt_error_t __stdcall bt_gettrace(bt_desc_t btd, char * lpBuffer,
                                           unsigned long * lpNumMessages);
BT_EXTERN bt_error_t __stdcall bt_status(bt_desc_t btd, bt_data32_t * Status_p);
BT_EXTERN bt_error_t __stdcall bt_querydriver(int btUnitNo, 
                                              unsigned long *pdwBusNum, 
                                              unsigned long *pdwSlotNum, 
                                              unsigned long *pdwLocId);
BT_EXTERN bt_error_t __stdcall bt_querydriver2(int btUnitNo, 
                                              unsigned long *pdwBusNum, 
                                              unsigned long *pdwSlotNum, 
                                              unsigned long *pdwLocId,
                                              char * pRegistryName);
#endif /* _BTWAPI_H */
