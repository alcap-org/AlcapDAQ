
#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TStopwatch.h"

#include "AlCapConstants.h"
#include "TMETreeBranches.h"

#include <iostream>
#include <sstream>

void SpeedingUp(std::string filename) {

  TStopwatch sw;

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  int n_entries = tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {

    if (i_tme % 10000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }

    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiL1, SiR1, All) for easy looping
  }

  std::string outfilename = "speed-up_"+filename;
  TFile* output_file = new TFile(outfilename.c_str(), "RECREATE");
  output_file->Write();
  output_file->Close();
  std::cout << "Real Time = " << sw.RealTime() << ", CPU Time =  " << sw.CpuTime() << std::endl;
}
