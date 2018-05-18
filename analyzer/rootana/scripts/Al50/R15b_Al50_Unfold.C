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
static const int   NITER       = 4;
// The names of the histograms in the above file. The zero indicates
// all segments are summed. The first element should be the histograms for
// the left Si detector package and the second element the right.
static const char* IHNAME_P[2] = {"evde_l0_proton",   "evde_r0_proton"};
static const char* IHNAME_D[2] = {"evde_l0_deuteron", "evde_r0_deuteron"};
static const char* IHNAME_T[2] = {"evde_l0_triton",   "evde_r0_triton"};
static const char* IHNAME_A[2] = {"evde_l0_alpha",    "evde_r0_alpha"};
////////////////////////////////////////////////////////////////////////////////


void unfold(const char* ifname_tm, const char* ifname_data,
            const char* ihnames[2], const char* ofname) {
  TH1::SetDefaultSumw2(kTRUE);
  TFile* fdata = new TFile(ifname_data);
  TFile* fmctm = new TFile(ifname_tm);
  TH3* hevdevt[2] = { (TH3*)fdata->Get(ihnames[0]),
                      (TH3*)fdata->Get(ihnames[1])};
  hevdevt[0]->GetZaxis()->SetRangeUser(400., 100e3);
  hevdevt[1]->GetZaxis()->SetRangeUser(400., 100e3);
  TH2* hevde[2] = { (TH2*)hevdevt[0]->Project3D("yx"),
                    (TH2*)hevdevt[1]->Project3D("yx") };
  RooUnfoldResponse* resp[2] = { (RooUnfoldResponse*)fmctm->Get("SiL_TM_0"),
                                 (RooUnfoldResponse*)fmctm->Get("SiR_TM_0") };
  TH1* heraw[2] = { hevde[0]->ProjectionX(),
                    hevde[1]->ProjectionX() };
  heraw[0]->Rebin(4);
  heraw[1]->Rebin(4);
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
    case 'p': unfold(ifname_tm, ifname_data, IHNAME_P, ofname); return;
    case 'd': unfold(ifname_tm, ifname_data, IHNAME_D, ofname); return;
    case 't': unfold(ifname_tm, ifname_data, IHNAME_T, ofname); return;
    case 'a': unfold(ifname_tm, ifname_data, IHNAME_A, ofname); return;
  }
}
