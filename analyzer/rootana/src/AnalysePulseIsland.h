#ifndef AnalysePulseIsland_h__
#define AnalysePulseIsland_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

#include "TAnalysedPulse.h"

class AnalysePulseIsland : public FillHistBase{
 public:
  AnalysePulseIsland(char *HistogramDirectoryName);
  ~AnalysePulseIsland();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  void GetAllParameters_MaxBin(TSetupData* gSetup, const TPulseIsland* pulse,
			       double& amplitude, double& time, double& integral, double& tintegral, double& energy, double& ratio);
  void GetAllParameters_MBCFT(TSetupData* gSetup, const TPulseIsland* pulse,
			      double& amplitude, double& time, double& integral, double& tintegral, double& energy, double& ratio);
  void GetAllParameters_InterCFT(TSetupData* gSetup, const TPulseIsland* pulse,
			      double& amplitude, double& time, double& integral, double& tintegral, double& energy, double& ratio);


};

bool IsTimeOrdered(TAnalysedPulse* a, TAnalysedPulse* b);

#endif

