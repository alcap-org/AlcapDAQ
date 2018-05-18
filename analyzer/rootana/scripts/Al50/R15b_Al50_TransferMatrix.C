#include "./scripts/Al50/util.h"

#include "TBranch.h"
#include "TFile.h"
#include "TFileMerger.h"
#include "TH2.h"
#include "TH2I.h"
#include "TRandom3.h"
#include "TTree.h"

#include "RooUnfoldResponse.h"

#include <algorithm>
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
// Run specific particle and file: root -l -b -q R15b_Al50_MC_EvdE.C+g\(mode\)
// Where mode is a char and one of (any other will compile only):
//   p (Protons), d (Deuteron), t (Tritons), a (Alphas)
// Input files should be output from g4sim with either p, d, t, or a generated
// inside of the target.
// Output files contain a RooUnfoldResponse object for the left and right
// Si detector packages. Also a tree containing the e0 and emeas, as well as
// the position of the target it was generated in.
////////////////////////////////////////////////////////////////////////////////

// Necessary since hadd doesn't know about RooUnfold objects
void Merge(const char* ifnameprefix, int n) {
  TFileMerger ofile;
  for (Long_t i = 1; i <= n; ++i) {
    TString ifname = TString(ifnameprefix) + i + ".root";
    ofile.AddFile(ifname);
  }
  TString ofname = TString(ifnameprefix) + ".root";
  ofile.OutputFile(ofname);
  ofile.Merge();
}

class Arm {
  Side arm;
  RooUnfoldResponse* response[10];
  TH2* diffVe[10];

public:
  Arm() {}
  Arm(Side s) : arm(s) {
    int    NE   = 75;
    double E[2] = {0., 15e3};
    char hname[128];
    for (int i = 0; i <= 9; ++i) {
      sprintf(hname, "Si%c_TM_%d", s == kLeft ? 'L' : 'R', i);
      response[i] = new RooUnfoldResponse(NE, E[0], E[1], hname);
      sprintf(hname, "Si%c_DM_%d", s == kLeft ? 'L' : 'R', i);
      diffVe[i] = new TH2I(hname,
                           "Difference vs Energy;E_0 [keV];E_0-E_{meas} [keV]",
                           NE, E[0], E[1], NE, E[0], E[1]/5);
    }
  }
  void Miss(double etrue, int i) {
    response[0]->Miss(etrue);
    response[i]->Miss(etrue);
  }
  void Hit(double emeas, double etrue, int i) {
    static TRandom3 smear;
    double e = smear.Gaus(emeas, 70.); // ARBITRARY RESOLUTION, FIX
    response[0]->Fill(e,     etrue);
    diffVe  [0]->Fill(etrue, etrue-e);
    response[i]->Fill(e,     etrue);
    diffVe  [i]->Fill(etrue, etrue-e);
  }
  void Fake(double emeas, int i) {
    response[0]->Fake(emeas);
    response[i]->Fake(emeas);
  }
  void Write() {
    for (int i = 0; i <= 9; ++i) {
      response[i]->Write();
      diffVe  [i]->Write();
    }
  }
  Side Position()      const { return arm; }
  TH2* Response(int i) const { return response[i]->Hresponse(); }
};

class Hit {
 private:
  bool isparticle, stopped;
  int seg;
  double edep/*keV*/;
  string vol;
  double x, y, z;
 public:
  Hit() : isparticle(false), stopped(false), seg(-1),
  edep(0), vol(""), x(0), y(0), z(0) {}
  Hit(bool isparticle, bool stopped, int seg, double edep,
      string vol, double x, double y, double z) :
  isparticle(isparticle), stopped(stopped),
  seg(seg), edep(edep), vol(vol), x(x), y(y), z(z) {}
  bool IsThick()      const { return vol == "SiL3" || vol == "SiR2" || vol == "SiR3"; }
  bool IsThin()       const { return vol == "SiL1" || vol == "SiR1"; }
  bool IsLeft()       const { return vol == "SiL1" || vol == "SiL3"; }
  bool IsRight()      const { return vol == "SiR1" || vol == "SiR2" || vol == "SiR3"; }
  bool FiducialLeft() const {
    return (vol == "SiL1" && 2 <= seg && seg <= 15) || vol == "SiL3";
  }
  bool IsSiDet()      const { return FiducialLeft() || IsRight(); }
  bool Stopped()      const { return stopped; }
  double EDep()       const { return edep; }
  string Volume()     const { return vol; }
  double X()          const { return x; }
  double Y()          const { return y; }
  double Z()          const { return z; }
  bool IsADetectedParticle()       const { return isparticle && IsSiDet(); }
  bool MatchedVolume(const Hit& h) const { return h.seg == seg && h.vol == vol; }
  int DetectorSegment()            const { return seg; }
  bool Valid()                     const { return !vol.empty(); }
  void Print() const {
    std::cout << "P: " << isparticle << ", stop: " << stopped << ", Vol: "
              << vol << "(" << seg << "), edep: " << edep << std::endl;
  }
};

