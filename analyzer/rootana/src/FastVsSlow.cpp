#include "FastVsSlow.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>

#include "TH2.h"

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern std::map<std::string, std::vector<TDetectorPulse*> > gDetectorPulseMap;

std::map<std::string, TH2F*> time_hists;

FastVsSlow::FastVsSlow(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){
}

FastVsSlow::~FastVsSlow(){  
}

int FastVsSlow::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){

  for (std::map<std::string, std::vector<TDetectorPulse*> >::iterator mapIter = gDetectorPulseMap.begin(); mapIter != gDetectorPulseMap.end(); ++mapIter) {


    std::string detname = mapIter->first;

    // Create the histogram if it's not been created yet
    if ( time_hists.find(detname) == time_hists.end() ) {
      std::cout << "Histogram not found" << std::endl;
      std::cout << "Creating now..." << std::endl;

      // hTimeCorrelation
      std::string histname = "h" + detname + "_TimeCorrelation";
      std::string histtitle = "Plot of fast time vs slow time in the " + detname + " detector";
      TH2F* hTimeCorrelation = new TH2F(histname.c_str(), histtitle.c_str(), 1500,-1,150, 1500,-1,150);
      hTimeCorrelation->GetXaxis()->SetTitle("t_{F}");
      hTimeCorrelation->GetYaxis()->SetTitle("t_{S}");
      time_hists[detname] = hTimeCorrelation;
    }

    // Loop through the detector pulses and plot the correlations
    std::vector<TDetectorPulse*> detPulses = mapIter->second;
    for (std::vector<TDetectorPulse*>::iterator detPulseIter = detPulses.begin(); detPulseIter != detPulses.end(); ++detPulseIter) {

      double fast_pulse_time = (*detPulseIter)->GetFastPulseTime() * 1e-6;
      double slow_pulse_time = (*detPulseIter)->GetSlowPulseTime() * 1e-6;

      //Check that we have both pulses
      std::cout << fast_pulse_time << ", " << slow_pulse_time << std::endl;
      (time_hists[detname])->Fill(fast_pulse_time, slow_pulse_time);
    }
  } // end loop through TDetetorPulse map
  return 0;
}
