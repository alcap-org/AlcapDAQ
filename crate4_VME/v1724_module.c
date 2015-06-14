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

#define NBOARDS    1             /* Number of connected boards */
static int dev_handle[NBOARDS];
//static uint32_t V1724_BASE[NBOARDS] = {0x32100000};
static char      *caen_data_buffer[NBOARDS];         // data buffers used by CAEN
static uint32_t  caen_data_buffer_size[NBOARDS];
static char      *data_buffer[NBOARDS];              // data buffers used by MIDAS
static uint32_t  data_buffer_size[NBOARDS];
static uint32_t  data_size[NBOARDS];

extern HNDLE hDB;

static INT v1724_init();
static void v1724_exit();
static INT v1724_pre_bor();
static INT v1724_eor();
static INT v1724_poll_live();
static INT v1724_read(char *pevent); // MIDAS readout routine
static void v1724_readout(); // read data from digitizer buffers


bool block_sig;
typedef struct timespec timer_start;

struct readout_module v1724_module = {
  v1724_init,             // init
  v1724_exit,             // exit
  v1724_pre_bor,          // pre_bor
  NULL,                   // bor
  v1724_eor,              // eor
  v1724_poll_live,        // poll_live
  NULL,                   // poll_dead
  NULL,                   // start_block
  NULL,                   // stop_block
  v1724_read              // read
};

// ======================================================================
// ODB structures
// ======================================================================

// board settings
typedef struct s_v1724_odb
{
  DWORD vme_base;                     ///< VME base address of the module
  BOOL enabled;                       ///< don't readout the module if false
  BYTE  board_num;                    ///< Board number in daisychance
  BYTE  link_num;                     ///< Link number in A3818 card
  DWORD wf_length;                    ///< waveform length, samples
  BYTE  acquisition_mode;             ///< Acquisition mode: 0=CAEN_DGTZ_SW_CONTROLLED, 1=CAEN_DGTZ_S_IN_CONTROLLED;
  BYTE  software_trigger_mode;        ///< Software trigger mode: 0=CAEN_DGTZ_TRGMODE_DISABLED, 1=CAEN_DGTZ_TRGMODE_ACQ_ONLY, 2=CAEN_DGTZ_TRGMODE_EXTOUT_ONLY, 3=CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT
  BYTE  hardware_trigger_mode;        ///< Hardware trigger mode: 0=CAEN_DGTZ_TRGMODE_DISABLED, 1=CAEN_DGTZ_TRGMODE_ACQ_ONLY, 2=CAEN_DGTZ_TRGMODE_EXTOUT_ONLY, 3=CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT
  BYTE  trigger_edge;                 ///< Trigger edge, 0=CAEN_DGTZ_TriggerOnRisingEdge, 1=CAEN_DGTZ_TriggerOnFallingEdge
  INT down_sampling_factor;           ///< Sampling frequency will be divided by this factor, 1=no down sampling
  char  ROC_FirmwareRel[128];
  char  AMC_FirmwareRel[128];
  struct {
    BOOL      enabled;
    DWORD     offset;                 ///< DC offset
    BYTE      self_trigger_mode;      ///< Self trigger mode:  0=CAEN_DGTZ_TRGMODE_DISABLED, 1=CAEN_DGTZ_TRGMODE_ACQ_ONLY, 2=CAEN_DGTZ_TRGMODE_EXTOUT_ONLY, 3=CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT
    DWORD     trigger_threshold;      ///< Trigger threshold
  } ch[8];

} S_V1724_ODB_DEF;

static S_V1724_ODB_DEF S_V1724_ODB[NBOARDS];

