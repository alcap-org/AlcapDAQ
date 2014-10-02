#ifndef PlotTME_SiliconArmAsymmetry_H__
#define PlotTME_SiliconArmAsymmetry_H__

#include "TVMuonEventGenerator.h"
#include "BaseModule.h"
#include "TGlobalData.h"

namespace modules{
  class options;
}

class PlotTME_SiliconArmAsymmetry : public BaseModule{

 public:
  PlotTME_SiliconArmAsymmetry(modules::options* opts);
  ~PlotTME_SiliconArmAsymmetry();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData* aSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup);
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* aSetup);

  TVMuonEventGenerator* fGenerator;
  modules::options* fOptions;

  double fRightCounter;
  double fLeftCounter;

  TH1F* fAmplitudeHistogram;
};

#endif // MAKEMUONEVENTS_H__
