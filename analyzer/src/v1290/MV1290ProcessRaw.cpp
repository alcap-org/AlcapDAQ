////////////////////////////////////////////////////////////////////////////////
/// \defgroup MV1290ProcessRaw
/// \ingroup process_raw
///
/// \brief
/// Produces TPIs from raw data read in from BU CAEN.
/// @{
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <map>

/* MIDAS includes */
#include "midas.h"

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "v1290.h"

/*-- Module declaration --------------------------------------------*/
static INT  module_init_tdc(void);
static INT  module_event_tdc(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
static std::vector<std::string> bank_names;

ANA_MODULE MV1290ProcessRaw_module =
{
  "MV1290ProcessRaw",    /* module name           */
  "Vladimir Tishchenko", /* author                */
  module_event_tdc,      /* event routine         */
  NULL,                  /* BOR routine           */
  NULL,                  /* EOR routine           */
  module_init_tdc,       /* init routine          */
  NULL,                  /* exit routine          */
  NULL,                  /* parameter structure   */
  0,                     /* structure size        */
  NULL,                  /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT module_init_tdc() {
  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin();
      mapIter != bank_to_detector_map.end(); mapIter++) {

    const std::string& bankname = mapIter->first;
    if (TSetupData::IsTDC(bankname))
      bank_names.push_back(bankname);
  }
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT module_event_tdc(EVENT_HEADER *pheader, void *pevent) {
  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  std::map< std::string, std::vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
  
  // clear old data
  typedef std::map< std::string, std::vector<int64_t> >::iterator map_iterator;
  for (map_iterator theMapIter = tdc_map.begin(); theMapIter != tdc_map.end(); theMapIter++) 
    {
      std::vector<int64_t> *theHits = &theMapIter->second;
      theHits->clear();
    }



  BYTE *pdata;
  const char *cbk_name = "DCDC";
  unsigned int bank_len = bk_locate(pevent, cbk_name, &pdata);

  if ( bank_len == 0 ) return SUCCESS;

  // data format version
  char dfver[8];
  memcpy(dfver,pdata,4);
  dfver[4] = '\0';
  //printf("bank [%s] data format version: [%s] size: [%i] bytes\n",cbk_name,dfver,bank_len);
  pdata+=4;

  uint32_t *p32 = (uint32_t*)pdata;
  const uint32_t *p32_0 = p32;

  // Added by VT on 06/05/2015
  // Rollover
  const int64_t rollover = 2097152;
  int64_t rollover_counter = 0;
  int64_t t_last = -1;
  int64_t t0 = 0;

  int data_size = (bank_len - 4)/4;
  while ((p32 - p32_0) < data_size) {
    //printf("0x%08x\n",*p32);
    if (V1290_IS_TDC_MEASURE(*p32)) {
      int chn = V1290_GET_TDC_MSR_CHANNEL(*p32);
      int64_t meas = V1290_GET_TDC_MSR_MEASURE(*p32);
      //printf("chn %i meas %i\n",chn,meas);
      //if ( chn == 1 ) printf("chn %i meas %i\n",chn,meas);
      if ( t_last == -1 )
	{
	  t0 = meas;
	}
      else if ( meas < t_last )
	{
	  rollover_counter++;
	  //printf("rollover %i\n",rollover_counter);
	}
      t_last = meas;
      meas = meas - t0 + rollover_counter*rollover;
      char bnk[5];
      sprintf(bnk, "T4%02d", chn);
      tdc_map[bnk].push_back(meas);
    } else {
      printf("V1290 Analyzer: Found non-measurement TDC data! (%08x)\n", *p32);
    }
    ++p32;
  }

  // Print for testing
  if(pheader->serial_number == 1) {
    for (std::vector<std::string>::iterator bankName = bank_names.begin();
         bankName != bank_names.end(); bankName++) {
      std::vector<int64_t>& tdc_meas = tdc_map[*bankName];
      printf("TEST MESSAGE: Read %d events from bank %s in event 1\n",
             tdc_meas.size(), bankName->c_str());
    }
  }

  return SUCCESS;
}

/// @}
