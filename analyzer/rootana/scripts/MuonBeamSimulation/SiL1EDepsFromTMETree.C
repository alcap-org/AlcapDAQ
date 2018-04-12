// Get the energy deposited in the SiL1
// only useful for active target runs

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"

#include "scripts/TMETree/AlCapConstants.h"
#include "scripts/TMETree/TMETreeBranches.h"

#include <iostream>
#include <sstream>

void SiL1EDepsFromTMETree(std::string filename, std::string out_filename, bool Si16a_dataset = false) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  double min_energy = 0;
  double max_energy = 10000;
  double energy_width = 1;
  int n_energy_bins = (max_energy - min_energy) / energy_width;
  TH1F* hSiL1EDeps = new TH1F("hSiL1EDeps", "Energy Deposit in SiL1 (bit_mask == 1)", n_energy_bins,min_energy,max_energy);
  hSiL1EDeps->SetXTitle("SiL1 EDep [keV]");
  hSiL1EDeps->SetYTitle("Raw Count");
  hSiL1EDeps->SetLineWidth(2);
  hSiL1EDeps->SetLineColor(kBlue);

  TH1F* hSiL1EDeps_wSiL3Hit = new TH1F("hSiL1EDeps_wSiL3Hit", "Energy Deposit in SiL1 (bit_mask == 1, N SiL3 hits > 0)", n_energy_bins,min_energy,max_energy);
  hSiL1EDeps_wSiL3Hit->SetXTitle("SiL1 EDep [keV]");
  hSiL1EDeps_wSiL3Hit->SetYTitle("Raw Count");
  hSiL1EDeps_wSiL3Hit->SetLineWidth(2);
  hSiL1EDeps_wSiL3Hit->SetLineColor(kBlue);

  TH1F* hSiL1EDeps_wNoSiL3Hit = new TH1F("hSiL1EDeps_wNoSiL3Hit", "Energy Deposit in SiL1 (bit_mask == 1, N SiL3 hits == 0)", n_energy_bins,min_energy,max_energy);
  hSiL1EDeps_wNoSiL3Hit->SetXTitle("SiL1 EDep [keV]");
  hSiL1EDeps_wNoSiL3Hit->SetYTitle("Raw Count");
  hSiL1EDeps_wNoSiL3Hit->SetLineWidth(2);
  hSiL1EDeps_wNoSiL3Hit->SetLineColor(kBlue);

  int n_entries = tmetree->GetEntries();
  if (n_entries > tmetree->GetEntries()) {
    std::cout << "Error: entries requested exceed number in tree" << std::endl;
    n_entries = tmetree->GetEntries();
  }

  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {
    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }
    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiL1, SiR1, All) for easy looping

    for(int i_SiL1_channel = 0; i_SiL1_channel < n_SiL1_channels; ++i_SiL1_channel) {
      std::vector<SimplePulse>* i_pulse_list = all_SiL1_channels[i_SiL1_channel];
      int n_pulses = i_pulse_list->size();
      
      if (n_pulses > 0) {
	for (int i_pulse = 0; i_pulse < n_pulses; ++i_pulse) {
	  double energy = i_pulse_list->at(i_pulse).E;
	  double time = i_pulse_list->at(i_pulse).tTME;

	  if (i_pulse_list->at(i_pulse).bit_mask == 1) {
	    hSiL1EDeps->Fill(energy);

	    // This only makes sens if this is the Si16a datset (i.e. SiL3 is behind SiL1)
	    if (Si16a_dataset) {
	      std::vector<SimplePulse>* SiL3_pulse_list = SiL3;
	      if (SiL3_pulse_list->size() > 0) {
		hSiL1EDeps_wSiL3Hit->Fill(energy);
	      }
	      else {
		hSiL1EDeps_wNoSiL3Hit->Fill(energy);
	      }
	    }
	  }
	}
      }
    }
  }

  hSiL1EDeps->Draw();
  
  TFile* output = new TFile(out_filename.c_str(), "RECREATE");
  hSiL1EDeps->Write();
  
  if (Si16a_dataset) {
    hSiL1EDeps_wSiL3Hit->Write();
    hSiL1EDeps_wNoSiL3Hit->Write();
  }

  output->Write();
  output->Close();
}
