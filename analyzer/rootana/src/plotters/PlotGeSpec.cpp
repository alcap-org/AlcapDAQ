//#define USE_PRINT_OUT 

#include "PlotGeSpec.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "RegisterModule.inc"
#include "definitions.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern StringAnalPulseMap gAnalysedPulseMap;

PlotGeSpec::PlotGeSpec(char *HistogramDirectoryName) :
  BaseModule(HistogramDirectoryName) 
{  
  fSlope = 1.21975e-1;
  fConstant = 5.06728e-1;

  std::string detname = "Ge-S";
  std::string histname = "GeS_energy";
  std::string histtitle = "Energy spectrum Ge-S";
  fGeS_spectrum = new TH1F(histname.c_str(), histtitle.c_str(), 
      2048,-0.5,2047.5);
  fGeS_spectrum->GetXaxis()->SetTitle("Amplitude [keV]");
  fGeS_spectrum->GetYaxis()->SetTitle("Counts/keV");

  histname = "GeS_Tdiff";
  histtitle = "Time diff between two Ge-S pulses";
  fGeS_Tdiff = new TH1F(histname.c_str(), histtitle.c_str(),
      2048, 0, 24.5e3);
  fGeS_Tdiff->SetBit(TH1::kCanRebin);
  fGeS_Tdiff->GetXaxis()->SetTitle("Time diff [ns]");
  fGeS_Tdiff->GetYaxis()->SetTitle("Counts");

  detname = "Ge-F";
  histtitle = "GeF_energy";
  histtitle = "Energy spectrum Ge-F";
  //int n_bits = gSetup->GetNBits(gSetup->GetBankName(detname));
  int n_bits = TSetupData::Instance()->GetNBits(TSetupData::Instance()->GetBankName(detname));
  double max_adc_value = std::pow(2, n_bits);
  fGeF_spectrum = new TH1F(histname.c_str(), histtitle.c_str(), 
      max_adc_value,0,max_adc_value);
  fGeF_spectrum->GetXaxis()->SetTitle("Amplitude [Adc]");
  fGeF_spectrum->GetYaxis()->SetTitle("Counts");

  dir->cd("/");
}

PlotGeSpec::PlotGeSpec(modules::options* opts) : BaseModule(opts->GetString("0").c_str()) {
  fSlope = 1.21975e-1;
  fConstant = 5.06728e-1;

  std::string detname = "Ge-S";
  std::string histname = "GeS_energy";
  std::string histtitle = "Energy spectrum Ge-S";
  fGeS_spectrum = new TH1F(histname.c_str(), histtitle.c_str(), 
      2048,-0.5,2047.5);
  fGeS_spectrum->GetXaxis()->SetTitle("Amplitude [keV]");
  fGeS_spectrum->GetYaxis()->SetTitle("Counts/keV");

  histname = "GeS_Tdiff";
  histtitle = "Time diff between two Ge-S pulses";
  fGeS_Tdiff = new TH1F(histname.c_str(), histtitle.c_str(),
      2048, 0, 24.5e3);
  fGeS_Tdiff->SetBit(TH1::kCanRebin);
  fGeS_Tdiff->GetXaxis()->SetTitle("Time diff [ns]");
  fGeS_Tdiff->GetYaxis()->SetTitle("Counts");

  detname = "Ge-F";
  histname = "GeF_energy";
  histtitle = "Energy spectrum Ge-F";
  int n_bits = 
    TSetupData::Instance()->GetNBits(TSetupData::Instance()->GetBankName(detname));
  double max_adc_value = std::pow(2, n_bits);
  fGeF_spectrum = new TH1F(histname.c_str(), histtitle.c_str(), 
      max_adc_value,0,max_adc_value);
  fGeF_spectrum->GetXaxis()->SetTitle("Amplitude [Adc]");
  fGeF_spectrum->GetYaxis()->SetTitle("Counts");

  dir->cd("/");
}

PlotGeSpec::~PlotGeSpec(){  
}

int PlotGeSpec::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){

  std::string fDetNameA  = "Ge-S";
  std::string fDetNameB = "Ge-F";
  AnalysedPulseList detA_pulses;
  AnalysedPulseList detB_pulses;

  if (gAnalysedPulseMap.find(fDetNameA) == gAnalysedPulseMap.end()) {
    if (Debug())
      std::cout << fDetNameA << " pulses not found" << std::endl;
  }
  else {
    detA_pulses = gAnalysedPulseMap[fDetNameA];
  }

  if (gAnalysedPulseMap.find(fDetNameB) == gAnalysedPulseMap.end()) {
    if (Debug())
      std::cout << fDetNameB << " pulses not found" << std::endl;
  }
  else {
    detB_pulses = gAnalysedPulseMap[fDetNameB];
  }

  for (AnalysedPulseList::iterator detAPulseIter = detA_pulses.begin(); 
      detAPulseIter != detA_pulses.end(); ++detAPulseIter)
  {
    fGeS_spectrum->Fill(Adc2keV((*detAPulseIter)->GetAmplitude()));
    if (detA_pulses.size()>2)
    {
      if (detAPulseIter+1!=detA_pulses.end())
      {
        double t0 = (*detAPulseIter)->GetTime();
        double t1 = (*(detAPulseIter+1))->GetTime();
        fGeS_Tdiff->Fill(t1-t0);
      }
    }
  }

  for (AnalysedPulseList::iterator detBPulseIter = detB_pulses.begin(); 
      detBPulseIter != detB_pulses.end(); ++detBPulseIter)
  {
    fGeF_spectrum->Fill((*detBPulseIter)->GetAmplitude());
  }


  return 0;
}

ALCAP_REGISTER_MODULE(PlotGeSpec)
