#include "./scripts/Al50/psel.h"

#include "TCanvas.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH2.h"
#include "TH2D.h"

#include <cmath>
#include <vector>
using std::abs;
using std::vector;

void deve_psel() {
  TChain* trp[2] = { new TChain(), new TChain() };
  TChain* tr [2] = { new TChain(), new TChain() };
  tr [0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LD");
  tr [0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LT");
  tr [0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LA");
  tr [0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LU");
  tr [1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RD");
  tr [1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RT");
  tr [1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RA");
  tr [1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RU");
  trp[0]->Add("data/Al50/psel.3layer.pidpow2.root/PID_LP");
  trp[1]->Add("data/Al50/psel.3layer.pidpow2.root/PID_RP");

  TGraph* gr[2] = {
    new TGraph(tr[0]->GetEntries()),
    new TGraph(tr[1]->GetEntries())
  };
  TGraph* grp[2] = {
    new TGraph(trp[0]->GetEntries()),
    new TGraph(trp[1]->GetEntries())
  };

  for (int i = 0; i < 2; ++i) {
    double e1, e2, e3, t1, t2;
    tr[i]->SetBranchAddress("e1", &e1);
    tr[i]->SetBranchAddress("e2", &e2);
    tr[i]->SetBranchAddress("e3", &e3);
    tr[i]->SetBranchAddress("t1", &t1);
    tr[i]->SetBranchAddress("t2", &t2);
    for (int j = 0; j < tr[i]->GetEntries(); ++j) {
      tr[i]->GetEvent(j);
      if (400<t2 && abs(t2-t1)<200 && e3==0. && e1<6e3 && e1+e2<15e3)
        gr[i]->SetPoint(j, e1+e2, e1);
    }
    tr[i]->ResetBranchAddresses();
    trp[i]->SetBranchAddress("e1", &e1);
    trp[i]->SetBranchAddress("e2", &e2);
    trp[i]->SetBranchAddress("e3", &e3);
    trp[i]->SetBranchAddress("t1", &t1);
    trp[i]->SetBranchAddress("t2", &t2);
    for (int j = 0; j < trp[i]->GetEntries(); ++j) {
      trp[i]->GetEvent(j);
      if (400<t2 && abs(t2-t1)<200 && e3==0. && e1<6e3 && e1+e2<15e3)
        grp[i]->SetPoint(j, e1+e2, e1);
    }
    trp[i]->ResetBranchAddresses();

    gr[i]->SetMarkerColor(kBlack);
    grp[i]->SetMarkerColor(kGreen);

    // gr[i]->GetYaxis()->SetRangeUser(0, 15e3);
    // gr[i]->GetYaxis()->SetRangeUser(0, 3e3);
  }

  gr[0]->SetTitle("Al 50#mum, Left Arm;E (keV);dE (keV)");
  gr[1]->SetTitle("Al 50#mum, Right Arm;E (keV);dE (keV)");

  vector<ParticleLikelihood::PSelPow2> psel[2] = {
    ParticleLikelihood::LoadParticleLikelihoodsPow2('l'),
    ParticleLikelihood::LoadParticleLikelihoodsPow2('r')
  };
  TCanvas* cl = new TCanvas("cr", "cr", 700, 500);
  gr [1]->Draw("AP");
  grp[1]->Draw("P SAME");
  psel[1][0].Draw(kRed);
  cl->SaveAs("img/thesis/5/data_deve_pid_cut_right.png");
}
