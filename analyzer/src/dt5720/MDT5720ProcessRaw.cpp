////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDT5720ProcessRaw
/// \ingroup process_raw
/// \author Volodya Tishchenko
/// \author Joe Grange
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

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>
#include "TRandom3.h"

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "DT5720DataFormats.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
static INT module_init(void);
static INT module_event_caen(EVENT_HEADER*, void*);
static INT module_event_caen_std(uint32_t*, const int);
static INT module_event_caen_dpp(uint32_t*, const int);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
static vector<string> bank_names;
/// \brief
/// Number of samples to record before a trigger.
/// \details
/// At the moment this is \b hard \b coded into the ::module_init function
/// instead of grabbed from the ODB. The way the CAEN should work is that
/// a percentage of the total sampling window is set to be the "after trigger".
/// However, it does \e not seem like this was set correctly in the front end.
/// \todo
/// Find out the problem and characterize.
static unsigned int nPreSamples;
/// \brief
/// Number of channels in DT5720.
static const int NCHAN = 4;
static BOOL DPP;

ANA_MODULE MDT5720ProcessRaw_module =
{
  "MDT5720ProcessRaw",            /* module name           */
  "Vladimir Tishchenko",         /* author                */
  module_event_caen,                  /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  module_init,                   /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT module_init()
{

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin();
      mapIter != bank_to_detector_map.end(); mapIter++) {

    std::string bankname = mapIter->first;
    if (TSetupData::IsWFD(bankname) && bankname[1] == '5')
      bank_names.push_back(bankname);
  }

  /*** Get necessary data from ODB ***/
  char key[80];
  int size;
  BYTE post_trigger_percentage;
  unsigned int nSamples;


  // Get Trigger Time Tag info
  // Timestamp will be shifted by number of presamples

  // sprintf(key, "/Equipment/Crate 5/Settings/CAEN/Waveform length STD");
  // size = sizeof(nSamples);
  // db_get_value(hDB, 0, key, &nSamples, &size, TID_DWORD, 1);
  // sprintf(key, "/Equipment/Crate 5/Settings/CAEN/Post trigger size");
  // size = sizeof(post_trigger_percentage);
  // db_get_value(hDB, 0, key, &post_trigger_percentage, &size, TID_BYTE, 1);
  // //nPreSamples = (int) (0.01 * ((100 - post_trigger_percentage) * nSamples));
  nPreSamples = 20; // From the Golden Data, it looks like there are 20 presamples.
                    // The frontend does not seem to correctly load post_trigger_size
                    // onto the CAEN.

  sprintf(key, "/Equipment/Crate 5/Settings/CAEN/DPP");
  size = sizeof(DPP);
  db_get_value(hDB, 0, key, &DPP, &size, TID_BOOL, 0);

  return SUCCESS;
}

