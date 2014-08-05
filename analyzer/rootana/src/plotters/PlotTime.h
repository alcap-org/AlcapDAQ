#ifndef PlotTime_h__
#define PlotTime_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotTime : public BaseModule{
 public:
  PlotTime(modules::options* opts);
  ~PlotTime();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  std::map<std::string, TH1F*> fTimePlots;
};

#endif

