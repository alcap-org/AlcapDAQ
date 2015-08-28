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
#include "TDirectory.h"

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
static TH1* hMasterPulseAllTDiff;
static TH1* hMasterPulseAvgTDiff;

static std::vector<int> identify_pulses_and_calculate_times(const std::vector<TPulseIsland*>&);

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
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("SyncPulseTDiff/")->cd();

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
                                         2000000, 72000000., 74000000.);

  // These next two only matter for run 7319, where
  // the clock is plugged into TDC Ch. 2 (TMaster)
  hMasterPulseAllTDiff = new TH1D("hMasterPulseAllTDiff_TDC",
                                  "Master Clock All dT Distribution TDC;Ti-T(i-1) (Ticks)",
                                  2000000, 0., 2000000);
  hMasterPulseAvgTDiff = new TH1D("hMasterPulseAvgTDiff_TDC",
                                  "Master Clock Avg dT Distribution TDC;Tf-Ti (Ticks)",
                                  2000000, 0., 2000000);

  cwd->cd();
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

    const std::vector<int> times = identify_pulses_and_calculate_times(wfd_map.at(bank));
    for (int ipulse = 1; ipulse < times.size(); ++ipulse)
      vhSyncPulseAllTDiff[icrate]->Fill(times[ipulse] - times[ipulse-1]);
    if (times.size() > 2)
      vhSyncPulseAvgTDiff[icrate]->Fill((times.back() - times.front())/(double)(times.size()-1));
  }

  std::string bank = gSetup->GetBankName("TSync");
  if (tdc_map.count(bank)) {
    const std::vector<int64_t>& hits = tdc_map.at(bank);
    for (int ihit = 1; ihit < hits.size(); ++ihit)
      vhSyncPulseAllTDiff[NCRATE]->Fill(hits[ihit]-hits[ihit-1]);
    if (hits.size() >= 2)
      vhSyncPulseAvgTDiff[NCRATE]->Fill((hits.back()-hits.front()) /
                                        (hits.size()-1));
  }

  bank = gSetup->GetBankName("TMaster");
  if (tdc_map.count(bank)) {
    const std::vector<int64_t>& hits = tdc_map.at(bank);
    for (int ihit = 1; ihit < hits.size(); ++ihit)
      hMasterPulseAllTDiff->Fill(hits[ihit]-hits[ihit-1]);
    if (hits.size() >= 2)
      hMasterPulseAvgTDiff->Fill((hits.back()-hits.front()) /
                                 (hits.size()-1));
  }

  return SUCCESS;
}

static const int PULSE_HEIGHT_CUT = 500;

std::vector<int> identify_pulses_and_calculate_times(const std::vector<TPulseIsland*>& tpis) {
  std::vector<int> ts;
  ts.reserve(tpis.size());
  for (int i = 0; i < tpis.size(); ++i)
    if (tpis[i]->GetPulseHeight() > PULSE_HEIGHT_CUT)
      ts.push_back(tpis[i]->GetTimeStamp() + tpis[i]->GetPeakSample());
  return ts;
}

/// @}
