////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_IslandLength
/// \ingroup lldq
/// \author Nam Tran
///
/// \brief
/// Plots information related to length of TPIs in each bank
/// per MIDAS event.
///
/// \details
/// Creates several histograms.
/// @{

/// \var DQ_IslandLength_histograms_map
/// \brief
/// Map of bank name to histogram of TPI lengths per event.

/// \var DQ_IslandLength_histograms_normalised_map
/// \brief
/// Same as ::DQ_IslandLength_histograms_map but normalized
/// to number of muons according to TDC.
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <cstdio>
#include <stdlib.h>
#include <string>
#include <map>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"
#include "TDirectory.h"

using std::string;
using std::map;
using std::vector;

/*-- Module declaration --------------------------------------------*/
INT MDQ_IslandLength_init(void);
INT MDQ_IslandLength(EVENT_HEADER*, void*);
INT MDQ_IslandLength_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

namespace {
  map <string, TH1F*> hIslandLengthMap;
  TDirectory* DIR;
}

ANA_MODULE MDQ_IslandLength_module =
{
  "MDQ_IslandLength",    /* module name           */
  "Nam Tran",            /* author                */
  MDQ_IslandLength,      /* event routine         */
  NULL,                  /* BOR routine           */
  MDQ_IslandLength_eor,  /* EOR routine           */
  MDQ_IslandLength_init, /* init routine          */
  NULL,                  /* exit routine          */
  NULL,                  /* parameter structure   */
  0,                     /* structure size        */
  NULL,                  /* initial parameters    */
};

INT MDQ_IslandLength_init() {
  TDirectory* cwd = gDirectory;
  DIR = gDirectory->mkdir("DQ_IslandLength");
  DIR->cd();

  map<string, string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(map<string, string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); ++mapIter) { 

    const std::string& bankname = mapIter->first;
    const std::string detname = gSetup->GetDetectorName(bankname);
    if(TSetupData::IsTDC(bankname)) continue;

    const std::string histname = "IslandLength_" + detname + "_" + bankname;
    const std::string histtitle = "Length of each TPulseIsland in " + detname;
    TH1F* hist = new TH1F(histname.c_str(), histtitle.c_str(), 1000, 0, 1000);
    hist->GetXaxis()->SetTitle("Length [samples]");
    hist->GetYaxis()->SetTitle("Number of Islands");
    hist->Sumw2();
    hIslandLengthMap[bankname] = hist;
  }

  cwd->cd();
  return SUCCESS;
}

INT MDQ_IslandLength_eor(INT run_number) {
  TDirectory* cwd = gDirectory;
  DIR->cd();

  for (map<string, TH1F*>::const_iterator ihist = hIslandLengthMap.begin(),
	 end = hIslandLengthMap.end(); ihist != end; ++ihist) {
    const TH1* h0 = ihist->second;
    std::string name(h0->GetName());
    std::string title(h0->GetTitle());
    name += "_norm";
    title += " (normalized)";
    TH1* hist = (TH1*)h0->Clone(name.c_str());
    hist->SetTitle(title.c_str());
    hist->Scale(1./gData->NMuRun());
  }
  cwd->cd();
  return SUCCESS;
}


INT MDQ_IslandLength(EVENT_HEADER *pheader, void *pevent) {
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef map<string, vector<TPulseIsland*> >::const_iterator map_iterator;
  
  const TStringPulseIslandMap& pulse_islands_map =
    gData->fPulseIslandToChannelMap;

  for (map_iterator mapIter = pulse_islands_map.begin(); 
       mapIter != pulse_islands_map.end(); ++mapIter)  {
    const string& bankname = mapIter->first;
    if (hIslandLengthMap.find(bankname) == hIslandLengthMap.end()) continue;
    const vector<TPulseIsland*>& thePulses = mapIter->second;
    TH1F* hist = hIslandLengthMap.at(bankname);
    
    for (vector<TPulseIsland*>::const_iterator pulseIter = thePulses.begin(),
	   end = thePulses.end(); pulseIter != end; ++pulseIter)
      hist->Fill((*pulseIter)->GetPulseLength());
  }
    return SUCCESS;
}

/// @}
