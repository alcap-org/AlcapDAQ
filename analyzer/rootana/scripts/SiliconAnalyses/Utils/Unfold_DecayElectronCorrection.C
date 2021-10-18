#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"

#include <iostream>
#include <sstream>

struct Unfold_DecayElectronCorrectionArgs {
  std::string infilename;
  std::string inhistname;

  std::string outfilename;
  std::string outdirname;

  std::string corrfilename;
  std::string corrhistname;

  std::string countfilename;
  std::string counttreename;

  double capture_fraction;
};

void Unfold_DecayElectronCorrection(const Unfold_DecayElectronCorrectionArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem openeing file " << args.infilename << std::endl;
    return;
  }
  TH1F* hRawSpectrum = (TH1F*) file->Get(args.inhistname.c_str());
  if (!hRawSpectrum) {
    std::cout << "Problem getting histogram " << args.inhistname << std::endl;
    return;
  }
  //  hRawSpectrum->Rebin(2);
  hRawSpectrum->SetName("hInputSpectrum");

  TFile* corrfile = new TFile(args.corrfilename.c_str(), "READ");
  if (corrfile->IsZombie()) {
    std::cout << "Problem openeing corrfile " << args.corrfilename << std::endl;
    return;
  }
  TH1F* hMuPlusEDep = (TH1F*) corrfile->Get(args.corrhistname.c_str());
  if (!hMuPlusEDep) {
    std::cout << "Problem getting corr histogram " << args.corrhistname << std::endl;
    return;
  }

  TH1F* hCorrection = (TH1F*) hMuPlusEDep->Clone("hCorrection"); //->ProjectionY("hCorrection", min_time_bin, max_time_bin);
  if (!hCorrection) {
    std::cout << "Problem copying correction histogram" << std::endl;
    return;
  }
  /*  hCorrection->Reset();
  // shift things to the right
  int bin_shift = 1;
  for (int i_bin = 1; i_bin < hMuPlusEDep->GetNbinsX(); ++i_bin) {
    double this_bin_content = hMuPlusEDep->GetBinContent(i_bin);
    hCorrection->SetBinContent(i_bin+bin_shift, this_bin_content);
  }
  */
  
  
  TFile* countfile = new TFile(args.countfilename.c_str(), "READ");
  if (countfile->IsZombie()) {
    std::cout << "Problem opening countfile " << args.countfilename << std::endl;
    return;
  }
  TTree* counttree = (TTree*) countfile->Get(args.counttreename.c_str());
  if (!counttree) {
    std::cout << "Problem getting counttree " << args.counttreename << std::endl;
    return;
  }
  double n_stopped_muons = 0;
  double n_stopped_muons_error = 0;
  counttree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
  counttree->SetBranchAddress("n_stopped_muons_error", &n_stopped_muons_error);
  counttree->GetEntry(0);

  double n_decay_muons = (1-args.capture_fraction)*n_stopped_muons;
  double n_decay_muons_error = (1-args.capture_fraction)*n_stopped_muons_error; 

  //  double scale = n_decay_muons;
  //  hCorrection->Scale(scale);
  //  hCorrection->Rebin(2);
  TH1F* hScale = (TH1F*) hCorrection->Clone("hScale");
  hScale->Reset();
  for (int i_bin = 1; i_bin <= hScale->GetNbinsX(); ++i_bin) {
    hScale->SetBinContent(i_bin, n_decay_muons);
    hScale->SetBinError(i_bin, n_decay_muons_error);
  }
  hCorrection->Multiply(hScale);
  
  TH1D* hCorrectedSpectrum = (TH1D*) hRawSpectrum->Clone("hCorrectedSpectrum");
  std::cout << "Unfolding DecayElectron: n_stopped_muons = " << n_stopped_muons << " +/- " << n_stopped_muons_error << std::endl;
  std::cout << "Unfolding DecayElectron: n_decay_muons = " << n_decay_muons << " +/- " << n_decay_muons_error << std::endl;
  std::cout << "Unfolding DecayElectron: hRawSpectrum Bin Width = " << hRawSpectrum->GetXaxis()->GetBinWidth(1) << " keV, Min = " << hRawSpectrum->GetXaxis()->GetXmin() << ", Max = " << hRawSpectrum->GetXaxis()->GetXmax() << std::endl;
  std::cout << "Unfolding DecayElectron: hCorrection Bin Width = " << hCorrection->GetXaxis()->GetBinWidth(1) << " keV, Min = " << hCorrection->GetXaxis()->GetXmin() << ", Max = " << hCorrection->GetXaxis()->GetXmax() << std::endl;
  hCorrectedSpectrum->Add(hCorrection, -1);

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hRawSpectrum->Write();
  hCorrection->Write();
  hCorrectedSpectrum->Write();
  hScale->Write();
  outfile->Write();
  outfile->Close();

  file->Close();
  corrfile->Close();
  countfile->Close();
}
