#ifndef TemplateFitter_h__
#define TemplateFitter_h__

#include "TFitterMinuit.h"

class TemplateFitter {
  
 public:
  TemplateFitter();
  ~TemplateFitter();

 private:
  int fNumParameters;
  TFitterMinuit* fFitter;   // MINUIT fitter to template

 public:
  void FitPulse(TH1F* hTemplate, const TPulseIsland* pulse);
};

#endif
