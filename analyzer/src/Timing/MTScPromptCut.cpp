////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTScPromptCut
///
/// \brief
/// Prompt timing cuts for TSc
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
static INT MTScPromptCut(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static const double TIME_BEFORE = 500, TIME_AFTER = 500; // ns
static const std::string filtered_bank_prefix("TScPromptCut_");

ANA_MODULE MTScPromptCut_module =
{
  "MTScPromptCut", /* module name           */
  "John R Quirk",   /* author                */
  MTScPromptCut,   /* event routine         */
  NULL,             /* BOR routine           */
  NULL,             /* EOR routine           */
  NULL,             /* init routine          */
  NULL,             /* exit routine          */
  NULL,             /* parameter structure   */
  0,                /* structure size        */
  NULL,             /* initial parameters    */
};


/*-- module event routine -----------------------------------------*/
INT MTScPromptCut(EVENT_HEADER *pheader, void *pevent) {

  std::map< std::string, std::vector<TPulseIsland*> >& pulses_map =
    gData->fPulseIslandToChannelMap;

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
  if (!pulsess_map.count(mu_bank)) {
    printf("MTScPromptCut: No TSc pulses to cut in bank %s!\n", mu_bank.c_str());
    return SUCCESS;
  }
  const std::vector<int64_t>& mu_pulses = hits_map[mu_bank];
  const double mu_tick = gSetup->GetClockTick(mu_bank);

  std::map< std::string, std::vector<TPulseIsland*> >::const_iterator ipulmap;
  for (ipulmap = pulses_map.begin(); ipulmap != pulses_map.end(); ++ipulmap) {
    if (ipulmap->first.length() != 4) continue;

    const std::string filtered_bank = filtered_bank_prefix + ipulmap->first;
    const std::vector<TPulseIsland*>& pulses = ipulmap->second;
    std::vector<TPulseIsland*>& filtered_pulses = pulses_map[filtered_bank];

    const double pulse_tick = gSetup->GetClockTick(ipulmap->first);
    for (int ipulse = 0, imu = 0; ipulse < pulses.size(); ++ipulse) {
      const double tpulse = pulse_tick * pulses[ipulse]->GetTimeStamp();
      for (; imu < mu_pulses.size(); ++imu) {
        const double tmu = mu_tick * mu_pulses[imu]->GetTimeStamp();
        if(tmu < tpulse - TIME_BEFORE)
          continue;
        else if (tmu < tpulse + TIME_AFTER)
          filtered_pulses.push_back(pulses[ipulse]);
        break;
      }
      if (imu == mu_pulses.size()) break;
    }
  }

  return SUCCESS;
}

/// @}
