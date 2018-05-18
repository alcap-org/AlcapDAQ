#include "scripts/Al50/util.h"

#include "TBranch.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH3F.h"
#include "TLegend.h"
#include "TTree.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using std::string;
using std::vector;


////////////////////////////////////////////////////////////////////////////////
// USER MODIFIABLE VARIABLES
// Compile only: root -l -b -q R15b_Al50_MC_EvdE.C+g
// Serially run all: root -l -b -q R15b_Al50_MC_EvdE.C+g\(false\)
// Run specific particle and file:
//   root -l -b -q R15b_Al50_MC_EvdE.C+g(n, mode)
// Where n indicated file number/index and mode is one of:
//   0 (compile ony, default), 1 (Protons), 2 (Deuteron), 3 (Triton), 4 (Alpha),
//   5 (All particle types)
// n==0 loops over all files, assuming NIFILES is correctly set.
// Input files should be output from g4sim with either p, d, t, or a generated
// inside of the target. The energy, position, and direction do not matter as
// long as the primary is generated inside of the target and is one of p, d,
// t, or a.
// Input files are assumed to have the file name format NAME_#.root, where #
// counts up from 1. For each of these, one output file is created and has
// the same number suffix as the input file.
// Output files should be hadded by the user when finished.
static const char*  IFNAMEFMT_P = "~/data/R15b/mc/R15bAl50ProtTarg_%d.root";
static const char*  IFNAMEFMT_D = "~/data/R15b/mc/R15bAl50DeutTarg_%d.root";
static const char*  IFNAMEFMT_T = "~/data/R15b/mc/R15bAl50TritTarg_%d.root";
static const char*  IFNAMEFMT_A = "~/data/R15b/mc/R15bAl50AlphaTarg_%d.root";
static const char*  IFNAMEFMT_M = "XXXXXXXXXXXXX";
static const char*  IFNAMEFMT_E = "XXXXXXXXXXXXX";
static const char*  IFNAMEFMT_G = "XXXXXXXXXXXXX";
static const char*  OFNAMEFMT_P = "~/data/R15b/Proton_EvdE_MC_%d.root";
static const char*  OFNAMEFMT_D = "~/data/R15b/Deuteron_EvdE_MC_%d.root";
static const char*  OFNAMEFMT_T = "~/data/R15b/Triton_EvdE_MC_%d.root";
static const char*  OFNAMEFMT_A = "~/data/R15b/Alpha_EvdE_MC_%d.root";
static const char*  OFNAMEFMT_M = "XXXXXXXXXXXXX";
static const char*  OFNAMEFMT_E = "XXXXXXXXXXXXX";
static const char*  OFNAMEFMT_G = "XXXXXXXXXXXXX";
// The number of g4sim generated files for each particle.
static const int    NIFILES     = 12;
// Histogram bins
static const double NE          = 2000;              // 10 keV
static const double NDE         = 1000;              // 10 keV
static const double ELO         = 0;                 // keV
static const double EHI         = 20e3;              // keV
static const double DELO        = 0;                 // keV
static const double DEHI        = 10e3;              // keV
static const int    NT          = 2;
static const Double_t T[NT+1]   = {-200, 400, 4500}; // ns?
////////////////////////////////////////////////////////////////////////////////

Double_t* ConstructBins(int n, Double_t xlo, Double_t xhi) {
  Double_t* x = new Double_t[n+1];
  Double_t dx = (xhi-xlo)/n;
  for (int i = 0; i <= n; ++i)
    x[i] = i*dx+xlo;
  return x;
}

class ParticleHist {
  string type;
  string particle_name;
  bool stopped;
  TH3F* hEvdE;

public:
  ParticleHist() {}
  ParticleHist(const string& type, const string& pname, bool stop) :
  type(type), particle_name(pname), stopped(stop) {}
  bool Match(string p, bool s) {
    return p == particle_name && s == stopped;
  }
  int Fill(double E, double dE, double t) {
    return hEvdE->Fill(E, dE, t);
  }
  void BookHist(int nx, const Double_t* x,
                int ny, const Double_t* y,
                int nz, const Double_t* z,
                string d) {
    TH1::SetDefaultSumw2(kTRUE);
    char name[128], title[256];
    sprintf(name,  "hEvdE_%s_%s", d.c_str(), type.c_str());
    sprintf(title, "EvdE %s (%s);E_{tot} [keV];dE [keV];t [ns]",
            d.c_str(), type.c_str());
    hEvdE = new TH3F(name, title, nx, x, ny, y, nz, z);
  }
  void Write(TFile* f) {
    hEvdE->SetDirectory(f);
    hEvdE->Write();
  }
};

