//#define USE_PRINT_OUT 

#include "PlotTAP_StoppedMuonDiscrepancy.h"
#include <iostream>
#include <string>
#include <sstream>
#include <map>
//#include <utility>
//#include <algorithm>
#include <cmath>

#include "TAnalysedPulse.h"
//#include "TDetectorPulse.h"
#include "RegisterModule.inc"
#include "definitions.h"
#include "SetupNavigator.h"
//#include "debug_tools.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotTAP_StoppedMuonDiscrepancy::PlotTAP_StoppedMuonDiscrepancy(modules::options* opts) : 
  BaseModule("PlotTAP_StoppedMuonDiscrepancy",opts),
  fCoincidenceTimeLow_SiL1_GeLoGain(opts->GetDouble("coinc_time_low_sil1_gelo")),
  fCoincidenceTimeHigh_SiL1_GeLoGain(opts->GetDouble("coinc_time_high_sil1_gelo")),
  fCoincidenceTimeLow_SiL3_GeLoGain(opts->GetDouble("coinc_time_low_sil3_gelo")),
  fCoincidenceTimeHigh_SiL3_GeLoGain(opts->GetDouble("coinc_time_high_sil3_gelo")),
  fCoincidenceTimeLow_SiL3_SiL1(opts->GetDouble("coinc_time_low_sil3_sil1")),
  fCoincidenceTimeHigh_SiL3_SiL1(opts->GetDouble("coinc_time_high_sil3_sil1"))
{

    }

PlotTAP_StoppedMuonDiscrepancy::~PlotTAP_StoppedMuonDiscrepancy(){  
}

