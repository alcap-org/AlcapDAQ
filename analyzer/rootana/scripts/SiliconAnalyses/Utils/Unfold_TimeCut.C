#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"

#include <iostream>
#include <sstream>

struct Unfold_TimeCutArgs {
  std::string infilename;
  std::string inhistname;

  std::string outfilename;
  std::string outdirname;

  double time_window_min;
  double time_window_max;
};

void Unfold_TimeCut(const Unfold_TimeCutArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem openeing file " << args.infilename.c_str() << std::endl;
    return;
  }
  TH1F* hInputSpectrum = (TH1F*) file->Get(args.inhistname.c_str());
  hInputSpectrum->SetName("hInputSpectrum");
  if (!hInputSpectrum) {
    std::cout << "Problem getting histogram " << args.inhistname.c_str() << std::endl;
  }

  double time_window_min = args.time_window_min;
  double time_window_max = args.time_window_max;
  double decay_lifetime = 767;
  double fraction_in_time_window = (TMath::Exp(-time_window_min/decay_lifetime) - TMath::Exp(-time_window_max/decay_lifetime));

  
  TH1D* hCorrectedSpectrum = (TH1D*) hInputSpectrum->Clone("hCorrectedSpectrum");
  hCorrectedSpectrum->Scale(1.0/fraction_in_time_window);

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hInputSpectrum->Write();
  hCorrectedSpectrum->Write();
  outfile->Write();
  outfile->Close();
}
