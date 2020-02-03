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

void draw_cut(char lr) {
  const double e[2] = { 1.9e3, 17e3 };
  // Bethe formula approximately:
  // E*dE/ln(E*...) = Constant
  // Bethe formula log term:
  // ln(4*m_e*x/m_p/I); I = 0.173 keV, m_e/m_p = 544.61702e-6; x is energy in keV
  // TF1*   hi    = new TF1("upbound",  "22e5/x^0.9", e[0], e[1]); // Right
  // TF1*   lo    = new TF1("lowbound", "12e5/x^0.9", e[0], e[1]); // Right
  static TF1* hi[2] = {
    new TF1("", "(x<=[0])*[1]*x^[2]+(x>[0])*[1]*[0]^([2]-[3])*x^[3]", e[0], e[1]),
    new TF1("", "(x<=[0])*[1]*x^[2]+(x>[0])*[1]*[0]^([2]-[3])*x^[3]", e[0], e[1])
  };
  static TF1* lo[2] = {
    new TF1("", "(x<=[0])*[1]*x^[2]+(x>[0])*[1]*[0]^([2]-[3])*x^[3]", e[0], e[1]),
    new TF1("", "(x<=[0])*[1]*x^[2]+(x>[0])*[1]*[0]^([2]-[3])*x^[3]", e[0], e[1])
  };
  hi[0]->SetParameters(4e3, 44e5, -1.00, -0.78);
  lo[0]->SetParameters(4e3, 35e5, -1.02, -0.85);
  hi[1]->SetParameters(4e3, 11e6, -1.10, -0.85);
  lo[1]->SetParameters(4e3, 60e5, -1.09, -0.95);
  // alo,   ahi,   klo1, klo2,  khi1,  khi2, emin, c, emax
  // {35e5, 44e5, -1.02, -0.85, -1.00, -0.78} // Left
  // {60e5, 11e6, -1.09, -0.95, -1.10, -0.85} // Right
  // TF1*   hi    = new TF1("upbound",  "5.5e6/x", e[0], e[1]); // Left
  // TF1*   lo    = new TF1("lowbound", "3.0e6/x", e[0], e[1]); // Left
  TLine* left[2]  = {
    new TLine(e[0], lo[0]->Eval(e[0]), e[0], hi[0]->Eval(e[0])),
    new TLine(e[0], lo[1]->Eval(e[0]), e[0], hi[1]->Eval(e[0])) };
  TLine* right[2] = {
    new TLine(e[1], lo[0]->Eval(e[1]), e[1], hi[0]->Eval(e[1])),
    new TLine(e[1], lo[1]->Eval(e[1]), e[1], hi[1]->Eval(e[1])) };
  hi[0]   ->SetLineColor(kRed);
  lo[0]   ->SetLineColor(kRed);
  hi[1]   ->SetLineColor(kRed);
  lo[1]   ->SetLineColor(kRed);
  left[0] ->SetLineColor(kRed);
  right[0]->SetLineColor(kRed);
  left[1] ->SetLineColor(kRed);
  right[1]->SetLineColor(kRed);
  if (lr == 'l') {
    hi[0]   ->Draw("SAME");
    lo[0]   ->Draw("SAME");
    left[0] ->Draw("SAME");
    right[0]->Draw("SAME");
  } else if (lr == 'r') {
    hi[1]   ->Draw("SAME");
    lo[1]   ->Draw("SAME");
    left[1] ->Draw("SAME");
    right[1]->Draw("SAME");
  } else {
    throw "Not L/R";
  }
}

