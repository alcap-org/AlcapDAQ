#ifndef PlotGeSpec_h__
#define PlotGeSpec_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotGeSpec : public BaseModule{
 public:
  PlotGeSpec(char *HistogramDirectoryName);
  PlotGeSpec(modules::options* opts);
  ~PlotGeSpec();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  double Adc2keV(double adc) {return adc*fSlope + fConstant;}

  double fSlope;
  double fConstant;
  TH1F * fGeS_spectrum;
  TH1F * fGeF_spectrum;
  TH1F * fGeS_Tdiff;
};

#endif

