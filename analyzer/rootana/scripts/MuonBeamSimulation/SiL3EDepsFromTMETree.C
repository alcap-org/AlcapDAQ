// Get the energy deposited in the SiL3
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

void SiL3EDepsFromTMETree(std::string filename, std::string out_filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  double min_energy = 0;
  double max_energy = 10000;
  double energy_width = 1;
  int n_energy_bins = (max_energy - min_energy) / energy_width;
  TH1F* hSiL3EDeps = new TH1F("hSiL3EDeps", "Energy Deposit in SiL3 (bit_mask == 1)", n_energy_bins,min_energy,max_energy);
  hSiL3EDeps->SetXTitle("SiL3 EDep [keV]");
  hSiL3EDeps->SetYTitle("Raw Count");
  hSiL3EDeps->SetLineWidth(2);
  hSiL3EDeps->SetLineColor(kBlue);


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

    std::vector<SimplePulse>* i_pulse_list = SiL3;
    int n_pulses = i_pulse_list->size();
      
    if (n_pulses > 0) {
      for (int i_pulse = 0; i_pulse < n_pulses; ++i_pulse) {
	double energy = 4.366*i_pulse_list->at(i_pulse).Amp + 35.17;
	double time = i_pulse_list->at(i_pulse).tTME;

	if (i_pulse_list->at(i_pulse).bit_mask == 1 && time>-200 && time<200) {
	  hSiL3EDeps->Fill(energy);
	}
      }
    }
  }

  hSiL3EDeps->Draw();
  
  TFile* output = new TFile(out_filename.c_str(), "RECREATE");
  hSiL3EDeps->Write();

  output->Write();
  output->Close();
}
