#include "util.h"

#include "TCanvas.h"
#include "TColor.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "THStack.h"
#include "TLegend.h"
#include "TMultiGraph.h"

void compare_proton_tcut_unfolded() {
  static const int N = 19;
  TFile* fs[N];
  TH1* hs[N][3];
  double roi1[N][3], roi2[N][3];
  TGraphErrors* grroi1[3] = { new TGraphErrors(N), new TGraphErrors(N), new TGraphErrors(N) };
  TGraphErrors* grroi2[3] = { new TGraphErrors(N), new TGraphErrors(N), new TGraphErrors(N) };
  for (int i = 0; i < N; ++i) {
    int t = (2+i)*100;
    char ifname[64];
    sprintf(ifname,
            "data/Al50/en.p.t%dns.dt200ns.rebin5.pp10us.elo2e3keV.ehi17e3keV.root",
            t);
    fs[i] = new TFile(ifname);
    hs[i][0] = (TH1*)fs[i]->Get("hl_e_reco_bay");
    hs[i][1] = (TH1*)fs[i]->Get("hr_e_reco_bay");
    hs[i][0]->Scale(1./(Normalization::Al50(t)));
    hs[i][1]->Scale(1./(Normalization::Al50(t)));
    hs[i][2] = (TH1*)hs[i][0]->Clone();
    hs[i][2]->Add(hs[i][1]);
    hs[i][2]->Scale(0.5);
    double Emin = 2e3, Emax = 14e3;
    hs[i][0]->GetXaxis()->SetRangeUser(Emin, Emax);
    hs[i][1]->GetXaxis()->SetRangeUser(Emin, Emax);
    hs[i][2]->GetXaxis()->SetRangeUser(Emin, Emax);
    double roi1_err[3], roi2_err[3];
    roi1[i][0] = hs[i][0]->IntegralAndError(hs[i][0]->FindFixBin(3.5e3), hs[i][0]->FindFixBin(10e3), roi1_err[0]);
    roi1[i][1] = hs[i][1]->IntegralAndError(hs[i][1]->FindFixBin(3.5e3), hs[i][1]->FindFixBin(10e3), roi1_err[1]);
    roi1[i][2] = hs[i][2]->IntegralAndError(hs[i][2]->FindFixBin(3.5e3), hs[i][2]->FindFixBin(10e3), roi1_err[2]);
    roi2[i][0] = hs[i][0]->IntegralAndError(hs[i][0]->FindFixBin(4e3),   hs[i][0]->FindFixBin(8e3),  roi2_err[0]);
    roi2[i][1] = hs[i][1]->IntegralAndError(hs[i][1]->FindFixBin(4e3),   hs[i][1]->FindFixBin(8e3),  roi2_err[1]);
    roi2[i][2] = hs[i][2]->IntegralAndError(hs[i][2]->FindFixBin(4e3),   hs[i][2]->FindFixBin(8e3),  roi2_err[2]);
    grroi1[0]->SetPoint(i, t, roi1[i][0]);
    grroi1[1]->SetPoint(i, t, roi1[i][1]);
    grroi1[2]->SetPoint(i, t, roi1[i][2]);
    grroi2[0]->SetPoint(i, t, roi2[i][0]);
    grroi2[1]->SetPoint(i, t, roi2[i][1]);
    grroi2[2]->SetPoint(i, t, roi2[i][2]);
    grroi1[0]->SetPointError(i, 0, roi1_err[0]);
    grroi1[1]->SetPointError(i, 0, roi1_err[1]);
    grroi1[2]->SetPointError(i, 0, roi1_err[2]);
    grroi2[0]->SetPointError(i, 0, roi2_err[0]);
    grroi2[1]->SetPointError(i, 0, roi2_err[1]);
    grroi2[2]->SetPointError(i, 0, roi2_err[2]);
  }

  TMultiGraph* mgroi1 = new TMultiGraph("mgroi1", "3.5-10 MeV;Proton Time Cut [ns];Rate [p/#mucap]");
  TMultiGraph* mgroi2 = new TMultiGraph("mgroi2", "4-8 MeV;Proton Time Cut [ns];Rate [p/#mucap]");
  mgroi1->Add(grroi1[0]);
  mgroi1->Add(grroi1[1]);
  mgroi1->Add(grroi1[2]);
  mgroi2->Add(grroi2[0]);
  mgroi2->Add(grroi2[1]);
  mgroi2->Add(grroi2[2]);
  grroi1[0]->SetLineColor(kBlue);
  grroi1[1]->SetLineColor(kRed);
  grroi1[2]->SetLineColor(kBlack);
  grroi2[0]->SetLineColor(kBlue);
  grroi2[1]->SetLineColor(kRed);
  grroi2[2]->SetLineColor(kBlack);

  TLegend* l = new TLegend(0.1, 0.7, 0.3, 0.9);
  l->AddEntry(grroi1[0], "SiL");
  l->AddEntry(grroi1[1], "SiR");
  l->AddEntry(grroi1[2], "Avg");

  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  mgroi1->Draw("ALP");
  l->Draw();
  c->cd(2);
  mgroi2->Draw("ALP");
  l->Draw();
  c->SaveAs("img/compare_proton_tcut_unfolded.png");
}

