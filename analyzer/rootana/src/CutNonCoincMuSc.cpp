//#define USE_PRINT_OUT 

#include "CutNonCoincMuSc.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

std::map<std::string, TH1F*> tdiff_plots;

CutNonCoincMuSc::CutNonCoincMuSc(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){  
  dir->cd("/");
}

CutNonCoincMuSc::~CutNonCoincMuSc(){  
}

int CutNonCoincMuSc::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Get the muSc pulses ready for later
  std::vector<TAnalysedPulse*> muSc_pulses = gAnalysedPulseMap["muSc"];
  
  // Loop through the detectors
  //  std::cout << "Size of gAnalysedPulseMap " << gAnalysedPulseMap.size() << std::endl;
  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++) {

    std::string detname = detIter->first;
    std::vector<TAnalysedPulse*>& pulses = detIter->second;

    // Create the histogram if it's not been created yet
    if ( tdiff_plots.find(detname) == tdiff_plots.end() ) {

      // hTimeCorrelation
      std::string histname = "h" + detname + "_" + GetName();
      std::string histtitle = "Plot of the tdiff between muSc and  " + detname;

      TH1F* hTimeCorrelation = new TH1F(histname.c_str(), histtitle.c_str(), 2e6,-1e8,1e8);
      hTimeCorrelation->GetXaxis()->SetTitle("Time [ns]");
      hTimeCorrelation->GetYaxis()->SetTitle("Arbitrary Unit");

      tdiff_plots[detname] = hTimeCorrelation;
    }

    // Loop through the detector pulses
    for (std::vector<TAnalysedPulse*>::iterator pulseIter = pulses.begin(); pulseIter != pulses.end(); ++pulseIter) {

      // Loop through the muSc pulses
      bool coinc_found = false;
      for (std::vector<TAnalysedPulse*>::iterator muScPulseIter = muSc_pulses.begin(); muScPulseIter != muSc_pulses.end(); ++muScPulseIter) {

	double time = (*pulseIter)->GetTime();
	double muSc_time = (*muScPulseIter)->GetTime();
	double t_diff = time - muSc_time;
	//	tdiff_plots[detname]->Fill(t_diff);

	if (std::abs(t_diff) < 100) { // if within 200 ns
	  coinc_found = true;
	  break; // no need to go through the muSc pulses any more
	}
      } // end loop through muSc pulses

      // If no coincidence was found
      if (coinc_found == false) {
	delete *pulseIter;
      }

    } // end loop through pulses
  } // end loop through detectors
  return 0;
}
