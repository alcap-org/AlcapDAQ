#ifndef TemplateFitFCN_h__
#define TemplateFitFCN_h__

#include "Minuit2/FCNBase.h"
#include "TH1D.h"

#include <vector>

class TemplateFitFCN : public ROOT::Minuit2::FCNBase {

 private:
  TH1D* fH1; // The template
  TH1D* fH2; // The histogram to fit

 public:
  TemplateFitFCN(TH1D* = NULL, TH1D* = NULL);
  ~TemplateFitFCN();

  void SetH1(TH1D*);
  void SetH2(TH1D*);

  // Used for calls with parameters
  // The return value is the chi squared
  // weighted by errors in fH1
  // Parameters:
  // 1. Pedestal
  // 2. Amplitude
  // 3. Timing
  double operator() (const std::vector<double>& par) const;
  // Used for error... somehow?
  double Up() const;
};

#endif
