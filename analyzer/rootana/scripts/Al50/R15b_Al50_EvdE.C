#include "./src/plotters/SimplePulse.h"
#include "./scripts/TMETree/TMETreeBranches.h"
#include "./scripts/TMETree/TMEUtils.h"

#include "TChain.h"
#include "TFile.h"
#include "TH2F.h"
#include "TTree.h"

#include <iostream>
#include <vector>
using std::vector;

////////////////////////////////////////////////////////////////////////////////
// USER MODIFIABLE VARIABLES
// Compile only: root -l -b -q R15b_Al50_EvdE.C+g
// Run on all Al50: root -l -b -q R15b_Al50_EvdE.C+g\(false\)
// On a specific run #: root -l -b -q R15b_Al50_EvdE.C+g\(#\)
// The input files should be contain TMETrees from rootana output.
static const char IFNAMEFMT[] = "~/R15bTME/Al50/tme%05d.root";
static const char OFNAMEFMT[] = "~/data/R15b/evde%05d.root";
////////////////////////////////////////////////////////////////////////////////

void BookHistograms(TFile* f, TH2* hl[17], TH2* hr[5]) {
  TH1::SetDefaultSumw2(kTRUE);
  char hname[16], htitle[16];
  TH1::SetDefaultSumw2(kTRUE);
  for (int i = 0; i < 17; ++i) {
    sprintf(hname,  "evde_l%d",    i);
    sprintf(htitle, "E vs dE L%d", i);
    hl[i] = new TH2F(hname, htitle, 2e3, 0, 20e3, 1e3, 0, 10e3);
    hl[i]->SetDirectory(f);
  }
  for (int i = 0; i < 5; ++i) {
    sprintf(hname,  "evde_r%d",    i);
    sprintf(htitle, "E vs dE R%d", i);
    hr[i] = new TH2F(hname, htitle, 2e3, 0, 20e3, 1e3, 0, 10e3);
    hl[i]->SetDirectory(f);
  }
}

vector< vector<SimplePulse>* >& SiTs  = all_SiT_channels;
vector< vector<SimplePulse>* >& SiR1s = all_SiR1_channels;
vector< vector<SimplePulse>* >& SiL1s = all_SiL1_channels;
void evde(TTree* tr, const char* ofname) {
  TFile* ofile = new TFile(ofname, "RECREATE");
  SetTMEBranchAddresses(tr);
  CollectChannels();
  TMECal::Init();
  TH2 *hl[17], *hr[5];
  BookHistograms(ofile, hl, hr);
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (i % 10000 == 0) std::cout << i << "/" << tr->GetEntries() << std::endl;
    if (TMECuts::OnlyOneHit(SiR2))
      for (int j = 0; j < 4; ++j)
        if (TMECuts::OnlyOneHit(SiR1s[j]))
          hr[j+1]->Fill(SiR1s[j]->front().E+SiR2->front().E,
                        SiR1s[j]->front().E);
    if (TMECuts::OnlyOneHit(SiL3))
      for (int j = 0; j < 14; ++j)
        if (TMECuts::OnlyOneHit(SiL1s[j]))
          hl[j+2]->Fill(TMECal::SiL1A2E[j+2](SiL1s[j]->front().Amp)+SiL3->front().E,
                        TMECal::SiL1A2E[j+2](SiL1s[j]->front().Amp));
  }
  for (int i = 1; i < 5; ++i)
    hr[0]->Add(hr[i]);
  for (int i = 1; i < 17; ++i)
    hl[0]->Add(hl[i]);
  ofile->Write();
  ofile->Close();
}

void R15b_Al50_EvdE(int run) {
  char ifname[128], ofname[128];
  if (9890 <= run && run <= 10128)
    sprintf(ifname, IFNAMEFMT, run);
  else
    throw "Unrecognized run";
  sprintf(ofname, OFNAMEFMT, run);
  TChain* ch = new TChain("TMETree/TMETree");
  ch->Add(ifname);
  evde(ch, ofname);
}

// Dummy function for compiling
void R15b_Al50_EvdE(bool compile_only=true) {
  if (compile_only) return;
  for (int i = 9890; i <= 10128; ++i)
    R15b_Al50_EvdE(i);
}
