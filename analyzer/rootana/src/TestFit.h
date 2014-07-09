#ifndef TestFit_h__
#define TestFit_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class TestFit : public FillHistBase{
 public:
  TestFit(char *HistogramDirectoryName);
  ~TestFit();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif
