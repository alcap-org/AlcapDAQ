#include "scripts/Al50/util.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2.h"
#include "TH2I.h"

#include "RooUnfoldResponse.h"

#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using std::map;
using std::string;
using std::vector;

////////////////////////////////////////////////////////////////////////////////
// USER MODIFIABLE VARIABLES
// Compile only:                   root -l -b -q R15b_Al50_TransferMatrix.C+g
// Serially run all:               root -l -b -q R15b_Al50_MC_EvdE.C+g\(5\)
// Run specific particle and file: root -l -b -q R15b_Al50_MC_EvdE.C+g\(mode\)
// Where mode is one of:
//   0 (Compile only), 1 (Protons), 2 (Deuteron), 3 (Tritons), 4 (Alphas),
//   5 (All particles)
// Input files should be output from g4sim with either p, d, t, or a generated
// inside of the target. The energy, position, and direction do not matter as
// long as the primary is generated inside of the target and is one of p, d,
// t, or a.
// Input files are assumed to have the file name format IFNAMEPRE_#.root,
// where # counts up from 1.
// Output files contain a RooUnfoldResponse object for the left and right
// Si detector packages.
static const char IFNAMEPRE_P[] = "~/data/R15b/mc/R15bAl50ProtTarg";
static const char IFNAMEPRE_D[] = "~/data/R15b/mc/R15bAl50DeutTarg";
static const char IFNAMEPRE_T[] = "~/data/R15b/mc/R15bAl50TritTarg";
static const char IFNAMEPRE_A[] = "~/data/R15b/mc/R15bAl50AlphaTarg";
static const char OFNAME_P[]    = "~/data/R15b/al50ptm.root";
static const char OFNAME_D[]    = "~/data/R15b/al50dtm.root";
static const char OFNAME_T[]    = "~/data/R15b/al50ttm.root";
static const char OFNAME_A[]    = "~/data/R15b/al50atm.root";
// The number of g4sim generated files for each particle.
static const int NIFILES = 12;
////////////////////////////////////////////////////////////////////////////////

bool IsThick(const string& det) {
  static const string sil("SiL3"), sir("SiR2");
  return det == sil || det == sir;
}
bool IsThin(const string& det) {
  static const string sil("SiL1"), sir("SiR1");
  return det == sil || det == sir;
}
bool IsLeft(const string& det) {
  static const string arm("SiL"), thin("SiL1"), thick("SiL3");
  return det == arm || det == thin || det == thick;
}
bool IsRight(const string& det) {
  static const string arm("SiR"), thin("SiR1"), thick("SiR2");
  return det == arm || det == thin || det == thick;
}
bool IsSiArmDet(const string& det) {
  return IsLeft(det) || IsRight(det);
}
bool PrimaryIsIncorrect(const Particle& p0, const string& p,
                        const string& ovol, int trk) {
  return p0.Name != p || ovol != "Target" || trk != 1;
}
bool IsADetectedPrimary(int trk, const string& vol) {
  return trk == 1 && IsSiArmDet(vol);
}

class Arm {
  Side arm;
  RooUnfoldResponse* response;

public:
  Arm() {}
  Arm(const string& det, const Particle& p) :
  arm(IsLeft(det) ? kLeft : kRight), response(0x0) {
    response = new RooUnfoldResponse(200, 0, 20e3, (det+"_TM").c_str());
  }
  void Hit(double emeas, double etrue) { response->Fill(emeas, etrue); }
  void Miss(double etrue)              { response->Miss(etrue); }
  void Fake(double emeas)              { response->Fake(emeas); }
  void Write()                         { response->Write(); }
  Side Position()                const { return arm; }
  TH2* Response()                const { return response->Hresponse(); }
};

class LayerHit {
private:
  Layer layer;
  Side arm;
  double t;
  double e0, edep;

public:
  LayerHit() {}
  LayerHit(const string& det, double t, double e0, double edep) :
  layer(IsThick(det) ? kThick : kThin), arm(IsLeft(det) ? kLeft : kRight),
  t(t), e0(e0), edep(edep){}
  bool Match(const LayerHit& lh) const {
    // Add time cut?
    // std::abs(time - lh.time) < DT?
    return layer != lh.layer && arm == lh.arm;
  }
  bool   IsThin()    const { return layer == kThin; }
  double E()         const { return edep; }
  double E0()        const { return e0; }
  Side   Position()  const { return arm; }
  Layer  Thickness() const { return layer; }
};

