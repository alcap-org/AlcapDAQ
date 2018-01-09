#include "./scripts/Al50/R15b_Al50_psel.C"

#include "TCanvas.h"
#include "TF2.h"
#include "TLegend.h"

#include <iostream>

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


vector<ParticleLikelihood> pls_l, pls_r;
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
void init_fcns() {
  static bool init = false;
  if (init) return;
  init = true;

  pls_l = LoadParticleLikelihoods('L');
  pls_r = LoadParticleLikelihoods('R');
  fpl = new TF2("fpl", pfcn_l, 0., 20.e3, 0., 10.e3);
  fpr = new TF2("fpr", pfcn_r, 0., 20.e3, 0., 10.e3);
  fdl = new TF2("fdl", dfcn_l, 0., 20.e3, 0., 10.e3);
  fdr = new TF2("fdr", dfcn_r, 0., 20.e3, 0., 10.e3);
  ftl = new TF2("ftl", tfcn_l, 0., 20.e3, 0., 10.e3);
  ftr = new TF2("ftr", tfcn_r, 0., 20.e3, 0., 10.e3);
  fal = new TF2("fal", afcn_l, 0., 20.e3, 0., 10.e3);
  far = new TF2("far", afcn_r, 0., 20.e3, 0., 10.e3);

  Double_t fcont[1] = { 1.e-4 };
  fpl->SetNpx(200); fpl->SetNpy(200); fpl->SetContour(1, fcont);
  fpr->SetNpx(200); fpr->SetNpy(200); fpr->SetContour(1, fcont);
  fdl->SetNpx(200); fdl->SetNpy(200); fdl->SetContour(1, fcont);
  fdr->SetNpx(200); fdr->SetNpy(200); fdr->SetContour(1, fcont);
  ftl->SetNpx(200); ftl->SetNpy(200); ftl->SetContour(1, fcont);
  ftr->SetNpx(200); ftr->SetNpy(200); ftr->SetContour(1, fcont);
  fal->SetNpx(200); fal->SetNpy(200); fal->SetContour(1, fcont);
  far->SetNpx(200); far->SetNpy(200); far->SetContour(1, fcont);
  fpl->SetLineColor(kRed);
  fpr->SetLineColor(kRed);
  fdl->SetLineColor(kGreen);
  fdr->SetLineColor(kGreen);
  ftl->SetLineColor(kBlue);
  ftr->SetLineColor(kBlue);
  fal->SetLineColor(kMagenta);
  far->SetLineColor(kMagenta);
}

