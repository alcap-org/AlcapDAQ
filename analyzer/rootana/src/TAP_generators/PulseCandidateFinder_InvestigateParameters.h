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
  /// Store the module options for use in ProcessEntry
  modules::options* fOpts;

  std::map<std::string, TH1D*> fParameterHistograms;

  /// Get the RMS noise of the pulse
  /// (Is the a function that should be elsewhere?)
  double GetRMSNoise(TPulseIsland* pulse, int n_samples);

  /// Where we will store the RMS noise histograms for each channel
  std::map<std::string, TH1D*> fRMSNoiseHistograms;
};

#endif //PULSECANDIDATEFINDER_INVESTIGATEPARAMETERS_H_