void draw_nice_deve() {
  TFile* f = new TFile("data/Al50/psel.3layer.pidpow2.root");
  init(f);

  int n = PID_RP->Draw("de:e", "abs(dt) < 200", "goff");
  TGraph* gr_rp = new TGraph(n, PID_RP->GetV2(), PID_RP->GetV1());
  n = PID_RD->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_rd = new TGraph(n, PID_RD->GetV2(), PID_RD->GetV1());
  n = PID_RT->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_rt = new TGraph(n, PID_RT->GetV2(), PID_RT->GetV1());
  n = PID_RA->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_ra = new TGraph(n, PID_RA->GetV2(), PID_RA->GetV1());
  n = PID_RU->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_ru = new TGraph(n, PID_RU->GetV2(), PID_RU->GetV1());
  n = PID_LP->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_lp = new TGraph(n, PID_LP->GetV2(), PID_LP->GetV1());
  n = PID_LD->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_ld = new TGraph(n, PID_LD->GetV2(), PID_LD->GetV1());
  n = PID_LT->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_lt = new TGraph(n, PID_LT->GetV2(), PID_LT->GetV1());
  n = PID_LA->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_la = new TGraph(n, PID_LA->GetV2(), PID_LA->GetV1());
  n = PID_LU->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_lu = new TGraph(n, PID_LU->GetV2(), PID_LU->GetV1());


  n = PID_RP->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_rp_tcut = new TGraph(n, PID_RP->GetV2(), PID_RP->GetV1());
  n = PID_RD->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_rd_tcut = new TGraph(n, PID_RD->GetV2(), PID_RD->GetV1());
  n = PID_RT->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_rt_tcut = new TGraph(n, PID_RT->GetV2(), PID_RT->GetV1());
  n = PID_RA->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_ra_tcut = new TGraph(n, PID_RA->GetV2(), PID_RA->GetV1());
  n = PID_RU->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_ru_tcut = new TGraph(n, PID_RU->GetV2(), PID_RU->GetV1());
  n = PID_LP->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_lp_tcut = new TGraph(n, PID_LP->GetV2(), PID_LP->GetV1());
  n = PID_LD->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_ld_tcut = new TGraph(n, PID_LD->GetV2(), PID_LD->GetV1());
  n = PID_LT->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_lt_tcut = new TGraph(n, PID_LT->GetV2(), PID_LT->GetV1());
  n = PID_LA->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_la_tcut = new TGraph(n, PID_LA->GetV2(), PID_LA->GetV1());
  n = PID_LU->Draw("de:e", "t > 400 && abs(dt) < 200 && e3==0", "goff");
  TGraph* gr_lu_tcut = new TGraph(n, PID_LU->GetV2(), PID_LU->GetV1());

  TMultiGraph* mg[2] = { new TMultiGraph(), new TMultiGraph() };
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
  TMultiGraph* mg_tcut[2] = { new TMultiGraph(), new TMultiGraph() };
  mg_tcut[0]->Add(gr_lp_tcut);
  mg_tcut[0]->Add(gr_ld_tcut);
  mg_tcut[0]->Add(gr_lt_tcut);
  mg_tcut[0]->Add(gr_la_tcut);
  mg_tcut[0]->Add(gr_lu_tcut);
  mg_tcut[1]->Add(gr_rp_tcut);
  mg_tcut[1]->Add(gr_rd_tcut);
  mg_tcut[1]->Add(gr_rt_tcut);
  mg_tcut[1]->Add(gr_ra_tcut);
  mg_tcut[1]->Add(gr_ru_tcut);

  TCanvas* c = new TCanvas;
  gr_rp->SetMarkerColor(kGreen);
  gr_lp->SetMarkerColor(kGreen);
  mg[1]->SetTitle("Al 50#mum, Right Arm;E (keV);#Delta E (keV)");
  mg[1]->Draw("AP");
  mg[1]->GetXaxis()->SetRangeUser(0, 15e3);
  mg[1]->GetYaxis()->SetRangeUser(0, 6e3);
  mg[1]->GetYaxis()->SetTitleOffset(1.2);
  mg[1]->Draw("AP");
  draw_labels();
  c->SaveAs("img/data_deve_right.png");
  c->Clear();
  // mg[0]->SetTitle("Al 50#mum, Left Arm;E (keV);#Delta E (keV)");
  // mg[0]->Draw("AP");
  // mg[0]->GetXaxis()->SetRangeUser(0, 15e3);
  // mg[0]->GetYaxis()->SetRangeUser(0, 6e3);
  // mg[0]->GetYaxis()->SetTitleOffset(1.2);
  // mg[0]->Draw("AP");
  // c->SaveAs("img/deve_left.png");

  TLatex* txt_mu = new TLatex(1e3,   1.2e3, "#mu");
  TLatex* txt_p  = new TLatex(1.5e3, 2.0e3, "p");
  TLatex* txt_d  = new TLatex(2.3e3, 2.5e3, "d");
  TLatex* txt_t  = new TLatex(2.9e3, 2.9e3, "t");

  TCanvas* ctcut = new TCanvas("ctcut", "ctcut", 1400, 500);
  ctcut->Divide(2);
  ctcut->cd(1);
  gr_rp_tcut->SetMarkerColor(kGreen);
  gr_lp_tcut->SetMarkerColor(kGreen);
  mg_tcut[0]->SetTitle("Al 50#mum, Left Arm;E (keV);#Delta E (keV)");
  mg_tcut[0]->Draw("AP");
  mg_tcut[0]->GetXaxis()->SetRangeUser(0, 15e3);
  mg_tcut[0]->GetYaxis()->SetRangeUser(0, 6e3);
  mg_tcut[0]->GetYaxis()->SetTitleOffset(1.2);
  mg_tcut[0]->Draw("AP");
  // draw_labels();
  draw_cut('l');
  ctcut->cd(2);
  gr_rp_tcut->SetMarkerColor(kGreen);
  gr_lp_tcut->SetMarkerColor(kGreen);
  mg_tcut[1]->SetTitle("R15b Data on 50#mum Aluminum Target;E (keV);#Delta E (keV)");
  mg_tcut[1]->Draw("AP");
  mg_tcut[1]->GetXaxis()->SetRangeUser(0, 15e3);
  mg_tcut[1]->GetYaxis()->SetRangeUser(0, 6e3);
  mg_tcut[1]->GetYaxis()->SetTitleOffset(1.2);
  mg_tcut[1]->Draw("AP");
  draw_cut('r');
  txt_mu->Draw();
  txt_p ->Draw();
  txt_d ->Draw();
  txt_t ->Draw();
  ctcut->SaveAs("img/data_deve_tcut.png");

  // TCanvas* c = new TCanvas("")
  // gr_rp->SetMarkerColor(kGreen);
  // gr_lp->SetMarkerColor(kGreen);
  // mg[1]->SetTitle("Al 50#mum, Right Arm, Proton Cut;E (keV);#Delta E (keV)");
  // mg[1]->Draw("AP");
  // draw_labels();
  // draw_cut('r');
  // c->SaveAs("img/data_deve_pid_cut_right.png");
}
