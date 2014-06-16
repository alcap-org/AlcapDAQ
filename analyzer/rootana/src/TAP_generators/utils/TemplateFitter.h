#ifndef TemplateFitter_h_
#define TemplateFitter_h_

#include "TFitterMinuit.h"
#include "TPulseIsland.h"
#include "TH1D.h"

class TemplateFitter {
 public:
  TemplateFitter();
  ~TemplateFitter();

 private:
  TFitterMinuit* fMinuitFitter;

 public:
  void FitPulseToTemplate(TH1D* hTemplate, const TPulseIsland* pulse);
  double GetPedestal() { return fMinuitFitter->GetParameter(0); }
  double GetAmplitude() { return fMinuitFitter->GetParameter(1); }
  double GetTime() { return fMinuitFitter->GetParameter(2); }
  double GetChi2();
};

#endif
