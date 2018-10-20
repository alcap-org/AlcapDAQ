#ifndef TemplateCreatorModule_H__
#define TemplateCreatorModule_H__

#include <string>
#include <vector>
#include <map>

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class TPulseIsland;

class TemplateCreatorModule : public FillHistBase{
  typedef std::vector<TPulseIsland*> PulseIslandList_t;
  typedef std::map<std::string, PulseIslandList_t > BankPulseList_t;

 public:
  TemplateCreatorModule(modules::options* opts);
  ~TemplateCreatorModule();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData,TSetupData *setup);
  //virtual int AfterLastEntry(TGlobalData* gData){return 0;};

  modules::options* fOptions;

 private:
  void InitialParameterGuess(const TPulseIsland* pulse, double& amplitude, double& time);
  void AddPulseToTemplate(TH1D* hTemplate, TPulseIsland* pulse, double shift);
};

#endif // TemplateCreatorModule_H__
