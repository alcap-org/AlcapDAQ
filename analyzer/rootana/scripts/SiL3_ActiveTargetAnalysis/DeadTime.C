// EvdE plot for Si16b dataset
// To use (from analyzer/rootana):
// > root -l -b
// > .L scripts/TMETree/FirstMacro.C+
// > FirstMacro("out_tme-tree.root")

#include "TFile.h"
#include "TTree.h"

#include <iostream>

#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"

#include "src/plotters/SimplePulse.h"

void DeadTime(){

  //////////////////////////////
  // User parameters
  //  std::string filename = "~/data/out/local/out09040_tme-tree_templates.root";
  std::string filename = "~/data/out/v13/SiL3.root";
  std::string outfilename = "~/data/results/SiL3_active/DeadTime.root";

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* taptree = (TTree*) file->Get("TAPTree/TAPTree");

  double min_tdiff = 0;
  double max_tdiff = 20000;
  double tdiff_width = 200;
  int n_tdiff_bins = (max_tdiff - min_tdiff) / tdiff_width;
  double min_energy = 0;
  double max_energy = 20000;
  double energy_width = 100;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  TH2F* hTDiff = new TH2F("hTDiff", "", n_tdiff_bins,min_tdiff,max_tdiff, n_energy_bins,min_energy,max_energy);
  hTDiff->SetTitle("SiL3 Time Between Consecutive Pulses (TAP Tree)");
  hTDiff->SetXTitle("t_{i+1} - t_{i} [ns]");
  hTDiff->SetYTitle("Energy [keV]");

  Int_t blockId;
  TBranch* br_blockId = taptree->GetBranch("blockId");
  br_blockId->SetAddress(&blockId);

  SimplePulse* sil3_s_tap = new SimplePulse();
  taptree->SetBranchAddress("SiL3_S", &sil3_s_tap);
  
  std::cout << "Total TAPs = " << taptree->GetEntries() << std::endl;
  int n_entries = taptree->GetEntries();
  bool first_in_event = false;
  double prev_time = 0;
  for (int i_tap = 0; i_tap < n_entries; ++i_tap) {
    taptree->GetEntry(i_tap);

    if (i_tap % 100000 == 0) {
      std::cout << i_tap << " / " << n_entries << std::endl;
    }

    SimplePulse* this_tap = sil3_s_tap;
    double Amp = this_tap->Amp;
    if (Amp<=0) {
      first_in_event = false;
      continue;
    }
    double energy = Amp*4.58 - 10.39;
    double time = this_tap->tblock;
    if (!first_in_event) {
      //      std::cout <<"Start of event: t = " << time << std::endl;
      first_in_event = true;
    }
    else {
      //      std::cout << "t = " << time << ", prev_t = " << prev_time << ", delta_t = " << time - prev_time << std::endl;
      hTDiff->Fill(time - prev_time, energy);
    }
    prev_time = time;
  }

  hTDiff->Draw("COLZ");
  
  TFile* output = new TFile(outfilename.c_str(), "RECREATE");
  hTDiff->Write();
  
  output->Write();
  output->Close();
}
