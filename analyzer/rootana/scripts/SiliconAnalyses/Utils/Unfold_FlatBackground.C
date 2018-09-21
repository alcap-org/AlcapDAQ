#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"

#include <iostream>
#include <sstream>

struct Unfold_FlatBackgroundArgs {
  std::string infilename;
  std::string inhistname;

  std::string outfilename;
  std::string outdirname;

  std::string corrfilename;
  std::string corrhistname;
  double corr_time_cut_min;
  double corr_time_cut_max;
};

void Unfold_FlatBackground(const Unfold_FlatBackgroundArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem openeing file " << args.infilename.c_str() << std::endl;
    return;
  }
  TH1F* hRawSpectrum = (TH1F*) file->Get(args.inhistname.c_str());
  hRawSpectrum->SetName("hInputSpectrum");
  if (!hRawSpectrum) {
    std::cout << "Problem getting histogram " << args.inhistname.c_str() << std::endl;
  }

  TFile* corrfile = new TFile(args.corrfilename.c_str(), "READ");
  if (corrfile->IsZombie()) {
    std::cout << "Problem openeing corrfile " << args.corrfilename.c_str() << std::endl;
    return;
  }
  TH1F* hCorrection = (TH1F*) corrfile->Get(args.corrhistname.c_str());
  if (!hCorrection) {
    std::cout << "Problem getting corr histogram " << args.corrhistname.c_str() << std::endl;
    return;
  }
  hCorrection->SetName("hCorrection");

  
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
