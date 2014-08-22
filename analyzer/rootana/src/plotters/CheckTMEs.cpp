#include "CheckTMEs.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TMuonEvent.h"

#include <TH1F.h>
#include <TH2F.h>

#include <iostream>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

CheckTMEs::CheckTMEs(modules::options* opts):
   BaseModule("CheckTMEs",opts){
}

CheckTMEs::~CheckTMEs(){
}

int CheckTMEs::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    using namespace IDs;
    fDetectors.push_back(IDs::channel (kGe     , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kScR    , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kScL    , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kScVe   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kScGe   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_1 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_2 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_3 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL1_4 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_1 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_2 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_3 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR1_4 , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiR2   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kSiL2   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kMuSc   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kMuScA  , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kNDet   , kNotApplicable ));
    fDetectors.push_back(IDs::channel (kNDet2  , kNotApplicable ));

    // Plot the total number of pulses
    fTotalPulses=new TH1F("hTotalPulses", "Total number of pulses per TME", 100, 0 ,100);
    fTotalPulses->SetXTitle("Number of pulses");

    // Plot the number of pulses per channel
    fPulsesPerDetector=new TH2F("hPulsesPerChannel", "Pulses per channel per TME", 
            100, 0 ,100,fDetectors.size(),0,fDetectors.size());
    fPulsesPerDetector->SetXTitle("Number of pulses");
    fPulsesPerDetector->SetYTitle("Detector");
    for(DetectorList::const_iterator i_det=fDetectors.begin();
            i_det!=fDetectors.end(); ++i_det){
        fPulsesPerDetector->GetYaxis()->SetBinLabel(i_det-fDetectors.begin()+1, i_det->str().c_str());
    }

  return 0;
}

int CheckTMEs::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
    // Loop over each TME
    for(MuonEventList::const_iterator i_tme=gMuonEvents.begin();
            i_tme!=gMuonEvents.end(); ++i_tme){
        // total pulses
        fTotalPulses->Fill((*i_tme)->TotalNumPulses());
        for(DetectorList::const_iterator i_det=fDetectors.begin();
                i_det!=fDetectors.end(); ++i_det){
            // pulses per channel
            fPulsesPerDetector->Fill((*i_tme)->NumPulses(*i_det), i_det - fDetectors.begin());
        }
    }
  return 0;
}

int CheckTMEs::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

ALCAP_REGISTER_MODULE(CheckTMEs);
