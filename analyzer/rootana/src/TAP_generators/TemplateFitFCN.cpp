#include "TemplateFitFCN.h"

#include <cmath>
#include <iostream>

TemplateFitFCN::TemplateFitFCN(TH1D* h1, TH1D* h2) : fH1(h1), fH2(h2) {
}

TemplateFitFCN::~TemplateFitFCN() {
}

void TemplateFitFCN::SetH1(TH1D* h1) {
  if (fH1 && fH1 != h1)
    delete fH1;
  fH1 = h1;
}

void TemplateFitFCN::SetH2(TH1D* h2) {
  if (fH2 && fH2 != h2)
    delete fH2;
  fH2 = h2;
}

double TemplateFitFCN::operator() (const std::vector<double>& par) const {
  // Chi2 fit with pedestal P, amplitude A, and timing T
  // Warning: The time is truncated to an int, so if there's
  // so if the step size in MINUIT is smalled than 1,
  // Any value of T will probably be seen as minimized, which it
  // almost certainly will not be.
  double chi2 = 0.;
  double P = par[0];
  double A = par[1];
  int T_i = (int)par[2];            // Integral part of time shift
  double T_f = par[2] - (double)T_i; // Floating point offset for linear interpolation
  

  int bounds[2];
  bounds[0] = std::max(T_i - fH1->GetNbinsX() / 2, 1);
  bounds[1] = std::min(T_i + fH1->GetNbinsX() / 2 - 1, fH2->GetNbinsX());

  // Chi2 will be zero if shift is too high
  if (bounds[1] <= bounds[0])
    std::cout << "ERROR: Fit of two histograms involves shifting one out-of-bounds (no overlap)!" << std::endl;

  double f;
  for (int i = bounds[0]; i <= bounds[1]; ++i) {
    f = fH1->GetBinContent(i - T_i) + (fH1->GetBinContent(i - T_i + 1) - fH1->GetBinContent(i - T_i)) * T_f;
    f = A * f + P;
    chi2 += std::pow((fH1->GetBinContent(i) - f) / fH1->GetBinError(i), 2.);
  }

  static bool print_dbg = true;
  if (print_dbg) {
    std::cout << "Fit:\tChi2 " << chi2 << "\tP "
	      << P << "(" << par[0] << ")\tA " << A << "(" << par[1] << ")\tT " << T_i << " " << T_f << "(" << par[2] << ")" << " " << 0.02345
	      << std::endl;
    std::cout << "Bounds " << bounds[0] << "-" << bounds[1]
	      << "\tH1NX " << fH1->GetNbinsX()
	      << "\tH2NX " << fH2->GetNbinsX()
	      << std::endl; 
  }
  return chi2;
}

double TemplateFitFCN::Up() const {
  return 1.;
}
