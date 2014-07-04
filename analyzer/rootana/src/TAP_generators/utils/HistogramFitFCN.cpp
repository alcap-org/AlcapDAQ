#include "HistogramFitFCN.h"

#include <cmath>
#include <iostream>

HistogramFitFCN::HistogramFitFCN(TH1D* hTemplate, TH1D* hPulse) : fTemplateHist(hTemplate), fPulseHist(hPulse) {
}

HistogramFitFCN::~HistogramFitFCN() {
}

void HistogramFitFCN::SetTemplateHist(TH1D* hTemplate) {
  fTemplateHist = hTemplate;
}

void HistogramFitFCN::SetPulseHist(TH1D* hPulse) {
  fPulseHist = hPulse;
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
  int T_int = (int)par[2];            // Integral part of time shift
  double T_flt = par[2] - (double)T_int; // Floating point offset for linear interpolation

  static bool print_dbg = false;
  if (print_dbg) { 
    std::cout << "HistogramFitFCN::operator() (start):" << std::endl;
    std::cout << "\tpedestal = " << P << ", amplitude = " << A << ", time (integer part) = " << T_int << " and time (float part) = " << T_flt << std::endl;
  }
 
  int bounds[2];
  bounds[0] = std::max(T_int - fTemplateHist->GetNbinsX() / 2, 1);
  bounds[1] = std::min(T_int + fTemplateHist->GetNbinsX() / 2 - 1, fPulseHist->GetNbinsX());

  fNDoF = bounds[1] - bounds[0] + 1 - par.size(); // +1 because we include both ends of the bounds when we loop through

  if (print_dbg) {
    std::cout << "NBinsX: hTemplate = " << fTemplateHist->GetNbinsX() << ", hPulse = " << fPulseHist->GetNbinsX() << std::endl;
    std::cout << "Bound Defns: " << std::endl;
    std::cout << "\tbounds[0] = std::max(T_int - fTemplateHist->GetNbinsX() / 2, 1) = " << bounds[0] << std::endl;
    std::cout << "\tbounds[1] = std::min(T_int + fTemplateHist->GetNbinsX() / 2 - 1, fPulseHist->GetNbinsX()) = " << bounds[1] << std::endl;
  }

  // Chi2 will be zero if shift is too high
  if (bounds[1] <= bounds[0])
    std::cout << "ERROR: Fit of two histograms involves shifting one out-of-bounds (no overlap)!" << std::endl;

  double f;
  for (int i = bounds[0]; i <= bounds[1]; ++i) {
    // We shift and scale the template so that it matches the pulse.
    // This is because, when we have a normalised template, we will get the actual amplitude, pedestal and time from the fit and not just offsets
    f = fTemplateHist->GetBinContent(i - T_int) + T_flt*(fTemplateHist->GetBinContent(i - T_int + 1) - fTemplateHist->GetBinContent(i - T_int)); // linear interpolation between the i'th and the (i+1)'th bin
    f = A * f + P;

    double delta = fPulseHist->GetBinContent(i) - f;
    double hTemplate_bin_error = fTemplateHist->GetBinError(i);
    double hPulse_bin_error = fPulseHist->GetBinError(i);
    chi2 += delta*delta / ((hTemplate_bin_error*hTemplate_bin_error) + (hPulse_bin_error)*(hPulse_bin_error));
    //    std::cout << "Histogram Errors: Template = " << hTemplate_bin_error << ", Pulse = " << hPulse_bin_error << std::endl;
  }

  if (print_dbg) {
    std::cout << "HistogramFitFCN::operator() (end): " << std::endl;
    std::cout << "\tFit:\tChi2 " << chi2 << "\tP "
	      << P << "(" << par[0] << ")\tA " << A << "(" << par[1] << ")\tT " << T_int << " " << T_flt << "(" << par[2] << ")" << " " << 0.02345
	      << std::endl << std::endl;
  }
  return chi2;
}

double HistogramFitFCN::Up() const {
  return 1.;
}
