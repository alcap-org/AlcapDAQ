#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include <iostream>
#include <sstream>

struct CrossCheckSpectrumArgs {
  std::string infilename;
  std::string treename;
  std::string outfilename;
  std::string outhistname;

  std::string layername;
  bool layerveto;
  std::string vetolayername;
  
  double min_ge_energy;
  double max_ge_energy;
  double ge_energy_width;
  double min_ge_time;
  double max_ge_time;
  double ge_time_width;

  double min_muon_time;
  double max_muon_time;
  double min_muon_energy;
  double max_muon_energy;
};

void CrossCheckSpectrum(CrossCheckSpectrumArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");
  TTree* geTargetTree = (TTree*) in_file->Get(args.treename.c_str());

  int n_ge_energy_bins = (args.max_ge_energy - args.min_ge_energy) / args.ge_energy_width;
  int n_ge_time_bins = (args.max_ge_time - args.min_ge_time) / args.ge_time_width;
  std::string histname = args.outhistname;
  TH2F* hEnergyTime_XRay = new TH2F(histname.c_str(), histname.c_str(), n_ge_time_bins,args.min_ge_time,args.max_ge_time, n_ge_energy_bins,args.min_ge_energy,args.max_ge_energy);

  std::string outtreename = args.outhistname + "_cuts";
  TTree* cuttree = new TTree(outtreename.c_str(), outtreename.c_str());

  double ge_energy;
  double ge_time;
  double target_energy;
  double target_time;
  geTargetTree->SetBranchAddress("ge_energy", &ge_energy);
  geTargetTree->SetBranchAddress("ge_time", &ge_time);
  std::string target_energy_var = args.layername + "_energy";
  std::string target_time_var = args.layername + "_time";
  geTargetTree->SetBranchAddress(target_energy_var.c_str(), &target_energy);
  geTargetTree->SetBranchAddress(target_time_var.c_str(), &target_time);

  int veto_tpi_id;
  if (args.layerveto) {
    std::string veto_var = args.vetolayername + "_tpi_id";
    geTargetTree->SetBranchAddress(veto_var.c_str(), &veto_tpi_id);
  }

  
  int n_entries = geTargetTree->GetEntries();
  for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
    geTargetTree->GetEntry(i_entry);
    if (i_entry % 100000 == 0) {
      std::cout << i_entry << " / " << n_entries << std::endl;
    }

    if (args.layerveto) {
      if (veto_tpi_id>-1) {
	continue; // to the next entry
      }
    }

    if (target_energy > args.min_muon_energy && target_energy < args.max_muon_energy && target_time > args.min_muon_time && target_time < args.max_muon_time) {
      hEnergyTime_XRay->Fill(ge_time, ge_energy);
    }
  }
  
  std::cout << args.outhistname << " " << hEnergyTime_XRay->GetEntries() << " entries" << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  hEnergyTime_XRay->Write();
  cuttree->Branch("min_muon_energy", &args.min_muon_energy);
  cuttree->Branch("max_muon_energy", &args.max_muon_energy);
  cuttree->Branch("min_muon_time", &args.min_muon_time);
  cuttree->Branch("max_muon_time", &args.max_muon_time);
  cuttree->Fill();
  cuttree->Write();

  outfile->Write();
  outfile->Close();
}
