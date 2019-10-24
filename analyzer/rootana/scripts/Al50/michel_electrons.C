#include "./scripts/Al50/util.h"
#include "./scripts/TMETree/TMETreeBranches.h"
#include "./scripts/TMETree/TMEUtils.h"
#include "./src/plotters/SimplePulse.h"

#include "TChain.h"
#include "TFile.h"
#include "TH2.h"
#include "TH2D.h"
#include "TTree.h"

#include <iostream>

using std::vector;


class HistsHandler {
private:
  TH2* het[2][5];
  TH2* hedt[2][5];
  enum HitType {
    kAny, kSingle, kElectron, kProton, kPunchThroughProton
  };
  void BookEvTHists(TFile* f) {
    het[0][2] = het[0][4] = nullptr;
    het[0][0] = new TH2D("hletall",     "SiL3 Hits All;Time [ns];Energy [keV];Counts/10ns/100keV",        2000, -10e3, 10e3, 80, 0, 8e3);
    het[0][1] = new TH2D("hlet0si1",    "SiL3 Hits !SiL1;Time [ns];Energy [keV];Counts/10ns/100keV",      2000, -10e3, 10e3, 80, 0, 8e3);
    het[0][3] = new TH2D("hletsi1",     "SiL3 Hits SiL1;Time [ns];Energy [keV];Counts/10ns/100keV",       2000, -10e3, 10e3, 80, 0, 8e3);
    het[1][0] = new TH2D("hretall",     "SiR2 Hits All;Time [ns];Energy [keV];Counts/10ns/100keV",        2000, -10e3, 10e3, 80, 0, 8e3);
    het[1][1] = new TH2D("hret0si1",    "SiR2 Hits !SiR1;Time [ns];Energy [keV];Counts/10ns/100keV",      2000, -10e3, 10e3, 80, 0, 8e3);
    het[1][2] = new TH2D("hret0si1si3", "SiR2 Hits !SiR1+SiR3;Time [ns];Energy [keV];Counts/10ns/100keV", 2000, -10e3, 10e3, 80, 0, 8e3);
    het[1][3] = new TH2D("hretsi10si3", "SiR2 Hits SiR1+!SiR3;Time [ns];Energy [keV];Counts/10ns/100keV", 2000, -10e3, 10e3, 80, 0, 8e3);
    het[1][4] = new TH2D("hretsi1si3",  "SiR2 Hits SiR1+SiR3;Time [ns];Energy [keV];Counts/10ns/100keV",  2000, -10e3, 10e3, 80, 0, 8e3);
    het[0][0]->SetDirectory(f);
    het[0][1]->SetDirectory(f);
    het[0][3]->SetDirectory(f);
    het[1][0]->SetDirectory(f);
    het[1][1]->SetDirectory(f);
    het[1][2]->SetDirectory(f);
    het[1][3]->SetDirectory(f);
    het[1][4]->SetDirectory(f);
  }
  void BookEvdTHists(TFile* f) {
    hedt[0][0] = hedt[0][1] = hedt[0][2] = hedt[0][3] = hedt[0][4] = nullptr;
    hedt[1][1] = hedt[1][3] = nullptr;
    hedt[1][0] = new TH2D("hredtall",     "SiR2 Hits All;T_{3}-T_{2} [ns];Energy [keV];Counts/10ns/100keV",        2000, -10e3, 10e3, 80, 0, 8e3);
    hedt[1][2] = new TH2D("hredt0si1si3", "SiR2 Hits !SiR1+SiR3;T_{3}-T_{2} [ns];Energy [keV];Counts/10ns/100keV", 2000, -10e3, 10e3, 80, 0, 8e3);
    hedt[1][4] = new TH2D("hredtsi1si3",  "SiR2 Hits SiR1+SiR3;T_{3}-T_{2} [ns];Energy [keV];Counts/10ns/100keV",  2000, -10e3, 10e3, 80, 0, 8e3);
    hedt[1][0]->SetDirectory(f);
    hedt[1][2]->SetDirectory(f);
    hedt[1][4]->SetDirectory(f);
  }
public:
  HistsHandler(TFile* f) {
    BookEvTHists(f);
    BookEvdTHists(f);
  }
  typedef vector<SimplePulse> Pulses;
  void FillET(const Pulses* sps, TH2* h) {
    for (int i = 0; i < sps->size(); ++i)
      h->Fill(sps->at(i).tTME, sps->at(i).E);
  }
  void FillEdT(const Pulses* sps2, const Pulses* sps3, TH2* h) {
    h->Fill(sps3->at(0).tTME-sps2->at(0).tTME, sps2->at(0).E);
  }
  void FillLeftAllT           (const Pulses* sps) { FillET (sps, het [0][0]); }
  void FillLeftNoSi1T         (const Pulses* sps) { FillET (sps, het [0][1]); }
  void FillLeftStopProtonsT   (const Pulses* sps) { FillET (sps, het [0][3]); }
  void FillRightAllT          (const Pulses* sps) { FillET (sps, het [1][0]); }
  void FillRightNoSi1T        (const Pulses* sps) { FillET (sps, het [1][1]); }
  void FillRightMichelT       (const Pulses* sps) { FillET (sps, het [1][2]); }
  void FillRightStopProtonsT  (const Pulses* sps) { FillET (sps, het [1][3]); }
  void FillRightPunchProtonsT (const Pulses* sps) { FillET (sps, het [1][4]); }
  void FillRightAlldT         (const Pulses* sps2, const Pulses* sps3) {
    FillEdT(sps2, sps3, hedt[1][0]);
  }
  void FillRightMicheldT      (const Pulses* sps2, const Pulses* sps3) {
    FillEdT(sps2, sps3, hedt[1][2]);
  }
  void FillRightPunchProtonsdT(const Pulses* sps2, const Pulses* sps3) {
    FillEdT(sps2, sps3, hedt[1][4]);
  }
};