#define S_V1724_ODB_STR "\
[.]\n\
VME base = DWORD : 0x32100000\n\
enabled = BOOL : y\n\
Board number = BYTE : 0\n\
Link number = BYTE : 0\n\
waveform length = DWORD : 32\n\
acquisition mode = BYTE : 1\n\
software trigger mode = BYTE : 0\n\
hardware trigger mode = BYTE : 0\n\
trigger edge (0-ris 1-fal) = BYTE : 1\n\
down sampling factor = INT : 1\n\
ROC firmware = STRING : [128] -\n\
AMC firmware = STRING : [128] -\n\
\n\
[Ch0]\n\
enabled = BOOL : y\n\
DC offset = DWORD : 0\n\
self-trigger mode = BYTE : 0\n\
trigger threshhold = DWORD : 0\n\
\n\
[Ch1]\n\
enabled = BOOL : y\n\
DC offset = DWORD : 0\n\
self-trigger mode = BYTE : 1\n\
trigger threshhold = DWORD : 0\n\
\n\
[Ch2]\n\
enabled = BOOL : y\n\
DC offset = DWORD : 0\n\
self-trigger mode = BYTE : 1\n\
trigger threshhold = DWORD : 0\n\
\n\
[Ch3]\n\
enabled = BOOL : y\n\
DC offset = DWORD : 0\n\
self-trigger mode = BYTE : 1\n\
trigger threshhold = DWORD : 0\n\
\n\
[Ch4]\n\
enabled = BOOL : y\n\
DC offset = DWORD : 0\n\
self-trigger mode = BYTE : 1\n\
trigger threshhold = DWORD : 0\n\
\n\
[Ch5]\n\
enabled = BOOL : y\n\
DC offset = DWORD : 0\n\
self-trigger mode = BYTE : 1\n\
trigger threshhold = DWORD : 0\n\
\n\
[Ch6]\n\
enabled = BOOL : y\n\
DC offset = DWORD : 0\n\
self-trigger mode = BYTE : 1\n\
trigger threshhold = DWORD : 0\n\
\n\
[Ch7]\n\
enabled = BOOL : y\n\
DC offset = DWORD : 0\n\
self-trigger mode = BYTE : 1\n\
trigger threshhold = DWORD : 0\n\
"

INT v1724_init()
{

  /* ODB */
  char str[1024];
  HNDLE hKey;
  for (int iboard=0; iboard<NBOARDS; iboard++)
    {
      sprintf(str,"/Equipment/Crate %i/Settings/CAEN%i",crate_number,iboard);
      int status = db_check_record(hDB,0,str,S_V1724_ODB_STR,FALSE);
      if(status==DB_NO_KEY || status==DB_STRUCT_MISMATCH)
	{
	  status = db_create_record(hDB,0,str,S_V1724_ODB_STR);
	  printf("Creating key with status: %d \n",status);
	}
      else if (status==DB_INVALID_HANDLE)
	{
	  printf("Error: Cannot connect to database to setup CAEN V1724");
	  return FE_ERR_HW;
	}


      status = db_find_key(hDB,0,str,&hKey);
      status = db_open_record(hDB, hKey, &(S_V1724_ODB[iboard]), sizeof(S_V1724_ODB[0]), MODE_READ, NULL, NULL);
    }

  printf("Opening CAEN VME interface ...");
  fflush(stdout);

  CAEN_DGTZ_ErrorCode ret;
  CAEN_DGTZ_BoardInfo_t BoardInfo[NBOARDS];
  uint32_t data;

  //CAEN_DGTZ_ConnectionType linkType = CAEN_DGTZ_USB;
  CAEN_DGTZ_ConnectionType linkType = CAEN_DGTZ_OpticalLink;

  for (int iboard=0; iboard<NBOARDS; iboard++)
    {

      if ( !S_V1724_ODB[iboard].enabled ) continue;
      ret = CAEN_DGTZ_OpenDigitizer(linkType,
				    S_V1724_ODB[iboard].link_num,
				    S_V1724_ODB[iboard].board_num,
				    S_V1724_ODB[iboard].vme_base,
				    &dev_handle[iboard]);
      if(ret != CAEN_DGTZ_Success)
	{
	  cm_msg(MERROR,"v1724_init","Can't open digitizer at address 0x%08x\n",S_V1724_ODB[iboard].vme_base);
	  return FE_ERR_HW;
	}

      ret = CAEN_DGTZ_GetInfo(dev_handle[iboard], &(BoardInfo[iboard]));
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot get board info. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}
      printf("\nConnected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo[iboard].ModelName, iboard);
      printf("\tROC FPGA Release is %s\n", BoardInfo[iboard].ROC_FirmwareRel);
      printf("\tAMC FPGA Release is %s\n", BoardInfo[iboard].AMC_FirmwareRel);

      ret = CAEN_DGTZ_Reset(dev_handle[iboard]);                                               /* Reset Digitizer */
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot reset the board. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}

      ret = CAEN_DGTZ_GetInfo(dev_handle[iboard], &(BoardInfo[iboard]));
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot get board info. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}


      // =====================================================================================
      // Setup 1024 data buffers (max.)
      // =====================================================================================
      //ret = CAEN_DGTZ_WriteRegister(dev_handle[iboard], 0x800C, 0xA);
      ret = CAEN_DGTZ_WriteRegister(dev_handle[iboard], 0x800C, 0x08);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot configure data buffers. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}


      // =====================================================================================
      // Configure max. number of buffers for BLT readout
      // =====================================================================================
      ret = CAEN_DGTZ_SetMaxNumEventsBLT(dev_handle[iboard],1024);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot configure data buffers for BLT readout. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}

      // =====================================================================================
      // Allocate memory buffer to hold data received from digitizer
      // =====================================================================================
      caen_data_buffer[iboard] = NULL;
      ret = CAEN_DGTZ_MallocReadoutBuffer(dev_handle[iboard], &(caen_data_buffer[iboard]), &(caen_data_buffer_size[iboard]));
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot MallocReadoutBuffer. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}
      printf("Readout buffer size: %i\n",caen_data_buffer_size[iboard]);


      // =====================================================================================
      // Allocate memory buffer to hold all data from digitizer
      // =====================================================================================
      data_buffer_size[iboard] = 0x2000000; // 32 MB
      data_buffer[iboard] = (char*)malloc(data_buffer_size[iboard]);
      if ( !data_buffer[iboard] )
	{
	  cm_msg(MERROR,"v1724_init","Cannot allocate memory for data buffers.\n");
	  return FE_ERR_HW;
	}

      // =====================================================================================
      // Set to use External Clock input
      // - Not possible via software
      // - Set via dipswitch SW3
      // =====================================================================================


#if 0
      // PW
      uint32_t mydata;
      ret = CAEN_DGTZ_ReadRegister(dev_handle[iboard], 0x8100, &mydata);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot read register 0x8100. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}
      printf("Acquisition control register is: %d\n", mydata);
      mydata |= (1<<4);
      ret = CAEN_DGTZ_WriteRegister(dev_handle[iboard], 0x8100, mydata);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot write register 0x8100. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}
      ret = CAEN_DGTZ_ReadRegister(dev_handle[iboard], 0x8100, &mydata);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot read register 0x8100. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}
      printf("2nd time Acquisition control register is: %d\n", mydata);

      ret = CAEN_DGTZ_ReadRegister(dev_handle[iboard], CAEN_DGTZ_DOWNSAMPLE_FACT_ADD, &mydata);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot read register 0x%04x. Error 0x%08x\n",CAEN_DGTZ_DOWNSAMPLE_FACT_ADD, ret);
	  return FE_ERR_HW;
	}
      printf("The down sampling factor currently is: 0x%08x\n", mydata);
      mydata = 10;
      ret = CAEN_DGTZ_WriteRegister(dev_handle[iboard], CAEN_DGTZ_DOWNSAMPLE_FACT_ADD, mydata);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot write register 0x8128. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}
      ret = CAEN_DGTZ_ReadRegister(dev_handle[iboard], CAEN_DGTZ_DOWNSAMPLE_FACT_ADD, &mydata);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","2: Cannot read register 0x%04x. Error 0x%08x\n", CAEN_DGTZ_DOWNSAMPLE_FACT_ADD,ret);
	  return FE_ERR_HW;
	}
      printf("The down sampling factor currently is: 0x%08x\n", mydata);
      // end PW
