////////////////////////////////////////////////////////////////////////////////
/// \defgroup MV1720ProcessRaw
/// \ingroup process_raw
/// \author Volodya Tishchenko
/// \author Joe Grange
///
/// \brief
/// Produces TPIs from raw data read in from UH CAEN.
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

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>
#include "TRandom3.h"

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
static INT  MV1720ProcessRaw_init(void);
static INT  MV1720ProcessRaw_event(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of UH CAEN bank names for the event loop.
static vector<string> bank_names;
/// \brief
/// Number of samples to record before a trigger.
/// \details
/// Number of samples and fraction of sampling window to set
/// aside for after trigger are stored in  ODB; this is calculated
/// from those.
static unsigned int nPreSamples;
/// \brief
/// Number of channels in V1720.
static const int NCHAN = 8;


ANA_MODULE MV1720ProcessRaw_module =
{
  "MV1720ProcessRaw",            /* module name           */
  "Vladimir Tishchenko",         /* author                */
  MV1720ProcessRaw_event,        /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MV1720ProcessRaw_init,         /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MV1720ProcessRaw_init()
{

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin();
      mapIter != bank_to_detector_map.end(); mapIter++) {

    std::string bankname = mapIter->first;

    // We only want the CAEN banks here
    if (TSetupData::IsWFD(bankname) && bankname[1] == '8')
      bank_names.push_back(bankname);
  }

  /*** Get necessary data from ODB ***/
  char key[80];
  int size;
  DWORD post_trigger_percentage, nSamples;

  // Get Trigger Time Tag info
  // Timestamp will be shifted by number of presamples
  sprintf(key, "/Equipment/Crate 8/Settings/CAEN0/Waveform length");
  size = sizeof(nSamples);
  db_get_value(hDB, 0, key, &nSamples, &size, TID_DWORD, 1);
  post_trigger_percentage = 80; // This is hardcoded in the frontend
  nPreSamples = (int) (0.01 * ((100 - post_trigger_percentage) * nSamples));

  return SUCCESS;
}

/*-- module event routine -----------------------------------------*/
INT MV1720ProcessRaw_event(EVENT_HEADER *pheader, void *pevent)
{
  //printf("===================> %s MIDAS event %i\n",__FILE__,pheader->serial_number);

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  std::map< std::string, std::vector<TPulseIsland*> >& pulse_islands_map =
    gData->fPulseIslandToChannelMap;

  // Clear out any previous events
  for (std::map< std::string, std::vector<TPulseIsland*> >::iterator iter = pulse_islands_map.begin();
       iter != pulse_islands_map.end(); ++iter) {
    if (iter->first[0] == 'D' && iter->first[1] == '8') {
      std::vector<TPulseIsland*>& islands = iter->second;
      for (int i = 0; i < islands.size(); ++i) {
        if (islands[i]) {
          delete islands[i];
          islands[i] = NULL;
        }
      }
      islands.clear();
    }
  }

  // Get the event number
  int midas_event_number = pheader->serial_number;

  BYTE *pdata;

  //  printf("In caen ER!\n");

  char bank_name[8];
  sprintf(bank_name,"D8B%i", 0); // one MIDAS bank per board
  unsigned int bank_len = bk_locate(pevent, bank_name, &pdata);


  uint32_t *p32 = (uint32_t*)pdata;
  uint32_t *p32_0 = (uint32_t*)pdata;

  while ((p32 - p32_0)*4 < bank_len ) {

    uint32_t caen_event_cw = p32[0]>>28;
    //printf("CW: %08x\n",caen_event_cw);
    if ( caen_event_cw != 0xA ) {
  	  printf("***ERROR UH CAEN! Wrong data format: incorrect control word 0x%08x\n", caen_event_cw);
  	  return SUCCESS;
  	}

    uint32_t caen_event_size = p32[0] & 0x0FFFFFFF;
    //printf("caen event size: %i\n",caen_event_size);

    uint32_t caen_channel_mask = p32[1] & 0x000000FF;
    // count the number of channels in the event
    int nchannels = 0;
    for (int ichannel=0; ichannel<NCHAN; ichannel++ ) {
  	  if ( caen_channel_mask & (1<<ichannel) ) nchannels++;
  	}

    uint32_t caen_event_counter = p32[2] & 0x00FFFFFF;
    //      printf("caen event counter: %i\n",caen_event_counter);

    //
    uint32_t caen_trigger_time = 2*p32[3];
    //      printf("caen trigger time: %i\n",caen_trigger_time);// = clock ticks?

    // number of samples per channel
    int nsamples = ((caen_event_size-4)*2) / nchannels;
    //      printf("waveform length: %i\n",nsamples);

    // Loop through the channels (i.e. banks)
    for (int ich = 0, iprocchan = 0; ich < NCHAN; ++ich) {
      if (!(caen_channel_mask & 1<<ich)) continue;

    	std::vector<int> sample_vector;
	    int isample = 0;
	    int nwords = nsamples/2;
	    for (int iword=0; iword<nwords; iword++) {
    		for (int isubword=0; isubword<2; isubword++) {
  		    uint32_t adc;
  		    if (isubword == 0)
  		      adc = (p32[4+iword+iprocchan*nwords] & 0x0FFF);
  		    else
  		      adc = ((p32[4+iword+iprocchan*nwords] >> 16) & 0x0FFF);
  		    //printf("CAEN V1720 channel %d: adc[%i] = %i\n", ichannel, isample, adc);
    	    //		    h2_v1720_pulses[ichannel]->Fill(isample,adc);
  		    isample++;
  		    sample_vector.push_back(adc);
        }
      }
      ++iprocchan;

      char bankname[5];
      sprintf(bankname, "D8%02d", ich);
      std::vector<TPulseIsland*>& pulse_islands = pulse_islands_map[bankname];
	    pulse_islands.push_back(new TPulseIsland(caen_trigger_time - nPreSamples,
                                               sample_vector, bankname));
    }

    // align the event by two bytes.
    p32 += caen_event_size + (caen_event_size%2);
    //      printf("offset: %i bank size: %i\n", (int)(p32-p32_0), bank_len);
  }


  // for (int ich = 0; ich < 4; ++ich) {
  //   char bankname[5];
  //   sprintf(bankname, "D8%02d", ich);
  //   printf("V1720 Info: Processed %i pulses in channel %d.\n", pulse_islands_map[bankname].size(), ich);
  // }


  // print for testing
  if(midas_event_number == 1) {
    // Loop through all the banks and print an output (because this ProcessRaw loops through pulses then banks, it has been put here)
    for (std::vector<std::string>::iterator bankNameIter = bank_names.begin();
      	 bankNameIter != bank_names.end(); bankNameIter++) {

      vector<TPulseIsland*>& pulse_islands = pulse_islands_map[*(bankNameIter)];
      printf("TEST MESSAGE: Read %d events from bank %s in event %d\n",
      	     pulse_islands.size(),
      	     bankNameIter->c_str(),
      	     midas_event_number);
    }
  }

  return SUCCESS;
}

/// @}
