//#define USE_PRINT_OUT 

#include "PlotTAP_AmplitudeTime.h"
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

PlotTAP_AmplitudeTime::PlotTAP_AmplitudeTime(modules::options* opts) : 
    BaseModule("PlotTAP_AmplitudeTime",opts) {
    }

PlotTAP_AmplitudeTime::~PlotTAP_AmplitudeTime(){  
}

int PlotTAP_AmplitudeTime::BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup){
    return 0;
}

int PlotTAP_AmplitudeTime::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){

    // Loop over each TAP list
    for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
            i_det != gAnalysedPulseMap.end();
            i_det++) {

        const std::string& detname = i_det->first.str();
        std::string keyname = i_det->first.str() + GetName();

        // Create the histogram if it's not been created yet
        if ( fAmplitudeTimePlots.find(keyname) == fAmplitudeTimePlots.end() ) {

            // hAmplitudeTime
            std::string histname = "h" + detname + "_AmplitudeTime";
            std::stringstream histtitle;
            histtitle<<"AmplitudeTime of pulses from source " << i_det->first;
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            int n_bits = gSetup->GetNBits(gSetup->GetBankName(i_det->first.Channel().str()));
            const double max_adc_value = std::pow(2, n_bits);
	    //TODO: make the time axis configurable in the cfg file
            TH2F* hAmplitudeTime = new TH2F(histname.c_str(), histtitle.str().c_str(), max_adc_value,0,max_adc_value, 1000,0,110e6);
            hAmplitudeTime->GetXaxis()->SetTitle("Max Bin Amplitude [ADC]");
            hAmplitudeTime->GetYaxis()->SetTitle("Time [ns]");
            fAmplitudeTimePlots[keyname] = hAmplitudeTime;
        }

        const AnalysedPulseList *pulses = &i_det->second;
        //if(Debug() && pulses->empty()) DEBUG_PREFIX<<" no pulses to fill for "<<i_det->first<<std::endl;

        for (AnalysedPulseList::const_iterator pulseIter = pulses->begin(); pulseIter != pulses->end(); ++pulseIter) {
	    double Amplitude = (*pulseIter)->GetAmplitude();
	    double Time = (*pulseIter)->GetTime();
            fAmplitudeTimePlots[keyname]->Fill(Amplitude, Time);
        } // end loop through pulses

    } // end loop through detectors
    return 0;
}

ALCAP_REGISTER_MODULE(PlotTAP_AmplitudeTime)
