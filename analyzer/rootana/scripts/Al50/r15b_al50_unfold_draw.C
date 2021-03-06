#include "TCanvas.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TText.h"

// Are these wrong?
// p = 1.02, nmu_hi = 121e6
// p = 1.03, nmu_hi = 30.3e6
// all,      nmu_hi = 151.3e6

void r15b_al50_unfold_draw(const char* ifname) {

  TFile* ifile       = new TFile(ifname);
  TH1* hl            = (TH1*)ifile->Get("hl_e_reco_bay");
  TH1* hr            = (TH1*)ifile->Get("hr_e_reco_bay");

  // double nmu_hi = 109e6; // 1.02 pp
  // double nmu_hi = 27.2e6; // 1.03 pp
  double nmu_hi = 137e6;  // All Al50
  // double nmu_hi = 105e6;  // Al100
  double enmu_hi = 0.1*nmu_hi; // ? NEEDS TO BE SET FOR EACH MOM. DATASET
  double caprate = 0.609;
  TF1* ftime = new TF1("f", "exp([0]^2/(2*[1]^2)-x/[1])*TMath::Erfc(([0]^2-[1]*x)/(sqrt(2)*[0]*[1]))", 0, 10000);
  ftime->SetParameters(52.7, 864.);
  double tcuteff = ftime->Integral(400, 1e6)/ftime->Integral(0, 1e6); // Al50
  // double tcuteff = ftime->Integral(100, 1e6)/ftime->Integral(0, 1e6); // Al100
  // double tcuteff = 1.;                                                // Al100


  int nl_all   = hl->Integral();
  int nr_all   = hr->Integral();
  int nl_0_10  = hl->Integral(hl->FindFixBin(0),     hl->FindFixBin(10e3));
  int nr_0_10  = hr->Integral(hr->FindFixBin(0),     hr->FindFixBin(10e3));
  int nl_2_10  = hl->Integral(hl->FindFixBin(2e3),   hl->FindFixBin(10e3));
  int nr_2_10  = hr->Integral(hr->FindFixBin(2e3),   hr->FindFixBin(10e3));
  int nl_35_10 = hl->Integral(hl->FindFixBin(3.5e3), hl->FindFixBin(10e3));
  int nr_35_10 = hr->Integral(hr->FindFixBin(3.5e3), hr->FindFixBin(10e3));
  int nl_4_8   = hl->Integral(hl->FindFixBin(4e3),   hl->FindFixBin(8e3));
  int nr_4_8   = hr->Integral(hr->FindFixBin(4e3),   hr->FindFixBin(8e3));

  hl->Scale(1./(nmu_hi*caprate*tcuteff));
  hr->Scale(1./(nmu_hi*caprate*tcuteff));

  hl->SetLineColor(kBlue);
  hr->SetLineColor(kRed);

  double Emin = 2e3, Emax = 14e3;
  hl->GetXaxis()->SetRangeUser(Emin, Emax);
  hr->GetXaxis()->SetRangeUser(Emin, Emax);

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
  hl->SetTitle("Reconstructed Proton Spectrum;E [keV];Counts [/(500 keV)]");
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas();
  // hr->Fit(fr);
  // hl->Fit(fl);
  hr->Draw();
  hl->Draw("SAME"); l->Draw();
  char lrate_str[128], rrate_str[128];
  sprintf(lrate_str, "%.3g/cap", hl->Integral());
  sprintf(rrate_str, "%.3g/cap", hr->Integral());
  TText* lrate = new TText(4000, 0.002, lrate_str);
  TText* rrate = new TText(4000, 0.001, rrate_str);
  lrate->SetTextColor(kBlue);
  rrate->SetTextColor(kRed);
  lrate->Draw();
  rrate->Draw();
  // fr->Draw("SAME");
  // fl->Draw("SAME");

  c->SaveAs("img/proton_e_reco_lr.png");
  c->SaveAs("img/proton_e_reco_lr.root");

  TCanvas* cavg = new TCanvas();
  TH1* h = (TH1*)hl->Clone("havg");
  h->Add(hr);
  h->Scale(0.5);
  for (int i = 1; i <= h->GetNbinsX(); ++i) {
    double xl = hl->GetBinContent(i);
    double xr = hr->GetBinContent(i);
    double el = hl->GetBinError(i);
    double er = hr->GetBinError(i);
    double mean = 0.5*(xl+xr);
    double sys_err  = sqrt(pow(mean-xr, 2) + pow(mean-xl, 2));
    double stat_err = sqrt(er*er+el*el);
    double err = sqrt(sys_err*sys_err+stat_err*stat_err);
    h->SetBinError(i, err);
  }
  h->SetMinimum(0);
  h->SetLineColor(kBlack);
  // h->Fit(fa);

  char avgrate_str[128];
  sprintf(avgrate_str, "%.3g/cap", h->Integral());
  TText* avgrate = new TText(6e3, 0.003, avgrate_str);
  avgrate->SetTextColor(kBlack);
  avgrate->SetTextSize(0.12);
  cavg->SetLogy(false);
  h->Draw();
  avgrate->Draw();
  // fap->Draw("SAME");
  cavg->SaveAs("img/proton_e_reco_avg.png");
  cavg->SaveAs("img/proton_e_reco_avg.root");

  printf("Total rate:     %.3g/cap\n", h ->Integral());
  printf("Total rate (R): %.3g/cap\n", hr->Integral());
  printf("Total rate (L): %.3g/cap\n", hl->Integral());
  printf("0-10 MeV:       %.3g/cap\n", h ->Integral(h ->FindFixBin(0),
                                                    h ->FindFixBin(10e3)));
  printf("0-10 MeV (R):   %.3g/cap\n", hr->Integral(hr->FindFixBin(0),
                                                    hr->FindFixBin(10e3)));
  printf("0-10 MeV (L):   %.3g/cap\n", hl->Integral(hl->FindFixBin(0),
                                                    hl->FindFixBin(10e3)));
  printf("2-10 MeV:       %.3g/cap\n", h ->Integral(h ->FindFixBin(2e3),
                                                    h ->FindFixBin(10e3)));
  printf("2-10 MeV (R):   %.3g/cap\n", hr->Integral(hr->FindFixBin(2e3),
                                                    hr->FindFixBin(10e3)));
  printf("2-10 MeV (L):   %.3g/cap\n", hl->Integral(hl->FindFixBin(2e3),
                                                    hl->FindFixBin(10e3)));
  printf("3.5-10 MeV:     %.3g/cap\n", h ->Integral(h ->FindFixBin(3.5e3),
                                                    h ->FindFixBin(10e3)));
  printf("3.5-10 MeV (R): %.3g/cap\n", hr->Integral(hr->FindFixBin(3.5e3),
                                                    hr->FindFixBin(10e3)));
  printf("3.5-10 MeV (L): %.3g/cap\n", hl->Integral(hl->FindFixBin(3.5e3),
                                                    hl->FindFixBin(10e3)));
  printf("4-8 MeV:        %.3g/cap\n", h ->Integral(h ->FindFixBin(4e3),
                                                    h ->FindFixBin(8e3)));
  printf("4-8 MeV (R):    %.3g/cap\n", hr->Integral(hr->FindFixBin(4e3),
                                                    hr->FindFixBin(8e3)));
  printf("4-8 MeV (L):    %.3g/cap\n", hl->Integral(hl->FindFixBin(4e3),
                                                    hl->FindFixBin(8e3)));
  printf("Count:\t\tLeft\tRight\n");
  printf("All:\t\t%d\t%d\n",      nl_all,   nr_all);
  printf("0-10 MeV:\t%d\t%d\n",   nl_0_10,  nr_0_10);
  printf("2-10 MeV:\t%d\t%d\n",   nl_2_10,  nr_2_10);
  printf("3.5-10 MeV:\t%d\t%d\n", nl_35_10, nr_35_10);
  printf("4-8 MeV:\t%d\t%d\n",    nl_4_8,   nr_4_8);
}
