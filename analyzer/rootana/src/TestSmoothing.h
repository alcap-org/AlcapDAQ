#ifndef TestSmoothing_h__
#define TestSmoothing_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class TestSmoothing : public FillHistBase{
 public:
  TestSmoothing(char *HistogramDirectoryName);
  ~TestSmoothing();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

