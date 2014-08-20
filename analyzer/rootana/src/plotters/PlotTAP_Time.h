#ifndef PlotTAP_Time_h__
#define PlotTAP_Time_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotTAP_Time : public BaseModule{
 public:
  PlotTAP_Time(modules::options* opts);
  ~PlotTAP_Time();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  std::map<std::string, TH1F*> fTimePlots;
};

#endif

