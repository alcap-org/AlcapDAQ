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

void Andy_Si16b_LayerCoincidences(std::string filename) {

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* tmetree = (TTree*) file->Get("TMETree/TMETree");

  double min_time = -2000;
  double max_time = 2000;
  double time_width = 2;
  int n_time_bins = (max_time - min_time) / time_width;

  TH1F* hLayerTDiff_SiR1SiR2 = new TH1F("hLayerTDiff_SiR1SiR2", "", n_time_bins,min_time,max_time);
  hLayerTDiff_SiR1SiR2->SetTitle("TDiff SiR1-SiR2");
  hLayerTDiff_SiR1SiR2->SetXTitle("t_{SiR1} - t_{SiR2} [ns]");

  TH1F* hLayerTDiff_SiR2SiR3 = new TH1F("hLayerTDiff_SiR2SiR3", "", n_time_bins,min_time,max_time);
  hLayerTDiff_SiR2SiR3->SetTitle("TDiff SiR2-SiR3");
  hLayerTDiff_SiR2SiR3->SetXTitle("t_{SiR2} - t_{SiR3} [ns]");

  TH1F* hCount = new TH1F("hCount", "", 1,0,1);
  hCount->SetXTitle("Channel");

  SetTMEBranchAddresses(tmetree); // setup the branch addresses

  std::cout << "Total TMEs = " << tmetree->GetEntries() << std::endl;
  int n_entries = tmetree->GetEntries();
  for (int i_tme = 0; i_tme < n_entries; ++i_tme) {

    if (i_tme % 100000 == 0) {
      std::cout << i_tme << " / " << n_entries << std::endl;
    }

    tmetree->GetEntry(i_tme);
    CollectChannels(); // collect all channels into various groups (e.g. SiR1, SiL1, All) for easy looping

    std::vector<SimplePulse>* i_thick_pulse_list = SiR2;
    int n_thick_pulses = i_thick_pulse_list->size();

    for (int i_thin_channel = 0; i_thin_channel < n_SiR1_channels; ++i_thin_channel) {
      std::vector<SimplePulse>* i_thin_pulse_list = all_SiR1_channels[i_thin_channel];
      int n_thin_pulses = i_thin_pulse_list->size();

      hCount->Fill(all_SiR1_channel_names[i_thin_channel].c_str(), n_thin_pulses);
      hCount->Fill("SiR1 (total)", n_thin_pulses);
      if (n_thin_pulses < 1) {
	continue;
      }


      for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
	double thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;

	for (int i_thin_pulse = 0; i_thin_pulse < n_thin_pulses; ++i_thin_pulse) {
	  double thin_time = i_thin_pulse_list->at(i_thin_pulse).tTME;
	  double thin_energy = i_thin_pulse_list->at(i_thin_pulse).E;

	  hLayerTDiff_SiR1SiR2->Fill(thin_time - thick_time);
	}
      }
    }

    std::vector<SimplePulse>* i_third_pulse_list = SiR3;
    int n_third_pulses = i_third_pulse_list->size();
    for (int i_thick_pulse = 0; i_thick_pulse < n_thick_pulses; ++i_thick_pulse) {
      double thick_time = i_thick_pulse_list->at(i_thick_pulse).tTME;
      
      for (int i_third_pulse = 0; i_third_pulse < n_third_pulses; ++i_third_pulse) {
	double third_time = i_third_pulse_list->at(i_third_pulse).tTME;
	double third_energy = i_third_pulse_list->at(i_third_pulse).E;
	
	hLayerTDiff_SiR2SiR3->Fill(thick_time - third_time);
      }
    }
    hCount->Fill("SiR2", n_thick_pulses);
    hCount->Fill("SiR3", n_third_pulses);
  }

  hLayerTDiff_SiR1SiR2->Draw("COLZ");

}
