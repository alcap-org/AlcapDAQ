#include "TCanvas.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TText.h"

// p = 1.02, nmu_hi = 121e6
// p = 1.03, nmu_hi = 30.3e6

void r15b_al50_unfold_draw(const char* ifname) {

  const char* IFNAME = "~/data/R15b/enal50p_102.root";
  TFile* ifile       = new TFile(IFNAME);
  TH1* hl            = (TH1*)ifile->Get("hl_e_reco");
  TH1* hr            = (TH1*)ifile->Get("hr_e_reco");

  double nmu_hi  = 121e6;  // 1.02
  // double nmu_hi  = 30.3e6; // 1.03
  // double nmu_lo  = 9.26e7;
  double enmu_hi = 2.2e6; // NEEDS TO BE SET FOR EACH MOM. DATASET
  // double enmu_lo = 0.15e7;
  double caprate = 0.609;
  // double nmu     = 0.5*(nmu_hi+nmu_lo);
  // double enmu    = sqrt(enmu_hi*enmu_hi+enmu_lo*enmu_lo)/2;
  TF1* ftime = new TF1("f", "exp([0]^2/(2*[1]^2)-x/[1])*erfc(([0]^2-[1]*x)/(sqrt(2)*[0]*[1]))", 0, 10000);
  ftime->SetParameters(52.7, 864.);
  double tcuteff = ftime->Integral(400, 1e6)/ftime->Integral(0, 1e6);
  // tcuteff = 1.;

  hl->Scale(1./(nmu_hi*caprate*tcuteff));
  hr->Scale(1./(nmu_hi*caprate*tcuteff));

  hl->SetLineColor(kBlue);
  hr->SetLineColor(kRed);

  hl->GetXaxis()->SetRangeUser(2.0e3, 15e3);
  hr->GetXaxis()->SetRangeUser(2.0e3, 15e3);

  TF1* fl = new TF1("flp", "[0]*(1-[1]/x)^[2]*exp(-x/[3])", 0, 15e3);
  TF1* fr = new TF1("frp", "[0]*(1-[1]/x)^[2]*exp(-x/[3])", 0, 15e3);
  TF1* fa = new TF1("fap", "[0]*(1-[1]/x)^[2]*exp(-x/[3])", 0, 15e3);
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
  hl->SetTitle("Reconstructed Proton Spectrum;E [keV];Counts [/(200 keV)]");
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

  printf("Total rate: %.3g/cap\n", h->Integral());
  printf("0-10 MeV:   %.3g/cap\n", h->Integral(h->FindFixBin(0),
                                               h->FindFixBin(10e3)));
  printf("4-8 MeV:    %.3g/cap\n", h->Integral(h->FindFixBin(4e3),
                                               h->FindFixBin(8e3)));
}
