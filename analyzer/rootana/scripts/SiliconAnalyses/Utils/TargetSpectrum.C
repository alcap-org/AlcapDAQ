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
  double layer_coincidence_time;
  
  std::string outfilename;
  std::string outdirname;
  
  double min_energy;
  double max_energy;
  double energy_width;
  double min_time;
  double max_time;
  double time_width;

  double recoil_fraction; // if >=0 then it's a fraction, if it's <0 then it's an absolute value that needs to be subtracted
};

void TargetSpectrum(TargetSpectrumArgs& args) {
  TFile* in_file = new TFile(args.infilename.c_str(), "READ");
  TTree* targettree = (TTree*) in_file->Get(args.treename.c_str());

  int n_energy_bins = (args.max_energy - args.min_energy) / args.energy_width;
  int n_time_bins = (args.max_time - args.min_time) / args.time_width;

  TH2F* hEnergyTime = new TH2F("hEnergyTime", "hEnergyTime", n_time_bins,args.min_time,args.max_time, n_energy_bins,args.min_energy,args.max_energy);
  hEnergyTime->SetXTitle("Time [ns]");
  hEnergyTime->SetYTitle("Energy [keV]");
  hEnergyTime->SetZTitle("Raw Count");

  std::string outtreename = "cuttree";
  TTree* cuttree = new TTree(outtreename.c_str(), outtreename.c_str());

  double energy;
  double time;
  int layer_tpi_id;
  std::string energy_var = args.layername;
  if (args.layername!="") { energy_var += "_"; }
  energy_var += "energy";
  std::string time_var = args.layername;
  if (args.layername!="") { time_var += "_"; }
  time_var += "time";
  std::string tpi_id_var = args.layername;
  if (args.layername!="") { tpi_id_var += "_"; }
  tpi_id_var += "tpi_id";
  targettree->SetBranchAddress(energy_var.c_str(), &energy);
  targettree->SetBranchAddress(time_var.c_str(), &time);
  targettree->SetBranchAddress(tpi_id_var.c_str(), &layer_tpi_id);

  cuttree->Branch("energy_var", &energy_var);
  cuttree->Branch("time_var", &time_var);
  cuttree->Branch("tpi_id_var", &tpi_id_var);

  double recoil_fraction = args.recoil_fraction;
  cuttree->Branch("recoil_fraction", &recoil_fraction);
  
  int veto_tpi_id;
  double veto_time;
  std::string veto_tpi_id_var;
  std::string veto_time_var;
  if (args.layerveto) {
    veto_tpi_id_var = args.vetolayername + "_tpi_id";
    targettree->SetBranchAddress(veto_tpi_id_var.c_str(), &veto_tpi_id);
    cuttree->Branch("veto_tpi_id_var", &veto_tpi_id_var);
    
    veto_time_var = args.vetolayername + "_time";
    targettree->SetBranchAddress(veto_time_var.c_str(), &veto_time);
    cuttree->Branch("veto_time_var", &veto_time_var);

    cuttree->Branch("layer_coincidence_time", &args.layer_coincidence_time);
  }
  int reqd_tpi_id;
  double reqd_time;
  std::string reqd_tpi_id_var;
  std::string reqd_time_var;
  if (args.layerreqd) {
    reqd_tpi_id_var = args.reqdlayername + "_tpi_id";
    targettree->SetBranchAddress(reqd_tpi_id_var.c_str(), &reqd_tpi_id);
    cuttree->Branch("reqd_tpi_id_var", &reqd_tpi_id_var);

    reqd_time_var = args.reqdlayername + "_time";
    targettree->SetBranchAddress(reqd_time_var.c_str(), &reqd_time);
    cuttree->Branch("reqd_time_var", &reqd_time_var);

    cuttree->Branch("layer_coincidence_time", &args.layer_coincidence_time);
  }
    
  int n_entries = targettree->GetEntries();
  for (int i_entry = 0; i_entry < n_entries; ++i_entry) {
    targettree->GetEntry(i_entry);
    if (i_entry % 100000 == 0) {
      std::cout << i_entry << " / " << n_entries << std::endl;
    }

    // Convert energy to MeV
    energy = energy * 1e-3;

    if (layer_tpi_id<0) {
      continue; // no hit in this layer
    }

    if (args.layerveto) {
      if (veto_tpi_id>-1 && std::fabs(veto_time - time)<args.layer_coincidence_time) {
	continue; // to next entry
      }
    }

    if (args.layerreqd) {
      if (reqd_tpi_id<0 || std::fabs(reqd_time - time)>args.layer_coincidence_time) {
	continue; // to next entry
      }
    }

    if (recoil_fraction>=0) {
      hEnergyTime->Fill(time, energy/(1+recoil_fraction));
    }
    else {
      hEnergyTime->Fill(time, energy - recoil_fraction);
    }
  }
  
  std::cout << "TargetSpectrum (" << args.outdirname << "): " << hEnergyTime->GetName() << " " << hEnergyTime->GetEntries() << " entries" << std::endl;
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hEnergyTime->Write();
  cuttree->Fill();
  cuttree->Write();

  outfile->Write();
  outfile->Close();
}
