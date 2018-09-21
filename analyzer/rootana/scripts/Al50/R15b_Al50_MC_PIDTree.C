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

static TRandom3 SMEAR;

struct PIDEvent {
  double e0, e1, e2, e3, t1, t2, t3;
  int seg;
  PIDEvent (double e0=0, double e1=0, double e2=0, double e3=0,
            double t1=0, double t2=0, double t3=0, int seg=-1) :
  e0(e0), e1(e1), e2(e2), e3(e3), t1(t1), t2(t2), t3(t3), seg(seg) {}
  bool Valid() const { return seg != -1; }
  static void SaveAsTree(TFile* f, const char* trname,
                         const vector<PIDEvent>& evs) {
    TTree* tr = new TTree(trname, trname);
    tr->SetDirectory(f);
    PIDEvent ev;
    tr->Branch("e0",  &ev.e0);
    tr->Branch("e1",  &ev.e1);
    tr->Branch("e2",  &ev.e2);
    tr->Branch("e3",  &ev.e3);
    tr->Branch("t1",  &ev.t1);
    tr->Branch("t2",  &ev.t2);
    tr->Branch("t3",  &ev.t3);
    tr->Branch("seg", &ev.seg);
    for (int i = 0; i < evs.size(); ++i) {
      ev = evs[i];
      tr->Fill();
    }
    tr->SetAlias("e",  "e1+e2+e3");
    tr->SetAlias("de", "e1");
    tr->SetAlias("t",  "t1");
    tr->SetAlias("dt", "t2-t1");
    tr->Write();
  }
};

class Hit {
 private:
  bool isparticle, stopped;
  int seg;
  double edep /*keV*/;
  string vol;
  double x, y, z, t;
 public:
  Hit() : isparticle(false), stopped(false), seg(-1),
  edep(0), vol(""), x(0), y(0), z(0), t(0) {}
  Hit(bool isparticle, bool stopped, int seg, double edep,
      string vol, double x, double y, double z, double t) :
  isparticle(isparticle), stopped(stopped),
  seg(seg), edep(edep), vol(vol), x(x), y(y), z(z), t(t) {}
  bool IsSi1()        const { return vol == "SiL1" || vol == "SiR1"; }
  bool IsSi2()        const { return vol == "SiL3" || vol == "SiR2"; }
  bool IsSi3()        const { return vol == "SiR3";                  }
  bool IsThick()      const { return IsSi2()       || IsSi3();       }
  bool IsThin()       const { return IsSi1();                        }
  bool IsLeft()       const { return vol == "SiL1" || vol == "SiL3"; }
  bool IsRight()      const { return vol == "SiR1" || vol == "SiR2" || vol == "SiR3"; }
  bool FiducialLeft() const {
    return (vol == "SiL1" && 2 <= seg && seg <= 15) || vol == "SiL3";
  }
  bool IsSiDet()                   const { return FiducialLeft() || IsRight();  }
  bool Stopped()                   const { return stopped;                      }
  double EDep()                    const { return edep;                         }
  string Volume()                  const { return vol;                          }
  double X()                       const { return x;                            }
  double Y()                       const { return y;                            }
  double Z()                       const { return z;                            }
  double T()                       const { return t;                            }
  bool IsADetectedParticle()       const { return isparticle && IsSiDet() &&
                                           SiUtils::OverThreshold(vol, seg, edep); }
  bool MatchedVolume(const Hit& h) const { return h.seg == seg && h.vol == vol; }
  int DetectorSegment()            const { return seg;                          }
  int Layer() {
    if      (IsSi1()) return 0;
    else if (IsSi2()) return 1;
    else if (IsSi3()) return 2;
    else              return -1;
  }
  bool Valid()                     const { return !vol.empty();                 }
  void Print() const {
    std::cout << "P: " << isparticle << ", stop: " << stopped << ", Vol: "
              << vol << "(" << seg << "), edep: " << edep << std::endl;
  }
};

