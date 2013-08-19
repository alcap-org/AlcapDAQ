/*********************************************************************

  Name:         bt617.c
  Created by:   Stefan Ritt

  Cotents:      Routines for accessing VME over SBS Bit3 Model 617
                Interface under Windows NT using the NT device driver 
                Model 983 and under Linux using the vmehb device
                driver.

  $LOG$
*********************************************************************/

#include    <stdio.h>

#ifdef _MSC_VER
#define BT_NTDRIVER
#define BT_WINNT
#define BT973
#include    "btapi.h"
#endif

#include    "mvmestd.h"

/*------------------------------------------------------------------*/

int vme_open(int device, int mode)
{
   bt_error_t status;           /* Bit 3 library error return type */
   char devname[BT_MAX_DEV_NAME];       /* Device to open */
   bt_devdata_t flag;
   bt_desc_t btd;

   /* derive device name */
   switch (mode) {
   case VME_A16D16:
   case VME_A16D32:
      bt_gen_name(device, BT_DEV_A16, devname, BT_MAX_DEV_NAME);
      break;
   case VME_A24D16:
   case VME_A24D32:
      bt_gen_name(device, BT_DEV_A24, devname, BT_MAX_DEV_NAME);
      break;
   case VME_A32D16:
   case VME_A32D32:
      bt_gen_name(device, BT_DEV_A32, devname, BT_MAX_DEV_NAME);
      break;

   case VME_LM:
      bt_gen_name(device, BT_DEV_LM, devname, BT_MAX_DEV_NAME);
      break;

   default:
      printf("Unknown VME mode %d\n", mode);
      return -1;
   }

   /* open device */
   status = bt_open(&btd, devname, BT_RDWR);
   if (status != BT_SUCCESS) {
      bt_perror(btd, status, "vme_open error");
      return -1;
   }
   bt_init(btd);
   bt_clrerr(btd);

   /* select swapping mode */
   if (mode != VME_LM) {
      flag = BT_SWAP_NONE;
      bt_set_info(btd, BT_INFO_SWAP, flag);

      /* set data mode */
      flag = (mode == VME_A16D16 || mode == VME_A24D16 || mode == VME_A32D16) ?
        BT_WIDTH_D16 : BT_WIDTH_D32;
      bt_set_info(btd, BT_INFO_DATAWIDTH, flag);

      /* switch on block transfer */
      flag = TRUE;
      bt_set_info(btd, BT_INFO_BLOCK, flag);

      flag = 256;
      bt_set_info(btd, BT_INFO_DMA_POLL_CEILING, flag);

      flag = 256;
      bt_set_info(btd, BT_INFO_DMA_THRESHOLD, flag);
   }

   return (int) btd;
}

/*------------------------------------------------------------------*/

int vme_close(int btd)
{
   bt_error_t status;

   status = bt_close((bt_desc_t) btd);
   if (status != BT_SUCCESS)
      bt_perror((bt_desc_t) btd, status, "vme_close error");

   return (int) status;
}

/*------------------------------------------------------------------*/

static int last_dma = -1;

int vme_read(int vh, void *dst, int vme_addr, int size, int dma)
{
   bt_error_t status;
   int read, flag;

   if (dma != last_dma) {
      last_dma = dma;

      flag = dma ? 256 : 100000000;

      bt_set_info((bt_desc_t) vh, BT_INFO_DMA_POLL_CEILING, flag);
      bt_set_info((bt_desc_t) vh, BT_INFO_DMA_THRESHOLD, flag);
   }

   status = bt_read((bt_desc_t) vh, dst, vme_addr, size, &read);
   if (status != BT_SUCCESS)
      bt_perror((bt_desc_t) vh, status, "vme_read error");

   return read;
}

/*------------------------------------------------------------------*/

int vme_write(int vh, void *src, int vme_addr, int size, int dma)
{
   bt_error_t status;
   int read, flag;

   if (dma != last_dma) {
      last_dma = dma;

      flag = dma ? 256 : 100000000;

      bt_set_info((bt_desc_t) vh, BT_INFO_DMA_POLL_CEILING, flag);
      bt_set_info((bt_desc_t) vh, BT_INFO_DMA_THRESHOLD, flag);
   }

   status = bt_write((bt_desc_t) vh, src, vme_addr, size, &read);
   if (status != BT_SUCCESS)
      bt_perror((bt_desc_t) vh, status, "vme_write error");

   return read;
}

/*------------------------------------------------------------------*/

int vme_mmap(int vh, void **ptr, int vme_addr, int size)
{
   bt_error_t status;

   status = bt_mmap((bt_desc_t) vh, ptr, vme_addr, size, BT_RDWR, BT_SWAP_DEFAULT);
   if (status != BT_SUCCESS)
      bt_perror((bt_desc_t) vh, status, "vme_mmap error");

   return status == BT_SUCCESS ? 1 : (int) status;
}

/*------------------------------------------------------------------*/

int vme_unmap(int vh, void *ptr, int size)
{
   bt_error_t status;

   status = bt_unmmap((bt_desc_t) vh, ptr, size);
   if (status != BT_SUCCESS)
      bt_perror((bt_desc_t) vh, status, "vme_unmap error");

   return status == BT_SUCCESS ? 1 : (int) status;
}

/*------------------------------------------------------------------*/

int vme_ioctl(int vh, int request, int *param)
{
   switch (request) {
   case VME_IOCTL_AMOD_SET:
      bt_set_info((bt_desc_t) vh, BT_INFO_DMA_AMOD, *param);
      bt_set_info((bt_desc_t) vh, BT_INFO_PIO_AMOD, *param);
      bt_set_info((bt_desc_t) vh, BT_INFO_MMAP_AMOD, *param);
      break;
   case VME_IOCTL_AMOD_GET:
      bt_get_info((bt_desc_t) vh, BT_INFO_DMA_AMOD, param);
      break;
   }
   return SUCCESS;
}
