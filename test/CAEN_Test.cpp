#include "CAENDigitizerType.h"
#include "CAENDigitizer.h"
#include "CAENComm.h"
#include <iostream>
#include <cstdio>

//---------------------------------------------------------
#include <stdlib.h>
#include <sys/time.h>                           // Rel. 1.4
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "inc/CAENUSBdrvB.h"
int file_handle;   // Handle got from CreateFile
#define         BOOL    int
#define         HANDLE  int
#define         TRUE    1
#define         FALSE   0
#define         INVALID_HANDLE_VALUE    -1 

static int openUSBDriver(int *file_handle, int dtNum);
static BOOL closeUSBDriver(HANDLE file_handle);
static long rebootBoard(HANDLE file_handle, int page);
int simulate_power_cycle(int dtNum);
//------------------------------------------------------------------

int main() {

  //------
  simulate_power_cycle(1);
  return 0;
  //------

  int handle;
  //CAEN_DGTZ_ErrorCode ret;
  CAENComm_ErrorCode ret;

  //---Open Digitizer---
  //ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_OpticalLink, 0, 1, 0x0, &handle);
  //ret = CAENComm_OpenDevice(CAENComm_OpticalLink, 0, 1, 0x0, &handle);
    ret = CAENComm_OpenDevice(CAENComm_USB, 0, 0, 0x0, &handle);

  std::cout << "Open return value:\t" << ret << std::endl;

  //---Software Reset---
  //ret = CAEN_DGTZ_WriteRegister(handle, 0xEF24, 1);
  //ret = CAENComm_Write32(handle, 0xEF24, 1);
  //std::cout << "Software Reset Write Reg. return value:\t" << ret << std::endl;

  //---Configuration Reload---
  //ret = CAEN_DGTZ_WriteRegister(handle, 0xEF34, 1);
  ret = CAENComm_Write32(handle, 0xEF34, 1);
  std::cout << "Confg. Reload Write Reg.  return value:\t" << ret << std::endl;

  //ret = CAEN_DGTZ_CloseDigitizer(handle);
  ret = CAENComm_CloseDevice(handle);
  std::cout << "Close return value:\t" << ret << std::endl;
  return 0;

  // int handle;
  // CAEN_DGTZ_ErrorCode ret;
  //ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, 0, 0, 0x0, &handle);
  // std::cout << "Open Return Value:\t" << ret << std::endl;
  
  // CAEN_DGTZ_BoardInfo_t BoardInfo;
  // ret = CAEN_DGTZ_GetInfo(handle,&BoardInfo);
  // std::cout << "Get Info Return Value:\t" << ret << std::endl;
  // printf("\nConnected to CAEN Desktop Digitizer Model %s\n",BoardInfo.ModelName);
  // printf("\tROC FPGA Release is %s\n",BoardInfo.ROC_FirmwareRel);
  // printf("\tAMC FPGA Release is %s\n",BoardInfo.AMC_FirmwareRel);

  // uint32_t data;
  // ret = CAEN_DGTZ_ReadRegister(handle, 0x8104, &data);
  // std::cout << "Register Value:\t" << data << "\t" 
  //           << "\n 0: " << (data&(1<<0)) 
  //           << "\n 1: " << (data&(1<<1))
  //           << "\n 2: " << (data&(1<<2))
  //           << "\n 3: " << (data&(1<<3))
  //           << "\n 4: " << (data&(1<<4))
  //           << "\n 5: " << (data&(1<<5))
  //           << "\n 6: " << (data&(1<<6))
  //           << "\n 7: " << (data&(1<<7))
  //           << std::endl;


  //ret = CAEN_DGTZ_WriteRegister(handle, CAEN_DGTZ_ACQ_CONTROL_ADD, 1<<6);

  //ret = CAEN_DGTZ_CloseDigitizer(handle);
  //std::cout << "Close Return Value:\t" << ret << std::endl;
  
  return 0;
}

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
