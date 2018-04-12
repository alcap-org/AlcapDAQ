#include "RooUnfoldBayes.h"
#include "RooUnfoldResponse.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

////////////////////////////////////////////////////////////////////////////////
// USER DEFINED VARIABLES
// Takes the post-cut data E vs dE plots and transfer matrices from
// the simulation and unfolds the energy spectrum for the desired particle
// Compile only: root -l -b -q scripts/Al50/R15b_Al50_Unfold.C+g
// Run:          root -l -b -q scripts/Al50/R15b_Al50_Unfold.C+g\(#\)
// where # is one of
//   0 (compile only), 1 (protons), 2 (deuterons), 3 (tritons), 4 (alphas),
//   5 (all particles)
//
// Iterations for Bayesian unfolding
static const int   NITER       = 4;
// Data from R15b_Al50_psel.C script. Contains EvdE plots from data
// each of p, d, t, and a using simulation selection functions.
static const char* IFNAME_DATA = "~/data/R15b/pselal50.root";
// The names of the histograms in the above file. The zero indicates
// all segments are summed. The first element should be the histograms for
// the left Si detector package and the second element the right.
static const char* IHNAME_P[2] = {"evde_l0_proton",   "evde_r0_proton"};
static const char* IHNAME_D[2] = {"evde_l0_deuteron", "evde_r0_deuteron"};
static const char* IHNAME_T[2] = {"evde_l0_triton",   "evde_r0_triton"};
static const char* IHNAME_A[2] = {"evde_l0_alpha",    "evde_r0_alpha"};
// The input filenames with the transfer matrices (RooUnfodResponse instances)
// produced from R15b_Al50_TransferMatrix.C.
static const char* IFNAME_P_TM = "~/data/R15b/al50ptm.root";
static const char* IFNAME_D_TM = "~/data/R15b/al50dtm.root";
static const char* IFNAME_T_TM = "~/data/R15b/al50ttm.root";
static const char* IFNAME_A_TM = "~/data/R15b/al50atm.root";
// The output filenames that will each contain just two TH1s, one for
// each of left and right.
static const char* OFNAME_P    = "~/data/R15b/al50unfoldp.root";
static const char* OFNAME_D    = "~/data/R15b/al50unfoldd.root";
static const char* OFNAME_T    = "~/data/R15b/al50unfoldt.root";
static const char* OFNAME_A    = "~/data/R15b/al50unfolda.root";
////////////////////////////////////////////////////////////////////////////////


void unfold(const char* ifname, const char* ofname, const char* ihnames[2]) {
  TH1::SetDefaultSumw2(kTRUE);
  TFile* fdata = new TFile(IFNAME_DATA);
  TFile* fmctm = new TFile(ifname);
  TH3* hevdevt[2] = { (TH3*)fdata->Get(ihnames[0]),
                      (TH3*)fdata->Get(ihnames[1])};
  hevdevt[0]->GetZaxis()->SetRange(2, 5);
  hevdevt[1]->GetZaxis()->SetRange(2, 5);
  TH2* hevde[2] = { (TH2*)hevdevt[0]->Project3D("yx"),
                    (TH2*)hevdevt[1]->Project3D("yx") };
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)fmctm->Get("SiL_TM"),
                                 (RooUnfoldResponse*)fmctm->Get("SiR_TM") };
  TH1* heraw[2] = { hevde[0]->ProjectionX(),
                    hevde[1]->ProjectionX() };
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

void R15b_Al50_Unfold(int mode=0) {
  switch(mode) {
    case 0:                                                   return;
    case 1: unfold(IFNAME_P_TM, OFNAME_P, IHNAME_P);          return;
    case 2: unfold(IFNAME_D_TM, OFNAME_D, IHNAME_D);          return;
    case 3: unfold(IFNAME_T_TM, OFNAME_T, IHNAME_T);          return;
    case 4: unfold(IFNAME_A_TM, OFNAME_A, IHNAME_A);          return;
    case 5: for (int i = 1; i <= 4; ++i) R15b_Al50_Unfold(i); return;
  }
}
