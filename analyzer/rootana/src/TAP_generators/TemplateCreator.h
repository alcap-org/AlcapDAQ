#ifndef TEMPLATECREATOR_H_
#define TEMPLATECREATOR_H_

#include "BaseModule.h"
class gTGlobalData;
class gTSetupData;
class gModulesOptions;

#include "utils/TemplateArchive.h"
#include "utils/PulseCandidateFinder.h"

class TemplateCreator : public BaseModule{

 public:
  TemplateCreator(modules::options* opts);
  ~TemplateCreator();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup);
  virtual int AfterLastEntry(TGlobalData* gData, const TSetupData* setup);

  /// Store the options for use in ProcessEntry
  modules::options* fOpts;

  TemplateArchive* fTemplateArchive;

  void AddPulseToTemplate(TH1D* & hTemplate, const TPulseIsland* pulse);

  std::map<std::string, int> fNFitAttempts;
  std::map<std::string, int> fNSuccessfulFits;
};

#endif //TEMPLATECREATOR_H_
