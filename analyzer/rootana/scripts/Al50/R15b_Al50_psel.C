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
  double e1, e2, e3, t1, t2, t3;
  int seg;
  PIDEvent (double e1=0, double e2=0, double e3=0,
            double t1=0, double t2=0, double t3=0, int seg=0) :
  e1(e1), e2(e2), e3(e3), t1(t1), t2(t2), t3(t3), seg(seg) {}
};

void ConstructAndSaveTrees(TFile* f, char lr,
                           const vector< vector<PIDEvent> >& pids) {
  TDirectory* cwd = gDirectory;
  f->cd();
  for (int i = 0; i < pids.size(); ++i) {
      char trname[32];
      if (i < pids.size()-1)
        sprintf(trname, "PID_%c%c", std::toupper(lr),
                std::toupper(PARTICLES[i]->Name[0]));
      else
        sprintf(trname, "PID_%cU", std::toupper(lr));
      TTree* tr = new TTree(trname, PARTICLES[i]->Name.c_str());
      PIDEvent pid;
      tr->Branch("e1",  &pid.e1);
      tr->Branch("e2",  &pid.e2);
      tr->Branch("e3",  &pid.e3);
      tr->Branch("t1",  &pid.t1);
      tr->Branch("t2",  &pid.t2);
      tr->Branch("t3",  &pid.t3);
      tr->Branch("seg", &pid.seg);
      for (int j = 0; j < pids[i].size(); ++j) {
        pid = pids[i][j];
        tr->Fill();
      }
      tr->SetAlias("e",  "e1+e2+e3");
      tr->SetAlias("de", "e1");
      tr->SetAlias("t",  "t1");
      tr->SetAlias("dt", "t2-t1");
    }
  cwd->cd();
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
  vector< vector<PIDEvent> > vrpids(NPTYPE), vlpids(NPTYPE);
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
          vrpids[k].push_back(PIDEvent(ev.E(0), ev.E(1), ev.E(2),
                                       ev.T(0), ev.T(1), ev.T(2), j+1));
        }
      }
    }
    if (TMECuts::OnlyOneHit(SiL3)) {
      for (int j = 1; j < 15; ++j) {
        SiEvent ev(SiL1s[j], SiL3, nullptr, &TMECal::SiL1A2E[j]);
        if (ev.Valid()) {
          int k = WhichParticle(pls_l, ev.dE(), ev.E());
          vlpids[k].push_back(PIDEvent(ev.E(0), ev.E(1), ev.E(2),
                                       ev.T(0), ev.T(1), ev.T(2), j+1));
        }
      }
    }
  }
  ConstructAndSaveTrees(ofile, 'r', vrpids);
  ConstructAndSaveTrees(ofile, 'l', vlpids);
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