/*-- module event routine -----------------------------------------*/
INT module_event_caen(EVENT_HEADER *pheader, void *pevent) {
  std::map< std::string, std::vector<TPulseIsland*> >& pulse_islands_map =
    gData->fPulseIslandToChannelMap;

  // Clear out any previous events
  for (std::map< std::string, std::vector<TPulseIsland*> >::iterator iter = pulse_islands_map.begin();
       iter != pulse_islands_map.end(); ++iter) {
    if (iter->first[0] == 'D' && iter->first[1] == '5') {
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

  BYTE* pdata;

  char bank_name[8];
  sprintf(bank_name,"CND%i",0); // one MIDAS bank per board
  int bank_len = bk_locate(pevent, bank_name, &pdata);

  INT ret = SUCCESS;
  if (bank_len > 0) {
    if (DPP)
      ret = module_event_caen_dpp((uint32_t*) pdata, bank_len);
    else
      ret = module_event_caen_std((uint32_t*) pdata, bank_len);
  }

  // print for testing
  if(pheader->serial_number == 1) {
    // Loop through all the banks and print an output (because this ProcessRaw loops through pulses then banks, it has been put here)
    for (std::vector<std::string>::iterator bankNameIter = bank_names.begin();
      	 bankNameIter != bank_names.end(); bankNameIter++) {

      vector<TPulseIsland*>& pulse_islands = pulse_islands_map[*(bankNameIter)];
      printf("TEST MESSAGE: Read %d events from bank %s in event 1\n",
	     pulse_islands.size(),
	     bankNameIter->c_str());
    }
  }

  return ret;
}

INT module_event_caen_std(uint32_t* p32, const int nbytes) {
 std::map< std::string, std::vector<TPulseIsland*> >& pulse_islands_map =
    gData->fPulseIslandToChannelMap;

  const uint32_t *p32_0 = p32;

  while ((p32 - p32_0)*4 < nbytes) {
    uint32_t caen_event_cw = p32[0]>>28;
    //      printf("CW: %08x\n",caen_event_cw);
    if (caen_event_cw != 0xA) {
      printf("***ERROR! Wrong data format it dt5720: incorrect control word 0x%08x\n", caen_event_cw);
      return SUCCESS;
    }

    uint32_t caen_event_size = p32[0] & 0x0FFFFFFF;
    //      printf("caen event size: %i\n",caen_event_size);

    uint32_t caen_channel_mask = p32[1] & 0x000000FF;
    // count the number of channels in the event
    int nchannels = 0;
    for (int ichannel=0; ichannel<NCHAN; ichannel++) {
      if (caen_channel_mask & (1<<ichannel)) nchannels++;
    }
    //      printf("caen channel mask: 0x%08x (%i)\n",caen_channel_mask,nchannels);

    // The trigger time stamps has a period twice that of the ADC sampling clock
    uint32_t caen_trigger_time = 2*p32[3];
    //      printf("caen trigger time: %i\n",caen_trigger_time);// = clock ticks?

    // number of samples per channel
    int nsamples = ((caen_event_size-4)*2) / nchannels;
    //      printf("waveform length: %i\n",nsamples);

    // Loop through the channels (i.e. banks)
    for (std::vector<std::string>::iterator bankNameIter = bank_names.begin();
     bankNameIter != bank_names.end(); bankNameIter++) {

      vector<TPulseIsland*>& pulse_islands = pulse_islands_map[*(bankNameIter)];
      std::vector<int> sample_vector;
      int ichannel = bankNameIter - bank_names.begin();

      //  printf("TGlobalData bank [%s] ----------------------------------------\n", (*bankNameIter).c_str());
      //  printf("Number of TPulseIslands already existing = %d\n", pulse_islands.size());
      //  printf("*-- channel %i -------------------------------------------------------------*\n",ichannel);

      if (caen_channel_mask & (1<<ichannel)) {
        // channel enabled
        int isample = 0;
        int nwords = nsamples/2;

        for (int iword=0; iword<nwords; iword++) {
          for (int isubword=0; isubword<2; isubword++) {
            uint32_t adc;

            if (isubword == 0 )
              adc = (p32[4+iword+ichannel*nwords] & 0x3fff);
            else
              adc = ((p32[4+iword+ichannel*nwords] >> 16) & 0x3fff);

            isample++;

            sample_vector.push_back(adc);
          }
        }
        pulse_islands.push_back(new TPulseIsland(caen_trigger_time - nPreSamples, sample_vector, *bankNameIter));
      }
    }

    // *** updated by VT: align data by 32bit
    p32 += caen_event_size + (caen_event_size%2);
    //printf("offset: %i bank size: %i\n", (int)(p32-p32_0), bank_len);
  }
  return SUCCESS;
}

INT module_event_caen_dpp(uint32_t* p32, const int nbytes) {
 std::map< std::string, std::vector<TPulseIsland*> >& pulses_map =
    gData->fPulseIslandToChannelMap;

  const uint32_t* p32_0 = p32;
  while (4*(p32-p32_0) < nbytes) {
    const uint32_t caen_event_cw = p32[0]>>28;
    if (caen_event_cw != 0xA) {
      printf("***ERROR! Wrong data format it dt5730: incorrect control word 0x%08x\n",
             caen_event_cw);
      return SUCCESS;
    }

    DT5720BoardData board;
    const int nwords = board.Process(p32);
    if (nwords < 0) {
      printf("DT5720: Error processing DPP data!\n");
      return SUCCESS;
    }

    p32 += nwords;
    for (int ich = 0; ich < DT5720BoardData::kNChan; ++ich) {
      if (board.channel_enabled(ich)) {
        char bankname[5];
        sprintf(bankname, "D5%02d", ich);
        std::vector<TPulseIsland*>& pulses = pulses_map[bankname];
        const DT5720ChannelData& channel = board.channel_data(ich);
        for (int ievt = 0; ievt < channel.num_events(); ++ievt)
          pulses.push_back(new TPulseIsland(channel.time_tag(ievt),
                                            channel.waveform(ievt),
                                            bankname));
      }
    }
  }
  return SUCCESS;
}

/// @}
