#include "./scripts/Al50/psel.h"
#include "./scripts/Al50/util.h"
#include "./scripts/TMETree/TMETreeBranches.h"
#include "./scripts/TMETree/TMEUtils.h"
#include "./src/plotters/SimplePulse.h"

#include "TChain.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3.h"
#include "TH3F.h"
#include "TProfile.h"
#include "TString.h"
#include "TTree.h"
#include "TVector2.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using SiUtils::SiEvent;

using std::string;
using std::vector;

////////////////////////////////////////////////////////////////////////////////
// USER MODIFIABLE VARIABLES
// Compile only: root -l -b -q R15b_Al50_psel.C+g
// Run on a single run #: root -l -b -q R15b_Al50_psel.C+g\(#\)
// Input data files should be TME tree files output from rootana.
// Input simulation (MC) files should be output from the
// script R15b_Al50_MC_EvdE.C.
static const char* IFNAMEFMT = "~/R15bTME/Al50/tme%05d.root";
static const char* OFNAMEFMT = "~/data/R15b/psel%05d.root";
////////////////////////////////////////////////////////////////////////////////

static const int NSIL = 16;
static const int NSIR = 4;

void BookEvdEHistograms(TFile* f, char lr, vector< vector<TH3*> >& hs) {
  const int NX = 400, NY = 200, NZ = 4;
  Double_t* X       = HistUtils::ConstructBins(NX, 0, 20e3);
  Double_t* Y       = HistUtils::ConstructBins(NY, 0, 10e3);
  Double_t  Z[NZ+1] = {-200, 400, 1000, 2000, 4500};
  TH1::SetDefaultSumw2(kTRUE);
  for (int i = 0; i < hs.size(); ++i) {
    for (int j = 0; j < hs[i].size()-1; ++j) {
      char hname[128], htitle[128];
      sprintf(hname,  "evde_%c%d_%s", lr, i, PARTICLES[j]->Name.c_str());
      sprintf(htitle, "E vs dE %c%d", std::toupper(lr), i);
      hs[i][j] = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
      hs[i][j]->SetDirectory(f);
    }
    char hname[128], htitle[128];
    sprintf(hname,  "evde_%c%d_unclassified", lr, i);
    sprintf(htitle, "E vs dE %c%d", std::toupper(lr), i);
    hs[i].back() = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
    hs[i].back()->SetDirectory(f);
  }
}

void ConstructAndSaveGraphs(TFile* f, char lr,
                            const vector< vector< vector<double> > >& xs,
                            const vector< vector< vector<double> > >& ys) {
  TDirectory* cwd = gDirectory;
  f->cd();
  for (int i = 0; i < xs.size(); ++i) {
    for (int j = 0; j < xs[i].size()-1; ++j) {
      char gname[128], gtitle[128];
      sprintf(gname, "gevde_%c%d_%s", lr, i, PARTICLES[j]->Name.c_str());
      sprintf(gtitle, "E vs dE %c%d", std::toupper(lr), i);
      TGraph* g = new TGraph(xs[i][j].size(), &xs[i][j][0], &ys[i][j][0]);
      g->SetTitle(gtitle);
      g->Write(gname);
    }
    char gname[128], gtitle[128];
    sprintf(gname, "gevde_%c%d_unclassified", lr, i);
    sprintf(gtitle, "E vs dE %c%d", std::toupper(lr), i);
    TGraph* g = new TGraph(xs[i].back().size(),
                           &xs[i].back()[0], &ys[i].back()[0]);
    g->SetTitle(gtitle);
    g->Write(gname);
  }
  cwd->cd();
}

