#ifndef PlotAmplitude_h__
#define PlotAmplitude_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotAmplitude : public FillHistBase{
 public:
  PlotAmplitude(char *HistogramDirectoryName);
  PlotAmplitude(modules::options* opts);
  ~PlotAmplitude();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  std::map<std::string, TH1F*> fAmplitudePlots;
};

#endif