vector< vector<SimplePulse>* >& SiTs  = all_SiT_channels;
vector< vector<SimplePulse>* >& SiR1s = all_SiR1_channels;
vector< vector<SimplePulse>* >& SiL1s = all_SiL1_channels;
void michel_electrons(TTree* tr, const char* ofname, bool verbose=false) {
  TFile* ofile = new TFile(ofname, "RECREATE");
  HistsHandler handler(ofile);
  SetTMEBranchAddresses(tr);
  CollectChannels();
  for (int i = 0; i < tr->GetEntries(); ++i) {
    tr->GetEntry(i);
    if (verbose && i % 100000 == 0)
      std::cout << i << "/" << tr->GetEntries() << std::endl;
    if(!TMECuts::PileupProtected(10e3))
      continue;
    handler.FillLeftAllT(SiL3);
    handler.FillRightAllT(SiR2);
    if (TMECuts::OnlyOneHit(SiR2) && TMECuts::OnlyOneHit(SiR3))
      handler.FillRightAlldT(SiR2, SiR3);
    if (TMECuts::NoHits(SiL1s)) {
      handler.FillLeftNoSi1T(SiL3);
    } else {
      handler.FillLeftStopProtonsT(SiL3);
    }
    if (TMECuts::NoHits(SiR1s)) {
      handler.FillRightNoSi1T(SiR2);
      if (TMECuts::AtLeastOneHit(SiR3))
        handler.FillRightMichelT(SiR2);
      if (TMECuts::OnlyOneHit(SiR2) && TMECuts::OnlyOneHit(SiR3))
        handler.FillRightMicheldT(SiR2, SiR3);
    } else {
      if (SiR3->empty()) {
        handler.FillRightStopProtonsT(SiR2);
      } else if (TMECuts::OnlyOneHit(SiR2) && TMECuts::OnlyOneHit(SiR3)) {
        handler.FillRightPunchProtonsT(SiR2);
        handler.FillRightPunchProtonsdT(SiR2, SiR3);
      }
    }
  }
  ofile->Write();
  ofile->Close();
  delete ofile;
}

void michel_electrons(int run=0) {
  if (run == 0) return;
  char ifname[64], ofname[64];
  std::sprintf(ifname, "~/R15bTME/Al50/tme%05d.root",    run);
  std::sprintf(ofname, "out/michel_electrons_%05d.root", run);
  TChain* ch = new TChain("TMETree/TMETree");
  ch->Add(ifname);
  if (ch->GetEntries()==0) return;
  michel_electrons(ch, ofname, true);
  delete ch;
}
