//#define USE_PRINT_OUT 

#include "PlotTAPAmplitude.h"
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

PlotTAPAmplitude::PlotTAPAmplitude(modules::options* opts) : 
    BaseModule("PlotTAPAmplitude",opts) {
    }

PlotTAPAmplitude::~PlotTAPAmplitude(){  
}

int PlotTAPAmplitude::BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup){
    return 0;
}

int PlotTAPAmplitude::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){

    // Loop over each TAP list
    for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
            i_det != gAnalysedPulseMap.end();
            i_det++) {

        const std::string& detname = i_det->first.str();
        std::string keyname = i_det->first.str() + GetName();

        // Create the histogram if it's not been created yet
        if ( fAmplitudePlots.find(keyname) == fAmplitudePlots.end() ) {

            // hAmplitude
            std::string histname = "h" + detname + "_Amplitude";
            std::stringstream histtitle;
            histtitle<<"Amplitude of pulses from source " << i_det->first;
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            int n_bits = gSetup->GetNBits(gSetup->GetBankName(i_det->first.Channel().str()));
            double max_adc_value = std::pow(2, n_bits);
            TH1F* hAmplitude = new TH1F(histname.c_str(), histtitle.str().c_str(), max_adc_value,0,max_adc_value);
            hAmplitude->GetXaxis()->SetTitle("Amplitude (ADC value)");
            hAmplitude->GetYaxis()->SetTitle("Arbitrary Units");
            fAmplitudePlots[keyname] = hAmplitude;
        }

        const AnalysedPulseList *pulses = &i_det->second;
        //if(Debug() && pulses->empty()) DEBUG_PREFIX<<" no pulses to fill for "<<i_det->first<<std::endl;

        for (AnalysedPulseList::const_iterator pulseIter = pulses->begin(); pulseIter != pulses->end(); ++pulseIter) {
            double amplitude = (*pulseIter)->GetAmplitude();
            fAmplitudePlots[keyname]->Fill(amplitude);

        } // end loop through pulses

    } // end loop through detectors
    return 0;
}

ALCAP_REGISTER_MODULE(PlotTAPAmplitude)
