#include "RooUnfoldBayes.h"
#include "RooUnfoldResponse.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// USER DEFINED VARIABLES
// Takes the post-cut data E vs dE plots and transfer matrices from
// the simulation and unfolds the energy spectrum for the desired particle
// Compile only: root -l -b -q scripts/Al50/R15b_Al50_Unfold.C+g
// Run:          root -l -b -q scripts/Al50/R15b_Al50_Unfold.C+g\(mode,
//                 ifname_tm, ifname_data, ofname\)
// where mode is a char and one of (all others compile only)
//   p (protons), d (deuterons), t (tritons), a (alphas)
// ifname_tm, ifname_data, and ofname are all c-strings.
// ifname_tm:   input file name with the transfer matrices whose names match
//              the global constants below
// ifname_data: The output of the particle selection script; the data.
// ofname:      Output file name.
// Iterations for Bayesian unfolding
static const int   NITER       = 6;
////////////////////////////////////////////////////////////////////////////////

void Rebin(RooUnfoldResponse* r, int n) {
  r->Htruth()   ->Rebin(n);
  r->Hmeasured()->Rebin(n);
  r->Hresponse()->Rebin2D(n, n);
}

void unfold(const char* ifname_tm, const char* ifname_data,
            const char* ofname) {
  TH1::SetDefaultSumw2(kTRUE);
  TFile* fdata = new TFile(ifname_data);
  TFile* fmctm = new TFile(ifname_tm);
  TTree* tr[2] = { (TTree*)fdata->Get("PID_LP"), (TTree*)fdata->Get("PID_RP") };
  tr[0]->Draw("e>>hle(30,0,15e3)", "t > 400 && abs(dt) < 200 && e3 == 0");
  tr[1]->Draw("e>>hre(30,0,15e3)", "t > 400 && abs(dt) < 200 && e3 == 0");
  TH1* heraw[2] = { (TH1*)gDirectory->Get("hle"),
                    (TH1*)gDirectory->Get("hre") };
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)fmctm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)fmctm->Get("SiR_TM") };
  Rebin(resp[0], 5);
  Rebin(resp[1], 5);
  RooUnfoldBayes* unfold[2] = { new RooUnfoldBayes(resp[0], heraw[0], NITER),
                                new RooUnfoldBayes(resp[1], heraw[1], NITER) };
  TH1* h_e_reco[2] = { unfold[0]->Hreco(),
                       unfold[1]->Hreco()};
  TFile* ofile = new TFile(ofname, "RECREATE");
  h_e_reco[0]->SetDirectory(ofile);
  h_e_reco[1]->SetDirectory(ofile);
  h_e_reco[0]->SetName("hl_e_reco");
  h_e_reco[1]->SetName("hr_e_reco");
  ofile->Write();
  ofile->Close();
}

void R15b_Al50_Unfold(char mode='\0', const char* ifname_tm=nullptr,
                      const char* ifname_data=nullptr,
                      const char* ofname=nullptr) {
  switch(mode) {
    case 'p': unfold(ifname_tm, ifname_data, ofname); return;
    case 'd': unfold(ifname_tm, ifname_data, ofname); return;
    case 't': unfold(ifname_tm, ifname_data, ofname); return;
    case 'a': unfold(ifname_tm, ifname_data, ofname); return;
  }
}
