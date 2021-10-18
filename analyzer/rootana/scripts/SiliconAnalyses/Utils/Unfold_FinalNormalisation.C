#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"

#include <iostream>
#include <sstream>

struct Unfold_FinalNormalisationArgs {
  std::string infilename;
  std::string inhistname;

  std::string outfilename;
  std::string outdirname;

  std::string countfilename;
  std::string counttreename;

  double capture_fraction;
  int rebin_factor;
};

void Unfold_FinalNormalisation(const Unfold_FinalNormalisationArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem opening file " << args.infilename << std::endl;
    return;
  }
  TH1F* hUnfoldedSpectrum = (TH1F*) file->Get(args.inhistname.c_str());
  if (!hUnfoldedSpectrum) {
    std::cout << "Problem getting histogram " << args.inhistname << std::endl;
    return;
  }
  hUnfoldedSpectrum->SetName("hUnfoldedSpectrum");
  TH1D* hNormalisedSpectrum = (TH1D*) hUnfoldedSpectrum->Clone("hNormalisedSpectrum");
  
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
  
  double n_captured_muons = args.capture_fraction*n_stopped_muons;
  double n_captured_muons_error = args.capture_fraction*n_stopped_muons_error;

  TH1F* hScale = (TH1F*) hNormalisedSpectrum->Clone("hScale");
  hScale->Reset();
  for (int i_bin = 1; i_bin <= hScale->GetNbinsX(); ++i_bin) {
    hScale->SetBinContent(i_bin, n_captured_muons);
    hScale->SetBinError(i_bin, n_captured_muons_error);
  }
  
  //  double scale = 1.0 / n_captured_muons;
  //  hNormalisedSpectrum->Scale(scale);
  hNormalisedSpectrum->Divide(hScale);
  hNormalisedSpectrum->SetYTitle("Rate of Charged Particle Emission / captured muon");

  hNormalisedSpectrum->Rebin(args.rebin_factor);
  double scale = 1.0 / hNormalisedSpectrum->GetXaxis()->GetBinWidth(1);
  hNormalisedSpectrum->Scale(scale);
  hNormalisedSpectrum->SetYTitle("Rate of Charged Particle Emission / captured muon / MeV");
  
  std::cout << "Unfolding FinalNormalisation: n_stopped_muons = " << n_stopped_muons << " +/- " << n_stopped_muons_error << std::endl;
  std::cout << "Unfolding FinalNormalisation: n_captured_muons = " << n_captured_muons << " +/- " << n_captured_muons_error << std::endl;

  // TF1* spectral_fit = new TF1("spectral_fit","[0]*(1 - [1]/x)^[2]*(exp(-x/[3]))",1400,30000);//27000);
  // //  TF1* spectral_fit = new TF1("spectral_fit","[0]*(1 - [1]/x)^[2]*(exp(-x/[3])+[4]*exp(-x/[5]))",1000,27000);
  // spectral_fit->SetParName(0, "N");
  // spectral_fit->SetParName(1, "T_{th}");
  // spectral_fit->SetParName(2, "#alpha");
  // spectral_fit->SetParName(3, "T_{0}");
  // spectral_fit->SetParameter(0, 1e-6);
  // spectral_fit->SetParameter(1, 1400);
  // spectral_fit->SetParLimits(1, 0, 5000);
  // spectral_fit->SetParameter(2, 1.328);
  // spectral_fit->SetParLimits(2, 0, 5);
  // spectral_fit->SetParameter(3, 4600);
  // //  spectral_fit->SetParName(4, "r");
  // //  spectral_fit->SetParName(5, "T_{1}");
  // //  spectral_fit->SetParameter(4, 0.5);
  // //  spectral_fit->SetParLimits(4, 0, 100);
  // //  spectral_fit->SetParameter(5, 3800);
  // //  spectral_fit->SetParLimits(5, 0, 5000);
  // //  TFitResultPtr fit_result = hNormalisedSpectrum->Fit(spectral_fit, "RMES");
  // //  if ( (Int_t) fit_result != 0) {
  // //    std::cout << "Unfolding FinalNormalisation: Fit Failed! Returning..." << std::endl;
  // //    return;
  // //  }
  // spectral_fit->SetLineWidth(2);

  
  // const int n_ranges = 5;
  // double min_energies[n_ranges] = {1.4,10, 5, 3.5, 4};
  // double max_energies[n_ranges] = {26, 26, 10, 10, 8};
  
  // for (int i_range = 0; i_range < n_ranges; ++i_range) {
  //   double min_energy = min_energies[i_range];
  //   double max_energy = max_energies[i_range];
  //   std::cout << "Integral of fit (" << min_energy << " MeV -- " << max_energy << " MeV) = " << spectral_fit->Integral(min_energy, max_energy) << " #pm "  << spectral_fit->IntegralError(min_energy, max_energy) << std::endl;

  //   int min_integral_bin = hNormalisedSpectrum->GetXaxis()->FindBin(min_energy);
  //   int max_integral_bin = hNormalisedSpectrum->GetXaxis()->FindBin(max_energy);
  //   double error = 0;
  //   std::cout << "Integral of hist (" << min_energy / 1000 << " MeV -- " << max_energy / 1000 << " MeV) = " << hNormalisedSpectrum->IntegralAndError(min_integral_bin, max_integral_bin, error, "width") << " #pm "  << error << std::endl;
  // }

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hUnfoldedSpectrum->Write();
  hNormalisedSpectrum->Write();
  hScale->Write();
  //  spectral_fit->Write();
  outfile->Write();
  outfile->Close();

  file->Close();
  countfile->Close();
}
