// A first example macro
// -- it loops through the first 100 TMEs in the TMETree and will print all its information
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"

#include "AlCapConstants.h"
#include "TMETreeBranches.h"

#include <iostream>

void FirstMacro(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  int n_entries = 100;//tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {

    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiL1, SiR1, All) for easy looping

    std::cout << "Run " << runId << " Block " << blockId << " TME " << TMEId << ":" << std::endl;
    std::cout << "This TME uses a +/- " << TMEWindowWidth << " ns window" << std::endl;
    std::cout << "Previous TME was " << timeToPrevTME << " ns ago and next TME will be in " << timeToNextTME << " ns (both times muon-to-muon)" << std::endl << std::endl;
    std::cout << "Central Muon: E = " << centralMuonEnergy << " keV, t = " << centralMuonTime << " ns (" << *centralMuonChannel << ", TPI ID " << centralMuonTPIID << ")" << std::endl << std::endl;

    std::cout << "All pulses in this TME (E [keV], t [ns], bitmask):" << std::endl;
    for (int i_channel = 0; i_channel < n_all_channels; ++i_channel) {
      int n_pulses = *(all_nPulses[i_channel]);
      std::vector<SimplePulse>* i_pulse_list = all_channels[i_channel];
      if (n_pulses > 0) {
	std::cout << all_channel_names[i_channel] << " (" << n_pulses << " pulse(s)): ";
	for (int i_pulse = 0; i_pulse < n_pulses; ++i_pulse) {
	  std::cout << "(" << i_pulse_list->at(i_pulse).E << ", " << i_pulse_list->at(i_pulse).tTME << ", "<< (int)i_pulse_list->at(i_pulse).bit_mask << "), ";
	}
	std::cout << std::endl;
      }
    }

    std::cout << "=======================================" << std::endl;
    std::cout << std::endl;
  }
}
