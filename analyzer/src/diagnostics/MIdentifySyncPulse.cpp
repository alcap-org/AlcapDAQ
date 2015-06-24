////////////////////////////////////////////////////////////////////////////////
/// \defgroup MIdentifySyncPulse
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
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1D.h"
#include "TH2D.h"

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MIdentifySyncPulse_init(void);
static INT MIdentifySyncPulse(EVENT_HEADER*, void*);
static std::vector<double> calculate_and_sort_times(const std::vector<TPulseIsland*>&, double);
static std::vector<double> calculate_and_sort_times(const std::vector<int64_t>&, double);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
// static TH2* hIdentifySyncPulse;
static TH1* hIdentifySyncPulse;
static TH1* hMaxFractionTScPulsesMatched;
// TESTING HISTOGRAMS
static TH1* hTimingCorrelationGe;
static const double COINC_WINDOW = 100.; // +/- ns


ANA_MODULE MIdentifySyncPulse_module =
{
  "MIdentifySyncPulse",    /* module name           */
  "John R Quirk",          /* author                */
  MIdentifySyncPulse,      /* event routine         */
  NULL,                    /* BOR routine           */
  NULL,                    /* EOR routine           */
  MIdentifySyncPulse_init, /* init routine          */
  NULL,                    /* exit routine          */
  NULL,                    /* parameter structure   */
  0,                       /* structure size        */
  NULL,                    /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MIdentifySyncPulse_init() {
  hIdentifySyncPulse = new TH1D("hIdentifySyncPulse",
                                "Sync Pulse Alignment for TDC (First WFD Sync Pulse);Sync Pulse Index",
                                120, -20., 100.);
  hMaxFractionTScPulsesMatched = new TH1D("hMaxFractionTScPulsesMatched",
                                          "Fraction TSc pulses matched in WFD; Matched/TDC Total",
                                          100, 0., 1.);
  hTimingCorrelationGe = new TH1D("hTimingCorrelationGe", "Ge TCorr;dT (ns)",
                                  20000, 0., 200000.);
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MIdentifySyncPulse(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;

  const double wfd_tick = 2.;    // ns
  const double tdc_tick_scale = 9.76562093498751671e-01;
  const double tdc_tick = tdc_tick_scale * 0.025; // ns

  const std::vector<double> syncwfd = calculate_and_sort_times(wfd_map.at(gSetup->GetBankName("SyncCrate7")), wfd_tick);
  const std::vector<double> synctdc = calculate_and_sort_times(tdc_map.at(gSetup->GetBankName("TSync")), tdc_tick);
  const std::vector<double> tscwfd  = calculate_and_sort_times(wfd_map.at(gSetup->GetBankName("TSc")), wfd_tick);
  const std::vector<double> tsctdc  = calculate_and_sort_times(tdc_map.at(gSetup->GetBankName("TTSc")), tdc_tick);

  const std::vector<double>::const_iterator tscwfd_beg = tscwfd.begin();
  const std::vector<double>::const_iterator tscwfd_end = tscwfd.end();
  int sync = 0, nmax = 0;
  for (int isync = 0; isync < synctdc.size(); ++isync) {
    int n = 0;
    const double t0_wfd = syncwfd.at(0);
    const double t0_tdc = synctdc[isync];
    for (int imu = 0; imu < tsctdc.size(); ++imu) {
      int nmatch = std::upper_bound(tscwfd_beg, tscwfd_end,
                                    tsctdc[imu] - t0_tdc + t0_wfd + COINC_WINDOW) -
                   std::upper_bound(tscwfd_beg, tscwfd_end,
                                    tsctdc[imu] - t0_tdc + t0_wfd - COINC_WINDOW);
      if (nmatch) ++n;
    }
    if (n > nmax) {
      sync = isync;
      nmax = n;
    }
  }
  hIdentifySyncPulse->Fill(sync);
  hMaxFractionTScPulsesMatched->Fill((double)nmax/(double)tsctdc.size());



  std::vector<double> tmp_get = calculate_and_sort_times(tdc_map.at(gSetup->GetBankName("TGeCHT")), tdc_tick);
  std::vector<double> tmp_gee = calculate_and_sort_times(wfd_map.at(gSetup->GetBankName("GeCHEL")), 1./16.1290e6);
  std::vector<double> tmp_syncwfd = calculate_and_sort_times(wfd_map.at(gSetup->GetBankName("SyncCrate4")), 1./16.1290e6);
  for (int i = 0; i < tmp_get.size(); ++i) {
    for (int j = 0; j < tmp_gee.size(); ++j) {
      hTimingCorrelationGe->Fill((tmp_get[i]-synctdc[sync]) - (tmp_gee[j]));
    }
  }

  return SUCCESS;
}

std::vector<double> calculate_and_sort_times(const std::vector<TPulseIsland*>& tpis, double tick) {
  std::vector<double> ts;
  ts.reserve(tpis.size());
  for (int i = 0; i < tpis.size(); ++i)
    ts.push_back(tick*tpis[i]->GetTimeStamp());
  std::sort(ts.begin(), ts.end());
  return ts;
}

std::vector<double> calculate_and_sort_times(const std::vector<int64_t>& hits, double tick) {
  std::vector<double> ts;
  ts.reserve(hits.size());
  for (int i = 0; i < hits.size(); ++i)
    ts.push_back(tick*hits[i]);
  std::sort(ts.begin(), ts.end());
  return ts;
}
/// @}
