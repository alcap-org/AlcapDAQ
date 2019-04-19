#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"

#include <iostream>
#include <sstream>

struct XRaySpectrum_wTargetCoincArgs {
  std::string infilename;
  std::string treename;
  std::string outfilename;
  std::string outdirname;

  std::string layername;
  bool layerveto;
  std::string vetolayername;
  bool layerreqd; // if we want to have a hit in the same event
  std::string reqdlayername;
  double layer_coincidence_time;
  
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

void XRaySpectrum_wTargetCoinc(XRaySpectrum_wTargetCoincArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");
  TTree* geTargetTree = (TTree*) in_file->Get(args.treename.c_str());

  int n_ge_energy_bins = (args.max_ge_energy - args.min_ge_energy) / args.ge_energy_width;
  int n_ge_time_bins = (args.max_ge_time - args.min_ge_time) / args.ge_time_width;
  TH2F* hEnergyTime = new TH2F("hEnergyTime", "hEnergyTime", n_ge_time_bins,args.min_ge_time,args.max_ge_time, n_ge_energy_bins,args.min_ge_energy,args.max_ge_energy);

  std::string outtreename = "cuttree";
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

  cuttree->Branch("target_energy_var", &target_energy_var);
  cuttree->Branch("target_time_var", &target_time_var);

  int veto_tpi_id;
  double veto_time;
  std::string veto_tpi_id_var;
  std::string veto_time_var;
  if (args.layerveto) {
    veto_tpi_id_var = args.vetolayername + "_tpi_id";
    geTargetTree->SetBranchAddress(veto_tpi_id_var.c_str(), &veto_tpi_id);
    cuttree->Branch("veto_tpi_id_var", &veto_tpi_id_var);
    
    veto_time_var = args.vetolayername + "_time";
    geTargetTree->SetBranchAddress(veto_time_var.c_str(), &veto_time);
    cuttree->Branch("veto_time_var", &veto_time_var);

    cuttree->Branch("layer_coincidence_time", &args.layer_coincidence_time);
  }

  int reqd_tpi_id;
  double reqd_time;
  std::string reqd_tpi_id_var;
  std::string reqd_time_var;
  if (args.layerreqd) {
    reqd_tpi_id_var = args.reqdlayername + "_tpi_id";
    geTargetTree->SetBranchAddress(reqd_tpi_id_var.c_str(), &reqd_tpi_id);
    cuttree->Branch("reqd_tpi_id_var", &reqd_tpi_id_var);

    reqd_time_var = args.reqdlayername + "_time";
    geTargetTree->SetBranchAddress(reqd_time_var.c_str(), &reqd_time);
    cuttree->Branch("reqd_time_var", &reqd_time_var);

    cuttree->Branch("layer_coincidence_time", &args.layer_coincidence_time);
  }


  
  int n_entries = geTargetTree->GetEntries();
  for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
    geTargetTree->GetEntry(i_entry);
    if (i_entry % 100000 == 0) {
      std::cout << i_entry << " / " << n_entries << std::endl;
    }

    if (args.layerveto) {
      if (veto_tpi_id>-1 && std::fabs(veto_time - target_time)<args.layer_coincidence_time) {
	continue; // to the next entry
      }
    }

    if (args.layerreqd) {
      if (reqd_tpi_id<0 || std::fabs(reqd_time - target_time)>args.layer_coincidence_time) {
	continue; // to next entry
      }
    }


    if (target_energy > args.min_muon_energy && target_energy < args.max_muon_energy && target_time > args.min_muon_time && target_time < args.max_muon_time) {
      hEnergyTime->Fill(ge_time, ge_energy);
    }
  }
  
  std::cout << "XRaySpectrum_wTargetCoinc (" << args.outdirname << "): " << hEnergyTime->GetName() << " " << hEnergyTime->GetEntries() << " entries" << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hEnergyTime->Write();
  cuttree->Branch("min_muon_energy", &args.min_muon_energy);
  cuttree->Branch("max_muon_energy", &args.max_muon_energy);
  cuttree->Branch("min_muon_time", &args.min_muon_time);
  cuttree->Branch("max_muon_time", &args.max_muon_time);
  cuttree->Fill();
  cuttree->Write();

  outfile->Write();
  outfile->Close();
}
