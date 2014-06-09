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

TAnalysedPulse::TAnalysedPulse()
{
  Reset();
}

TAnalysedPulse::TAnalysedPulse(double amplitude, double time, double integral, double energy, std::string det_name)
{
  Reset();
  fAmplitude = amplitude;
  fTime = time;
  fIntegral = integral;
  fEnergy = energy;
  fDetName = det_name;
  fAlgo = "";
}

TAnalysedPulse::TAnalysedPulse(double amplitude, double time, double integral, 
		double energy, std::string det_name, std::string algo)
{
  Reset();
  fAmplitude = amplitude;
  fTime = time;
  fIntegral = integral;
  fEnergy = energy;
  fDetName = det_name;
  fAlgo = algo;
}

void TAnalysedPulse::Reset(Option_t* o)
{
  fAmplitude = 0;
  fTime = 0;
  fIntegral = 0;
  fEnergy = 0.;
  fDetName = "";
  fAlgo = "";
}

void TAnalysedPulse::Draw(const TH1F* tpi_pulse)const{
	std::cout<<"Plotting TAP";
	if(tpi_pulse) {
	  std::string name=tpi_pulse->GetName();
	  std::cout<<" corresponding to TPI: "<<name;
	  int n_bins=tpi_pulse->GetXaxis()->GetNbins();
	  double x_max=tpi_pulse->GetXaxis()->GetXmax();
	  double x_min=tpi_pulse->GetXaxis()->GetXmin();
	  TH1F* tap_pulse=new TH1F((name+"_AP").c_str(),("MaxBin TAP for "+name).c_str(),n_bins,x_min,x_max);
	  int bin=tap_pulse->FindBin(fTime);
	  PrintValue(bin);
	  PrintValue(fAmplitude);
	  PrintValue(fTime);
	  tap_pulse->SetBinContent(bin,fAmplitude);
	}
	std::cout<<std::endl;
	//name+="_Ana
	//TH1F* tap_pulse=new TH1F(tpi
}
