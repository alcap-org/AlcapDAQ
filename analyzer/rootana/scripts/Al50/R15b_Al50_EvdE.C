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

void BookHistograms(TFile* f, TH3* hl[17], TH3* hr[5], TH3* hr2[5]) {
  const int NX = 600, NY = 200, NZ = 4;
  Double_t* X       = HistUtils::ConstructBins(NX, 0, 30e3);
  Double_t* Y       = HistUtils::ConstructBins(NY, 0, 10e3);
  Double_t  Z[NZ+1] = {-200, 400, 1000, 2000, 4500};
  TH1::SetDefaultSumw2(kTRUE);
  for (int i = 0; i < 17; ++i) {
    char hname[128], htitle[128];
    sprintf(hname,  "evde_l%d",                             i);
    sprintf(htitle, "E vs dE L%d;E [keV];E1 [keV];T1 [ns]", i);
    hl[i] = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
    hl[i]->SetDirectory(f);
  }
  for (int i = 0; i < 5; ++i) {
    char hname[128], htitle[128];
    sprintf(hname,  "evde_r%d",                             i);
    sprintf(htitle, "E vs dE R%d;E [keV];E1 [keV];T1 [ns]", i);
    hr[i] = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
    hr[i]->SetDirectory(f);
  }
  for (int i = 0; i < 5; ++i) {
    char hname[128], htitle[128];
    sprintf(hname,  "e2ve3_r%d",                              i);
    sprintf(htitle, "E2 vs E3 R%d;E2 [keV];E3 [keV];T1 [ns]", i);
    hr2[i] = new TH3F(hname, htitle, NX, X, NX, X, NZ, Z);
    hr2[i]->SetDirectory(f);
  }
}

template <class T>
void CombineGraphs(vector< vector<T> >& xs,
                   vector< vector<T> >& ys) {
  assert(xs.size() == ys.size());
  for (int i = 1; i < xs.size(); ++i) {
      xs[0].insert(xs[0].end(), xs[i].begin(), xs[i].end());
      ys[0].insert(ys[0].end(), ys[i].begin(), ys[i].end());
  }
}

void ConstructAndSaveGraphs(TFile* f, char lr,
                            const vector< vector<double> >& xs,
                            const vector< vector<double> >& ys) {
  assert(xs.size() == ys.size());
  TDirectory* cwd = gDirectory;
  f->cd();
  for (int i = 0; i < xs.size(); ++i) {
      char gname[128], gtitle[128];
      sprintf(gname, "gevde_%c%d", lr, i);
      sprintf(gtitle, "E vs dE %c%d", std::toupper(lr), i);
      TGraph* g = new TGraph(xs[i].size(), &xs[i][0], &ys[i][0]);
      g->SetTitle(gtitle);
      g->Write(gname);
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
  TH3 *hl[17], *hr[5], *hr2[5];
  vector< vector<double> > vle(17), vlde(17), vre(5), vrde(5), vre2(5), vre3(5);
  BookHistograms(ofile, hl, hr, hr2);
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (verbose && i % 10000 == 0)
      std::cout << i << "/" << tr->GetEntries() << std::endl;
    if (!TMECuts::OnlyOneMuon())
      continue;
    if (TMECuts::OnlyOneHit(SiR2))
      for (int j = 0; j < 4; ++j) {
        SiEvent ev(SiR1s[j], SiR2, (usealllayers ? SiR3 : nullptr), nullptr);
        if (ev.Valid()) {
          hr[j+1]->Fill(ev.E(), ev.dE(), ev.T());
          if (ev.ThreeHits()) {
            hr2[j+1]->Fill(ev.E(1), ev.E(2), ev.T());
            vre2[j+1].push_back(ev.E(1));
            vre3[j+1].push_back(ev.E(2));
          }
          vre [j+1].push_back(ev.E());
          vrde[j+1].push_back(ev.dE());
        }
      }
    if (TMECuts::OnlyOneHit(SiL3))
      for (int j = 0; j < 14; ++j) {
        SiEvent ev(SiL1s[j], SiL3, nullptr, &TMECal::SiL1A2E[j+2]);
        if (ev.Valid()) {
          hl[j+2]->Fill(ev.E(), ev.dE(), ev.T());
          vle [j+2].push_back(ev.E());
          vlde[j+2].push_back(ev.dE());
        }
      }
  }
  for (int i = 1; i < 5; ++i)
    hr[0]->Add(hr[i]);
  for (int i = 1; i < 5; ++i)
    hr2[0]->Add(hr2[i]);
  for (int i = 1; i < 17; ++i)
    hl[0]->Add(hl[i]);
  CombineGraphs(vle, vlde);
  CombineGraphs(vre, vrde);
  CombineGraphs(vre2, vre3);
  ConstructAndSaveGraphs(ofile, 'l', vle, vlde);
  ConstructAndSaveGraphs(ofile, 'r', vre, vrde);
  // ConstructAndSaveGraphs(ofile, "r2", vre2, vre3);
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
