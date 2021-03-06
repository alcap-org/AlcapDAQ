//#define USE_PRINT_OUT 

#include "PlotTAP_Energy.h"
#include <iostream>
#include <string>
#include <sstream>
#include <map>
//#include <utility>
//#include <algorithm>
#include <cmath>

#include "TAnalysedPulse.h"
//#include "TDetectorPulse.h"
#include "RegisterModule.inc"
#include "definitions.h"
#include "SetupNavigator.h"
//#include "debug_tools.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotTAP_Energy::PlotTAP_Energy(modules::options* opts) : 
  BaseModule("PlotTAP_Energy",opts), 
  fNBins(opts->GetInt("n_bins", 0)),
  fMinEnergy(opts->GetDouble("min_energy", 0)),
  fMaxEnergy(opts->GetDouble("max_energy", 0)){
    }

PlotTAP_Energy::~PlotTAP_Energy(){  
}

int PlotTAP_Energy::BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup){
    return 0;
}

int PlotTAP_Energy::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){

    // Loop over each TAP list
    for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
            i_det != gAnalysedPulseMap.end();
            i_det++) {

        const std::string& detname = i_det->first.str();
        std::string keyname = i_det->first.str() + GetName();

        // Create the histogram if it's not been created yet
        if ( fEnergyPlots.find(keyname) == fEnergyPlots.end() ) {

            // hEnergy
            std::string histname = "h" + detname + "_Energy";
            std::stringstream histtitle;
            histtitle<<"Energy of pulses from source " << i_det->first;
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();

	    if (fNBins==0) { // if we have the default number of bins
	      int n_bits = gSetup->GetNBits(gSetup->GetBankName(i_det->first.Channel().str()));
	      const double max_adc_value = std::pow(2, n_bits);
	      fNBins = max_adc_value;
	      double gain  = 1;
	      double offset= 0;
	      try{
		gain  = SetupNavigator::Instance()->GetAdcToEnergyGain(    i_det->first.Channel());
		offset= SetupNavigator::Instance()->GetAdcToEnergyConstant(i_det->first.Channel());
	      }catch( Except::InvalidDetector& e){};
	      fMinEnergy = 0;
	      fMaxEnergy = gain*max_adc_value + offset;
	    }
            TH1F* hEnergy = new TH1F(histname.c_str(), histtitle.str().c_str(), fNBins,fMinEnergy,fMaxEnergy);
            hEnergy->GetXaxis()->SetTitle("Energy (KeV)");
            hEnergy->GetYaxis()->SetTitle("Arbitrary Units");
            fEnergyPlots[keyname] = hEnergy;
        }

        const AnalysedPulseList *pulses = &i_det->second;
        //if(Debug() && pulses->empty()) DEBUG_PREFIX<<" no pulses to fill for "<<i_det->first<<std::endl;

        for (AnalysedPulseList::const_iterator pulseIter = pulses->begin(); pulseIter != pulses->end(); ++pulseIter) {
            double Energy = (*pulseIter)->GetEnergy();
            fEnergyPlots[keyname]->Fill(Energy);

        } // end loop through pulses

    } // end loop through detectors
    return 0;
}

ALCAP_REGISTER_MODULE(PlotTAP_Energy, n_bins, min_energy, max_energy)