void BookTvEHistograms(TFile* f, char lr, vector< vector<TH2*> >& hs) {
  TH1::SetDefaultSumw2(kTRUE);
  char hname[128], htitle[128];
  for (int i = 0; i < hs.size(); ++i) {
    for (int j = 0; j < hs[i].size()-1; ++j) {
      sprintf(hname,  "tve_%c%d_%s", lr, i, PARTICLES[j]->Name.c_str());
      sprintf(htitle, "T vs E %c%d", std::toupper(lr), i);
      hs[i][j] = new TH2F(hname, htitle, 1000, -5e3, 5e3, 200, 0, 20e3);
      hs[i][j]->SetDirectory(f);
    }
  }
  for (int i = 0; i < hs.size(); ++i) {
    sprintf(hname,  "tve_%c%d_unclassified", lr, i);
    sprintf(htitle, "T vs E %c%d", std::toupper(lr), i);
    hs[i].back() = new TH2F(hname, htitle, 1000, -5e3, 5e3, 200, 0, 20e3);
    hs[i].back()->SetDirectory(f);
  }
}
void BookdTHistograms(TFile* f, char lr, vector< vector<TH1*> >& hs) {
  TH1::SetDefaultSumw2(kTRUE);
  char hname[128], htitle[128];
  for (int i = 0; i < hs.size(); ++i) {
    for (int j = 0; j < hs[i].size()-1; ++j) {
      sprintf(hname,  "dt_%c%d_%s", lr, i, PARTICLES[j]->Name.c_str());
      sprintf(htitle, "T_{thick}-T_{thin} %c%d", std::toupper(lr), i);
      hs[i][j] = new TH1F(hname, htitle, 1000, -5e3, 5e3);
      hs[i][j]->SetDirectory(f);
    }
  }
  for (int i = 0; i < hs.size(); ++i) {
    sprintf(hname,  "dt_%c%d_unclassified", lr, i);
    sprintf(htitle, "T_{thick} vs T_{thin} %c%d", std::toupper(lr), i);
    hs[i].back() = new TH1F(hname, htitle, 1000, -5e3, 5e3);
    hs[i].back()->SetDirectory(f);
  }
}

template <class T>
int WhichParticle(const vector<T>& pls, double dE, double E) {
  for (int i = 0; i < pls.size(); ++i)
    if (pls[i].IsParticle(dE, E))
      return i;
  return pls.size();
}

template <class T>
void CombineHistograms(vector< vector<T*> >& hs) {
  for (int i = 1; i < hs.size(); ++i)
    for (int j = 0; j < hs[i].size(); ++j)
      hs[0][j]->Add(hs[i][j]);
}

template <class T>
void CombineGraphs(vector< vector< vector<T> > >& xs,
                   vector< vector< vector<T> > >& ys) {
  assert(xs.size()==ys.size());
  for (int i = 1; i < xs.size(); ++i) {
    assert(xs[i].size()==ys[i].size());
    for (int j = 0; j < xs[i].size(); ++j) {
      xs[0][j].insert(xs[0][j].end(), xs[i][j].begin(), xs[i][j].end());
      ys[0][j].insert(ys[0][j].end(), ys[i][j].begin(), ys[i][j].end());
    }
  }
}

