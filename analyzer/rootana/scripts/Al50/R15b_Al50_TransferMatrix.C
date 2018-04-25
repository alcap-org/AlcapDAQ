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
namespace {
  const char DATADIR[] = "~/data/R15b";
  const char IFNAMEPRE_P[] = "mc/R15bAl50ProtTarg";
  const char IFNAMEPRE_D[] = "mc/R15bAl50DeutTarg";
  const char IFNAMEPRE_T[] = "mc/R15bAl50TritTarg";
  const char IFNAMEPRE_A[] = "mc/R15bAl50AlphaTarg";
  const char OFNAMEPRE_P[] = "al50ptm";
  const char OFNAMEPRE_D[] = "al50dtm";
  const char OFNAMEPRE_T[] = "al50ttm";
  const char OFNAMEPRE_A[] = "al50atm";
}
////////////////////////////////////////////////////////////////////////////////

// Necessary since hadd doesn't know about RooUnfold objects
void Merge(const char* fnamepre, int n) {
  TFileMerger ofile;
  for (Long_t i = 1; i <= n; ++i) {
    TString ifname = TString(DATADIR) + '/' + fnamepre + '_' + i + ".root";
    ofile.AddFile(ifname);
  }
  TString ofname = TString(DATADIR) + '/' + fnamepre + ".root";
  ofile.OutputFile(ofname);
  ofile.Merge();
}

class Arm {
  Side arm;
  RooUnfoldResponse* response;
  TH2* diffVe;

public:
  Arm() {}
  Arm(Side s, const Particle& p) :
  arm(s), response(nullptr) {
    int    NE   = 100;
    double E[2] = {0., 20e3};
    char hname[128];
    sprintf(hname, "Si%c_TM", s == kLeft ? 'L' : 'R');
    response = new RooUnfoldResponse(NE, E[0], E[1], hname);
    sprintf(hname, "Si%c_DM", s == kLeft ? 'L' : 'R');
    diffVe = new TH2I(hname,
                      "Si Difference vs Energy;E_0 [keV];E_0-E_{meas} [keV]",
                      NE, E[0], E[1], NE, E[0], E[1]);
  }
  void Hit(double emeas, double etrue) {
    static TRandom3 smear;
    double e = smear.Gaus(emeas, 70.);
    response->Fill(e, etrue);
    diffVe  ->Fill(etrue, etrue-e);
  }
  void Miss(double etrue)              { response->Miss(etrue); }
  void Fake(double emeas)              { response->Fake(emeas); }
  void Write()                         { response->Write(); diffVe->Write(); }
  Side Position()                const { return arm; }
  TH2* Response()                const { return response->Hresponse(); }
};

class Hit {
 private:
  bool isproton, stopped;
  string vol;
  double edep/*keV*/, t/*ns*/;
 public:
  Hit() : isproton(false), stopped(false), vol(""), edep(0), t(0) {}
  Hit(bool isproton, bool stopped, string vol, double edep, double t) :
  isproton(isproton), stopped(stopped), vol(vol), edep(edep), t(t) {}
  bool IsThick() { return vol == "SiL3" || vol == "SiR2" || vol == "SiR3"; }
  bool IsThin()  { return vol == "SiL1" || vol == "SiR1"; }
  bool IsLeft()  { return vol == "SiL1" || vol == "SiL3"; }
  bool IsRight() { return vol == "SiR1" || vol == "SiR2" || vol == "SiR3"; }
  bool IsSiDet() { return IsLeft()      || IsRight(); }
  bool Stopped() { return stopped; }
  double EDep()  { return edep; }
  string Volume()          { return vol; }
  bool IsADetectedProton() { return isproton && IsSiDet(); }
  void Print() {
    std::cout << "P: " << isproton << ", stop: " << stopped << ", Vol: "
              << vol << ", edep: " << edep << ", t: " << t << std::endl;
  }
};

