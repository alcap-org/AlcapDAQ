#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include <iostream>
#include <sstream>

struct TargetSpectrumArgs {
  std::string infilename;
  std::string treename;

  std::string layername;
  bool layerveto;
  std::string vetolayername;
  bool layerreqd; // if we want to have a hit in the same event
  std::string reqdlayername;
  
  std::string outfilename;
  std::string outhistname;
  
  double min_energy;
  double max_energy;
  double energy_width;
  double min_time;
  double max_time;
  double time_width;
};

void TargetSpectrum(TargetSpectrumArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");
  TTree* targettree = (TTree*) in_file->Get(args.treename.c_str());

  int n_energy_bins = (args.max_energy - args.min_energy) / args.energy_width;
  int n_time_bins = (args.max_time - args.min_time) / args.time_width;

  TH2F* hEnergyTime = new TH2F(args.outhistname.c_str(), args.outhistname.c_str(), n_time_bins,args.min_time,args.max_time, n_energy_bins,args.min_energy,args.max_energy);
  hEnergyTime->SetXTitle("Time [ns]");
  hEnergyTime->SetYTitle("Energy [keV]");
  hEnergyTime->SetZTitle("Raw Count");

  std::string outtreename = args.outhistname + "_cuts";
  TTree* cuttree = new TTree(outtreename.c_str(), outtreename.c_str());

  double energy;
  double time;
  int layer_tpi_id;
  std::string energy_var = args.layername + "_energy";
  std::string time_var = args.layername + "_time";
  std::string tpi_id_var = args.layername + "_tpi_id";
  targettree->SetBranchAddress(energy_var.c_str(), &energy);
  targettree->SetBranchAddress(time_var.c_str(), &time);
  targettree->SetBranchAddress(tpi_id_var.c_str(), &layer_tpi_id);

  int veto_tpi_id;
  if (args.layerveto) {
    std::string veto_var = args.vetolayername + "_tpi_id";
    targettree->SetBranchAddress(veto_var.c_str(), &veto_tpi_id);
  }
  int reqd_tpi_id;
  if (args.layerreqd) {
    std::string reqd_var = args.reqdlayername + "_tpi_id";
    targettree->SetBranchAddress(reqd_var.c_str(), &reqd_tpi_id);
  }
  
  
  int n_entries = targettree->GetEntries();
  for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
    targettree->GetEntry(i_entry);
    if (i_entry % 100000 == 0) {
      std::cout << i_entry << " / " << n_entries << std::endl;
    }

    if (layer_tpi_id <0) {
      continue; // no hit in this layer
    }

    if (args.layerveto) {
      if (veto_tpi_id>-1) {
	continue; // to next entry
      }
    }

    if (args.layerreqd) {
      if (reqd_tpi_id<0) {
	continue; // to next entry
      }
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