TH2      *hpl, *hpr, *hdl, *hdr, *htl, *htr, *hal, *har;
TProfile *ppl, *ppr, *pdl, *pdr, *ptl, *ptr, *pal, *par;
TH2      *spl, *spr, *sdl, *sdr, *stl, *str, *sal, *sar, *sul, *sur;
TH1      *epl, *epr, *edl, *edr, *etl, *etr, *eal, *ear;
void init_hists() {
  static bool init = false;
  if (init) return;
  init = true;
  TH1::SetDefaultSumw2();
  init_fcns();

  hpl = pls_l[0].GetHist();
  hpr = pls_r[0].GetHist();
  hdl = pls_l[1].GetHist();
  hdr = pls_r[1].GetHist();
  htl = pls_l[2].GetHist();
  htr = pls_r[2].GetHist();
  hal = pls_l[3].GetHist();
  har = pls_r[3].GetHist();

  ppl = pls_l[0].GetProfile();
  ppr = pls_r[0].GetProfile();
  pdl = pls_l[1].GetProfile();
  pdr = pls_r[1].GetProfile();
  ptl = pls_l[2].GetProfile();
  ptr = pls_r[2].GetProfile();
  pal = pls_l[3].GetProfile();
  par = pls_r[3].GetProfile();

  TFile* psel_file = new TFile(IFNAME_PSEL);
  spl = (TH2*)psel_file->Get("evde_l0_proton");
  spr = (TH2*)psel_file->Get("evde_r0_proton");
  sdl = (TH2*)psel_file->Get("evde_l0_deuteron");
  sdr = (TH2*)psel_file->Get("evde_r0_deuteron");
  stl = (TH2*)psel_file->Get("evde_l0_triton");
  str = (TH2*)psel_file->Get("evde_r0_triton");
  sal = (TH2*)psel_file->Get("evde_l0_alpha");
  sar = (TH2*)psel_file->Get("evde_r0_alpha");
  sul = (TH2*)psel_file->Get("evde_l0_unclassified");
  sur = (TH2*)psel_file->Get("evde_r0_unclassified");


  epl = spl->ProfileX();
  epr = spr->ProfileX();
  edl = sdl->ProfileX();
  edr = sdr->ProfileX();
  etl = stl->ProfileX();
  etr = str->ProfileX();
  eal = sal->ProfileX();
  ear = sar->ProfileX();

  hpl->SetMarkerColor(kRed);
  hpr->SetMarkerColor(kRed);
  hdl->SetMarkerColor(kGreen);
  hdr->SetMarkerColor(kGreen);
  htl->SetMarkerColor(kBlue);
  htr->SetMarkerColor(kBlue);
  hal->SetMarkerColor(kMagenta);
  har->SetMarkerColor(kMagenta);

  ppl->SetLineColor(kRed);
  ppr->SetLineColor(kRed);
  pdl->SetLineColor(kGreen);
  pdr->SetLineColor(kGreen);
  ptl->SetLineColor(kBlue);
  ptr->SetLineColor(kBlue);
  pal->SetLineColor(kMagenta);
  par->SetLineColor(kMagenta);

  spl->SetMarkerColor(kRed);
  spr->SetMarkerColor(kRed);
  sdl->SetMarkerColor(kGreen);
  sdr->SetMarkerColor(kGreen);
  stl->SetMarkerColor(kBlue);
  str->SetMarkerColor(kBlue);
  sal->SetMarkerColor(kMagenta);
  sar->SetMarkerColor(kMagenta);
  sul->SetMarkerColor(kBlack);
  sur->SetMarkerColor(kBlack);

  spl->SetLineColor(kRed);
  spr->SetLineColor(kRed);
  sdl->SetLineColor(kGreen);
  sdr->SetLineColor(kGreen);
  stl->SetLineColor(kBlue);
  str->SetLineColor(kBlue);
  sal->SetLineColor(kMagenta);
  sar->SetLineColor(kMagenta);
  sul->SetLineColor(kBlack);
  sur->SetLineColor(kBlack);

  epl->SetLineColor(kBlue);
  epr->SetLineColor(kRed);
  edl->SetLineColor(kBlue);
  edr->SetLineColor(kRed);
  etl->SetLineColor(kBlue);
  etr->SetLineColor(kRed);
  eal->SetLineColor(kBlue);
  ear->SetLineColor(kRed);

  hpl->SetStats(false);
  hpr->SetStats(false);
  hdl->SetStats(false);
  hdr->SetStats(false);
  htl->SetStats(false);
  htr->SetStats(false);
  hal->SetStats(false);
  har->SetStats(false);

  ppl->SetStats(false);
  ppr->SetStats(false);
  pdl->SetStats(false);
  pdr->SetStats(false);
  ptl->SetStats(false);
  ptr->SetStats(false);
  pal->SetStats(false);
  par->SetStats(false);

  spl->SetStats(false);
  spr->SetStats(false);
  sdl->SetStats(false);
  sdr->SetStats(false);
  stl->SetStats(false);
  str->SetStats(false);
  sal->SetStats(false);
  sar->SetStats(false);
  sul->SetStats(false);
  sur->SetStats(false);

  epl->SetStats(false);
  epr->SetStats(false);
  edl->SetStats(false);
  edr->SetStats(false);
  etl->SetStats(false);
  etr->SetStats(false);
  eal->SetStats(false);
  ear->SetStats(false);

  hpl->SetTitle("SiL MC;E_{tot} [keV];E_{thin} [keV]");
  hpr->SetTitle("SiR MC;E_{tot} [keV];E_{thin} [keV]");
  ppl->SetTitle("SiL MC Cut;E_{tot} [keV];E_{thin} [keV]");
  ppr->SetTitle("SiR MC Cut;E_{tot} [keV];E_{thin} [keV]");
  spl->SetTitle("SiL Particle Classification;E_{tot} [keV];dE [keV]");
  spr->SetTitle("SiR Particle Classification;E_{tot} [keV];dE [keV]");
  epl->SetTitle("Proton Raw Energy");
  edl->SetTitle("Deuteron Raw Energy");
  etl->SetTitle("Triton Raw Energy");
  eal->SetTitle("Alpha Raw Energy");
}

