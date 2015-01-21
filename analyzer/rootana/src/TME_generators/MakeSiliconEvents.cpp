#include "MakeSiliconEvents.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TSiliconEvent.h"
#include "TMuonEvent.h"

#include <iostream>
using std::cout;
using std::endl;

extern SourceDetPulseMap gDetectorPulseMap;
extern MuonEventList gMuonEvents;

MakeSiliconEvents::MakeSiliconEvents(modules::options* opts):
   BaseModule("MakeSiliconEvents",opts,false),
  fSiWindow(opts->GetDouble("si_window",100)){
}

MakeSiliconEvents::~MakeSiliconEvents(){
}

int MakeSiliconEvents::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
  // Get all the sources for the silicon channels
  for(int i=0;i<LR::kNum;i++){
     for(int ch=0;ch<Ch::kNum;ch++){
        fSourceList[i][ch]=GetTDPSource(Form("Si%s%s",LR::str(i),Ch::str(ch)));
     }
  }
  return 0;
}

int MakeSiliconEvents::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
  
  // loop over each TME
  TSiliconEvent si_hit;
  for(MuonEventList::iterator i_tme=gMuonEvents.begin();
          i_tme!=gMuonEvents.end(); ++i_tme){
    TMuonEvent* tme=*i_tme;
    // loop over both Si packages
    for(int i_side=0;i_side <2; ++i_side){
       // Loop over the thick silicon
       for(DetectorPulseList::const_iterator i_Si2=tme->BeginPulses(fSourceList[i_side][Ch::k2]);
             i_Si2!=tme->EndPulses(fSourceList[i_side][Ch::k2]); ++i_Si2){
           si_hit.Reset();
           double thick_time=(*i_Si2)->GetTime();
           double E_thick=(*i_Si2)->GetEnergy(TDetectorPulse::kSlow);
           si_hit.SetThick(E_thick,thick_time);
           for(int i=0; i<4;++i){
              for(DetectorPulseList::const_iterator i_thin=tme->BeginPulses(fSourceList[i_side][i]);
             i_thin!=tme->EndPulses(fSourceList[i_side][i]); ++i_thin){
                 const TDetectorPulse* pulse=*i_thin;
                 if(!pulse->IsGood())continue;
                 double time=pulse->GetTime(TDetectorPulse::kFast);
                 if( time > thick_time +fSiWindow) break;
                 if( time < thick_time -fSiWindow) continue;
                 si_hit.AddThin(i+1,pulse->GetEnergy(TDetectorPulse::kSlow));
              }
           }
           tme->InsertSiliconEvent((TMuonEvent::LeftRight_t) i_side,si_hit);
       }
    } // loop over both Si packages
  } // loop over each TME
  return 0;
}

int MakeSiliconEvents::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

const IDs::source& MakeSiliconEvents::GetTDPSource(const std::string& ch ){
    IDs::channel chan(ch);
    SourceDetPulseMap::const_iterator i_so=gDetectorPulseMap.begin();
    while(i_so!=gDetectorPulseMap.end() && !(i_so->first==chan)) ++i_so;
    if(i_so==gDetectorPulseMap.end()){
        static IDs::source invalid(IDs::kErrorDetector, IDs::kErrorSlowFast, "","");
        return invalid;
    }
    return i_so->first;
}

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



ALCAP_REGISTER_MODULE(MakeSiliconEvents, si_window);
