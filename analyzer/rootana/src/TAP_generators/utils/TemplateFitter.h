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
  void FitPulseToTemplate(TH1D* hTemplate, const TPulseIsland* pulse);
  double GetPedestal() { return fPedestal; }
  double GetAmplitude() { return fAmplitude; }
  double GetTime() { return fTime; }
  double GetChi2() { return fChi2; }

 private:
  IDs::channel fChannel;
  double fPedestal;
  double fAmplitude;
  double fTime;
  double fChi2;

 public:
  /// \brief
  /// Sets the intial estimates for the template fitter
  void SetInitialParameterEstimates(double pedestal, double amplitude, double time);
};

#endif
