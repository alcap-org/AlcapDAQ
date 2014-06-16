#ifndef PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_
#define PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

class PulseCandidateFinder_InvestigateParameters : public BaseModule{

 public:
  PulseCandidateFinder_InvestigateParameters(modules::options* opts);
  ~PulseCandidateFinder_InvestigateParameters();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

 private:
  std::map<std::string, TH1D*> fParameterHistograms;
};

#endif //PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_
