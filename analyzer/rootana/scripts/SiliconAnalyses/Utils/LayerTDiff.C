#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include <iostream>
#include <sstream>

struct LayerTDiffArgs {
  std::string infilename;
  std::string treename;

  std::string layer1name;
  std::string layer2name;
  
  std::string outfilename;
  std::string outdirname;
  
  double min_tdiff;
  double max_tdiff;
  double tdiff_width;
};

void LayerTDiff(LayerTDiffArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");
  TTree* tree = (TTree*) in_file->Get(args.treename.c_str());

  int n_tdiff_bins = (args.max_tdiff - args.min_tdiff) / args.tdiff_width;

  TH1F* hLayerTDiff = new TH1F("hLayerTDiff", "hLayerTDiff", n_tdiff_bins,args.min_tdiff,args.max_tdiff);
  std::string axistitle = "t_{" + args.layer2name + "} - t_{" + args.layer1name + "} [ns]";
  hLayerTDiff->SetXTitle(axistitle.c_str());
  hLayerTDiff->SetYTitle("Raw Count");

  std::string outtreename = "cuttree";
  TTree* cuttree = new TTree(outtreename.c_str(), outtreename.c_str());

  double layer1_time;
  double layer2_time;
  int layer1_tpi_id;
  int layer2_tpi_id;
  std::string layer1_time_var = args.layer1name + "_time";
  std::string layer2_time_var = args.layer2name + "_time";
  std::string layer1_tpi_id_var = args.layer1name + "_tpi_id";
  std::string layer2_tpi_id_var = args.layer2name + "_tpi_id";
  tree->SetBranchAddress(layer1_time_var.c_str(), &layer1_time);
  tree->SetBranchAddress(layer2_time_var.c_str(), &layer2_time);
  tree->SetBranchAddress(layer1_tpi_id_var.c_str(), &layer1_tpi_id);
  tree->SetBranchAddress(layer2_tpi_id_var.c_str(), &layer2_tpi_id);

  cuttree->Branch("layer1", &layer1_time_var);
  cuttree->Branch("layer2", &layer2_time_var);
      
  int n_entries = tree->GetEntries();
  for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
    tree->GetEntry(i_entry);
    if (i_entry % 100000 == 0) {
      std::cout << i_entry << " / " << n_entries << std::endl;
    }

    if (layer1_tpi_id<0 || layer2_tpi_id<0) { // need two layers
      continue; // to next entry
    }

    hLayerTDiff->Fill(layer2_time - layer1_time);
  }
  
  std::cout << "LayerTDiff (" << args.outdirname << "): " << hLayerTDiff->GetName() << " " << hLayerTDiff->GetEntries() << " entries" << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hLayerTDiff->Write();
  cuttree->Fill();
  cuttree->Write();

  outfile->Write();
  outfile->Close();
}
