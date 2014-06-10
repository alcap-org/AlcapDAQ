#ifndef PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_
#define PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_

#include "FillHistBase.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

class PulseCandidateFinder_InvestigateParameters : public FillHistBase{

 public:
  PulseCandidateFinder_InvestigateParameters(modules::options* opts);
  ~PulseCandidateFinder_InvestigateParameters();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

 private:
  TH1D* fSampleDifferences;
};

#endif //PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_
