#ifndef PlotTAP_Amplitude_h__
#define PlotTAP_Amplitude_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class PlotTAP_Amplitude : public BaseModule{
 public:
  PlotTAP_Amplitude(modules::options* opts);
  ~PlotTAP_Amplitude();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  std::map<std::string, TH1F*> fAmplitudePlots;
};

#endif

