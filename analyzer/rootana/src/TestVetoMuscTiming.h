#ifndef TestVetoMuscTiming_h__
#define TestVetoMuscTiming_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TH1.h"

class TestVetoMuscTiming : public FillHistBase{
 public:
  TestVetoMuscTiming(char *HistogramDirectoryName);
  ~TestVetoMuscTiming();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  TH1F* hTDiff;
};

#endif
