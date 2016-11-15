//#define USE_PRINT_OUT 

#include "PlotTAP_PedestalAmplitude.h"
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

PlotTAP_PedestalAmplitude::PlotTAP_PedestalAmplitude(modules::options* opts) : 
    BaseModule("PlotTAP_PedestalAmplitude",opts) {
    }

PlotTAP_PedestalAmplitude::~PlotTAP_PedestalAmplitude(){  
}

int PlotTAP_PedestalAmplitude::BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup){
    return 0;
}

int PlotTAP_PedestalAmplitude::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){

    // Loop over each TAP list
    for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
            i_det != gAnalysedPulseMap.end();
            i_det++) {

        const std::string& detname = i_det->first.str();
        std::string keyname = i_det->first.str() + GetName();

        // Create the histogram if it's not been created yet
        if ( fPedestalAmplitudePlots.find(keyname) == fPedestalAmplitudePlots.end() ) {

            // hPedestalAmplitude
            std::string histname = "h" + detname + "_PedestalAmplitude";
            std::stringstream histtitle;
            histtitle<<"PedestalAmplitude of pulses from source " << i_det->first;
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            int n_bits = gSetup->GetNBits(gSetup->GetBankName(i_det->first.Channel().str()));
            const double max_adc_value = std::pow(2, n_bits);
            TH2F* hPedestalAmplitude = new TH2F(histname.c_str(), histtitle.str().c_str(), max_adc_value,0,max_adc_value, max_adc_value/10,0,max_adc_value);
            hPedestalAmplitude->GetXaxis()->SetTitle("Pedestal [ADC]");
            hPedestalAmplitude->GetYaxis()->SetTitle("Amplitude (Not Pedestal Subtracted) [ADC]");
            fPedestalAmplitudePlots[keyname] = hPedestalAmplitude;
        }

        const AnalysedPulseList *pulses = &i_det->second;
        //if(Debug() && pulses->empty()) DEBUG_PREFIX<<" no pulses to fill for "<<i_det->first<<std::endl;

        for (AnalysedPulseList::const_iterator pulseIter = pulses->begin(); pulseIter != pulses->end(); ++pulseIter) {
	    double Pedestal = (*pulseIter)->GetPedestal();
	    double Amplitude = (*pulseIter)->GetAmplitude();
            fPedestalAmplitudePlots[keyname]->Fill(Pedestal, Amplitude);

        } // end loop through pulses

    } // end loop through detectors
    return 0;
}

ALCAP_REGISTER_MODULE(PlotTAP_PedestalAmplitude)
