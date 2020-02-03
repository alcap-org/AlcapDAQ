#include "./scripts/Al50/util.h"

#include "TCanvas.h"
#include "TColor.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TText.h"

#include <cmath>
#include <vector>

using std::vector;

void PropagateFractionalError(TH1* h, double err) {
  if (err < 0 || 1 < err) throw "Invalid fractional error!";
  for (int i = 1; i <= h->GetNbinsX(); ++i) {
    double n       = h->GetBinContent(i);
    double bin_err = h->GetBinError(i)/n;
    h->SetBinError(i, n*std::sqrt(bin_err*bin_err+err*err));
  }
}

void DrawPreliminaryBox(double x, double y) {
  TText* txt = new TText(x, y, "PRELIMINARY");
  int i = TColor::GetFreeColorIndex();
  TColor* col = new TColor(i, 204./256., 0./256., 0./256., "BUCOL");
  txt->SetTextColor(i);
  txt->SetTextSize(0.12);
  txt->Draw();
}

void CombineWithError(TH1* hl, TH1* hr) {
  vector<TFile*> ferr = {
    new TFile("data/Al50/corranderr/binwidth.root"),
    new TFile("data/Al50/corranderr/cutoff.root"),
    new TFile("data/Al50/corranderr/iterations.root"),
    new TFile("data/Al50/corranderr/pid.root"),
    new TFile("data/Al50/corranderr/siecal.root")
  };
  static const int N = ferr.size();
  TH1* herr[][2] = {
    { (TH1*)ferr[0]->Get("hl"),           (TH1*)ferr[0]->Get("hr")           },
    { (TH1*)ferr[1]->Get("hl"),           (TH1*)ferr[1]->Get("hr")           },
    { (TH1*)ferr[2]->Get("hl"),           (TH1*)ferr[2]->Get("hr")           },
    { (TH1*)ferr[3]->Get("hlpidcorrerr"), (TH1*)ferr[3]->Get("hrpidcorrerr") },
    { (TH1*)ferr[4]->Get("hl"),           (TH1*)ferr[4]->Get("hr")           }
  };
  for (int i = 0; i < N; ++i) {
    hl->Divide(herr[i][0]);
    hr->Divide(herr[i][1]);
  }
  // hl->Divide(herr[3][0]);
  // hr->Divide(herr[3][1]);
}

