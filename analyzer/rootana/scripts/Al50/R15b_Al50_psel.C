#include "./src/plotters/SimplePulse.h"
#include "./scripts/TMETree/TMETreeBranches.h"
#include "./scripts/TMETree/TMEUtils.h"
#include "./scripts/Al50/util.h"

#include "TChain.h"
#include "TFile.h"
#include "TH2.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TTree.h"

#include <cctype>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using std::string;
using std::vector;

////////////////////////////////////////////////////////////////////////////////
// USER MODIFIABLE VARIABLES
// Compile only: root -l -b -q R15b_Al50_psel.C+g
// Serially run on all runs: root -l -b -q R15b_Al50_psel.C+g\(-1\)
// Run on a single run #: root -l -b -q R15b_Al50_psel.C+g\(#\)
// Input data files should be TME tree files output from rootana.
// Input simulation (MC) files should be output from the
// script R15b_Al50_MC_EvdE.C.
static const char* IFNAMEFMT = "~/R15bTME/Al50/tme%05d.root";
static const char* PFNAME    = "~/data/R15b/Proton_EvdE_MC.root";
static const char* DFNAME    = "~/data/R15b/Deuteron_EvdE_MC.root";
static const char* TFNAME    = "~/data/R15b/Triton_EvdE_MC.root";
static const char* AFNAME    = "~/data/R15b/Alpha_EvdE_MC.root";
static const char* OFNAMEFMT = "~/data/R15b/psel%05d.root";
static const double EMIN_P   = 2e3;
static const double EMIN_D   = 2.5e3;
static const double EMIN_T   = 4e3;
static const double EMIN_A   = 10e3;
////////////////////////////////////////////////////////////////////////////////

// p, d, t, a, unclassified
static const int       NPTYPE       = 5;
static const int       NSIL         = 16;
static const int       NSIR         = 4;
static const char*     MCIFNAMES[4] = { PFNAME,  DFNAME,    TFNAME,  AFNAME };
static const Particle* PARTICLES[5] = { &PROTON, &DEUTERON, &TRITON, &ALPHA };
static const double    EMIN[4]      = { EMIN_P,  EMIN_D,    EMIN_T,  EMIN_A };

class ParticleLikelihood {
  Particle p;
  TH2* H;
  TProfile* h;
  double lim, emin;
  double res;
public:
  ParticleLikelihood() {}
  // Takes ownership of H
  ParticleLikelihood(const Particle* p, TH2* H=0x0,
                     double lim=0., double emin=0., double res=0.) :
  p(*p), H(H), h(0x0), lim(lim), emin(emin), res(res) {
    h = H->ProfileX("_pfx", 1, -1, "S");
    H->SetDirectory(0);
    h->SetDirectory(0);
  }
  double Prob(double dE, double E) const {
    if (E < emin) return 0.;
    int b       = h->FindFixBin(E);
    double mu   = h->GetBinContent(b);
    double std  = h->GetBinError(b);
    double sig2 = res*res+std*std;
    double A    = 1./std::sqrt(2*M_PI*sig2);
    return A*std::exp(-0.5*(dE-mu)*(dE-mu)/sig2);
  }
  double operator()(double dE, double E) const {
    return Prob(dE, E);
  }
  bool IsParticle(double dE, double E) const {
    return Prob(dE, E) > lim;
  }
  TH2*      GetHist()    { return H;   }
  TProfile* GetProfile() { return h;   }
  double    GetLim()     { return lim; }
};

void BookHistograms(TFile* f, char lr, vector< vector<TH2*> >& hs) {
  TH1::SetDefaultSumw2(kTRUE);
  char hname[128], htitle[128];
  for (int i = 0; i < hs.size(); ++i) {
    for (int j = 0; j <= hs[i].size(); ++j) {
      sprintf(hname,  "evde_%c%d_%s", lr, i, PARTICLES[i]->Name.c_str());
      sprintf(htitle, "E vs dE %c%d", std::toupper(lr), i);
      hs[i][j] = new TH2F(hname, htitle, 200, 0, 20e3, 100, 0, 10e3);
      hs[i][j]->SetDirectory(f);
    }
  }
  for (int j = 0; j <= hs[0].size())
}