class TelescopeHit {
  LayerHit thin, thick;
public:
  TelescopeHit(const LayerHit& h1, const LayerHit& h2) :
  thin(h1.IsThin() ? h1 : h2), thick(h2.IsThin() ? h1 : h2) {}
  static vector<TelescopeHit> BuildHits(const vector<LayerHit>& si1,
                                        const vector<LayerHit>& si2) {
    vector<TelescopeHit> telehits;
    for (int i = 0; i < si1.size(); ++i)
      for (int j = 0; j < si2.size(); ++j)
        if (si1[i].Match(si2[j]))
          telehits.push_back(TelescopeHit(si1[i], si2[j]));
  return telehits;
  }
  double E()              const { return thin.E() + thick.E(); }
  double E0()             const { return thin.E0(); }
  Side Position()         const { return thin.Position(); }
  const LayerHit& Thin()  const { return thin;}
  const LayerHit& Thick() const { return thick;}
};

void TM(const char* ifname, map<Side,Arm> arms, const Particle& p) {

  TFile* ifile = new TFile(ifname, "READ");
  TTree* tree = (TTree*) ifile->Get("tree");

  vector<string> *particleNames = 0x0;
  vector<string> *volNames = 0x0, *ovolNames = 0x0;
  vector<double> *edeps = 0x0;
  vector<int> *stops = 0x0;
  vector<int> *tids = 0x0;
  vector<double> *ts = 0x0, *es = 0x0;

  tree->SetBranchAddress("M_particleName", &particleNames);
  tree->SetBranchAddress("M_volName",      &volNames);
  tree->SetBranchAddress("M_ovolName",     &ovolNames);
  tree->SetBranchAddress("M_stopped",      &stops);
  tree->SetBranchAddress("M_tid",          &tids);  // Starts at 1
  tree->SetBranchAddress("M_t",            &ts);    // don't know, ns?
  tree->SetBranchAddress("M_e",            &es);    // GeV
  tree->SetBranchAddress("M_edep",         &edeps);
  int n_entries = tree->GetEntries();
  for (int iEntry = 0; iEntry < n_entries; ++iEntry) {
    tree->GetEvent(iEntry);
    if (iEntry % 10000 == 0)
      std::cout << iEntry << " / " << n_entries << std::endl;
    if (PrimaryIsIncorrect(p, particleNames->at(0), ovolNames->at(0),
                           tids->at(0)))
      throw "Incorrect primary in simulation!";

    vector<LayerHit> layer_hits[2];
    double e0 = (es->at(0)-p.M)*1e6; // keV, kinetic energy
    for (int iElement = 1; iElement < particleNames->size(); ++iElement) {
      const string& vol  = volNames->at(iElement);
      const int     trk  = tids->at(iElement);
      const bool    stop = stops->at(iElement);
      const double  edep = edeps->at(iElement)*1e6;
      const double  t    = ts->at(iElement);
      if (!IsADetectedPrimary(trk, vol))
        continue;
      if      (IsThin(vol)  && !stop)
        layer_hits[0].push_back(LayerHit(vol, t, e0, edep));
      else if (IsThick(vol) && stop)
        layer_hits[1].push_back(LayerHit(vol, t, e0, edep));
    }
    vector<TelescopeHit> tele_hits = TelescopeHit::BuildHits(layer_hits[0], layer_hits[1]);
    for (int i_th = 0; i_th < tele_hits.size(); ++i_th) {
      switch (tele_hits[i_th].Position()) {
        case kLeft:
        arms[kLeft] .Hit(tele_hits[i_th].E(), e0);
        arms[kRight].Miss(e0);
        break;
        case kRight:
        arms[kRight].Hit(tele_hits[i_th].E(), e0);
        arms[kLeft] .Miss(e0);
        break;
      }
    }
    if (tele_hits.empty()) {
      arms[kLeft] .Miss(e0);
      arms[kRight].Miss(e0);
    }
  }
  ifile->Close();
  delete ifile;
}

void TM(const char* ofname, const char* ifprefix, int n, const Particle& p) {
  vector<string> ifname;
  for (int i = 1; i <= n; ++i) {
    char fname[256];
    sprintf(fname, "%s_%d.root", ifprefix, i);
    ifname.push_back(string(fname));
  }

  map<Side,Arm> arms;
  arms[kLeft]  = Arm("SiR", p);
  arms[kRight] = Arm("SiL", p);
  for (int i = 0; i < ifname.size(); ++i)
    TM(ifname[i].c_str(), arms, p);
  TFile* ofile = new TFile(ofname, "RECREATE");
  arms[kLeft] .Write();
  arms[kRight].Write();
  ofile->Close();
}

void R15b_Al50_TransferMatrix(int mode=0) {
  switch(mode) {
  case 0:                                                           return;
  case 1: TM(OFNAME_P, IFNAMEPRE_P, NIFILES, PROTON);               return;
  case 2: TM(OFNAME_D, IFNAMEPRE_D, NIFILES, DEUTERON);             return;
  case 3: TM(OFNAME_T, IFNAMEPRE_T, NIFILES, TRITON);               return;
  case 4: TM(OFNAME_A, IFNAMEPRE_A, NIFILES, ALPHA);                return;
  case 5: for (int i = 1; i <= 4; ++i) R15b_Al50_TransferMatrix(i); return;
  }
}
