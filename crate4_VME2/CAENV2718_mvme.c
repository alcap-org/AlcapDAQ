/********************************************************************\

  Name:        CAENV2718_mvme.c
  Created by:  Exaos Lee
  Contents:    MIDAS VME standard (MVMESTD) layer for CAEN V2718 VME
               controller using CAENVMElib

\********************************************************************/

#ifdef __linux__
#ifndef OS_LINUX
#define OS_LINUX
#endif
#endif
#ifdef _MSC_VER
#define OS_WINNT
#define WIN32
#endif

#ifdef OS_LINUX
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "CAENVMElib.h"
#include "mvmestd.h"

/* map mvmestd dmode to CVDataWidth */
CVDataWidth map_dmode(int dmode)
{
    CVDataWidth cdm;
    switch(dmode) {
    case MVME_DMODE_D8:     cdm = cvD8;  break;
    case MVME_DMODE_D16:    cdm = cvD16; break;
    case MVME_DMODE_D32:    cdm = cvD32; break;
    case MVME_DMODE_D64:    cdm = cvD64; break;
    case MVME_DMODE_RAMD16:
    case MVME_DMODE_RAMD32:
    case MVME_DMODE_LM:
    default:
        cdm = cvD32;
        break;
    }

    return cdm;
}

/* map CVErrorCodes to mvmestd status codes */
int map_errcode(CVErrorCodes ret)
{
    int st;
    switch(ret) {
    case cvSuccess:      st = MVME_SUCCESS; break;
    case cvInvalidParam: st = MVME_INVALID_PARAM; break;
    case cvCommError:
    case cvBusError:
    case cvGenericError:
    case cvTimeoutError:
    default:
        st = MVME_ACCESS_ERROR;  break;
    }
    return st;
}

int mvme_open(MVME_INTERFACE **vme, int idx)
{
    CVBoardTypes VMEBoard = cvV2718;
    int32_t      cvHdl;

    *vme = (MVME_INTERFACE *) malloc(sizeof(MVME_INTERFACE));
    if (*vme == NULL)
        return MVME_NO_MEM;

    memset(*vme, 0, sizeof(MVME_INTERFACE));

    /*
      CAENVME_Init(CVBoardTypes BdType, short Link, short BdNum, int32_t *Handle);
      Default:  Link = 0; BdNum = idx;
      WARNING: the Handler of CAENVME_* is type int32_t; (*vme)->handle is int!!
    */
    if( CAENVME_Init(VMEBoard, 0, (short)idx, &cvHdl) != cvSuccess ) {
        return MVME_NO_INTERFACE;
    }

    (*vme)->handle    = cvHdl;
    (*vme)->am        = MVME_AM_DEFAULT;
    (*vme)->dmode     = MVME_DMODE_D32;
    (*vme)->blt_mode  = MVME_BLT_NONE;
    (*vme)->table     = NULL; /* not used */

    return MVME_SUCCESS;
}

int mvme_close(MVME_INTERFACE *vme)
{
    CAENVME_End((int32_t)vme->handle);
    return MVME_SUCCESS;
}

int mvme_sysreset(MVME_INTERFACE *vme)
{
    CAENVME_DeviceReset((int32_t)vme->handle);
    return MVME_SUCCESS;
}

