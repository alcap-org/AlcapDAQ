#include <stdlib.h>
#include <sys/time.h>                           // Rel. 1.4
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "CAENUSBdrvB.h"
int file_handle;   // Handle got from CreateFile
#define         BOOL    int
#define         HANDLE  int
#define         TRUE    1
#define         FALSE   0
#define         INVALID_HANDLE_VALUE    -1 

//static int openUSBDriver(int *file_handle, int dtNum);
//static BOOL closeUSBDriver(HANDLE file_handle);
//static long rebootBoard(HANDLE file_handle, int page);
//int simulate_power_cycle(int dtNum);


static int openUSBDriver(int *file_handle,int dtNum)
{
   char devname[80];

   sprintf(devname,"/dev/usb/v1718_%d",dtNum); // HACK : to be generic it should have a parameter board_number
   printf("Opening USB device: %s\n",devname);
            
   *file_handle = open(devname, O_RDWR);
   if( *file_handle == INVALID_HANDLE_VALUE ){
     return FALSE;
   } else {
     return TRUE;
   }
}

static BOOL closeUSBDriver(HANDLE file_handle)
{
  BOOL ret;
  ret = !close(file_handle);
  file_handle = INVALID_HANDLE_VALUE;
  return ret;
}

static long rebootBoard(HANDLE file_handle, int page)
{
  switch(page)
  {
    case 0 :
      printf("Warning: We should never want to reboot from backup firmware image!");
      //printf("Sending command to reboot from backup firmware image ...\n");
      //ioctl(file_handle, V1718_IOCTL_REBOOTB, NULL);
      //printf("Command sent.\n");
      break;
    case 1 :   
      printf("Sending command to reboot from standard firmware image ...\n");
      ioctl(file_handle, V1718_IOCTL_REBOOTF, NULL);
      printf("Command sent.\n");
      break;
    default:
      break;
  }
  
  return 0;
}

int simulate_power_cycle(int dtNum)
{
    if(openUSBDriver(&file_handle,dtNum)) {
      rebootBoard(file_handle, 1);//second arg MUST be 1 to use Standard Firmware. (0 would load old Backup firmware)
      closeUSBDriver(file_handle);
      return 0;
    } else {
      printf("Error: Cannot connect to device via USB.\n");
      return -1;
    }
}
