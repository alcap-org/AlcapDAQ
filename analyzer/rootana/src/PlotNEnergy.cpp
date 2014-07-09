//#define USE_PRINT_OUT 

#include "PlotNEnergy.h"
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

std::map<std::string, TH1D*> energy_plots;

PlotNEnergy::PlotNEnergy(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){  
  dir->cd("/");
}

PlotNEnergy::~PlotNEnergy(){  
}

int PlotNEnergy::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Loop through and find a fast channel
  //  std::cout << "Size of gAnalysedPulseMap " << gAnalysedPulseMap.size() << std::endl;
  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++) {

    std::string detname = detIter->first;
    std::string keyname = detIter->first + GetName();

    // Put in detector choosing here
    if((detname != "NDet") && (detname != "NDet2"))
      continue;

    // Create the histogram if it's not been created yet
    if ( energy_plots.find(keyname) == energy_plots.end() ) {

      // hEnergy
      std::string histname = "h" + detname + "_Energy"; // changed to energy
      std::string histtitle = "Plot of the energy of the pulses in the " + detname + " detector"; //changed to energy

      int n_bits = gSetup->GetNBits(gSetup->GetBankName(detname));
      double max_adc_value = std::pow(2, n_bits);
      double max_energy = 0;

      //determine which detector and therfore which calibration to use
      if(detname == "NDet")
        max_energy = max_adc_value * 0.00571;

      if(detname == "NDet2")
        max_energy = max_adc_value * 0.00378;

      TH1D* hEnergy = new TH1D(histname.c_str(), histtitle.c_str(), (max_adc_value + 1),0,max_energy);  //changed to max_energy
      hEnergy->GetXaxis()->SetTitle("Energy [MeVee]");  //changed to energy
      hEnergy->GetYaxis()->SetTitle("Arbitrary Unit");
      energy_plots[keyname] = hEnergy;
    }

    std::vector<TAnalysedPulse*> pulses = detIter->second;

    for (std::vector<TAnalysedPulse*>::iterator pulseIter = pulses.begin(); pulseIter != pulses.end(); ++pulseIter) {
      double amplitude = (*pulseIter)->GetAmplitude();
      double energy = 0;

      if(detname == "NDet")
        energy = amplitude * 0.00571;  //different value to clear up binning error

      if(detname == "NDet2")
        energy = amplitude * 0.00378;
      
      energy_plots[keyname]->Fill(energy);
	    
    } // end loop through pulses
	  
  } // end loop through detectors
  return 0;
}
