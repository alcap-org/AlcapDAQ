#include "./scripts/Al50/util.h"

#include "RooUnfoldResponse.h"

#include "TBranch.h"
#include "TFile.h"
#include "TFileMerger.h"
#include "TH2.h"
#include "TH2I.h"
#include "TRandom3.h"
#include "TTree.h"


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
// Compile only:                   root -l -b -q R15b_Al50_TransferMatrix.C+g
// Where mode is a char and one of (any other will compile only):
//   p (Protons), d (Deuteron), t (Tritons), a (Alphas)
// Input files should be output from g4sim with either p, d, t, or a generated
// inside of the target.
// Output files contain a RooUnfoldResponse object for the left and right
// Si detector packages. Also a tree containing the e0 and emeas, as well as
// the position of the target it was generated in.
////////////////////////////////////////////////////////////////////////////////

namespace {
  const double ERES[2] = {70., 30.}; //[Thin/Thick], keV
  TRandom3 SMEAR;
}

class Arm {
  Side arm;
  RooUnfoldResponse* response;
  TH2* diffVe;

public:
  Arm() : arm(kLeft), response(nullptr), diffVe(nullptr) {}
  Arm(Side s) : arm(s), response(nullptr), diffVe(nullptr) {
    int    NE   = 200;
    double E[2] = {0., 20e3};
    response = new RooUnfoldResponse(NE, E[0], E[1],
                                     TString::Format("Si%c_TM", s == kLeft ?
                                                     'L' : 'R'));
    diffVe = new TH2I(TString::Format("Si%c_DM", s == kLeft ? 'L' : 'R'),
                      "Difference vs Energy;E_0 [keV];E_0-E_{meas} [keV]",
                      NE, E[0], E[1], NE, E[0], E[1]/5);
  }
  void Hit(double emeas, double etrue, int seg) {
    double eres[2]; // [Thin/Thick]
    if (arm == kLeft) {
      eres[0] = SiUtils
    }
    double e = SMEAR.Gaus(emeas, std::sqrt(ERES[0]*ERES[0]+ERES[1]*ERES[1]));
    response->Fill(e,     etrue);
    diffVe  ->Fill(etrue, etrue-e);
  }
  void Miss(double etrue) { response->Miss(etrue); }
  void Fake(double emeas) { response->Fake(emeas); }
  void Write()            { response->Write(); diffVe->Write(); }
  Side Position()   const { return arm; }
  TH2* Response()   const { return response->Hresponse(); }
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
  bool IsVeto()       const { return vol == "SiR3"; }
  bool IsThick()      const { return vol == "SiL3" || vol == "SiR2"; }
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
  bool IsADetectedParticle()       const { return isparticle && IsSiDet() && SiUtils::WithinThreshold(vol, seg, edep); }
  bool MatchedVolume(const Hit& h) const { return h.seg == seg && h.vol == vol; }
  int DetectorSegment()            const { return seg; }
  bool Valid()                     const { return !vol.empty(); }
  void Print() const {
    std::cout << "P: " << isparticle << ", stop: " << stopped << ", Vol: "
              << vol << "(" << seg << "), edep: " << edep << std::endl;
  }
};

