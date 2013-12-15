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

using std::string;
using std::map;
using std::vector;
using std::pair;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

std::map<std::string, TH1F*> amplitude_plots;

PlotAmplitude::PlotAmplitude(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){  
  dir->cd("/");
}

PlotAmplitude::~PlotAmplitude(){  
}

int PlotAmplitude::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Loop through and find a fast channel
  //  std::cout << "Size of gAnalysedPulseMap " << gAnalysedPulseMap.size() << std::endl;
  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++) {

    std::string detname = detIter->first;


    // Create the histogram if it's not been created yet
    if ( amplitude_plots.find(detname) == amplitude_plots.end() ) {

      // hTimeCorrelation
      std::string histname = "h" + detname + "_Amplitude";
      std::string histtitle = "Plot of the amplitude of pulses in the " + detname + " detector";
      int n_bits = gSetup->GetNBits(gSetup->GetBankName(detname));
      double max_adc_value = std::pow(2, n_bits);
      TH1F* hTimeCorrelation = new TH1F(histname.c_str(), histtitle.c_str(), max_adc_value,0,max_adc_value);
      hTimeCorrelation->GetXaxis()->SetTitle("Amplitude [ADC value]");
      hTimeCorrelation->GetYaxis()->SetTitle("Arbitrary Unit");
      amplitude_plots[detname] = hTimeCorrelation;
    }

    std::vector<TAnalysedPulse*> pulses = detIter->second;

    for (std::vector<TAnalysedPulse*>::iterator fastPulseIter = pulses.begin(); fastPulseIter != pulses.end(); ++fastPulseIter) {
      double amplitude = (*fastPulseIter)->GetAmplitude();
      amplitude_plots[detname]->Fill(amplitude);
	    
    } // end loop through pulses
	  
  } // end loop through detectors
  return 0;
}
