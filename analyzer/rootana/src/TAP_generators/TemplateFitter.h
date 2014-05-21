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
};

#endif
