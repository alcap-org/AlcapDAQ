//#define USE_PRINT_OUT 

#include "PlotTime.h"
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

PlotTime::PlotTime(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){  
  dir->cd("/");
}

PlotTime::PlotTime(modules::options* opts) : FillHistBase(opts->GetString("0").c_str()) {
  dir->cd("/");
}

PlotTime::~PlotTime(){  
}

int PlotTime::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Loop through and find a fast channel
  //  std::cout << "Size of gAnalysedPulseMap " << gAnalysedPulseMap.size() << std::endl;
  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++) {

    std::string detname = detIter->first;


    // Create the histogram if it's not been created yet
    if ( fTimePlots.find(detname) == fTimePlots.end() ) {

      // hTimeCorrelation
      std::string histname = "h" + detname + "_" + GetName();
      std::string histtitle = "Plot of the time of pulses in the " + detname + " detector";

      TH1F* hTimeCorrelation = new TH1F(histname.c_str(), histtitle.c_str(), 1e6,0,1e8);
      hTimeCorrelation->GetXaxis()->SetTitle("Time [ns]");
      hTimeCorrelation->GetYaxis()->SetTitle("Arbitrary Unit");
      fTimePlots[detname] = hTimeCorrelation;
    }

    std::vector<TAnalysedPulse*> pulses = detIter->second;

    for (std::vector<TAnalysedPulse*>::iterator pulseIter = pulses.begin(); pulseIter != pulses.end(); ++pulseIter) {
      double time = (*pulseIter)->GetTime();
      fTimePlots[detname]->Fill(time);
	    
    } // end loop through pulses
	  
  } // end loop through detectors
  return 0;
}

ALCAP_REGISTER_MODULE(PlotTime)