int PlotTAP_StoppedMuonDiscrepancy::BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup){
  
  //  fSiL1.push_back(IDs::channel (IDs::kSiL1_1 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_2 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_3 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_4 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_5 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_6 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_7 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_8 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_9 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_10 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_11 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_12 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_13 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_14 , IDs::kSlow ));
  fSiL1.push_back(IDs::channel (IDs::kSiL1_15 , IDs::kSlow ));
  //  fSiL1.push_back(IDs::channel (IDs::kSiL1_16 , IDs::kSlow ));
  fSiL3 = new IDs::channel (IDs::kSiL3   , IDs::kSlow );
  fGeLoGain = new IDs::channel (IDs::kGeLoGain   , IDs::kNotApplicable );

  // Book the histograms
  // hEnergy_NoCuts
  std::string histname = "hSiL1_Energy_NoCuts";
  std::stringstream histtitle;
  histtitle<<"Energy of pulses from source SiL1";
  histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
  histtitle <<" (no cuts)";
  fSiL1_EnergyPlot_NoCuts = new TH1F(histname.c_str(), histtitle.str().c_str(), 1800,0,18000);
  fSiL1_EnergyPlot_NoCuts->GetXaxis()->SetTitle("Energy [keV]");
  fSiL1_EnergyPlot_NoCuts->GetYaxis()->SetTitle("Arbitrary Units");

  histname = "hSiL3_Energy_NoCuts";
  fSiL3_EnergyPlot_NoCuts = (TH1F*) fSiL1_EnergyPlot_NoCuts->Clone(histname.c_str());
  histtitle.str("");
  histtitle<<"Energy of pulses from source SiL3";
  histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
  histtitle <<" (no cuts)";
  fSiL3_EnergyPlot_NoCuts->GetXaxis()->SetTitle("Energy [keV]");
  fSiL3_EnergyPlot_NoCuts->GetYaxis()->SetTitle("Arbitrary Units");

  histname = "hGeLoGain_Energy_NoCuts";
  fGeLoGain_EnergyPlot_NoCuts = (TH1F*) fSiL1_EnergyPlot_NoCuts->Clone(histname.c_str());
  histtitle.str("");
  histtitle<<"Energy of pulses from source GeLoGain";
  histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
  histtitle <<" (no cuts)";
  fGeLoGain_EnergyPlot_NoCuts->GetXaxis()->SetTitle("Energy [keV]");
  fGeLoGain_EnergyPlot_NoCuts->GetYaxis()->SetTitle("Arbitrary Units");


  fSiL3_EnergyPlot_wSiL1Coincidence = (TH1F*) fSiL1_EnergyPlot_NoCuts->Clone("hSiL3_Energy_wSiL1Coincidence");
  histtitle.str("");
  histtitle<<"Energy of pulses from SiL3";
  histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
  histtitle <<" (with SiL1 coincidence)";
  fSiL3_EnergyPlot_wSiL1Coincidence->SetTitle(histtitle.str().c_str());   


  histname = "hSiL1_Energy_wNoSiL3Coincidence";
  fSiL1_EnergyPlot_wNoSiL3Coincidence = (TH1F*) fSiL1_EnergyPlot_NoCuts->Clone(histname.c_str());
  histtitle.str("");
  histtitle<<"Energy of pulses from SiL1";
  histtitle<<" for run "<<SetupNavigator::Instance()->GetRunNumber();
  histtitle <<" (with no SiL3 coincidence)";
  fSiL1_EnergyPlot_wNoSiL3Coincidence->SetTitle(histtitle.str().c_str());

  histname = "hGeLoGain_SiL3_CoincidentEnergies";
  fSiL3_GeLoGain_CoincidentEnergies = new TH2F(histname.c_str(), "", 1800,0,18000, 18000,0,18000);
  fSiL3_GeLoGain_CoincidentEnergies->SetXTitle("SiL3 Energy");
  fSiL3_GeLoGain_CoincidentEnergies->SetYTitle("GeLoGain Energy");

  histname = "hGeLoGain_SiL1_CoincidentEnergies";
  fSiL1_GeLoGain_CoincidentEnergies = new TH2F(histname.c_str(), "", 1800,0,18000, 18000,0,18000);
  fSiL1_GeLoGain_CoincidentEnergies->SetXTitle("SiL1 Energy");
  fSiL1_GeLoGain_CoincidentEnergies->SetYTitle("GeLoGain Energy");

  histname = "hSiL3_SiL1_TDiff";
  fSiL3_SiL1_TDiff = new TH1F(histname.c_str(), "", 200,0,2e3);
  fSiL3_SiL1_TDiff->SetXTitle("|t_{SiL3, no cuts} - t_{SiL1, no cuts}| [ns]");

  histname = "hSiL3_GeLoGain_TDiff";
  fSiL3_GeLoGain_TDiff = new TH1F(histname.c_str(), "", 1000,1e4,2e4);
  fSiL3_GeLoGain_TDiff->SetXTitle("|t_{SiL3, w/SiL1 coinc} - t_{GeLoGain, no cuts}| [ns]");

  histname = "hSiL1_GeLoGain_TDiff";
  fSiL1_GeLoGain_TDiff = new TH1F(histname.c_str(), "", 1000,1e4,2e4);
  fSiL1_GeLoGain_TDiff->SetXTitle("|t_{SiL1, w/o SiL3 coinc} - t_{GeLoGain, no cuts}| [ns]");

  fCuts = new TH1F("hCuts", "Coincidence Cuts Used",6,0,6);
  fCuts->GetXaxis()->SetBinLabel(1, "SiL3-SiL1 (Low)");
  fCuts->GetXaxis()->SetBinLabel(2, "SiL3-SiL1 (High)");
  fCuts->GetXaxis()->SetBinLabel(3, "SiL3-GeLoGain (Low)");
  fCuts->GetXaxis()->SetBinLabel(4, "SiL3-GeLoGain (High)");
  fCuts->GetXaxis()->SetBinLabel(5, "SiL1-GeLoGain (Low)");
  fCuts->GetXaxis()->SetBinLabel(6, "SiL1-GeLoGain (High)");
  fCuts->SetBinContent(1, fCoincidenceTimeLow_SiL3_SiL1);
  fCuts->SetBinContent(2, fCoincidenceTimeHigh_SiL3_SiL1);
  fCuts->SetBinContent(3, fCoincidenceTimeLow_SiL3_GeLoGain);
  fCuts->SetBinContent(4, fCoincidenceTimeHigh_SiL3_GeLoGain);
  fCuts->SetBinContent(5, fCoincidenceTimeLow_SiL1_GeLoGain);
  fCuts->SetBinContent(6, fCoincidenceTimeHigh_SiL1_GeLoGain);

  return 0;
}