int mvme_write(MVME_INTERFACE *vme, mvme_addr_t vme_addr, void *src, mvme_size_t n_bytes)
{
    /*
      1. single VME write cycle
      CAENVME_API CAENVME_WriteCycle(int32_t Handle, uint32_t Address, void *Data,
      CVAddressModifier AM, CVDataWidth DW);

      2. a block of VME write cycle
      CAENVME_API CAENVME_MultiWrite(int32_t Handle, uint32_t *Addrs, uint32_t *Buffer,
      int NCycles, CVAddressModifier *AMs, CVDataWidth *DWs, CVErrorCodes *ECs);

      3. a VME block transfer write cycle
      CAENVME_API CAENVME_BLTWriteCycle(int32_t Handle, uint32_t Address, void *Buffer,
      int size, CVAddressModifier AM, CVDataWidth DW, int *count);
      4. a VME block transfer write cycle. The address is not incremented during cycle
      CAENVME_API CAENVME_FIFOBLTWriteCycle(int32_t Handle, uint32_t Address, void *Buffer,
      int size, CVAddressModifier AM, CVDataWidth DW, int *count);
      5. a VME multiplexed block transfer write cycle
      CAENVME_API CAENVME_MBLTWriteCycle(int32_t Handle, uint32_t Address, void *Buffer,
      int size, CVAddressModifier AM, int *count);
      6. a VME multiplexed block transfer write cycle. The address is not
      incremented during the cycle
      CAENVME_API CAENVME_FIFOMBLTWriteCycle(int32_t Handle, uint32_t Address, void *Buffer,
      int size, CVAddressModifier AM, int *count);
    */
    CVDataWidth       dtsize;
    CVAddressModifier am;
    CVErrorCodes      ret;
    int               blts, count;

    dtsize = map_dmode(vme->dmode);
    am = (CVAddressModifier) vme->am;

    /* Unsupported write mode: A16 BLT, CR/CSR BLT */
    if( (am == cvA16_U) || (am == cvCR_CSR) )
        return MVME_UNSUPPORTED; 

    /* Check block transfer conditions:
       1. Address mode: A24 or A32
       2. Data mode: D32, D64
       3. Edge mode: BLT, MBLT
    */
    if( dtsize == cvD64 ) {
        if( am == cvA24_U_DATA )
            am = cvA24_U_MBLT;
        else
            am = cvA32_U_MBLT;
    } else {
        if( am == cvA24_U_DATA )
            am = cvA24_U_BLT;
        else
            am = cvA32_U_BLT;
    }

    blts = n_bytes; /* ??? */
    ret = CAENVME_BLTWriteCycle((int32_t)vme->handle, (uint32_t)vme_addr, src,
                                blts, am, dtsize, &count);

    return map_errcode(ret);
}

int mvme_write_value(MVME_INTERFACE *vme, mvme_addr_t vme_addr, unsigned int value)
{
    CVDataWidth       dtsize;
    CVAddressModifier am;
    CVErrorCodes      ret;

    dtsize = map_dmode(vme->dmode);
    if(dtsize == cvD64) { /* Cannot execute D64 write cycle */
        return MVME_UNSUPPORTED;
    }
    am = (CVAddressModifier) vme->am;

    ret = CAENVME_WriteCycle((int32_t) vme->handle, (uint32_t)vme_addr, &value, am, dtsize);

    return map_errcode(ret);
}

int mvme_read(MVME_INTERFACE *vme, void *dst, mvme_addr_t vme_addr, mvme_size_t* n_bytes)
{
    CVDataWidth       dtsize;
    CVAddressModifier am;
    CVErrorCodes      ret;
    int               count;

    dtsize = map_dmode(vme->dmode);
    am = (CVAddressModifier) vme->am;
    if( (am == cvA16_U) || (am == cvCR_CSR) )
        return MVME_UNSUPPORTED;
    if( dtsize == cvD64 ) {
        if( am == cvA24_U_DATA )
            am = cvA24_U_MBLT;
        else
            am = cvA32_U_MBLT;
    } else {
        if( am == cvA24_U_DATA )
            am = cvA24_U_BLT;
        else
            am = cvA32_U_BLT;
    }

    ret = CAENVME_BLTReadCycle((int32_t)vme->handle, (uint32_t) vme_addr, dst,
                               *n_bytes, am, dtsize, &count);
    *n_bytes = count;

    return map_errcode(ret);
}

unsigned int mvme_read_value(MVME_INTERFACE *vme, mvme_addr_t vme_addr)
{
    CVDataWidth       dtsize;
    CVAddressModifier am;
    CVErrorCodes      ret;
    uint32_t          data;

    dtsize = map_dmode(vme->dmode);
    if(dtsize == cvD64) {
        return MVME_UNSUPPORTED;
    }
    am = (CVAddressModifier) vme->am;

    ret = CAENVME_ReadCycle((int32_t)vme->handle, (uint32_t) vme_addr, &data, am, dtsize);
    if(ret != cvSuccess) return 0;

    return data;
}

int mvme_set_am(MVME_INTERFACE *vme, int am)
{
    vme->am = am;
    return MVME_SUCCESS;
}

int mvme_get_am(MVME_INTERFACE *vme, int *am)
{
    *am = vme->am;
    return MVME_SUCCESS;
}

int mvme_set_dmode(MVME_INTERFACE *vme, int dmode)
{
    vme->dmode = dmode;
    return MVME_SUCCESS;
}

int mvme_get_dmode(MVME_INTERFACE *vme, int *dmode)
{
    *dmode = vme->dmode;
    return MVME_SUCCESS;
}

int mvme_set_blt(MVME_INTERFACE *vme, int mode)
{
    vme->blt_mode = mode;
    return MVME_SUCCESS;
}

int mvme_get_blt(MVME_INTERFACE *vme, int *mode)
{
    *mode = vme->blt_mode;
    return MVME_SUCCESS;
}

