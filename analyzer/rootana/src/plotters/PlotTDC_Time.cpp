//#define USE_PRINT_OUT

#include "PlotTDC_Time.h"
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

PlotTDC_Time::PlotTDC_Time(modules::options* opts) :
    BaseModule("PlotTDC_Time",opts) {
    for (int i = 0; i < 16; ++i) {
      TString bank = TString::Format("T4%02d", i);
      fTimePlots[bank.Data()] = new TH1F('h'+bank+"time", "Time "+bank,
                                         1e5, 0., 110.e6);
      fNHitsPlots[bank.Data()] = new TH1F('h'+bank+"hits", "Hits/Block "+bank,
                                          1000, 0., 1000.);
    }
}

PlotTDC_Time::~PlotTDC_Time(){
}

int PlotTDC_Time::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup) {
    // Loop over each TDC list
    for (map< string, vector<int64_t> >::const_iterator i_det = gData->fTDCHitsToChannelMap.begin();
            i_det != gData->fTDCHitsToChannelMap.end();
            i_det++) {
      fNHitsPlots[i_det->first]->Fill(i_det->second.size());
      TH1F* h = fTimePlots[i_det->first];
      for (int i = 0; i < i_det->second.size(); ++i) {
        double time = i_det->second[i]*(1./40.96);
        h->Fill(time);
      }
    }
    return 0;
}

ALCAP_REGISTER_MODULE(PlotTDC_Time)
