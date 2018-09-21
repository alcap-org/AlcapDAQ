#ifndef PlotTAP_amplitude_time_h__
#define PlotTAP_amplitude_time_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "TH2F.h"

class PlotTAP_AmplitudeTime : public BaseModule{
 public:
  PlotTAP_AmplitudeTime(modules::options* opts);
  ~PlotTAP_AmplitudeTime();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  std::map<std::string, TH2F*> fAmplitudeTimePlots;
};

#endif

