#ifndef TestTiming_h__
#define TestTiming_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class TestTiming : public FillHistBase{
 public:
  TestTiming(char *HistogramDirectoryName);
  ~TestTiming();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif
