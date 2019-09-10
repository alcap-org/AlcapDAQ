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

  double efficiency;
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

  double efficiency = args.efficiency;
  TTree* efftree = new TTree("efftree", "efftree");
  efftree->Branch("efficiency", &efficiency);  
  
  std::cout << "Unfolding PIDCutEfficiency: Eff = " << efficiency << std::endl;
  
  TH1D* hCorrectedSpectrum = (TH1D*) hInputSpectrum->Clone("hCorrectedSpectrum");
  hCorrectedSpectrum->Scale(1.0/efficiency);

  efftree->Fill();
  
  TFile* outfile = new TFile(args.outfilename.c_str(), "UPDATE");
  TDirectory* outdir = outfile->mkdir(args.outdirname.c_str());
  outdir->cd();
  hInputSpectrum->Write();
  hCorrectedSpectrum->Write();
  efftree->Write();
  outfile->Write();
  outfile->Close();
}
