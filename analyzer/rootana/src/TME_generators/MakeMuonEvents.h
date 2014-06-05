#ifndef MAKEMUONEVENTS_H__
#define MAKEMUONEVENTS_H__

#include "TVMuonEventGenerator.h"
#include "FillHistBase.h"
#include "TGlobalData.h"

namespace modules{
  class options;
}

class MakeMuonEvents : public FillHistBase{

 public:
  MakeMuonEvents(modules::options* opts);
  ~MakeMuonEvents();

 private:
  virtual int BeforeFirstEntry(TGlobalData *gData, TSetupData* aSetup);
  virtual int ProcessEntry(TGlobalData *gData, TSetupData* aSetup);

  std::string fAlgorithm;
  TVMuonEventGenerator* fGenerator;
  modules::options* fOptions;
};

#endif // MAKEMUONEVENTS_H__