#endif

    }

  printf("  [done]\n");

  return SUCCESS;
}

void v1724_exit()
{

  CAEN_DGTZ_ErrorCode ret;
  for (int iboard=0; iboard<NBOARDS; iboard++)
    {

      if ( !S_V1724_ODB[iboard].enabled ) continue;

      // =====================================================================================
      // Free memory allocated for data buffer
      // =====================================================================================
      ret = CAEN_DGTZ_FreeReadoutBuffer(&(caen_data_buffer[iboard]));
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_exit","Cannot FreeReadoutBuffer. Error 0x%08x\n",ret);
	}

      // =====================================================================================
      // Close digitizer
      // =====================================================================================
      ret = CAEN_DGTZ_CloseDigitizer(dev_handle[iboard]);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_exit","Cannot CloseDigitizer. Error 0x%08x\n",ret);
	}

      // =====================================================================================
      // Free memory buffers
      // =====================================================================================
      free( data_buffer[iboard] );
    }
}

INT v1724_pre_bor()
{

  CAEN_DGTZ_ErrorCode ret;
  for (int iboard=0; iboard<NBOARDS; iboard++)
    {

      if ( !S_V1724_ODB[iboard].enabled ) continue;

      // ==========================================================================================
      // Reset digitizer
      // ==========================================================================================
      ret = CAEN_DGTZ_Reset(dev_handle[iboard]);                                               /* Reset Digitizer */
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot reset the board. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}

      // =====================================================================================
      // Enable channels
      // =====================================================================================
      uint32_t channel_mask = 0x0; // disable all channels
      for (uint32_t ichan=0; ichan<8; ichan++)
	{
	  if ( S_V1724_ODB[iboard].ch[ichan].enabled )
	    channel_mask |= (1 << ichan); // enable channel ichannel
	}
      printf("channel enable mask: 0x%08x\n",channel_mask);
      ret = CAEN_DGTZ_SetChannelEnableMask(dev_handle[iboard],channel_mask);  // enable all channels
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot Enable channels. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}

      // =====================================================================================
      // Acquisition control
      // =====================================================================================
      //ret = CAEN_DGTZ_SetAcquisitionMode(dev_handle[iboard],CAEN_DGTZ_S_IN_CONTROLLED);
      ret = CAEN_DGTZ_SetAcquisitionMode(dev_handle[iboard], (CAEN_DGTZ_AcqMode_t)S_V1724_ODB[iboard].acquisition_mode);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot configure Acquisition Control. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}



      // =====================================================================================
      // Record length
      // =====================================================================================
      ret = CAEN_DGTZ_SetRecordLength(dev_handle[iboard], S_V1724_ODB[iboard].wf_length);
      //ret = CAEN_DGTZ_SetRecordLength(dev_handle[iboard], 4096);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot SetRecordLength. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}

      // =====================================================================================
      // Self-trigger mode
      // =====================================================================================

      // CAEN_DGTZ_TRGMODE_DISABLED
      uint32_t self_trigger_mask = 0x0; // disable all channels
      for (uint32_t ichan=0; ichan<8; ichan++)
	{
	  if ( S_V1724_ODB[iboard].ch[ichan].self_trigger_mode == CAEN_DGTZ_TRGMODE_DISABLED )
	    self_trigger_mask |= (1 << ichan); // enable channel ichannel
	}
      printf("self-trigger DISABLED mask: 0x%08x\n",channel_mask);
      ret = CAEN_DGTZ_SetChannelSelfTrigger(dev_handle[iboard], CAEN_DGTZ_TRGMODE_ACQ_ONLY, self_trigger_mask);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot SetCahnngelSelfTrigger. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}

      // CAEN_DGTZ_TRGMODE_ACQ_ONLY
      self_trigger_mask = 0x0; // disable all channels
      for (uint32_t ichan=0; ichan<8; ichan++)
	{
	  if ( S_V1724_ODB[iboard].ch[ichan].self_trigger_mode == CAEN_DGTZ_TRGMODE_ACQ_ONLY )
	    self_trigger_mask |= (1 << ichan); // enable channel ichannel
	}
      printf("self-trigger ACQ_ONLY mask: 0x%08x\n",channel_mask);
      ret = CAEN_DGTZ_SetChannelSelfTrigger(dev_handle[iboard], CAEN_DGTZ_TRGMODE_ACQ_ONLY, self_trigger_mask);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot SetCahnngelSelfTrigger. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}


      // CAEN_DGTZ_TRGMODE_EXTOUT_ONLY
      self_trigger_mask = 0x0; // disable all channels
      for (uint32_t ichan=0; ichan<8; ichan++)
	{
	  if ( S_V1724_ODB[iboard].ch[ichan].self_trigger_mode == CAEN_DGTZ_TRGMODE_EXTOUT_ONLY )
	    self_trigger_mask |= (1 << ichan); // enable channel ichannel
	}
      printf("self-trigger EXTOUT_ONLY mask: 0x%08x\n",channel_mask);
      ret = CAEN_DGTZ_SetChannelSelfTrigger(dev_handle[iboard], CAEN_DGTZ_TRGMODE_EXTOUT_ONLY, self_trigger_mask);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot SetCahnngelSelfTrigger. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}


      // CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT
      self_trigger_mask = 0x0; // disable all channels
      for (uint32_t ichan=0; ichan<8; ichan++)
	{
	  if ( S_V1724_ODB[iboard].ch[ichan].self_trigger_mode == CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT )
	    self_trigger_mask |= (1 << ichan); // enable channel ichannel
	}
      printf("self-trigger ACQ_AND_EXTOUT mask: 0x%08x\n",channel_mask);
      ret = CAEN_DGTZ_SetChannelSelfTrigger(dev_handle[iboard], CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT, self_trigger_mask);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot SetCahnngelSelfTrigger. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}


      // =====================================================================================
      // Trigger polarity
      // =====================================================================================
      int trigger_edge = S_V1724_ODB[iboard].trigger_edge;
      if ( trigger_edge == 0 )
	ret = CAEN_DGTZ_SetTriggerPolarity (dev_handle[iboard], 0, CAEN_DGTZ_TriggerOnRisingEdge);
      else
	ret = CAEN_DGTZ_SetTriggerPolarity (dev_handle[iboard], 0, CAEN_DGTZ_TriggerOnFallingEdge);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot SetTriggerPolarity. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}

      // =====================================================================================
      // Trigger threshold
      // =====================================================================================
      for (uint32_t ichan=0; ichan<8; ichan++)
	{
	  ret = CAEN_DGTZ_SetChannelTriggerThreshold(dev_handle[iboard], ichan, S_V1724_ODB[iboard].ch[ichan].trigger_threshold);
	  if ( ret != CAEN_DGTZ_Success )
	    {
	      cm_msg(MERROR,"v1724_init","Cannot SetChannelTriggerThreshold. Error 0x%08x\n",ret);
	      return FE_ERR_HW;
	    }
	}

