////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_IslandCounter
/// \ingroup lldq
/// \author Andrew Edmonds
///
/// \brief
/// Plots number of TPIs in each detector per MIDAS event with
/// different normalizations.
///
/// \details
/// Creates several plots.
/// @{

/// \var DQ_IslandCounter_histograms_map
/// \brief
/// Maps bank name to histogram of number of TPIs in MIDAS event

/// \var DQ_IslandCounter_histograms_normalised_map
/// \brief
/// Same as ::DQ_IslandCounter_histograms_map, but normalized
/// vertical axis to muon count according to TDC.

/// \var DQ_IslandCounter_histograms_both_axes_normalised_map
/// \brief
/// Same as ::DQ_IslandCounter_histograms_normalised_map, but
/// additionally horizontal axis is normalized in the same way.
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MDQ_IslandCounter_init(void);
INT  MDQ_IslandCounter(EVENT_HEADER*, void*);
INT  MDQ_IslandCounter_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

namespace {
  map <string, TH1F*> hIslandCounterMap;
  map <string, TH1F*> hIslandRateMap;
}

ANA_MODULE MDQ_IslandCounter_module =
{
  "MDQ_IslandCounter",    /* module name           */
  "Andrew Edmonds",       /* author                */
  MDQ_IslandCounter,      /* event routine         */
  NULL,                   /* BOR routine           */
  NULL,                   /* EOR routine           */
  MDQ_IslandCounter_init, /* init routine          */
  NULL,                   /* exit routine          */
  NULL,                   /* parameter structure   */
  0,                      /* structure size        */
  NULL,                   /* initial parameters    */
};

INT MDQ_IslandCounter_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("IslandCounter")->cd();
  
  // Create a histogram for each detector
  const map<string, string>& bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(map<string, string>::const_iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); ++mapIter) { 

    const string& bankname = mapIter->first;
    const string detname = gSetup->GetDetectorName(bankname);

    if(TSetupData::IsTDC(bankname)) continue;

    string histname = "hIslandCounter_" + detname + "_" + bankname;
    string histtitle = "Islands per block in " + detname;
    TH1F* hist = new TH1F(histname.c_str(), histtitle.c_str(), 3500, 0, 3500);
    hist->GetXaxis()->SetTitle("Number of TPulseIslands");
    hist->GetYaxis()->SetTitle("Number of Blocks");
    hIslandCounterMap[bankname] = hist;

    histname = "hIslandRate_" + detname + "_" + bankname;
    histtitle = "Islands per mu per block in " + detname;
    hist = new TH1F(histname.c_str(), histtitle.c_str(), 3500, 0, 2.);
    hist->GetXaxis()->SetTitle("Number of TPulseIslands");
    hist->GetYaxis()->SetTitle("Number of Blocks");
    hIslandRateMap[bankname] = hist;
  }

  cwd->cd();
  return SUCCESS;
}

INT MDQ_IslandCounter(EVENT_HEADER *pheader, void *pevent) {
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef map<string, vector<TPulseIsland*> >::const_iterator map_iterator;

  const TStringPulseIslandMap& wfd_map =
    gData->fPulseIslandToChannelMap;

  for (map_iterator mapIter = wfd_map.begin(), end = wfd_map.end;
       mapIter != end; ++mapIter) {
    const string& bankname = mapIter->first;
    const int n_pulse_islands = mapIter->second.size();
    if (hIslandCounterMap.find(bankname) != hIslandCounterMap.end()) {
      hIslandCounterMap[bankname]->Fill(n_pulse_islands);
      hIslandRateMap[bankname]->Fill(n_pulse_islands/gData->NMuBlock());
    }
  }

  return SUCCESS;
}

/// @}
