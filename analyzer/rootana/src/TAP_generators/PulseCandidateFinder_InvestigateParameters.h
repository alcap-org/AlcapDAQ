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
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

 private:
  /// The PulseCandidateFinder that we will use
  PulseCandidateFinder* fPulseCandidateFinder;

  /// Where we will store the parameter histograms for each channel
  std::map<std::string, TH1D*> fParameterHistograms;

  /// Get the RMS noise of the pulse
  /// (Is the a function that should be elsewhere?)
  double GetRMSNoise(TPulseIsland* pulse, int n_samples);
};

#endif //PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_
