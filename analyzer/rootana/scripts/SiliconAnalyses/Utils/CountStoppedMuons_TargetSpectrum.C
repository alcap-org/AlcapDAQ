#ifndef CountStoppedMuons_TargetSpectrum_C_
#define CountStoppedMuons_TargetSpectrum_C_

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include "scripts/XRayAnalysis/XRayUtils.h"

struct CountStoppedMuons_TargetSpectrumArgs {
  std::string infilename;
  std::string inhistname;
  std::string outfilename;
  std::string outtreename;

  double min_muon_time;
  double max_muon_time;
  double min_muon_energy;
  double max_muon_energy;
};

void CountStoppedMuons_TargetSpectrum(CountStoppedMuons_TargetSpectrumArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  TH2F* hEnergyTime = (TH2F*) file->Get(args.inhistname.c_str());

  int min_time_bin = hEnergyTime->GetXaxis()->FindBin(args.min_muon_time);
  int max_time_bin = hEnergyTime->GetXaxis()->FindBin(args.max_muon_time)-1;
  TH1F* hTarget_Spectrum = (TH1F*) hEnergyTime->ProjectionY("hTarget_Spectrum", min_time_bin, max_time_bin);

  int min_energy_bin = hTarget_Spectrum->FindBin(args.min_muon_energy);
  int max_energy_bin = hTarget_Spectrum->FindBin(args.max_muon_energy)-1;
  
  double n_stopped_muons = 0;
  double n_stopped_muons_error = 0;
  n_stopped_muons = hTarget_Spectrum->IntegralAndError(min_energy_bin, max_energy_bin, n_stopped_muons_error);
  std::cout << "AE: N stopped muons from integral = " << n_stopped_muons << " +/- " << n_stopped_muons_error << std::endl;

  //  TF1* fit = new TF1("fit", "[0]*TMath::Gaus(x, [1], [2])+[3]*x+[4]", args.min_muon_energy, args.max_muon_energy);
  //  fit->SetParameters(hTarget_Spectrum->GetMaximum(), args.min_muon_energy + (args.max_muon_energy-args.min_muon_energy)/2, 1000, 0.1, 1000);
  //  hTarget_Spectrum->Fit(fit, "R");

  //  n_stopped_muons = fit->GetParameter(0) * fit->GetParameter(2) * (std::sqrt(2*3.14)/hTarget_Spectrum->GetXaxis()->GetBinWidth(1));
  //  std::cout << "AE: Direct Count (from fit) = " << n_stopped_muons << std::endl;
  TTree* direct_count_tree = new TTree(args.outtreename.c_str(), "");
  direct_count_tree->Branch("n_stopped_muons", &n_stopped_muons);
  direct_count_tree->Branch("n_stopped_muons_error", &n_stopped_muons_error);
  direct_count_tree->Branch("min_muon_time", &args.min_muon_time);
  direct_count_tree->Branch("max_muon_time", &args.max_muon_time);
  direct_count_tree->Branch("min_muon_energy", &args.min_muon_energy);
  direct_count_tree->Branch("max_muon_energy", &args.max_muon_energy);
  direct_count_tree->Fill();
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  hTarget_Spectrum->Write();
  direct_count_tree->Write();
  outfile->Write();
  outfile->Close();
}

#endif