int PlotTAP_StoppedMuonDiscrepancy::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){

  // Get all the pulses I want first
  std::vector<const AnalysedPulseList*> SiL1_pulse_lists;
  const AnalysedPulseList* SiL3_pulse_list;
  const AnalysedPulseList* GeLoGain_pulse_list;
  for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
       i_det != gAnalysedPulseMap.end();
       i_det++) {
    
    const std::string& detname = i_det->first.str();

    // see if this channel is one of the SiL1 channels
    IDs::channel ch = i_det->first.Channel();
    for (DetectorList::const_iterator i_SiL1_ch = fSiL1.begin(); i_SiL1_ch != fSiL1.end(); ++i_SiL1_ch) {
      if (ch == *i_SiL1_ch) {
	SiL1_pulse_lists.push_back(&i_det->second);
	break;
      }
    }
    if (ch == *fSiL3) {
      SiL3_pulse_list = &i_det->second;
    }
    else if (ch == *fGeLoGain) {
      GeLoGain_pulse_list = &i_det->second;
    }
  }

  // Plot the energies without any cuts
  for (std::vector<const AnalysedPulseList*>::const_iterator i_SiL1_pulse_list = SiL1_pulse_lists.begin(); i_SiL1_pulse_list != SiL1_pulse_lists.end(); ++i_SiL1_pulse_list) {
    //    std::cout << "AE: " << (*i_SiL1_pulse_list)->size() << std::endl;
    std::stringstream keyname;
    keyname << "SiL1-" << i_SiL1_pulse_list - SiL1_pulse_lists.begin() + 1 << "-S";

    for (AnalysedPulseList::const_iterator i_SiL1_pulse = (*i_SiL1_pulse_list)->begin(); i_SiL1_pulse != (*i_SiL1_pulse_list)->end(); ++i_SiL1_pulse) {
      
      double SiL1_energy = (*i_SiL1_pulse)->GetEnergy();
      fSiL1_EnergyPlot_NoCuts->Fill(SiL1_energy);
    }
  }
  for (AnalysedPulseList::const_iterator i_SiL3_pulse = SiL3_pulse_list->begin(); i_SiL3_pulse != SiL3_pulse_list->end(); ++i_SiL3_pulse) {
    
    double SiL3_energy = (*i_SiL3_pulse)->GetEnergy();
    fSiL3_EnergyPlot_NoCuts->Fill(SiL3_energy);
  }
  for (AnalysedPulseList::const_iterator i_GeLoGain_pulse = GeLoGain_pulse_list->begin(); i_GeLoGain_pulse != GeLoGain_pulse_list->end(); ++i_GeLoGain_pulse) {
    
    double GeLoGain_energy = (*i_GeLoGain_pulse)->GetEnergy();
    fGeLoGain_EnergyPlot_NoCuts->Fill(GeLoGain_energy);
  }  
  
  // Get my SiL3 stops (i.e. with coincidence w/ any SiL1-X-S)
  AnalysedPulseList SiL3_w_SiL1_coincidence_pulse_list;
  FindCoincidentPulses(SiL3_pulse_list, SiL1_pulse_lists, SiL3_w_SiL1_coincidence_pulse_list, fCoincidenceTimeLow_SiL3_SiL1, fCoincidenceTimeHigh_SiL3_SiL1, false, fSiL3_SiL1_TDiff);
  // Plot their energies
  for (AnalysedPulseList::const_iterator i_SiL3_coincident_pulse = SiL3_w_SiL1_coincidence_pulse_list.begin(); i_SiL3_coincident_pulse != SiL3_w_SiL1_coincidence_pulse_list.end(); ++i_SiL3_coincident_pulse) {
    
    double SiL3_coincident_energy = (*i_SiL3_coincident_pulse)->GetEnergy();
    fSiL3_EnergyPlot_wSiL1Coincidence->Fill(SiL3_coincident_energy);
  }

  //Get the Ge spectrum with coincident SiL3-S pulses
  AnalysedPulseList GeLoGain_w_SiL3_coincidence_pulse_list;
  if ( !SiL3_w_SiL1_coincidence_pulse_list.empty() ) {
    FindCoincidentPulses( GeLoGain_pulse_list, &SiL3_w_SiL1_coincidence_pulse_list, GeLoGain_w_SiL3_coincidence_pulse_list, fCoincidenceTimeLow_SiL3_GeLoGain, fCoincidenceTimeHigh_SiL3_GeLoGain, false, fSiL3_GeLoGain_TDiff);
  }
  else {
    FindCoincidentPulses( GeLoGain_pulse_list, SiL3_pulse_list, GeLoGain_w_SiL3_coincidence_pulse_list, fCoincidenceTimeLow_SiL3_GeLoGain, fCoincidenceTimeHigh_SiL3_GeLoGain, false, fSiL3_GeLoGain_TDiff);
  }

  // Get the SiL3-S spectrum that is coincident with the GeLoGain
  AnalysedPulseList SiL3_w_GeLoGain_coincidence_pulse_list;
  if ( !SiL3_w_SiL1_coincidence_pulse_list.empty() ) {
    FindCoincidentPulses( &SiL3_w_SiL1_coincidence_pulse_list, GeLoGain_pulse_list, SiL3_w_GeLoGain_coincidence_pulse_list, fCoincidenceTimeLow_SiL3_GeLoGain, fCoincidenceTimeHigh_SiL3_GeLoGain, false, NULL);
  }
  else {
    FindCoincidentPulses( SiL3_pulse_list, GeLoGain_pulse_list, SiL3_w_GeLoGain_coincidence_pulse_list, fCoincidenceTimeLow_SiL3_GeLoGain, fCoincidenceTimeHigh_SiL3_GeLoGain, false, NULL);
  }


  // Now fill the plot
  for (AnalysedPulseList::const_iterator i_GeLoGain_coincident_pulse = GeLoGain_w_SiL3_coincidence_pulse_list.begin(), 
	 i_SiL3_coincident_pulse = SiL3_w_GeLoGain_coincidence_pulse_list.begin(); 
       i_GeLoGain_coincident_pulse != GeLoGain_w_SiL3_coincidence_pulse_list.end() && i_SiL3_coincident_pulse != SiL3_w_GeLoGain_coincidence_pulse_list.end(); 
       ++i_GeLoGain_coincident_pulse, ++i_SiL3_coincident_pulse) {
    
    double GeLoGain_coincident_energy = (*i_GeLoGain_coincident_pulse)->GetEnergy();
    double SiL3_coincident_energy = (*i_SiL3_coincident_pulse)->GetEnergy();
    fSiL3_GeLoGain_CoincidentEnergies->Fill(SiL3_coincident_energy, GeLoGain_coincident_energy);
  }


  // Get my non-punchthrough SiL1-X-S pulses (i.e. no coincidence w/SiL3)
  for (std::vector<const AnalysedPulseList*>::const_iterator i_SiL1_pulse_list = SiL1_pulse_lists.begin(); i_SiL1_pulse_list != SiL1_pulse_lists.end(); ++i_SiL1_pulse_list) {

    AnalysedPulseList SiL1_w_no_SiL3_coincidence_pulse_list;
    FindCoincidentPulses( (*i_SiL1_pulse_list), SiL3_pulse_list, SiL1_w_no_SiL3_coincidence_pulse_list, fCoincidenceTimeLow_SiL3_SiL1, fCoincidenceTimeHigh_SiL3_SiL1, true, NULL);
    for (AnalysedPulseList::const_iterator i_SiL1_coincident_pulse = SiL1_w_no_SiL3_coincidence_pulse_list.begin(); i_SiL1_coincident_pulse != SiL1_w_no_SiL3_coincidence_pulse_list.end(); ++i_SiL1_coincident_pulse) {
    
      double SiL1_coincident_energy = (*i_SiL1_coincident_pulse)->GetEnergy();
      fSiL1_EnergyPlot_wNoSiL3Coincidence->Fill(SiL1_coincident_energy);
    }

    // Get the Ge coincidences
    AnalysedPulseList GeLoGain_w_SiL1_coincidence_pulse_list;
    FindCoincidentPulses( GeLoGain_pulse_list, &SiL1_w_no_SiL3_coincidence_pulse_list, GeLoGain_w_SiL1_coincidence_pulse_list, fCoincidenceTimeLow_SiL1_GeLoGain, fCoincidenceTimeHigh_SiL1_GeLoGain, false, fSiL1_GeLoGain_TDiff);
    
    // Get the SiL1-S spectrum that is coincident with the GeLoGain
    AnalysedPulseList SiL1_w_GeLoGain_coincidence_pulse_list;
    FindCoincidentPulses( &SiL1_w_no_SiL3_coincidence_pulse_list, GeLoGain_pulse_list, SiL1_w_GeLoGain_coincidence_pulse_list, fCoincidenceTimeLow_SiL1_GeLoGain, fCoincidenceTimeHigh_SiL1_GeLoGain, false, NULL);

    // Now fill the plot
    for (AnalysedPulseList::const_iterator i_GeLoGain_coincident_pulse = GeLoGain_w_SiL1_coincidence_pulse_list.begin(), 
	   i_SiL1_coincident_pulse = SiL1_w_GeLoGain_coincidence_pulse_list.begin(); 
	 i_GeLoGain_coincident_pulse != GeLoGain_w_SiL1_coincidence_pulse_list.end() && i_SiL1_coincident_pulse != SiL1_w_GeLoGain_coincidence_pulse_list.end(); 
	 ++i_GeLoGain_coincident_pulse, ++i_SiL1_coincident_pulse) {
      
      double GeLoGain_coincident_energy = (*i_GeLoGain_coincident_pulse)->GetEnergy();
      double SiL1_coincident_energy = (*i_SiL1_coincident_pulse)->GetEnergy();
      fSiL1_GeLoGain_CoincidentEnergies->Fill(SiL1_coincident_energy, GeLoGain_coincident_energy);
    }
  }

  
  
  return 0;
}

