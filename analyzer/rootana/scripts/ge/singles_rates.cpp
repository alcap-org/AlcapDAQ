#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include <iostream>

void singles_rates(const char* fname) {
  TFile* infile = new TFile(fname, "READ");

  // Get number of muons and livetime
  TTree* intree = (TTree*)infile->Get("Stats/BlockStats");
  double nmu     = 0.;
  double tot_nmu = 0.;
  double lt      = 0.;
  double tot_lt  = 0.;
  intree->SetBranchAddress("nmu", &nmu);
  intree->SetBranchAddress("lt",  &lt);

  for (UInt_t i = 0; i < intree->GetEntries(); ++i) {
  	intree->GetEntry(i);
  	tot_nmu += nmu;
  	tot_lt += lt;
  }


  // Get number of photons
  TH1* hg = (TH1*)infile->Get("GeSpectrum/hADC");

  const unsigned int npoints = 11;
  for (unsigned int i = 0; i < npoints; ++i)
    std::cout << 100*i << " bins:    " << hg->Integral(100*i, hg->GetNbinsX()+1)/tot_nmu << "    " << hg->Integral(100*i, hg->GetNbinsX()+1)/tot_nmu*50.e3 << std::endl;

  return;
}