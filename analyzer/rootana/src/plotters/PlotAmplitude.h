#ifndef PlotAmplitude_h__
#define PlotAmplitude_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotAmplitude : public BaseModule{
 public:
  PlotAmplitude(char *HistogramDirectoryName);
  PlotAmplitude(modules::options* opts);
  ~PlotAmplitude();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  std::map<std::string, TH1F*> fAmplitudePlots;
};

#endif

