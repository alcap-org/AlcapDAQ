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

static const int NCRATE = 10;
static const int NCHANWFD[NCRATE] = { 0, 0, 0, 0, 8,
                                      4, 0, 8, 8, 0 };
static const double TICKWFD[NCRATE] = { 0., 0., 0., 0.,        62.0019800848,
                                        4., 0., 2.00010727671, 4., 0.}; //ns
// static const double TICKWFD[NCRATE] = { 0., 0., 0., 0., 1.e3/16.129,
//                                         4., 0., 2., 4., 0.}; //ns
static const double TICKTDC = 0.0244153170119; //ns;
// static const double TICKTDC = 0.0244140523374687946;

static TH1* vvhIdentifySyncPulse[NCRATE][10];
static TH1* vvhNumMatches[NCRATE][10];
static TH1* vvhIdentifySyncPulseJut[NCRATE][10];
static const double COINC_WINDOW = 50000.; // +/- ns
// Measure of goodness
// TDiff as function of time in MIDAS block
// Fraction matched as function of time in MIDAS block

static std::string WFD_CORRESPONDING_TDC_CHAN[NCRATE][10];
static std::string WFD_BANK_NAME[NCRATE][10];
static std::string WFD_SYNC_CHAN[NCRATE];
static std::string TDC_SYNC_CHAN;

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

  // static int ev = 0;
  // printf("Event %d\n", ev++);
  // printf("WFD\n");
  // for (std::map< std::string, std::vector<TPulseIsland*> >::const_iterator it = wfd_map.begin(); it != wfd_map.end(); ++it)
  //   printf("\t%s (%s):\t%d pulses\n", gSetup->GetDetectorName(it->first).c_str(), it->first.c_str(), it->second.size());
  // printf("TDC\n");
  // for (std::map< std::string, std::vector<int64_t> >::const_iterator it = tdc_map.begin(); it != tdc_map.end(); ++it)
  //   printf("\t%s (%s):\t%d pulses\n", gSetup->GetDetectorName(it->first).c_str(), it->first.c_str(), it->second.size());


  const std::vector<double> synctdc = calculate_and_sort_times(tdc_map.at(TDC_SYNC_CHAN), TICKTDC);
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    gData->fTDCSynchronizationPulseIndex.push_back(-1);
    gData->fTDCSynchronizationPulseOffset.push_back(std::numeric_limits<double>::min());

    if (!wfd_map.count(WFD_SYNC_CHAN[icrate]))
      continue;
    const std::vector<double> syncwfd = calculate_and_sort_times(wfd_map.at(WFD_SYNC_CHAN[icrate]), TICKWFD[icrate]);
    if (syncwfd.empty())
      continue;

    int ch_match[NCHANWFD[icrate]];
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      ch_match[ich] = -1;
      const std::string& wfd_bank = WFD_BANK_NAME[icrate][ich];
      const std::string& tdc_bank = WFD_CORRESPONDING_TDC_CHAN[icrate][ich];
      if (!wfd_map.count(wfd_bank) || !tdc_map.count(tdc_bank))
	continue;
      const std::vector<double> wfd = calculate_and_sort_times(wfd_map.at(wfd_bank), TICKWFD[icrate]);
      const std::vector<double> tdc = calculate_and_sort_times(tdc_map.at(tdc_bank), TICKTDC);

      std::vector<int> nmatches(synctdc.size(), 0);
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
        nmatches[isync] = n;
      }

      const int max_sync = std::max_element(nmatches.begin(), nmatches.end()) -
                           nmatches.begin();
      vvhIdentifySyncPulse[icrate][ich]->Fill(max_sync);
      ch_match[ich] = max_sync;
      const int nmax = nmatches[max_sync];
      if (nmatches.size() == 1) {
        vvhIdentifySyncPulseJut[icrate][ich]->Fill(nmax);
        continue;
      }
      const double tot = std::accumulate(nmatches.begin(), nmatches.end(), 0);
      const double avg = (tot - nmax) / (double)(nmatches.size() - 1);
      if (avg != 0.)
        vvhIdentifySyncPulseJut[icrate][ich]->Fill((nmax/avg)*(nmax/tot));
    }

    if (std::unique(ch_match, ch_match+NCHANWFD[icrate]) - ch_match == 2) {
      const int i = *std::upper_bound(ch_match, ch_match+2, -1);
      gData->fTDCSynchronizationPulseIndex.back() = i;
      gData->fTDCSynchronizationPulseOffset.back() = synctdc[i] - syncwfd[0];
    }

  }

    // Get all the times so we can later easily loop through them
    // std::vector< std::vector<double> > wfd, tdc;
    // wfd.reserve(NCHANWFD[icrate]);
    // tdc.reserve(NCHANWFD[icrate]);
    // for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
    //   const std::string& wfd_bank = WFD_BANK_NAME[icrate][ich];
    //   const std::string& tdc_bank = WFD_CORRESPONDING_TDC_CHAN[icrate][ich];
    //   if (wfd_map.count(wfd_bank) && tdc_map.count(tdc_bank)) {
    // 	wfd.push_back(calculate_and_sort_times(wfd_map.at(wfd_bank), TICKWFD[icrate]));
    // 	tdc.push_back(calculate_and_sort_times(tdc_map.at(tdc_bank), TICKTDC));
    //   } else {
    // 	static const std::vector<double> default_vector = std::vector<double>();
    // 	wfd.push_back(default_vector);
    // 	tdc.push_back(default_vector);
    //   }
    // }

  //   int sync[NCHANWFD[icrate]], nmax[NCHANWFD[icrate]], ;
  //   memset(sync, 0, NCHANWFD[icrate]*sizeof(int));
  //   memset(nmax, 0, NCHANWFD[icrate]*sizeof(int));
  //   for (int isync = 0; isync < synctdc.size(); ++isync) {
  //     const double t0_wfd = syncwfd.at(0);
  //     const double t0_tdc = synctdc[isync];
  //     for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
  // 	int n = 0;
  // 	const std::vector<double>::const_iterator wfd_beg = wfd[ich].begin();
  // 	const std::vector<double>::const_iterator wfd_end = wfd[ich].end();
  // 	for (int itdc = 0; itdc < tdc[ich].size(); ++itdc) {
  // 	  int nmatch = std::upper_bound(wfd_beg, wfd_end,
  // 					tdc[ich][itdc] - t0_tdc + t0_wfd + COINC_WINDOW) -
  // 	               std::upper_bound(wfd_beg, wfd_end,
  // 					tdc[ich][itdc] - t0_tdc + t0_wfd - COINC_WINDOW);
  // 	  if (nmatch) ++n;
  // 	}
  // 	vvhNumMatches[icrate][ich]->Fill(isync, n);
  // 	if (n > nmax[ich]) {
  // 	  printf("D%d%02d\t%d\t%d\t%d\n", icrate, ich, n, nmax[ich], isync);
  // 	  sync[ich] = isync;
  // 	  nmax[ich] = n;
  // 	}
  //     }
  //   }
  //   for (int ich = 0; ich < NCHANWFD[icrate]; ++ich)
  //     vvhIdentifySyncPulse[icrate][ich]->Fill(sync[ich]);
  // }


  // std::vector<double> tmp_get = calculate_and_sort_times(tdc_map.at(gSetup->GetBankName("TGeCHT")), tdc_tick);
  // std::vector<double> tmp_gee = calculate_and_sort_times(wfd_map.at(gSetup->GetBankName("GeCHEL")), 1./16.1290e6);
  // std::vector<double> tmp_syncwfd = calculate_and_sort_times(wfd_map.at(gSetup->GetBankName("SyncCrate4")), 1./16.1290e6);
  // for (int i = 0; i < tmp_get.size(); ++i) {
  //   for (int j = 0; j < tmp_gee.size(); ++j) {
  //     hTimingCorrelationGe->Fill((tmp_get[i]-synctdc[sync]) - (tmp_gee[j]));
  //   }
  // }

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
