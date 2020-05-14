#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TMath.h"
#include "TStyle.h"
#include "TTimeStamp.h"
#include "TSpectrum.h"

#include <algorithm>
#include <cmath>
#include <tuple>

struct Peak_t {
  static constexpr long double RT2PI = 2.50662827463100050241;
  double  x,  amp,  sig;
  double ex, eamp, esig;
  friend bool operator<(const Peak_t&, const Peak_t&);
  double Integral() const {
    return RT2PI*amp*sig;
  }
  double ErrIntegral() const {
    return Integral()*std::sqrt(std::pow(esig/sig, 2)+std::pow(eamp/amp, 2));
  }
};

bool operator<(const Peak_t& l, const Peak_t& r) {
  return std::tie(l.x, l.amp, l.sig) < std::tie(r.x, r.amp, r.sig);
}

void ge_calib() {
  // Run 10320

  // const double NDECAYS = 1582250645.71; // Using ODB and pic of source 41

  // Activity
  unsigned int src_date[]  = {1,   3, 2000}; // 1 March 2000?
  unsigned int meas_date[] = {22, 11, 2015}; // 22 Nov 2015
  double halflife = 13.537;
  double activity0 = 47.8e3; // RunPSI2015:273
  TTimeStamp t0(src_date [2], src_date [1], src_date [0], 0, 0, 0);
  TTimeStamp t (meas_date[2], meas_date[1], meas_date[0], 0, 0, 0);
  double dt = (t.GetSec() - t0.GetSec())/60./60./24./365.2425;
  double activity = activity0*TMath::Power(0.5, dt/halflife);
  double livetime = 30301*0.098; // nblock*block_length(s)
  double NEVT     = activity*livetime;

  std::cout << activity << " " << livetime << std::endl;

  static const int N = 10;
  double E [N]  = { 244.6975, 344.2785, 411.1163, 443.965, 778.9040, 867.378, 964.079, 1085.869, 1112.074, 1408.006 };
  double eE[N]  = {   0.0008,   0.0012,   0.0011,   0.003,   0.0018,   0.004,   0.018,    0.024,    0.004,    0.003 };
  double I [N]  = {    7.583,     26.5,    2.234,   3.148,   12.942,   4.245,  14.605,   10.207,   13.644,   21.005 }; // Check 443 intensity!
  double eI[N]  = {    0.019,      0.4,    0.004,   0.019,    0.019,   0.019,   0.021,    0.021,    0.021,    0.024 };
  // double A [N]  = {  259.275,  571.948,  37.8172, 47.3987,  85.5331, 21.4432, 66.1893,  40.8962,   50.374,  54.0642 };
  // double eA[N]  = {      6.6,      8.9,      2.5,     2.7,      2.8,     1.5,     2.2,      1.6,      1.8,      1.7 };
  // double S [N] = {   4.6207,  4.94818,  5.39726, 5.52389,  7.56013, 8.57254,  8.8625,  10.8543,  10.4427,  11.0932 };
  // double eS[N] = {     0.10,    0.056,     0.33,    0.28,     0.19,    0.56,    0.20,     0.35,     0.29,     0.24 };
  // TF1* fspec[N];
  // double V[N], eV[N], Eff[N], eEff[N];
  // for (int i = 0; i < N; ++i) {
  //   fspec[i] = new TF1("", "gaus(0)");
  //   fspec[i]->SetParameters(A[i], E[i], S[i]);
  //   fspec[i]->SetParError(0, eA[i]);
  //   fspec[i]->SetParError(1, eE[i]);
  //   fspec[i]->SetParError(2, eS[i]);
  //   V [i] = fspec[i]->Integral(-10e3, 10e3);
  //   eV[i] = V[i]*sqrt(eA[i]*eA[i]/(A[i]*A[i])+eS[i]*eS[i]/(S[i]*S[i]));
  //   Eff [i] = V[i]/(NDECAYS*I[i]*0.01);
  //   eEff[i] = Eff[i]*eV[i]/V[i];
  // }
  // TGraph* greff = new TGraphErrors(N, E, Eff, eE, eEff);
  // TF1*    feff  = new TF1("feff", "expo(0)");
  // greff->Fit(feff);
  // greff->Draw("AP");
  TFile* f     = new TFile("data/GeCalib/out10319.root");
  TH1* h       = (TH1*)f->Get("PlotTAP_Amplitude/hGeHiGain#FirstComplete_RoughSyncPulseCut#{constant_fraction=0.20}{no_time_shift=true}_Amplitude");
  h->GetXaxis()->SetRangeUser(1e3, 8.5e3);
  TSpectrum* s = new TSpectrum(10);
  s->Search(h, 3, "", 0.007);

  TF1* fitfnc[N];
  TFitResultPtr res[N];
  Peak_t ps[N];
  for (int i = 0; i < N; ++i) {
    fitfnc[i] = new TF1("", "gaus(0)+pol1(3)");
    double x  = s->GetPositionX()[i], y = s->GetPositionY()[i];
    double y0 = h->GetBinContent(h->FindFixBin(x)-25);
    fitfnc[i]->SetParameters(y-y0, x, 5, y0, 0);
    h->GetXaxis()->SetRangeUser(x-25, x+25);
    res[i] = h->Fit(fitfnc[i], "SME+");

    ps[i] = {
      res[i]->Parameter(1), res[i]->Parameter(0), res[i]->Parameter(2),
      res[i]->ParError(1),  res[i]->ParError(0),  res[i]->ParError(2)
    };

  }
  std::sort(ps, ps+N);
  double X[N], eX[N], e[N], ee[N];
  for (int i = 0; i < N; ++i) {
    X [i] = ps[i].x;
    eX[i] = ps[i].ex;
    e [i] = ps[i].Integral()/(NEVT*I[i]*0.01);
    ee[i] = std::sqrt(std::pow(ps[i].ErrIntegral()/ps[i].Integral(), 2) +
                      1/NEVT + std::pow(eI[i]/I[i], 2)) * e[i];
    std::cout << ps[i].ErrIntegral()/ps[i].Integral() << std::endl;
  }


  TGraphErrors* gen  = new TGraphErrors(N, X, E, eX, eE);
  TGraphErrors* geff = new TGraphErrors(N, E, e, eE, ee);

  gen ->SetTitle("^{152}Eu Ge Energy Calibration;ADC Value;Peak Energy [keV]");
  geff->SetTitle("^{152}Eu Ge Efficiency Calibration;Peak Energy [keV];#epsilon=#gamma Counts/Expected");
  geff->GetYaxis()->SetTitleOffset(1.5);

  TF1* fen  = new TF1("fen",  "pol1(0)");
  TF1* feff = new TF1("feff", "[0]/(x^[1])");
  feff->SetParameters(9.23, 0.95712);
  TFitResultPtr res_en  = gen ->Fit(fen,  "MESF");
  TFitResultPtr res_eff = geff->Fit(feff, "MES");

  TLatex* txt_en  = new TLatex(5e3, 400,    "E=p0+p1*ADC");
  TLatex* txt_eff = new TLatex(800, 0.7e-3, "#epsilon=p0*E^{-p1}");

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(111);
  gStyle->SetStatX(0.7);
  TCanvas* c = new TCanvas("c", "c", 1400, 500);
  c->Divide(2);
  c->cd(1); gen ->Draw("AP"); txt_en ->Draw();
  c->cd(2); geff->Draw("AP"); txt_eff->Draw();
  c->SaveAs("img/thesis/5/ge_calib.png");

  h->GetXaxis()->SetRangeUser(1e3, 8e3);
  h->SetTitle("^{152}Eu Energy Spectrum;Pulse Amplitude (ADC Value);Counts");

  gStyle->SetOptFit(0);
  h->SetStats(false);
  TCanvas* c2 = new TCanvas("c2", "c2", 700, 500);
  h->Draw();
  c2->SaveAs("img/thesis/5/ge_calib_spec.png");

}
