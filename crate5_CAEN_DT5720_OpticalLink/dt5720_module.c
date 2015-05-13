/*---------------------------------*/
/* SOFTWARE CONTROLLED ACQUISITION */
//#define SW_CONTROL
#ifdef SW_CONTROL
#undef SW_CONTROL
#endif
/*---------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/io.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "midas.h"

#include "crate.h"
#include "diag.h"

// CAEN includes
#include "CAENDigitizer.h"
#include "CAENDigitizerType.h"

//
#include "../CAEN_DesktopDigi_utils/CAENdtDigiUtils.h"

static int       handle;
static uint32_t  VME_BASE = 0x00000000;
static char      *caen_data_buffer = NULL;         // data buffers used by CAEN
static uint32_t  caen_data_buffer_size = 0;
static char      *data_buffer;              // data buffers used by MIDAS
static uint32_t  data_buffer_size = 32*1024*1024;
static uint32_t  data_size;
static BOOL      pll_lost = FALSE;   // PLL lost during MIDAS block?
static BOOL      board_full = FALSE; // Board full at least once during MIDAS block?



static INT dt5720_init();
static void dt5720_exit();
static INT dt5720_pre_bor();
static INT dt5720_eor();
static INT dt5720_poll_live();
static INT dt5720_read(char *pevent); // MIDAS readout routine 
static BOOL dt5720_readout(); // read data from digitizer buffers

static BOOL dt5720_update_digitizer();
static BOOL is_caen_error(CAEN_DGTZ_ErrorCode,int, const char*);
static uint32_t analog2adc_trigger(int);
static uint32_t analog2adc_offset(int);
static BOOL cleanup();

bool block_sig;
typedef struct timespec timer_start;

struct readout_module dt5720_module = {
  dt5720_init,             // init
  dt5720_exit,             // exit
  dt5720_pre_bor,          // pre_bor
  NULL,                    // bor
  dt5720_eor,              // eor
  dt5720_poll_live,        // poll_live
  NULL,                    // poll_dead
  NULL,                    // start_block
  NULL,                    // stop_block
  dt5720_read              // read
};

// ======================================================================
// ODB structures
// ======================================================================

typedef struct s_dt5720_odb{
  BYTE      software_trigger_mode;
  BYTE      hardware_trigger_mode;
  BYTE      self_trigger_mode;
  BOOL      trigger_positive_edge;
  DWORD     max_events_per_block;
  BOOL      event_aligned_readout;
  char      logic_level[4];
  BYTE      post_trigger_size;
  BOOL      gpi_acquisition_mode;
  BYTE      zero_suppression_mode;
  BOOL      event_packing;
  BOOL      ext_clock;
  DWORD     wf_length;
  struct {
    BOOL      enable;
    float     offset;
    float     self_trigger_threshold;
  } ch00;
  struct {
    BOOL      enable;
    float     offset;
    float     self_trigger_threshold;
  } ch01;
  struct {
    BOOL      enable;
    float     offset;
    float     self_trigger_threshold;
  } ch02;
  struct {
    BOOL      enable;
    float     offset;
    float     self_trigger_threshold;
  } ch03;
} S_DT5720_ODB_DEF;

static S_DT5720_ODB_DEF S_DT5720_ODB;

#define S_DT5720_ODB_STR "\
[.]\n\
software_trigger_mode = BYTE : 0\n\
hardware_trigger_mode = BYTE : 0\n\
self_trigger_mode = BYTE : 0\n\
trigger_positive_edge = BOOL : y\n\
max_events_per_block = DWORD : 1024\n\
event_aligned_readout = BOOL : y\n\
logic_level = STRING : [4] NIM\n\
post_trigger_size = BYTE : 20\n\
gpi_acquisition_mode = BOOL : n\n\
zero_suppression_mode = BYTE : 0\n\
event_packing = BOOL : n\n\
ext_clock = BOOL : n\n\
waveform length = DWORD : 64\n\
\n\
[Ch00]\n\
enable = BOOL : n\n\
offset = FLOAT : 0\n\
self_trigger_threshhold = FLOAT : 0\n\
\n\
[Ch01]\n\
enable = BOOL : n\n\
offset = FLOAT : 0\n\
self_trigger_threshhold = FLOAT : 0\n\
\n\
[Ch02]\n\
enable = BOOL : n\n\
offset = FLOAT : 0\n\
self_trigger_threshhold = FLOAT : 0\n\
\n\
[Ch03]\n\
enable = BOOL : n\n\
offset = FLOAT : 0\n\
self_trigger_threshhold = FLOAT : 0\
"


extern HNDLE hDB;

INT dt5720_init()
{
  
  //Power cycling
  //simulate_power_cycle(0);//0 or 1 selects the 5720 or 5730, depending on which one had its usb plugged in first

  BOOL use_optical = true;
  if (use_optical){
     printf("Opening CAEN OpticalLink interface ...");
  } else {
    printf("Opening CAEN USB interface ...");
  }
  fflush(stdout);

  HNDLE hKey;
  CAEN_DGTZ_ErrorCode ret;
  CAEN_DGTZ_BoardInfo_t BoardInfo;

  /* Grab Board */
  /* int iLine = 0;
       ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,iLine,0,VME_BASE,&handle);*/
  //ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,VME_BASE,&handle);

  // for A3818 PCIe card
  if (use_optical){
    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_OpticalLink,0,1,VME_BASE,&handle);
  } else {
    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,VME_BASE,&handle);
  }
  printf("\nCAEN Handle:\t%d\n", handle);
  if(is_caen_error(ret,__LINE__-1,"dt5720_init")) return FE_ERR_HW;

  /* Get Board Info */
  ret = CAEN_DGTZ_GetInfo(handle,&BoardInfo);
  if(is_caen_error(ret,__LINE__-1,"dt5720_init")) return FE_ERR_HW;
  printf("\nConnected to CAEN Desktop Digitizer Model %s\n", BoardInfo.ModelName);
  printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
  printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
  
  /* Reset Digitizer */
  ret = CAEN_DGTZ_Reset(handle);
  if(is_caen_error(ret,__LINE__-1,"dt5720_init")) return FE_ERR_HW;
  

  /* Link to Database */
  //cm_get_experiment_database(&hDB,NULL);
  char str[1024];
  sprintf(str,"/Equipment/Crate %i/Settings/CAEN",crate_number);
  int status = db_check_record(hDB,0,str,S_DT5720_ODB_STR,FALSE);
  if(status==DB_NO_KEY || status==DB_STRUCT_MISMATCH)
    {
      status = db_create_record(hDB,0,str,S_DT5720_ODB_STR);
      printf("Creating key with status: %d \n",status);
    }
  else if (status==DB_INVALID_HANDLE)
    {
      printf("Error: Cannot connect to database to setup CAEN DT5720.");
    }

  status = db_find_key(hDB,0,str,&hKey);
  status = db_open_record(hDB,hKey,&S_DT5720_ODB,sizeof(S_DT5720_ODB),MODE_READ,NULL,NULL);

  // configure clock input
  if ( S_DT5720_ODB.ext_clock ) 
    {
      printf("Using external clock\n");
      ret = CAEN_DGTZ_WriteRegister(handle, CAEN_DGTZ_ACQ_CONTROL_ADD, 1<<6); 
      if(is_caen_error(ret,__LINE__-1,"dt5720_init")) return FE_ERR_HW;
    }
  else
    {
      ret = CAEN_DGTZ_WriteRegister(handle, CAEN_DGTZ_ACQ_CONTROL_ADD, 0); 
      if(is_caen_error(ret,__LINE__-1,"dt5720_init")) return FE_ERR_HW;
      printf("Using internal clock\n");
    }


  data_buffer = (char*) malloc(data_buffer_size);


  return status;

}

