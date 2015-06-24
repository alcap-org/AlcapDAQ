////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTTScPromptCut
///
/// \brief
/// Prompt timing cuts for TTSc
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


/*-- Module declaration --------------------------------------------*/
static INT MTTScPromptCut(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static const double TIME_BEFORE = 500, TIME_AFTER = 500; // ns
static const std::string filtered_bank_prefix("TScPromptCut_");

ANA_MODULE MTTScPromptCut_module =
{
  "MTTScPromptCut", /* module name           */
  "John R Quirk",   /* author                */
  MTTScPromptCut,   /* event routine         */
  NULL,             /* BOR routine           */
  NULL,             /* EOR routine           */
  NULL,             /* init routine          */
  NULL,             /* exit routine          */
  NULL,             /* parameter structure   */
  0,                /* structure size        */
  NULL,             /* initial parameters    */
};


/*-- module event routine -----------------------------------------*/
INT MTTScPromptCut(EVENT_HEADER *pheader, void *pevent) {

  std::map< std::string, std::vector<TPulseIsland*> >& pulses_map =
    gData->fPulseIslandToChannelMap;
  std::map< std::string, std::vector<int64_t> >& hits_map =
    gData->fTDCHitsToChannelMap;

  // Clear all bank this module creates.
  // Do not delete TPulseIslands since those are taken care of in
  // the ProcessRaws take care of that.
  std::map< std::string, std::vector<TPulseIsland*> >::iterator ipulses;
  for (std::map< std::string, std::vector<TPulseIsland*> >::iterator ipulses =
         pulses_map.begin();
       ipulses != pulses_map.end(); ++ipulses) {
    if (ipulses->first.length() < 4) continue;
    if (ipulses->first.substr(0, filtered_bank_prefix.length()) ==
        filtered_bank_prefix)
      ipulses->second.clear();
  }

  const std::string mu_bank = gSetup->GetBankName("TTSc");
  if (!hits_map.count(mu_bank)) {
    printf("MGePromptCut: No TTSc hits to cut in bank %s!\n", mu_bank.c_str());
    return SUCCESS;
  }
  const std::vector<int64_t>& mu_hits = hits_map[mu_bank];

  std::map< std::string, std::vector<TPulseIsland*> >::const_iterator ipulmap;
  for (ipulmap = pulses_map.begin(); ipulmap != pulses_map.end(); ++ipulmap) {
    if (ipulmap->first.length() != 4) continue;

    const std::string filtered_bank = filtered_bank_prefix + ipulmap->first;
    const std::vector<TPulseIsland*>& pulses = ipulmap->second;
    std::vector<TPulseIsland*>& filtered_pulses = pulses_map[filtered_bank];

    for (int ipulse = 0, imu = 0; ipulse < pulses.size(); ++ipulse) {
      const double tpulse = pulses[ipulse]->GetPulseTime();
      for (; imu < mu_hits.size(); ++imu) {
        static const double tdc_tick = 0.025; // ns
        const double tmu = tdc_tick * mu_hits[imu];
        if(tmu < tpulse - TIME_BEFORE)
          continue;
        else if (tmu < tpulse + TIME_AFTER)
          filtered_pulses.push_back(pulses[ipulse]);
        break;
      }
      if (imu == mu_hits.size())
        filtered_pulses.push_back(pulses[ipulse]);
    }
  }

  return SUCCESS;
}

/// @}