class MCEvent {
  int detseg;
  double emeas, demeas, e0;
  double x0, y0, z0;
  int stoplayer; // Number indicating Si 1, 2, or 3; -1 for no-stop.
 public:
  MCEvent() : detseg(-1), emeas(0.), demeas(0.), e0(0.), x0(0), y0(0), z0(0),
              stoplayer(-1) {}
  MCEvent(int detseg, double emeas, double demeas, double e0,
          double x0, double y0, double z0, int stoplayer) :
  detseg(detseg), emeas(emeas), demeas(demeas), e0(e0), x0(x0), y0(y0), z0(z0),
  stoplayer(stoplayer) {
    if (emeas > 0.) {// Only if detected
      double desig = SMEAR.Gaus(0, ERES[0]);
      double esig  = SMEAR.Gaus(0, ERES[1]);
      emeas  += desig + esig;
      demeas += desig;
    }
  }
  static void SaveAsTree(TFile* f, const char* trname,
                         const vector<MCEvent>& evs) {
    TTree* tr = new TTree(trname, trname);
    tr->SetDirectory(f);
    MCEvent ev;
    tr->Branch("detseg",    &ev.detseg);
    tr->Branch("e0",        &ev.e0);
    tr->Branch("emeas",     &ev.emeas);
    tr->Branch("demeas",    &ev.demeas);
    tr->Branch("x0",        &ev.x0);
    tr->Branch("y0",        &ev.y0);
    tr->Branch("z0",        &ev.z0);
    tr->Branch("stoplayer", &ev.stoplayer);
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
  double hitEs[2][3]; // [Side][Thin/Thick/Veto]
  int stoplayer;
 public:
  MCProcessor(TTree* tr) :
  tr(tr), p0(), particles(nullptr), vols(nullptr), edeps(nullptr),
  es(nullptr), xs(nullptr), ys(nullptr), zs(nullptr), stops(nullptr),
  segs(nullptr), hits{Hit(), Hit(), Hit()}, hitEs{0.}, stoplayer(-1) {
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
  int NEvents()          const { return tr->GetEntries();          }
  int NHits()            const { return particles->size();         }
  double E0()            const { return es->at(0)*1e6;             }
  double EL()            const { return hitEs[0][0]+hitEs[0][1];   }
  double ER()            const { return hitEs[1][0]+hitEs[1][1];   }
  double dEL()           const { return hitEs[0][0];               }
  double dER()           const { return hitEs[1][0];               }
  double X0()            const { return xs->at(0);                 }
  double Y0()            const { return ys->at(0);                 }
  double Z0()            const { return zs->at(0);                 }
  int DetectorSegment()  const { return hits[0].DetectorSegment(); }
  int StopLayer()        const { return stoplayer;                 }
  bool ValidStop()       const { return stoplayer == 2;            }
  Particle GetParticle() const { return p0;                        }
  Hit GetHit(int i) const {
    return Hit(particles->at(i) == p0.Name, stops->at(i), segs->at(i)+1,
               edeps->at(i)*1e6, vols->at(i), xs->at(i), ys->at(i),
               zs->at(i));
  }
  void SetParticle() { p0 = Particle(particles->front()); }
  void PreProcessClear() {
    hits = { Hit(), Hit(), Hit() };
    hitEs[0][0] = hitEs[0][1] = hitEs[0][2] = hitEs[1][0] =
    hitEs[1][1] = hitEs[1][2] = 0.;
    stoplayer = -1;
  }
  void Process(int i) {
    PreProcessClear();
    tr->GetEvent(i);
    if (p0 == NULLPARTICLE)
      SetParticle();

    for (int j = 1; j < NHits(); ++j) {
      Hit hit = GetHit(j);
      if (!hit.IsADetectedParticle())
        continue;
      int s = hit.IsLeft() ? 0 : 1;
      if      (hit.IsThin())                      hitEs[s][0] += hit.EDep();
      else if (hit.IsThick())                     hitEs[s][1] += hit.EDep();
      else if (hit.IsVeto())                      hitEs[s][2] += hit.EDep();
      if      (!hits[0].Valid() && hit.IsThin())  hits[0]     = hit;
      else if (!hits[1].Valid() && hit.IsThick()) hits[1]     = hit;
      else if (!hits[2].Valid() && hit.IsVeto())  hits[2]     = hit;
      if      (hit.Stopped()    && hit.IsThin())  stoplayer   = 1;
      else if (hit.Stopped()    && hit.IsThick()) stoplayer   = 2;
      else if (hit.Stopped()    && hit.IsVeto())  stoplayer   = 3;
    }
  }
  bool ValidLeftHit()  {
    return hits[0].IsLeft() && hits[0].IsThin() &&
           hits[1].IsLeft() && hits[1].IsThick() &&
           ValidStop(); // This shouldn't be here...
  }
  bool ValidRightHit(bool veto=true) {
    return hits[0].IsRight() && hits[0].IsThin()  &&
           hits[1].IsRight() && hits[1].IsThick() &&
           !(veto && hits[2].Valid())             &&
           ValidStop(); // This shouldn't be here...
  }
  bool ValidGeneratedParticle() {
    return !vols->empty() && vols->at(0) == "Target" &&
    particles->at(0) == p0.Name;
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

Particle TM(const char* ifname, map<Side,Arm> arms, vector<MCEvent>& levs,
        vector<MCEvent>& revs, bool verbose=false) {
  TFile* ifile = new TFile(ifname, "READ");
  if (ifile->IsZombie())
    PrintAndThrow("Cannot find file!");
  TTree* tree = (TTree*)ifile->Get("tree");
  MCProcessor evs(tree);
  for (int i = 0; i < evs.NEvents(); ++i) {
    if (verbose && i % 100000 == 0)
      std::cout << i << " / " << evs.NEvents() << std::endl;

    evs.Process(i);
    if (!evs.ValidGeneratedParticle())
      continue;
    int detseg = evs.DetectorSegment();
    if (evs.ValidLeftHit()) {
      arms[kLeft] .Hit(evs.EL(), evs.E0());
      arms[kRight].Miss(evs.E0());
      levs.push_back(MCEvent(detseg, evs.EL(), evs.dEL(), evs.E0(),
                             evs.X0(), evs.Y0(), evs.Z0(), evs.StopLayer()));
      revs.push_back(MCEvent(0, 0, 0, evs.E0(),
                             evs.X0(), evs.Y0(), evs.Z0(), evs.StopLayer()));
    } else if (evs.ValidRightHit()) {
      arms[kRight].Hit(evs.ER(), evs.E0());
      arms[kLeft] .Miss(evs.E0());
      levs.push_back(MCEvent(0, 0, 0, evs.E0(),
                             evs.X0(), evs.Y0(), evs.Z0(), evs.StopLayer()));
      revs.push_back(MCEvent(detseg, evs.ER(), evs.dER(), evs.E0(),
                             evs.X0(), evs.Y0(), evs.Z0(), evs.StopLayer()));
    } else {
      arms[kLeft] .Miss(evs.E0());
      arms[kRight].Miss(evs.E0());
      levs.push_back(MCEvent(0, 0, 0, evs.E0(),
                             evs.X0(), evs.Y0(), evs.Z0(), evs.StopLayer()));
      revs.push_back(MCEvent(0, 0, 0, evs.E0(),
                             evs.X0(), evs.Y0(), evs.Z0(), evs.StopLayer()));
    }
  }
  ifile->Close();
  delete ifile;
  return evs.GetParticle();
}

void SaveConfigTree(TFile* f, const Particle& p, const string& dataset) {
  TTree* tr = new TTree("config", "config");
  tr->SetDirectory(f);
  double thinressig, thickressig;
  string particle, dset;
  tr->Branch("thinressig",  &thinressig);
  tr->Branch("thickressig", &thickressig);
  tr->Branch("particle",    &particle);
  tr->Branch("dataset",     &dset);
  thinressig  = ERES[0];
  thickressig = ERES[1];
  particle    = p.Name;
  dset        = dataset;
  tr->Fill();
  tr->Write();
}

void TM(const char* ifname, const char* ofname, const string& dataset,
        bool verbose=false) {
  map<Side,Arm> arms;
  vector<MCEvent> levs, revs;
  arms[kLeft]  = Arm(kLeft);
  arms[kRight] = Arm(kRight);
  Particle p = TM(ifname, arms, levs, revs, verbose);
  TFile* ofile = new TFile(ofname, "RECREATE");
  arms[kLeft] .Write();
  arms[kRight].Write();
  MCEvent::SaveAsTree(ofile, "SiL_TransTree", levs);
  MCEvent::SaveAsTree(ofile, "SiR_TransTree", revs);
  SaveConfigTree(ofile, p, dataset);
  ofile->Close();
}


// The merging function
void R15b_Al50_TransferMatrix(const char* ifnamefmt, const char* ofname,
                              int n) {
  // Example
  // n          = 36;
  // ifnamefmt = "data/Al100/tm%d.2layer.root";
  // ofname    = "data/Al100/tm.2layer.root";
  TFileMerger ofile;
  for (int i = 1; i <= n; ++i) {
    char ifname[256];
    sprintf(ifname, ifnamefmt, i);
    ofile.AddFile(ifname);
  }
  ofile.OutputFile(ofname);
  ofile.Merge();
}

//// WRITE CODE TO GET PARTICLE FROM TRANSFER MATRIX

// The producing function
void R15b_Al50_TransferMatrix(const char* ifname=nullptr,
                              const char* ofname=nullptr,
                              const char* dataset=nullptr, bool verbose=false) {
  if (!ifname) return;
  string dset(dataset);
  if (!Sanity::IsValidDataset(dset))
    PrintAndThrow("ERROR: Unknown dataset: " + dset);
  TM(ifname, ofname, string(dataset), verbose);
}