void r15b_al50_psel_draw() {
  init_hists();
  TLegend* l = new TLegend(0.7, 0.7, 0.9, 0.9);
  l->AddEntry(ppl, "Proton",   "l");
  l->AddEntry(pdl, "Deuteron", "l");
  l->AddEntry(ptl, "Triton",   "l");
  l->AddEntry(pal, "#alpha",   "l");
  char legentry_l[32], legentry_r[32];
  TLegend* lep = new TLegend(0.7, 0.7, 0.9, 0.9);
  TLegend* led = new TLegend(0.7, 0.7, 0.9, 0.9);
  TLegend* let = new TLegend(0.7, 0.7, 0.9, 0.9);
  TLegend* lea = new TLegend(0.7, 0.7, 0.9, 0.9);
  sprintf(legentry_l, "Left (%d)",  epl->GetEntries());
  sprintf(legentry_r, "Right (%d)", epr->GetEntries());
  lep->AddEntry(epl, legentry_l);
  lep->AddEntry(epr, legentry_r);
  sprintf(legentry_l, "Left (%d)",  edl->GetEntries());
  sprintf(legentry_r, "Right (%d)", edr->GetEntries());
  led->AddEntry(edl, legentry_l);
  led->AddEntry(edr, legentry_r);
  sprintf(legentry_l, "Left (%d)",  etl->GetEntries());
  sprintf(legentry_r, "Right (%d)", etr->GetEntries());
  let->AddEntry(epl, legentry_l);
  let->AddEntry(epr, legentry_r);
  sprintf(legentry_l, "Left (%d)",  eal->GetEntries());
  sprintf(legentry_r, "Right (%d)", ear->GetEntries());
  lea->AddEntry(eal, legentry_l);
  lea->AddEntry(ear, legentry_r);

  TCanvas* chl = new TCanvas;
  hpl->Draw();
  hdl->Draw("SAME");
  htl->Draw("SAME");
  hal->Draw("SAME");
  l->Draw();
  TCanvas* chr = new TCanvas;
  hpr->Draw();
  hdr->Draw("SAME");
  htr->Draw("SAME");
  har->Draw("SAME");
  l->Draw();

  TFile* data_file = new TFile(IFNAME_PRAW);
  TH2* datl = (TH2*)data_file->Get("evde_l0");
  TH2* datr = (TH2*)data_file->Get("evde_r0");
  datl->SetTitle("EvdE SiL;E_{tot}[keV];dE[keV]");
  datr->SetTitle("EvdE SiR;E_{tot}[keV];dE[keV]");
  datl->SetStats(0);
  datr->SetStats(0);
  TCanvas* cpl = new TCanvas();
  ppl->GetYaxis()->SetRangeUser(0, 10e3);
  datl->Draw();
  ppl->Draw("SAME"); fpl->Draw("SAME CONT3");
  pdl->Draw("SAME"); fdl->Draw("SAME CONT3");
  ptl->Draw("SAME"); ftl->Draw("SAME CONT3");
  pal->Draw("SAME"); fal->Draw("SAME CONT3");
  l->Draw();
  TCanvas* cpr = new TCanvas();
  ppr->GetYaxis()->SetRangeUser(0, 10e3);
  datr->Draw();
  ppr->Draw("SAME"); fpr->Draw("SAME CONT3");
  pdr->Draw("SAME"); fdr->Draw("SAME CONT3");
  ptr->Draw("SAME"); ftr->Draw("SAME CONT3");
  par->Draw("SAME"); far->Draw("SAME CONT3");
  l->Draw();
  TCanvas* csl = new TCanvas();
  spl->Draw();       sdl->Draw("SAME");
  stl->Draw("SAME"); sal->Draw("SAME");
  sul->Draw("SAME"); l->Draw();
  TCanvas* csr = new TCanvas();
  spr->Draw();       sdr->Draw("SAME");
  str->Draw("SAME"); sar->Draw("SAME");
  sur->Draw("SAME"); l->Draw();
  TCanvas* cep = new TCanvas();
  epl->Draw(); epr->Draw("SAME"); lep->Draw();
  TCanvas* ced = new TCanvas();
  edl->Draw(); edr->Draw("SAME"); led->Draw();
  TCanvas* cet = new TCanvas();
  etl->Draw(); etr->Draw("SAME"); let->Draw();
  TCanvas* cea = new TCanvas();
  eal->Draw(); ear->Draw("SAME"); lea->Draw();

  chl->SaveAs("mc_evde_l_raw.png");
  chr->SaveAs("mc_evde_r_raw.png");
  cpl->SaveAs("mc_evde_l_cut.png");
  cpr->SaveAs("mc_evde_r_cut.png");
  csl->SaveAs("data_evde_l_classified.png");
  csr->SaveAs("data_evde_r_classified.png");
  cep->SaveAs("data_e_proton.png");
  ced->SaveAs("data_e_deuteron.png");
  cet->SaveAs("data_e_triton.png");
  cea->SaveAs("data_e_alpha.png");
}
