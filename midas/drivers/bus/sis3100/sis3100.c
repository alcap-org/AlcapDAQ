/********************************************************************

  Name:         sis3100.c
  Created by:   Stefan Ritt

  Contents:     Midas VME standard (MVMESTD) layer for SIS 3100/1100
                VME controller using sis1100w.lib

  $Log: sis3100.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:08  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/08/11 15:31:35  midas
  Adjusted inlude directories

  Revision 1.1  2004/08/11 15:30:07  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include <string.h>
#include "mvmestd.h"
#include "PlxApi.h"
#include "sis1100w.h" // Header file for sis1100w.dll

/*------------------------------------------------------------------*/

#define MAX_SIS3100_DEVICES 4

int n_sis3100, i_sis3100;

typedef struct {
   struct SIS1100_Device_Struct sis1100_dev;
   int amod;
   int dma;
   int fifo_mode;
} SIS3100_DEVICE;

SIS3100_DEVICE sis3100_device[MAX_SIS3100_DEVICES];

/*------------------------------------------------------------------*/

int mvme_init()
{
   int i;

	sis1100w_Find_No_Of_sis1100(&n_sis3100);
   if (n_sis3100 == 0)
      return MVME_NO_INTERFACE;

   for (i=0 ; i < n_sis3100; i++) {
      if (sis1100w_Get_Handle_And_Open(i+1, &sis3100_device[i].sis1100_dev) != 0) 
         return MVME_NO_INTERFACE;

      if (sis1100w_Init(&sis3100_device[i].sis1100_dev, 0) != 0)
         return MVME_NO_INTERFACE;

      if (sis1100w_Init_sis3100(&sis3100_device[i].sis1100_dev, 0) != 0)
         return MVME_NO_CRATE; 

      /* default values */
      sis3100_device[i].amod      = MVME_AMOD_A32;
      sis3100_device[i].dma       = 0;
      sis3100_device[i].fifo_mode = 0;
	}

   /* use first device by default */
   i_sis3100 = 0;

   return MVME_SUCCESS;
}

/*------------------------------------------------------------------*/

int mvme_exit()
{
   int i;

   for (i=0 ; i<n_sis3100 ; i++)
      sis1100w_Close(&sis3100_device[i].sis1100_dev);

   return MVME_SUCCESS;
}

/*------------------------------------------------------------------*/

int mvme_write(mvme_addr_t vme_addr, void *src, mvme_size_t n_bytes)
{
   mvme_size_t n;
   DWORD status, data;

   if (n_bytes <= 4) {
      data = 0;
      memcpy(&data, src, n_bytes);
      status = sis1100w_Vme_Single_Write(&sis3100_device[i_sis3100].sis1100_dev, 
         vme_addr, sis3100_device[i_sis3100].amod, n_bytes, data);
      if (status == 0)
         n = n_bytes;
      else
         n = 0;
   } else {

      status = sis1100w_Vme_Dma_Write(&sis3100_device[i_sis3100].sis1100_dev, 
         (U32) vme_addr, 
         (U32) sis3100_device[i_sis3100].amod, 
         (U32) 4, 
         (U32) sis3100_device[i_sis3100].fifo_mode,
			(U32*) src, (U32) n_bytes/4, (U32*) &n);
   }

   return n;
}

/*------------------------------------------------------------------*/

int mvme_read(void *dst, mvme_addr_t vme_addr, mvme_size_t n_bytes)
{
   mvme_size_t n;
   DWORD data;

   data = 0;
   if (n_bytes <= 4) {
      data = 0;
      n = sis1100w_Vme_Single_Read(&sis3100_device[i_sis3100].sis1100_dev, 
         vme_addr, sis3100_device[i_sis3100].amod, n_bytes, &data);
      memcpy(dst, &data, n_bytes);
   } else {

      sis1100w_Vme_Dma_Read(&sis3100_device[i_sis3100].sis1100_dev, 
         (U32) vme_addr, 
         (U32) sis3100_device[i_sis3100].amod, 
         (U32) n_bytes, 
         (U32) sis3100_device[i_sis3100].fifo_mode,
			(U32*) dst, (U32) n_bytes/4, (U32*) &n);
   }

   return n;
}

/*------------------------------------------------------------------*/

int mvme_ioctl(int req, int *param)
{
   switch (req) {
      case MVME_IOCTL_CRATE_SET:
         *param = i_sis3100;
         break;
      case MVME_IOCTL_CRATE_GET:
         if (*param < n_sis3100)
            i_sis3100 = *param;
         else
            return MVME_NO_CRATE;
         break;
      case MVME_IOCTL_AMOD_SET:
         sis3100_device[i_sis3100].amod = *param;
         break;
      case MVME_IOCTL_AMOD_GET:
         *param = sis3100_device[i_sis3100].amod;
         break;
      case MVME_IOCTL_FIFO_SET:
         sis3100_device[i_sis3100].fifo_mode = *param;
         break;
      case MVME_IOCTL_FIFO_GET:
         *param = sis3100_device[i_sis3100].fifo_mode;
         break;

      case MVME_IOCTL_DMA_SET:
         return MVME_UNSUPPORTED;
      case MVME_IOCTL_DMA_GET:
         return MVME_UNSUPPORTED;

   }

   return MVME_SUCCESS;
}
