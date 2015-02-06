#include "ActiveSiRMuStopAlgo.h"
#include "IdSource.h"
#include "TMuonEvent.h"

TMEAlgorithm::ActiveSiRStop::ActiveSiRStop(const IDs::source& sir2, double muSc_min, double muSc_max, double SiR2_min, double SiR2_max):
     fMuScMax(muSc_max),fMuScMin(muSc_min),  fSiR2Max(SiR2_max),fSiR2Min(SiR2_min),fSiR2source(sir2){
}


bool TMEAlgorithm::ActiveSiRStop::operator()(const TMuonEvent* tme, TDetectorPulse::ParentChannel_t channel)const{
  const double muSc_amp=tme->GetCentralMuon()->GetAmplitude(TDetectorPulse::kFast);
  bool ret_val=false;
  if(muSc_amp>fMuScMin && muSc_amp< fMuScMax){
    const int N_sir2=tme->NumPulses(fSiR2source);
    for(int i=0; i< N_sir2; ++i){
      const double sir2_amp=tme->GetPulse(fSiR2source,i)->GetAmplitude(channel);
      if(sir2_amp > fSiR2Min && sir2_amp < fSiR2Max){
         ret_val=true;
         break;
      }
    }
  }
  return ret_val;
}
