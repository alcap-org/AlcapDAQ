// EvdE plot for Si16b dataset
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"


//#include "AlCapConstants.h"
//#include "TMETreeBranches.h"

#include <iostream>

#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"

#include "src/plotters/SimplePulse.h"

void Andy_SiL3_FastSlowMatching(std::string filename){

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* taptree = (TTree*) file->Get("TAPTree/TAPTree");

  double min_time = -30000;
  double max_time = 30000;
  double time_width = 100;
  int n_time_bins = (max_time - min_time) / time_width;

  TH1F* hTDiff = new TH1F("hTDiff", "", n_time_bins,min_time,max_time);
  hTDiff->SetTitle("Andy, SiL3 Dataset");
  hTDiff->SetXTitle("t_{fast} - t_{slow} [ns]");

  Int_t blockId;
  TBranch* br_blockId = taptree->GetBranch("blockId");
  br_blockId->SetAddress(&blockId);

  SimplePulse* sil3_s_tap = new SimplePulse();
  taptree->SetBranchAddress("SiL3_S", &sil3_s_tap);
  SimplePulse* sil3_f_tap = new SimplePulse();
  taptree->SetBranchAddress("SiL3_F", &sil3_f_tap);

  std::vector<SimplePulse> slow_pulses;
  std::vector<SimplePulse> fast_pulses;
  int this_blockId = 0;
  std::cout << "Total TAPs = " << taptree->GetEntries() << std::endl;
  int n_entries = taptree->GetEntries();
  double n_matched = 0;
  double n_fast_unmatched = 0;
  double n_slow_unmatched = 0;

  for (int i_tap = 0; i_tap < n_entries; ++i_tap) {
    taptree->GetEntry(i_tap);
    if (i_tap % 100000 == 0) {
      std::cout << i_tap << " / " << n_entries << std::endl;
    }

    if (this_blockId != blockId) {
      int this_n_matched = 0;
      for (int i_slow_pulse = 0; i_slow_pulse < slow_pulses.size(); ++i_slow_pulse) {
	double slow_time = slow_pulses.at(i_slow_pulse).tblock;
	for (int i_fast_pulse = 0; i_fast_pulse < fast_pulses.size(); ++i_fast_pulse) {

	  double fast_time = fast_pulses.at(i_fast_pulse).tblock;
	  hTDiff->Fill(fast_time - slow_time);
	  if (std::fabs(slow_time - fast_time) < 500) {
	    //	    std::cout << "S = " << slow_time << ", F = " << fast_time << ", dt = " << slow_time - fast_time << " ";
	    //	    std::cout << "Matched!" << std::endl;
	    ++this_n_matched;
	  }
	}
	n_fast_unmatched += fast_pulses.size() - this_n_matched;
	n_slow_unmatched += slow_pulses.size() - this_n_matched;
	n_matched += this_n_matched;
      }
      
      slow_pulses.clear();
      fast_pulses.clear();
      this_blockId = blockId;
      continue;
    }
    
    double slow_amp = sil3_s_tap->Amp;
    double fast_amp = sil3_f_tap->Amp;
    if (slow_amp > 0) {
      slow_pulses.push_back(*sil3_s_tap);
      continue;
    }
    if (fast_amp > 0) {
      fast_pulses.push_back(*sil3_f_tap);
      continue;
    }
  }

  std::cout << "Total Matched = " << n_matched << std::endl;
  std::cout << "Umatched = " << n_fast_unmatched << " (fast), " << n_slow_unmatched << " (slow)" << std::endl;
}
