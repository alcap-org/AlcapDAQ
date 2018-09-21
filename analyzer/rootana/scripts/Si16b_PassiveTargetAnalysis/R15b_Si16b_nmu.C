#include "./scripts/TMETree/TMETreeBranches.h"
#include "./src/plotters/SimplePulse.h"

#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// USER MODIFIABLE VARIABLES
// Compile only:  root -l -b -q R15b_Si16b_nmu
// A single run:  root -l -b -q R15b_Si16b_nmu(#)
// All Si16b runs: root -l -b -q R15b_Si16b_nmu(-1)
// The input trees should contain TMETrees from rootana output.
static const char IFNAMEFMT[] = "~/data/out/v10/out%05d.root";
static const char OFNAMEFMT[] = "~/data/results/nmu%05d.root";
static const double PROMPT[]  = {-200., 200.}; // ns
static const double XRAYE[]   = {380.,  420.}; // keV
////////////////////////////////////////////////////////////////////////////////

bool IsPrompt(const SimplePulse& p) {
  return PROMPT[0] < p.tTME && p.tTME < PROMPT[1];
}
bool IsSi2p1s(const SimplePulse& p) {
  return XRAYE[0] < p.E && p.E < XRAYE[1];
}

void R15b_Si16b_nmu(int run=0) {
  if (run == 0) // Compile only
    return;
  else if (run < 0) // Run on all
    for (int i = 10362; i <= 10450; ++i)
      R15b_Si16b_nmu(run);

  char ifname[128], ofname[128];
  TH1::SetDefaultSumw2(kTRUE);
  sprintf(ifname, IFNAMEFMT, run);
  sprintf(ofname, OFNAMEFMT, run);
  TFile* ifile = new TFile(ifname);
  if (ifile->IsZombie()) throw "Can't find input file.";
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
    if (i % 100000 == 0) {
      std::cout << i << " / " << tr->GetEntries() << std::endl;
    }
    hmu->Fill(centralMuonEnergy);
    for (int i = 0; i < GeHiGain->size(); ++i) {
      const SimplePulse& p = GeHiGain->at(i);
      hgehi_e_all->Fill(p.E);
      if (IsPrompt(p)) hgehi_e_tcut->Fill(p.E);
      if (IsSi2p1s(p)) hgehi_t_ecut->Fill(p.tTME);
    }
    for (int i = 0; i < GeLoGain->size(); ++i) {
      const SimplePulse& p = GeLoGain->at(i);
      hgelo_e_all->Fill(p.E);
      if (IsPrompt(p)) hgelo_e_tcut->Fill(p.E);
      if (IsSi2p1s(p)) hgelo_t_ecut->Fill(p.tTME);
    }
  }
  // hmu_all->SetDirectory(ofile);
  ofile->Write();
}
