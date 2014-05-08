#ifndef MAKEANALYSEDPULSES_H__
#define MAKEANALYSEDPULSES_H__

#include <string>
#include <vector>
#include <map>

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class TVAnalysedPulseGenerator;
class TPulseIsland;
class TAnalysedPulse;

class MakeAnalysedPulses : public FillHistBase{
  typedef std::vector<TPulseIsland*> PulseIslandList_t;
  typedef std::map<std::string, PulseIslandList_t > BankPulseList_t;
  typedef std::vector<TAnalysedPulse*> AnalysedPulseList_t;
  typedef std::map<std::string, AnalysedPulseList_t > BankAnalPulseList_t;
  typedef std::map<std::string,std::string> GeneratorTypes;
  typedef std::map<std::string,TVAnalysedPulseGenerator*> ChannelGenerators;

 public:
  MakeAnalysedPulses(char *HistogramDirectoryName, const char* fastGen="", const char* slowGen="");
  MakeAnalysedPulses(modules::options* opts);
  ~MakeAnalysedPulses();

  void SetGenerators(const std::string& fastGen, const std::string& slowGen=""){
      fFastGeneratorType=fastGen;
      fSlowGeneratorType=slowGen;
  }
  TVAnalysedPulseGenerator* MakeGenerator(const std::string& generatorType);

  void SetAnalysedPulseMap(BankAnalPulseList_t& aMap){fAnalysedPulseMap=&aMap;}
 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData,TSetupData *setup);
  //virtual int AfterLastEntry(TGlobalData* gData){return 0;};

  GeneratorTypes fDesiredGenerators;
  ChannelGenerators fGenerators;
  BankAnalPulseList_t* fAnalysedPulseMap;
  std::string fSlowGeneratorType;
  std::string fFastGeneratorType;
  std::vector<std::string> fChannelsToAnalyse;
  modules::options* fOptions;

  PulseIslandList_t RemoveFalseTPIs(PulseIslandList_t theIslands);
};

#endif // MAKEANALYSEDPULSES_H__
