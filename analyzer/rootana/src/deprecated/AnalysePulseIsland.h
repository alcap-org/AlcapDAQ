#ifndef AnalysePulseIsland_h__
#define AnalysePulseIsland_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"

#include "TAnalysedPulse.h"

class AnalysePulseIsland : public BaseModule{
 public:
  AnalysePulseIsland(char *HistogramDirectoryName);
  ~AnalysePulseIsland();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  void GetAllParameters_MaxBin(TSetupData* gSetup, const TPulseIsland* pulse,
			       double& amplitude, double& time, double& integral, double& energy);

  std::vector<TPulseIsland*> FindPulses(TPulseIsland* theIsland);
};

bool IsTimeOrdered(TAnalysedPulse* a, TAnalysedPulse* b);

#endif

