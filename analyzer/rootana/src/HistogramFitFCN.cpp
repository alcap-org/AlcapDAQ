#include "HistogramFitFCN.h"

#include <cmath>
#include <iostream>

HistogramFitFCN::HistogramFitFCN(TH1D* h1 = NULL, TH!D* h2 = NULL) : fH1(h1), fH2(h2) {
}

HistogramFitFCN::~HistogramFitFCN() {
}

void HistogramFitFCN::SetH1(TH1D* h1) {
  fH1 = h1;
}

void HistogramFitFCN::SetH2(TH1D* h2) {
  fH2 = h2;
}

double HistogramFitFCN::operator() (const std::vector<double>& par) const {
  // Chi2 fit with pedestal P, amplitude A, and timing T
  // Warning: The time is truncated to an int, so if there's
  // so if the step size in MINUIT is smalled than 1,
  // Any value of T will probably be seen as minimized, which it
  // almost certainly will not be.
  double chi2 = 0.;
  double P = par[0];
  double A = par[1];
  int T = (int)par[2]; // Loss of precision here may just straight up break the fitting

  int bounds[2];
  bounds[0] = std::max(1, 1 - T);
  bounds[1] = std::min(fH2.GetNBinsX() - T, fH1.GetNBinsX());

  // Chi2 will be zero if shift is too high
  if (bounds[1] <= bounds[0])
    std::cout << "ERROR: Fit of two histograms involves shifting one out-of-bounds (no overlap)!" << std::endl;

  double f;
  for (int i = bounds[0]; i <= bounds[1]; ++i) {
    f = A * fH1.GetBinContent(i) + P
    chi2 += std::pow((fH1.GetBinContent(i + T) - f) / fH1.GetBinError(i), 2.);
  }

  return chi2;
}

double HistogramFitFCN::Up() const {
  return 1.;
}