#if 0
      // =====================================================================================
      // Number of samples over threshold for trigger generation
      // =====================================================================================
      for (uint32_t ichan=0; ichan<8; ichan++)
	{
	  uint32_t addr = V1724_BASE[iboard]+0x1084 + 0x100*ichan;
	  CAEN_DGTZ_WriteRegister(dev_handle[iboard], addr, 1);
	  if ( ret != CAEN_DGTZ_Success )
	    {
	      cm_msg(MERROR,"v1724_init","Cannot Set the number of samples over threshold for trigger. Error 0x%08x\n",ret);
	      return FE_ERR_HW;
	    }
	}
#endif

#if 1
      // =====================================================================================
      // Set post-trigger size
      // =====================================================================================
      ret = CAEN_DGTZ_SetPostTriggerSize(dev_handle[iboard],80);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot SetPostTriggerSize. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}
#endif

      // =====================================================================================
      // DC offset
      // =====================================================================================
      for (uint32_t ichan=0; ichan<8; ichan++)
	{
	  ret = CAEN_DGTZ_SetChannelDCOffset(dev_handle[iboard], ichan, S_V1724_ODB[iboard].ch[ichan].offset);
	  if ( ret != CAEN_DGTZ_Success )
	    {
	      cm_msg(MERROR,"v1724_init","Cannot SetChannelDCOffset. Error 0x%08x\n",ret);
	      return FE_ERR_HW;
	    }
	}

