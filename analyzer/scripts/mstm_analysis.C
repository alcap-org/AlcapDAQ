#include <iostream>
#include <cmath>
#include <sstream>

#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TFile.h"
#include "TAxis.h"
#include "TCanvas.h"

using std::pow;
using std::sqrt;
using std::vector;

const double ADCRESOLUTION = 6.;
const double CAPT = 0.609;
const double E[3] = {346.828, 843.74, 1808.73};
const double I[3] = {0.798, CAPT*0.13*0.718, CAPT*0.51};
const double TWINDOW[3][2] = { { 1625., 1750. },
			       { 1625., 4317. },
			       { -1., -1. } };
const char* HISTNAME[3] = { "tdist347keV", "tdist844keV", "tdist1809keV" };

class Run {
 private:
  int number;

  string filename() {
    stringstream fname;
    fname << "/home/jrquirk/tmp/hist/Al4/hist0" << number << ".root";
    return fname.str();
  }

 public:
  Run(int n) : number(n) {
  }
  const TH1* Spectrum_E() {
    TFile f(filename().c_str(), "READ");
    TH1* h;
    f.GetObject("DQ_Amplitude/hDQ_Amplitude_GeCHEH_D406_ped_sub", h);
    h->SetDirectory(0);
    f.Close();
    return h;
  }
  const TH2* Spectrum_TVsE() {
    TFile f(filename().c_str(), "READ");
    TH2* h;
    f.GetObject("TTScTCorrWFD/hTTScTCorrWFD_D406_E", h);
    h->SetDirectory(0);
    f.Close();
    return h;
  }
};


double E2ADC(double e) {
  const double a = 6.85761e-1;
  const double b = 6.58311;
  const double c = -1.19382e-5;
  return a+b*e+c*e*e;
}

double ADC2E(double adc) {
  const double a = 6.85761e-1;
  const double b = 6.58311;
  const double c = -1.19382e-5;
  return (sqrt(-4.*a*c+b*b+4*c*adc)-b)/(2*c);
}

double Efficiency(double e) {
  const double amp  = 1.46033e-1;
  const double rate = -9.22775e-1;
  return amp*pow(e, rate);
}

vector<double> FitGaussOverLine(TH1* h1, double e) {
  TF1 fit("fit", "gaus(0)+pol1(3)");

  double mu = E2ADC(e);
  double ped = h1->GetBinContent((int)mu-25);
  double amp = h1->GetBinContent(mu) - ped;
  fit.SetParameters(amp, mu, ADCRESOLUTION, ped, 0.);
  h1->GetXaxis()->SetRangeUser((int)mu-25., (int)mu+25.);
  h1->Fit(&fit, "ME");

  const int nparam = 5;
  vector<double> params;
  params.reserve(nparam);
  for (int i = 0; i < nparam; ++i)
    params.push_back(fit.GetParameter(i));
  return params;
}

double Count(TH1* h1, double e) {
  vector<double> params = FitGaussOverLine(h1, e);
  const double rt2pi = sqrt(2.*TMath::Pi());
  return rt2pi*params[0]*params[2]/Efficiency(e);
}

double Count(const TH2* h2, double e, const double t[2]) {
  TH1* h1 = h2->ProjectionY("", t[0], t[1]);
  double n = Count(h1, e);
  delete h1;
  return n;
}

double Rate(TH1* h1, double e) {
  static const double nmu = Count(h1, E[0])/I[0];
  double r = Count(h1, e)/nmu;
  return r;
}

double S2N(const TH2* h2, double e, const double t[2]) {
  TH1* h1;
  if (t[0] == t[1]) {
    if (t[0] > 0.)
      h1 = h2->ProjectionY("", h2->GetNbinsX()+1, h2->GetNbinsX()+1);
    else
      h1 = h2->ProjectionY("", 0, 0);
  } else {
    const TAxis* x = h2->GetXaxis();
    h1 = h2->ProjectionY("", x->FindFixBin(t[0]), x->FindFixBin(t[1]));
  }
  vector<double> params = FitGaussOverLine(h1, e);
  { // TMP
    TF1 f("f", "gaus(0)+pol1(3)", 0, 100000.);
    f.SetParameters(params[0], params[1], params[2], params[3], params[4]);
    h1->DrawCopy();
    f.DrawCopy("SAME");
    new TCanvas;
  } // TMP
  delete h1;

  int adc_i = params[1]-3*params[2];
  int adc_f = params[1]+3*params[2];
  TF1 pk_fnc("pk_fnc", "gaus(0)");
  TF1 bg_fnc("bg_fnc", "pol1(0)");
  pk_fnc.SetParameters(params[0], params[1], params[2]);
  bg_fnc.SetParameters(params[3], params[4]);
  double peak = pk_fnc.Integral(adc_i, adc_f);
  double bg   = bg_fnc.Integral(adc_i, adc_f);

  std::cout << "E Peak/BG:\t" << e << " " << peak << "/" << bg << std::endl;

  return peak/bg;
}

