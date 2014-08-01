#ifndef PlotAmplitude_h__
#define PlotAmplitude_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotAmplitude : public BaseModule{
 public:
  PlotAmplitude(modules::options* opts);
  ~PlotAmplitude();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  std::map<std::string, TH1F*> fAmplitudePlots;
};

#endif

