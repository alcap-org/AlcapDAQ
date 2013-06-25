/* vmedrv_conf.h */
/* VME device driver for Bit3 Model 617 on Linux 2.6.x */
/* Created by Enomoto Sanshiro on 28 November 1999. */
/* Last updated by Enomoto Sanshiro on 17 October 2005. */


#ifndef __VMEDRV_CONF_H_
#define __VMEDRV_CONF_H_

#include "vmedrv.h"
#include <linux/moduleparam.h>

#ifndef VMEDRV_MAJOR
#define VMEDRV_MAJOR 0
#endif
static int vmedrv_major = VMEDRV_MAJOR;
module_param(vmedrv_major, int, S_IRUGO);

static char* vmedrv_name = "vmedrv";
module_param(vmedrv_name, charp, S_IRUGO);

static int target_card_number = 0;
module_param(target_card_number, int, S_IRUGO);


struct vmedrv_config_t {
    char* mode_name;
    unsigned address_modifier;
    unsigned dma_address_modifier;
    unsigned data_width;
    unsigned function_code;
    unsigned byte_swapping;
};

static const struct vmedrv_config_t vmedrv_config_table[] = {
/*  Name      AM code     DMA AM    Data Width  Function Code   BYTE SWAP */
{ "A16D16",  amA16,      amINVALID,   dwWORD,  fcREMOTE_BUS_IO,   bsNONE  },
{ "A16D32",  amA16,      amINVALID,   dwLONG,  fcREMOTE_BUS_IO,   bsNONE  },
{ "A24D16",  amA24DATA,  amA24BLOCK,  dwWORD,  fcREMOTE_BUS_IO,   bsNONE  },
{ "A24D32",  amA24DATA,  amA24BLOCK,  dwLONG,  fcREMOTE_BUS_IO,   bsNONE  },
{ "A32D16",  amA32DATA,  amA32BLOCK,  dwWORD,  fcREMOTE_BUS_IO,   bsNONE  },
{ "A32D32",  amA32DATA,  amA32BLOCK,  dwLONG,  fcREMOTE_BUS_IO,   bsNONE  },
/* The order of entries in this table must be exactly same with        */
/* the order in vmedrv_access_modes_t in "vmedrv.h".                   */
};


static const int PIO_WINDOW_PAGES = 16;  /* 64 kB */
static const int PIO_BUFFER_SIZE = 0x1000;  /* 4 kB */

//static const int DMA_BUFFER_SIZE = 0x10000;  /* 64 kB */
static const int DMA_BUFFER_SIZE = 0x100000;   /* 1 MB */



#endif