// For when we want to check coincidences with multiple channels (e.g. SiL3-S with any SiL1-X-S)
void PlotTAP_StoppedMuonDiscrepancy::FindCoincidentPulses(const AnalysedPulseList* pulse_list_1, std::vector<const AnalysedPulseList*> multichannel_pulse_lists, AnalysedPulseList& coincident_pulses, double coinc_time_low, double coinc_time_high, bool anti_coincidence, TH1F* hTDiffToFill) {

  for (std::vector<const AnalysedPulseList*>::const_iterator i_pulse_list = multichannel_pulse_lists.begin(); i_pulse_list != multichannel_pulse_lists.end(); ++i_pulse_list) {
    FindCoincidentPulses(pulse_list_1, (*i_pulse_list), coincident_pulses, coinc_time_low, coinc_time_high, anti_coincidence, hTDiffToFill);
  }
}

void PlotTAP_StoppedMuonDiscrepancy::FindCoincidentPulses(const AnalysedPulseList* pulse_list_1, const AnalysedPulseList* pulse_list_2, AnalysedPulseList& coincident_pulses, double coinc_time_low, double coinc_time_high, bool anti_coincidence, TH1F* hTDiffToFill) {

  // Will copy any pulses from pulse_list_1 to coincident_pulses, if any pulse in pulse_list_2 is within the time window
  // unless anti_coincidence is true, in which case, only pulses where there are no coincidences will be copied

  // Loop through the first pulse list
  for (AnalysedPulseList::const_iterator i_pulse_1 = pulse_list_1->begin(); i_pulse_1 != pulse_list_1->end(); ++i_pulse_1) {
    double pulse_1_time = (*i_pulse_1)->GetTime();

    // Now loop through the second pulse list and see if there's a coincidence
    bool coincidence_found = false;
    for (AnalysedPulseList::const_iterator i_pulse_2 = pulse_list_2->begin(); i_pulse_2 != pulse_list_2->end(); ++i_pulse_2) {
      double pulse_2_time = (*i_pulse_2)->GetTime();
      double time_difference = std::fabs(pulse_1_time - pulse_2_time);

      if (hTDiffToFill) {
	hTDiffToFill->Fill(time_difference);
      }

      if (time_difference > coinc_time_low && time_difference < coinc_time_high) {
	coincidence_found = true;
	//	std::cout << "AE: Pulse 1 (" << i_pulse_1 - pulse_list_1->begin() << "/" << pulse_list_1->size() << ") time = " << pulse_1_time << ", pulse 2 time = " << pulse_2_time << ", tdiff = " << time_difference << std::endl;
	//	std::cout << "Coincidence Found!" << std::endl;
      }
    }
    //    std::cout << "AE: Pulse 1 still exists? " << *i_pulse_1 << std::endl;
    if (!anti_coincidence && coincidence_found) {
      coincident_pulses.push_back(*i_pulse_1);
    }
    else if (anti_coincidence && !coincidence_found) {
      coincident_pulses.push_back(*i_pulse_1);
    }
  }
}

ALCAP_REGISTER_MODULE(PlotTAP_StoppedMuonDiscrepancy)
