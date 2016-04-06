////////////////////////////////////////////////////////////////////////////////
/// \defgroup MPedestals
/// \author John R Quirk
///
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <cmath>
#include <numeric>
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TProfile.h>
#include <TTree.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"
#include "TTrendTree.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT MPedestals_init(void);
INT MPedestals_eor(INT);
INT MPedestals(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;
extern TTrendTree* gTrendTree;

static map<string, TProfile*> vhAvg;
static map<string, TProfile*> vhRMS;

ANA_MODULE MPedestals_module =
{
  "MPedestals",    /* module name           */
  "John R Quirk",  /* author                */
  MPedestals,      /* event routine         */
  NULL,            /* BOR routine           */
  MPedestals_eor,  /* EOR routine           */
  MPedestals_init, /* init routine          */
  NULL,            /* exit routine          */
  NULL,            /* parameter structure   */
  0,               /* structure size        */
  NULL,            /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MPedestals_init() {
  
  // Create a histogram for each bank
  std::map<std::string, std::string> Bank2DetMap = gSetup->fBankToDetectorMap;
  typedef std::map<std::string, std::string>::iterator String2StringMapIter;

  for(String2StringMapIter mapIter = Bank2DetMap.begin(); 
      mapIter != Bank2DetMap.end(); mapIter++) {

    std::string bankname = mapIter->first;
    std::string histname = "hPedAvg_" + bankname;
    std::string histtitle = "Pedestals in " +
      bankname + ";N presamples;Avg of presamples";
    vhAvg[bankname] = new TProfile(histname.c_str(), histtitle.c_str(),
				   9, 2, 11, "S");
    histname = "hPedRMS_" + bankname;
    histtitle = "RMS of pedestals in " +
      bankname + ";N presamples;RMS of presamples";
    vhRMS[bankname] = new TProfile(histname.c_str(), histtitle.c_str(),
				   9, 2, 11, "S");

    gTrendTree->InitRunBranch<Double_t>(bankname+"_PedAvg", -1.);
    gTrendTree->InitRunBranch<Double_t>(bankname+"_PedRMS", -1.);
    gTrendTree->InitRunBranch<Double_t>(bankname+"_PedPulseRMS", -1.);
    gTrendTree->InitBlockBranch<double>(bankname+"_PedAvg", -1.);
  }
  return SUCCESS;
}

INT MPedestals_eor(INT run)
{
  map<std::string, TProfile*>::const_iterator his;
  for (his = vhAvg.begin(); his != vhAvg.end(); ++his) {
    gTrendTree->FillRunTree(his->first+"_PedAvg",
			    vhAvg.at(his->first)->GetBinContent(3));
    gTrendTree->FillRunTree(his->first+"_PedRMS",
			    vhAvg.at(his->first)->GetBinError(3));
    gTrendTree->FillRunTree(his->first+"_PedPulseRMS",
			    vhRMS.at(his->first)->GetBinContent(3));
  }
  return SUCCESS;
}

INT MPedestals(EVENT_HEADER *pheader, void *pevent)
{
  // Some typedefs
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map = gData->fPulseIslandToChannelMap;

  // Loop over the map and get each bankname, vector pair
  for (map_iterator mapIter = pulse_islands_map.begin(); 
       mapIter != pulse_islands_map.end(); ++mapIter) {
    const std::string& bankname = mapIter->first;
    const std::vector<TPulseIsland*>& thePulses = mapIter->second;
    double block_avg = 0.;
    // Loop over the TPulseIslands and plot the histogram
    for (std::vector<TPulseIsland*>::const_iterator pulseIter =
	   thePulses.begin(); pulseIter != thePulses.end(); ++pulseIter) {
      if (vhAvg.find(bankname) == vhAvg.end()) continue;
      TProfile* hAvg = vhAvg[bankname];
      TProfile* hRMS = vhRMS[bankname];
      const vector<int>::const_iterator& s0 =
	(*pulseIter)->GetSamples().begin();
      for (int ds = 2; ds <= 10; ++ds) {
	double sum = std::accumulate(s0, s0 + ds, 0.);
	double sqsum = std::inner_product(s0, s0 + ds,
					  s0, 0.);
	double avg = sum/ds;
	double std = std::sqrt(sqsum/(ds-1) - avg*avg*ds/(ds-1));
	hAvg->Fill(ds, avg);
	hRMS->Fill(ds, std);
	if (ds == 4) block_avg += avg;
      }
    }
    if (thePulses.size() > 0) {
      block_avg /= thePulses.size();
      gTrendTree->FillBlockTree(mapIter->first+"_PedAvg", block_avg);
    }
  }
  return SUCCESS;
}

/// @}
