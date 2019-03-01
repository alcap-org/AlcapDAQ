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

  std::string cutfilename;
  std::string cuttreename;

  std::string outfilename;
  std::string outdirname;

  std::string corrfilename;
  std::string corrhistname;
  std::string corrtreename;
};

void Unfold_FlatBackground(const Unfold_FlatBackgroundArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem openeing file " << args.infilename.c_str() << std::endl;
    return;
  }
  TH1F* hRawSpectrum = (TH1F*) file->Get(args.inhistname.c_str());
  if (!hRawSpectrum) {
    std::cout << "Problem getting histogram " << args.inhistname.c_str() << std::endl;
    return;
  }
  TFile* cutfile = new TFile(args.cutfilename.c_str(), "READ");
  if (cutfile->IsZombie()) {
    std::cout << "Problem openeing cut file " << args.cutfilename.c_str() << std::endl;
    return;
  }
  TTree* incuttree = (TTree*) cutfile->Get(args.cuttreename.c_str());
  if (!incuttree) {
    std::cout << "Problem getting tree " << args.cuttreename.c_str() << std::endl;
    return;
  }

  double in_time_cut_min = 0;
  double in_time_cut_max = 0;
  incuttree->SetBranchAddress("min_time", &in_time_cut_min);
  incuttree->SetBranchAddress("max_time", &in_time_cut_max);
  incuttree->GetEntry(0);

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
  TTree* outcuttree = (TTree*) file->Get(args.corrtreename.c_str());
  if (!outcuttree) {
    std::cout << "Problem getting tree " << args.corrtreename.c_str() << std::endl;
    return;
  }
  double out_time_cut_min = 0;
  double out_time_cut_max = 0;
  outcuttree->SetBranchAddress("min_time", &out_time_cut_min);
  outcuttree->SetBranchAddress("max_time", &out_time_cut_max);
  outcuttree->GetEntry(0);
  
  TH1D* hCorrectedSpectrum = (TH1D*) hRawSpectrum->Clone("hCorrectedSpectrum");
  // Scale to be the same time width
  double scale_factor = (in_time_cut_max - in_time_cut_min) / (out_time_cut_max - out_time_cut_min);
  hCorrection->Scale(scale_factor);
  hCorrectedSpectrum->Add(hCorrection, -1);

  std::cout << "Unfolding FlatBackground: FlatBkg Histogram Has Time Cut = " << out_time_cut_min << " ns -- " << out_time_cut_max << " ns" << std::endl;
  std::cout << "Unfolding FlatBackground: RawSpectrum Histogram Has Time Cut = " << in_time_cut_min << " ns -- " << in_time_cut_max << " ns" << std::endl;
  std::cout << "Unfolding FlatBackground: Scale Factor = " << scale_factor << std::endl;

  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hRawSpectrum->Write();
  hCorrection->Write();
  hCorrectedSpectrum->Write();
  outfile->Write();
  outfile->Close();
}
