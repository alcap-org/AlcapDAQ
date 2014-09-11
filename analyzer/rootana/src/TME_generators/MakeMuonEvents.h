#ifndef MAKEMUONEVENTS_H__
#define MAKEMUONEVENTS_H__

#include "TVMuonEventGenerator.h"
#include "BaseModule.h"
#include "TGlobalData.h"

namespace modules{
  class options;
}

class MakeMuonEvents : public BaseModule{

 public:
  MakeMuonEvents(modules::options* opts);
  ~MakeMuonEvents();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, const TSetupData* aSetup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* aSetup);

  std::string fAlgorithm;
  TVMuonEventGenerator* fGenerator;
  modules::options* fOptions;
};

#endif // MAKEMUONEVENTS_H__
