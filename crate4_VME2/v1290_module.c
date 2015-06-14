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
#include "CAENComm.h"

//
#include "../CAEN_DesktopDigi_utils/CAENdtDigiUtils.h"

#include "v1290.h"


static char          *data_buffer, *data_buffer_0; // data buffers used by MIDAS
const static uint32_t data_buffer_size = 32*1024*1024;
static uint32_t       data_size = 0; // bytes

static INT v1290_init();
static void v1290_exit();
static INT v1290_pre_bor();
static INT v1290_poll_live();
static INT v1290_read(char *pevent); // MIDAS readout routine
static BOOL v1290_readout();

static BOOL v1290_update_tdc();

struct readout_module v1290_module = {
  v1290_init,      // init
  v1290_exit,      // exit
  v1290_pre_bor,   // pre_bor
  NULL,            // bor
  NULL,            // eor
  v1290_poll_live, // poll_live
  NULL,            // poll_dead
  NULL,            // start_block
  NULL,            // stop_block
  v1290_read       // read
};

// ======================================================================
// ODB structures
// ======================================================================

//#define DEBUG
//#undef DEBUG

extern HNDLE hDB;

static int dev_handle = -1;
static void v1290_SetupTrigger(int handle);
static void v1290_SetupTDC(int handle);
static void v1290_SetupReadout(int handle);


typedef struct s_v1290_odb{
  DWORD vme_base;
  BYTE  board_num;                    ///< Board number in daisychain
  BYTE  link_num;                     ///< Link number in A3818 card
  BOOL  leading_edge_detection;
  BYTE  edge_resolution;
  BYTE  dead_time_between_hits;
  BOOL  enable_channel[16];
} S_V1290_ODB_DEF;

static S_V1290_ODB_DEF S_V1290_ODB;

#define S_V1290_ODB_STR "\
[.]\n\
VME base = DWORD : 0xEE000000\n\
Board number = BYTE : 0\n\
Link number = BYTE : 0\n\
Leading edge detection = BOOL : y\n\
Edge resolution = BYTE : 3\n\
Dead time Between hits = BYTE : 0\n\
Enable Ch0 = BOOL : n\n\
Enable Ch1 = BOOL : n\n\
Enable Ch2 = BOOL : n\n\
Enable Ch3 = BOOL : n\n\
Enable Ch4 = BOOL : n\n\
Enable Ch5 = BOOL : n\n\
Enable Ch6 = BOOL : n\n\
Enable Ch7 = BOOL : n\n\
Enable Ch8 = BOOL : n\n\
Enable Ch9 = BOOL : n\n\
Enable Ch10 = BOOL : n\n\
Enable Ch11 = BOOL : n\n\
Enable Ch12 = BOOL : n\n\
Enable Ch13 = BOOL : n\n\
Enable Ch14 = BOOL : n\n\
Enable Ch15 = BOOL : n\n\
"


/*-- Frontend Init -------------------------------------------------*/
INT v1290_init()
{
  /* Link to Database */
  HNDLE hKey;
  char str[1024];
  sprintf(str, "/Equipment/Crate %i/Settings/TDC", crate_number);
  int status = db_check_record(hDB, 0, str, S_V1290_ODB_STR, FALSE);
  if(status==DB_NO_KEY || status==DB_STRUCT_MISMATCH)
    {
      status = db_create_record(hDB, 0, str, S_V1290_ODB_STR);
      printf("Creating key with status: %d \n", status);
    }
  else if (status==DB_INVALID_HANDLE)
    {
      printf("Error: Cannot connect to database to setup CAEN V1290.");
    }
  status = db_find_key(hDB, 0, str, &hKey);
  status = db_open_record(hDB, hKey, &S_V1290_ODB, sizeof(S_V1290_ODB), MODE_READ, NULL, NULL);

  CAENComm_ErrorCode ret;
  CAENComm_ConnectionType linkType = CAENComm_OpticalLink;

  ret = CAENComm_OpenDevice(linkType,
			    S_V1290_ODB.link_num,
			    S_V1290_ODB.board_num,
			    S_V1290_ODB.vme_base,
			    &dev_handle);
  printf("CAENComm open: %d\n", ret);
  printf("Resetting module.\n");
  v1290_ModuleReset(dev_handle);
  ss_sleep(1000);

  uint16_t v1290_FWRevision =  v1290_FWRev( dev_handle );
  uint16_t rev_minor =  v1290_FWRevision & 0xF;
  uint16_t rev_major =  (v1290_FWRevision >> 4 ) & 0xF;
  printf("v1290 Firmware version: %i.%i\n",rev_minor,rev_major);

  printf("Setting continuous mode.\n");
  v1290_ContinuosSet(dev_handle);
  ss_sleep(1000);

  printf("Enabling BERR terminated readout.\n");
  v1290_EnableBusError(dev_handle, true);
  ss_sleep(1000);


  /* v1290_SetupTDC(dev_handle); */
  /* ss_sleep(1000); */

  /* v1290_SetupTrigger(dev_handle); */
  /* ss_sleep(1000); */

  /* v1290_ReadStatusRegister(dev_handle); */
  /* ss_sleep(5*1000); */

  data_buffer_0 = data_buffer = (char*) malloc(data_buffer_size);

  return SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/
void v1290_exit()
{
  CAENComm_CloseDevice(dev_handle);
  return;
}

INT v1290_pre_bor()
{
  if(!v1290_update_tdc()) return FE_ERR_HW;
  v1290_SoftClear(dev_handle);
  data_size = 0;
  data_buffer = data_buffer_0;
  return SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/
INT v1290_poll_live()
{
  if (!v1290_readout()) {
    printf("Failed to readout V1290 in poll_live!\n");
    return FE_ERR_HW;
  }
  return SUCCESS;
}


/*-- Event readout -------------------------------------------------*/
double get_time()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

#include <stdint.h>
int v1290_read(char *pevent)
{
  if(!v1290_readout())
    return FE_ERR_HW;

  //bk_init32(pevent);
  char* pdata;

  bk_create(pevent, "DCDC", TID_BYTE, &pdata);
  if ( data_size > MAX_EVENT_SIZE )
    {
      cm_msg(MERROR,"v1290_read","Event size is too large. Truncating data...\n");
      data_size = MAX_EVENT_SIZE;
    }

  memcpy(pdata, "V1.0", 4);
  pdata += 4;

  memcpy(pdata, data_buffer_0, data_size);
  pdata += data_size;
  bk_close(pevent, pdata);
  data_size = 0;
  data_buffer = data_buffer_0;

  return SUCCESS;
}

BOOL v1290_readout()
{
  const int maxreads = 100;
  int nreads = 0;
  int nwords32_request = 2048;
  //printf("v1290_readout() \n");
  while (v1290_IsDataReady(dev_handle))
    {
      int nwords32 = v1290_ReadEventBLT(dev_handle, (uint32_t*)data_buffer,nwords32_request);
      //printf("readout: %i 32-bit-words read out\n",nwords32);
      data_buffer += 4*nwords32;
      data_size   += 4*nwords32;
      if (++nreads >= maxreads )
	{
	  //cm_msg(MINFO,
	  //	 frontend_name,
	  //	 "V1290: Read 100 times, leaving readout loop.");
	  break;
	}

      if ( nwords32 < nwords32_request ) break;

      ss_sleep(1);

    }

  return TRUE;
}

BOOL v1290_update_tdc()
{
  // Measure leading edge (true) or falling edge.
  printf("V1290: Setting edge detection.\n");
  if (S_V1290_ODB.leading_edge_detection)
    v1290_SetEdgeDetection(dev_handle, (uint16_t)2);
  else
    v1290_SetEdgeDetection(dev_handle, (uint16_t)1);
  ss_sleep(1000);

  printf("V1290: Setting edge resolution.\n");
  if (S_V1290_ODB.edge_resolution >= 0 &&
      S_V1290_ODB.edge_resolution <= 3)
    v1290_SetEdgeResolution(dev_handle, (uint16_t)S_V1290_ODB.edge_resolution);
  else
    {
      cm_msg(MINFO,
	     frontend_name,
	     "V1290: Invalid edge resolution, setting to 25 ps.");
      v1290_SetEdgeResolution(dev_handle, (uint16_t)3);
    }
  ss_sleep(1000);

  // Set pileup dead time between hits.
  printf("V1290: Setting dead time.\n");
  if (S_V1290_ODB.dead_time_between_hits >= 0 &&
      S_V1290_ODB.dead_time_between_hits <= 3)
    v1290_SetDeadtime(dev_handle,
		      (uint16_t)S_V1290_ODB.dead_time_between_hits);
  else
    {
      cm_msg(MINFO,
	     frontend_name,
	     "V1290: Invalid pileup rejection, setting to 5 ns.");
      v1290_SetDeadtime(dev_handle, (uint16_t)0);
    }
  ss_sleep(1000);

  // Enable/Disable channels
  printf("V1290: Setting channel enables.\n");
  uint32_t chn_en_mask = 0;
  for (int ich = 0; ich < NCHAN; ++ich)
    if (S_V1290_ODB.enable_channel[ich])
      chn_en_mask += 1 << ich;
  v1290_WriteEnablePattern(dev_handle, chn_en_mask);
  ss_sleep(1000);

  return TRUE;
}

void v1290_SetupTrigger(int handle)
{
  printf("Setting acquisition mode for V1290N, be patience ...");
  v1290_TriggerMatchingSet(handle);
  printf(".");
  ss_sleep(1000);
  v1290_SetWindowWidth(handle, 20000);
  printf(".");
  ss_sleep(1000);
  v1290_SetWindowOffset(handle, -20000);
  printf(".");
  ss_sleep(1000);
  v1290_EnableTriggerSubtraction(handle, true);
  printf(".");
  ss_sleep(1000);
  printf("\n");
  v1290_AcqModeRead(handle);
}
void v1290_SetupTDC(int handle)
{
  printf("Setting TDC ...\n");
  v1290_SetEdgeDetection(handle, 0x1);
  ss_sleep(1000);
}

void v1290_SetupReadout(int handle)
{
  v1290_EnableHeader(handle, false);
  ss_sleep(1000);
  printf("header enable %d\n", v1290_HeaderIsEnabled(handle));
  ss_sleep(1000);
  printf("max hits %d\n", v1290_ReadMaxHits(handle));
  ss_sleep(1000);
  v1290_SetFIFOSize(handle, 0x6);
  ss_sleep(1000);
  printf("fifo size %d\n", v1290_ReadFIFOSize(handle));
  ss_sleep(1000);
  v1290_DisableAllChannels(handle);
  v1290_WriteEnablePattern(handle, 0x5);
  v1290_EnableChannel(handle, 0x0, false);
  ss_sleep(1000);
  printf("enable pattern %x\n", v1290_ReadEnablePattern(handle));
  v1290_EnableChannel(handle, 0, true);
}