class MCProcessor {
  TTree* tr;
  Particle p0;
  vector<string> *particles, *vols;
  vector<double> *edeps, *es, *xs, *ys, *zs, *ts;
  vector<int>    *stops, *segs;
  PIDEvent pid[2];
 public:
  MCProcessor(TTree* tr, const Particle& p0) :
  tr(tr), p0(p0), particles(nullptr), vols(nullptr), edeps(nullptr),
  es(nullptr), xs(nullptr), ys(nullptr), zs(nullptr), ts(nullptr),
  stops(nullptr), segs(nullptr), pid{PIDEvent(), PIDEvent()} {
    tr->SetBranchAddress("M_particleName", &particles);
    tr->SetBranchAddress("M_volName",      &vols);
    tr->SetBranchAddress("M_volID",        &segs);   // Segment (repetition #)
    tr->SetBranchAddress("M_stopped",      &stops);
    tr->SetBranchAddress("M_local_x",      &xs);     // cm
    tr->SetBranchAddress("M_local_y",      &ys);     // cm
    tr->SetBranchAddress("M_local_z",      &zs);     // cm
    tr->SetBranchAddress("M_t",            &ts);     // ns?
    tr->SetBranchAddress("M_ekin",         &es);     // GeV
    tr->SetBranchAddress("M_edep",         &edeps);  // GeV
  }
  PIDEvent GetLeftEvent()  const { return pid[0];                            }
  PIDEvent GetRightEvent() const { return pid[1];                            }
  double E0()              const { return es->at(0)*1e6;                     }
  int NEvents()            const { return tr->GetEntries();                  }
  int NHits()              const { return particles->size();                 }
  bool HasLeftHit()        const { return pid[0].Valid();                    }
  bool HasRightHit()       const { return pid[1].Valid();                    }
  bool Valid()             const { return HasRightHit() + HasLeftHit() == 1; }
  Hit GetHit(int i)        const {
    return Hit(particles->at(i) == p0.Name, stops->at(i), segs->at(i)+1,
               edeps->at(i)*1e6, vols->at(i), xs->at(i), ys->at(i),
               zs->at(i), ts->at(i));
  }
  void Process(int i) {
    tr->GetEvent(i);
    pid[0] = pid[1] = PIDEvent();
    int seg = 0;
    double Es[2][3] = {}, Ts[2][3] = {};

    for (int j = 1; j < NHits(); ++j) {
      Hit hit = GetHit(j);
      if (!hit.IsADetectedParticle())
        continue;
      int s = hit.IsLeft() ? 0 : 1;
      int l = hit.Layer();
      Es[s][l] += hit.EDep();
      Ts[s][l]  = hit.T();
      if (l == 0)
        seg = hit.DetectorSegment();
    }
    if (Es[0][0] + Es[0][1] + Es[0][2] > 0)
      pid[0] = PIDEvent(E0(), Es[0][0], Es[0][1], Es[0][2],
                        Ts[0][0], Ts[0][1], Ts[0][2], seg);
    if (Es[1][0] + Es[1][1] + Es[1][2] > 0)
      pid[1] = PIDEvent(E0(), Es[1][0], Es[1][1], Es[1][2],
                        Ts[1][0], Ts[1][1], Ts[1][2], seg);
  }
  bool ValidGeneratedParticle() {
    return !vols->empty() && vols->at(0) == "Target" &&
    particles->at(0) == p0.Name;
  }
};

void PID(const char* ifname, vector<PIDEvent>& levs, vector<PIDEvent>& revs,
         const Particle& p, bool verbose=false) {
  TFile* ifile = new TFile(ifname, "READ");
  if (ifile->IsZombie())
    PrintAndThrow("Cannot find file!");
  TTree* tree = (TTree*)ifile->Get("tree");
  MCProcessor evs(tree, p);
  for (int i = 0; i < evs.NEvents(); ++i) {
    if (verbose && i % 100000 == 0)
      std::cout << i << " / " << evs.NEvents() << std::endl;
    evs.Process(i);
    if (!evs.ValidGeneratedParticle() || !evs.Valid())
      continue;
    if (evs.HasLeftHit())
      levs.push_back(evs.GetLeftEvent());
    else if (evs.HasRightHit())
      revs.push_back(evs.GetRightEvent());
  }
  ifile->Close();
  delete ifile;
}

void PID(const char* ifname, const char* ofname, const Particle& p,
        bool verbose=false) {
  vector<PIDEvent> levs, revs;
  PID(ifname, levs, revs, p, verbose);
  TFile* ofile = new TFile(ofname, "RECREATE");
  PIDEvent::SaveAsTree(ofile, "PID_L", levs);
  PIDEvent::SaveAsTree(ofile, "PID_R", revs);
  ofile->Close();
}

void R15b_Al50_MC_PIDTree(char mode='\0', const char* ifname=nullptr,
                         const char* ofname=nullptr, bool verbose=false) {
  Particle p;
  switch(mode) {
    case 'p':  p = PROTON;   break;
    case 'd':  p = DEUTERON; break;
    case 't':  p = TRITON;   break;
    case 'a':  p = ALPHA;    break;
    default: return;
  }
  PID(ifname, ofname, p, verbose);
}