void dt5720_exit()
{

  CAEN_DGTZ_ErrorCode ret;
  ret = CAEN_DGTZ_CloseDigitizer(handle);
  is_caen_error(ret,__LINE__-1,"dt5720_exit");
  free(data_buffer);
}

INT dt5720_pre_bor()
{
  CAEN_DGTZ_ErrorCode ret;

  /* Setup Digitizer */
  if(!dt5720_update_digitizer()) return FE_ERR_HW;


  /* Allocate space for readout from digitizer */
  ret = CAEN_DGTZ_MallocReadoutBuffer(handle,&caen_data_buffer,&caen_data_buffer_size);
  if(is_caen_error(ret,__LINE__-1,"dt5720_bor")) return FE_ERR_HW;
  printf("Allocated %i bytes for CAEN data buffer\n", caen_data_buffer_size);

  /* Clear any remaining data */
  ret = CAEN_DGTZ_ClearData(handle);
  if(is_caen_error(ret,__LINE__-1,"dt5720_bor")) return FE_ERR_HW;

  data_size = 0;

  // VT
  //ret = CAEN_DGTZ_Reset(handle);
  //ret = CAEN_DGTZ_SetRecordLength(handle,32);
  //ret = CAEN_DGTZ_SetChannelEnableMask(handle,1);
  //ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,0,32768);		
  //ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY,1);
  //ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY); 
  //ret = CAEN_DGTZ_SetAcquisitionMode(handle,CAEN_DGTZ_S_IN_CONTROLLED);

  // The SW Acq bit must be set/unset even for external gate control (S_IN via GPI)
  ret = CAEN_DGTZ_SWStartAcquisition(handle);
  if(is_caen_error(ret,__LINE__-1,"dt5720_bor")) return FE_ERR_HW;

  // I found that I need to add sleep here. Otherwise I get HW error in digitizer
  // (the red "busy" light on the front panel turns on; no communication with the board)
  //ss_sleep(100);

  return SUCCESS;
}

