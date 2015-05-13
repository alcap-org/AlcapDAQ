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
#include <utility>
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

#include "v1290.h"

using std::string;
using std::map;
using std::vector;

/*-- Module declaration --------------------------------------------*/
static INT  module_init_tdc(void);
static INT  module_event_tdc(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
static vector<string> bank_names;

ANA_MODULE MV1290ProcessRaw_module =
{
  "MV1290ProcessRaw",            /* module name           */
  "Vladimir Tishchenko",         /* author                */
  module_event_tdc,              /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  module_init_tdc,               /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT module_init_tdc()
{
  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 
    
    std::string bankname = mapIter->first;
    // We only want the CAEN TDC banks here
    if (bankname[0] == 'T')
      bank_names.push_back(bankname);
  }
  return SUCCESS;
}

/*-- module event routine -----------------------------------------*/
INT module_event_tdc(EVENT_HEADER *pheader, void *pevent)
{
  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  std::map<std::string, std::vector<long> >& tdc_map =
    gData->fTDCHitsToChannelMap;
    
  // Get the event number
  int midas_event_number = pheader->serial_number;
  BYTE *pdata;
  unsigned int bank_len = bk_locate(pevent, "TDC", &pdata);
  uint32_t *p32 = (uint32_t*)pdata;
  uint32_t *p32_0 = p32;
  
  while ( (p32 - p32_0)*4 < bank_len )
    {
      if (V1290_IS_TDC_MEASURE((*p32)))
	{
	  int chn = V1290_GET_TDC_MSR_CHANNEL(*p32);
	  long meas = V1290_GET_TDC_MSR_MEASURE(*p32);
	  char bnk[5];
	  sprintf(bnk, "T4%02d", chn);
	  tdc_map[bnk].push_back(meas);
	}
      else
	{
	  printf("V1290 Analyzer: Found non-measurement TDC data! (%08x)\n", *p32);
	}
      ++p32;
    }

  // print for testing
  if(midas_event_number == 1) {
    // Loop through all the banks and print an output (because this ProcessRaw loops through pulses then banks, it has been put here)
    for (std::vector<std::string>::iterator bankNameIter = bank_names.begin();
	 bankNameIter != bank_names.end(); bankNameIter++) {
      
      vector<long>& tdc_meas = tdc_map[*(bankNameIter)];
      printf("TEST MESSAGE: Read %d events from bank %s in event %d\n",
	     tdc_meas.size(),
	     (*(bankNameIter)).c_str(),
	     midas_event_number);
    }
  }

  return SUCCESS;
}

/// @}
