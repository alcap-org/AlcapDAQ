#ifndef MAKEDETECTORPULSES_H__
#define MAKEDETECTORPULSES_H__

#include <string>
#include <vector>
#include <map>

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
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData,TSetupData *setup);
  std::string GetOtherChannelName(std::string in_name,std::string det_name);
  std::string GetDetectorName(std::string in_name);

 private:
  TVDetectorPulseGenerator* fGenerator; 
  StringDetPulseMap* fDetectorPulseMap;
  typedef std::vector<std::pair<IDs::source,IDs::source> > ChannelPairing_t;
  ChannelPairing_t fFastSlowPairs;
  modules::options* fOptions;
  std::string fAlgorithm;

};

#endif // MAKEDETECTORPULSES_H__