class Arm {
  string det, thin, thick, part;
  vector<ParticleHist> hists;
  TH3F* hAllEvdE;

public:
  Arm(const string& det, const string& thin, const string& thick,
      const string& part) :
  det(det), thin(thin), thick(thick), part(part) {}

  bool ValidThinMatch(const string& d, const string& p) {
    return d == thin && p == part;
  }
  bool ValidThickMatch(const string& d, const string& p) {
    return d == thick && p == part;
  }
  void Fill(const string& p, bool stopped, double E, double dE, double t) {
    for (int i = 0; i < hists.size(); ++i) {
      if (hists[i].Match(p, stopped)) {
        hists[i].Fill(E, dE, t);
        break;
      }
    }
    hAllEvdE->Fill(E, dE, t);
  }
  void Register(const ParticleHist& p) {
    hists.push_back(p);
  }
  void BookHists(int nx, const Double_t* x,
                 int ny, const Double_t* y,
                 int nz, const Double_t* z) {
    TH1::SetDefaultSumw2(kTRUE);
    char name[128], title[128];
    sprintf(name,  "hAll_EvdE_%s", det.c_str());
    sprintf(title, "EvdE plot for the %s detector;E + dE [keV];dE [keV]",
            det.c_str());
    hAllEvdE = new TH3F(name, title, nx, x, ny, y, nz, z);
    for (int i = 0; i < hists.size(); ++i)
      hists[i].BookHist(nx, x, ny, y, nz, z, det);
  }
  void Save(TFile* f) {
    for (int i = 0; i < hists.size(); ++i)
      hists[i].Write(f);
    hAllEvdE->SetDirectory(f);
    hAllEvdE->Write();
  }
};

struct PIDEvent {
  Double_t e, de, t, dt;
  PIDEvent (Double_t e=0, Double_t de=0, Double_t t=0, Double_t dt=0) :
  e(e), de(de), t(t) , dt(dt) {}
};

void ConstructAndSaveTrees(TFile* f, const vector<PIDEvent> pids[2]) {
  TDirectory* cwd = gDirectory;
  f->cd();
  TTree* tr[2] = { new TTree("PID_L", "PID Left MC"),
                   new TTree("PID_R", "PID Right MC") };
  PIDEvent pid[2];
  for (int i = 0; i < 2; ++i) {
    tr[i]->Branch("e",  &pid[i].e);
    tr[i]->Branch("de", &pid[i].de);
    tr[i]->Branch("t",  &pid[i].t);
    tr[i]->Branch("dt", &pid[i].dt);
    for (int j = 0; j < pids[i].size(); ++j) {
      pid[i] = pids[i][j];
      tr[i]->Fill();
    }
  }
  cwd->cd();
}

