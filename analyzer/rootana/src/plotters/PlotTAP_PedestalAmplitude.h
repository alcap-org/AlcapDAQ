#ifndef PlotTAP_pedestal_amplitude_h__
#define PlotTAP_pedestal_amplitude_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "TH2F.h"

class PlotTAP_PedestalAmplitude : public BaseModule{
 public:
  PlotTAP_PedestalAmplitude(modules::options* opts);
  ~PlotTAP_PedestalAmplitude();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);

  std::map<std::string, TH2F*> fPedestalAmplitudePlots;
};

#endif

