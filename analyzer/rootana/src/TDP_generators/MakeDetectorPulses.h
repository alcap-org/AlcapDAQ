#ifndef MAKEDETECTORPULSES_H__
#define MAKEDETECTORPULSES_H__

#include <string>
#include <set>

#include "BaseModule.h"
#include "definitions.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"

class TVDetectorPulseGenerator;

class MakeDetectorPulses : public BaseModule{

 public:
  MakeDetectorPulses(modules::options* opts);
  ~MakeDetectorPulses();

  void SetAlgorithm(const TString& algorithm){fAlgorithm=algorithm;};

  void SetDetectorPulseMap(StringDetPulseMap& aMap){fDetectorPulseMap=&aMap;}
 private:
  TVDetectorPulseGenerator* MakeGenerator(const std::string& generatorType);
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup);

void DumpgAnalysedPulseMap(const SourceAnalPulseMap& aMap);

 private:
  TVDetectorPulseGenerator* fGenerator; 
  StringDetPulseMap* fDetectorPulseMap;
  typedef std::set<std::pair<IDs::source,IDs::source> > ChannelPairing_t;
  ChannelPairing_t fFastSlowPairs;
  modules::options* fOptions;
  std::string fAlgorithm;

};

#endif // MAKEDETECTORPULSES_H__