void finalspec() {

  double prot_tcut   = 400.;
  bool adderr        = true;
  bool norm          = true;

  TFile* ifile       = new TFile("data/Al50/en.p.t400ns.dt200ns.rebin5.pp10us.elo2e3keV.ehi12e3keV.bay5.pidcorr.root");
  TH1* hl            = (TH1*)ifile->Get("hl_e_reco_bay");
  TH1* hr            = (TH1*)ifile->Get("hr_e_reco_bay");

  TH1* ha_stat = (TH1*)hl->Clone("ha_stat_e_reco_bay");
  ha_stat->Add(hr);

  Double_t nl_all_err, nr_all_err, na_all_err, nl_0_10_err, nr_0_10_err,
  na_0_10_err, nl_2_10_err, nr_2_10_err, na_2_10_err, nl_35_10_err,
  nr_35_10_err, na_35_10_err, nl_4_8_err, nr_4_8_err, na_4_8_err;
  int nl_all   = hl->IntegralAndError(1, hl->GetNbinsX(), nl_all_err);
  int nr_all   = hr->IntegralAndError(1, hr->GetNbinsX(), nr_all_err);
  int nl_0_10  = hl->IntegralAndError(hl->FindFixBin(0),
                                      hl->FindFixBin(10e3)-1,
                                      nl_0_10_err);
  int nr_0_10  = hr->IntegralAndError(hr->FindFixBin(0),
                                      hr->FindFixBin(10e3)-1,
                                      nr_0_10_err);
  int na_0_10  = ha_stat->IntegralAndError(ha_stat->FindFixBin(0),
                                      ha_stat->FindFixBin(10e3)-1,
                                      na_0_10_err);
  int nl_2_10  = hl->IntegralAndError(hl->FindFixBin(2e3),
                                      hl->FindFixBin(10e3)-1,
                                      nl_2_10_err);
  int nr_2_10  = hr->IntegralAndError(hr->FindFixBin(2e3),
                                      hr->FindFixBin(10e3)-1,
                                      nr_2_10_err);
  int nl_35_10 = hl->IntegralAndError(hl->FindFixBin(3.5e3),
                                      hl->FindFixBin(10e3)-1,
                                      nl_35_10_err);
  int nr_35_10 = hr->IntegralAndError(hr->FindFixBin(3.5e3),
                                      hr->FindFixBin(10e3)-1,
                                      nr_35_10_err);
  int na_35_10 = ha_stat->IntegralAndError(ha_stat->FindFixBin(3.5e3),
                                           ha_stat->FindFixBin(10e3)-1,
                                           na_35_10_err);
  int nl_4_8   = hl->IntegralAndError(hl->FindFixBin(4e3),
                                      hl->FindFixBin(8e3)-1,
                                      nl_4_8_err);
  int nr_4_8   = hr->IntegralAndError(hr->FindFixBin(4e3),
                                      hr->FindFixBin(8e3)-1,
                                      nr_4_8_err);

  hl->Scale(1./(Normalization::Al50(prot_tcut)));
  hr->Scale(1./(Normalization::Al50(prot_tcut)));
  CombineWithError(hl, hr);
  TH1* ha = (TH1*)hl->Clone("ha_e_reco_bay");
  ha->Add(hr);
  ha->Scale(0.5);

  ha->SetTitle("Proton Emission Rate;E [keV];p/#mu-cap/500 keV");
  ha->SetLineColor(kBlack);

  double Emin = 2e3, Emax = 12e3;
  ha->GetXaxis()->SetRangeUser(Emin, Emax);

  TF1* fa = new TF1("fap", "[0]*(1-[1]/x)^[2]*exp(-x/[3])", Emin, Emax);
  fa->SetParameters(0.05, 1500, 2, 2500);
  fa->SetLineColor(kBlack);
  fa->SetParNames("A", "T_{th}", "#alpha", "T_{0}");

  gStyle->SetOptStat(0);

  TCanvas* cavg = new TCanvas();
  cavg->SetLeftMargin(0.12);

  char avgrate_str[128];
  sprintf(avgrate_str, "%.3g/cap", ha->Integral());
  TText* avgrate = new TText(6e3, 0.003, avgrate_str);
  avgrate->SetTextColor(kBlack);
  avgrate->SetTextSize(0.12);
  // cavg->SetLogy(true);
  // hl->Draw("SAME");
  // hr->Draw("SAME");
  TH1* hasig1 = (TH1*)ha->Clone("hasig1");
  TH1* hasig2 = (TH1*)ha->Clone("hasig2");
  for (int i = 1; i <= hasig2->GetNbinsX(); ++i)
    hasig2->SetBinError(i, 2*hasig2->GetBinError(i));
  hasig1->SetFillColor(kYellow);
  hasig2->SetFillColor(kGreen);
  ha    ->SetMarkerStyle(kFullCircle);
  hasig2->GetXaxis()->SetTitleSize(0.05);
  hasig2->GetYaxis()->SetTitleSize(0.05);
  hasig2->GetXaxis()->SetTitleOffset(0.90);
  hasig2->GetYaxis()->SetTitleOffset(1.1);
  hasig2->Draw("E4");
  hasig1->Draw("E4 SAME");
  ha    ->Draw("HIST P SAME");
  TLegend* leg = new TLegend(0.80, 0.62, 0.95, 0.92);
  leg->AddEntry(ha,     "Rate",    "p");
  leg->AddEntry(hasig1, "1#sigma", "f");
  leg->AddEntry(hasig2, "2#sigma", "f");
  leg->Draw();
  // l->Draw();
  // DrawPreliminaryBox(5400., 0.0025);
  // avgrate->Draw();
  // fap->Draw("SAME");
  // cavg->SetLogy();
  cavg->SaveAs("img/psi2019/finalspec.png");

  Double_t r_all_err, rl_all_err, rr_all_err, r_0_10_err, rl_0_10_err,
  rr_0_10_err, r_2_10_err, rl_2_10_err, rr_2_10_err, r_35_10_err, rl_35_10_err,
  rr_35_10_err, r_4_8_err, rl_4_8_err, rr_4_8_err;
  double r_all    = ha->IntegralAndError(1, ha->GetNbinsX(), r_all_err);
  double rl_all   = hl->IntegralAndError(1, hl->GetNbinsX(), rl_all_err);
  double rr_all   = hr->IntegralAndError(1, hr->GetNbinsX(), rr_all_err);
  double r_0_10   = ha->IntegralAndError(ha->FindFixBin(0),
                                         ha->FindFixBin(10e3)-1,
                                         r_0_10_err);
  double rl_0_10  = hl->IntegralAndError(hl->FindFixBin(0),
                                         hl->FindFixBin(10e3)-1,
                                         rl_0_10_err);
  double rr_0_10  = hr->IntegralAndError(hr->FindFixBin(0),
                                         hr->FindFixBin(10e3)-1,
                                         rr_0_10_err);
  double r_2_10   = ha->IntegralAndError(ha->FindFixBin(2e3),
                                         ha->FindFixBin(10e3)-1,
                                         r_2_10_err);
  double rl_2_10  = hl->IntegralAndError(hl->FindFixBin(2e3),
                                         hl->FindFixBin(10e3)-1,
                                         rl_2_10_err);
  double rr_2_10  = hr->IntegralAndError(hr->FindFixBin(2e3),
                                         hr->FindFixBin(10e3)-1,
                                         rr_2_10_err);
  double r_35_10  = ha->IntegralAndError(ha->FindFixBin(3.5e3),
                                         ha->FindFixBin(10e3)-1,
                                         r_35_10_err);
  double rl_35_10 = hl->IntegralAndError(hl->FindFixBin(3.5e3),
                                         hl->FindFixBin(10e3)-1,
                                         rl_35_10_err);
  double rr_35_10 = hr->IntegralAndError(hr->FindFixBin(3.5e3),
                                         hr->FindFixBin(10e3)-1,
                                         rr_35_10_err);
  double r_4_8    = ha->IntegralAndError(ha->FindFixBin(4e3),
                                         ha->FindFixBin(8e3)-1,
                                         r_4_8_err);
  double rl_4_8   = hl->IntegralAndError(hl->FindFixBin(4e3),
                                         hl->FindFixBin(8e3)-1,
                                         rl_4_8_err);
  double rr_4_8   = hr->IntegralAndError(hr->FindFixBin(4e3),
                                         hr->FindFixBin(8e3)-1,
                                         rr_4_8_err);

  printf("Total rate:     %.4g+/-%.3g/cap\n", r_all,    r_all_err);
  printf("Total rate (R): %.4g+/-%.3g/cap\n", rr_all,   rr_all_err);
  printf("Total rate (L): %.4g+/-%.3g/cap\n", rl_all,   rl_all_err);
  printf("0-10 MeV:       %.4g+/-%.3g/cap\n", r_0_10,   r_0_10_err);
  printf("0-10 MeV (R):   %.4g+/-%.3g/cap\n", rr_0_10,  rr_0_10_err);
  printf("0-10 MeV (L):   %.4g+/-%.3g/cap\n", rl_0_10,  rl_0_10_err);
  printf("2-10 MeV:       %.4g+/-%.3g/cap\n", r_2_10,   r_2_10_err);
  printf("2-10 MeV (R):   %.4g+/-%.3g/cap\n", rr_2_10,  rr_2_10_err);
  printf("2-10 MeV (L):   %.4g+/-%.3g/cap\n", rl_2_10,  rl_2_10_err);
  printf("3.5-10 MeV:     %.4g+/-%.3g/cap\n", r_35_10,  r_35_10_err);
  printf("3.5-10 MeV (R): %.4g+/-%.3g/cap\n", rr_35_10, rr_35_10_err);
  printf("3.5-10 MeV (L): %.4g+/-%.3g/cap\n", rl_35_10, rl_35_10_err);
  printf("4-8 MeV:        %.4g+/-%.3g/cap\n", r_4_8,    r_4_8_err);
  printf("4-8 MeV (R):    %.4g+/-%.3g/cap\n", rr_4_8,   rr_4_8_err);
  printf("4-8 MeV (L):    %.4g+/-%.3g/cap\n", rl_4_8,   rl_4_8_err);
  printf("Count:\t\tLeft\tRight\n");
  printf("All:\t\t%d (%d)\t%d (%d)\n",      nl_all,   (int)nl_all_err,   nr_all,   (int)nr_all_err);
  printf("0-10 MeV:\t%d (%d)\t%d (%d)\n",   nl_0_10,  (int)nl_0_10_err,  nr_0_10,  (int)nr_0_10_err);
  printf("2-10 MeV:\t%d  (%d)\t%d (%d)\n",  nl_2_10,  (int)nl_2_10_err,  nr_2_10,  (int)nr_2_10_err);
  printf("3.5-10 MeV:\t%d (%d)\t%d (%d)\n", nl_35_10, (int)nl_35_10_err, nr_35_10, (int)nr_35_10_err);
  printf("4-8 MeV:\t%d (%d)\t%d (%d)\n",    nl_4_8,   (int)nl_4_8_err,   nr_4_8,   (int)nr_4_8_err);

  std::cout << "Proton stat error 3.5-10MeV average: " << na_35_10_err/na_35_10 << std::endl;
}