TH1* TimeDistribution(const TH2* h2, double e, double de) {
  return (TH1*)h2->ProjectionX("", (int)E2ADC(e-de), (int)E2ADC(e+de));
}

vector<Run> construct_runs() {
  const int nr = 148;
  const int runlist_array[nr] =
    { 6653, 6654, 6655, 6656, 6712, 6713, 6714, 6715, 6716, 6717,
      6718, 6719, 6720, 6723, 6724, 6725, 6726, 6727, 6728, 6729,
      6730, 6731, 6732, 6733, 6734, 6735, 6736, 6737, 6740, 6742,
      6743, 6744, 6745, 6746, 6747, 6748, 6749, 6750, 6751, 6752,
      6753, 6755, 6756, 6757, 6758, 6759, 6760, 6761, 6762, 6763,
      6764, 6765, 6766, 6767, 6768, 6769, 6770, 6771, 6772, 6773,
      6774, 6775, 6776, 6777, 6778, 6779, 6780, 6781, 6782, 6784,
      6785, 6786, 6787, 6788, 6789, 6790, 6791, 6792, 6793, 6797,
      6798, 6799, 6800, 6801, 6802, 6803, 6805, 6806, 6807, 6808,
      6809, 6810, 6811, 6812, 6813, 6814, 6815, 6816, 6817, 6818,
      6820, 6821, 6822, 6823, 6824, 6825, 6826, 6851, 6852, 6853,
      6854, 6855, 6856, 6857, 6858, 6859, 6878, 6879, 6880, 6881,
      6882, 6883, 6884, 6885, 6886, 6887, 6888, 6889, 6892, 6893,
      6894, 6895, 6896, 6897, 6898, 6899, 6900, 6901, 6902, 6903,
      7311, 7312, 7313, 7314, 7315, 7316, 7317, 7318 };
  vector<Run> runs;
  runs.reserve(nr);
  for (int i = 0; i < nr; ++i)
    runs.push_back(Run(runlist_array[i]));
  return runs;
}

void construct_spectra(vector<Run>& runs, TH1*& e, TH2*& tvse) {
  const TH1* he    = runs.front().Spectrum_E();
  const TH2* htvse = runs.front().Spectrum_TVsE();
  std::cout << "Got initial spectra." << std::endl;
  e                = (TH1*)he->Clone("spec_e");
  tvse             = (TH2*)htvse->Clone("spec_tvse");
  std::cout << "Cloned spectra." << std::endl;
  delete he; delete htvse;
  e->SetDirectory(0);
  tvse->SetDirectory(0);
  for (int i = 1; i < runs.size(); ++i) {
    he    = runs[i].Spectrum_E();
    htvse = runs[i].Spectrum_TVsE();
    std::cout << "Adding spectra " << i << "...";
    e   ->Add(he);
    tvse->Add(htvse);
    std::cout << "added...";
    delete he; delete htvse;
    std::cout << "deleted." << std::endl;
  }
  std::cout << "Done adding spectra." << std::endl;
}

void mstm_analysis() {
  double rate[3], s2n[3];
  TH1* tdist[3];

  vector<Run> runs = construct_runs();
  TH1* e; TH2* tvse;
  std::cout << "Construct spectrum..." << std::endl;
  construct_spectra(runs, e, tvse);
  std::cout << "done." << std::endl;
  for (int i = 0; i < 3; ++i) {
    std::cout << "Loop index:\t" << i << std::endl;
    std::cout << "Rates...";
    rate[i] = Rate(e, E[i]);
    std::cout << "S2N...";
    s2n[i] = S2N(tvse, E[i], TWINDOW[i]);
    std::cout << "TD...";
    tdist[i] = TimeDistribution(tvse, E[i], 2.);
    std::cout << std::endl;
  }


  std::cout << "Rates" << std::endl;
  std::cout << "347 keV:\t" <<  rate[0] << std::endl;
  std::cout << "844 keV:\t" <<  rate[1] << std::endl;
  std::cout << "1809 keV:\t" << rate[2] << std::endl;

  std::cout << "S/N" << std::endl;
  std::cout << "347 keV:\t" <<  s2n[0] << std::endl;
  std::cout << "844 keV:\t" <<  s2n[1] << std::endl;
  std::cout << "1809 keV:\t" << s2n[2] << std::endl;

  TFile ofile("mstm_analysis.root", "RECREATE");
  tdist[0]->SetDirectory(&ofile);
  tdist[1]->SetDirectory(&ofile);
  tdist[2]->SetDirectory(&ofile);
  tdist[0]->Write();
  tdist[1]->Write();
  tdist[2]->Write();
  ofile.Close();
}
