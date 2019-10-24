#include "scripts/Al50/util.h"

#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <string>
#include <vector>
using std::string;
using std::vector;

struct PIDEvent {
  double e0, e1, e2, e3, t1, t2, t3;
  int stop;
  PIDEvent (double e0=0, double e1=0, double e2=0, double e3=0,
            double t1=0, double t2=0, double t3=0, int stop=0) :
  e0(e0), e1(e1), e2(e2), e3(e3), t1(t1), t2(t2), t3(t3), stop(stop) {}
};

class EventTreeBuilder {
  string particle, dets[3];
  vector<PIDEvent> evs;

public:
  EventTreeBuilder(const string& part, const string& det1, const string& det2,
      const string& det3) :
  particle(part), dets{det1, det2, det3}, evs() {}

  bool ValidThinMatch(const string& d, const string& p) {
    return d == dets[0] && p == particle;
  }
  bool ValidThickMatch(const string& d, const string& p) {
    return d == dets[1] && p == particle;
  }
  bool ValidVetoMatch(const string& d, const string& p) {
    return d == dets[2] && p == particle;
  }
  void Fill(double e0, double E[3], double T[3], int stop) {
    evs.push_back(PIDEvent(e0, E[0], E[1], E[2], T[0], T[1], T[2], stop));
  }
  void BuildAndSaveTree(TFile* f, const char* trname) {
    TDirectory* cwd = gDirectory;
    f->cd();
    TTree* tr = new TTree(trname, "");
    PIDEvent ev;
    tr->Branch("e0",   &ev.e0);
    tr->Branch("e1",   &ev.e1);
    tr->Branch("e2",   &ev.e2);
    tr->Branch("e3",   &ev.e3);
    tr->Branch("t1",   &ev.t1);
    tr->Branch("t2",   &ev.t2);
    tr->Branch("t3",   &ev.t3);
    tr->Branch("stop", &ev.stop);
    for (int i = 0; i < evs.size(); ++i) {
      ev = evs[i];
      tr->Fill();
    }
    tr->SetAlias("e",  "e1+e2+e3");
    tr->SetAlias("de", "e1");
    tr->SetAlias("t",  "t1");
    tr->SetAlias("dt", "t2-t1");
    tr->Write();
    cwd->cd();
  }
};

void EvdE(const char* ifname, const char* ofname, const Particle& parttype,
          bool verbose=false) {
  TFile* ifile = new TFile(ifname, "READ");
  TTree* tree  = (TTree*)ifile->Get("tree");
  TFile* ofile = new TFile(ofname, "RECREATE");

  vector<int>*    tids      = nullptr;
  vector<string>* particles = nullptr;
  vector<string>* vols      = nullptr;
  vector<double>* es        = nullptr;
  vector<double>* edeps     = nullptr;
  vector<int>*    stops     = nullptr;
  vector<double>* ts        = nullptr;

  tree->SetBranchAddress("M_tid",          &tids);
  tree->SetBranchAddress("M_particleName", &particles);
  tree->SetBranchAddress("M_volName",      &vols);
  tree->SetBranchAddress("M_e",            &es);
  tree->SetBranchAddress("M_edep",         &edeps);
  tree->SetBranchAddress("M_stopped",      &stops);
  tree->SetBranchAddress("M_Ot",           &ts);

  const int n_arms = 2;
  EventTreeBuilder LeftArm (parttype.Name, "SiL1", "SiL3", "SiL2");
  EventTreeBuilder RightArm(parttype.Name, "SiR1", "SiR2", "SiR3");
  EventTreeBuilder arms[2] = {LeftArm, RightArm};

  for (int i_entry = 0; i_entry < tree->GetEntries(); ++i_entry) {
     tree->GetEvent(i_entry);

    if (verbose && i_entry % 10000 == 0)
      std::cout << i_entry << " / " << tree->GetEntries() << std::endl;

    double E[2][3] = {}, T[2][3] = {};
    int    stop[2] = {};
    for (int iElement = 0; iElement < particles->size(); ++iElement) {
      const string& particle = particles->at(iElement);
      const string& vol      = vols     ->at(iElement);
      const double  t        = ts       ->at(iElement);     // ns
      const double  edep     = edeps    ->at(iElement)*1e6; // keV
      // Loop through the arms
      for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
        if        (arms[i_arm].ValidThinMatch(vol, particle)) {
          E[i_arm][0] += edep;
          T[i_arm][0] = t;
          if (stops->at(iElement) == true)
            stop[i_arm] = 1;
        } else if (arms[i_arm].ValidThickMatch(vol, particle)) {
          E[i_arm][1] += edep;
          T[i_arm][1] =  t;
          if (stops->at(iElement) == true)
            stop[i_arm] = 2;
        } else if (arms[i_arm].ValidVetoMatch(vol, particle)) {
          E[i_arm][2] += edep;
          T[i_arm][2] =  t;
          if (stops->at(iElement) == true)
            stop[i_arm] = 3;
        }
      }
    }
    for (int i_arm = 0; i_arm < 2; ++i_arm)
      if (E[i_arm][0]+E[i_arm][1]+E[i_arm][2] > 0.)
        arms[i_arm].Fill((es->at(0)-parttype.M)*1e6, E[i_arm], T[i_arm],
                         stop[i_arm]);
  }

  arms[0].BuildAndSaveTree(ofile, "PID_L");
  arms[1].BuildAndSaveTree(ofile, "PID_R");
  ofile->Close();
  ifile->Close();
}

void R15b_Al50_MC_EvdE(char mode='\0', const char* ifname=nullptr,
                       const char* ofname=nullptr, bool verbose=false) {
  Particle p;
  switch (mode) {
    case '\0':               return;
    case 'p': p = PROTON;   break;
    case 'd': p = DEUTERON; break;
    case 't': p = TRITON;   break;
    case 'a': p = ALPHA;    break;
    case 'u': p = MUON;     break;
    case 'e': p = ELECTRON; break;
    case 'g': p = PHOTON;   break;
  }
  EvdE(ifname, ofname, p, verbose);
}
