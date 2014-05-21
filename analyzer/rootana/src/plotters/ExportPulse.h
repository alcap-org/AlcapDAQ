#ifndef ExportPulse_H__
#define ExportPulse_H__

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

class ExportPulse : public FillHistBase{
  typedef std::vector<TPulseIsland*> PulseIslandList_t;
  typedef std::map<std::string, PulseIslandList_t > BankPulseList_t;
  typedef std::vector<TAnalysedPulse*> AnalysedPulseList_t;
  typedef std::map<std::string, AnalysedPulseList_t > BankAnalPulseList_t;
  typedef std::map<std::string,std::string> GeneratorTypes;
  typedef std::map<std::string,TVAnalysedPulseGenerator*> ChannelGenerators;

 public:
  ExportPulse(modules::options* opts);
  ~ExportPulse();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData,TSetupData *setup);
  //virtual int AfterLastEntry(TGlobalData* gData){return 0;};

  BankAnalPulseList_t* fAnalysedPulseMap;
  modules::options* fOptions;

  int fPulseCounter;
  std::vector<int> fPulsesToPlot;

 public:
  void AddToExportList(int pulse_id);
  static ExportPulse* Instance();
};

inline ExportPulse* ExportPulse::Instance() {
  return modules::navigator::Instance()->GetModule<ExportPulse>("ExportPulse");
}


#endif // ExportPulse_H__
