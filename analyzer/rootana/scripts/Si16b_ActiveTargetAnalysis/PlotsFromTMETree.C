// EvdE plot for Si16b dataset
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"

#include "scripts/TMETree/AlCapConstants.h"
#include "scripts/TMETree/TMETreeBranches.h"

#include <iostream>
#include <sstream>

#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLatex.h"

void PlotsFromTMETree() {

  std::string filename = "~/data/out/local/out10404_tme-tree.root";
  std::string outfilename = "scripts/Si16b_ActiveTargetAnalysis/hist_files/PlotsFromTMETree_Run10404_DetArmVeto.root";

  // The cuts I'm doing
  int n_total_tmes = 0;
  int max_central_muon_pulses = 1; // only accept TMEs where there is a single muon in the central muon channel
  int n_analysed_tmes = 0;
  double min_muon_energy = 400; // count stopped muons that pass these cuts
  double max_muon_energy = 1200;
  double muon_time_cut = 200;
  int n_stopped_muons = 0;
  double overall_energy_cut = 100; // only accept energy deposits greater than this (to remove noise)
  
  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  double min_time = -30000;
  double max_time = 30000;
  double time_width = 100;
  int n_time_bins = (max_time - min_time) / time_width;

  double min_energy = 0;
  double max_energy = 30000;
  double energy_width = 100;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  TH2F* hTimeEnergy = new TH2F("hTimeEnergy", "", n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);
  hTimeEnergy->SetTitle("");
  hTimeEnergy->SetXTitle("Time [ns]");
  hTimeEnergy->SetYTitle("Energy [keV]");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses
  CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_tme_entries = tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_tme_entries; ++i_tme) {
    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_tme_entries << std::endl;
    }
    tmetree->GetEntry(i_tme);

    ++n_total_tmes;

    // Quickly check for any double counted pulses in SiL1
    bool double_counted = false;
    for (int i_SiL1_channel = 0; i_SiL1_channel < n_SiL1_channels; ++i_SiL1_channel) {
      std::vector<SimplePulse>* i_target_pulse_list = all_SiL1_channels[i_SiL1_channel];
      int n_target_pulses = i_target_pulse_list->size();
   
      for (int i_target_pulse = 0; i_target_pulse < n_target_pulses; ++i_target_pulse) {
	if (i_target_pulse_list->at(i_target_pulse).bit_mask == 2) {
	  double_counted = true;
	  break;
	}
      }
    }
    if (double_counted) {
      continue;
    }

    if (SiL3->size() > 0 || SiR1_1->size()>0 || SiR1_2->size() > 0 || SiR1_3->size() > 0 || SiR1_4->size() > 0) {
      continue; // veto on any hits to try and reduce the contamination of ejected protons that don't stop in the target itself
    }


    std::vector<SimplePulse>* i_central_muon_pulse_list = NULL;
    if (*centralMuonChannel=="SiT-1-S") {
      i_central_muon_pulse_list = SiT_1;
    }
    else if (*centralMuonChannel=="SiT-2-S") {
      i_central_muon_pulse_list = SiT_2;
    }
    else if (*centralMuonChannel=="SiT-3-S") {
      i_central_muon_pulse_list = SiT_3;
    }
    else if (*centralMuonChannel=="SiT-4-S") {
      i_central_muon_pulse_list = SiT_4;
    }

    int n_central_muon_pulses = i_central_muon_pulse_list->size();
    if (n_central_muon_pulses > max_central_muon_pulses) {
      continue;
    }

    ++n_analysed_tmes;    


    int muon_element = -1;
    double muon_time = 0;
    for (int i_SiL1_channel = 0; i_SiL1_channel < n_SiL1_channels; ++i_SiL1_channel) {
      std::vector<SimplePulse>* i_target_pulse_list = all_SiL1_channels[i_SiL1_channel];
      int n_target_pulses = i_target_pulse_list->size();
      
      for (int i_target_pulse = 0; i_target_pulse < n_target_pulses; ++i_target_pulse) {      
	double target_time = i_target_pulse_list->at(i_target_pulse).tTME;
	double target_energy = i_target_pulse_list->at(i_target_pulse).E;
	if (target_energy > min_muon_energy && target_energy < max_muon_energy && std::fabs(target_time)<muon_time_cut) {
	  muon_element = i_target_pulse;
	  muon_time = target_time;
	  
	  ++n_stopped_muons;
	  //	std::cout << "Muon! E = " << target_energy << ", t = " << target_time << std::endl;
	}
      }
    }

    for (int i_SiL1_channel = 0; i_SiL1_channel < n_SiL1_channels; ++i_SiL1_channel) {
      std::vector<SimplePulse>* i_target_pulse_list = all_SiL1_channels[i_SiL1_channel];
      int n_target_pulses = i_target_pulse_list->size();
      
      // Plot the energy and time difference of everything else
      for (int i_target_pulse = 0; i_target_pulse < n_target_pulses; ++i_target_pulse) {
	
	double target_time = i_target_pulse_list->at(i_target_pulse).tTME;
	double target_energy = i_target_pulse_list->at(i_target_pulse).E;
	double target_bit_mask = i_target_pulse_list->at(i_target_pulse).bit_mask;
	//      	std::cout << "Proton? E = " << target_energy << ", t = " << target_time << ", dt = " << target_time - muon_time << std::endl;
	if (target_energy > overall_energy_cut) {
	  hTimeEnergy->Fill(target_time, target_energy);
	}
      }
    }
  }

  hTimeEnergy->Draw("COLZ");
  
  TFile* output = new TFile(outfilename.c_str(), "RECREATE");
  hTimeEnergy->Write();
  
  TTree* tree = new TTree("cuts", "cuts");
  tree->Branch("n_total_tmes", &n_total_tmes);
  tree->Branch("n_analysed_tmes", &n_analysed_tmes);
  tree->Branch("max_central_muon_pulses", &max_central_muon_pulses);
  tree->Branch("n_stopped_muons", &n_stopped_muons);
  tree->Branch("min_muon_energy", &min_muon_energy);
  tree->Branch("max_muon_energy", &max_muon_energy);
  tree->Branch("muon_time_cut", &muon_time_cut);
  tree->Branch("overall_energy_cut", &overall_energy_cut);
  tree->Fill();
  tree->Write();

  output->Write();
  output->Close();
}
