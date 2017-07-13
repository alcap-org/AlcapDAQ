#include "TFile.h"
#include "TTree.h"

#include "AlCapConstants.h"
#include "TMETreeBranches.h"

#include <iostream>

void FirstMacro(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  SetTMEBranchAddresses(tmetree);

  int n_entries = 1000;
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {
    tmetree->GetEntry(i_tme);
    
    for (int i_GeLoGain_pulse = 0; i_GeLoGain_pulse < GeLoGain_nPulses; ++i_GeLoGain_pulse) {
      std::cout << "Pulse #" << i_GeLoGain_pulse << ":" << GeLoGain_E->at(i_GeLoGain_pulse) << std::endl;
    }
  }
}
