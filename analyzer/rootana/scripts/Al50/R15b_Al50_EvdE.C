#include "./scripts/Al50/util.h"
#include "./src/plotters/SimplePulse.h"
#include "./scripts/TMETree/TMETreeBranches.h"
#include "./scripts/TMETree/TMEUtils.h"

#include "TChain.h"
#include "TFile.h"
#include "TGraph.h"
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
static const char IFNAMEFMT[] = "~/R15bTME/Al50/tme%05d.root";
static const char OFNAMEFMT[] = "~/data/R15b/evde%05d.root";
////////////////////////////////////////////////////////////////////////////////

void BookHistograms(TFile* f, TH3* hl[17], TH3* hr[5]) {
  const int NX = 2000, NY = 1000, NZ = 2;
  Double_t* X       = HistUtils::ConstructBins(NX, 0, 20e3);
  Double_t* Y       = HistUtils::ConstructBins(NY, 0, 10e3);
  Double_t  Z[NZ+1] = {-200, 400, 4500};
  TH1::SetDefaultSumw2(kTRUE);
  char hname[64], htitle[64];
  for (int i = 0; i < 17; ++i) {
    sprintf(hname,  "evde_l%d",    i);
    sprintf(htitle, "E vs dE L%d", i);
    hl[i] = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
    hl[i]->SetDirectory(f);
  }
  for (int i = 0; i < 5; ++i) {
    sprintf(hname,  "evde_r%d",    i);
    sprintf(htitle, "E vs dE R%d", i);
    hr[i] = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
    hr[i]->SetDirectory(f);
  }
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
  TH3 *hl[17], *hr[5];
  BookHistograms(ofile, hl, hr);
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (verbose && i % 10000 == 0)
      std::cout << i << "/" << tr->GetEntries() << std::endl;
    if (TMECuts::OnlyOneHit(SiR2))
      for (int j = 0; j < 4; ++j) {
        SiEvent ev(SiR1s[j], SiR2, (usealllayers ? SiR3 : nullptr), nullptr);
        if (ev.Valid())
          hr[j+1]->Fill(ev.E(), ev.dE(), ev.T());
      }
    if (TMECuts::OnlyOneHit(SiL3))
      for (int j = 0; j < 14; ++j) {
        SiEvent ev(SiL1s[j], SiL3, nullptr, &TMECal::SiL1A2E[j+2]);
        if (ev.Valid())
          hl[j+2]->Fill(ev.E(), ev.dE(), ev.T());
      }
  }
  for (int i = 1; i < 5; ++i)
    hr[0]->Add(hr[i]);
  for (int i = 1; i < 17; ++i)
    hl[0]->Add(hl[i]);
  ofile->Write();
  ofile->Close();
}

void R15b_Al50_EvdE(int run, bool allsilayers=true, bool verbose=false) {
  char ifname[128], ofname[128];
  if (9890 <= run && run <= 10128)
    sprintf(ifname, IFNAMEFMT, run);
  else
    throw "Unrecognized run";
  sprintf(ofname, OFNAMEFMT, run);
  TChain* ch = new TChain("TMETree/TMETree");
  ch->Add(ifname);
  if (ch->GetEntries() == 0) return;
  evde(ch, ofname, allsilayers, verbose);
}

// Dummy function for compiling
void R15b_Al50_EvdE(bool compile_only=true, bool allsilayers=true,
                    bool verbose=false) {
  if (compile_only) return;
  for (int i = 9890; i <= 10128; ++i)
    R15b_Al50_EvdE(i, allsilayers, verbose);
}
