#ifndef PlotTME_Test_H__
#define PlotTME_Test_H__

#include "TVMuonEventGenerator.h"
#include "BaseModule.h"
#include "TGlobalData.h"

namespace modules{
  class options;
}

class PlotTME_Test : public BaseModule{

 public:
  PlotTME_Test(modules::options* opts);
  ~PlotTME_Test();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData* aSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup);
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* aSetup);

  TVMuonEventGenerator* fGenerator;
  modules::options* fOptions;

  int fRightCounter;
  int fLeftCounter;
};

#endif // MAKEMUONEVENTS_H__
