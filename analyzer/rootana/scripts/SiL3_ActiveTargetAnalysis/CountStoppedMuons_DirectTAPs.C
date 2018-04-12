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

void CountStoppedMuons_DirectTAPs(){

  //////////////////////////////
  // User parameters
  //  std::string filename = "~/data/out/local/out09040_tme-tree_templates.root";
  std::string filename = "~/data/out/v13/SiL3.root";
  std::string outfilename = "~/data/results/SiL3_active/CountStoppedMuons_DirectTAPs.root";

  TFile* file = new TFile(filename.c_str(), "READ");
  TTree* taptree = (TTree*) file->Get("TAPTree/TAPTree");

  double min_energy = 0;
  double max_energy = 20000;
  double energy_width = 100;
  int n_energy_bins = (max_energy - min_energy) / energy_width;

  TH1F* hEnergy = new TH1F("hEnergy", "", n_energy_bins,min_energy,max_energy);
  hEnergy->SetTitle("Run 9040 SiL3 Energy (TAP Tree)");
  hEnergy->SetXTitle("Energy [keV]");
  hEnergy->SetYTitle("Raw Count");

  Int_t blockId;
  TBranch* br_blockId = taptree->GetBranch("blockId");
  br_blockId->SetAddress(&blockId);

  SimplePulse* sil3_s_tap = new SimplePulse();
  taptree->SetBranchAddress("SiL3_S", &sil3_s_tap);
  
  double min_muon_energy = 3000;
  double max_muon_energy = 6000;
  int n_stopped_muons = 0;

  std::cout << "Total TAPs = " << taptree->GetEntries() << std::endl;
  int n_entries = taptree->GetEntries();
  for (int i_tap = 0; i_tap < n_entries; ++i_tap) {
    taptree->GetEntry(i_tap);

    if (i_tap % 100000 == 0) {
      std::cout << i_tap << " / " << n_entries << std::endl;
    }

    SimplePulse* this_tap = sil3_s_tap;
    double Amp = this_tap->Amp;
    if (Amp<=0) {
      continue;
    }
    double E = this_tap->Amp*4.58 - 10.39; // new calibration
    if (E > 100) {
      hEnergy->Fill(E);
    }

    if (min_muon_energy <= E && E <= max_muon_energy) {
      ++n_stopped_muons;
    }
  }
  std::cout << n_stopped_muons << std::endl;

  int min_integral_bin = hEnergy->GetXaxis()->FindBin(min_muon_energy);
  int max_integral_bin = hEnergy->GetXaxis()->FindBin(max_muon_energy)-1;
  std::cout << "AE: " << hEnergy->GetXaxis()->GetBinLowEdge(min_integral_bin) << ", " << hEnergy->GetXaxis()->GetBinUpEdge(max_integral_bin) << std::endl;
  double integral_error = 0;
  double integral = hEnergy->IntegralAndError(min_integral_bin, max_integral_bin, integral_error);
  
  TFile* output = new TFile(outfilename.c_str(), "RECREATE");
  hEnergy->Write();
  
  TTree* tree = new TTree("mustops", "mustops");
  tree->Branch("min_muon_energy", &min_muon_energy);
  tree->Branch("max_muon_energy", &max_muon_energy);
  tree->Branch("direct_count", &n_stopped_muons);
  tree->Branch("integral", &integral);
  tree->Branch("integral_error", &integral_error);
  tree->Fill();
  tree->Write();

  output->Write();
  output->Close();

}
