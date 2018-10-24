#ifndef TemplateCreatorModule_H__
#define TemplateCreatorModule_H__

#include <string>
#include <vector>
#include <map>

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

class TPulseIsland;

class TemplateCreatorModule : public BaseModule{
  typedef std::vector<TPulseIsland*> PulseIslandList_t;
  typedef std::map<std::string, PulseIslandList_t > BankPulseList_t;

 public:
  TemplateCreatorModule(modules::options* opts);
  ~TemplateCreatorModule() {}

 private:
  virtual int ProcessEntry    (TGlobalData*, const TSetupData*);
  virtual int BeforeFirstEntry(TGlobalData*, const TSetupData*) { return 0; }
  virtual int AfterLastEntry  (TGlobalData*, const TSetupData*) { return 0; }

  modules::options* fOptions;

 private:
  void InitialParameterGuess(const TPulseIsland* pulse, double& amplitude, double& time);
  void AddPulseToTemplate(TH1D* hTemplate, TPulseIsland* pulse, double shift);
};

#endif // TemplateCreatorModule_H__
