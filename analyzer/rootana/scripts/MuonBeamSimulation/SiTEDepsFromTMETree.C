// Get the energy deposited in the SiT
// -- it loops through the first 100 TMEs in the TMETree and will print all its information
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"

#include "scripts/TMETree/AlCapConstants.h"
#include "scripts/TMETree/TMETreeBranches.h"

#include <iostream>
#include <sstream>

void SiTEDepsFromTMETree(std::string filename, std::string out_filename, bool active_target_run = false) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  double min_energy = 0;
  double max_energy = 10000;
  double energy_width = 1;
  int n_energy_bins = (max_energy - min_energy) / energy_width;
  TH1F* hCentralMuonEnergies = new TH1F("hCentralMuonEnergies", "Energy of the Central Muon in each TME", n_energy_bins,min_energy,max_energy);
  hCentralMuonEnergies->SetXTitle("Central Muon Energy [keV]");
  hCentralMuonEnergies->SetYTitle("Raw Count");
  hCentralMuonEnergies->SetLineWidth(2);
  hCentralMuonEnergies->SetLineColor(kBlue);

  TH1F* hCentralMuonEnergies_wSiL1Hit = new TH1F("hCentralMuonEnergies_wSiL1Hit", "Energy of the Central Muon in each TME with an SiL1 hit", n_energy_bins,min_energy,max_energy);
  hCentralMuonEnergies_wSiL1Hit->SetXTitle("Central Muon Energy [keV]");
  hCentralMuonEnergies_wSiL1Hit->SetYTitle("Raw Count");
  hCentralMuonEnergies_wSiL1Hit->SetLineWidth(2);
  hCentralMuonEnergies_wSiL1Hit->SetLineColor(kBlue);

  TH1F* hCentralMuonEnergies_wNoSiL1Hit = new TH1F("hCentralMuonEnergies_wNoSiL1Hit", "Energy of the Central Muon in each TME with no SiL1 hit", n_energy_bins,min_energy,max_energy);
  hCentralMuonEnergies_wNoSiL1Hit->SetXTitle("Central Muon Energy [keV]");
  hCentralMuonEnergies_wNoSiL1Hit->SetYTitle("Raw Count");
  hCentralMuonEnergies_wNoSiL1Hit->SetLineWidth(2);
  hCentralMuonEnergies_wNoSiL1Hit->SetLineColor(kBlue);


  double min_time = -5000;
  double max_time = 5000;
  double time_width = 10;
  int n_time_bins = (max_time - min_time) / time_width;
  TH2F* hCorrelations = new TH2F("hCorrelations", "Correlations", n_time_bins,min_time,max_time, n_energy_bins,min_energy,max_energy);


  int n_entries = tmetree->GetEntries();
  if (n_entries > tmetree->GetEntries()) {
    std::cout << "Error: entries requested exceed number in tree" << std::endl;
    n_entries = tmetree->GetEntries();
  }
  std::stringstream i_centralMuonChannel;
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {
    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }
    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiL1, SiR1, All) for easy looping
    hCentralMuonEnergies->Fill(centralMuonEnergy);

    // This only makes sens if this is an active target run
    if (active_target_run) {
      bool SiL1_hit = false;
      for(int i_SiL1_channel = 0; i_SiL1_channel < n_SiL1_channels; ++i_SiL1_channel) {
	std::vector<SimplePulse>* i_pulse_list = all_SiL1_channels[i_SiL1_channel];
	if (i_pulse_list->size() > 0) {
	  SiL1_hit = true;
	  break;
	}
      }
      if (SiL1_hit) {
	hCentralMuonEnergies_wSiL1Hit->Fill(centralMuonEnergy);
      }
      else {
	hCentralMuonEnergies_wNoSiL1Hit->Fill(centralMuonEnergy);
      }
    }

    for(int i_SiT_channel = 0; i_SiT_channel < n_SiT_channels; ++i_SiT_channel) {
      std::vector<SimplePulse>* i_pulse_list = all_SiT_channels[i_SiT_channel];
      i_centralMuonChannel.str("");
      i_centralMuonChannel << "SiT-" << i_SiT_channel+1 << "-S";
      
      int n_pulses = i_pulse_list->size();
      if (n_pulses > 0) {
	for (int i_pulse = 0; i_pulse < n_pulses; ++i_pulse) {
	  double energy = i_pulse_list->at(i_pulse).E;
	  double time = i_pulse_list->at(i_pulse).tTME;

	  if (*centralMuonChannel != i_centralMuonChannel.str() ||
	      (*centralMuonChannel == i_centralMuonChannel.str() && centralMuonTPIID != i_pulse_list->at(i_pulse).tpi_id)) {
	    
	      hCorrelations->Fill(time, energy);
	  }
	}
      }
    }

    /*    if (anyDoubleCountedPulses) {
      continue;
    }

    for(int i_SiT_channel = 0; i_SiT_channel < n_SiT_channels; ++i_SiT_channel) {
      std::vector<SimplePulse>* i_pulse_list = all_SiT_channels[i_SiT_channel];
      int n_pulses = i_pulse_list->size();
      if (n_pulses > 0) {
	for (int i_pulse = 0; i_pulse < n_pulses; ++i_pulse) {
	  double energy = i_pulse_list->at(i_pulse).E;
	  hCentralMuonEnergies->Fill(energy);
	  hCentralMuonEnergies_Zoom->Fill(energy);
	  hCentralMuonEnergies_Zoom2->Fill(energy);
	}
      }
    }
    */
    
  }

  //  hCentralMuonEnergies->Draw();
  //  hCentralMuonEnergies_Zoom->Draw();
  hCorrelations->Draw("COLZ");
  std::cout << "Total Number of Incoming Muons = " << hCentralMuonEnergies->Integral() << std::endl;
  
  TFile* output = new TFile(out_filename.c_str(), "RECREATE");
  hCentralMuonEnergies->Write();
  hCorrelations->Write();
  
  if (active_target_run) {
    hCentralMuonEnergies_wSiL1Hit->Write();
    hCentralMuonEnergies_wNoSiL1Hit->Write();
  }

  output->Write();
  output->Close();

}
