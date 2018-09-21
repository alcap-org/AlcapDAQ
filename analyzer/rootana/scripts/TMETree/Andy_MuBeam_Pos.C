// EvdE plot for Si16b dataset
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"

#include "AlCapConstants.h"
#include "TMETreeBranches.h"

#include <iostream>

#include "TH2.h"
#include "TMath.h"

void Andy_MuBeam_Pos(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  TH1F* hNMuonsPerStrip = new TH1F("", "", 16,0,16);
  hNMuonsPerStrip->SetTitle("NMuons Per Strip");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_entries = tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {

    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }

    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

    for (int i_thin_channel = 0; i_thin_channel < n_SiL1_channels; ++i_thin_channel) {
      std::vector<SimplePulse>* i_thin_pulse_list = all_SiL1_channels[i_thin_channel];
      int n_thin_pulses = i_thin_pulse_list->size();

      for (int i_thin_pulse = 0; i_thin_pulse < n_thin_pulses; ++i_thin_pulse) {
	double thin_time = i_thin_pulse_list->at(i_thin_pulse).tTME;
	double thin_energy = i_thin_pulse_list->at(i_thin_pulse).E;
	int thin_bit_mask = i_thin_pulse_list->at(i_thin_pulse).bit_mask;
	if (thin_energy > 400 && thin_energy < 2000 && thin_bit_mask==1) {
	  hNMuonsPerStrip->Fill(i_thin_channel+1);
	}
      }
    }
  }

  hNMuonsPerStrip->Draw();
}