void compare_proton_tcut_raw() {
  static const int N = 19;
  TFile* fs[N];
  TH1* hs[N][2];
  THStack* hst[2] = { new THStack("hls", "SiL Raw;E [keV];p/teff [/500keV]"),
                      new THStack("hrs", "SiR Raw;E [keV];p/teff [/500keV]") };
  double roi1[N][2], roi2[N][2];
  TGraphErrors* grroi1[3] = { new TGraphErrors(N), new TGraphErrors(N), new TGraphErrors(N) };
  TGraphErrors* grroi2[3] = { new TGraphErrors(N), new TGraphErrors(N), new TGraphErrors(N) };
  TFile* ifile = new TFile("data/Al50/psel.3layer.root");
  TTree* tr[2] = { (TTree*)ifile->Get("PID_LP"), (TTree*)ifile->Get("PID_RP") };
  for (int i = 0; i < N; ++i) {
    int t = (2+i)*100;
    char hname[2][16], cut[128], cmd[2][128];
    std::sprintf(hname[0], "hl%d", i);
    std::sprintf(hname[1], "hr%d", i);
    std::sprintf(cut,      "e3==0 && %d<t && t<10e3 && abs(dt)<200", t);
    std::sprintf(cmd[0],   "e>>%s(24,2e3,14e3)", hname[0]);
    std::sprintf(cmd[1],   "e>>%s(24,2e3,14e3)", hname[1]);
    tr[0]->Draw(cmd[0], cut, "goff");
    tr[1]->Draw(cmd[1], cut, "goff");
    hs[i][0] = (TH1*)gDirectory->Get(hname[0]);
    hs[i][1] = (TH1*)gDirectory->Get(hname[1]);
    hs[i][0]->Scale(1./(Normalization::TCutEfficiency(t)));
    hs[i][1]->Scale(1./(Normalization::TCutEfficiency(t)));
    double Emin = 2e3, Emax = 14e3;
    hs[i][0]->GetXaxis()->SetRangeUser(Emin, Emax);
    hs[i][1]->GetXaxis()->SetRangeUser(Emin, Emax);
    double roi1_err[2], roi2_err[2];
    roi1[i][0] = hs[i][0]->IntegralAndError(hs[i][0]->FindFixBin(3.5e3), hs[i][0]->FindFixBin(10e3), roi1_err[0]);
    roi1[i][1] = hs[i][1]->IntegralAndError(hs[i][1]->FindFixBin(3.5e3), hs[i][1]->FindFixBin(10e3), roi1_err[1]);
    roi2[i][0] = hs[i][0]->IntegralAndError(hs[i][0]->FindFixBin(4e3),   hs[i][0]->FindFixBin(8e3),  roi2_err[0]);
    roi2[i][1] = hs[i][1]->IntegralAndError(hs[i][1]->FindFixBin(4e3),   hs[i][1]->FindFixBin(8e3),  roi2_err[1]);
    grroi1[0]->SetPoint(i, t, roi1[i][0]);
    grroi1[1]->SetPoint(i, t, roi1[i][1]);
    grroi2[0]->SetPoint(i, t, roi2[i][0]);
    grroi2[1]->SetPoint(i, t, roi2[i][1]);
    grroi1[0]->SetPointError(i, 0, roi1_err[0]);
    grroi1[1]->SetPointError(i, 0, roi1_err[1]);
    grroi2[0]->SetPointError(i, 0, roi2_err[0]);
    grroi2[1]->SetPointError(i, 0, roi2_err[1]);
    hs[i][0]->SetLineColor(TColor::GetColor(0., 0., Float_t(i)/N));
    hs[i][1]->SetLineColor(TColor::GetColor(Float_t(i)/N, 0., 0.));
    hst[0]->Add(hs[i][0]);
    hst[1]->Add(hs[i][1]);
  }

  TMultiGraph* mgroi1 = new TMultiGraph("mgroi1", "3.5-10 MeV;Proton Time Cut [ns];Time Normalized Raw Count");
  TMultiGraph* mgroi2 = new TMultiGraph("mgroi2", "4-8 MeV;Proton Time Cut [ns];Time Normalized Raw Count");
  mgroi1->Add(grroi1[0]);
  mgroi1->Add(grroi1[1]);
  mgroi2->Add(grroi2[0]);
  mgroi2->Add(grroi2[1]);
  grroi1[0]->SetLineColor(kBlue);
  grroi1[1]->SetLineColor(kRed);
  grroi2[0]->SetLineColor(kBlue);
  grroi2[1]->SetLineColor(kRed);

  TLegend* l = new TLegend(0.1, 0.7, 0.3, 0.9);
  l->AddEntry(grroi1[0], "SiL");
  l->AddEntry(grroi1[1], "SiR");

  TLegend* leg[2] = { new TLegend(0.7, 0.7, 0.9, 0.9),
                      new TLegend(0.7, 0.7, 0.9, 0.9) };
  leg[0]->AddEntry(hs[0][0], "t>0ns");
  leg[0]->AddEntry(hs[N-1][0], "t>1600ns");
  leg[1]->AddEntry(hs[0][1], "t>0ns");
  leg[1]->AddEntry(hs[N-1][1], "t>1600ns");

  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1);
  mgroi1->Draw("ALP");
  l->Draw();
  c->cd(2);
  mgroi2->Draw("ALP");
  l->Draw();
  c->SaveAs("img/compare_proton_tcut_raw.png");

  TCanvas* cst = new TCanvas("cst", "cst", 1400, 500);
  cst->Divide(2);
  cst->cd(1);
  hst[0]->Draw("NOSTACK");
  leg[0]->Draw();
  cst->cd(2);
  hst[1]->Draw("NOSTACK");
  leg[1]->Draw();
  cst->SaveAs("img/compare_proton_tcut_raw_spectra.png");
}

// Command to make table
/*
for (int i = 0; i < 20; ++i) { int t = i*100; char cut[128]; sprintf(cut, "3500<e && e<10e3 && e3==0 && %d<t && t<10e3 && abs(dt)<200", t); int n = PID_RP->GetEntries(cut); double teff=Normalization::TCutEfficiency(t); printf("%dns\t%d\t%f\t%d\n", t, n, teff, (int)(n/teff)); }
*/

void compare_proton_tcut() {
  compare_proton_tcut_raw();
  // compare_proton_tcut_unfolded();
}
