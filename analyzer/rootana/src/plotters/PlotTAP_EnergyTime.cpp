//#define USE_PRINT_OUT 

#include "PlotTAP_EnergyTime.h"
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

PlotTAP_EnergyTime::PlotTAP_EnergyTime(modules::options* opts) : 
    BaseModule("PlotTAP_EnergyTime",opts) {
    }

PlotTAP_EnergyTime::~PlotTAP_EnergyTime(){  
}

int PlotTAP_EnergyTime::BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup){
    return 0;
}

int PlotTAP_EnergyTime::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){

    // Loop over each TAP list
    for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
            i_det != gAnalysedPulseMap.end();
            i_det++) {

        const std::string& detname = i_det->first.str();
        std::string keyname = i_det->first.str() + GetName();

        // Create the histogram if it's not been created yet
        if ( fEnergyTimePlots.find(keyname) == fEnergyTimePlots.end() ) {

            // hEnergyTime
            std::string histname = "h" + detname + "_EnergyTime";
            std::stringstream histtitle;
            histtitle<<"EnergyTime of pulses from source " << i_det->first;
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            int n_bits = gSetup->GetNBits(gSetup->GetBankName(i_det->first.Channel().str()));
            const double max_adc_value = std::pow(2, n_bits);
            double gain  = 1;
            double offset= 0;
            try{
               gain  = SetupNavigator::Instance()->GetAdcToEnergyGain(    i_det->first.Channel());
               offset= SetupNavigator::Instance()->GetAdcToEnergyConstant(i_det->first.Channel());
            }catch( Except::InvalidDetector& e){};
	    //TODO: make the time axis configurable in the cfg file
            TH2F* hEnergyTime = new TH2F(histname.c_str(), histtitle.str().c_str(), max_adc_value,0,gain*max_adc_value + offset, 10000,-10000,10000);
            hEnergyTime->GetXaxis()->SetTitle("Energy (KeV)");
            hEnergyTime->GetYaxis()->SetTitle("Time [ns]");
            fEnergyTimePlots[keyname] = hEnergyTime;
        }

        const AnalysedPulseList *pulses = &i_det->second;
        //if(Debug() && pulses->empty()) DEBUG_PREFIX<<" no pulses to fill for "<<i_det->first<<std::endl;

        for (AnalysedPulseList::const_iterator pulseIter = pulses->begin(); pulseIter != pulses->end(); ++pulseIter) {
	    double Energy = (*pulseIter)->GetEnergy();
	    double Time = (*pulseIter)->GetTime();
            fEnergyTimePlots[keyname]->Fill(Energy, Time);

        } // end loop through pulses

    } // end loop through detectors
    return 0;
}

ALCAP_REGISTER_MODULE(PlotTAP_EnergyTime)
