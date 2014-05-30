#ifndef PlotTime_h__
#define PlotTime_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotTime : public FillHistBase{
 public:
  PlotTime(char *HistogramDirectoryName);
  PlotTime(modules::options* opts);
  ~PlotTime();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  std::map<std::string, TH1F*> fTimePlots;
};

#endif

