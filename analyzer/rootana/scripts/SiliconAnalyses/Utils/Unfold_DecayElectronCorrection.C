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
  double time_window_min;
  double time_window_max;
  double geom_acceptance;

  std::string countfilename;
  std::string counttreename;
};

void Unfold_DecayElectronCorrection(const Unfold_DecayElectronCorrectionArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem openeing file " << args.infilename << std::endl;
    return;
  }
  TH1F* hRawSpectrum = (TH1F*) file->Get(args.inhistname.c_str());
  hRawSpectrum->SetName("hInputSpectrum");
  //  hRawSpectrum->Rebin(2);
  if (!hRawSpectrum) {
    std::cout << "Problem getting histogram " << args.inhistname << std::endl;
    return;
  }

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
  hCorrection->Reset();
  // shift things to the right
  int bin_shift = 0;
  for (int i_bin = 1; i_bin < hMuPlusEDep->GetNbinsX(); ++i_bin) {
    double this_bin_content = hMuPlusEDep->GetBinContent(i_bin);
    hCorrection->SetBinContent(i_bin+bin_shift, this_bin_content);
  }

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
  counttree->SetBranchAddress("n_stopped_muons", &n_stopped_muons);
  counttree->GetEntry(0);
  
  double capture_fraction = 0.658;
  double n_decay_muons = (1-capture_fraction)*n_stopped_muons;
  double geom_acceptance = args.geom_acceptance;
  double time_window_min = args.time_window_min;
  double time_window_max = args.time_window_max;
  double decay_lifetime = 767;
  double fraction_in_time_window = (TMath::Exp(-time_window_min/decay_lifetime) - TMath::Exp(-time_window_max/decay_lifetime));
  double scale = n_decay_muons*fraction_in_time_window*args.geom_acceptance;
  //  double scale = hRawSpectrum->GetBinContent(hRawSpectrum->GetXaxis()->FindBin(100)) / hCorrection->GetBinContent(hCorrection->GetXaxis()->FindBin(100));
  std::cout << "AE: N Stop = " << n_stopped_muons << std::endl;
  std::cout << "AE: N Decay = " << (1-capture_fraction) << "*" << n_stopped_muons << " = " << n_decay_muons << std::endl;
  std::cout << "AE: Peak (before scaling) = " << hCorrection->GetMaximum() << std::endl;
  std::cout << "AE: Integral (before scaling) = " << hCorrection->Integral() << std::endl;
  std::cout << "AE: Scale = " << args.geom_acceptance << "*" << fraction_in_time_window << "*" << n_decay_muons << " = " << scale << std::endl;
  hCorrection->Scale(scale);
  std::cout << "AE: Peak (after scaling) = " << hCorrection->GetMaximum() << std::endl;
  std::cout << "AE: Integral (after scaling) = " << hCorrection->Integral() << std::endl;
  
  TH1D* hCorrectedSpectrum = (TH1D*) hRawSpectrum->Clone("hCorrectedSpectrum");
  hCorrectedSpectrum->Add(hCorrection, -1);

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hRawSpectrum->Write();
  hCorrection->Write();
  hCorrectedSpectrum->Write();
  outfile->Write();
  outfile->Close();
}
