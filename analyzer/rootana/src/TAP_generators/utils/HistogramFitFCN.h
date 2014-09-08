#ifndef HistogramFitFCN_h__
#define HistogramFitFCN_h__

#include "Minuit2/FCNBase.h"
#include "TH1D.h"

#include <vector>

class HistogramFitFCN : public ROOT::Minuit2::FCNBase {

 public:
  HistogramFitFCN(const TH1D* = NULL,const  TH1D* = NULL);
  ~HistogramFitFCN();

  void SetTemplateHist(const TH1D*);
  void SetPulseHist(const TH1D*);

  void SetRefineFactor(int refine_factor) {fRefineFactor = refine_factor;}
  void SetTimeOffset(double time_offset);


  /// @brief Used for calls with parameters
  /// The return value is the chi squared
  /// weighted by errors in fTemplateHist
  /// Parameters:
  /// 1. Pedestal
  /// 2. Amplitude
  /// 3. Timing
  double operator() (const std::vector<double>& par) const;

  /// Used for error... somehow?
  double Up() const;

  int GetNDoF() { return fNDoF; }

 private:
  /// record this for TemplateFitter to retrieve later (NB mutable so that it
  /// can be set in operator(), which is const)
  mutable int fNDoF;
  
  /// the time offset to use
  double fTimeOffset;

  int fRefineFactor;

  const TH1D* fTemplateHist; // The template
  const TH1D* fPulseHist; // The histogram to fit

};

#endif