INT dt5720_eor()
{
  CAEN_DGTZ_ErrorCode ret;

  // The SW Acq bit must be set/unset even for external gate control (S_IN via GPI)
  ret = CAEN_DGTZ_SWStopAcquisition(handle);
  if(is_caen_error(ret,__LINE__-1,"dt5720_bor")) return FE_ERR_HW;


  ret = CAEN_DGTZ_FreeReadoutBuffer(&caen_data_buffer);
  if(is_caen_error(ret,__LINE__-1,"dt5720_eor")) return FE_ERR_HW;

  caen_data_buffer = NULL;

  return SUCCESS;
}

INT dt5720_read(char *pevent)
{
  //printf("Read out data from desktop digitizer\n");

  // =====================================================================================
  // Read out remaining data from the digitizer
  // =====================================================================================
  BOOL readOK = dt5720_readout();
  if (!readOK){
    printf("readOK is FALSE\n");
     return FE_ERR_HW;
  }
 
  // =====================================================================================
  // Fill MIDAS event
  // =====================================================================================
  bk_init32(pevent);
  char bk_name[5];
  char *pdata;

  sprintf(bk_name,"CND0");
  bk_create(pevent, bk_name, TID_BYTE, &pdata);
  if ( data_size > MAX_EVENT_SIZE )
    {
      cm_msg(MERROR,"dt5720_read","Event size is too large. Truncating data...\n");
      data_size = MAX_EVENT_SIZE;
    }
  memcpy(pdata, data_buffer, data_size);
  pdata += data_size;
  bk_close(pevent, pdata);
  // reset data counter for the next event
  data_size = 0;

  // =====================================================================================
  // Get status information
  // =====================================================================================
  sprintf(bk_name, "CNS0");
  bk_create(pevent, bk_name, TID_BYTE, &pdata);
  memcpy(pdata++, &pll_lost, 1);
  memcpy(pdata++, &board_full, 1);
  pll_lost = board_full = FALSE;
  bk_close(pevent, pdata);
  
  return SUCCESS;
}

BOOL dt5720_readout()
{
  CAEN_DGTZ_SendSWtrigger(handle);
  CAEN_DGTZ_SendSWtrigger(handle);
  CAEN_DGTZ_SendSWtrigger(handle);
  CAEN_DGTZ_ErrorCode ret;  
  uint32_t caen_data_size;

#if 0
  // VT
  uint32_t data;
  ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_CONTROL_ADD, &data);
  printf("Acquisition control status: 0x%08x\n",data);
