/*********************************************************************

  Name:         vxVME.c
  Created by:   Pierre-Andre Amaudruz

  Cotents:      Routines for accessing VME under VxWorks
                
  $Log: vxVME.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.5  2004/10/06 18:57:05  pierre
  fix definition MVME

  Revision 1.4  2004/06/23 17:07:54  pierre
  add read/write func code

  Revision 1.3  2004/02/06 01:15:27  pierre
  fix new definition

  Revision 1.2  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.1  2001/10/05 22:28:34  pierre
  Initial version

*********************************************************************/
#include <stdio.h>
#include "vme.h"

#ifdef OS_VXWORKS
#include "vxWorks.h"
#include "intLib.h"
#include "sys/fcntlcom.h"
#ifdef PPCxxx
#include "arch/ppc/ivPpc.h"
#else
#include "arch/mc68k/ivMc68k.h"
#endif
#include "taskLib.h"

#ifdef PPCxxx
#define A24D24         0xfa000000       /* A24D24 camac base address */
#define A24D16         0xfa000002       /* A24D16 camac base address */
#else
#define A24D24         0xf0000000       /* A24D24 camac base address */
#define A24D16         0xf0000002       /* A24D16 camac base address */
#endif
#endif

#include    "mvmestd.h"

/*------------------------------------------------------------------*/
int vme_open(int device, int mode)
{
   /* derive device name */
   switch (mode) {
   case MVME_A16D16:
   case MVME_A16D32:

      break;
   case MVME_A24D16:
   case MVME_A24D32:

      break;
   case MVME_A32D16:
   case MVME_A32D32:

      break;

   default:
      printf("Unknown VME mode %d\n", mode);
      return -1;
   }

   /* open device */

   /* select swapping mode */

   /* set data mode */

   /* switch on block transfer */

   return (int) 1;
}

/*------------------------------------------------------------------*/
int vme_close(int vh)
{
   return (int) 1;
}

/*------------------------------------------------------------------*/
static int last_dma = -1;

int vme_read(int vh, void *dst, int vme_addr, int size, int dma)
{
  int *ptr, status;
  status = sysBusToLocalAdrs(MVME_AM_STD_USR_DATA, vme_addr, &ptr);
  switch (size) {
  case 2:
    *((WORD *)dst) = *((WORD *)ptr);
    break;
  case 4:
    *((DWORD *)dst) = *((DWORD *)ptr);
#if 0
    printf("size:%d - dst:[%p]=0x%x - vme:0x%x - ptr:%p\n"
	   , size, dst, *((DWORD *)dst), vme_addr, ptr);
#endif
    break;
  default:
    printf ("no go \n");
  }
  return (int) 1;
}

/*------------------------------------------------------------------*/

int vme_write(int vh, void *src, int vme_addr, int size, int dma)
{
  int *ptr, status;
  status = sysBusToLocalAdrs(MVME_AM_STD_USR_DATA, vme_addr, &ptr);
  switch (size) {
  case 2:
    *((WORD *)ptr) = *((WORD *)src);
    break;
  case 4:
    *((DWORD *)ptr) = *((DWORD *)src);
#if 0
    printf("size:%d - src:[%p]=0x%x - vme:0x%x - ptr:%p\n", size, src, *((DWORD
									  *)src), vme_addr, ptr);
#endif
    break;
  default:
    printf ("no go \n");
  }
  return (int) 1;
}

/*------------------------------------------------------------------*/
int vme_mmap(int vh, void **ptr, int vme_addr, int size)
{
   int status;
   status = sysBusToLocalAdrs(MVME_AM_STD_USR_DATA, vme_addr, (int *) ptr);
   return status == SUCCESS ? 1 : (int) status;
}

/*------------------------------------------------------------------*/
int vme_unmap(int vh, void *ptr, int size)
{
   return 1;
}

/*------------------------------------------------------------------*/
int vme_ioctl(int vh, int request, int *param)
{
   switch (request) {
   case MVME_IOCTL_AMOD_SET:
      break;
   case MVME_IOCTL_AMOD_GET:
      break;
   }
   return SUCCESS;
}
