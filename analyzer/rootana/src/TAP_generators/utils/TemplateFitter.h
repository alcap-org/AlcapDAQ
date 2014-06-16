#ifndef TemplateFitter_h_
#define TemplateFitter_h_

#include "TFitterMinuit.h"

class TemplateFitter {
 public:
  TemplateFitter();
  ~TemplateFitter();

 private:
  TFitterMinuit* fMinuitFitter;
};

#endif