#endif


  /* Read out data if there is any */
  ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,caen_data_buffer, &caen_data_size);
  if (is_caen_error(ret,__LINE__-1,"dt5720_readout")) return FALSE;
   /* printf("Error in CAEN_DGTZ_ReadData()\n"); */
   /* if (ret==CAEN_DGTZ_CommError){ */
   /*    printf("Stopping digitizer acquisition...\n"); */
   /*    ret = CAEN_DGTZ_SWStopAcquisition(handle); */
   /*    is_caen_error(ret,__LINE__-1,"dt5720_readout: CAEN_DGTZ_SWStopAcquisition"); */

   /*    ret = CAEN_DGTZ_FreeReadoutBuffer(&caen_data_buffer); */
   /*    is_caen_error(ret,__LINE__-1,"dt5720_readout: CAEN_DGTZ_FreeReadoutBuffer"); */

   /*    caen_data_buffer = NULL; */

   /*    ret = CAEN_DGTZ_Reset(handle); */
   /*    is_caen_error(ret,__LINE__-1,"dt5720_readout: CAEN_DGTZ_Reset"); */

   /*    //Software Reset */
   /*    ret = CAEN_DGTZ_WriteRegister(handle, 0xEF24, 1); */
   /*    is_caen_error(ret,__LINE__-1,"dt5720_readout: CAEN_DGTZ_WriteRegister"); */

   /*    //---Configuration Reload--- */
   /*    ret = CAEN_DGTZ_WriteRegister(handle, 0xEF34, 1); */
   /*    is_caen_error(ret,__LINE__-1,"dt5720_readout: CAEN_DGTZ_WriteRegister (Config Reload)"); */

   /*    ss_sleep(10); */
   /*    printf("Closing digitizer...\n"); */
   /*    ret = CAEN_DGTZ_CloseDigitizer(handle); */
   /*    is_caen_error(ret,__LINE__-1,"dt5720_readout: CAEN_DGTZ_CloseDigitizer"); */

   /*    ss_sleep(10); */
   /*    printf("Opening digitizer (USB Mode)...\n"); */
   /*    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,VME_BASE,&handle); */
   /*    is_caen_error(ret,__LINE__-1,"dt5720_readout: CAEN_DGTZ_OpenDigitizer"); */
   /*    if (ret==0){ */
   /* 	printf("re-established communication using the USB.\n"); */
   /*      ret = CAEN_DGTZ_Reset(handle); */
   /*      is_caen_error(ret,__LINE__-1,"dt5720_readout: CAEN_DGTZ_Reset"); */
   /*      printf("Closing digitizer...\n"); */
   /*      ret = CAEN_DGTZ_CloseDigitizer(handle); */
   /*      is_caen_error(ret,__LINE__-1,"dt5720_readout: CAEN_DGTZ_CloseDigitizer"); */
   /*      //ret = CAEN_DGTZ_SWStartAcquisition(handle); */
   /*    } else { */
   /*       is_caen_error(ret,__LINE__-1,"dt5720_readout"); */
   /*    } */

   /*    printf("Opening digitizer (Optical Mode)...\n"); */
   /*    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_PCIE_OpticalLink,0,1,VME_BASE,&handle); */
   /*    if (ret==0){ */
   /* 	printf("re-established communication.\n"); */
   /*      ret = CAEN_DGTZ_SWStartAcquisition(handle); */
   /*    } else { */
   /*       is_caen_error(ret,__LINE__-1,"dt5720_readout"); */
   /*    } */

   /* } */

   // Check if board full or loss of PLL lock
   
   
  /* return TRUE; */
  /* } */
  caen_digi_status ds = caen_digi_get_status(handle);
  if (ds.pll_lost)
    pll_lost = TRUE;
  if (ds.board_full)
    board_full = TRUE;

  /* If there's data, copy from digitizers local buffer to different local buffer */
  if(caen_data_size > 0)
    {
      printf("data size: %i\n", caen_data_size);
      
      if(data_size+caen_data_size < data_buffer_size )
	{
	  memcpy((data_buffer+data_size), caen_data_buffer, caen_data_size);
	  data_size += caen_data_size;
	}
    }
  return TRUE;
}

