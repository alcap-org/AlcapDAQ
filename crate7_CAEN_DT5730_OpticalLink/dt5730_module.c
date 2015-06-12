/**
 * PROGRAM:     dt5730_module.c
 * DESCRIPTION: Control and readout of CAEN DT5730 digitizers.
 * author:      V.Tishchenko
 * date:        20-Mar-2015
 * Based on the readout modules V1724 and DT5720
 *
 *
 * Modifications:
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/io.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "midas.h"

#include "crate.h"
#include "diag.h"


// CAEN includes
#include "CAENDigitizer.h"
#include "CAENDigitizerType.h"
 #include "../CAEN_DesktopDigi_utils/CAENdtDigiUtils.h"

static const int          NCHAN                 = 8;
static int                dev_handle;
static char              *caen_data_buffer      = NULL;         // data buffers used by CAEN
static uint32_t           caen_data_buffer_size = 0;
static char              *data_buffer;                          // data buffers used by MIDAS
static const uint32_t     data_buffer_size      = 32*1024*1024;
static uint32_t           data_size;
//static BOOL            pll_lost = FALSE;   // PLL lost during MIDAS block?
//static BOOL            board_full = FALSE; // Board full at least once during MIDAS block?

static INT  dt5730_init(void);
static void dt5730_exit(void);
static INT  dt5730_bor(void);
static INT  dt5730_eor(void);
static INT  dt5730_poll_live(void);
static INT  dt5730_read(char *pevent); // MIDAS readout routine
static BOOL dt5730_readout(void); // read data from digitizer buffers
static BOOL dt5730_open(void);

static BOOL dt5730_update_digitizer(void);
static BOOL dt5730_update_digitizer_generic(void);
static BOOL dt5730_update_digitizer_std(void);
static BOOL dt5730_update_digitizer_dpp(void);
static BOOL is_caen_error(CAEN_DGTZ_ErrorCode,int, const char*);
static void print_odb_struct(void);

struct readout_module dt5730_module = {
  dt5730_init,             // init
  dt5730_exit,             // exit
  NULL,                    // pre_bor
  dt5730_bor,              // bor
  dt5730_eor,              // eor
  dt5730_poll_live,        // poll_live
  NULL,                    // poll_dead
  NULL,                    // start_block
  NULL,                    // stop_block
  dt5730_read              // read
};

// ======================================================================
// ODB structures
// ======================================================================

typedef struct s_dt5730_odb {
  BOOL      optical_link;
  DWORD     vme_base;
  BYTE      board_num;
  BYTE      link_num;
  BOOL      dpp;
  BOOL      trigger_positive_edge_std;
  DWORD     max_events_per_block_std;
  BOOL      event_aligned_readout;
  char      logic_level[4];
  BOOL      gpi_acquisition_mode;
  BYTE      post_trigger_size_std;
  BOOL      ext_clock;
  DWORD     wf_length_std;
  struct {
    BOOL    enable;
    WORD    offset_std;
    BYTE    baseline_average_dpp;
    INT     polarity_dpp;
    DWORD   wf_length_dpp;
    DWORD   pre_trigger_size_dpp;
    WORD    self_trigger_threshold_std;
    WORD    self_trigger_threshold_dpp;
  } ch[NCHAN];
} S_DT5730_ODB_DEF;

static S_DT5730_ODB_DEF S_DT5730_ODB;

#define S_DT5730_ODB_STR "\
[.]\n\
Optical link = BOOL : y\n\
VME Base = DWORD : 0x00000000\n\
Board number = BYTE : 0\n\
Link number = BYTE : 0\n\
DPP = BOOL : y\n\
Trigger positive edge = BOOL : y\n\
Max events per block STD = DWORD : 1024\n\
Event aligned readout = BOOL : y\n\
Logic level = STRING : [4] NIM\n\
GPI acquisition mode = BOOL : n\n\
Post trigger size = BYTE : 20\n\
External clock = BOOL : n\n\
Waveform length STD = DWORD : 64\n\
\n\
[Ch00]\n\
Enable = BOOL : n\n\
Offset STD = WORD : 0\n\
Baseline average DPP = BYTE : 2\n\
Polarity DPP = INT : 1\n\
Waveform length DPP = DWORD : 60\n\
Pre trigger size DPP = DWORD : 10\n\
Self trigger threshold STD = WORD : 0\n\
Self trigger threshold DPP = WORD : 0\
\n\
[Ch01]\n\
Enable = BOOL : n\n\
Offset STD = WORD : 0\n\
Baseline average DPP = BYTE : 2\n\
Polarity DPP = INT : 1\n\
Waveform length DPP = DWORD : 60\n\
Pre trigger size DPP = DWORD : 10\n\
Self trigger threshold STD = WORD : 0\n\
Self trigger threshold DPP = WORD : 0\
\n\
[Ch02]\n\
Enable = BOOL : n\n\
Offset STD = WORD : 0\n\
Baseline average DPP = BYTE : 2\n\
Polarity DPP = INT : 1\n\
Waveform length DPP = DWORD : 60\n\
Pre trigger size DPP = DWORD : 10\n\
Self trigger threshold STD = WORD : 0\n\
Self trigger threshold DPP = WORD : 0\
\n\
[Ch03]\n\
Enable = BOOL : n\n\
Offset STD = WORD : 0\n\
Baseline average DPP = BYTE : 2\n\
Polarity DPP = INT : 1\n\
Waveform length DPP = DWORD : 60\n\
Pre trigger size DPP = DWORD : 10\n\
Self trigger threshold STD = WORD : 0\n\
Self trigger threshold DPP = WORD : 0\
\n\
[Ch04]\n\
Enable = BOOL : n\n\
Offset STD = WORD : 0\n\
Baseline average DPP = BYTE : 2\n\
Polarity DPP = INT : 1\n\
Waveform length DPP = DWORD : 60\n\
Pre trigger size DPP = DWORD : 10\n\
Self trigger threshold STD = WORD : 0\n\
Self trigger threshold DPP = WORD : 0\
\n\
[Ch05]\n\
Enable = BOOL : n\n\
Offset STD = WORD : 0\n\
Baseline average DPP = BYTE : 2\n\
Polarity DPP = INT : 1\n\
Waveform length DPP = DWORD : 60\n\
Pre trigger size DPP = DWORD : 10\n\
Self trigger threshold STD = WORD : 0\n\
Self trigger threshold DPP = WORD : 0\
\n\
[Ch06]\n\
Enable = BOOL : n\n\
Offset STD = WORD : 0\n\
Baseline average DPP = BYTE : 2\n\
Polarity DPP = INT : 1\n\
Waveform length DPP = DWORD : 60\n\
Pre trigger size DPP = DWORD : 10\n\
Self trigger threshold STD = WORD : 0\n\
Self trigger threshold DPP = WORD : 0\
\n\
[Ch07]\n\
Enable = BOOL : n\n\
Offset STD = WORD : 0\n\
Baseline average DPP = BYTE : 2\n\
Polarity DPP = INT : 1\n\
Waveform length DPP = DWORD : 60\n\
Pre trigger size DPP = DWORD : 10\n\
Self trigger threshold STD = WORD : 0\n\
Self trigger threshold DPP = WORD : 0\
"

extern HNDLE hDB;

INT dt5730_init() {
  printf("Setting up DB and memory for CAEN DT5730...\n");

  /* Link to Database */
  char str[1024];
  sprintf(str, "/Equipment/Crate %i/Settings/CAEN", crate_number);
  int status = db_check_record(hDB,0, str, S_DT5730_ODB_STR, FALSE);
  if(status==DB_NO_KEY || status==DB_STRUCT_MISMATCH) {
    status = db_create_record(hDB, 0, str, S_DT5730_ODB_STR);
    printf("Creating key with status: %d \n",status);
  } else if (status==DB_INVALID_HANDLE) {
    printf("Error: Cannot connect to database to setup CAEN DT5730.\n");
  }
  HNDLE hKey;
  status = db_find_key(hDB, 0, str, &hKey);
  status = db_open_record(hDB, hKey, &S_DT5730_ODB, sizeof(S_DT5730_ODB),
                          MODE_READ, NULL, NULL);
  printf("Status of connecting to DB: %d\n", status);

  data_buffer = (char*) malloc(data_buffer_size);

  return status;
}

BOOL dt5730_open() {
  printf("Opening CAEN DT5730...\n");

  CAEN_DGTZ_ErrorCode ret;

  /* Grab Board */
  if (S_DT5730_ODB.optical_link)
    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_OpticalLink, S_DT5730_ODB.link_num,
                                  S_DT5730_ODB.board_num, S_DT5730_ODB.vme_base,
                                  &dev_handle);
  else
    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, S_DT5730_ODB.board_num, 0, 0,
                                  &dev_handle);
  if (is_caen_error(ret, __LINE__, "dt5730_open")) return false;

  /* Get Board Info */
  CAEN_DGTZ_BoardInfo_t BoardInfo;
  ret = CAEN_DGTZ_GetInfo(dev_handle, &BoardInfo);
  if (is_caen_error(ret, __LINE__, "dt5730_open")) return false;
  printf("\nConnected to CAEN Desktop Digitizer Model %s\n", BoardInfo.ModelName);
  printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
  printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);

  /* Reset Digitizer */
  ret = CAEN_DGTZ_Reset(dev_handle);
  if(is_caen_error(ret, __LINE__-1, "dt5720_init")) return false;

  return true;
}

void dt5730_exit() {
  free(data_buffer);
}

INT dt5730_bor() {
  CAEN_DGTZ_ErrorCode ret;

  dt5730_open();

  /* Setup Digitizer */
  if(!dt5730_update_digitizer()) return FE_ERR_HW;

  /* Allocate space for readout from digitizer */
  ret = CAEN_DGTZ_MallocReadoutBuffer(dev_handle, &caen_data_buffer,
                                      &caen_data_buffer_size);
  if(is_caen_error(ret,__LINE__-1,"dt5730_bor")) return FE_ERR_HW;
  printf("Allocated %i bytes for CAEN data buffer\n", caen_data_buffer_size);

  data_size = 0;

  // The SW Acq bit must be set/unset even for external gate control (S_IN via GPI)
  ret = CAEN_DGTZ_SWStartAcquisition(dev_handle);
  if(is_caen_error(ret, __LINE__, "dt5730_bor")) return FE_ERR_HW;

  ss_sleep(1000);

  return SUCCESS;
}

INT dt5730_eor() {
  CAEN_DGTZ_ErrorCode ret;

  // The SW Acq bit must be set/unset even for external gate control
  // (S_IN via GPI)
  ret = CAEN_DGTZ_SWStopAcquisition(dev_handle);
  if(is_caen_error(ret, __LINE__, "dt5730_eor")) return FE_ERR_HW;

  ret = CAEN_DGTZ_FreeReadoutBuffer(&caen_data_buffer);
  if(is_caen_error(ret, __LINE__, "dt5730_eor")) return FE_ERR_HW;
  caen_data_buffer = NULL;

  ret = CAEN_DGTZ_CloseDigitizer(dev_handle);
  if(is_caen_error(ret,__LINE__-1,"dt5720_eor")) return FE_ERR_HW;

  return SUCCESS;
}

INT dt5730_read(char *pevent) {
  // Check if board full or loss of PLL lock
  // Must do before readout to get more accurate board full status.
  caen_digi_status ds = caen_digi_get_status(dev_handle);

#if 0
  // Added by VT
  // Problem: stale data in onboard memories at the end of each MIDAS block
  // Restart acquisition between each MIDAS block
  CAEN_DGTZ_ErrorCode ret;
  ret = CAEN_DGTZ_SWStopAcquisition(dev_handle);
  is_caen_error(ret, __LINE__, "dt5730_read");
#endif

  // ===========================================================================
  // Read out remaining data from the digitizer
  // ===========================================================================
  dt5730_readout();

  // Flush on-board memory buffers and read remaning data
  CAEN_DGTZ_ErrorCode ret;
  ret = CAEN_DGTZ_WriteRegister(dev_handle, 0x803C, 0x1);
  is_caen_error(ret, __LINE__, "dt5730_read");
  printf("Reading remaning data\n");
  dt5730_readout();


  // ===========================================================================
  // Fill MIDAS event
  // ===========================================================================
  bk_init32(pevent);
  char bk_name[5];
  char *pdata;

  sprintf(bk_name, "CND1");
  bk_create(pevent, bk_name, TID_BYTE, &pdata);
  if (data_size > MAX_EVENT_SIZE) {
    cm_msg(MERROR, "dt5730_read","Event size is too large. Truncating data...\n");
    data_size = MAX_EVENT_SIZE;
  }
  memcpy(pdata, data_buffer, data_size);
  pdata += data_size;
  bk_close(pevent, pdata);
  printf("data size: %i bytes\n",data_size);
  data_size = 0;

  // ===========================================================================
  // Get status information
  // ===========================================================================
  sprintf(bk_name, "CNS1");
  bk_create(pevent, bk_name, TID_BYTE, &pdata);
  memcpy(pdata++, &(ds.pll_lost), sizeof(ds.pll_lost));
  memcpy(pdata++, &(ds.board_full), sizeof(ds.board_full));
  memcpy(pdata++, &(ds.internal_comm_to), sizeof(ds.internal_comm_to));
  memcpy(pdata++, &(ds.over_temperature), sizeof(ds.over_temperature));
  memcpy(pdata++, &(ds.no_power), sizeof(ds.no_power));
  bk_close(pevent, pdata);

#if 0
  // Added by VT
  // Problem: stale data in onboard memories at the end of each MIDAS block
  // Restart acquisition between each MIDAS block
  ret = CAEN_DGTZ_SWStartAcquisition(dev_handle);
  is_caen_error(ret, __LINE__, "dt5730_read");
#endif


  return SUCCESS;
}

BOOL dt5730_readout() {
  CAEN_DGTZ_ErrorCode ret;
  uint32_t caen_data_size;

  /* Read out data if there is any */
  ret = CAEN_DGTZ_ReadData(dev_handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
                           caen_data_buffer, &caen_data_size);
  if (is_caen_error(ret, __LINE__, "dt5730_readout")) return FALSE;

#if 0
  // moved by VT to dt5730_read()
  // Check if board full or loss of PLL lock
  caen_digi_status ds = caen_digi_get_status(dev_handle);
  if (ds.pll_lost)
    pll_lost = TRUE;
  if (ds.board_full)
    board_full = TRUE;
#endif

  /* If there's data, copy from digitizers local buffer to different local buffer */
  if(caen_data_size > 0) {
    //printf("data size: %i\n", caen_data_size);
    if(data_size+caen_data_size < data_buffer_size ) {
      memcpy((data_buffer+data_size), caen_data_buffer, caen_data_size);
      data_size += caen_data_size;
    }
  }
  return TRUE;
}

INT dt5730_poll_live() {
  if(!dt5730_readout()) return FE_ERR_HW;
  return SUCCESS;
}

BOOL dt5730_update_digitizer() {
  if (!dt5730_update_digitizer_generic())
    return false;
  //then update the digi parameters specific to firmwares
  if (S_DT5730_ODB.dpp)
    return dt5730_update_digitizer_dpp();
  else
    return dt5730_update_digitizer_std();
}

BOOL dt5730_update_digitizer_generic() {
  CAEN_DGTZ_ErrorCode ret;

  if (S_DT5730_ODB.ext_clock) {
    printf("Using external clock\n");
    uint32_t data;
    ret = CAEN_DGTZ_ReadRegister(dev_handle, CAEN_DGTZ_ACQ_CONTROL_ADD, &data);
    if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_generic")) return false;
    data |= 1<<6;
    ret = CAEN_DGTZ_WriteRegister(dev_handle, CAEN_DGTZ_ACQ_CONTROL_ADD, data);
    if (is_caen_error(ret,__LINE__-1,"dt5730_update_digitizer_generic")) return false;
  } else {
    printf("Using internal clock\n");
    uint32_t data;
    ret = CAEN_DGTZ_ReadRegister(dev_handle, CAEN_DGTZ_ACQ_CONTROL_ADD, &data);
    if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_generic")) return false;
    data &= ~(1<<6);
    ret = CAEN_DGTZ_WriteRegister(dev_handle, CAEN_DGTZ_ACQ_CONTROL_ADD, data);
    if (is_caen_error(ret,__LINE__-1,"dt5730_update_digitizer_generic")) return false;
  }

  ret = CAEN_DGTZ_SetSWTriggerMode(dev_handle, CAEN_DGTZ_TRGMODE_DISABLED);
  if (is_caen_error(ret,__LINE__,"dt5730_update_digitizer_generic")) return false;
  ret = CAEN_DGTZ_SetExtTriggerInputMode(dev_handle,CAEN_DGTZ_TRGMODE_DISABLED);
  if (is_caen_error(ret,__LINE__,"dt5730_update_digitizer_generic")) return false;

  if (strcmp(S_DT5730_ODB.logic_level, "NIM") == 0) {
    ret = CAEN_DGTZ_SetIOLevel(dev_handle, CAEN_DGTZ_IOLevel_NIM);
  } else if (strcmp(S_DT5730_ODB.logic_level, "TTL") == 0) {
    ret = CAEN_DGTZ_SetIOLevel(dev_handle, CAEN_DGTZ_IOLevel_TTL);
  } else {
    ret = CAEN_DGTZ_SetIOLevel(dev_handle, CAEN_DGTZ_IOLevel_NIM);
    cm_msg(MINFO,"dt5730_update_digitizer_generic","Invalid logic level. Set to NIM.");
  }
  if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_generic")) return false;

  if (S_DT5730_ODB.gpi_acquisition_mode) {
    ret = CAEN_DGTZ_SetAcquisitionMode(dev_handle,CAEN_DGTZ_S_IN_CONTROLLED);
    cm_msg(MINFO, "dt5730_update_digitizer_generic", "Using S_IN_CONTROLLED mode");
  } else {
    ret = CAEN_DGTZ_SetAcquisitionMode(dev_handle,CAEN_DGTZ_SW_CONTROLLED);
    cm_msg(MINFO, "dt5730_update_digitizer_generic", "Using SW_CONTROLLED mode");
  }
  if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_generic")) return false;

  uint32_t channel_mask = 0;
  for (int ch = 0; ch < NCHAN; ++ch)
    if (S_DT5730_ODB.ch[ch].enable)
      channel_mask |= (1 << ch);
  ret = CAEN_DGTZ_SetChannelEnableMask(dev_handle, channel_mask);
  if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_generic")) return false;

  for (int ch = 0; ch < NCHAN; ++ch) {
    ret = CAEN_DGTZ_SetChannelDCOffset(dev_handle, ch, S_DT5730_ODB.ch[ch].offset_std);
    if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_generic")) return false;
  }


  // ===========================================================================
  // Channel configuration set bit 0x8004=CAEN_DGTZ_BROAD_CH_CONFIGBIT_SET_ADD
  // ===========================================================================
  const int allow_trigger_overlap_bit = 1<<1;
  ret = CAEN_DGTZ_WriteRegister(dev_handle, CAEN_DGTZ_BROAD_CH_CONFIGBIT_SET_ADD,
				allow_trigger_overlap_bit);
  if(is_caen_error(ret, __LINE__-1, "dt5730_update_digitizer_generic")) return false;

  return true;
}

BOOL dt5730_update_digitizer_std() {
  CAEN_DGTZ_ErrorCode ret;

  const int channel_mask = 0xFF;
  ret = CAEN_DGTZ_SetChannelSelfTrigger(dev_handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY,
                                        channel_mask);
  if(is_caen_error(ret, __LINE__, "dt5730_update_digitizer_std")) return false;

  if(S_DT5730_ODB.max_events_per_block_std >= 2 &&
     S_DT5730_ODB.max_events_per_block_std <= 1024) {
    ret = CAEN_DGTZ_SetMaxNumEventsBLT(dev_handle,S_DT5730_ODB.max_events_per_block_std);
  } else {
    ret = CAEN_DGTZ_SetMaxNumEventsBLT(dev_handle, 1024);
    cm_msg(MINFO, "dt5730_update_digitizer_std","Invalid max block size. Set to 1024.");
  }
  if (is_caen_error(ret,__LINE__,"dt5730_update_digitizer_std")) return false;

  // SetRecordLength MUST be called before SetPostTriggerSize
  ret = CAEN_DGTZ_SetRecordLength(dev_handle, S_DT5730_ODB.wf_length_std);
  if (is_caen_error(ret,__LINE__,"dt5730_update_digitizer_std")) return false;

  if (S_DT5730_ODB.post_trigger_size_std >= 0 &&
      S_DT5730_ODB.post_trigger_size_std <=100) {
    ret = CAEN_DGTZ_SetPostTriggerSize(dev_handle,S_DT5730_ODB.post_trigger_size_std);
  } else {
    ret = CAEN_DGTZ_SetPostTriggerSize(dev_handle,80);
    cm_msg(MINFO, "dt5730_update_digitizer_std",
	   "Invalid post trigger size. Set to 80 percent.");
  }
  if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_std")) return false;

  // According to correspondence with CAEN,
  // second argument (channel nuumber) is ignored.
  // But just in case...
  if (S_DT5730_ODB.trigger_positive_edge_std) {
    for (int ch = 0; ch < NCHAN; ++ch) {
      ret = CAEN_DGTZ_SetTriggerPolarity(dev_handle, ch, CAEN_DGTZ_TriggerOnRisingEdge);
      if (is_caen_error(ret,__LINE__,"dt5730_update_digitizer_std")) return false;
    }
  } else {
    for (int ch = 0; ch < NCHAN; ++ch) {
      ret = CAEN_DGTZ_SetTriggerPolarity(dev_handle, ch, CAEN_DGTZ_TriggerOnFallingEdge);
      if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_std")) return false;
    }
  }

  for (int ch = 0; ch < NCHAN; ++ch) {
    ret = CAEN_DGTZ_SetChannelTriggerThreshold(dev_handle, ch, S_DT5730_ODB.ch[ch].self_trigger_threshold_std);
    if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_std")) return false;
  }

  return true;
}

BOOL dt5730_update_digitizer_dpp() {
  CAEN_DGTZ_ErrorCode ret;
  CAEN_DGTZ_DPP_PSD_Params_t DPPParams = {0};

  uint32_t channel_mask = 0xFFFF;

  // DPP parameters
  for(int ch=0; ch < NCHAN; ++ch) {
    DPPParams.thr[ch] = S_DT5730_ODB.ch[ch].self_trigger_threshold_dpp;
    // The following parameter is used to specifiy the number of samples for the baseline averaging:
    // 0: Fixed, 1: 16samp, 2: 64samp, 3: 256samp, 4: 1024samp
    // Not sure how correct this is, check DPP user manual
    DPPParams.nsbl[ch] = S_DT5730_ODB.ch[ch].baseline_average_dpp;
    DPPParams.trgc[ch] = CAEN_DGTZ_DPP_TriggerConfig_Threshold; // Trigger on threshold
    DPPParams.selft[ch] = 1;                                    // Self trigger enable

    // The below are for features unused by us.
    DPPParams.lgate[ch] = 32;    // Long Gate Width (N*4ns)
    DPPParams.sgate[ch] = 24;    // Short Gate Width (N*4ns)
    DPPParams.pgate[ch] = 8;     // Pre Gate Width (N*4ns)
    DPPParams.csens[ch] = 0;
    DPPParams.tvaw[ch] = 50;
  }

  DPPParams.purh = CAEN_DGTZ_DPP_PSD_PUR_DetectOnly; //NOTE: Ignored on 5730
  DPPParams.purgap = 100;  // Pile-up Rejection Gap  //NOTE: Ignored on 5730
  DPPParams.blthr = 3;     // Baseline Threshold     //This parameter is deprecated
  DPPParams.bltmo = 100;   // Baseline Timeout       //This parameter is deprecated
  DPPParams.trgho = 0;     // Trigger HoldOff

  // Set the DPP specific parameters for the channels in the given channelMask
  ret = CAEN_DGTZ_SetDPPParameters(dev_handle, channel_mask, &DPPParams);
  if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_dpp")) return false;

  ret = CAEN_DGTZ_SetDPPAcquisitionMode(dev_handle, CAEN_DGTZ_DPP_ACQ_MODE_Mixed, CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime);
  if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_dpp")) return false;

  // For DT5730, can only set even channels.
  // Setting applies for even/odd couples.
  for (int ch = 0; ch < NCHAN; ch += 2)
    ret = CAEN_DGTZ_SetRecordLength(dev_handle, S_DT5730_ODB.ch[ch].wf_length_dpp, ch);
  if (is_caen_error(ret,__LINE__,"dt5730_update_digitizer_dpp")) return false;

  for (int ch = 0; ch < NCHAN; ++ch) {
    // Set the Pre-Trigger size (in samples)
    ret = CAEN_DGTZ_SetDPPPreTriggerSize(dev_handle, ch, S_DT5730_ODB.ch[ch].pre_trigger_size_dpp);
    if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_dpp")) return false;

    // Set the polarity for the given channel (CAEN_DGTZ_PulsePolarityPositive or CAEN_DGTZ_PulsePolarityNegative)
    if (S_DT5730_ODB.ch[ch].polarity_dpp >= 0)
      ret = CAEN_DGTZ_SetChannelPulsePolarity(dev_handle, ch, CAEN_DGTZ_PulsePolarityPositive);
    else
      ret = CAEN_DGTZ_SetChannelPulsePolarity(dev_handle, ch, CAEN_DGTZ_PulsePolarityNegative);
    if (is_caen_error(ret, __LINE__, "dt5730_update_digitizer_dpp")) return false;
  }

  // Set how many events to accumulate in the board memory before
  // being available for readout
  // Since automatic, should be set after many other parameters I would think?
  ret = CAEN_DGTZ_SetDPPEventAggregation(dev_handle, 0, 0);
  if(is_caen_error(ret, __LINE__, "dt5730_update_digitizer_dpp")) return false;

  // Need to figure out what to do here
  //ret = CAEN_DGTZ_SetDPP_PSD_VirtualProbe(dev_handle, CAEN_DGTZ_DPP_VIRTUALPROBE_SINGLE, CAEN_DGTZ_DPP_PSD_VIRTUALPROBE_Baseline, CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_GateLong, CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_OverThr);
  //if(is_caen_error(ret, __LINE__, "dt5730_update_digitizer_dpp")) return false;

  return true;
}


 BOOL is_caen_error(CAEN_DGTZ_ErrorCode e, int l, const char* r) {
  /*
    l: line number
    f: file name
    r: parent routine
  */

  static char* f = __FILE__;

  if (e == CAEN_DGTZ_Success)
    return FALSE;

  --l;
  switch(e) {
  case CAEN_DGTZ_CommError:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Communication error.");
    break;
  case CAEN_DGTZ_GenericError:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Generic (unspecified) error.");
    break;
  case CAEN_DGTZ_InvalidParam:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Invalid parameter error.");
    break;
  case CAEN_DGTZ_InvalidLinkType:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Invalid LinkType.");
    break;
  case CAEN_DGTZ_InvalidHandle:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Invalid handle.");
    break;
  case CAEN_DGTZ_MaxDevicesError:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Too many devices.");
    break;
  case CAEN_DGTZ_BadBoardType:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Operation not allowed on this model.");
    break;
  case CAEN_DGTZ_BadInterruptLev:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Invalid interrupt level.");
    break;
  case CAEN_DGTZ_BadEventNumber:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Invalid event number.");
    break;
  case CAEN_DGTZ_ReadDeviceRegisterFail:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Unable to read from registry.");
    break;
  case CAEN_DGTZ_WriteDeviceRegisterFail:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Unable to write to registry.");
    break;
  case CAEN_DGTZ_InvalidChannelNumber:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Invalid channel number.");
    break;
  case CAEN_DGTZ_ChannelBusy:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Channel is busy.");
    break;
  case CAEN_DGTZ_FPIOModeInvalid:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Invalid FPIO (?) mode.");
    break;
  case CAEN_DGTZ_WrongAcqMode:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Invalid acquisition mode.");
    break;
  case CAEN_DGTZ_FunctionNotAllowed:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Module does not support this function.");
    break;
  case CAEN_DGTZ_Timeout:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Communication timed out.");
    break;
  case CAEN_DGTZ_InvalidBuffer:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Invalid buffer (readout or event).");
    break;
  case CAEN_DGTZ_EventNotFound:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Unable to find event.");
    break;
  case CAEN_DGTZ_InvalidEvent:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Event is invalid.");
    break;
  case CAEN_DGTZ_OutOfMemory:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Memory has run out.");
    break;
  case CAEN_DGTZ_CalibrationError:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Calibration failed.");
    break;
  case CAEN_DGTZ_DigitizerNotFound:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Unable to locate digitizer.");
    break;
  case CAEN_DGTZ_DigitizerAlreadyOpen:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Digitizer is already open.");
    break;
  case CAEN_DGTZ_DigitizerNotReady:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Digitizer not ready.");
    break;
  case CAEN_DGTZ_InterruptNotConfigured:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Interrupts not configured.");
    break;
  case CAEN_DGTZ_DigitizerMemoryCorrupted:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Flash memory corrupted.");
    break;
  case CAEN_DGTZ_DPPFirmwareNotSupported:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Firmware not supported by these libraries.");
    break;
  case CAEN_DGTZ_NotYetImplemented:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Function not yet implemented.");
    break;
  default:
    cm_msg(MT_ERROR,f,l,r,"CAEN DT5730 Unknown error.");
    break;
  }
  return TRUE;
}

void print_odb_struct() {

  printf("Optical link: %d\n", S_DT5730_ODB.optical_link);
  printf("VME Base: %d\n", S_DT5730_ODB.vme_base);
  printf("Board number: %d\n", S_DT5730_ODB.board_num);
  printf("Link number: %d\n", S_DT5730_ODB.link_num);
  printf("DPP: %d\n", S_DT5730_ODB.dpp);
  printf("Trig. pos. edge: %d\n", S_DT5730_ODB.trigger_positive_edge_std);
  printf("Max events/block: %d\n", S_DT5730_ODB.max_events_per_block_std);
  printf("Event aligned readout: %d\n", S_DT5730_ODB.event_aligned_readout);
  printf("Logic level: %s\n", S_DT5730_ODB.logic_level);
  printf("GPI acq. mode: %d\n", S_DT5730_ODB.gpi_acquisition_mode);
  printf("Post trigger size: %d\n", S_DT5730_ODB.post_trigger_size_std);
  printf("Ext. clock: %d\n", S_DT5730_ODB.ext_clock);
  printf("Record length: %d\n", S_DT5730_ODB.wf_length_std);
  for (int ch = 0; ch < NCHAN; ++ch) {
    printf("  Ch%02d--\n", ch);
    printf("  Enable: %d\n", S_DT5730_ODB.ch[ch].enable);
    printf("  Offset: %d\n", S_DT5730_ODB.ch[ch].offset_std);
    printf("  Baseline avg.: %d\n", S_DT5730_ODB.ch[ch].baseline_average_dpp);
    printf("  Polarity DPP: %d\n", S_DT5730_ODB.ch[ch].polarity_dpp);
    printf("  Record length DPP: %d\n", S_DT5730_ODB.ch[ch].wf_length_dpp);
    printf("  Pre-trig. size: %d\n", S_DT5730_ODB.ch[ch].pre_trigger_size_dpp);
    printf("  STD trig. thresh.: %d\n", S_DT5730_ODB.ch[ch].self_trigger_threshold_std);
    printf("  DPP trig. thresh.: %d\n", S_DT5730_ODB.ch[ch].self_trigger_threshold_dpp);
  }

}
