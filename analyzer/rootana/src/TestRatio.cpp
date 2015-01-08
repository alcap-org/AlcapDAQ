//#define USE_PRINT_OUT 

#include "TestRatio.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>

#include "TAnalysedPulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

std::map<std::string, TH1F*> ratioTest_plots;

TestRatio::TestRatio(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){  
  dir->cd("/");
}



TestRatio::~TestRatio(){
}

int TestRatio::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){


  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++) {

    std::string detname = detIter->first;
    std::string keyname = detIter->first + GetName();

    // Create the histogram if it's not been created yet
    if (ratioTest_plots.find(keyname) == ratioTest_plots.end() ) {

      // hAmplitude
      std::string histname = "h" + detname + "_Ratio";
      std::string histtitle = "Plot of the integral ratio of pulses in the " + detname + " detector";

      double max_ratio = 0.5;
      TH1F* hRatio = new TH1F(histname.c_str(), histtitle.c_str(), 200,0,max_ratio);
      hRatio->GetXaxis()->SetTitle("Integral Ratio");
      hRatio->GetYaxis()->SetTitle("Arbitrary Unit");
      ratioTest_plots[keyname] = hRatio;
    }

    std::vector<TAnalysedPulse*> pulses = detIter->second;

    for (std::vector<TAnalysedPulse*>::iterator pulseIter = pulses.begin(); pulseIter != pulses.end(); ++pulseIter) {
      double ratio = (*pulseIter)->GetRatio();
      ratioTest_plots[keyname]->Fill(ratio);
	    
    } // end loop through pulses
	  
  } // end loop through detectors
  return 0;
}
