#ifndef MAKEDETECTORPULSES_H__
#define MAKEDETECTORPULSES_H__

#include <string>
#include <vector>
#include <map>

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"

class TVDetectorPulseGenerator;
class TPulseIsland;
class TDetectorPulse;

class MakeDetectorPulses : public FillHistBase{
  typedef std::vector<TAnalysedPulse*> AnalysedPulseList_t;
  typedef std::map<std::string, AnalysedPulseList_t > BankAnalPulseList_t;
  typedef std::vector<TDetectorPulse*> DetectorPulseList_t;
  typedef std::map<std::string, DetectorPulseList_t > BankDetPulseList_t;

 public:
  MakeDetectorPulses(modules::options* opts);
  ~MakeDetectorPulses();

  void SetAlgorithm(const TString& algorithm){fAlgorithm=algorithm;};

  void SetDetectorPulseMap(BankDetPulseList_t& aMap){fDetectorPulseMap=&aMap;}
 private:
  TVDetectorPulseGenerator* MakeGenerator(const std::string& generatorType);
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData,TSetupData *setup);
  std::string GetOtherChannelName(std::string in_name,std::string det_name);
  std::string GetDetectorName(std::string in_name);

 private:
  TVDetectorPulseGenerator* fGenerator; 
  BankDetPulseList_t* fDetectorPulseMap;
  typedef std::map<std::string,std::pair<std::string,std::string> > ChannelPairing_t;
  ChannelPairing_t fFastSlowPairs;
  std::string fAlgorithm;

};

#endif // MAKEDETECTORPULSES_H__