void EvdE(const char* ifname, const char* ofname, const Particle& parttype,
          bool verbose=false) {
  TFile* ifile = new TFile(ifname, "READ");
  TTree* tree  = (TTree*)ifile->Get("tree");
  TFile* ofile = new TFile(ofname, "RECREATE");

  vector<int>*    tids      = 0x0;
  vector<string>* particles = 0x0;
  vector<string>* vols      = 0x0;
  vector<double>* edeps     = 0x0;
  vector<int>*    stops     = 0x0;
  vector<double>* ts        = 0x0;

  tree->SetBranchAddress("M_tid",          &tids);
  tree->SetBranchAddress("M_particleName", &particles);
  tree->SetBranchAddress("M_volName",      &vols);
  tree->SetBranchAddress("M_edep",         &edeps);
  tree->SetBranchAddress("M_stopped",      &stops);
  tree->SetBranchAddress("M_Ot",           &ts);

  const int n_arms = 2;
  Arm LeftArm ("SiL", "SiL1", "SiL3", parttype.Name);
  Arm RightArm("SiR", "SiR1", "SiR2", parttype.Name);
  Arm arms[n_arms] = {LeftArm, RightArm};
  vector<PIDEvent> vpids[2];

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    ParticleHist stopped    ("stopped",     parttype.Name, true);
    ParticleHist not_stopped("not_stopped", parttype.Name, false);
    arms[i_arm].Register(stopped);
    arms[i_arm].Register(not_stopped);
    arms[i_arm].BookHists(NE,  ConstructBins(NE,  ELO,  EHI),
                          NDE, ConstructBins(NDE, DELO, DEHI),
                          NT,  T);
  }

  for (int i_entry = 0; i_entry < tree->GetEntries(); ++i_entry) {
     tree->GetEvent(i_entry);

    if (verbose && i_entry % 10000 == 0)
      std::cout << i_entry << " / " << tree->GetEntries() << std::endl;

    double dE = 0, E = 0, t0 = 0., t1 = 0.;
    bool   stop = false;
    for (int iElement = 0; iElement < particles->size(); ++iElement) {
      const string& particle = particles->at(iElement);
      const string& vol      = vols     ->at(iElement);
      const double  t        = ts       ->at(iElement);     // ns
      const double  edep     = edeps    ->at(iElement)*1e6; // keV
      // Loop through the arms
      for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
        if        (arms[i_arm].ValidThickMatch(vol, particle)) {
          E    = edep;
          t1   = t;
          stop = stops->at(iElement);
        } else if (arms[i_arm].ValidThinMatch(vol, particle)) {
          dE = edep;
          t0 = t;
        }
        if (E > 0. && dE > 0.) {
          arms[i_arm].Fill(particle, stop, E+dE, dE, t0);
          vpids[i_arm].push_back(PIDEvent(E+dE, dE));
          E    = 0.;
          dE   = 0.;
          stop = false;
        }
      }
    }
  }

  for (int i_arm = 0; i_arm < n_arms; ++i_arm)
    arms[i_arm].Save(ofile);
  ConstructAndSaveTrees(ofile, vpids);
  ofile->Close();
  ifile->Close();
}

// Particle SelectParticle(int mode=0, const char* ifname=0x0,
//                         const char* ofname=0x0) {
//   switch (mode) {
//     case 1: return PROTON;
//     case 2: return DEUTERON;
//     case 3: return TRITON;
//     case 4: return ALPHA;
//     case 5: return MUON;
//     case 6: return ELECTRON;
//     case 7: return PHOTON;
//     default: return Particle();
//   }
// }

void R15b_Al50_MC_EvdE(int mode, const char* ifname, const char* ofname,
                       bool verbose=false) {
  Particle p;
  switch (mode) {
    case 0:               return;
    case 1: p = PROTON;   break;
    case 2: p = DEUTERON; break;
    case 3: p = TRITON;   break;
    case 4: p = ALPHA;    break;
    case 5: p = MUON;     break;
    case 6: p = ELECTRON; break;
    case 7: p = PHOTON;   break;
  }
  EvdE(ifname, ofname, p, verbose);
}

void R15b_Al50_MC_EvdE(int mode=0, int n=0, bool verbose=false) {
  char ifname[64], ofname[64];
  Particle p;
  switch (mode) {
  case 0:
    return;
  case 1:
    sprintf(ifname, IFNAMEFMT_P, n);
    sprintf(ofname, OFNAMEFMT_P, n);
    p = PROTON;
    break;
  case 2:
    sprintf(ifname, IFNAMEFMT_D, n);
    sprintf(ofname, OFNAMEFMT_D, n);
    p = DEUTERON;
    break;
  case 3:
    sprintf(ifname, IFNAMEFMT_T, n);
    sprintf(ofname, OFNAMEFMT_T, n);
    p = TRITON;
    break;
  case 4:
    sprintf(ifname, IFNAMEFMT_A, n);
    sprintf(ofname, OFNAMEFMT_A, n);
    p = ALPHA;
    break;
  case 5:
    sprintf(ifname, IFNAMEFMT_M, n);
    sprintf(ofname, OFNAMEFMT_M, n);
    p = MUON;
    break;
  case 6:
    sprintf(ifname, IFNAMEFMT_E, n);
    sprintf(ofname, OFNAMEFMT_E, n);
    p = ELECTRON;
    break;
  case 7:
    sprintf(ifname, IFNAMEFMT_G, n);
    sprintf(ofname, OFNAMEFMT_G, n);
    p = PHOTON;
    break;
  }
  EvdE(ifname, ofname, p, verbose);
}
