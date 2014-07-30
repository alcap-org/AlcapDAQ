#include "TAnalysedPulse.h"
#include "TPulseIsland.h"

#include <TH1F.h>
#include <cmath>
#include <sstream>
#include <string>
#include <iostream>

using std::vector;
using std::string;

#define PrintHelp std::cout<<__FILE__<<":"<<__LINE__<<": "
#define PrintValue(value) PrintHelp<<#value "= |"<<value<<"|"<<std::endl;

TAnalysedPulse::TAnalysedPulse(const IDs::source& sourceID,
             const TPulseIslandID& parentID, const TPulseIsland* parentTPI):
  fParentID(fDefaultValue),
  fTPILength(fDefaultValue),
  fAmplitude(fDefaultValue),
  fTime(fDefaultValue),
  fIntegral(fDefaultValue),
  fEnergy(fDefaultValue),
  fPedestal(fDefaultValue),
  fTriggerTime(fDefaultValue){
    SetSource(sourceID);
    SetParentTPIProperties(parentID, parentTPI);
}

TAnalysedPulse::TAnalysedPulse():
  fParentID(fDefaultValue),
  fTPILength(fDefaultValue),
  fAmplitude(fDefaultValue),
  fTime(fDefaultValue),
  fIntegral(fDefaultValue),
  fEnergy(fDefaultValue),
  fPedestal(fDefaultValue),
  fTriggerTime(fDefaultValue){
    //Reset();
}

void TAnalysedPulse::Reset(Option_t* o) {
  fParentID=fDefaultValue;
  fTPILength=fDefaultValue;
  fAmplitude=fDefaultValue;
  fTime=fDefaultValue;
  fIntegral=fDefaultValue;
  fEnergy=fDefaultValue;
  fPedestal=fDefaultValue;
  fTriggerTime=fDefaultValue;
}

void TAnalysedPulse::Draw(const TH1F* tpi_pulse)const{
	if(tpi_pulse) {
	  std::string name=tpi_pulse->GetName();
	  int n_bins=tpi_pulse->GetXaxis()->GetNbins();
	  double x_max=tpi_pulse->GetXaxis()->GetXmax();
	  double x_min=tpi_pulse->GetXaxis()->GetXmin();
	  TH1F* tap_pulse=new TH1F((name+"_AP").c_str(),("TAP for "+name).c_str(),n_bins,x_min,x_max);
	  int bin=tap_pulse->FindBin(fTime);
	  tap_pulse->SetBinContent(bin,fAmplitude);
	}
}

void TAnalysedPulse::SetParentTPIProperties(const TPulseIslandID& id,
                                            const TPulseIsland* pulse){
  SetParentID(id);
  if(!pulse) {
    std::cerr<<"NULL pointer to TPulseIsland passed as parent for TAnalysedPulse."<<std::endl;
    return;
  }
  SetTPILength(pulse->GetPulseLength());
  SetTriggerTime(pulse->GetTimeStamp()*pulse->GetClockTickInNs());
}
