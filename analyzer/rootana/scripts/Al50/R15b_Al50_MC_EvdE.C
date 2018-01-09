#include "scripts/Al50/util.h"

#include "TBranch.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH2.h"
#include "TLegend.h"
#include "TTree.h"

#include <algorithm>
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
static const char*  OFNAMEFMT_P = "~/data/R15b/Proton_EvdE_MC_%d.root";
static const char*  OFNAMEFMT_D = "~/data/R15b/Deuteron_EvdE_MC_%d.root";
static const char*  OFNAMEFMT_T = "~/data/R15b/Triton_EvdE_MC_%d.root";
static const char*  OFNAMEFMT_A = "~/data/R15b/Alpha_EvdE_MC_%d.root";
// The number of g4sim generated files for each particle.
static const int    NIFILES     = 12;
// Time cuts for histograms
static const int    NT    = 4;
static const double T[NT] = { 100, 500 , 10e3, 100e6};
////////////////////////////////////////////////////////////////////////////////

int GetHistTimeIndex(double t) {
  for (int i = 0; i < NT; ++i)
    if (t < T[i])
      return i;
  return NT;
}

bool ValidT(double t) {
  return 0. < t && t < T[NT-1];
}

class ParticleHist {
  string type;
  string particle_name;
  bool stopped;
  TH2F* hEvdE[NT+1];

public:
  ParticleHist() {}
  ParticleHist(const string& type, const string& pname, bool stop) :
  type(type), particle_name(pname), stopped(stop) {}
  bool Match(string p, bool s) {
    return p == particle_name && s == stopped;
  }
  int Fill(double E, double dE, double t) {
    hEvdE[NT]->Fill(E, dE);
    return hEvdE[GetHistTimeIndex(t)]->Fill(E, dE);
  }
  void BookHist(int nx, double xlo, double xhi, int ny, double ylo, double yhi,
                string d) {
    TH1::SetDefaultSumw2(kTRUE);
    for (int i = 0; i < NT; ++i) {
      char name[128];
      sprintf(name, "hEvdE_%s_%s_t%d", d.c_str(), type.c_str(), i);
      hEvdE[i] = new TH2F(name, name, nx, xlo, xhi, ny, ylo, yhi);
      hEvdE[i]->SetXTitle("E_{tot} [keV]");
      hEvdE[i]->SetYTitle("dE [keV]");
    }
    char name[128];
    sprintf(name, "hEvdE_%s_%s", d.c_str(), type.c_str());
    hEvdE[NT] = new TH2F(name, name, nx, xlo, xhi, ny, ylo, yhi);
    hEvdE[NT]->SetXTitle("E_{tot} [keV]");
    hEvdE[NT]->SetYTitle("dE [keV]");
  }
  void Write(TFile* f) {
    for (int i = 0; i <= NT; ++i) {
      hEvdE[i]->SetDirectory(f);
      hEvdE[i]->Write();
    }
  }
};

class Arm {
  string det, thin, thick, part;
  vector<ParticleHist> hists;
  TH2F* hAllEvdE[NT+1];

public:
  Arm(const string& det, const string& thin, const string& thick,
      const string& part) :
  det(det), thin(thin), thick(thick), part(part) {}

  bool ValidThinMatch(const string& d, const string& p, double t) {
    return d == thin && p == part && ValidT(t);
  }
  bool ValidThickMatch(const string& d, const string& p, double t) {
    return d == thick && p == part && ValidT(t);
  }
  void Fill(const string& p, bool stopped, double E, double dE, double t) {
    if (!ValidT(t)) return;
    for (int i = 0; i < hists.size(); ++i) {
      if (hists[i].Match(p, stopped)) {
        hists[i].Fill(E, dE, t);
        break;
      }
    }
    hAllEvdE[NT]->Fill(E, dE);
    hAllEvdE[GetHistTimeIndex(t)]->Fill(E, dE);
  }
  void Register(const ParticleHist& p) {
    hists.push_back(p);
  }
  void BookHists(double w, double xlo, double xhi, double ylo, double yhi) {
    TH1::SetDefaultSumw2(kTRUE);
    int nx = (xhi - xlo) / w;
    int ny = (yhi - ylo) / w;
    for (int i = 0; i < NT; ++i) {
      char name[128], title[128];
      sprintf(name, "hAll_EvdE_%s_t%d", det.c_str(), i);
      sprintf(title, "EvdE plot for the %s detector;E + dE [keV];dE [keV]",
              det.c_str());
      hAllEvdE[i] = new TH2F(name, title, nx, xlo, xhi, ny, ylo, yhi);
    }
    char name[128], title[128];
    sprintf(name, "hAll_EvdE_%s", det.c_str());
    sprintf(title, "EvdE plot for the %s detector;E + dE [keV];dE [keV]",
            det.c_str());
    hAllEvdE[NT] = new TH2F(name, title, nx, xlo, xhi, ny, ylo, yhi);
    for (int i = 0; i < hists.size(); ++i)
      hists[i].BookHist(nx, xlo, xhi, ny, ylo, yhi, det);
  }
  void Save(TFile* f) {
    for (int i = 0; i < hists.size(); ++i)
      hists[i].Write(f);
    for (int i = 0; i <= NT; ++i) {
      hAllEvdE[i]->SetDirectory(f);
      hAllEvdE[i]->Write();
    }
  }
};

