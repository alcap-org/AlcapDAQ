#include "./util.h"

#include "TCanvas.h"
#include "TColor.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TText.h"
#include "TVirtualFitter.h"

#include <array>
#include <cmath>
#include <vector>

using std::array;
using std::vector;

array<double,2> ExtendEnergy(TH1* h) {
  double x1 = 5e3, x2 = 10e3;
  double y1 = h->GetBinContent(h->FindFixBin(x1));
  double y2 = h->GetBinContent(h->FindFixBin(x2));
  double par[2] = { (x1*std::log(y2)-x2*std::log(y1))/(x1-x2),
                    std::log(y1/y2)/(x1-x2) };
  TF1* f = new TF1("fspec", "expo", 5e3, 10e3);
  f->SetParameters(par);
  TFitResultPtr res = h->Fit(f, "SEMR");
  std::cout << "Chi2 of extension: " << res->Chi2() << "/" << res->Ndf() << std::endl;
  return array<double,2>{f->Integral(10e3, 150e3)/h->GetBinWidth(1),
                         f->IntegralError(10e3, 150e3)/h->GetBinWidth(1)};
}

void DrawExtendedEnergyRange(TH1* h) {
  TF1* ffit = (TF1*)h->FindObject("fspec");
  TF1* fext = (TF1*)ffit->Clone("fspecext");
  h->SetLineColor(kBlack);
  h->SetFillColor(kBlack);
  ffit->SetLineColor(kRed);
  ffit->SetLineStyle(1);
  fext->SetLineColor(kRed);
  fext->SetLineStyle(2);
  fext->SetRange(10e3, 20e3);

  TH1* hext95 = new TH1D("hext95", "hext95", 40, 10e3, 20e3);
  TH1* hext68 = new TH1D("hext68", "hext68", 40, 10e3, 20e3);
  hext95->SetFillColor(kGreen);
  hext68->SetFillColor(kYellow);
  hext95->SetFillStyle(3004);
  hext68->SetFillStyle(3004);
  hext95->SetStats(false);
  hext68->SetStats(false);
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(hext95, 0.95);
  (TVirtualFitter::GetFitter())->GetConfidenceIntervals(hext68, 0.68);

  TH1* hbg = new TH1D("hbg", "hbg", 36, 2e3, 20e3);
  hbg->SetTitle("Extrapolated Proton Emission Spectrum");
  hbg->GetXaxis()->SetTitle(h->GetXaxis()->GetTitle());
  hbg->GetYaxis()->SetTitle(h->GetYaxis()->GetTitle());
  hbg->GetYaxis()->SetTitleOffset(h->GetYaxis()->GetTitleOffset());
  hbg->SetMaximum(0.005);
  hbg->SetMinimum(0.0001);

  TLegend* leg = new TLegend(0.52, 0.52, 0.9, 0.9);
  leg->AddEntry(h, "Data (1#sigma errors)", "p");
  leg->AddEntry(ffit, "Exponential fit", "l");
  leg->AddEntry(hext68, "Extrapolated fit (68% C.L.)");
  leg->AddEntry(hext95, "Extrapolated fit (95% C.L.)");

  TCanvas* cext = new TCanvas("cext", "cext", 700, 500);
  hbg->Draw();
  hext95->Draw("E3 SAME");
  hext68->Draw("E3 SAME");
  h->Draw("SAME");
  leg->Draw("SAME");
  // fext->Draw("SAME");
  cext->SetLogy();
  cext->SaveAs("img/proton_e_reco_avg_extrap.png");
}

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
    // ASSUMES CORRECT DIMESNIONS
    for (int b = 1; b <= hl->GetNbinsX(); ++b) {
      double err[2] = {
        herr[i][0]->GetBinError(b), herr[i][1]->GetBinError(b)
      };
      hl->SetBinError(b, hl->GetBinContent(b)*std::sqrt(std::pow(hl->GetBinError(b)/hl->GetBinContent(b), 2)+err[0]*err[0]));
      hr->SetBinError(b, hr->GetBinContent(b)*std::sqrt(std::pow(hr->GetBinError(b)/hr->GetBinContent(b), 2)+err[1]*err[1]));
    }
  }
  // hl->Divide(herr[3][0]);
  // hr->Divide(herr[3][1]);
}