INT dt5720_poll_live()
{

  // dt5720_readout();
 BOOL readOK = dt5720_readout();
  if (!readOK){
    printf("readOK is FALSE\n");
     return FE_ERR_HW;
  }
  return SUCCESS;
}

BOOL dt5720_update_digitizer()
{
  CAEN_DGTZ_ErrorCode ret;
  /* General CAEN settings */


  switch(S_DT5720_ODB.software_trigger_mode)
    {
    case 0:
      ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_DISABLED);
      break;
    case 1:
      ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY);
      break;
    case 2:
      ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_EXTOUT_ONLY);
      break;
    case 3:
      ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
      break;
    default:
      ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_DISABLED);
      cm_msg(MINFO,"dt5720_update_digitizer","Invalid software trigger mode. Set to DISABLED. (%i)",(int)S_DT5720_ODB.software_trigger_mode);
      break;
    }
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;

  switch(S_DT5720_ODB.hardware_trigger_mode)
    {
    case 0:
      ret = CAEN_DGTZ_SetExtTriggerInputMode(handle,CAEN_DGTZ_TRGMODE_DISABLED);
      break;
    case 1:
      ret = CAEN_DGTZ_SetExtTriggerInputMode(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY);
      break;
    case 2:
      ret = CAEN_DGTZ_SetExtTriggerInputMode(handle,CAEN_DGTZ_TRGMODE_EXTOUT_ONLY);
      break;
    case 3:
      ret = CAEN_DGTZ_SetExtTriggerInputMode(handle,CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
      break;
    default:
      ret = CAEN_DGTZ_SetExtTriggerInputMode(handle,CAEN_DGTZ_TRGMODE_DISABLED);
      cm_msg(MINFO,"dt5720_update_digitizer","Invalid hardware trigger mode. Set to DISABLED.");
      break;
    }
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;


  uint32_t caen_self_trigger_mask = 0b1111;
  switch(S_DT5720_ODB.self_trigger_mode)
    {
    case 0:
      ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_DISABLED,caen_self_trigger_mask);
      break;
    case 1:
      ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY,caen_self_trigger_mask);
      break;
    case 2:
      ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_EXTOUT_ONLY,caen_self_trigger_mask);
      break;
    case 3:
      ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT,caen_self_trigger_mask);
      break;
    default:
      ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_DISABLED,caen_self_trigger_mask);
      cm_msg(MINFO,"dt5720_update_digitizer","Invalid channel self trigger mode. Set to DISABLED. (%i)",(int)S_DT5720_ODB.self_trigger_mode);
      break;
    }
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;


  // According to correspondence with CAEN, second argument (channel nuumber) is ignored.
  // But just in case...
  if(S_DT5720_ODB.trigger_positive_edge)
    {
      ret = CAEN_DGTZ_SetTriggerPolarity(handle,0,CAEN_DGTZ_TriggerOnRisingEdge);
      ret = CAEN_DGTZ_SetTriggerPolarity(handle,1,CAEN_DGTZ_TriggerOnRisingEdge);
      ret = CAEN_DGTZ_SetTriggerPolarity(handle,2,CAEN_DGTZ_TriggerOnRisingEdge);
      ret = CAEN_DGTZ_SetTriggerPolarity(handle,3,CAEN_DGTZ_TriggerOnRisingEdge);
    }
  else
    {
      ret = CAEN_DGTZ_SetTriggerPolarity(handle,0,CAEN_DGTZ_TriggerOnFallingEdge);
      ret = CAEN_DGTZ_SetTriggerPolarity(handle,1,CAEN_DGTZ_TriggerOnFallingEdge);
      ret = CAEN_DGTZ_SetTriggerPolarity(handle,2,CAEN_DGTZ_TriggerOnFallingEdge);
      ret = CAEN_DGTZ_SetTriggerPolarity(handle,3,CAEN_DGTZ_TriggerOnFallingEdge);
    }
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;


  if(S_DT5720_ODB.max_events_per_block >= 2 && S_DT5720_ODB.max_events_per_block <= 1024)
    {
      ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle,S_DT5720_ODB.max_events_per_block);
    }
  else
    {
      ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle,1024);
      cm_msg(MINFO,"dt5720_update_digitizer","Invalid max block size. Set to 1024.");
    }
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;


  if(strcmp(S_DT5720_ODB.logic_level,"NIM") == 0)
    ret = CAEN_DGTZ_SetIOLevel(handle,CAEN_DGTZ_IOLevel_NIM);
  else if(strcmp(S_DT5720_ODB.logic_level,"TTL") == 0)
    ret = CAEN_DGTZ_SetIOLevel(handle,CAEN_DGTZ_IOLevel_TTL);
  else
    {
      ret = CAEN_DGTZ_SetIOLevel(handle,CAEN_DGTZ_IOLevel_NIM);
      cm_msg(MINFO,"dt5720_update_digitizer","Invalid logic level. Set to NIM.");
    }
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;


  if(S_DT5720_ODB.post_trigger_size >= 0 && S_DT5720_ODB.post_trigger_size <=100)
    {
      ret = CAEN_DGTZ_SetPostTriggerSize(handle,S_DT5720_ODB.post_trigger_size);
    }
  else
    {
      ret = CAEN_DGTZ_SetPostTriggerSize(handle,80);
      cm_msg(MINFO,"dt5720_update_digitizer","Invalid post trigger size. Set to 80 percent.");
    }
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;

  if(S_DT5720_ODB.gpi_acquisition_mode)
    {
      ret = CAEN_DGTZ_SetAcquisitionMode(handle,CAEN_DGTZ_S_IN_CONTROLLED);
      cm_msg(MINFO,"dt5720_update_digitizer","Using S_IN_CONTROLLED mode");
    }
  else
    {
      ret = CAEN_DGTZ_SetAcquisitionMode(handle,CAEN_DGTZ_SW_CONTROLLED);
      cm_msg(MINFO,"dt5720_update_digitizer","Using SW_CONTROLLED mode");
    }
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;


  switch(S_DT5720_ODB.zero_suppression_mode)
    {
    case 0:
      ret = CAEN_DGTZ_SetZeroSuppressionMode(handle,CAEN_DGTZ_ZS_NO);
      break;
    case 2:
      ret = CAEN_DGTZ_SetZeroSuppressionMode(handle,CAEN_DGTZ_ZS_ZLE);
      break;
    case 3:
      ret = CAEN_DGTZ_SetZeroSuppressionMode(handle,CAEN_DGTZ_ZS_AMP);
      break;
    default:
      ret = CAEN_DGTZ_SetZeroSuppressionMode(handle,CAEN_DGTZ_ZS_NO);
      cm_msg(MINFO,"dt5720_update_digitizer","Invalid zero suppression mode. Set to none.");
      break;
    }
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;


  if(S_DT5720_ODB.event_packing)
    {
      ret = CAEN_DGTZ_SetEventPackaging(handle,CAEN_DGTZ_ENABLE);
    }
  else
    {
      ret = CAEN_DGTZ_SetEventPackaging(handle,CAEN_DGTZ_DISABLE);
    }
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;

  // Channel specific
  uint32_t caen_channel_mask = 0;
  if(S_DT5720_ODB.ch00.enable)
    caen_channel_mask |= (1 << 0);
  if(S_DT5720_ODB.ch01.enable)
    caen_channel_mask |= (1 << 1);
  if(S_DT5720_ODB.ch02.enable)
    caen_channel_mask |= (1 << 2);
  if(S_DT5720_ODB.ch03.enable)
    caen_channel_mask |= (1 << 3);
  ret = CAEN_DGTZ_SetChannelEnableMask(handle,caen_channel_mask);
  cm_msg(MINFO,"DT5720","channel enable mask: 0x%08x",caen_channel_mask);

  ret = CAEN_DGTZ_SetChannelDCOffset(handle,
					      0,
					      analog2adc_offset(0));
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;
  ret = CAEN_DGTZ_SetChannelDCOffset(handle,
					      1,
					      analog2adc_offset(1));
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;
  ret = CAEN_DGTZ_SetChannelDCOffset(handle,
					      2,
					      analog2adc_offset(2));
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;
  ret = CAEN_DGTZ_SetChannelDCOffset(handle,
					      3,
					      analog2adc_offset(3));
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;

  ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,
						      0,
						      analog2adc_trigger(0));
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;
  ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,
						      1,
						      analog2adc_trigger(1));
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;
  ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,
						      2,
						      analog2adc_trigger(2));
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;
  ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,
						      3,
						      analog2adc_trigger(3));
  if(is_caen_error(ret,__LINE__,"dt5720_update_digitizer")) return false;


  // =====================================================================================
  // Record length
  // =====================================================================================
  ret = CAEN_DGTZ_SetRecordLength(handle, S_DT5720_ODB.wf_length);
  if ( ret != CAEN_DGTZ_Success )
    {
      cm_msg(MERROR,"dt5720_update_digitizer","Cannot SetRecordLength. Error 0x%08x\n",ret);
      return FE_ERR_HW;
    }

  
  // ======================================================================================
  // Channel configuration through register 0x8000=CAEN_DGTZ_BROAD_CH_CTRL_ADD
  // ======================================================================================
  uint32_t data;
  ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_BROAD_CH_CTRL_ADD, &data);
  if(is_caen_error(ret,__LINE__-1,"dt5720_update_digitizer")) return FE_ERR_HW;

  // Bit 4 must always be set to 1
  data |= (1<<4);
  // enable trigger overlap
  data |= (1<<1);
  ret = CAEN_DGTZ_WriteRegister(handle, CAEN_DGTZ_BROAD_CH_CTRL_ADD, data);
  if(is_caen_error(ret,__LINE__-1,"dt5720_update_digitizer")) return FE_ERR_HW;


  return true;
}

