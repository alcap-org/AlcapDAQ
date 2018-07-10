#include "./scripts/Al50/R15b_Al50_psel.C"

#include "Rtypes.h"
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

vector<ParticleLikelihood::PSelData> pls[2] = {
  ParticleLikelihood::LoadParticleLikelihoodsData('l'),
  ParticleLikelihood::LoadParticleLikelihoodsData('r')
};
// Proton, deuteron, triton, alpha; right, left
Double_t pfcn_l(Double_t* x, Double_t* par) { return pls[0][0](x[1], x[0]); }
Double_t pfcn_r(Double_t* x, Double_t* par) { return pls[1][0](x[1], x[0]); }
Double_t dfcn_l(Double_t* x, Double_t* par) { return pls[0][1](x[1], x[0]); }
Double_t dfcn_r(Double_t* x, Double_t* par) { return pls[1][1](x[1], x[0]); }
Double_t tfcn_l(Double_t* x, Double_t* par) { return pls[0][2](x[1], x[0]); }
Double_t tfcn_r(Double_t* x, Double_t* par) { return pls[1][2](x[1], x[0]); }
Double_t afcn_l(Double_t* x, Double_t* par) { return pls[0][3](x[1], x[0]); }
Double_t afcn_r(Double_t* x, Double_t* par) { return pls[1][3](x[1], x[0]); }

// Below indexed [Left,Right][Prot,deut,trit,alpha]
TF2* f[2][4]; // Contour functions
TF1* t[2][4]; // Apprimate Bethe-Bloche fit for each particle
void init_fcns() {
  static bool init = false;
  if (init) return;
  init = true;

  f[0][0] = new TF2("flp", pfcn_l, 0., 20.e3, 0., 10.e3);
  f[1][0] = new TF2("frp", pfcn_r, 0., 20.e3, 0., 10.e3);
  f[0][1] = new TF2("fld", dfcn_l, 0., 20.e3, 0., 10.e3);
  f[1][1] = new TF2("frd", dfcn_r, 0., 20.e3, 0., 10.e3);
  f[0][2] = new TF2("flt", tfcn_l, 0., 20.e3, 0., 10.e3);
  f[1][2] = new TF2("frt", tfcn_r, 0., 20.e3, 0., 10.e3);
  f[0][3] = new TF2("fla", afcn_l, 0., 20.e3, 0., 10.e3);
  f[1][3] = new TF2("fra", afcn_r, 0., 20.e3, 0., 10.e3);

  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      t[i][j] = pls[i][j].GetFcn();

      Double_t fcont[1] = { 1.e-4 };
      Int_t    cols[4]  = { kRed, kGreen, kBlue, kMagenta };
      f[i][j]->SetNpx(200);
      f[i][j]->SetNpy(200);
      f[i][j]->SetContour(1, fcont);
      f[i][j]->SetLineColor(cols[j]);
      t[i][j]->SetLineColor(cols[j]);
      t[i][j]->SetLineStyle(2);
    }
  }
}

TH2* h[2]; // Indexed left/right
void init_hists() {
  static bool init = false;
  if (init) return;
  init = true;
  TH1::SetDefaultSumw2();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(111);
  init_fcns();

  h[0] = pls[0][0].GetHist();
  h[1] = pls[1][0].GetHist();

  h[0]->SetStats(0);
  h[1]->SetStats(0);
}

void save(const char* ofname) {
  TFile ofile(ofname, "RECREATE");

  char s[2] = { 'l', 'r' }, p[4] = { 'p', 'd', 't', 'a' };
  for (int i = 0; i < 2; ++i) {
    h[i]->SetDirectory(&ofile);
    h[i]->Write(TString::Format("evde_%c", s[i]));
    for (int j = 0; j < 4; ++j) {
      f[i][j]->Write(TString::Format("cut_%c_%c", s[i], p[j]));
      t[i][j]->Clone(TString::Format("fcn_%c_%c", s[i], p[j]))->Write();
      // t[i][j]->Write();
    }
  }
}