void r15b_al50_unfold_draw(const char* ifname, double prot_tcut=400., bool adderr=false, bool norm=false) {

  TFile* ifile       = new TFile(ifname);
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

  if (norm) {
    // hl->Scale(1./(Normalization::Al50(prot_tcut)));
    // hr->Scale(1./(Normalization::Al50(prot_tcut)));
    hl->Scale(1./(Normalization::Al100(prot_tcut)));
    hr->Scale(1./(Normalization::Al100(prot_tcut)));
  } else {
    hl->Scale(1./(Normalization::TCutEfficiency(prot_tcut)));
    hr->Scale(1./(Normalization::TCutEfficiency(prot_tcut)));
  }
  if (adderr) {
    CombineWithError(hl, hr);
  }
  TH1* ha = (TH1*)hl->Clone("ha_e_reco_bay");
  ha->Add(hr);
  ha->Scale(0.5);
  // PropagateFractionalError(hl, 0.075);
  // PropagateFractionalError(hr, 0.075);
  // PropagateFractionalError(ha, 0.075);
  // PropagateFractionalError(hl, 0.1); // nmu error ~10%?
  // PropagateFractionalError(hr, 0.1); // nmu error ~10%?

  hl->SetTitle("Unfolded Proton Spectrum;E [keV];Count [p/500 keV]");
  hr->SetTitle("Unfolded Proton Spectrum;E [keV];Count [p/500 keV]");
  if (norm)
    ha->SetTitle("Proton Emission Rate;E [keV];p/#mu-cap/500 keV");
  else
    ha->SetTitle("Unfolded Proton Spectrum;E [keV];Count [p/500 keV]");
  hl->GetYaxis()->SetTitleOffset(1.45);
  hr->GetYaxis()->SetTitleOffset(1.45);
  ha->GetYaxis()->SetTitleOffset(1.45);
  hl->SetLineColor(ACStyle::kACBlue);
  hr->SetLineColor(ACStyle::kACRed);
  ha->SetLineColor(kBlack);

  double Emin = 2e3, Emax = 12e3;
  hl->GetXaxis()->SetRangeUser(Emin, Emax);
  hr->GetXaxis()->SetRangeUser(Emin, Emax);
  ha->GetXaxis()->SetRangeUser(Emin, Emax);

  TF1* fl = new TF1("flp", "[0]*(1-[1]/x)^[2]*exp(-x/[3])", Emin, Emax);
  TF1* fr = new TF1("frp", "[0]*(1-[1]/x)^[2]*exp(-x/[3])", Emin, Emax);
  TF1* fa = new TF1("fap", "[0]*(1-[1]/x)^[2]*exp(-x/[3])", Emin, Emax);
  fl->SetParameters(0.05, 1500, 2, 2500);
  fr->SetParameters(0.05, 1500, 2, 2500);
  fa->SetParameters(0.05, 1500, 2, 2500);
  fl->SetLineColor(kBlue);
  fr->SetLineColor(kRed);
  fa->SetLineColor(kBlack);
  fl->SetParNames("A", "T_{th}", "#alpha", "T_{0}");
  fr->SetParNames("A", "T_{th}", "#alpha", "T_{0}");
  fa->SetParNames("A", "T_{th}", "#alpha", "T_{0}");

  TLegend* l = new TLegend(0.7, 0.7, 0.9, 0.9);
  l->AddEntry(hl, "Left");
  l->AddEntry(hr, "Right");
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas();
  // hr->Fit(fr);
  // hl->Fit(fl);
  hl->Draw("SAME");
  hr->Draw("SAME");
  l->Draw();
  char lrate_str[128], rrate_str[128];
  sprintf(lrate_str, "%.3g/cap", hl->Integral());
  sprintf(rrate_str, "%.3g/cap", hr->Integral());
  TText* lrate = new TText(4000, 0.002, lrate_str);
  TText* rrate = new TText(4000, 0.001, rrate_str);
  lrate->SetTextColor(kBlue);
  rrate->SetTextColor(kRed);
  // lrate->Draw();
  // rrate->Draw();
  // fr->Draw("SAME");
  // fl->Draw("SAME");

  c->SaveAs("img/proton_e_reco_lr.png");
  c->SaveAs("img/proton_e_reco_lr.root");

  TCanvas* cavg = new TCanvas();
  // for (int i = 1; i <= h->GetNbinsX(); ++i) {
  //   double xl = hl->GetBinContent(i);
  //   double xr = hr->GetBinContent(i);
  //   double el = hl->GetBinError(i);
  //   double er = hr->GetBinError(i);
  //   double mean = 0.5*(xl+xr);
  //   double sys_err  = sqrt(pow(mean-xr, 2) + pow(mean-xl, 2));
  //   double stat_err = sqrt(er*er+el*el);
  //   double err = sqrt(sys_err*sys_err+stat_err*stat_err);
  //   double err = stat_err;
  //   h->SetBinError(i, err);
  // }
  l->AddEntry(ha, "Average");
  // h->Fit(fa);

  char avgrate_str[128];
  sprintf(avgrate_str, "%.3g/cap", ha->Integral());
  TText* avgrate = new TText(6e3, 0.003, avgrate_str);
  avgrate->SetTextColor(kBlack);
  avgrate->SetTextSize(0.12);
  // cavg->SetLogy(true);
  // hl->Draw("SAME");
  // hr->Draw("SAME");
  if (adderr) {
    TH1* hasig1 = (TH1*)ha->Clone("hasig1");
    TH1* hasig2 = (TH1*)ha->Clone("hasig2");
    for (int i = 1; i <= hasig2->GetNbinsX(); ++i)
      hasig2->SetBinError(i, 2*hasig2->GetBinError(i));
    hasig1->SetFillColor(kYellow);
    hasig2->SetFillColor(kGreen);
    ha    ->SetMarkerStyle(kFullCircle);
    hasig2->GetXaxis()->SetRangeUser(2e3, 12e3);
    hasig2->SetMinimum(0);
    hasig2->Draw("E3");
    hasig1->Draw("E3 SAME");
    ha    ->Draw("HIST P SAME");
    TLegend* leg = new TLegend(0.65, 0.62, 0.95, 0.92);
    leg->AddEntry(ha,     "Rate",           "p");
    leg->AddEntry(hasig1, "68% Confidence", "f");
    leg->AddEntry(hasig2, "95% Confidence", "f");
    leg->Draw();
  } else {
    ha->Draw("SAME");
  }
  // l->Draw();
  // if (norm)
  //   DrawPreliminaryBox(5400., 0.0025);
  // else
  //   DrawPreliminaryBox(5250., 225e3);
  // avgrate->Draw();
  // fap->Draw("SAME");
  // cavg->SetLogy();
  cavg->SaveAs("img/proton_e_reco_avg.png");
  cavg->SaveAs("img/proton_e_reco_avg.root");

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


  array<double,2> nfit = ExtendEnergy(ha);
  DrawExtendedEnergyRange(ha);
  std::cout << nfit[0] << " " << nfit[1] << std::endl;
}
