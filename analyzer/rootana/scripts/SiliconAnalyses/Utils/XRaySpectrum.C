#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include <iostream>
#include <sstream>

struct XRaySpectrumArgs {
  std::string infilename;
  std::string treename;
  std::string outfilename;
  std::string outhistname;
  
  double min_energy;
  double max_energy;
  double energy_width;
  double min_time;
  double max_time;
  double time_width;
};

void XRaySpectrum(XRaySpectrumArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");
  TTree* geTree = (TTree*) in_file->Get(args.treename.c_str());

  int n_energy_bins = (args.max_energy - args.min_energy) / args.energy_width;
  int n_time_bins = (args.max_time - args.min_time) / args.time_width;

  TH2F* hEnergyTime = new TH2F(args.outhistname.c_str(), args.outhistname.c_str(), n_time_bins,args.min_time,args.max_time, n_energy_bins,args.min_energy,args.max_energy);

  std::string outtreename = args.outhistname + "_cuts";
  TTree* cuttree = new TTree(outtreename.c_str(), outtreename.c_str());

  double energy;
  double time;
  geTree->SetBranchAddress("energy", &energy);
  geTree->SetBranchAddress("time", &time);
  
  int n_entries = geTree->GetEntries();
  for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
    geTree->GetEntry(i_entry);
    if (i_entry % 100000 == 0) {
      std::cout << i_entry << " / " << n_entries << std::endl;
    }

    hEnergyTime->Fill(time, energy);
  }
  
  std::cout << args.outhistname << " " << hEnergyTime->GetEntries() << " entries" << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  hEnergyTime->Write();
  cuttree->Fill();
  cuttree->Write();

  outfile->Write();
  outfile->Close();
}