void draw_psel_1d() {
  TFile* ifile = new TFile(IFNAME_PSEL);
  TH3* h3p[2] = {
    (TH3*)ifile->Get("evde_r0_proton"),
    (TH3*)ifile->Get("evde_l0_proton")
  };
  h3p[0]->GetZaxis()->SetRangeUser(400, 100e3);
  h3p[1]->GetZaxis()->SetRangeUser(400, 100e3);
  TH1* h1p[2] = { h3p[0]->Project3D("x"), h3p[1]->Project3D("x") };

  TCanvas* c = new TCanvas;
  TLegend* l = new TLegend(0.7, 0.7, 0.9, 0.9);
  char lt[2][128];
  sprintf(lt[0], "Left Telescope (%d)",  (int)h1p[0]->GetEntries());
  sprintf(lt[1], "Right Telescope (%d)", (int)h1p[1]->GetEntries());
  l->AddEntry(h1p[0], lt[0]);
  l->AddEntry(h1p[1], lt[1]);
  h1p[0]->SetLineColor(kRed);
  h1p[1]->SetLineColor(kBlue);
  h1p[0]->SetTitle("Measured Proton Energy Spectrum;E [keV]");
  h1p[0]->SetStats(false);
  h1p[1]->SetStats(false);
  h1p[0]->Draw();
  h1p[1]->Draw("SAME");
  l->Draw();
  c->SaveAs("img/e_raw_proton.png");
}

