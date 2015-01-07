#ifndef TestVetoMuscCoinc_h__
#define TestVetoMuscCoinc_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class TestVetoMuscCoinc : public FillHistBase{
 public:
  TestVetoMuscCoinc(char *HistogramDirectoryName);
  ~TestVetoMuscCoinc();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif
