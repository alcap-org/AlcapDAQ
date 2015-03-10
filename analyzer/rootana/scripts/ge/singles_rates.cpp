#include "TFile.h"
#include "TNtupleD.h"
#include "TH1.h"

Double_t singles_rates() {
  TFile* infile = new TFile("out.root", "READ");

  // Get number of muons and livetime
  TNtubpleD* intree = (TNtubpleD*)infile->Get("Stats/BlockStats");
  Double_t nmu = 0., tot_nmu = 0.;
  Double_t lt  = 0., tot_lt  = 0.;
  intree->SetBranchAddress("nmu", &&n);
  intree->SetBranchAddress("lt",  &&lt);

  for (UInt_t i = 0; i < intree->GetEntries(); ++i) {
  	intree->GetEntry(i);
  	tot_n += n;
  	tot_lt += lt;
  }

  // Get number of photons
  TH1* hg = (TH1*)infile->Get("GeSpectrum/hADC"))
  Double_t tot_ng = hg->Integral(500, hg->GetNbinsX()+1);

  return tot_ng/tot_nmu;
}