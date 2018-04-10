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

void EventSelection() {

  ///////////////////////////////////////////
  // User parameters
  //  std::string filename = "~/data/out/local/out09040_tme-tree_templates.root";
  std::string filename = "~/data/out/v13/SiL3.root";
  std::string outfilename = "~/data/results/SiL3_active/EventSelection.root";
  int max_muSc_pulses = 1; // only accept TMEs where there is a single muon in the central muon channel
  double min_muon_energy = 3000; // count stopped muons that pass these cuts
  double max_muon_energy = 6000;
  double muon_time_cut = 200;
  double overall_energy_cut = 100; // only accept energy deposits greater than this (to remove noise)

  int n_total_tmes = 0;
  int n_analysed_tmes = 0;
  int n_stopped_muons = 0;
  
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
  hTimeEnergy->SetBinErrorOption(TH1::kPoisson);

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

    std::vector<SimplePulse>* i_thick_pulse_list = SiL3;
    int n_thick_pulses = i_thick_pulse_list->size();

    // Quickly check for any double counted pulses in SiL3
    bool double_counted = false;
    for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
      double thick_bit_mask = i_thick_pulse_list->at(i_thick_pulse).bit_mask;
      if (thick_bit_mask == 2) {
	double_counted = true;
      }
    }
    if (double_counted) {
      continue;
    }

    std::vector<SimplePulse>* i_muSc_pulse_list = muSc;
    int n_muSc_pulses = i_muSc_pulse_list->size();
    if (n_muSc_pulses > max_muSc_pulses) {
      continue;
    }

    ++n_analysed_tmes;    
   
    int muon_element = -1;
    double muon_time = 0;
    for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {      
      double thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
      double thick_energy = i_thick_pulse_list->at(i_thick_pulse).Amp*4.58 - 10.39;
      if (thick_energy > min_muon_energy && thick_energy < max_muon_energy && std::fabs(thick_time)<muon_time_cut) {
	muon_element = i_thick_pulse;
	muon_time = thick_time;

	++n_stopped_muons;
      }
    }

    // Plot the energy and time difference of everything else
    for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
      //      if (i_thick_pulse != muon_element) {
      double thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
	double thick_energy = i_thick_pulse_list->at(i_thick_pulse).Amp*4.58 - 10.39;

	if (thick_energy > overall_energy_cut) {
	  hTimeEnergy->Fill(thick_time, thick_energy);
	}
	//      }
    }
  }

  hTimeEnergy->Draw("COLZ");
  
  TFile* output = new TFile(outfilename.c_str(), "RECREATE");
  hTimeEnergy->Write();
  
  TTree* tree = new TTree("cuts", "cuts");
  tree->Branch("n_total_tmes", &n_total_tmes);
  tree->Branch("n_analysed_tmes", &n_analysed_tmes);
  tree->Branch("max_muSc_pulses", &max_muSc_pulses);
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