vector<ParticleLikelihood> LoadParticleLikelihoods(char lr) {
  vector<ParticleLikelihood> pls;
  for (int i = 0; i < NPTYPE; ++i) {
    TFile f(MCIFNAMES[i]);
    char hname[64];
    sprintf(hname, "hEvdE_Si%c_stopped", lr);
    TH2F* H = (TH2F*)f.Get(hname);
    pls.push_back(ParticleLikelihood(PARTICLES[i], H, 1e-4, EMIN[i], 30.));
  }
  return pls;
}

int WhichParticle(const vector<ParticleLikelihood>& pls, double dE, double E) {
  for (int i = 0; i < pls.size(); ++i)
    if (pls[i].IsParticle(dE, E))
      return i;
  return pls.size();
}


vector< vector<SimplePulse>* >& SiTs  = all_SiT_channels;
vector< vector<SimplePulse>* >& SiR1s = all_SiR1_channels;
vector< vector<SimplePulse>* >& SiL1s = all_SiL1_channels;
void psel(TTree* tr, const char* ofname) {
  TFile* ofile = new TFile(ofname, "RECREATE");
  SetTMEBranchAddresses(tr);
  CollectChannels();
  TMECal::Init();
  vector< vector<TH2*> > hl(NPTYPE, vector<TH2*>(NSIR+1));
  vector< vector<TH2*> > hr(NPTYPE, vector<TH2*>(NSIL+1));
  BookHistograms(ofile, 'r', hr);
  BookHistograms(ofile, 'l', hl);
  vector<ParticleLikelihood> pls_r = LoadParticleLikelihoods('r');
  vector<ParticleLikelihood> pls_l = LoadParticleLikelihoods('l');
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (i % 10000 == 0) std::cout << i << "/" << tr->GetEntries() << std::endl;
    if (TMECuts::OnlyOneHit(SiR2)) {
      for (int j = 0; j < 4; ++j) {
        if (TMECuts::OnlyOneHit(SiR1s[j])) { // Inlcude time difference cut?
          double E  = SiR2    ->front().E;
          double dE = SiR1s[j]->front().E;
          int k = WhichParticle(pls_r, dE, E+dE);
          hr[j+1][k]->Fill(E+dE, dE);
        }
      }
    }
    if (TMECuts::OnlyOneHit(SiL3)) {
      for (int j = 0; j < 14; ++j) {
        if (TMECuts::OnlyOneHit(SiL1s[j])) {
          double E  = SiL3    ->front().E;
          double dE = TMECal::SiL1A2E[j+2](SiL1s[j]->front().Amp);
          int    k  = WhichParticle(pls_l, dE, E+dE);
          hl[j+2][k]->Fill(E+dE, dE);
        }
      }
    }
  }
  for (int i = 1; i < 5; ++i)
    for (int j = 0; j < NPTYPE; ++j)
      hr[0][j]->Add(hr[i][j]);
  for (int i = 1; i < 17; ++i)
    for (int j = 0; j < NPTYPE; ++j)
      hl[0][j]->Add(hl[i][j]);
  ofile->Write();
}

// run = 0: compile only
// run < 0: serially run on all runs
// run is an Al50 run number: run on that single run in Al50 dataset
// otherwise: fail
void R15b_Al50_psel(int run=0) {
  char ifname[128], ofname[128];
  if (run == 0) {
    return;
  } else if (run < 0) {
    for (int i = 9890; i <= 10128; ++i)
      R15b_Al50_psel(i);
    return;
  } else if (9890 <= run && run <= 10128) {
    sprintf(ifname, IFNAMEFMT, run);
  } else {
    throw "Unrecognized run";
  }
  sprintf(ofname, OFNAMEFMT, run);
  TChain* ch = new TChain("TMETree/TMETree");
  ch->Add(ifname);
  psel(ch, ofname);
}
