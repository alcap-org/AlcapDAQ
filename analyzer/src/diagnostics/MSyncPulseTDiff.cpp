////////////////////////////////////////////////////////////////////////////////
/// \defgroup MSyncPulseTDiff
///
/// \brief
/// Rollover timing diff
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

/* ROOT includes */
#include "TH1D.h"

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MSyncPulseTDiff_init(void);
static INT MSyncPulseTDiff(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
static const int NCRATE = 9;
static TH1* vhSyncPulseAllTDiff[NCRATE+1];
static TH1* vhSyncPulseAvgTDiff[NCRATE+1];


ANA_MODULE MSyncPulseTDiff_module =
{
  "MSyncPulseTDiff",    /* module name           */
  "John R Quirk",       /* author                */
  MSyncPulseTDiff,      /* event routine         */
  NULL,                 /* BOR routine           */
  NULL,                 /* EOR routine           */
  MSyncPulseTDiff_init, /* init routine          */
  NULL,                 /* exit routine          */
  NULL,                 /* parameter structure   */
  0,                    /* structure size        */
  NULL,                 /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MSyncPulseTDiff_init() {
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    char name[64], title[128];

    sprintf(name, "hSyncPulseAllTDiff_%d", icrate);
    sprintf(title, "Sync Pulse All dT Distribution Crate %d;Ti-T(i-1) (Ticks)", icrate);
    vhSyncPulseAllTDiff[icrate] = new TH1D(name, title, 2000000, 0., 2e6);

    sprintf(name, "hSyncPulseAvgTDiff_%d", icrate);
    sprintf(title, "Sync Pulse Avg dT Distribution Crate %d;Tf-Ti (Ticks)", icrate);
    vhSyncPulseAvgTDiff[icrate] = new TH1D(name, title, 2000000, 0., 2e6);
  }

  vhSyncPulseAllTDiff[NCRATE] = new TH1D("hSyncPulseAllTDiff_TDC",
                                         "Sync Pulse All dT Distribution TDC;Ti-T(i-1) (Ticks)",
                                         2000000, 72000000., 74000000.);
  vhSyncPulseAvgTDiff[NCRATE] = new TH1D("hSyncPulseAvgTDiff_TDC",
                                         "Sync Pulse Avg dT Distribution TDC;Tf-Ti (Ticks)",
                                         1000000, 73000000., 74000000.);

  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MSyncPulseTDiff(EVENT_HEADER *pheader, void *pevent) {
  const std::map <std::string, std::vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    char det[16]; sprintf(det, "SyncCrate%d", icrate);
    std::string bank = gSetup->GetBankName(det);
    if (!wfd_map.count(bank)) continue;

    const std::vector<TPulseIsland*>& pulses = wfd_map.at(bank);
    for (int ipulse = 1; ipulse < pulses.size(); ++ipulse)
      vhSyncPulseAllTDiff[icrate]->Fill(pulses[ipulse]->GetTimeStamp() -
                                        pulses[ipulse-1]->GetTimeStamp());
    if (pulses.size() >= 2)
      vhSyncPulseAvgTDiff[icrate]->Fill((pulses.back()->GetTimeStamp() -
                                         pulses.front()->GetTimeStamp()) /
                                        (pulses.size()-1));
  }

  std::string bank = gSetup->GetBankName("TSync");
  if (!tdc_map.count(bank))
    return SUCCESS;

  const std::vector<int64_t>& hits = tdc_map.at(bank);
  for (int ihit = 1; ihit < hits.size(); ++ihit)
    vhSyncPulseAllTDiff[NCRATE]->Fill(hits[ihit]-hits[ihit-1]);
  if (hits.size() >= 2)
    vhSyncPulseAvgTDiff[NCRATE]->Fill((hits.back()-hits.front()) /
                                      (hits.size()-1));

  return SUCCESS;
}

/// @}
