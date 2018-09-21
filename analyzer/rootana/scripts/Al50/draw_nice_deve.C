#include "TAxis.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TLatex.h"
#include "TLine.h"
#include "TMultiGraph.h"
#include "TText.h"
#include "TTree.h"

#include <iostream>

static TTree *PID_LP, *PID_LD, *PID_LT, *PID_LA, *PID_LU;
static TTree *PID_RP, *PID_RD, *PID_RT, *PID_RA, *PID_RU;

void init(TFile* f) {
  PID_LP = (TTree*)f->Get("PID_LP");
  PID_LD = (TTree*)f->Get("PID_LD");
  PID_LT = (TTree*)f->Get("PID_LT");
  PID_LA = (TTree*)f->Get("PID_LA");
  PID_LU = (TTree*)f->Get("PID_LU");
  PID_RP = (TTree*)f->Get("PID_RP");
  PID_RD = (TTree*)f->Get("PID_RD");
  PID_RT = (TTree*)f->Get("PID_RT");
  PID_RA = (TTree*)f->Get("PID_RA");
  PID_RU = (TTree*)f->Get("PID_RU");
}

void draw_labels() {
  TLatex* mu = new TLatex(1000, 1200, "#mu");
  TText*   p = new TText (1600, 2000, "p");
  TText*   d = new TText (2300, 2500, "d");
  TText*   t = new TText (2900, 3000, "t");
  mu->Draw();
  p ->Draw();
  d ->Draw();
  t ->Draw();
}

void draw_cut() {
  const double e[2] = { 2e3, 17e3 };
  // Bethe formula approximately:
  // E*dE/ln(E*...) = Constant
  // Bethe formula log term:
  // ln(4*m_e*x/m_p/I); I = 0.173 keV, m_e/m_p = 544.61702e-6; x is energy in keV
  TF1*   hi    = new TF1("upbound",  "1.3e6/x*log(0.01259230107*x)", e[0], e[1]); // Right
  TF1*   lo    = new TF1("lowbound", "0.4e6/x*log(0.3*x)", e[0], e[1]); // Right
  // TF1*   hi    = new TF1("upbound",  "5.5e6/x", e[0], e[1]); // Left
  // TF1*   lo    = new TF1("lowbound", "3.0e6/x", e[0], e[1]); // Left
  TLine* left  = new TLine(e[0], lo->Eval(e[0]), e[0], hi->Eval(e[0]));
  TLine* right = new TLine(e[1], lo->Eval(e[1]), e[1], hi->Eval(e[1]));
  hi   ->SetLineColor(kRed);
  lo   ->SetLineColor(kRed);
  left ->SetLineColor(kRed);
  right->SetLineColor(kRed);
  hi   ->Draw("SAME");
  lo   ->Draw("SAME");
  left ->Draw("SAME");
  right->Draw("SAME");
}

void draw_nice_deve() {
  TFile* f = new TFile("data/Al50/psel.3layer.root");
  // TFile* f = new TFile("data/Al100/pselal100.root");
  init(f);

  TMultiGraph* mg[2] = { new TMultiGraph(), new TMultiGraph() };

  int n = PID_RP->Draw("de:e", "t > 400 && abs(dt) < 200", "goff");
  TGraph* gr_rp = new TGraph(n, PID_RP->GetV2(), PID_RP->GetV1());
  n = PID_RD->Draw("de:e", "t > 400 && abs(dt) < 200", "goff");
  TGraph* gr_rd = new TGraph(n, PID_RD->GetV2(), PID_RD->GetV1());
  n = PID_RT->Draw("de:e", "t > 400 && abs(dt) < 200", "goff");
  TGraph* gr_rt = new TGraph(n, PID_RT->GetV2(), PID_RT->GetV1());
  n = PID_RA->Draw("de:e", "t > 400 && abs(dt) < 200", "goff");
  TGraph* gr_ra = new TGraph(n, PID_RA->GetV2(), PID_RA->GetV1());
  n = PID_RU->Draw("de:e", "t > 400 && abs(dt) < 200", "goff");
  TGraph* gr_ru = new TGraph(n, PID_RU->GetV2(), PID_RU->GetV1());
  n = PID_LP->Draw("de:e", "t > 400 && abs(dt) < 200", "goff");
  TGraph* gr_lp = new TGraph(n, PID_LP->GetV2(), PID_LP->GetV1());
  n = PID_LD->Draw("de:e", "t > 400 && abs(dt) < 200", "goff");
  TGraph* gr_ld = new TGraph(n, PID_LD->GetV2(), PID_LD->GetV1());
  n = PID_LT->Draw("de:e", "t > 400 && abs(dt) < 200", "goff");
  TGraph* gr_lt = new TGraph(n, PID_LT->GetV2(), PID_LT->GetV1());
  n = PID_LA->Draw("de:e", "t > 400 && abs(dt) < 200", "goff");
  TGraph* gr_la = new TGraph(n, PID_LA->GetV2(), PID_LA->GetV1());
  n = PID_LU->Draw("de:e", "t > 400 && abs(dt) < 200", "goff");
  TGraph* gr_lu = new TGraph(n, PID_LU->GetV2(), PID_LU->GetV1());

  mg[0]->Add(gr_lp);
  mg[0]->Add(gr_ld);
  mg[0]->Add(gr_lt);
  mg[0]->Add(gr_la);
  mg[0]->Add(gr_lu);
  mg[1]->Add(gr_rp);
  mg[1]->Add(gr_rd);
  mg[1]->Add(gr_rt);
  mg[1]->Add(gr_ra);
  mg[1]->Add(gr_ru);

  gr_rp->SetMarkerColor(kGreen);
  gr_lp->SetMarkerColor(kGreen);
  // mg[1]->SetTitle("Al 50#mum, Right Arm;E (keV);#Delta E (keV)");
  // mg[1]->Draw("AP");
  // mg[1]->GetXaxis()->SetRangeUser(0, 15e3);
  // mg[1]->GetYaxis()->SetRangeUser(0, 6e3);
  // mg[1]->GetYaxis()->SetTitleOffset(1.2);
  // mg[1]->Draw("AP");
  mg[0]->SetTitle("Al 50#mum, Left Arm;E (keV);#Delta E (keV)");
  mg[0]->Draw("AP");
  mg[0]->GetXaxis()->SetRangeUser(0, 15e3);
  mg[0]->GetYaxis()->SetRangeUser(0, 6e3);
  mg[0]->GetYaxis()->SetTitleOffset(1.2);
  mg[0]->Draw("AP");
  // draw_labels();
  // draw_cut();
}