class MCEvent {
  int detseg, targseg;
  double emeas, e0;
  double x0, y0, z0;
 public:
  MCEvent() : detseg(-1), targseg(-1), emeas(0.), e0(0.) {}
  MCEvent(int detseg, int targseg, double emeas, double e0,
          double x0, double y0, double z0) :
  detseg(detseg), targseg(targseg), emeas(emeas), e0(e0),
  x0(x0), y0(y0), z0(z0) {}
  static void SaveAsTree(TFile* f, const char* trname,
                         const vector<MCEvent>& evs) {
    TTree* tr = new TTree(trname, trname);
    tr->SetDirectory(f);
    MCEvent ev;
    tr->Branch("detseg",  &ev.detseg);
    tr->Branch("targseg", &ev.targseg);
    tr->Branch("e0",      &ev.e0);
    tr->Branch("emeas",   &ev.emeas);
    tr->Branch("x0",      &ev.x0);
    tr->Branch("y0",      &ev.y0);
    tr->Branch("z0",      &ev.z0);
    for (int i = 0; i < evs.size(); ++i) {
      ev = evs[i];
      tr->Fill();
    }
    tr->Write();
  }
};

class MCProcessor {
  TTree* tr;
  Particle p0;
  vector<string> *particles, *vols;
  vector<double> *edeps, *es, *xs, *ys, *zs;
  vector<int>    *stops, *segs;
  vector<Hit>    hits;
  double hitEs[2][2]; // [Side][Thin/Thick]
 public:
  MCProcessor(TTree* tr, const Particle& p0) :
  tr(tr), p0(p0), particles(nullptr), vols(nullptr), edeps(nullptr),
  es(nullptr), xs(nullptr), ys(nullptr), zs(nullptr), stops(nullptr),
  segs(nullptr), hits{Hit(), Hit()} {
    tr->SetBranchAddress("M_particleName", &particles);
    tr->SetBranchAddress("M_volName",      &vols);
    tr->SetBranchAddress("M_volID",        &segs);   // Segment (repetition #)
    tr->SetBranchAddress("M_stopped",      &stops);
    tr->SetBranchAddress("M_local_x",      &xs);     // cm
    tr->SetBranchAddress("M_local_y",      &ys);     // cm
    tr->SetBranchAddress("M_local_z",      &zs);     // cm
    tr->SetBranchAddress("M_ekin",         &es);     // GeV
    tr->SetBranchAddress("M_edep",         &edeps);  // GeV
  }
  int NEvents() const { return tr->GetEntries(); }
  int NHits()   const { return particles->size(); }
  double E0()   const { return es->at(0)*1e6; }
  double EL()   const { return hitEs[0][0]+hitEs[0][1]; }
  double ER()   const { return hitEs[1][0]+hitEs[1][1]; }
  double X0()   const { return xs->at(0); }
  double Y0()   const { return ys->at(0); }
  double Z0()   const { return zs->at(0); }
  int TargetSegment() const {
    const double L    = 8.7; // cm, length of target side (height and width)
    const double X[4] = {-L/2, -L/2+L/3, -L/2+2*L/3, L/2};
    const double Y[4] = {-L/2, -L/2+L/3, -L/2+2*L/3, L/2};
    double x = xs->at(0), y = ys->at(0);
    if      (x < X[1] && y < Y[1]) return 1;
    else if (x < X[2] && y < Y[1]) return 2;
    else if (            y < Y[1]) return 3;
    else if (x < X[1] && y < Y[2]) return 4;
    else if (x < X[2] && y < Y[2]) return 5;
    else if (            y < Y[2]) return 6;
    else if (x < X[1]            ) return 7;
    else if (x < X[2]            ) return 8;
    else                           return 9;
  }
  int DetectorSegment() const {
    return hits[0].DetectorSegment();
  }
  Hit GetHit(int i) const {
    return Hit(particles->at(i) == p0.Name, stops->at(i), segs->at(i)+1,
               edeps->at(i)*1e6, vols->at(i), xs->at(i), ys->at(i),
               zs->at(i));
  }
  void Process(int i) {
    tr->GetEvent(i);
    hits = { Hit(), Hit() };
    // tranhit = Hit();
    // stophit = Hit();
    hitEs[0][0] = hitEs[0][1] = hitEs[1][0] = hitEs[1][1] = 0.;

    for (int j = 1; j < NHits(); ++j) {
      Hit hit = GetHit(j);
      if (!hit.IsADetectedParticle())
        continue;
      int s = hit.IsLeft() ? 0 : 1;
      if      (hit.IsThin())                    hitEs[s][0] += hit.EDep();
      else if (hit.IsThick())                   hitEs[s][1] += hit.EDep();
      if      (hit.Stopped())                   hits[1]      = hit;
      else if (!hits[0].Valid() && hit.IsThin()) hits[0]      = hit;
    }
  }
  bool ValidLeftHit()  {
    return hits[1].IsLeft() && hits[1].IsThick() &&
           hits[0].IsLeft() && hits[0].IsThin();
  }
  bool ValidRightHit() {
    return hits[1].IsRight() && hits[1].IsThick() &&
           hits[0].IsRight() && hits[0].IsThin();
  }
  void Print() {
    std::cout << "e0: " << E0() << ", (el, er): (" << EL() << ", " << ER()
              << "), tranhit-->" << std::endl;
    hits[0].Print();
    std::cout << "stophit-->" << std::endl;
    hits[1].Print();
  }
  void Print(int i) {
    GetHit(i).Print();
  }
};

