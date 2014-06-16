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
  void GetPedestal() { return fMinuitFitter->GetParameter(0); }
  void GetAmplitude() { return fMinuitFitter->GetParameter(1); }
  void GetTime() { return fMinuitFitter->GetParameter(2); }
  void GetChi2();
};

#endif