uint32_t analog2adc_offset(int ch)
{

  static const uint32_t adcmax = 65535;
  static const uint32_t adcmin = 0;
  static const float analogmax = 1.;
  static const float analogmin = -1.;
  static const float slope = (float)(adcmax-adcmin)/(analogmax-analogmin);

  /*
    Map analogue voltage offset (-1V-1V) to 16 bit ADC value (0-65535=2^16-1)
    Dynamic ranges:
    0x0000 -> -2V to 0V
    0x8000 -> -1V to 1V
    0xFFFF ->  0V to 2V
  */
  float v;
  switch(ch)
    {
    case 0: v = S_DT5720_ODB.ch00.offset; break;
    case 1: v = S_DT5720_ODB.ch01.offset; break;
    case 2: v = S_DT5720_ODB.ch02.offset; break;
    case 3: v = S_DT5720_ODB.ch03.offset; break;
    default: return 0x8000;
    }

  if(v>analogmax)
    {
      cm_msg(MINFO,"dt5720_update_digitizer","Invalid voltage offset. Set to 1V.");
      return adcmin;
    }
  else if(v<analogmin)
    {
      cm_msg(MINFO,"dt5720_update_digitizer","Invalid voltage offset. Set to -1V.");
      return adcmax;
    }

  v += 1.;

  uint32_t dac = slope*v;
  printf("DAC offset: channel %i DAC %i\n",ch, dac);


  return dac;
}

