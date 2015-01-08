#ifndef TestRatio_h__
#define TestRatio_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class TestRatio : public FillHistBase{
 public:
  TestRatio(char *HistogramDirectoryName);
  ~TestRatio();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif
