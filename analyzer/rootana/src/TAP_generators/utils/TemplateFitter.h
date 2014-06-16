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
  double GetPedestal() { return fPedestal; }
  double GetAmplitude() { return fAmplitude; }
  double GetTime() { return fTime; }
  double GetChi2() { return fChi2; }

 private:
  double fPedestal;
  double fAmplitude;
  double fTime;
  double fChi2;
};

#endif