uint32_t analog2adc_trigger(int ch)
{
  static const float dynamicrange = 2.;
  static const uint32_t adcmax = 4095;
  //static const uint32_t adcmax = 65535;
  static const uint32_t adcmin = 0;
  static const float slope = (adcmax-adcmin)/dynamicrange;

  float v, vmin;

  switch(ch)
    {
    case 0:
      v = S_DT5720_ODB.ch00.self_trigger_threshold;
      vmin = S_DT5720_ODB.ch00.offset - 1.;
      break;
    case 1:
      v = S_DT5720_ODB.ch01.self_trigger_threshold;
      vmin = S_DT5720_ODB.ch01.offset - 1.;
      break;
    case 2:
      v = S_DT5720_ODB.ch02.self_trigger_threshold;
      vmin = S_DT5720_ODB.ch02.offset - 1.;
      break;
    case 3:
      v = S_DT5720_ODB.ch03.self_trigger_threshold;
      vmin = S_DT5720_ODB.ch03.offset - 1.;
      break;
    default:
      return (adcmax-adcmin)/2;
    }

  if(v<vmin || v>(vmin+dynamicrange))
    {
      v = vmin+dynamicrange/2.;
      cm_msg(MINFO,"dt5720_update_digitizer","Invalid self trigger threshold. Set to center.");
    }

  uint32_t dac = (v-vmin)*slope;
  printf("Trigger threshold: channel %i DAC %i DAC\n",ch,dac);
  
  return dac;
}

