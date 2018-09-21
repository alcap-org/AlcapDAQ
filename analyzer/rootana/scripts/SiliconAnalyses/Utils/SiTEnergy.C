#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"

#include <iostream>
#include <sstream>

struct SiTEnergyArgs {
  std::string infilename;
  std::string treename;

  std::string outfilename;
  std::string outhistname;
  std::string outdirname;
  
  double min_energy;
  double max_energy;
  double energy_width;
};

void SiTEnergy(SiTEnergyArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");
  TTree* sittree = (TTree*) in_file->Get(args.treename.c_str());

  int n_energy_bins = (args.max_energy - args.min_energy) / args.energy_width;

  TH1F* hSiTEnergy = new TH1F(args.outhistname.c_str(), args.outhistname.c_str(), n_energy_bins,args.min_energy,args.max_energy);
  hSiTEnergy->SetXTitle("Energy [keV]");
  hSiTEnergy->SetYTitle("Raw Count");

  double energy;
  std::string energy_var = "thin_energy";
  sittree->SetBranchAddress(energy_var.c_str(), &energy);

  int n_entries = sittree->GetEntries();
  for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
    sittree->GetEntry(i_entry);
    if (i_entry % 100000 == 0) {
      std::cout << i_entry << " / " << n_entries << std::endl;
    }

    hSiTEnergy->Fill(energy);
  }
  
  std::cout << args.outhistname << " " << hSiTEnergy->GetEntries() << " entries" << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hSiTEnergy->Write();

  outfile->Write();
  outfile->Close();
}
