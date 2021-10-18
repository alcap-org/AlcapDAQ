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
  std::string incutfilename;
  std::string incuttreename;
  
  std::string outfilename;
  std::string outdirname;

  std::string min_time_cut_branch;
  std::string max_time_cut_branch;

  double decay_lifetime;
  double extra_efficiency;
};

void Unfold_TimeCut(const Unfold_TimeCutArgs& args) {

  TFile* file = new TFile(args.infilename.c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem openeing file " << args.infilename.c_str() << std::endl;
    return;
  }
  TH1F* hInputSpectrum = (TH1F*) file->Get(args.inhistname.c_str());
  if (!hInputSpectrum) {
    std::cout << "Problem getting histogram " << args.inhistname.c_str() << std::endl;
    return;
  }
  hInputSpectrum->SetName("hInputSpectrum");
  TFile* cutfile = new TFile(args.incutfilename.c_str(), "READ");
  if (cutfile->IsZombie()) {
    std::cout << "Problem openeing cutfile " << args.incutfilename.c_str() << std::endl;
    return;
  }
  TTree* cuttree = (TTree*) cutfile->Get(args.incuttreename.c_str());
  if (!cuttree) {
    std::cout << "Problem getting cuttree " << args.incuttreename.c_str() << std::endl;
    return;
  }  
  double time_window_min = 0;
  double time_window_max = 0;
  cuttree->SetBranchAddress(args.min_time_cut_branch.c_str(), &time_window_min);
  cuttree->SetBranchAddress(args.max_time_cut_branch.c_str(), &time_window_max);
  cuttree->GetEntry(0);

  double fraction_in_time_window = 0;
  TTree* efftree = new TTree("efftree", "efftree");
  efftree->Branch("efficiency", &fraction_in_time_window);  
  
  fraction_in_time_window = (TMath::Exp(-time_window_min/args.decay_lifetime) - TMath::Exp(-time_window_max/args.decay_lifetime));
  fraction_in_time_window = fraction_in_time_window * args.extra_efficiency;
  std::cout << "Unfolding TimeCut: " << time_window_min << " ns -- " << time_window_max << " ns: fraction in time window = " << fraction_in_time_window << std::endl;
  
  TH1D* hCorrectedSpectrum = (TH1D*) hInputSpectrum->Clone("hCorrectedSpectrum");
  hCorrectedSpectrum->Scale(1.0/fraction_in_time_window);

  efftree->Fill();
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hInputSpectrum->Write();
  hCorrectedSpectrum->Write();
  efftree->Write();
  outfile->Write();
  outfile->Close();

  file->Close();
  cutfile->Close();
}
