#ifndef HistogramFitFCN_h__
#define HistogramFitFCN_h__

#include "Minuit2/FCNBase.h"
#include "TH1D.h"

#include <vector>

class HistogramFitFCN : public ROOT::Minuit2::FCNBase {

 private:
  TH1D* fTemplateHist; // The template
  TH1D* fPulseHist; // The histogram to fit

 public:
  HistogramFitFCN(TH1D* = NULL, TH1D* = NULL);
  ~HistogramFitFCN();

  void SetTemplateHist(TH1D*);
  void SetPulseHist(TH1D*);

  // Used for calls with parameters
  // The return value is the chi squared
  // weighted by errors in fTemplateHist
  // Parameters:
  // 1. Pedestal
  // 2. Amplitude
  // 3. Timing
  double operator() (const std::vector<double>& par) const;
  // Used for error... somehow?
  double Up() const;

 private:
  mutable int fNDoF; // record this for TemplateFitter to retrieve later (NB mutable so that it can be set in operator(), which is const)
  
 public:
  int GetNDoF() { return fNDoF; }
};

#endif
