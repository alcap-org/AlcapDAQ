#ifndef PlotTAP_Pedestal_h__
#define PlotTAP_Pedestal_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotTAP_Pedestal : public BaseModule{
 public:
  PlotTAP_Pedestal(modules::options* opts);
  ~PlotTAP_Pedestal();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  std::map<std::string, TH1F*> fPedestalPlots;
};

#endif

