#ifndef TemplateFitter_h_
#define TemplateFitter_h_

#include "TFitterMinuit.h"
#include "TPulseIsland.h"
#include "TH1D.h"

#include "definitions.h"

class TemplateFitter {
 public:
  TemplateFitter(std::string detname);
  ~TemplateFitter();

 private:
  TFitterMinuit* fMinuitFitter;

 public:
  int FitPulseToTemplate(TH1D* hTemplate, const TPulseIsland* pulse);
  double GetPedestalOffset() { return fPedestalOffset; }
  double GetAmplitudeScaleFactor() { return fAmplitudeScaleFactor; }
  double GetTimeOffset() { return fTimeOffset; }
  double GetChi2() { return fChi2; }
  double GetNDoF() { return fNDoF; }

 private:
  IDs::channel fChannel;
  double fPedestalOffset;
  double fAmplitudeScaleFactor;
  double fTimeOffset;
  double fChi2;
  int fNDoF; // number of degress of freedom in fit

 public:
  /// \brief
  /// Sets the intial estimates for the template fitter
  void SetInitialParameterEstimates(double pedestal, double amplitude, double time);
};

#endif
