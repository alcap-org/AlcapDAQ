#ifndef _CAENDTDIGIUTILS_H__
#define _CAENDTDIGIUTILS_H__

#include <stdlib.h>
#include <sys/time.h>                           // Rel. 1.4
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "CAENUSBdrvB.h"
#include "CAENDigitizerType.h"
#include "CAENDigitizer.h"

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

typedef struct caen_digi_status
{

  BOOL run_active;
  BOOL evt_ready;
  BOOL board_full;
  BOOL ext_clock;
  BOOL pll_bypass;
  BOOL pll_lost;
  BOOL board_ready;
  BOOL internal_comm_to;
  BOOL over_temperature;
  BOOL no_power;
} caen_digi_status;

static int openUSBDriver(int *file_handle,int dtNum)
{
   char devname[80];

   sprintf(devname,"/dev/usb/v1718_%d",dtNum);
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

// // Check board status
caen_digi_status caen_digi_get_status(int handle) {
  caen_digi_status ds;
  uint32_t data;
  CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_STATUS_ADD, &data);
  /*
    8-bit Acquisition Status Register
    Register is reflected on front panel LEDs
    0: Reserved
    1: Reserved
    2: 1 if RUN ON
    3: 1 if event is ready to be read out
    4: 1 if maximum number of events currently on board
    5: 1 if using external clock
    6: 1 if PLL circuitry being bypassed, 1 if PLL is being used
    7: 1 if no loss of PLL lock since last read
    8: 1 if board ready for data taking
  */
  ds.run_active  = (BOOL)((data >>= 2) & 1);
  ds.evt_ready   = (BOOL)((data >>= 1) & 1);
  ds.board_full  = (BOOL)((data >>= 1) & 1);
  ds.ext_clock   = (BOOL)((data >>= 1) & 1);
  ds.pll_bypass  = (BOOL)((data >>= 1) & 1);
  ds.pll_lost    = !((BOOL)((data >>= 1) & 1));
  ds.board_ready = (BOOL)((data >>= 1) & 1);

  CAEN_DGTZ_ReadRegister(handle, 0x8178, &data);
  /*
    Fail Status Register
    0-3: Internal communication timeout
    4:   PLL lock loss (taken care of above)
    5:   Temperature failure
    6:   ADC power down due to temperature failure
  */
  ds.internal_comm_to = (BOOL)(data & 1);
  ds.over_temperature = (BOOL)((data >>= 5) & 1);
  ds.no_power         = (BOOL)((data >>= 1) & 1);
  return ds;
}

#endif // Include guard

