#include "./scripts/Al50/psel.h"
#include "./scripts/Al50/util.h"
#include "./scripts/TMETree/TMETreeBranches.h"
#include "./scripts/TMETree/TMEUtils.h"
#include "./src/plotters/SimplePulse.h"

#include "TChain.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
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

struct PIDEvent {
  Double_t e, de, t, dt;
  PIDEvent (Double_t e=0, Double_t de=0, Double_t t=0, Double_t dt=0) :
  e(e), de(de), t(t) , dt(dt) {}
};

void BookEvdEHistograms(TFile* f, char lr, vector< vector<TH3*> >& hs) {
  const int NX = 400, NY = 200, NZ = 4;
  // const int NX = 100, NY = 25, NZ = 240;
  Double_t* X = HistUtils::ConstructBins(NX, 0,    20e3);
  Double_t* Y = HistUtils::ConstructBins(NY, 0,    5e3);
  Double_t  Z[NZ+1] = { -200, 400, 1600, 2400, 4500};
  // Double_t* Z = HistUtils::ConstructBins(NZ, -1e3, 5e3);
  TH1::SetDefaultSumw2(kTRUE);
  for (int i = 0; i < hs.size(); ++i) {
    for (int j = 0; j < hs[i].size()-1; ++j) {
      char hname[128], htitle[128];
      sprintf(hname,  "evde_%c%d_%s", lr, i, PARTICLES[j]->Name.c_str());
      sprintf(htitle, "E vs dE %c%d;E [keV];dE [keV];t [ns]", std::toupper(lr), i);
      hs[i][j] = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
      hs[i][j]->SetDirectory(f);
    }
    char hname[128], htitle[128];
    sprintf(hname,  "evde_%c%d_unclassified", lr, i);
    sprintf(htitle, "E vs dE %c%d;E;dE;t", std::toupper(lr), i);
    hs[i].back() = new TH3F(hname, htitle, NX, X, NY, Y, NZ, Z);
    hs[i].back()->SetDirectory(f);
  }
}

void ConstructAndSaveTrees(TFile* f, char lr,
                           const vector< vector< vector<PIDEvent> > >& pids) {
  TDirectory* cwd = gDirectory;
  f->cd();
  for (int i = 0; i < pids.size(); ++i) {
    for (int j = 0; j < pids[i].size(); ++j) {
      char trname[32];
      if (j < pids[i].size()-1)
        sprintf(trname, "PID_%c%d%c", std::toupper(lr), i+1,
                std::toupper(PARTICLES[j]->Name[0]));
      else
        sprintf(trname, "PID_%c%dU", std::toupper(lr), i+1);
      TTree* tr = new TTree(trname, trname);
      PIDEvent pid;
      tr->Branch("e",  &pid.e);
      tr->Branch("de", &pid.de);
      tr->Branch("t",  &pid.t);
      tr->Branch("dt", &pid.dt);
      for (int k = 0; k < pids[i][j].size(); ++k) {
        pid = pids[i][j][k];
        tr->Fill();
      }
    }
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
      hs[i][j] = new TH2F(hname, htitle, 2000, -10e3, 10e3, 200, 0, 20e3);
      hs[i][j]->SetDirectory(f);
    }
  }
  for (int i = 0; i < hs.size(); ++i) {
    sprintf(hname,  "tve_%c%d_unclassified", lr, i);
    sprintf(htitle, "T vs E %c%d", std::toupper(lr), i);
    hs[i].back() = new TH2F(hname, htitle, 2000, -10e3, 10e3, 200, 0, 20e3);
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

void BookTHistograms(TFile* f, char lr, vector< vector<TH1*> >& hs) {
  TH1::SetDefaultSumw2(kTRUE);
  char hname[128], htitle[128];
  for (int i = 0; i < hs.size(); ++i) {
    for (int j = 0; j < hs[i].size()-1; ++j) {
      sprintf(hname, "t_%c%d_%s", lr, i, PARTICLES[j]->Name.c_str());
      sprintf(htitle, "TME T_{thin} %c%d;T [ns]", std::toupper(lr), i);
      hs[i][j] = new TH1F(hname, htitle, 4000, -20.e3, 20.e3);
      hs[i][j]->SetDirectory(f);
    }
  }
  for (int i = 0; i < hs.size(); ++i) {
    sprintf(hname, "t_%c%d_unclassified", lr, i);
    sprintf(htitle, "TME T_{thin} %c%d;T [ns]", std::toupper(lr), i);
    hs[i].back() = new TH1F(hname, htitle, 4000, -20.e3, 20.e3);
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
  vector< vector< vector<PIDEvent> > > // [Si Segment][Particle][Point]
    vrpids(NSIR, vector< vector<PIDEvent> >(NPTYPE)),
    vlpids(NSIL, vector< vector<PIDEvent> >(NPTYPE));
  vector< vector<TH2*> > hrtve (NSIR+1, vector<TH2*>(NPTYPE));
  vector< vector<TH2*> > hltve (NSIL+1, vector<TH2*>(NPTYPE));
  vector< vector<TH1*> > hrdt  (NSIR+1, vector<TH1*>(NPTYPE));
  vector< vector<TH1*> > hldt  (NSIL+1, vector<TH1*>(NPTYPE));
  vector< vector<TH1*> > hrt   (NSIR+1, vector<TH1*>(NPTYPE));
  vector< vector<TH1*> > hlt   (NSIL+1, vector<TH1*>(NPTYPE));
  BookEvdEHistograms(ofile, 'r', hrevde);
  BookEvdEHistograms(ofile, 'l', hlevde);
  BookTvEHistograms (ofile, 'r', hrtve);
  BookTvEHistograms (ofile, 'l', hltve);
  BookdTHistograms  (ofile, 'r', hrdt);
  BookdTHistograms  (ofile, 'l', hldt);
  BookTHistograms   (ofile, 'r', hrt);
  BookTHistograms   (ofile, 'l', hlt);
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
          vrpids[j]  [k].push_back(PIDEvent(ev.E(), ev.dE(), ev.T(), ev.dT()));
          hrtve [j+1][k]->Fill(ev.T(), ev.E());
          hrdt  [j+1][k]->Fill(ev.dT());
          hrt   [j+1][k]->Fill(ev.T());
        }
      }
    }
    if (TMECuts::OnlyOneHit(SiL3)) {
      for (int j = 1; j < 15; ++j) {
        SiEvent ev(SiL1s[j], SiL3, nullptr, &TMECal::SiL1A2E[j]);
        if (ev.Valid()) {
          int k = WhichParticle(pls_l, ev.dE(), ev.E());
          hlevde[j+1][k]->Fill(ev.E(), ev.dE(), ev.T());
          vlpids[j]  [k].push_back(PIDEvent(ev.E(), ev.dE(), ev.T(), ev.dT()));
          hltve [j+1][k]->Fill(ev.T(), ev.E());
          hldt  [j+1][k]->Fill(ev.dT());
        }
      }
    }
  }
  CombineHistograms(hrevde);
  CombineHistograms(hlevde);
  ConstructAndSaveTrees(ofile, 'r', vrpids);
  ConstructAndSaveTrees(ofile, 'l', vlpids);
  CombineHistograms(hrtve);
  CombineHistograms(hltve);
  CombineHistograms(hrdt);
  CombineHistograms(hldt);
  CombineHistograms(hrt);
  CombineHistograms(hlt);
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
