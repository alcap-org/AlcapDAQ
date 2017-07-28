// A first example macro
// -- it loops through the first 10 MIDAS events in the NonTMETree and will print all its information
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro_NonTME.C+
// > FirstMacro_NonTME("out_non-tme-tree.root")

#include "TFile.h"
#include "TTree.h"

#include "AlCapConstants.h"
#include "TMETreeBranches.h"

#include <iostream>

void FirstMacro_NonTME(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* nontmetree = (TTree*) file->Get("TMETree/NonTMETree");

  SetTMEBranchAddresses(nontmetree); // setup the branch addresses

  int n_entries = 10;//nontmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {

    nontmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiL1, SiR1, All) for easy looping

    std::cout << "Run " << runId << " Block " << blockId << " :" << std::endl;

    std::cout << "All pulses in this MIDAS Event (E [keV], tblock [ms]):" << std::endl;
    for (int i_channel = 0; i_channel < n_all_channels; ++i_channel) {
      //      int n_pulses = *(all_nPulses[i_channel]);
      std::vector<SimplePulse>* i_pulse_list = all_channels[i_channel];
      int n_pulses = i_pulse_list->size();
      if (n_pulses > 0) {
	std::cout << all_channel_names[i_channel] << " (" << n_pulses << " pulse(s)): ";
	for (int i_pulse = 0; i_pulse < n_pulses; ++i_pulse) {
	  std::cout << "(" << i_pulse_list->at(i_pulse).E << ", " << i_pulse_list->at(i_pulse).tblock*1e-6 << "), ";
	}
	std::cout << std::endl;
      }
    }

    std::cout << "=======================================" << std::endl;
    std::cout << std::endl;
  }
}
