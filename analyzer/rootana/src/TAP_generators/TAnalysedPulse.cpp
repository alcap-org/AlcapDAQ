#include "TAnalysedPulse.h"

#include <TH1F.h>
#include <cmath>
#include <sstream>
#include <string>
#include <iostream>

using std::vector;
using std::string;

#define PrintHelp std::cout<<__FILE__<<":"<<__LINE__<<": "
#define PrintValue(value) PrintHelp<<#value "= |"<<value<<"|"<<std::endl;

void TAnalysedPulse::Reset(Option_t* o)
{
  fParentTPI=fDefaultValue;
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
	  TH1F* tap_pulse=new TH1F((name+"_AP").c_str(),("MaxBin TAP for "+name).c_str(),n_bins,x_min,x_max);
	  int bin=tap_pulse->FindBin(fTime);
	  tap_pulse->SetBinContent(bin,fAmplitude);
	}
}