BOOL is_caen_error(CAEN_DGTZ_ErrorCode e, int l, const char* r)
{
  /*
    l: line number
    f: file name
    r: parent routine
  */

  static char* f = __FILE__;

  if(e==CAEN_DGTZ_Success)
    return FALSE;

  switch(e)
    {
    case CAEN_DGTZ_CommError:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Communication error.");      
      break;
    case CAEN_DGTZ_GenericError:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Generic (unspecified) error.");
      break;
    case CAEN_DGTZ_InvalidParam:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Invalid parameter error.");
      break;
    case CAEN_DGTZ_InvalidLinkType:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Invalid LinkType.");
      break;
    case CAEN_DGTZ_InvalidHandle:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Invalid handle.");
      break;
    case CAEN_DGTZ_MaxDevicesError:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Too many devices.");
      break;
    case CAEN_DGTZ_BadBoardType:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Operation not allowed on this model.");
      break;
    case CAEN_DGTZ_BadInterruptLev:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Invalid interrupt level.");
      break;
    case CAEN_DGTZ_BadEventNumber:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Invalid event number.");
      break;
    case CAEN_DGTZ_ReadDeviceRegisterFail:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Unable to read from registry.");
      break;
    case CAEN_DGTZ_WriteDeviceRegisterFail:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Unable to write to registry.");
      break;
    case CAEN_DGTZ_InvalidChannelNumber:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Invalid channel number.");
      break;
    case CAEN_DGTZ_ChannelBusy:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Channel is busy.");
      break;
    case CAEN_DGTZ_FPIOModeInvalid:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Invalid FPIO (?) mode.");
      break;
    case CAEN_DGTZ_WrongAcqMode:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Invalid acquisition mode.");
      break;
    case CAEN_DGTZ_FunctionNotAllowed:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Module does not support this function.");
      break;
    case CAEN_DGTZ_Timeout:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Communication timed out.");
      break;
    case CAEN_DGTZ_InvalidBuffer:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Invalid buffer (readout or event).");
      break;
    case CAEN_DGTZ_EventNotFound:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Unable to find event.");
      break;
    case CAEN_DGTZ_InvalidEvent:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Event is invalid.");
      break;
    case CAEN_DGTZ_OutOfMemory:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Memory has run out.");
      break;
    case CAEN_DGTZ_CalibrationError:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Calibration failed.");
      break;
    case CAEN_DGTZ_DigitizerNotFound:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Unable to locate digitizer.");
      break;
    case CAEN_DGTZ_DigitizerAlreadyOpen:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Digitizer is already open.");
      break;
    case CAEN_DGTZ_DigitizerNotReady:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Digitizer not ready.");
      break;
    case CAEN_DGTZ_InterruptNotConfigured:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Interrupts not configured.");
      break;
    case CAEN_DGTZ_DigitizerMemoryCorrupted:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Flash memory corrupted.");
      break;
    case CAEN_DGTZ_DPPFirmwareNotSupported:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Firmware not supported by these libraries.");
      break;
    case CAEN_DGTZ_NotYetImplemented:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Function not yet implemented.");
      break;
    default:
      cm_msg(MT_ERROR,f,l,r,"CAEN DT5720 Unknown error.");
      break;
    }
  //if (e!=CAEN_DGTZ_InvalidHandle){ 
  //   cleanup();
  //}
  return TRUE;
}

BOOL cleanup()
{
  //tell MIDAS to send EOR to all frontends
  
  
  CAEN_DGTZ_ErrorCode ret;
  //ret = CAEN_DGTZ_WriteRegister(handle, CAEN_DGTZ_ACQ_CONTROL_ADD, 1<<6); 

  ret = CAEN_DGTZ_CloseDigitizer(handle);
  if(ret<0){
    printf("Error closing. ret=%d\n",ret);
    
    return FALSE;
  }
  return TRUE;
}