void r15b_al50_psel_data_draw() {
  draw_psel_1d();

  init_hists();
  TLegend*  l1      = new TLegend(0.1, 0.7, 0.3, 0.9);
  TLegend*  l2      = new TLegend(0.7, 0.7, 0.9, 0.9);
  TLine* line_model = new TLine();
  TLine* line_class = new TLine();
  line_model->SetLineStyle(2);
  l1->AddEntry(line_model, "Fit function", "l");
  l1->AddEntry(line_class, "Cut from fit", "l");
  l2->AddEntry(f[0][0],    "Proton",       "l");
  l2->AddEntry(f[0][1],    "Deuteron",     "l");
  l2->AddEntry(f[0][2],    "Triton",       "l");
  l2->AddEntry(f[0][3],    "#alpha",       "l");

  TCanvas* cl = new TCanvas("cl", "cl");
  h[0]->SetTitle("Left Telescope PID;E [keV];dE [keV]");
  h[0]->GetYaxis()->SetTitleOffset(1.4);
  h[0]->Draw();
  t[0][0]->Draw("SAME"); f[0][0]->Draw("SAME CONT3");
  t[0][1]->Draw("SAME"); f[0][1]->Draw("SAME CONT3");
  t[0][2]->Draw("SAME"); f[0][2]->Draw("SAME CONT3");
  t[0][3]->Draw("SAME"); f[0][3]->Draw("SAME CONT3");
  l1->Draw();            l2->Draw();
  cl->SetLogz();
  cl->SaveAs("img/l_data_classification.png");
  TCanvas* cr = new TCanvas("cr", "cr");
  h[1]->SetTitle("Right Telescope PID;E [keV];dE [keV]");
  h[1]->GetYaxis()->SetTitleOffset(1.4);
  h[1]->Draw();
  t[1][0]->Draw("SAME"); f[1][0]->Draw("SAME CONT3");
  t[1][1]->Draw("SAME"); f[1][1]->Draw("SAME CONT3");
  t[1][2]->Draw("SAME"); f[1][2]->Draw("SAME CONT3");
  t[1][3]->Draw("SAME"); f[1][3]->Draw("SAME CONT3");
  l1->Draw();        l2->Draw();
  cr->SetLogz();
  cr->SaveAs("img/r_data_classification.png");

  TCanvas* cl_zoom = new TCanvas("cl_zoom", "cl_zoom");
  TH2* hl_zoom = (TH2*)h[0]->Clone("hl_zoom");
  hl_zoom->GetYaxis()->SetRangeUser(0, 3e3);
  hl_zoom->Draw();
  t[0][0]->Draw("SAME"); f[0][0]->Draw("SAME CONT3");
  t[0][1]->Draw("SAME"); f[0][1]->Draw("SAME CONT3");
  t[0][2]->Draw("SAME"); f[0][2]->Draw("SAME CONT3");
  t[0][3]->Draw("SAME"); f[0][3]->Draw("SAME CONT3");
  l1->Draw();        l2->Draw();
  cl_zoom->SetLogz();
  cl_zoom->SaveAs("img/l_data_classification_zoom.png");
  TCanvas* cr_zoom = new TCanvas("cr_zoom", "cr_zoom");
  TH2* hr_zoom = (TH2*)h[1]->Clone("hr_zoom");
  hr_zoom->GetYaxis()->SetRangeUser(0, 3e3);
  hr_zoom->Draw();
  t[1][0]->Draw("SAME"); f[1][0]->Draw("SAME CONT3");
  t[1][1]->Draw("SAME"); f[1][1]->Draw("SAME CONT3");
  t[1][2]->Draw("SAME"); f[1][2]->Draw("SAME CONT3");
  t[1][3]->Draw("SAME"); f[1][3]->Draw("SAME CONT3");
  l1->Draw();        l2->Draw();
  cr_zoom->SetLogz();
  cr_zoom->SaveAs("img/r_data_classification_zoom.png");


  // TLine* lim[2] = { new TLine(2.e3, 200., 19.e3, 200.),
  //                   new TLine(2.e3, 50.,  19.e3, 50.) };
  // lim[0]->SetLineStyle(9); lim[1]->SetLineStyle(9);
  // TCanvas* csigl = new TCanvas;
  // TGraph glp = pls_l[0].SigGraph();
  // TGraph gld = pls_l[1].SigGraph();
  // TGraph glt = pls_l[2].SigGraph();
  // TGraph gla = pls_l[3].SigGraph();
  // TMultiGraph* mgl = new TMultiGraph();
  // mgl->Add(&glp); mgl->Add(&gld); mgl->Add(&glt); mgl->Add(&gla);
  // mgl->SetTitle("SiL PID #sigma;Energy [keV];Fit value [keV']");
  // glp.SetLineColor(kRed);
  // gld.SetLineColor(kGreen);
  // glt.SetLineColor(kBlue);
  // gla.SetLineColor(kMagenta);
  // mgl->Draw("A");
  // lim[0]->Draw(); lim[1]->Draw();
  // l2->Draw();
  // csigl->SaveAs("img/l_data_class_sig.png");
  // csigl->SaveAs("img/l_data_class_sig.C");
  // TCanvas* csigr = new TCanvas;
  // TGraph grp = pls_r[0].SigGraph();
  // TGraph grd = pls_r[1].SigGraph();
  // TGraph grt = pls_r[2].SigGraph();
  // TGraph gra = pls_r[3].SigGraph();
  // TMultiGraph* mgr = new TMultiGraph();
  // mgr->Add(&grp); mgr->Add(&grd); mgr->Add(&grt); mgr->Add(&gra);
  // mgr->SetTitle("SiR PID #sigma;Energy [keV];Fit value [keV']");
  // grp.SetLineColor(kRed);
  // grd.SetLineColor(kGreen);
  // grt.SetLineColor(kBlue);
  // gra.SetLineColor(kMagenta);
  // mgr->Draw("A");
  // lim[0]->Draw(); lim[1]->Draw();
  // l2->Draw();
  // csigr->SaveAs("img/r_data_class_sig.png");
  // csigr->SaveAs("img/r_data_class_sig.C");

  // for (int i = /*0*/1; i < 4; ++i) {
  //   char ps[4] = {'p', 'd', 't', 'a'};
  //   vector<TH1*>& v = pls_r[i].GetHists();
  //   vector<TF1*>& f = pls_r[i].GetFits();
  //   for (int j = 0; j < v.size(); ++j) {
  //     TCanvas* c = new TCanvas();
  //     v[j]->Draw();
  //     f[j]->Draw("SAME");
  //     char ofname[256];
  //     sprintf(ofname, "img/%i_%c_fit.png", j, ps[i]);
  //     c->SaveAs(ofname);
  //   }
  //   break;
  // }

  save("img/psel.root");
}
