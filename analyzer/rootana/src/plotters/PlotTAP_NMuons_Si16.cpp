#include "PlotTAP_NMuons_Si16.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include "TAnalysedPulse.h"
#include "SetupNavigator.h"

#include "TCanvas.h"
#include "TLine.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cmath>
using std::cout;
using std::endl;

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotTAP_NMuons_Si16::PlotTAP_NMuons_Si16(modules::options* opts):
   BaseModule("PlotTAP_NMuons_Si16",opts){

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?
  // For example, perhaps this module wants an axis range:
  fADCCutLow=opts->GetDouble("adc_cut_low",200); 
  fADCCutHigh=opts->GetDouble("adc_cut_high",2500); 
  
}

PlotTAP_NMuons_Si16::~PlotTAP_NMuons_Si16(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int PlotTAP_NMuons_Si16::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotTAP_NMuons_Si16::BeforeFirstEntry(): I'm debugging!"<<endl;
  }

  std::string histname = "hNMuonsPerStrip";
  std::stringstream histtitle;
  histtitle<<"Number of Muons per Strip (" << fADCCutLow << " ADC < Amplitude < " << fADCCutHigh << ")";
  histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
  fNMuonsPerStrip = new TH1F(histname.c_str(), histtitle.str().c_str(), 16,1,17);
  fNMuonsPerStrip->GetXaxis()->SetTitle("Strip Number");
  fNMuonsPerStrip->GetYaxis()->SetTitle("N Muons");

  histname = "hSi16TAPAmplitudes";
  histtitle.str("");
  histtitle<<"TAP Amplitude Distributions for each strip";
  histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
  IDs::channel an_si16_channel = IDs::channel(IDs::kSiL1_1, IDs::kSlow);
  int n_bits = setup->GetNBits(setup->GetBankName(an_si16_channel.str()));
  double max_adc_value = std::pow(2, n_bits);
  fSi16TAPAmplitudes = new TH2F(histname.c_str(), histtitle.str().c_str(), 16,1,17, max_adc_value,0,max_adc_value);
  fSi16TAPAmplitudes->GetXaxis()->SetTitle("Strip Number");
  fSi16TAPAmplitudes->GetYaxis()->SetTitle("TAP Amplitude [ADC]");
  fSi16TAPAmplitudes->SetStats(false);

  histname = "hSi16TAPEnergies";
  histtitle.str("");
  histtitle<<"TAP Energy Distributions for each strip";
  histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
  fSi16TAPEnergies = new TH2F(histname.c_str(), histtitle.str().c_str(), 16,1,17, 1200,0,12000);
  fSi16TAPEnergies->GetXaxis()->SetTitle("Strip Number");
  fSi16TAPEnergies->GetYaxis()->SetTitle("TAP Energy [keV]");
  fSi16TAPEnergies->SetStats(false);
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotTAP_NMuons_Si16::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

    // Loop over each TAP list
    for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
	 i_det != gAnalysedPulseMap.end();
	 i_det++) {

      IDs::source i_source = i_det->first;
      const std::string& detname = i_source.str();
      std::string keyname = i_det->first.str() + GetName();

      // Check that this is one of the SiL1 channels
      std::string search_string = "SiL1";
      std::size_t location = detname.find(search_string);

      if (location != std::string::npos && i_source.isSlow()) {
	const AnalysedPulseList *pulses = &i_det->second;
	
	std::size_t first_hyphen = detname.find("-");
	std::size_t second_hyphen = detname.find("-", first_hyphen+1);
	std::string strip_number_str = detname.substr(first_hyphen+1, second_hyphen-first_hyphen-1);
	int strip_number = atoi(strip_number_str.c_str());
        //if(Debug() && pulses->empty()) DEBUG_PREFIX<<" no pulses to fill for "<<i_det->first<<std::endl;

        for (AnalysedPulseList::const_iterator pulseIter = pulses->begin(); pulseIter != pulses->end(); ++pulseIter) {
	  double amplitude = (*pulseIter)->GetAmplitude();
	  double energy = (*pulseIter)->GetEnergy();
	  fSi16TAPAmplitudes->Fill(strip_number, amplitude);
	  fSi16TAPEnergies->Fill(strip_number, energy);
	  if (amplitude > fADCCutLow && amplitude < fADCCutHigh) {
	    fNMuonsPerStrip->Fill(strip_number);
	  }
        } // end loop through pulses
      }
    } // end loop through detectors

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PlotTAP_NMuons_Si16::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotTAP_NMuons_Si16::AfterLastEntry(): I'm debugging!"<<endl;
  }

  TCanvas* c = new TCanvas("c", "c");
  c->SetLogz();
  fSi16TAPAmplitudes->Draw("COLZ");
  TLine* low_cut_line = new TLine(fSi16TAPAmplitudes->GetXaxis()->GetXmin(), fADCCutLow, fSi16TAPAmplitudes->GetXaxis()->GetXmax(), fADCCutLow);
  low_cut_line->SetLineWidth(2);
  low_cut_line->SetLineColor(kRed);
  low_cut_line->SetLineStyle(kDashed);
  low_cut_line->Draw("LSAME");

  TLine* high_cut_line = new TLine(fSi16TAPAmplitudes->GetXaxis()->GetXmin(), fADCCutHigh, fSi16TAPAmplitudes->GetXaxis()->GetXmax(), fADCCutHigh);
  high_cut_line->SetLineWidth(2);
  high_cut_line->SetLineColor(kRed);
  high_cut_line->SetLineStyle(kDashed);
  high_cut_line->Draw("LSAME");

  c->Write();

  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PlotTAP_NMuons_Si16,adc_cut_low,adc_cut_high);
