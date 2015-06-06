////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDT5730ProcessRaw
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

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "DT5730DataFormats.h"

/*-- Module declaration --------------------------------------------*/
static INT  module_init(void);
static INT  module_event_caen(EVENT_HEADER*, void*);
static INT  module_event_caen_std(uint32_t* data, const int nbytes);
static INT  module_event_caen_dpp(uint32_t* data, const int nbytes);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
static std::vector<std::string> bank_names;
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
/// Number of channels in DT5730.
static const int NCHAN = 8;
/// \brief
/// Switch to process either DPP or STD event.
static BOOL DPP;


ANA_MODULE MDT5730ProcessRaw_module =
{
  "MDT5730ProcessRaw",           /* module name           */
  "Vladimir Tishchenko",         /* author                */
  module_event_caen,             /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  module_init,                   /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT module_init() {

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin();
      mapIter != bank_to_detector_map.end(); mapIter++) {

    std::string bankname = mapIter->first;
    if (TSetupData::IsWFD(bankname) && bankname[1] == '7')
      bank_names.push_back(bankname);
  }

  /*** Get necessary data from ODB ***/
  char key[80];
  int  size;

  // Timestamp will be shifted by number of presamples for STD firmware
  // Timestamp is CFD for DPP firmware, so we will not use this in that case.
  // sprintf(key, "/Equipment/Crate 7/Settings/CAEN/Waveform length");
  // BYTE   post_trigger_percentage;
  // DWORD  nSamples;
  // size = sizeof(nSamples);
  // db_get_value(hDB, 0, key, &nSamples, &size, TID_DWORD, 0);
  // sprintf(key, "/Equipment/Crate 7/Settings/CAEN/Post trigger size");
  // size = sizeof(post_trigger_percentage);
  // db_get_value(hDB, 0, key, &post_trigger_percentage, &size, TID_BYTE, 0);
  // nPreSamples = (int) (0.01 * ((100 - post_trigger_percentage) * nSamples));
  nPreSamples = 20; // From the Golden Data, it looks like there are 20 presamples.
                    // The frontend does not seem to correctly load post_trigger_size
                    // onto the CAEN.

  sprintf(key, "/Equipment/Crate 7/Settings/CAEN/DPP");
  size = sizeof(DPP);
  db_get_value(hDB, 0, key, &DPP, &size, TID_BOOL, 0);

  return SUCCESS;
}

/*-- module event routine -----------------------------------------*/
INT module_event_caen(EVENT_HEADER *pheader, void *pevent) {
  std::map< std::string, std::vector<TPulseIsland*> >& pulse_islands_map =
    gData->fPulseIslandToChannelMap;

  // Clear out any previous events
  for (std::vector<std::string>::iterator iter = bank_names.begin();
       iter != bank_names.end(); ++iter) {
    std::vector<TPulseIsland*>& islands = pulse_islands_map[*iter];
    for (unsigned int i = 0; i < islands.size(); ++i) {
      if (islands[i]) {
	delete islands[i];
	islands[i] = NULL;
      }
      islands.clear();
    }
  }

  BYTE* pdata;

  char bank_name[8];
  sprintf(bank_name,"CND%i",1); // one MIDAS bank per board
  int bank_len = bk_locate(pevent, bank_name, &pdata);

  INT ret = SUCCESS;
  if (bank_len > 0) {
    if (DPP)
      ret = module_event_caen_dpp((uint32_t*)pdata, bank_len);
    else
      ret = module_event_caen_std((uint32_t*)pdata, bank_len);
  }

  // print for testing
  if(pheader->serial_number == 1)
    // Loop through all the banks and print an output (because this ProcessRaw loops through pulses then banks, it has been put here)
    for (std::vector<std::string>::iterator bankNameIter = bank_names.begin();
         bankNameIter != bank_names.end(); bankNameIter++)
      printf("TEST MESSAGE: Read %d events from bank %s in event 1\n",
             (int)pulse_islands_map[*bankNameIter].size(),
             (bankNameIter->c_str()));

  return ret;
}

INT module_event_caen_std(uint32_t* p32, const int nbytes) {
  std::map< std::string, std::vector<TPulseIsland*> >& pulses_map =
    gData->fPulseIslandToChannelMap;

  const uint32_t* p32_0 = p32;

  while (4*(p32 - p32_0) < nbytes) {
    uint32_t caen_event_cw = p32[0]>>28;
    if ( caen_event_cw != 0xA ) {
    	printf("***ERROR! Wrong data format it dt5730: incorrect control word 0x%08x\n",
             caen_event_cw);
     	return SUCCESS;
    }
    const uint32_t caen_event_size = p32[0] & 0x0FFFFFFF;
    const uint32_t caen_channel_mask = p32[1] & 0x000000FF;
    int nchannels = 0;
    for (int ichannel=0; ichannel<NCHAN; ++ichannel)
    	if (caen_channel_mask & (1<<ichannel))
        nchannels++;

    // The trigger time tag has an LSB resolution of 8ns. This is 4 times the
    // sampling time of 2ns, hence the factor of 4 and increase to uint64_t.
    const uint64_t caen_trigger_time = 4*p32[3];

    // The CAEN event is 4 header words, then waveform words.
    // Each word has 2 14-bit samples, and each enabled channel
    // has the same number of samples as the waveform length
    // is uniform and all channels trigger together.
    const uint32_t caen_header_size = 4;
    const int nwords = (caen_event_size-caen_header_size) / nchannels;

    p32 += caen_header_size;
    for (std::vector<std::string>::iterator bankNameIter = bank_names.begin();
         bankNameIter != bank_names.end(); ++bankNameIter) {
      std::vector<TPulseIsland*>& pulse_islands = pulses_map[*bankNameIter];
      std::vector<int> sample_vector;
      const int ichannel = bankNameIter - bank_names.begin();

      if (caen_channel_mask & (1<<ichannel)) {
        for (int iword=0; iword<nwords; ++iword, ++p32) {
          for (int isample=0; isample<2; ++isample) {
            uint32_t adc = ((*p32) >> 16*isample) & 0x3fff;
            sample_vector.push_back(adc);
          }
        }
        pulse_islands.push_back(new TPulseIsland(caen_trigger_time - nPreSamples, sample_vector, *bankNameIter));
      }
    }
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

    DT5730BoardData board;
    const int nwords = board.Process(p32);
    if (nwords < 0) {
      printf("DT5730: Error processing DPP data!\n");
      return SUCCESS;
    }

    p32 += nwords;
    for (int ich = 0; ich < DT5730BoardData::kNChan; ++ich) {
      if (board.channel_enabled(ich)) {
        char bankname[5];
        sprintf(bankname, "D7%02d", ich);
        std::vector<TPulseIsland*>& pulses = pulses_map[bankname];
        const DT5730ChannelData& channel = board.channel_data(ich);
        for (int ievt = 0; ievt < channel.num_events(); ++ievt) {
          pulses.push_back(new TPulseIsland(channel.time_tag(ievt),
                                            channel.waveform(ievt),
                                            bankname));
        }
      }
    }
  }
  return SUCCESS;
}
/// @}
