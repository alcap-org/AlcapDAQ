#include "psel.h"

#include "TCanvas.h"
#include "TChain.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStyle.h"

double ROI_L_LO_PARAM[5] = { 6.5e5, 8.5e5, -0.85, 2.5e3, 8e3 };
double ROI_L_HI_PARAM[5] = { 13e5, 16e5,   -0.85, 2.5e3, 8e3 };
double ROI_R_LO_PARAM[5] = { 9e5,  12e5,   -0.9,  2.5e3, 8e3 };
double ROI_R_HI_PARAM[5] = { 20e5, 24e5,   -0.9,  2.5e3, 8e3 };
double BINWIDTH          = 250;

class Event {
  public:
  TTree* tr;
  double e1, e2, e3, t1, t2, t3;
  ParticleLikelihood::PSelPow roihi, roilo;
  Event(TTree* tr, double parlo[5], double parhi[5]) :
  tr(tr),
  roihi(parhi[0], parhi[1], parhi[2], parhi[3], parhi[4]),
  roilo(parlo[0], parlo[1], parlo[2], parlo[3], parlo[4]) {
    tr->SetBranchAddress("e1", &e1);
    tr->SetBranchAddress("e2", &e2);
    tr->SetBranchAddress("e3", &e3);
    tr->SetBranchAddress("t1", &t1);
    tr->SetBranchAddress("t2", &t2);
    tr->SetBranchAddress("t3", &t3);
  }
  double dT()            { return std::abs(t2-t1); }
  double E()             { return e1 + e2 + e3; }
  double dE()            { return e1; }
  bool   GoodStop()      { return e1 > 0 && e2 > 0 && e3 == 0 && dT() < 200; }
  int    GetEntries()    { return tr->GetEntries(); }
  int    GetEntry(int i) { return tr->GetEntry(i); }
  bool IsInROIHi(double e, double de) { return roihi.IsParticle(e, de); }
  bool IsInROILo(double e, double de) { return roilo.IsParticle(e, de); }
};

void BuildChains(TChain* ch[2], TChain* p[2]) {
  ch[0]->Add("data/Al50/psel.3layer.root/PID_LP");
  ch[0]->Add("data/Al50/psel.3layer.root/PID_LD");
  ch[0]->Add("data/Al50/psel.3layer.root/PID_LT");
  ch[0]->Add("data/Al50/psel.3layer.root/PID_LA");
  ch[0]->Add("data/Al50/psel.3layer.root/PID_LU");
  ch[1]->Add("data/Al50/psel.3layer.root/PID_RP");
  ch[1]->Add("data/Al50/psel.3layer.root/PID_RD");
  ch[1]->Add("data/Al50/psel.3layer.root/PID_RT");
  ch[1]->Add("data/Al50/psel.3layer.root/PID_RA");
  ch[1]->Add("data/Al50/psel.3layer.root/PID_RU");
  p [0]->Add("data/Al50/psel.3layer.root/PID_LP");
  p [1]->Add("data/Al50/psel.3layer.root/PID_RP");
}

void NormalizeHist(ParticleLikelihood::PSelPow& psel, TH1* h) {
  TH1* hnorm  = (TH1*)h->Clone("hlo_norm");
  hnorm->Reset();
  for (int i = 1; i <= h->GetNbinsX(); ++i)
    hnorm->SetBinContent(i,
                         psel.f[1]->Integral(h->GetXaxis()->GetBinLowEdge(i),
                                             h->GetXaxis()->GetBinUpEdge(i)) -
                         psel.f[0]->Integral(h->GetXaxis()->GetBinLowEdge(i),
                                             h->GetXaxis()->GetBinUpEdge(i)));
  h->Divide(hnorm);
  delete hnorm;
}

void BuilddEvEHists(TChain* tr[2], TH2* h[2]) {
  TH1::SetDefaultSumw2(true);
  h[0] = new TH2D("hldeve", "Left BG Selection;E [keV];dE [keV]",
                  1450, 1.5e3, 16e3, 300, 0, 3e3);
  h[1] = new TH2D("hrdeve", "Right BG Selection;E [keV];dE [keV]",
                  1450, 1.5e3, 16e3, 300, 0, 3e3);
  tr[0]->Draw("de:e>>hldeve", "dt < 200 && e3 == 0", "goff");
  tr[1]->Draw("de:e>>hrdeve", "dt < 200 && e3 == 0", "goff");
}

