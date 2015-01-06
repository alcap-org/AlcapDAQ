#ifndef TestFGA_h__
#define TestFGA_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class TestFGA : public FillHistBase{
 public:
  TestFGA(char *HistogramDirectoryName);
  ~TestFGA();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif
