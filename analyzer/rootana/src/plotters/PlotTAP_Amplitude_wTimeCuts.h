#ifndef PlotTAP_Amplitude_wTimeCuts_h__
#define PlotTAP_Amplitude_wTimeCuts_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "IdChannel.h"

class PlotTAP_Amplitude_wTimeCuts : public BaseModule{
 public:
  PlotTAP_Amplitude_wTimeCuts(modules::options* opts);
  ~PlotTAP_Amplitude_wTimeCuts();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  std::map<std::string, TH1F*> fAmplitudePlots;

  double fMinTime;
  double fMaxTime;
};

#endif

