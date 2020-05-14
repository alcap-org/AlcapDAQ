#include "./scripts/TMETree/TMETreeBranches.h"
#include "./scripts/TMETree/TMEUtils.h"
#include "./scripts/Al50/util.h"
#include "./src/plotters/SimplePulse.h"

#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

#include <string>
using std::string;

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
bool IsTi2p1s(const SimplePulse& p) {
  return 925 < p.E && p.E < 933.5;
}

// pp = pileup protection time window. Set to 0 for no pp.
void R15b_Al50_nmu(const char* ifname=nullptr, const char* ofname=nullptr,
                   int run=0, double pp=0, const string& dataset="",
                   double tofflo=0., double toffhi=0.) {
  ECal ELo = ECal::ConstructECal("GeLoGain");
  ECal EHi = ECal::ConstructECal("GeHiGain");
  if (!ifname) return;
  TH1::SetDefaultSumw2(kTRUE);
  TFile* ifile = new TFile(ifname);
  if (ifile->IsZombie())
    PrintAndThrow("Can't find input file.");
  if (!Sanity::IsValidDataset(dataset))
    PrintAndThrow("Invalid dataset: "+dataset);
  TFile* ofile = new TFile(ofname, "RECREATE");
  TTree* tr    = (TTree*)ifile->Get("TMETree/TMETree");
  SetTMEBranchAddresses(tr);
  TH1::SetDefaultSumw2(kTRUE);
  TH1D* hmu             = new TH1D("hemu",            "hemu",            4000, 0,    10e3);
  TH1D* hgehi_e_all     = new TH1D("hgehi_e_all",     "hgehi_e_all",     16384, EHi(0)-EHi.G()/2., EHi(16383)+EHi.G()/2.);
  TH1D* hgelo_e_all     = new TH1D("hgelo_e_all",     "hgelo_e_all",     16384, ELo(0)-ELo.G()/2., ELo(16383)+ELo.G()/2.);
  TH1D* hgehi_e_early   = new TH1D("hgehi_e_early",   "hgehi_e_early",   16384, EHi(0)-EHi.G()/2., EHi(16383)+EHi.G()/2.);
  TH1D* hgelo_e_early   = new TH1D("hgelo_e_early",   "hgelo_e_early",   16384, ELo(0)-ELo.G()/2., ELo(16383)+ELo.G()/2.);
  TH1D* hgehi_e_prompt  = new TH1D("hgehi_e_prompt",  "hgehi_e_prompt",  16384, EHi(0)-EHi.G()/2., EHi(16383)+EHi.G()/2.);
  TH1D* hgelo_e_prompt  = new TH1D("hgelo_e_prompt",  "hgelo_e_prompt",  16384, ELo(0)-ELo.G()/2., ELo(16383)+ELo.G()/2.);
  TH1D* hgehi_e_delayed = new TH1D("hgehi_e_delayed", "hgehi_e_delayed", 16384, EHi(0)-EHi.G()/2., EHi(16383)+EHi.G()/2.);
  TH1D* hgelo_e_delayed = new TH1D("hgelo_e_delayed", "hgelo_e_delayed", 16384, ELo(0)-ELo.G()/2., ELo(16383)+ELo.G()/2.);
  TH1D* hgehi_e_oot     = new TH1D("hgehi_e_oot",     "hgehi_e_oot",     16384, EHi(0)-EHi.G()/2., EHi(16383)+EHi.G()/2.);
  TH1D* hgelo_e_oot     = new TH1D("hgelo_e_oot",     "hgelo_e_oot",     16384, ELo(0)-ELo.G()/2., ELo(16383)+ELo.G()/2.);
  TH1D* hgehi_t_ecut    = new TH1D("hgehi_t_ecut",    "hgehi_t_ecut",    600,   -1e3, 5e3);
  TH1D* hgelo_t_ecut    = new TH1D("hgelo_t_ecut",    "hgelo_t_ecut",    600,   -1e3, 5e3);
  TH1D* hgehi_a_all     = new TH1D("hgehi_a_all",     "hgehi_a_all",     16384, -0.5, 16383.5);
  TH1D* hgelo_a_all     = new TH1D("hgelo_a_all",     "hgelo_a_all",     16384, -0.5, 16383.5);
  TH1D* hgehi_a_early   = new TH1D("hgehi_a_early",   "hgehi_a_early",   16384, -0.5, 16383.5);
  TH1D* hgelo_a_early   = new TH1D("hgelo_a_early",   "hgelo_a_early",   16384, -0.5, 16383.5);
  TH1D* hgehi_a_prompt  = new TH1D("hgehi_a_prompt",  "hgehi_a_prompt",  16384, -0.5, 16383.5);
  TH1D* hgelo_a_prompt  = new TH1D("hgelo_a_prompt",  "hgelo_a_prompt",  16384, -0.5, 16383.5);
  TH1D* hgehi_a_delayed = new TH1D("hgehi_a_delayed", "hgehi_a_delayed", 16384, -0.5, 16383.5);
  TH1D* hgelo_a_delayed = new TH1D("hgelo_a_delayed", "hgelo_a_delayed", 16384, -0.5, 16383.5);
  TH1D* hgehi_a_oot     = new TH1D("hgehi_a_oot",     "hgehi_a_oot",     16384, -0.5, 16383.5);
  TH1D* hgelo_a_oot     = new TH1D("hgelo_a_oot",     "hgelo_a_oot",     16384, -0.5, 16383.5);

  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (pp && !TMECuts::PileupProtected(pp))
      continue;
    hmu->Fill(centralMuonEnergy);
    for (int i = 0; i < GeHiGain->size(); ++i) {
      const SimplePulse& p = GeHiGain->at(i);
      double a = p.Amp;
      double e = EHi(a);
      if (!pp || IsPP(p, pp, toffhi))  hgehi_a_all    ->Fill(a), hgehi_e_all    ->Fill(e);
      if      (IsEarly(p, pp, toffhi)) hgehi_a_early  ->Fill(a), hgehi_e_early  ->Fill(e);
      else if (IsPrompt(p, toffhi))    hgehi_a_prompt ->Fill(a), hgehi_e_prompt ->Fill(e);
      else if (IsDelayed(p, toffhi))   hgehi_a_delayed->Fill(a), hgehi_e_delayed->Fill(e);
      else if (IsOOT(p, pp, toffhi))   hgehi_a_oot    ->Fill(a), hgehi_e_oot    ->Fill(e);
      if      (IsAl2p1s(p))            hgehi_t_ecut   ->Fill(p.tTME+toffhi);
    }
    for (int i = 0; i < GeLoGain->size(); ++i) {
      const SimplePulse& p = GeLoGain->at(i);
      double a = p.Amp;
      double e = ELo(a);
      if (pp < 0. || IsPP(p, pp, tofflo))  hgelo_a_all->Fill(a), hgelo_e_all    ->Fill(e);
      if      (IsEarly(p, pp, tofflo)) hgelo_a_early  ->Fill(a), hgelo_e_early  ->Fill(e);
      else if (IsPrompt(p, tofflo))    hgelo_a_prompt ->Fill(a), hgelo_e_prompt ->Fill(e);
      else if (IsDelayed(p, tofflo))   hgelo_a_delayed->Fill(a), hgelo_e_delayed->Fill(e);
      else if (IsOOT(p, pp, tofflo))   hgelo_a_oot    ->Fill(a), hgelo_e_oot    ->Fill(e);
      if      (IsAl2p1s(p))            hgelo_t_ecut   ->Fill(p.tTME+tofflo);
    }
  }
  string dset = dataset;
  TTree* otr = new TTree("configtree", "Config Tree");
  otr->Branch("pileup_protection", &pp);
  double PROMPT_TS [2] = { PROMPT[0],  PROMPT[1] };
  double DELAYED_TS[2] = { DELAYED[0], DELAYED[1] };
  otr->Branch("run",        &run);
  otr->Branch("prompt_t1",  &PROMPT_TS[0]);
  otr->Branch("prompt_t2",  &PROMPT_TS[1]);
  otr->Branch("delayed_t1", &DELAYED_TS[0]);
  otr->Branch("delayed_t2", &DELAYED_TS[1]);
  otr->Branch("tofflo",     &tofflo);
  otr->Branch("toffhi",     &toffhi);
  otr->Branch("dataset",    &dset);
  otr->Fill();
  ofile->Write();
}
