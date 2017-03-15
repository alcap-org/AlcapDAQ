#ifndef PlotTDC_Time_h__
#define PlotTDC_Time_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotTDC_Time : public BaseModule{
 public:
  PlotTDC_Time(modules::options* opts);
  ~PlotTDC_Time();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  std::map<std::string, TH1F*> fTimePlots;
  std::map<std::string, TH1F*> fNHitsPlots;
  std::map<std::string, TH1F*> fDTPlots;
};

#endif

