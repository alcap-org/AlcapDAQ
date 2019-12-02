#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"

#include <iostream>
#include <sstream>

struct Unfold_PIDCutEfficiencyArgs {
  std::string infilename;
  std::string inhistname;
  
  std::string outfilename;
  std::string outdirname;

  std::string corrfilename;
  std::string particle;
  int rebin_factor;
};

void Unfold_PIDCutEfficiency(const Unfold_PIDCutEfficiencyArgs& args) {
  
  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem opening file " << args.infilename.c_str() << std::endl;
    return;
  }
  TH1F* hInputSpectrum = (TH1F*) file->Get(args.inhistname.c_str());
  if (!hInputSpectrum) {
    std::cout << "Problem getting histogram " << args.inhistname.c_str() << std::endl;
    return;
  }
  hInputSpectrum->SetName("hInputSpectrum");
  hInputSpectrum->Rebin(args.rebin_factor); // rebin to match the binning of the eff and purity histograms

  TFile* corrfile = new TFile(args.corrfilename.c_str(), "READ");
  if (corrfile->IsZombie()) {
    std::cout << "Problem opening file " << args.corrfilename.c_str() << std::endl;
    return;
  }
  std::string h_eff_name = "eff_" + args.particle;
  TH1F* hEffSpectrum = (TH1F*) corrfile->Get(h_eff_name.c_str());
  if (!hEffSpectrum) {
    std::cout << "Problem getting histogram " << h_eff_name << std::endl;
    return;
  }
  std::string h_purity_name = "purity_" + args.particle;
  TH1F* hPuritySpectrum = (TH1F*) corrfile->Get(h_purity_name.c_str());
  if (!hPuritySpectrum) {
    std::cout << "Problem getting histogram " << h_purity_name << std::endl;
    return;
  }
  
  std::cout << "Unfolding PIDCutEfficiency..." << std::endl;
  
  TH1D* hCorrectedSpectrum = (TH1D*) hInputSpectrum->Clone("hCorrectedSpectrum");
  hCorrectedSpectrum->Divide(hEffSpectrum);
  hCorrectedSpectrum->Multiply(hPuritySpectrum);
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hInputSpectrum->Write();
  hEffSpectrum->Write();
  hPuritySpectrum->Write();
  hCorrectedSpectrum->Write();
  outfile->Write();
  outfile->Close();

  file->Close();
  corrfile->Close();
}
