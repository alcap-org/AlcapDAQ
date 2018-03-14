#include "./scripts/Al50/R15b_Al50_psel.C"

#include "TCanvas.h"
#include "TF1.h"
#include "TF2.h"
#include "TLegend.h"
#include "TLine.h"
#include "TMultiGraph.h"
#include "TStyle.h"

#include <iostream>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// USER DEFINED VARIABLE
// This script was grown organically, so any attempt to understand variable
// names is a fool's errand. Generaly
// l/r represent left/right (except when l means TLegend)
// p/d/t/a/u mean proton, deuteron, triton, alpha, and unclassified, except
//   when p means profile or particle
// f means function
// h/p/s/e means histogram, profile (except where it means proton), selected,
//   and energy
// dat/fcn means data (derived form data file) and function (derived from
//   simulation)
// l/c means legend (except where it means left), and canvas
// The first input file, PRAW, is the raw particle E vs dE data.
// The second input file, PSEL, is the particle selection information.
// That just means the simulated E vs dE plots, which are used to construct
// particle select functions.
static const char IFNAME_PRAW[] = "~/data/R15b/evdeal50.root";
static const char IFNAME_PSEL[] = "~/data/R15b/pselal50.root";
////////////////////////////////////////////////////////////////////////////////

vector<ParticleLikelihoodData> pls_l, pls_r;
// Proton, deuteron, triton, alpha; right, left
Double_t pfcn_l(Double_t* x, Double_t* par) { return pls_l[0](x[1], x[0]); }
Double_t pfcn_r(Double_t* x, Double_t* par) { return pls_r[0](x[1], x[0]); }
Double_t dfcn_l(Double_t* x, Double_t* par) { return pls_l[1](x[1], x[0]); }
Double_t dfcn_r(Double_t* x, Double_t* par) { return pls_r[1](x[1], x[0]); }
Double_t tfcn_l(Double_t* x, Double_t* par) { return pls_l[2](x[1], x[0]); }
Double_t tfcn_r(Double_t* x, Double_t* par) { return pls_r[2](x[1], x[0]); }
Double_t afcn_l(Double_t* x, Double_t* par) { return pls_l[3](x[1], x[0]); }
Double_t afcn_r(Double_t* x, Double_t* par) { return pls_r[3](x[1], x[0]); }

TF2 *fpl, *fpr, *fdl, *fdr, *ftl, *ftr, *fal, *far;
TF1 *tpl, *tpr, *tdl, *tdr, *ttl, *ttr, *tal, *tar;
void init_fcns() {
  static bool init = false;
  if (init) return;
  init = true;

  pls_l = LoadParticleLikelihoodsData('l');
  pls_r = LoadParticleLikelihoodsData('r');
  fpl = new TF2("fpl", pfcn_l, 0., 20.e3, 0., 10.e3);
  fpr = new TF2("fpr", pfcn_r, 0., 20.e3, 0., 10.e3);
  fdl = new TF2("fdl", dfcn_l, 0., 20.e3, 0., 10.e3);
  fdr = new TF2("fdr", dfcn_r, 0., 20.e3, 0., 10.e3);
  ftl = new TF2("ftl", tfcn_l, 0., 20.e3, 0., 10.e3);
  ftr = new TF2("ftr", tfcn_r, 0., 20.e3, 0., 10.e3);
  fal = new TF2("fal", afcn_l, 0., 20.e3, 0., 10.e3);
  far = new TF2("far", afcn_r, 0., 20.e3, 0., 10.e3);
  tpl = pls_l[0].GetFcn();
  tpr = pls_r[0].GetFcn();
  tdl = pls_l[1].GetFcn();
  tdr = pls_r[1].GetFcn();
  ttl = pls_l[2].GetFcn();
  ttr = pls_r[2].GetFcn();
  tal = pls_l[3].GetFcn();
  tar = pls_r[3].GetFcn();

  Double_t fcont[1] = { 1.e-4 };
  fpl->SetNpx(200); fpl->SetNpy(200); fpl->SetContour(1, fcont);
  fpr->SetNpx(200); fpr->SetNpy(200); fpr->SetContour(1, fcont);
  fdl->SetNpx(200); fdl->SetNpy(200); fdl->SetContour(1, fcont);
  fdr->SetNpx(200); fdr->SetNpy(200); fdr->SetContour(1, fcont);
  ftl->SetNpx(200); ftl->SetNpy(200); ftl->SetContour(1, fcont);
  ftr->SetNpx(200); ftr->SetNpy(200); ftr->SetContour(1, fcont);
  fal->SetNpx(200); fal->SetNpy(200); fal->SetContour(1, fcont);
  far->SetNpx(200); far->SetNpy(200); far->SetContour(1, fcont);
  fpl->SetLineColor(kRed);     tpl->SetLineColor(kRed);
  fpr->SetLineColor(kRed);     tpr->SetLineColor(kRed);
  fdl->SetLineColor(kGreen);   tdl->SetLineColor(kGreen);
  fdr->SetLineColor(kGreen);   tdr->SetLineColor(kGreen);
  ftl->SetLineColor(kBlue);    ttl->SetLineColor(kBlue);
  ftr->SetLineColor(kBlue);    ttr->SetLineColor(kBlue);
  fal->SetLineColor(kMagenta); tal->SetLineColor(kMagenta);
  far->SetLineColor(kMagenta); tar->SetLineColor(kMagenta);
  tpl->SetLineStyle(2);
  tpr->SetLineStyle(2);
  tdl->SetLineStyle(2);
  tdr->SetLineStyle(2);
  ttl->SetLineStyle(2);
  ttr->SetLineStyle(2);
  tal->SetLineStyle(2);
  tar->SetLineStyle(2);
}

