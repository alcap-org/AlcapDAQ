/***********************************************/
// Fast signal integration module for use in   //
// rootana for the Alcap experiment            //
//                                             //
// by Damien Alexander                         //
/***********************************************/


//#define USE_PRINT_OUT 

#include "ScintInt.h"
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

//extern std::map<std::string, std::vector<TPulseIsland*> > gPulseIslandMap;

std::map<std::string, TH1F*> integral_plots;

ScintInt::ScintInt(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){  
  dir->cd("/");
}

ScintInt::~ScintInt(){  
}

int ScintInt::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Loop through and find a fast channel
  //  std::cout << "Size of gAnalysedPulseMap " << gAnalysedPulseMap.size() << std::endl;
  for (map_iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++) {

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    std::string keyname = mapIter->first + GetName();

    // Put in detector choosing here, we want all fast signals
    if((detname != "NDet") && (detname != "NDet2"))
      continue;

    // Create the histogram if it's not been created yet
    if ( integral_plots.find(keyname) == integral_plots.end() ) {

      // hEnergy
      std::string histname = "h" + detname + "_Energy"; // changed to energy
      std::string histtitle = "Plot of the energy of the pulses in the " + detname + " detector"; //changed to energy

      //int n_bits = gSetup->GetNBits(gSetup->GetBankName(detname));
      //double max_adc_value = std::pow(2, n_bits);
      //double max_energy = 0;

      //determine which detector and therfore which calibration to use
      //if(detname == NDet)
      //max_energy = max_adc_value * 0.00603;

      //if(detname == NDet2)
      //max_energy = max_adc_value * 0.00391;

      TH1F* hIntegral = new TH1F(histname.c_str(), histtitle.c_str(), 25000 ,0, 25000);  //changed to max_energy
      hIntegral->GetXaxis()->SetTitle("Energy (Integral in adc counts)");  //changed to energy
      hIntegral->GetYaxis()->SetTitle("Arbitrary Unit");
      integral_plots[keyname] = hIntegral;
    }

    std::vector<TPulseIsland*> pulses = mapIter->second;
    //double pedestal = gSetup->GetPedestal(detname);

    for (std::vector<TPulseIsland*>::iterator pulseIter = pulses.begin(); pulseIter != pulses.end(); ++pulseIter) {
      //double energy = 0;
      double integral = 0;
      std::vector<int> samples = (*pulseIter)->GetSamples();
   
      for(std::vector<int>::iterator sIter = samples.begin(); sIter != samples.end(); sIter++)
        integral += ((*sIter) - 1265);

	//if(detname == NDet)
        //energy = amplitude * 0.00603;

      //if(detname == NDet2)
	//energy = amplitude * 0.00391;
      integral_plots[keyname]->Fill(integral);
	    
    } // end loop through pulses
	  
  } // end loop through detectors
  return 0;
}
