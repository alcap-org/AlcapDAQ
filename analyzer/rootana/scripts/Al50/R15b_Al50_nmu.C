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
static const double DELAYED[] = {200,   4.e3}; // ns
static const double XRAYE[]   = {340.,  360.}; // keV
////////////////////////////////////////////////////////////////////////////////
/// RUNNING
/// R15b_Al50_nmu(ofile_fname)
////////////////////////////////////////////////////////////////////////////////

bool IsPP(const SimplePulse& p, double pp, double dt) {
  return -pp/2 < p.tTME+dt && p.tTME+dt <= pp/2;
}

bool IsEarly(const SimplePulse& p, double pp, double dt) {
  return (pp ? -pp/2 < p.tTME+dt : true) && p.tTME < PROMPT[0];
}

bool IsPrompt(const SimplePulse& p, double dt) {
  return PROMPT[0] <= p.tTME+dt && p.tTME+dt < PROMPT[1];
}
bool IsDelayed(const SimplePulse& p, double dt) {
  return DELAYED[0] <= p.tTME+dt && p.tTME+dt < DELAYED[1];
}
bool IsOOT(const SimplePulse& p, double pp, double dt) {
  return DELAYED[1] <= p.tTME+dt && (pp ? p.tTME+dt < pp/2 : true);
}
bool IsAl2p1s(const SimplePulse& p) {
  return XRAYE[0] <= p.E && p.E < XRAYE[1];
}

// pp = pileup protection time window. Set to 0 for no pp.
void R15b_Al50_nmu(const char* ifname=nullptr, const char* ofname=nullptr,
                   int run=0, double pp=0, double tofflo=0., double toffhi=0.) {
  if (!ifname) return;
  TH1::SetDefaultSumw2(kTRUE);
  TFile* ifile = new TFile(ifname);
  if (ifile->IsZombie()) PrintAndThrow("Can't find input file.");
  TFile* ofile = new TFile(ofname, "RECREATE");
  TTree* tr    = (TTree*)ifile->Get("TMETree/TMETree");
  SetTMEBranchAddresses(tr);
  TH1::SetDefaultSumw2(kTRUE);
  TH1D* hmu             = new TH1D("hemu",            "hemu",            4000, 0,    10e3);
  TH1D* hgehi_e_all     = new TH1D("hgehi_e_all",     "hgehi_e",         5851, 0,    2e3);
  TH1D* hgelo_e_all     = new TH1D("hgelo_e_all",     "hgelo_e",         5851, 0,    2e3);
  TH1D* hgehi_e_early   = new TH1D("hgehi_e_early",   "hgehi_e_early",   5851, 0,    2e3);
  TH1D* hgelo_e_early   = new TH1D("hgelo_e_early",   "hgelo_e_early",   5851, 0,    2e3);
  TH1D* hgehi_e_prompt  = new TH1D("hgehi_e_prompt",  "hgehi_e_prompt",  5851, 0,    2e3);
  TH1D* hgelo_e_prompt  = new TH1D("hgelo_e_prompt",  "hgelo_e_prompt",  5851, 0,    2e3);
  TH1D* hgehi_e_delayed = new TH1D("hgehi_e_delayed", "hgehi_e_delayed", 5851, 0,    2e3);
  TH1D* hgelo_e_delayed = new TH1D("hgelo_e_delayed", "hgelo_e_delayed", 5851, 0,    2e3);
  TH1D* hgehi_e_oot     = new TH1D("hgehi_e_oot",     "hgehi_e_oot",     5851, 0,    2e3);
  TH1D* hgelo_e_oot     = new TH1D("hgelo_e_oot",     "hgelo_e_oot",     5851, 0,    2e3);
  TH1D* hgehi_t_ecut    = new TH1D("hgehi_t_ecut",    "hgehi_t_ecut",    600,  -1e3, 5e3);
  TH1D* hgelo_t_ecut    = new TH1D("hgelo_t_ecut",    "hgelo_t_ecut",    600,  -1e3, 5e3);

  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (pp && !TMECuts::PileupProtected(pp))
      continue;
    hmu->Fill(centralMuonEnergy);
    for (int i = 0; i < GeHiGain->size(); ++i) {
      const SimplePulse& p = GeHiGain->at(i);
      if (!pp || IsPP(p, pp, toffhi))  hgehi_e_all    ->Fill(p.E);
      if      (IsEarly(p, pp, toffhi)) hgehi_e_early  ->Fill(p.E);
      else if (IsPrompt(p, toffhi))    hgehi_e_prompt ->Fill(p.E);
      else if (IsDelayed(p, toffhi))   hgehi_e_delayed->Fill(p.E);
      else if (IsOOT(p, pp, toffhi))   hgehi_e_oot    ->Fill(p.E);
      if      (IsAl2p1s(p))            hgehi_t_ecut   ->Fill(p.tTME+toffhi);
    }
    for (int i = 0; i < GeLoGain->size(); ++i) {
      const SimplePulse& p = GeLoGain->at(i);
      if (!pp || IsPP(p, pp, tofflo))  hgelo_e_all    ->Fill(p.E);
      if      (IsEarly(p, pp, tofflo)) hgelo_e_early  ->Fill(p.E);
      else if (IsPrompt(p, tofflo))    hgelo_e_prompt ->Fill(p.E);
      else if (IsDelayed(p, tofflo))   hgelo_e_delayed->Fill(p.E);
      else if (IsOOT(p, pp, tofflo))   hgelo_e_oot    ->Fill(p.E);
      if      (IsAl2p1s(p))            hgelo_t_ecut   ->Fill(p.tTME+tofflo);
    }
  }
  TTree* otr = new TTree("configtree", "Config Tree");
  otr->Branch("pileup_protection", &pp);
  double PROMPT_TS [2] = { PROMPT[0],  PROMPT[1] };
  double DELAYED_TS[2] = { DELAYED[0], DELAYED[1] };
  otr->Branch("prompt_tlo",  &PROMPT_TS[0]);
  otr->Branch("prompt_thi",  &PROMPT_TS[1]);
  otr->Branch("delayed_tlo", &DELAYED_TS[0]);
  otr->Branch("delayed_thi", &DELAYED_TS[1]);
  otr->Branch("tofflo",      &tofflo);
  otr->Branch("toffhi",      &toffhi);
  otr->Branch("run",         &run);
  otr->Fill();
  ofile->Write();
}
