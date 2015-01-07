#ifndef TestVeto_h__
#define TestVeto_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class TestVeto : public FillHistBase{
 public:
  TestVeto(char *HistogramDirectoryName);
  ~TestVeto();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

