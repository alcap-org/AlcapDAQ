#include "PlotTME_EvdE.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "debug_tools.h"

#include <TH2F.h>
#include <TH1F.h>

#include <iostream>
#include <algorithm>
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;
extern const SourceDetPulseMap gDetectorPulseMap;

const char* Ch::str(Ch::Type e){
    switch(e){
        case Ch::k1_1          : return "1_1";
        case Ch::k1_2          : return "1_2";
        case Ch::k1_3          : return "1_3";
        case Ch::k1_4          : return "1_4";
        case Ch::k2            : return "2";
        case Ch::kNum : return "NumThinThick";
    }
    return "-------";
}

const char* LR::str(LR::Type e, bool big){
    switch(e){
        case LR::kLeft:  return big?"Left":"L";
        case LR::kRight: return big?"Right":"R";
        case LR::kNum:   return "NumLR";
    }
    return "-------";
}

PlotTME_EvdE::PlotTME_EvdE(modules::options* opts):
   BaseModule("PlotTME_EvdE",opts){
}

PlotTME_EvdE::~PlotTME_EvdE(){
}

int PlotTME_EvdE::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

    // for left and right packages
    for(int i=0;i<LR::kNum;i++){
        // Get the source IDs for all necessary channels
        for(int ch=0;ch<Ch::kNum;ch++){
            DEBUG_VALUE(ch);
            fSourceList[i].sources[ch]=GetTDPSource(Form("Si%s%s",LR::str(i),Ch::str(ch)));
        }

        /******** Create all plots ********/
        // E vs dE without muon pile-up
        fSourceList[i].E_vs_dE_no_pileUp=new TH2F(
                Form("hE_vs_dE_no_pileUp_%s",LR::str(i,1)),
                Form("Pile-up protected E vs dE for Si%s",LR::str(i)),
                400,0,-1,400,0,-1);
        fSourceList[i].E_vs_dE_no_pileUp->SetXTitle("Amplitude");
        fSourceList[i].E_vs_dE_no_pileUp->SetYTitle("Amplitude");

        // E vs dE including muon pile-up
        fSourceList[i].E_vs_dE_with_pileUp=new TH2F(
                Form("hE_vs_dE_with_pileUp%s",LR::str(i,1)),
                Form("E vs dE (without pile-up protection) for Si%s",LR::str(i)),
                400,0,-1,400,0,-1);
        fSourceList[i].E_vs_dE_with_pileUp->SetXTitle("E + dE (KeV)");
        fSourceList[i].E_vs_dE_with_pileUp->SetYTitle("E (keV)");
    }

  return 0;
}

int PlotTME_EvdE::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
    for(MuonEventList::const_iterator i_tme=gMuonEvents.begin();
            i_tme!=gMuonEvents.end(); ++i_tme){
        // For each SiR2 hit
        // Add all correlated SiR1 hits together
        // Fill histograms
    }
  return 0;
}

int PlotTME_EvdE::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

const IDs::source& PlotTME_EvdE::GetTDPSource(const std::string& ch ){
    IDs::channel chan(ch);
    DEBUG_VALUE(ch);
    SourceDetPulseMap::const_iterator i_so=gDetectorPulseMap.begin();
    while(i_so!=gDetectorPulseMap.end() && !(i_so->first==chan)) ++i_so;
    if(i_so==gDetectorPulseMap.end()){
        static IDs::source invalid(IDs::kErrorDetector, IDs::kErrorSlowFast, "","");
        return invalid;
    }
    return i_so->first;
}
ALCAP_REGISTER_MODULE(PlotTME_EvdE);