void BuildPHists(TChain* p[2], TH1* hp[2]) {
  TH1::SetDefaultSumw2(true);
  double ELIM[2] = { 1.5e3, 16e3 };
  int    N       = (ELIM[1]-ELIM[0])/BINWIDTH;
  hp[0] = new TH1D("hlp", "Left;E [keV];(cts/keV^2)/250 keV",  N, ELIM[0], ELIM[1]);
  hp[1] = new TH1D("hrp", "Right;E [keV];(cts/keV^2)/250 keV", N, ELIM[0], ELIM[1]);
  p[0]->Draw("e>>hlp", "dt < 200 && e3 == 0", "goff");
  p[1]->Draw("e>>hrp", "dt < 200 && e3 == 0", "goff");
  ParticleLikelihood::PSelPow psels[2] = {
    ParticleLikelihood::PSelPow(85e4, 15e5, -0.85, ELIM[0], ELIM[1]),
    ParticleLikelihood::PSelPow(12e5, 22e5, -0.9,  ELIM[0], ELIM[1])
  };
  NormalizeHist(psels[0], hp[0]);
  NormalizeHist(psels[1], hp[1]);
  // TH1* hlp_norm = (TH1*)hp[0]->Clone("hlp_norm");
  // TH1* hrp_norm = (TH1*)hp[1]->Clone("hrp_norm");
  // hlp_norm ->Reset();
  // hrp_norm ->Reset();
  // for (int i = 1; i <= hp[0]->GetNbinsX(); ++i) {
  //   hlp_norm->SetBinContent(i,
  //                           psels[0].f[1]->Integral(hp[0]->GetXaxis()->GetBinLowEdge(i),
  //                                                   hp[0]->GetXaxis()->GetBinUpEdge(i)) -
  //                           psels[0].f[0]->Integral(hp[0]->GetXaxis()->GetBinLowEdge(i),
  //                                                   hp[0]->GetXaxis()->GetBinUpEdge(i)));
  //   hrp_norm->SetBinContent(i,
  //                           psels[1].f[1]->Integral(hp[1]->GetXaxis()->GetBinLowEdge(i),
  //                                                   hp[1]->GetXaxis()->GetBinUpEdge(i)) -
  //                           psels[1].f[0]->Integral(hp[1]->GetXaxis()->GetBinLowEdge(i),
  //                                                   hp[1]->GetXaxis()->GetBinUpEdge(i)));
  // }
  // hp[0]->Divide(hlp_norm);
  // hp[1]->Divide(hrp_norm);
  // delete hlp_norm;
  // delete hrp_norm;
}

double FitBackground(TH1* h) {
  TF1* f = new TF1("", "[0]");
  f->SetParameter(0, 0);
  f->SetLineStyle(9);
  f->SetLineColor(1);
  h->Fit(f);
  return f->GetParameter(0);
}
void BookBGHists(TH1*& hllo, TH1*& hlhi, TH1*& hlavg,
                 TH1*& hrlo, TH1*& hrhi, TH1*& hravg) {
  TH1::SetDefaultSumw2(true);
  double ELIM[2] = { 1.5e3, 16e3 };
  int    N       = (ELIM[1]-ELIM[0])/BINWIDTH;
  hllo  = new TH1D("hlbglo",  "Left BG w/avg;E [keV];(cts/keV^2)/250 keV",  N, ELIM[0], ELIM[1]);
  hlhi  = new TH1D("hlbghi",  "Left BG w/avg;E [keV];(cts/keV^2)/250 keV",  N, ELIM[0], ELIM[1]);
  hlavg = new TH1D("hlbgavg", "Left BG w/avg;E [keV];(cts/keV^2)/250 keV",  N, ELIM[0], ELIM[1]);
  hrlo  = new TH1D("hrbglo",  "Right BG w/avg;E [keV];(cts/keV^2)/250 keV", N, ELIM[0], ELIM[1]);
  hrhi  = new TH1D("hrbghi",  "Right BG w/avg;E [keV];(cts/keV^2)/250 keV", N, ELIM[0], ELIM[1]);
  hravg = new TH1D("hrbgavg", "Right BG w/avg;E [keV];(cts/keV^2)/250 keV", N, ELIM[0], ELIM[1]);
}

