#include "scripts/Al50/util.h"

#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TTree.h"

#include <stdio.h>

void draw_sit_si1_tcorr() {
  TFile* ifile = new TFile("data/Al50/psel.3layer.pidpow2.root");
  TTree* tr_l = (TTree*)ifile->Get("PID_LP");
  TTree* tr_r = (TTree*)ifile->Get("PID_RP");

  TH1* hl = new TH1D("hl", "Si1-SiT Time Coincidence;t [ns];Counts/10ns", 50, -250, 250);
  TH1* hr = new TH1D("hr", "Si1-SiT Time Coincidence;t [ns];Counts/10ns", 50, -250, 250);

  tr_l->Draw("t2>>+hl", "", "goff");
  tr_r->Draw("t2>>+hr", "", "goff");

  std::cout << hl << " " << hr << std::endl;

  TF1* fl = new TF1("fcnl", "gaus(0)", -150, 0);
  TF1* fr = new TF1("fcnr", "gaus(0)", -150, 0);
  fl->SetLineColor(kBlue);
  fr->SetLineColor(kRed);
  TFitResultPtr res_l = hl->Fit(fl, "RMS");
  TFitResultPtr res_r = hr->Fit(fr, "RMS");

  hl->SetLineColor(ACStyle::kACBlue);
  hr->SetLineColor(ACStyle::kACRed);


  TLegend* leg = new TLegend(0.1, 0.6, 0.4, 0.9);
  char legentry_l[32], legentry_r[32];
  sprintf(legentry_l, "SiL: #sigma=%.1fns", res_l->Parameter(2));
  sprintf(legentry_r, "SiR: #sigma=%.1fns", res_r->Parameter(2));
  leg->AddEntry(hl, legentry_l);
  leg->AddEntry(hr, legentry_r);

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c");
  hr->Draw();
  hl->Draw("SAME");
  leg->Draw();
  c->SaveAs("img/data_sit_si1_tcorr.png");
}
