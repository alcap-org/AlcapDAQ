#include "./scripts/TMETree/TMETreeBranches.h"
#include "./scripts/TMETree/TMEUtils.h"
#include "./scripts/Al50/util.h"
#include "./src/plotters/SimplePulse.h"

#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

////////////////////////////////////////////////////////////////////////////////
/// USER MODIFIABLE VARIABLES
static const double PROMPT[]  = {-200., 200.}; // ns
static const double XRAYE[]   = {340.,  360.}; // keV
////////////////////////////////////////////////////////////////////////////////
/// RUNNING
/// R15b_Al50_nmu(ttree_fname, ofile_fname)
////////////////////////////////////////////////////////////////////////////////

bool IsPrompt(const SimplePulse& p) {
  return PROMPT[0] < p.tTME && p.tTME < PROMPT[1];
}
bool IsAl2p1s(const SimplePulse& p) {
  return XRAYE[0] < p.E && p.E < XRAYE[1];
}

// pp = pileup protection time window. Set to 0 for no pp.
void R15b_Al50_nmu(const char* ifname=nullptr, const char* ofname=nullptr,
                   double pp=0) {
  if (!ifname) return;
  TH1::SetDefaultSumw2(kTRUE);
  TFile* ifile = new TFile(ifname);
  if (ifile->IsZombie()) PrintAndThrow("Can't find input file.");
  TFile* ofile = new TFile(ofname, "RECREATE");
  TTree* tr    = (TTree*)ifile->Get("TMETree/TMETree");
  SetTMEBranchAddresses(tr);
  TH1::SetDefaultSumw2(kTRUE);
  TH1D* hmu          = new TH1D("hemu",         "hemu",         4000, 0, 10e3);
  TH1D* hgehi_e_all  = new TH1D("hgehi_e_all",  "hgehi_e",      5851, 0, 2e3);
  TH1D* hgelo_e_all  = new TH1D("hgelo_e_all",  "hgelo_e",      5851, 0, 2e3);
  TH1D* hgehi_e_tcut = new TH1D("hgehi_e_tcut", "hgehi_e_tcut", 5851, 0, 2e3);
  TH1D* hgelo_e_tcut = new TH1D("hgelo_e_tcut", "hgelo_e_tcut", 5851, 0, 2e3);
  TH1D* hgehi_t_ecut = new TH1D("hgehi_t_ecut", "hgehi_t_ecut", 500,  0, 5e3);
  TH1D* hgelo_t_ecut = new TH1D("hgelo_t_ecut", "hgelo_t_ecut", 500,  0, 5e3);

  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (pp && !TMECuts::PileupProtected(pp))
      continue;
    hmu->Fill(centralMuonEnergy);
    for (int i = 0; i < GeHiGain->size(); ++i) {
      const SimplePulse& p = GeHiGain->at(i);
      hgehi_e_all->Fill(p.E);
      if (IsPrompt(p)) hgehi_e_tcut->Fill(p.E);
      if (IsAl2p1s(p)) hgehi_t_ecut->Fill(p.tTME);
    }
    for (int i = 0; i < GeLoGain->size(); ++i) {
      const SimplePulse& p = GeLoGain->at(i);
      hgelo_e_all->Fill(p.E);
      if (IsPrompt(p)) hgelo_e_tcut->Fill(p.E);
      if (IsAl2p1s(p)) hgelo_t_ecut->Fill(p.tTME);
    }
  }

  ofile->Write();
}
