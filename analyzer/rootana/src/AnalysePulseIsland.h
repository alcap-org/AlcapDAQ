#ifndef AnalysePulseIsland_h__
#define AnalysePulseIsland_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class AnalysePulseIsland : public FillHistBase{
 public:
  AnalysePulseIsland(char *HistogramDirectoryName);
  ~AnalysePulseIsland();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  void GetAllParameters_MaxBin(TSetupData* gSetup, const TPulseIsland* pulse,
			       double& amplitude, double& time, double& integral, double& energy);
};

#endif