void TM(const char* ifname, map<Side,Arm> arms, vector<MCEvent>& levs,
        vector<MCEvent>& revs, const Particle& p, bool verbose=false) {
  TFile* ifile = new TFile(ifname, "READ");
  if (ifile->IsZombie())
    PrintAndThrow("Cannot find file!");
  TTree* tree = (TTree*)ifile->Get("tree");
  MCProcessor evs(tree, p);
  for (int i = 0; i < evs.NEvents(); ++i) {
    if (verbose && i % 100000 == 0)
      std::cout << i << " / " << evs.NEvents() << std::endl;

    evs.Process(i);
    int targseg = evs.TargetSegment();
    int detseg  = evs.DetectorSegment();
    if (evs.ValidLeftHit()) {
      arms[kLeft] .Hit(evs.EL(), evs.E0(), targseg);
      levs.push_back(MCEvent(detseg, targseg, evs.EL(), evs.E0(),
                             evs.X0(), evs.Y0(), evs.Z0()));
      arms[kRight].Miss(evs.E0(), targseg);
    } else if (evs.ValidRightHit()) {
      arms[kRight].Hit(evs.ER(), evs.E0(), targseg);
      revs.push_back(MCEvent(detseg, targseg, evs.ER(), evs.E0(),
                             evs.X0(), evs.Y0(), evs.Z0()));
      arms[kLeft] .Miss(evs.E0(), targseg);
    } else {
      arms[kLeft] .Miss(evs.E0(), targseg);
      arms[kRight].Miss(evs.E0(), targseg);
    }
  }
  ifile->Close();
  delete ifile;
}

void TM(const char* ifname, const char* ofname, const Particle& p,
        bool verbose=false) {
  map<Side,Arm> arms;
  vector<MCEvent> levs, revs;
  arms[kLeft]  = Arm(kLeft);
  arms[kRight] = Arm(kRight);
  TM(ifname, arms, levs, revs, p, verbose);
  TFile* ofile = new TFile(ofname, "RECREATE");
  arms[kLeft] .Write();
  arms[kRight].Write();
  MCEvent::SaveAsTree(ofile, "SiL_TransTree", levs);
  MCEvent::SaveAsTree(ofile, "SiR_TransTree", revs);
  ofile->Close();
}

void R15b_Al50_TransferMatrix(const char* ifprefix, int n) {
  Merge(ifprefix, n);
}

void R15b_Al50_TransferMatrix(char mode='\0', const char* ifname=nullptr,
                              const char* ofname=nullptr, bool verbose=false) {
  Particle p;
  switch(mode) {
    case 'p':  p = PROTON;   break;
    case 'd':  p = DEUTERON; break;
    case 't':  p = TRITON;   break;
    case 'a':  p = ALPHA;    break;
    default: return;
  }
  TM(ifname, ofname, p, verbose);
}
