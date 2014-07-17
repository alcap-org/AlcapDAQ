#ifndef PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_
#define PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

#include "utils/PulseCandidateFinder.h"

class PulseCandidateFinder_InvestigateParameters : public BaseModule{

 public:
  PulseCandidateFinder_InvestigateParameters(modules::options* opts);
  ~PulseCandidateFinder_InvestigateParameters();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup);
  virtual int AfterLastEntry(TGlobalData* gData, const TSetupData* setup);

 private:
  /// Store the module options for use in ProcessEntry
  modules::options* fOpts;

  std::map<std::string, TH1D*> fParameterHistograms;
};

#endif //PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_
