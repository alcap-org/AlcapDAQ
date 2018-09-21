#include "./scripts/Al50/util.h"
#include "./src/plotters/SimplePulse.h"
#include "./scripts/TMETree/TMETreeBranches.h"
#include "./scripts/TMETree/TMEUtils.h"

#include "TChain.h"
#include "TFile.h"
#include "TH3F.h"
#include "TTree.h"

#include <iostream>
#include <vector>

using SiUtils::SiEvent;
using std::vector;

////////////////////////////////////////////////////////////////////////////////
// USER MODIFIABLE VARIABLES
// Compile only: root -l -b -q R15b_Al50_EvdE.C+g
// Run on all Al50: root -l -b -q R15b_Al50_EvdE.C+g\(false\)
// On a specific run #: root -l -b -q R15b_Al50_EvdE.C+g\(#\)
// The input files should be contain TMETrees from rootana output.
static const char IFNAMEFMT[] = "~/R15bTME/%s/%s%05d.root";
static const char OFNAMEFMT[] = "~/data/R15b/evde%05d.root";
static const int  NSIR        = 4;
static const int  NSIL        = 16;
////////////////////////////////////////////////////////////////////////////////

void BookHistograms(TFile* f, TH3* hl[17], TH3* hr[5], TH3* hr2[5]) {
  const int NX = 600, NY = 200, NZ = 4;
  Double_t* X       = HistUtils::ConstructBins(NX, 0, 30e3);
  Double_t* Y       = HistUtils::ConstructBins(NY, 0, 10e3);
  Double_t  Z[NZ+1] = {-200, 400, 1000, 2000, 4500};
  TH1::SetDefaultSumw2(kTRUE);
  for (int i = 0; i < NSIL+1; ++i) {
    char hname[128], htitle[128];
    sprintf(hname,  "evde_l%d",                             i);
    sprintf(htitle, "E vs dE L%d;E [keV];E1 [keV];T1 [ns]", i);
    hl[i] = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
    hl[i]->SetDirectory(f);
  }
  for (int i = 0; i < NSIR+1; ++i) {
    char hname[128], htitle[128];
    sprintf(hname,  "evde_r%d",                             i);
    sprintf(htitle, "E vs dE R%d;E [keV];E1 [keV];T1 [ns]", i);
    hr[i] = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
    hr[i]->SetDirectory(f);
  }
  for (int i = 0; i < NSIR+1; ++i) {
    char hname[128], htitle[128];
    sprintf(hname,  "e2ve3_r%d",                              i);
    sprintf(htitle, "E2 vs E3 R%d;E2 [keV];E3 [keV];T1 [ns]", i);
    hr2[i] = new TH3F(hname, htitle, NX, X, NX, X, NZ, Z);
    hr2[i]->SetDirectory(f);
  }
}

struct PIDEvent {
  Double_t e, de, t, dt;
  PIDEvent(Double_t e=0, Double_t de=0, Double_t t=0, Double_t dt=0) :
  e(e), de(de), t(t) , dt(dt) {}
  PIDEvent(const SiUtils::SiEvent& ev) :
  e(ev.E()), de(ev.dE()), t(ev.T()), dt(ev.dT()) {}
};

void ConstructAndSaveTrees(TFile* f, char lr,
                           const vector< vector<PIDEvent> >& pids,
                           const char* postfix="") {
  TDirectory* cwd = gDirectory;
  f->cd();
  for (int i = 0; i < pids.size(); ++i) {
    char trname[32];
    sprintf(trname, "PID_%c%d%s", std::toupper(lr), i+1, postfix);
    TTree* tr = new TTree(trname, trname);
    PIDEvent pid;
    tr->Branch("e",  &pid.e);
    tr->Branch("de", &pid.de);
    tr->Branch("t",  &pid.t);
    tr->Branch("dt", &pid.dt);
    for (int j = 0; j < pids[i].size(); ++j) {
      pid = pids[i][j];
      tr->Fill();
    }
  }
  cwd->cd();
}

vector< vector<SimplePulse>* >& SiTs  = all_SiT_channels;
vector< vector<SimplePulse>* >& SiR1s = all_SiR1_channels;
vector< vector<SimplePulse>* >& SiL1s = all_SiL1_channels;
void evde(TTree* tr, const char* ofname, bool usealllayers=true,
          bool verbose=false) {
  TFile* ofile = new TFile(ofname, "RECREATE");
  SetTMEBranchAddresses(tr);
  CollectChannels();
  TMECal::Init();
  TH3 *hl[NSIL+1], *hr[NSIR+1], *hr2[NSIR+1];
  vector< vector<PIDEvent> > lpid(NSIL), rpid(NSIR), r2pid(NSIR);
  BookHistograms(ofile, hl, hr, hr2);
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (verbose && i % 10000 == 0)
      std::cout << i << "/" << tr->GetEntries() << std::endl;
    if (!TMECuts::OnlyOneMuon())
      continue;
    if (TMECuts::OnlyOneHit(SiR2)) {
      for (int j = 0; j < 4; ++j) {
        SiEvent ev(SiR1s[j], SiR2, (usealllayers ? SiR3 : nullptr), nullptr);
        if (ev.Valid()) {
          hr[j+1]->Fill(ev.E(), ev.dE(), ev.T());
          if (ev.ThreeHits()) {
            hr2[j+1]->Fill(ev.E(1), ev.E(2), ev.T());
            r2pid[j].push_back(PIDEvent(ev.E(1)+ev.E(2), ev.E(1), ev.T(),
                                        ev.dT(2, 1)));
          }
          rpid[j].push_back(PIDEvent(ev));
        }
      }
    }
    if (TMECuts::OnlyOneHit(SiL3)) {
      for (int j = 0; j < 14; ++j) {
        SiEvent ev(SiL1s[j], SiL3, nullptr, &TMECal::SiL1A2E[j+2]);
        if (ev.Valid()) {
          hl[j+2]->Fill(ev.E(), ev.dE(), ev.T());
          lpid[j].push_back(PIDEvent(ev));
        }
      }
    }
  }
  for (int i = 1; i < 5; ++i)
    hr[0]->Add(hr[i]);
  for (int i = 1; i < 5; ++i)
    hr2[0]->Add(hr2[i]);
  for (int i = 1; i < 17; ++i)
    hl[0]->Add(hl[i]);
  ConstructAndSaveTrees(ofile, 'l',  lpid);
  ConstructAndSaveTrees(ofile, 'r',  rpid);
  ConstructAndSaveTrees(ofile, 'r',  r2pid, "2");
  ofile->Write();
  ofile->Close();
}

// Run = 0 means compile only
void R15b_Al50_EvdE(int run=0, bool allsilayers=true, bool verbose=false) {
  char dataset[8], prefix[8];
  if (run==0) {
    return;
  } else if (9410 <= run && run <= 9682) {
    sprintf(dataset, "Al100");
    sprintf(prefix, "out");
  } else if (9890 <= run && run <= 10128) {
    sprintf(dataset, "Al50");
    sprintf(prefix, "tme");
  } else {
    PrintAndThrow("Unrecognized run");
  }

  char ifname[128], ofname[128];
  sprintf(ifname, IFNAMEFMT, dataset, prefix, run);
  sprintf(ofname, OFNAMEFMT, run);

  std::cout << ifname << std::endl << ofname << std::endl;

  TChain* ch = new TChain("TMETree/TMETree");
  ch->Add(ifname);
  if (ch->GetEntries() == 0) return;
  evde(ch, ofname, allsilayers, verbose);
}
