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
#include <numeric>
#include <limits>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1D.h"
#include "TH2D.h"
#include "TDirectory.h"

//JG: added alcap includes
/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"

/*-- Module declaration --------------------------------------------*/
extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
using std::string;
using std::vector;
using std::map;

static INT MIdentifySyncPulse_init(void);
static INT MIdentifySyncPulse(EVENT_HEADER*, void*);
static vector<double> calculate_and_sort_times(const vector<TPulseIsland*>&, double);
static vector<double> calculate_and_sort_times(const vector<int64_t>&, double);

namespace {
  TH1* vvhIdentifySyncPulse[NCRATE][10];
  TH1* vvhNumMatches[NCRATE][10];
  TH1* vvhIdentifySyncPulseJut[NCRATE][10];
  const double COINC_WINDOW = 50000.; // +/- ns
  string WFD_CORRESPONDING_TDC_CHAN[NCRATE][10];
  string WFD_BANK_NAME[NCRATE][10];
  string WFD_SYNC_CHAN[NCRATE];
  string TDC_SYNC_CHAN;
}

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
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("IdentifySyncPulse/")->cd();

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      char bank[5], name[64], det[32], title[128];
      sprintf(bank, "D%d%02d", icrate, ich);
      WFD_BANK_NAME[icrate][ich] = bank;
      sprintf(det, gSetup->GetDetectorName(bank).c_str());
      
      sprintf(name, "hIdentifySyncPulse_%s", bank);
      sprintf(title,
	      "Sync Pulse Alignment for %s (First WFD Sync Pulse);Sync Pulse Index",
	      det);
      vvhIdentifySyncPulse[icrate][ich] = new TH1D(name, title, 100, 0., 100.);

      sprintf(name, "hNumMatches_%s", bank);
      sprintf(title,
	      "Num %s pulses matched in WFD;Sync Pulse Index;Number Matches",
	      det);
      vvhNumMatches[icrate][ich] = new TH1D(name, title, 100, 0., 100.);

      sprintf(name, "hIdentifySyncPulseJut_%s", bank);
      sprintf(title, "Factor %s pulses matched over asynchronous;Factor", det);
      vvhIdentifySyncPulseJut[icrate][ich] = new TH1D(name, title, 1e4, 0, 1e4);

      // Record which TDC channel is matched with which WFD channel
      if (strncmp("Ge", det, 2) == 0)
	WFD_CORRESPONDING_TDC_CHAN[icrate][ich] = gSetup->GetBankName("TGeCHT");
      else
	WFD_CORRESPONDING_TDC_CHAN[icrate][ich] =
	  gSetup->GetBankName("T"+std::string(det));
    }
    WFD_SYNC_CHAN[icrate] = gSetup->GetBankName(std::string("SyncCrate") +
						std::string(1, '0'+icrate));
  }

  TDC_SYNC_CHAN = gSetup->GetBankName("TSync");

  cwd->cd();
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MIdentifySyncPulse(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;
  gData->fTDCSynchronizationPulseIndex.clear();
  gData->fTDCSynchronizationPulseOffset.clear();


  const std::vector<double> synctdc = calculate_and_sort_times(tdc_map.at(TDC_SYNC_CHAN), TICKTDC);
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    gData->fTDCSynchronizationPulseIndex.push_back(-1);
    gData->fTDCSynchronizationPulseOffset.push_back(std::numeric_limits<double>::min());

    if (!wfd_map.count(WFD_SYNC_CHAN[icrate]))
      continue;
    const std::vector<double> syncwfd = calculate_and_sort_times(wfd_map.at(WFD_SYNC_CHAN[icrate]), TICKWFD[icrate]);
    if (syncwfd.empty())
      continue;

    std::set<int> matchedsyncs;
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      const std::string& wfd_bank = WFD_BANK_NAME[icrate][ich];
      const std::string& tdc_bank = WFD_CORRESPONDING_TDC_CHAN[icrate][ich];
      if (!wfd_map.count(wfd_bank) || !tdc_map.count(tdc_bank))
	continue;
      const std::vector<double> wfd = calculate_and_sort_times(wfd_map.at(wfd_bank), TICKWFD[icrate]);
      const std::vector<double> tdc = calculate_and_sort_times(tdc_map.at(tdc_bank), TICKTDC);

      std::vector<int> nmatchespersync(synctdc.size(), 0);
      for (int isync = 0; isync < synctdc.size(); ++isync) {
        const double t0_wfd = syncwfd[0];
        const double t0_tdc = synctdc[isync];
        const double tshift = t0_tdc - t0_wfd;
        const std::vector<double>::const_iterator wfd_beg = wfd.begin();
        const std::vector<double>::const_iterator wfd_end = wfd.end();
        int n = 0;
        for (int itdc = 0; itdc < tdc.size(); ++itdc) {
          bool ismatch = std::upper_bound(wfd_beg, wfd_end,
                                          tdc[itdc] - tshift + COINC_WINDOW) -
                         std::upper_bound(wfd_beg, wfd_end,
                                          tdc[itdc] - tshift - COINC_WINDOW);
          if (ismatch) ++n;
        }
        nmatchespersync[isync] = n;
      }

      const int max_sync = std::max_element(nmatchespersync.begin(), nmatchespersync.end()) -
                           nmatchespersync.begin();
      vvhIdentifySyncPulse[icrate][ich]->Fill(max_sync);
      matchedsyncs.insert(max_sync);

      // for (int i = 0; i < 20; ++i) {
      // 	printf("Cr%d Ch%d Sync%d: %d\n", icrate, ich, i, nmatchespersync[i]); 
      // 	vvhNumMatches[icrate][ich]->Fill(i, nmatchespersync[i]);
      // }

      // Trying to determine a metric for aligning
      const int nmax = nmatchespersync[max_sync];
      if (nmatchespersync.size() == 1) {
        vvhIdentifySyncPulseJut[icrate][ich]->Fill(nmax);
        continue;
      }
      const double tot = std::accumulate(nmatchespersync.begin(), nmatchespersync.end(), 0);
      const double avg = (tot - nmax) / (double)(nmatchespersync.size() - 1);
      if (avg != 0.)
        vvhIdentifySyncPulseJut[icrate][ich]->Fill((nmax/avg)*(nmax/tot));
    }

    // If all active channels agree on which TDC pulse aligns,
    // record in TGD.
    if (matchedsyncs.size() == 1) {
      const int i = *matchedsyncs.begin();
      gData->fTDCSynchronizationPulseIndex.back() = i;
      gData->fTDCSynchronizationPulseOffset.back() = synctdc[i] - syncwfd[0];
    }

  }

  return SUCCESS;
}

static const int PULSE_HEIGHT_CUT = 500;

std::vector<double> calculate_and_sort_times(const std::vector<TPulseIsland*>& tpis, double tick) {
  std::vector<double> ts;
  ts.reserve(tpis.size());
  for (int i = 0; i < tpis.size(); ++i)
    if (tpis[i]->GetPulseHeight() > PULSE_HEIGHT_CUT)
      ts.push_back(tick*(tpis[i]->GetTimeStamp() + tpis[i]->GetPeakSample()));
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
