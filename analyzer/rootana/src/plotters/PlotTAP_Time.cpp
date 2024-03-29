//#define USE_PRINT_OUT 

#include "PlotTAP_Time.h"
#include <iostream>
#include <string>
#include <sstream>
#include <map>
//#include <utility>
//#include <algorithm>
#include <cmath>
#include "definitions.h"
#include "SetupNavigator.h"

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "RegisterModule.inc"
//#include "debug_tools.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotTAP_Time::PlotTAP_Time(modules::options* opts) :
    BaseModule("PlotTAP_Time",opts) {
    }

PlotTAP_Time::~PlotTAP_Time(){  
}

int PlotTAP_Time::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){

  double min_time = 0;
  double max_time = 110e6; // ms
  double n_bins = 1e3;
    // Loop over each TAP list
    for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
            i_det != gAnalysedPulseMap.end();
            i_det++) {


        const std::string& detname = i_det->first.str();
        std::string keyname = i_det->first.str() + GetName();

        // Create the histogram if it's not been created yet
        if ( fTimePlots.find(keyname) == fTimePlots.end() ) {

            // hTime
            std::string histname = "h" + detname + "_Time";
            std::stringstream histtitle;
            histtitle<<"Time of pulses from source " << i_det->first;
            histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
            TH1F* hTime = new TH1F(histname.c_str(), histtitle.str().c_str(), n_bins,min_time,max_time);
            hTime->GetXaxis()->SetTitle("Time (ns)");
            hTime->GetYaxis()->SetTitle("Arbitrary Units");
            fTimePlots[keyname] = hTime;
        }

        const AnalysedPulseList *pulses =& i_det->second;
        //if(Debug() && pulses->empty()) DEBUG_PREFIX<<" no pulses to fill for "<<i_det->first<<std::endl;

        for (AnalysedPulseList::const_iterator pulseIter = pulses->begin(); pulseIter != pulses->end(); ++pulseIter) {
            double time = (*pulseIter)->GetTime();
            fTimePlots[keyname]->Fill(time);

        } // end loop through pulses

    } // end loop through detectors
    return 0;
}

ALCAP_REGISTER_MODULE(PlotTAP_Time)
