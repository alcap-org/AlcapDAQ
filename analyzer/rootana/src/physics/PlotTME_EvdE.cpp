#include "PlotTME_EvdE.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TMuonEvent.h"
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
   BaseModule("PlotTME_EvdE",opts),
   fSiWindow(opts->GetDouble("si_window",100)){
}

PlotTME_EvdE::~PlotTME_EvdE(){
}

int PlotTME_EvdE::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

    // for left and right packages
    for(int i=0;i<LR::kNum;i++){
        // Get the source IDs for all necessary channels
        for(int ch=0;ch<Ch::kNum;ch++){
            fSourceList[i].sources[ch]=GetTDPSource(Form("Si%s%s",LR::str(i),Ch::str(ch)));
        }

        /******** Create all plots ********/
        // E vs dE without muon pile-up
        fSourceList[i].E_vs_dE_no_pileUp=new TH2F(
                Form("hE_vs_dE_no_pileUp_%s",LR::str(i,1)),
                Form("Pile-up protected E vs dE for Si%s",LR::str(i)),
                400,0,8000,400,0,8000);
        fSourceList[i].E_vs_dE_no_pileUp->SetXTitle("Amplitude");
        fSourceList[i].E_vs_dE_no_pileUp->SetYTitle("Amplitude");

        // E vs dE including muon pile-up
        fSourceList[i].E_vs_dE_with_pileUp=new TH2F(
                Form("hE_vs_dE_with_pileUp%s",LR::str(i,1)),
                Form("E vs dE (without pile-up protection) for Si%s",LR::str(i)),
                400,0,8000,400,0,8000);
                //400,0,2500,400,0,0.1);
        fSourceList[i].E_vs_dE_with_pileUp->SetXTitle("E + dE (KeV)");
        fSourceList[i].E_vs_dE_with_pileUp->SetYTitle("E (keV)");
    }

  return 0;
}

int PlotTME_EvdE::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
    DetectorPulseList::const_iterator begin[Ch::kNum];
    DetectorPulseList::const_iterator end[Ch::kNum];
    DetectorPulseList::const_iterator i_thin[4];

    // for each TME
    for(MuonEventList::const_iterator i_tme=gMuonEvents.begin();
            i_tme!=gMuonEvents.end(); ++i_tme){
        const TMuonEvent* tme=*i_tme;
        // for each Si packet
        for(int side=0;side<LR::kNum;++side){
            // setup limits
            for(int i_ch=0;i_ch<Ch::kNum;++i_ch){
                begin[i_ch]=tme->BeginPulses(fSourceList[side].sources[i_ch]);
                end[i_ch]=tme->EndPulses(fSourceList[side].sources[i_ch]);
                if(i_ch<4) i_thin[i_ch]=begin[i_ch];
            }

            // For each SiR2 hit
            for(DetectorPulseList::const_iterator i_Si2=begin[Ch::k2]; i_Si2!=end[Ch::k2]; ++i_Si2){
                double time_start=(*i_Si2)->GetTime() - fSiWindow;
                double time_stop=(*i_Si2)->GetTime() + fSiWindow;
                double E_thick=(*i_Si2)->GetEnergy(TDetectorPulse::kSlow);
                for(int i=0; i<4;++i) n_thin[i]=0;  // reset the thin pulse counters

                // Add all correlated thin hits together
                double dE=0;
                for(int i=0; i<4;++i){
                    for(;i_thin[i]!=end[i] ; ++i_thin[i]){
                        const TDetectorPulse* pulse=*i_thin[i];
                        if(!pulse->IsGood())continue;
                        double time=pulse->GetTime(TDetectorPulse::kFast);
                        if( time > time_stop) break;
                        if( time < time_start) continue;
                        dE+=pulse->GetEnergy(TDetectorPulse::kSlow);
                    }
                }

                // Fill histograms
                if(!tme->HasMuonPileup()){
                    fSourceList[side].E_vs_dE_no_pileUp->Fill(E_thick+dE,dE);
                }
                fSourceList[side].E_vs_dE_with_pileUp->Fill(E_thick+dE,dE);

            } // each SiR2 hit
        } // each side
    } // each TME
  return 0;
}

int PlotTME_EvdE::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  if(Debug()){
    for(int side=0;side<LR::kNum;++side){
      cout<<"PlotTME_EvdE: E_vs_dE_no_pileUp "<< LR::str(side) <<" has "<<fSourceList[side].E_vs_dE_no_pileUp->GetEntries()<<endl;
      cout<<"PlotTME_EvdE: E_vs_dE_with_pileUp "<< LR::str(side) <<" has "<<fSourceList[side].E_vs_dE_with_pileUp->GetEntries()<<endl;
    }
  }
  return 0;
}

const IDs::source& PlotTME_EvdE::GetTDPSource(const std::string& ch ){
    IDs::channel chan(ch);
    SourceDetPulseMap::const_iterator i_so=gDetectorPulseMap.begin();
    while(i_so!=gDetectorPulseMap.end() && !(i_so->first==chan)) ++i_so;
    if(i_so==gDetectorPulseMap.end()){
        static IDs::source invalid(IDs::kErrorDetector, IDs::kErrorSlowFast, "","");
        return invalid;
    }
    return i_so->first;
}
ALCAP_REGISTER_MODULE(PlotTME_EvdE);