#if 0
      // =====================================================================================
      // Pulse polarity
      // =====================================================================================
      for (uint32_t ichan=0; ichan<8; ichan++)
	{
	  //ret = CAEN_DGTZ_SetChannelPulsePolarity(dev_handle[iboard], ichan, CAEN_DGTZ_PulsePolarityPositive);
	  //ret = CAEN_DGTZ_SetChannelPulsePolarity(dev_handle[iboard], ichan, CAEN_DGTZ_PulsePolarityNegative);
	  ret = CAEN_DGTZ_SetChannelPulsePolarity(dev_handle[iboard], ichan, (CAEN_DGTZ_PulsePolarity_t)S_V1724_ODB[iboard].ch[ichan].polarity);
	  if ( ret != CAEN_DGTZ_Success )
	    {
	      cm_msg(MERROR,"v1724_init","Cannot SetChannelPulsePolarity. Error 0x%08x\n",ret);
	      return FE_ERR_HW;
	    }
	}
#endif

#if 0
      // =====================================================================================
      // Set Zero suppresion parameters
      // =====================================================================================
      for (uint32_t ichan=0; ichan<8; ichan++)
	{
	  ret = CAEN_DGTZ_SetChannelZSParams(dev_handle[iboard], ichan, CAEN_DGTZ_ZS_FINE, 20, 4);
	  if ( ret != CAEN_DGTZ_Success )
	    {
	      cm_msg(MERROR,"v1724_init","Cannot SetChannelZSParams. Error 0x%08x\n",ret);
	      return FE_ERR_HW;
	    }
	}


      // =====================================================================================
      // Zero suppresion mode
      // =====================================================================================
      ret = CAEN_DGTZ_SetZeroSuppressionMode(dev_handle[iboard], CAEN_DGTZ_ZS_ZLE);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot SetZeroSuppressionMode. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}
#endif

      // =====================================================================================
      // Clear digitizer data buffers
      // =====================================================================================
      ret = CAEN_DGTZ_ClearData( dev_handle[iboard] );
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot clear CAEN data buffers. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}

      // =====================================================================================
      // Enable acquisition
      // =====================================================================================
      ret = CAEN_DGTZ_SWStartAcquisition(dev_handle[iboard]);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot CAEN_DGTZ_SWStartAcquisition. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}

      data_size[iboard] = 0;


      // ======================================================================================
      // Channel configuration through register 0x8000=CAEN_DGTZ_BROAD_CH_CTRL_ADD
      // ======================================================================================
      uint32_t data;
      ret = CAEN_DGTZ_ReadRegister(dev_handle[iboard], CAEN_DGTZ_BROAD_CH_CTRL_ADD, &data);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot read from register 0x8000. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}

      // enable trigger overlap
      data |= (1<<1);
      printf("channel configuration register 0x8000: %0x08x\n",data);
      ret = CAEN_DGTZ_WriteRegister(dev_handle[iboard], CAEN_DGTZ_BROAD_CH_CTRL_ADD, data);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot write to register 0x8000. Error 0x%08x\n",ret);
	  return FE_ERR_HW;
	}

    }

  return SUCCESS;
}