void Process(Event& ev, TH1* hlo, TH1* hhi, TH1* havg) {
  for (int i = 0; i < ev.GetEntries(); ++i) {
    ev.GetEntry(i);
    if      (!ev.GoodStop()) continue;
    if      (ev.IsInROIHi(ev.E(), ev.dE())) hhi->Fill(ev.E());
    else if (ev.IsInROILo(ev.E(), ev.dE())) hlo->Fill(ev.E());
  }
  // NormalizeHist(ev.roilo, hlo);
  // NormalizeHist(ev.roihi, hhi);
  TH1* hlo_norm  = (TH1*)hlo ->Clone("hlo_norm");
  TH1* hhi_norm  = (TH1*)hhi ->Clone("hhi_norm");
  TH1* havg_norm = (TH1*)havg->Clone("havg_norm");
  hlo_norm ->Reset();
  hhi_norm ->Reset();
  havg_norm->Reset();
  for (int i = 1; i <= hlo->GetNbinsX(); ++i) {
    hlo_norm->SetBinContent(i,
                            ev.roilo.f[1]->Integral(hlo->GetXaxis()->GetBinLowEdge(i),
                                                    hlo->GetXaxis()->GetBinUpEdge(i)) -
                            ev.roilo.f[0]->Integral(hlo->GetXaxis()->GetBinLowEdge(i),
                                                    hlo->GetXaxis()->GetBinUpEdge(i)));
    hhi_norm->SetBinContent(i,
                            ev.roihi.f[1]->Integral(hhi->GetXaxis()->GetBinLowEdge(i),
                                                    hhi->GetXaxis()->GetBinUpEdge(i)) -
                            ev.roihi.f[0]->Integral(hhi->GetXaxis()->GetBinLowEdge(i),
                                                    hhi->GetXaxis()->GetBinUpEdge(i)));
  }
  havg     ->Add(hlo);
  havg     ->Add(hhi);
  havg_norm->Add(hlo_norm);
  havg_norm->Add(hhi_norm);
  hlo      ->Divide(hlo_norm);
  hhi      ->Divide(hhi_norm);
  havg     ->Divide(havg_norm);
  delete hlo_norm;
  delete hhi_norm;
  delete havg_norm;
}

void background_from_side_bands() {
  TChain* all[2] = { new TChain, new TChain };
  TChain* p  [2] = { new TChain, new TChain };
  BuildChains(all, p);
  TH2* hdeve[2];
  TH1* hp[2];
  BuilddEvEHists(all, hdeve);
  BuildPHists(p, hp);
  TH1 *hllo, *hlhi, *hlavg, *hrlo, *hrhi, *hravg;
  BookBGHists(hllo, hlhi, hlavg, hrlo, hrhi, hravg);
  Event ev[2] = { Event(all[0], ROI_L_LO_PARAM, ROI_L_HI_PARAM),
                  Event(all[1], ROI_R_LO_PARAM, ROI_R_HI_PARAM) };
  Process(ev[0], hllo, hlhi, hlavg);
  Process(ev[1], hrlo, hrhi, hravg);
  double nbg[2] = { FitBackground(hlavg), FitBackground(hravg) };

  hllo ->SetLineColor(4);
  hlhi ->SetLineColor(2);
  hlavg->SetLineColor(1);
  hp[0]->SetLineColor(3);
  hrlo ->SetLineColor(4);
  hrhi ->SetLineColor(2);
  hravg->SetLineColor(1);
  hp[1]->SetLineColor(3);
  hllo->GetYaxis()->SetTitleOffset(1.4);
  hrlo->GetYaxis()->SetTitleOffset(1.4);

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hlhi,  "BG Above");
  leg->AddEntry(hllo,  "BG Below");
  leg->AddEntry(hlavg, "BG Avg");
  leg->AddEntry(hp[0], "Protons");


  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1400, 1000);
  c->Divide(2, 2);
  c->cd(1);
  hdeve[0]->Draw();
  ev[0].roihi.Draw(2);
  ev[0].roilo.Draw(4);
  c->cd(2);
  hdeve[1]->Draw();
  ev[1].roihi.Draw(2);
  ev[1].roilo.Draw(4);
  c->cd(3);
  hp[0]->Draw();
  hllo ->Draw("SAME E");
  hlhi ->Draw("SAME E");
  hlavg->Draw("SAME E");
  leg ->Draw();
  c->cd(4);
  hp[1]->Draw();
  hrlo ->Draw("SAME E");
  hrhi ->Draw("SAME E");
  hravg->Draw("SAME E");
  leg  ->Draw();
  c->SaveAs("img/bg_side_bands.png");

  printf("L/R BG (cts/keV^2)/100keV:\t%f\t%f\n", nbg[0], nbg[1]);
}
