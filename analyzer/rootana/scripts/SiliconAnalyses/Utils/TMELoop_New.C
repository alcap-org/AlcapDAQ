#include "TFile.h"
#include "TH2.h"
#include "TTree.h"

#include "TF1.h"

#include "scripts/TMETree/AlCapConstants.h"
#include "scripts/TMETree/TMETreeBranches.h"

#include <iostream>

struct TMELoopArgs {
  // Standard parameters
  std::string infilename;
  std::string tmetreename;
  std::string outfilename;
  int n_entries;

  // Channel configuration
  std::vector<std::vector<SimplePulse>** > muon_channels;
  std::vector<std::vector<SimplePulse>** > SiT_channels;

  std::vector<std::string> turn_on_branches;
  
  // Event vetoes
  bool veto_any_double_counts;
  bool veto_max_muon_channel_pulses;
  int max_muon_channel_pulses;
};

void TMELoop(const TMELoopArgs& args) {

  TFile* infile = new TFile(args.infilename.c_str(), "READ");
  TTree* tmetree = (TTree*) infile->Get(args.tmetreename.c_str());
  TFile* outfile = new TFile(args.outfilename.c_str(), "RECREATE");
  
  SetTMEBranchAddresses(tmetree); // setup the branch addresses
  tmetree->SetBranchStatus("*", 0);
  tmetree->SetBranchStatus("TMEId", 0);
  for (std::vector<std::string>::const_iterator i_channel = args.turn_on_branches.begin(); i_channel != args.turn_on_branches.end(); ++i_channel) {
    tmetree->SetBranchStatus(i_channel->c_str(), 1);
  }

  int n_total_tmes = 0;
  int n_analysed_tmes = 0;
  
  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_entries = args.n_entries;
  if (n_entries < 0) {
    n_entries = tmetree->GetEntries();
  }
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {
    //    std::cout << "TME #" << i_tme << std::endl;
    ++n_total_tmes;
    
    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }

    tmetree->GetEntry(i_tme);
    //    std::cout << blockId << " " << runId << " " << TMEId << std::endl;
    CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

    /////////////////////////////////////
    // Event Vetoes
    if (args.veto_any_double_counts && anyDoubleCountedPulses) {
      continue; // to the next TME
    }
    if (args.veto_max_muon_channel_pulses) {
      int n_muon_channels = args.muon_channels.size();
      int n_muon_channel_pulses = 0;
      for (int i_muon_channel = 0; i_muon_channel < n_muon_channels; ++i_muon_channel) {
	n_muon_channel_pulses += (*args.muon_channels[i_muon_channel])->size();
      }
      if (n_muon_channel_pulses > args.max_muon_channel_pulses) {
	continue; // to the next TME
      }
    }

    ++n_analysed_tmes;

    /////////////////////////////////
    // Histogram
    SiTEnergy(tmetree);
  } // end TME loop

  WriteSiTEnergy();
  outfile->Close();
}