INT v1724_eor()
{

  CAEN_DGTZ_ErrorCode ret;
  for (int iboard=0; iboard<NBOARDS; iboard++)
    {

      if ( !S_V1724_ODB[iboard].enabled ) continue;

      // =====================================================================================
      // Disable acquisition
      // =====================================================================================
      ret =  CAEN_DGTZ_SWStopAcquisition(dev_handle[iboard]);
      if ( ret != CAEN_DGTZ_Success )
	{
	  cm_msg(MERROR,"v1724_init","Cannot CAEN_DGTZ_SWStopAcquisition. Error 0x%08x\n",ret);
	}
    }

  return SUCCESS;
}

INT v1724_read(char *pevent)
{
  printf("Read out data from digitizer(s)\n");

  // =====================================================================================
  // Read out remaining data from the digitizer
  // =====================================================================================
  v1724_readout();

  // =====================================================================================
  // Fill MIDAS event
  // =====================================================================================
  char bk_name[80];
  char *pdata;
  for (int iboard=0; iboard<NBOARDS; iboard++)
    {

      if ( !S_V1724_ODB[iboard].enabled ) continue;

      sprintf(bk_name, "CDG%i", iboard);
      bk_create(pevent, bk_name, TID_BYTE, &pdata);
      if ( data_size[iboard] > MAX_EVENT_SIZE/2 )
	{
	  cm_msg(MERROR,"v1724_read","Event size is too large. Truncating data...\n");
	  data_size[iboard] = MAX_EVENT_SIZE/2;
	}
      memcpy(pdata, data_buffer[iboard], data_size[iboard]);
      pdata += data_size[iboard];
      bk_close(pevent, pdata);
      printf("v1724 board %i data size: %i\n",iboard,data_size[iboard]);
      // reset data couner for the next event
      data_size[iboard] = 0;
    }

  return SUCCESS;
}

void v1724_readout()
{

  CAEN_DGTZ_ErrorCode ret;
  for (int iboard=0; iboard<NBOARDS; iboard++) {
    if ( !S_V1724_ODB[iboard].enabled ) continue;

    uint32_t rdata;
    ret = CAEN_DGTZ_ReadRegister(dev_handle[iboard], 0x812C, &rdata);
    //printf("Event Stored: 0x%08x ret = %i\n",rdata, ret);
    if ( ret != CAEN_DGTZ_Success ) {
  	  cm_msg(MERROR,"v1724_readout","Cannot read register 0x812C. Error 0x%08x\n",ret);
  	}

    //ret = CAEN_DGTZ_ReadRegister(dev_handle[iboard], 0x814C, &rdata);
    //printf("Event size: 0x%08x ret = %i\n",rdata, ret);

    if ( rdata > 0 ) {
  	  // =====================================================================================
  	  // Read data
  	  // =====================================================================================
  	  uint32_t caen_data_size;
      ret = CAEN_DGTZ_ReadData(dev_handle[iboard], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, caen_data_buffer[iboard], &caen_data_size);
  	  if ( ret != CAEN_DGTZ_Success ) {
	      cm_msg(MERROR,"v1724_readout","Cannot DGTZ_ReadData. Error 0x%08x\n",ret);
	    }
  	  //printf("data size: %i\n", caen_data_size);

#if 0
	  uint32_t numEvents;
	  CAEN_DGTZ_GetNumEvents(dev_handle[iboard],caen_data_buffer[iboard],caen_data_size,&numEvents);
	  printf("numEvents: %i\n", numEvents);
#endif

  	  if ( data_size[iboard] + caen_data_size < data_buffer_size[iboard] ) {
	      memcpy( (data_buffer[iboard] + data_size[iboard]), caen_data_buffer[iboard], caen_data_size);
	      data_size[iboard] += caen_data_size;
	    }
  	}
  }

  // added by VT for testing
  //ss_sleep(1);
}

INT v1724_poll_live()
{

  v1724_readout();

  return SUCCESS;
}
