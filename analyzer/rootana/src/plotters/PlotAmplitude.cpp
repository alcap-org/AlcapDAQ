//#define USE_PRINT_OUT 

#include "PlotAmplitude.h"
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
#include "RegisterModule.inc"
#include "definitions.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern StringAnalPulseMap gAnalysedPulseMap;

PlotAmplitude::PlotAmplitude(char *HistogramDirectoryName) :
  BaseModule(HistogramDirectoryName){  
  dir->cd("/");
}

PlotAmplitude::PlotAmplitude(modules::options* opts) : BaseModule(opts->GetString("0").c_str()) {
  dir->cd("/");
}

PlotAmplitude::~PlotAmplitude(){  
}

int PlotAmplitude::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){

  // Loop through and find a fast channel
  //  std::cout << "Size of gAnalysedPulseMap " << gAnalysedPulseMap.size() << std::endl;
  for (StringAnalPulseMap::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++) {

    std::string detname = detIter->first;
    std::string keyname = detIter->first + GetName();

    // Create the histogram if it's not been created yet
    if ( fAmplitudePlots.find(keyname) == fAmplitudePlots.end() ) {

      // hAmplitude
      std::string histname = "h" + detname + "_Amplitude";
      std::string histtitle = "Plot of the amplitude of pulses in the " + detname + " detector";
      int n_bits = gSetup->GetNBits(gSetup->GetBankName(detname));
      double max_adc_value = std::pow(2, n_bits);
      TH1F* hAmplitude = new TH1F(histname.c_str(), histtitle.c_str(), max_adc_value,0,max_adc_value);
      hAmplitude->GetXaxis()->SetTitle("Amplitude [ADC value]");
      hAmplitude->GetYaxis()->SetTitle("Arbitrary Unit");
      fAmplitudePlots[keyname] = hAmplitude;
    }

    AnalysedPulseList pulses = detIter->second;

    for (AnalysedPulseList::iterator pulseIter = pulses.begin(); pulseIter != pulses.end(); ++pulseIter) {
      double amplitude = (*pulseIter)->GetAmplitude();
      fAmplitudePlots[keyname]->Fill(amplitude);
	    
    } // end loop through pulses
	  
  } // end loop through detectors
  return 0;
}

ALCAP_REGISTER_MODULE(PlotAmplitude)
