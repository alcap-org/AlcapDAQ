#include "TemplateFitter.h"

#include "HistogramFitFCN.h"

TemplateFitter::TemplateFitter() {

  HistogramFitFCN* fcn = new HistogramFitFCN();
  fMinuitFitter = new TFitterMinuit(3); //  Three (3) parameters to modify (amplitude, time, pedestal)
  fMinuitFitter->SetMinuitFCN(fcn);
}

TemplateFitter::~TemplateFitter() {
}