TH2 *hl, *hr;
void init_hists() {
  static bool init = false;
  if (init) return;
  init = true;
  TH1::SetDefaultSumw2();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(111);
  init_fcns();

  hl = pls_l[0].GetHist();
  hr = pls_r[0].GetHist();

  hl->SetStats(0);
  hr->SetStats(0);
}

void r15b_al50_psel_data_draw() {
  init_hists();
  TLegend*  l1      = new TLegend(0.1, 0.7, 0.3, 0.9);
  TLegend*  l2      = new TLegend(0.7, 0.7, 0.9, 0.9);
  TLine* line_model = new TLine();
  TLine* line_class = new TLine();
  line_model->SetLineStyle(2);
  l1->AddEntry(line_model, "Prediction",   "l");
  l1->AddEntry(line_class, "Cut from fit", "l");
  l2->AddEntry(fpl,        "Proton",       "l");
  l2->AddEntry(fdl,        "Deuteron",     "l");
  l2->AddEntry(ftl,        "Triton",       "l");
  l2->AddEntry(fal,        "#alpha",       "l");

  TCanvas* cl = new TCanvas;
  hl->Draw("COL");
  tpl->Draw("SAME"); fpl->Draw("SAME CONT3");
  tdl->Draw("SAME"); fdl->Draw("SAME CONT3");
  ttl->Draw("SAME"); ftl->Draw("SAME CONT3");
  tal->Draw("SAME"); fal->Draw("SAME CONT3");
  l1->Draw();        l2->Draw();
  cl->SetLogz();
  cl->SaveAs("img/l_data_classification.png");
  TCanvas* cr = new TCanvas;
  hr->Draw("COL");
  tpr->Draw("SAME"); fpr->Draw("SAME CONT3");
  tdr->Draw("SAME"); fdr->Draw("SAME CONT3");
  ttr->Draw("SAME"); ftr->Draw("SAME CONT3");
  tar->Draw("SAME"); far->Draw("SAME CONT3");
  l1->Draw();        l2->Draw();
  cr->SetLogz();
  cr->SaveAs("img/r_data_classification.png");

  TLine* lim[2] = { new TLine(2.e3, 200., 19.e3, 200.),
                    new TLine(2.e3, 50.,  19.e3, 50.) };
  lim[0]->SetLineStyle(9); lim[1]->SetLineStyle(9);
  TCanvas* csigl = new TCanvas;
  TGraph* glp = pls_l[0].SigGraph();
  TGraph* gld = pls_l[1].SigGraph();
  TGraph* glt = pls_l[2].SigGraph();
  TGraph* gla = pls_l[3].SigGraph();
  TMultiGraph* mgl = new TMultiGraph();
  mgl->Add(glp); mgl->Add(gld); mgl->Add(glt); mgl->Add(gla);
  mgl->SetTitle("SiL PID #sigma");
  glp->SetLineColor(kRed);
  gld->SetLineColor(kGreen);
  glt->SetLineColor(kBlue);
  gla->SetLineColor(kMagenta);
  mgl->SetTitle("");
  mgl->Draw("A");
  lim[0]->Draw(); lim[1]->Draw();
  l2->Draw();
  csigl->SaveAs("img/l_data_class_sig.png");
  TCanvas* csigr = new TCanvas;
  TGraph* grp = pls_r[0].SigGraph();
  TGraph* grd = pls_r[1].SigGraph();
  TGraph* grt = pls_r[2].SigGraph();
  TGraph* gra = pls_r[3].SigGraph();
  TMultiGraph* mgr = new TMultiGraph();
  mgr->SetTitle("SiL PID #sigma");
  mgr->Add(grp); mgr->Add(grd); mgr->Add(grt); mgr->Add(gra);
  grp->SetLineColor(kRed);
  grd->SetLineColor(kGreen);
  grt->SetLineColor(kBlue);
  gra->SetLineColor(kMagenta);
  mgr->SetTitle("");
  mgr->Draw("A");
  lim[0]->Draw(); lim[1]->Draw();
  l2->Draw();
  csigr->SaveAs("img/r_data_class_sig.png");

  vector<TH1*>& v = pls_r[2].GetHists();
  vector<TF1*>& f = pls_r[2].GetFits();
  for (int i = 0; i < v.size(); ++i) {
    new TCanvas();
    v[i]->Draw();
    f[i]->Draw("SAME");
  }
}
