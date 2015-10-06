////////////////////////////////////////////////////////////////////////////////
/// \defgroup MSyncPulseTDiff
///
/// \brief
/// Timing difference in each crate of a sync pulse and the following
/// sync pulse. Also the average based on the first and last sync
/// pulse.
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
#include "TH2D.h"
#include "TDirectory.h"

//JG: added alcap includes
/* AlCap includes */
#include "AlCap.h"
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
using namespace AlCap;
using std::string;
using std::vector;
using std::map;
namespace {
  TDirectory* DIR;
  TH1* vhSyncAllTDiffWFD[NCRATE];
  TH1* vhSyncAvgTDiffWFD[NCRATE];
  TH2* vhSyncTDiffTDCWFDRatio[NCRATE];
  TH1* hSyncAllTDiffTDC;
  TH1* hSyncAvgTDiffTDC;
  TH1* hMasterAllTDiff;
  TH1* hMasterAvgTDiff;

  string vsBankSyncWFD[NCRATE];
  string sBankSyncTDC;
  string sBankMaster;

  double vvHistBounds[NCRATE][2] = { {0., 0.015}, {0., 0.015},
				     {0., 0.015}, {0., 0.015},
				     {3.93e-4, 3.94e-4}, {0., 0.015},
				     {0., 0.015}, {1.2207e-2, 1.2208e-2},
				     {0., 0.015}, {0., 0.015} };
}

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
  DIR = gDirectory->mkdir("SyncPulseTDiff/");
  DIR->cd();

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    char det[32], name[64], title[128];
    sprintf(det, "SyncCrate%d", icrate);
    vsBankSyncWFD[icrate] = gSetup->GetBankName(det);

    sprintf(name, "hSyncPulseAllTDiff_%d", icrate);
    sprintf(title, "Sync Pulse All dT Distribution Crate %d;Ti-T(i-1) (Ticks)", icrate);
    vhSyncAllTDiffWFD[icrate] = new TH1D(name, title, 2000000, 0., 2e6);
    vhSyncAllTDiffWFD[icrate]->Sumw2();

    sprintf(name, "hSyncPulseAvgTDiff_%d", icrate);
    sprintf(title, "Sync Pulse Avg dT Distribution Crate %d;Tf-Ti (Ticks)", icrate);
    vhSyncAvgTDiffWFD[icrate] = new TH1D(name, title, 2000000, 0., 2e6);
    vhSyncAvgTDiffWFD[icrate]->Sumw2();

    sprintf(name, "hSyncTDiffTDCWFDRatio_%d", icrate);
    sprintf(title, "Sync Pulse TDiff Ratio Crate %d;TDC/WFD", icrate);
    vhSyncTDiffTDCWFDRatio[icrate] = new TH2D(name, title,
					      10000, vvHistBounds[icrate][0], vvHistBounds[icrate][1],
					      60,   0., 80.);
    vhSyncTDiffTDCWFDRatio[icrate]->Sumw2();
  }

  sBankSyncTDC = gSetup->GetBankName("TSync");
  hSyncAllTDiffTDC = new TH1D("hSyncPulseAllTDiff_TDC",
			      "Sync Pulse All dT Distribution TDC;Ti-T(i-1) (Ticks)",
			      2000000, 72000000., 74000000.);
  hSyncAllTDiffTDC->Sumw2();
  hSyncAvgTDiffTDC = new TH1D("hSyncPulseAvgTDiff_TDC",
			      "Sync Pulse Avg dT Distribution TDC;Tf-Ti (Ticks)",
			      2000000, 72000000., 74000000.);
  hSyncAvgTDiffTDC->Sumw2();

  // These next two only matter for run 7319, where
  // the clock is plugged into TDC Ch. 2 (TMaster)
  sBankMaster = gSetup->GetBankName("TMaster");
  hMasterAllTDiff = new TH1D("hMasterPulseAllTDiff_TDC",
			     "Master Clock All dT Distribution TDC;Ti-T(i-1) (Ticks)",
			     2000000, 0., 2000000);
  hMasterAllTDiff->Sumw2();
  hMasterAvgTDiff = new TH1D("hMasterPulseAvgTDiff_TDC",
			     "Master Clock Avg dT Distribution TDC;Tf-Ti (Ticks)",
			     2000000, 0., 2000000);
  hMasterAvgTDiff->Sumw2();

  cwd->cd();
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MSyncPulseTDiff(EVENT_HEADER *pheader, void *pevent) {
  const map < string, vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;
  const map< string, vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    if (!wfd_map.count(vsBankSyncWFD[icrate])) continue;
    const vector<int> times =
      identify_pulses_and_calculate_times(wfd_map.at(vsBankSyncWFD[icrate]));
    for (int ipulse = 1; ipulse < times.size(); ++ipulse)
      vhSyncAllTDiffWFD[icrate]->Fill(times[ipulse] - times[ipulse-1]);
    if (times.size() > 2)
      vhSyncAvgTDiffWFD[icrate]->Fill((times.back() - times.front())/
				      (double)(times.size()-1));
  }


  if (tdc_map.count(sBankSyncTDC)) {
    const vector<int64_t>& hits = tdc_map.at(sBankSyncTDC);
    for (int ihit = 1; ihit < hits.size(); ++ihit)
      hSyncAllTDiffTDC->Fill(hits[ihit]-hits[ihit-1]);
    if (hits.size() >= 2)
      hSyncAvgTDiffTDC->Fill((hits.back()-hits.front()) /
                                        (double)(hits.size()-1));

    for (int icrate = 0; icrate < NCRATE; ++icrate) {
      if (!wfd_map.count(vsBankSyncWFD[icrate])) continue; 
      const vector<int> times =
	identify_pulses_and_calculate_times(wfd_map.at(vsBankSyncWFD[icrate]));
      const int align = gData->fTDCSynchronizationPulseIndex[icrate];
      if (align == -1) continue;
      for (int i = align + 1, j = 1, h0 = hits[align], t0 = times[0];
	   i < hits.size() && j < times.size();
	   ++i, ++j)
	vhSyncTDiffTDCWFDRatio[icrate]->Fill((times[j] - t0) /
					     (double)(hits[i]-h0),
					     j);
    }
  }

  if (tdc_map.count(sBankMaster)) {
    const vector<int64_t>& hits = tdc_map.at(sBankMaster);
    for (int ihit = 1; ihit < hits.size(); ++ihit)
      hMasterAllTDiff->Fill(hits[ihit]-hits[ihit-1]);
    if (hits.size() >= 2)
      hMasterAvgTDiff->Fill((hits.back()-hits.front()) /
                                 (double)(hits.size()-1));
  }


  return SUCCESS;
}

static const int PULSE_HEIGHT_CUT = 500;

std::vector<int> identify_pulses_and_calculate_times(const std::vector<TPulseIsland*>& tpis) {
  std::vector<int> ts;
  ts.reserve(tpis.size());
  for (int i = 0; i < tpis.size(); ++i)
    if (tpis[i]->GetPulseHeight() > PULSE_HEIGHT_CUT)
      ts.push_back(tpis[i]->GetTimeStamp());
  return ts;
}

/// @}