vector< vector<SimplePulse>* >& SiTs  = all_SiT_channels;
vector< vector<SimplePulse>* >& SiR1s = all_SiR1_channels;
vector< vector<SimplePulse>* >& SiL1s = all_SiL1_channels;
void psel(TTree* tr, const char* ofname, bool usealllayers=true,
          bool verbose=false) {
  TFile* ofile = new TFile(ofname, "RECREATE");
  SetTMEBranchAddresses(tr);
  CollectChannels();
  TMECal::Init();
  vector< vector<TH3*> > hrevde(NSIR+1, vector<TH3*>(NPTYPE));
  vector< vector<TH3*> > hlevde(NSIL+1, vector<TH3*>(NPTYPE));
  vector< vector< vector<double> > > // [Si Segment][Particle][Point]
    vre (NSIR+1, vector< vector<double> >(NPTYPE)),
    vle (NSIL+1, vector< vector<double> >(NPTYPE)),
    vrde(NSIR+1, vector< vector<double> >(NPTYPE)),
    vlde(NSIL+1, vector< vector<double> >(NPTYPE));
  vector< vector<TH2*> > hrtve (NSIR+1, vector<TH2*>(NPTYPE));
  vector< vector<TH2*> > hltve (NSIL+1, vector<TH2*>(NPTYPE));
  vector< vector<TH1*> > hrdt  (NSIR+1, vector<TH1*>(NPTYPE));
  vector< vector<TH1*> > hldt  (NSIL+1, vector<TH1*>(NPTYPE));
  BookEvdEHistograms(ofile, 'r', hrevde);
  BookEvdEHistograms(ofile, 'l', hlevde);
  BookTvEHistograms (ofile, 'r', hrtve);
  BookTvEHistograms (ofile, 'l', hltve);
  BookdTHistograms  (ofile, 'r', hrdt);
  BookdTHistograms  (ofile, 'l', hldt);
  vector<ParticleLikelihood::PSelData> pls_r =
    ParticleLikelihood::LoadParticleLikelihoodsData('r');
  vector<ParticleLikelihood::PSelData> pls_l =
    ParticleLikelihood::LoadParticleLikelihoodsData('l');
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (verbose && i % 100000 == 0)
      std::cout << i << "/" << tr->GetEntries() << std::endl;
    if (TMECuts::OnlyOneHit(SiR2)) {
      for (int j = 0; j < 4; ++j) {
        SiEvent ev(SiR1s[j], SiR2, (usealllayers ? SiR3 : nullptr), nullptr);
        if (ev.Valid()) {
          int k = WhichParticle(pls_r, ev.dE(), ev.E());
          hrevde[j+1][k]->Fill(ev.E(), ev.dE(), ev.T());
          vre [j+1][k].push_back(ev.E());
          vrde[j+1][k].push_back(ev.dE());
          hrtve [j+1][k]->Fill(ev.T(), ev.E());
          hrdt  [j+1][k]->Fill(ev.dT());
        }
      }
    }
    if (TMECuts::OnlyOneHit(SiL3)) {
      for (int j = 0; j < 14; ++j) {
        SiEvent ev(SiL1s[j], SiL3, nullptr, &TMECal::SiL1A2E[j+2]);
        if (ev.Valid()) {
          int k = WhichParticle(pls_l, ev.dE(), ev.E());
          hlevde[j+2][k]->Fill(ev.E(), ev.dE(), ev.T());
          vle [j+1][k].push_back(ev.E());
          vlde[j+1][k].push_back(ev.dE());
          hltve [j+2][k]->Fill(ev.T(), ev.E());
          hldt  [j+2][k]->Fill(ev.dT());
        }
      }
    }
  }
  CombineHistograms(hrevde);
  CombineHistograms(hlevde);
  CombineGraphs(vre, vrde);
  CombineGraphs(vle, vlde);
  ConstructAndSaveGraphs(ofile, 'r', vre, vrde);
  ConstructAndSaveGraphs(ofile, 'l', vle, vlde);
  CombineHistograms(hrtve);
  CombineHistograms(hltve);
  CombineHistograms(hrdt);
  CombineHistograms(hldt);
  ofile->Write();
}

// run = 0: compile only
// run is an Al50 run number: run on that single run in Al50 dataset
// otherwise: fail
void R15b_Al50_psel(int run=0, bool usealllayers=true, bool verbose=false) {
  char ifname[128], ofname[128];
  if (run == 0)
    return;
  if (9890 <= run && run <= 10128) {
    sprintf(ifname, IFNAMEFMT, run);
  } else {
    PrintAndThrow("Unrecognized run");
  }
  sprintf(ofname, OFNAMEFMT, run);
  TChain* ch = new TChain("TMETree/TMETree");
  ch->Add(ifname);
  if (ch->GetEntries() <= 0)
    PrintAndThrow("No files to process!");
  psel(ch, ofname, usealllayers, verbose);
}
