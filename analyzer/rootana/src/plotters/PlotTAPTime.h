#ifndef PlotTAPTime_h__
#define PlotTAPTime_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotTAPTime : public BaseModule{
 public:
  PlotTAPTime(modules::options* opts);
  ~PlotTAPTime();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  std::map<std::string, TH1F*> fTimePlots;
};

#endif