class MCEvent {
  double emeas, e0;
 public:
  MCEvent(double emeas, double e0) : emeas(emeas), e0(e0) {}
  static void SaveAsTree(TFile* f, const char* trname,
                         const vector<MCEvent>& evs) {
    TTree* tr = new TTree(trname, trname);
    tr->SetDirectory(f);
    MCEvent ev(0, 0);
    tr->Branch("e0", &ev.e0);
    tr->Branch("emeas", &ev.emeas);
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
  vector<string> *particles, *vols, *ovols;
  vector<double> *edeps, *ts, *es;
  vector<int> *stops, *trkIDs;
  Hit stophit;
  double hitEs[2][2]; // [Side][Thin/Thick]
 public:
  MCProcessor(TTree* tr, const Particle& p0) :
  tr(tr), p0(p0), particles(nullptr), vols(nullptr), ovols(nullptr),
  edeps(nullptr), ts(nullptr), es(nullptr), stops(nullptr), trkIDs(nullptr) {
    tr->SetBranchAddress("M_particleName", &particles);
    tr->SetBranchAddress("M_volName",      &vols);
    tr->SetBranchAddress("M_ovolName",     &ovols);
    tr->SetBranchAddress("M_stopped",      &stops);
    tr->SetBranchAddress("M_tid",          &trkIDs); // Starts at 1
    tr->SetBranchAddress("M_t",            &ts);     // don't know, ns?
    tr->SetBranchAddress("M_e",            &es);     // GeV
    tr->SetBranchAddress("M_edep",         &edeps);
  }
  int NEvents() { return tr->GetEntries(); }
  int NHits()   { return particles->size(); }
  double E0()   { return (es->at(0)-p0.M)*1e6; }
  double EL()   { return hitEs[0][0]+hitEs[0][1]; }
  double ER()   { return hitEs[1][0]+hitEs[1][1]; }
  bool PrimaryIsIncorrect() {
    return p0.Name != particles->at(0) || ovols->at(0) != "Target" ||
           trkIDs->at(0) != 1;
  }
  Hit GetHit(int i) {
    return Hit(particles->at(i) == "proton", stops->at(i), vols->at(i),
               edeps->at(i)*1e6, ts->at(i));
  }
  void Process(int i) {
    tr->GetEvent(i);
    stophit = Hit();
    hitEs[0][0] = hitEs[0][1] = hitEs[1][0] = hitEs[1][1] = 0.;
    if (PrimaryIsIncorrect())
      PrintAndThrow("Incorrect primary in simulation!");
    for (int i = 1; i < NHits(); ++i) {
      Hit hit = GetHit(i);
      if (!hit.IsADetectedProton())
        continue;
      int s = hit.IsLeft() ? 0 : 1;
      if      (hit.IsThin() && !hit.Stopped()) hitEs[s][0] += hit.EDep();
      else if (hit.IsThick())                  hitEs[s][1] += hit.EDep();
      if      (hit.Stopped())                  stophit      = hit;
    }
  }
  bool StoppedInLeft()  { return stophit.IsLeft()  && stophit.IsThick(); }
  bool StoppedInRight() { return stophit.IsRight() && stophit.IsThick(); }
  bool Insane()         { return E0() < 1.5e3; }
  void Print() {
    std::cout << "e0: " << E0() << ", (el, er): (" << EL() << ", " << ER() << "), stophit-->" << std::endl;
    stophit.Print();
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
    if (verbose && i % 10000 == 0)
      std::cout << i << " / " << evs.NEvents() << std::endl;

    evs.Process(i);
    if (evs.StoppedInLeft()) {
      arms[kLeft] .Hit(evs.EL(), evs.E0());
      levs.push_back(MCEvent(evs.EL(), evs.E0()));
      arms[kRight].Miss(evs.E0());
    } else if (evs.StoppedInRight()) {
      arms[kRight].Hit(evs.ER(), evs.E0());
      revs.push_back(MCEvent(evs.ER(), evs.E0()));
      arms[kLeft] .Miss(evs.E0());
    } else {
      arms[kLeft] .Miss(evs.E0());
      arms[kRight].Miss(evs.E0());
    }
  }
  ifile->Close();
  delete ifile;
}

void TM(const char* ofprefix, const char* ifprefix, const Particle& p, int n,
        bool verbose=false) {
  char ifname[256], ofname[256];
  sprintf(ifname, "%s/%s_%d.root", DATADIR, ifprefix, n);
  sprintf(ofname, "%s/%s_%d.root", DATADIR, ofprefix, n);

  map<Side,Arm> arms;
  vector<MCEvent> levs, revs;
  arms[kLeft]  = Arm(kLeft,  p);
  arms[kRight] = Arm(kRight, p);
  TM(ifname, arms, levs, revs, p, verbose);
  TFile* ofile = new TFile(ofname, "RECREATE");
  arms[kLeft] .Write();
  arms[kRight].Write();
  MCEvent::SaveAsTree(ofile, "SiL_TransTree", levs);
  MCEvent::SaveAsTree(ofile, "SiR_TransTree", revs);
  ofile->Close();
}

void R15b_Al50_TransferMatrix(int mode=0, int fnum=0, bool verbose=false) {
  switch(mode) {
    case 0: return;
    case 1:  TM(OFNAMEPRE_P, IFNAMEPRE_P, PROTON,   fnum, verbose);
             return;
    case 2:  TM(OFNAMEPRE_D, IFNAMEPRE_D, DEUTERON, fnum, verbose);
             return;
    case 3:  TM(OFNAMEPRE_T, IFNAMEPRE_T, TRITON,   fnum, verbose);
             return;
    case 4:  TM(OFNAMEPRE_A, IFNAMEPRE_A, ALPHA,    fnum, verbose);
             return;
    case -1: Merge(OFNAMEPRE_P, fnum);
             return;
    case -2: Merge(OFNAMEPRE_D, fnum);
             return;
    case -3: Merge(OFNAMEPRE_T, fnum);
             return;
    case -4: Merge(OFNAMEPRE_A, fnum);
             return;
  }
}