void EvdE(const char* ifname, const char* ofname, const Particle& parttype) {

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

  const double de   = 100;
  const double elo  = 0;
  const double ehi  = 20e3;
  const double delo = 0;
  const double dehi = 10e3;

  const int n_arms = 2;
  Arm LeftArm ("SiL", "SiL1", "SiL3", parttype.Name);
  Arm RightArm("SiR", "SiR1", "SiR2", parttype.Name);
  Arm arms[n_arms] = {LeftArm, RightArm};

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    ParticleHist stopped    ("stopped",     parttype.Name, 1);
    ParticleHist not_stopped("not_stopped", parttype.Name, 0);
    arms[i_arm].Register(stopped);
    arms[i_arm].Register(not_stopped);
    arms[i_arm].BookHists(de, elo, ehi, delo, dehi);
  }

  for (int i_entry = 0; i_entry < tree->GetEntries(); ++i_entry) {
     tree->GetEvent(i_entry);

    if (i_entry % 10000 == 0)
      std::cout << i_entry << " / " << tree->GetEntries() << std::endl;

    double dE      = 0;
    double E       = 0;
    bool   stop    = false;
    for (int iElement = 0; iElement < particles->size(); ++iElement) {
      const string& particle = particles->at(iElement);
      const string& vol      = vols     ->at(iElement);
      const double t         = ts       ->at(iElement);     // ns
      const double edep      = edeps    ->at(iElement)*1e6; // keV
      // Loop through the arms
      for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
        if        (arms[i_arm].ValidThickMatch(vol, particle, t)) {
          E = edep;
          stop = stops->at(iElement);
        } else if (arms[i_arm].ValidThinMatch(vol, particle, t)) {
          dE = edep;
        }
        if (E > 0. && dE > 0.) {
          arms[i_arm].Fill(particle, stop, E+dE, dE, t);
          E  = 0.;
          dE = 0.;
          stop = false;
        }
      }
    }
  }
  // Now print the PID plots
  for (int i_arm = 0; i_arm < n_arms; ++i_arm)
    arms[i_arm].Save(ofile);
  ofile->Close();
  ifile->Close();
}

void R15b_Al50_MC_EvdE(int n=0, int mode=0) {
  char ifname[64], ofname[64];
  if (n == 0)
    for (int i = 1; i <= NIFILES; ++i)
      R15b_Al50_MC_EvdE(i, mode);
  switch (mode) {
  case 0:
    return;
  case 1:
    sprintf(ifname, IFNAMEFMT_P, n);
    sprintf(ofname, OFNAMEFMT_P, n);
    EvdE(ifname, ofname, PROTON);
    return;
  case 2:
    sprintf(ifname, IFNAMEFMT_D, n);
    sprintf(ofname, OFNAMEFMT_D, n);
    EvdE(ifname, ofname, DEUTERON);
    return;
  case 3:
    sprintf(ifname, IFNAMEFMT_T, n);
    sprintf(ofname, OFNAMEFMT_T, n);
    EvdE(ifname, ofname, TRITON);
    return;
  case 4:
    sprintf(ifname, IFNAMEFMT_A, n);
    sprintf(ofname, OFNAMEFMT_A, n);
    EvdE(ifname, ofname, ALPHA);
    return;
  case 5:
    R15b_Al50_MC_EvdE(n, kProton);
    R15b_Al50_MC_EvdE(n, kDeuteron);
    R15b_Al50_MC_EvdE(n, kTriton);
    R15b_Al50_MC_EvdE(n, kAlpha);
    return;
  }
}
