#ifndef MAKEANALYSEDPULSES_H__
#define MAKEANALYSEDPULSES_H__

#include <string>
#include <vector>
#include <map>

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "TAPGeneratorOptions.h"
#include "definitions.h"

class TVAnalysedPulseGenerator;
class TPulseIsland;
class TAnalysedPulse;

class MakeAnalysedPulses : public BaseModule{
  typedef std::vector<TVAnalysedPulseGenerator*> ChannelGenerators_t;

 public:
  MakeAnalysedPulses(modules::options* opts);
  ~MakeAnalysedPulses();

  bool AddGenerator(const std::string& detector,std::string generatorType,TAPGeneratorOptions* opts=NULL);
  bool ParseGeneratorList(std::string generatorList, const std::vector<std::string>&);

  void SetAnalysedPulseMap(StringAnalPulseMap& aMap){fAnalysedPulseMap=&aMap;}
 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup);
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData* setup){return 0;};

  void CalibratePulses(const TVAnalysedPulseGenerator* generator, AnalysedPulseList& theAnalysedPulses)const;

  ChannelGenerators_t fGenerators;
  StringAnalPulseMap* fAnalysedPulseMap;
  std::string fSlowGeneratorType;
  std::string fFastGeneratorType;
  std::vector<std::string> fChannelsToAnalyse;
  modules::options* fOptions;
  TAPGeneratorOptions* fDefaultOpts;

};

#endif // MAKEANALYSEDPULSES_H__
