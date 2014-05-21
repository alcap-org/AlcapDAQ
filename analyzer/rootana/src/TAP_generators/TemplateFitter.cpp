#include "TemplateFitter.h"

#include "TemplateFitFCN.h"

TemplateFitter::TemplateFitter() {

  //  Three (3) parameters to modify (amplitude, time, pedestal)
  fNumParameters = 3;

  // Create the fitter
  TemplateFitFCN* fcn = new TemplateFitFCN();
  fFitter = new TFitterMinuit(fNumParameters);
  fFitter->SetMinuitFCN(fcn);
}

TemplateFitter::~TemplateFitter() {
}
