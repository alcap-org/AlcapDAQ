#ifndef PlotTAPAmplitude_h__
#define PlotTAPAmplitude_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotTAPAmplitude : public BaseModule{
 public:
  PlotTAPAmplitude(modules::options* opts);
  ~PlotTAPAmplitude();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  std::map<std::string, TH1F*> fAmplitudePlots;
};

#endif

