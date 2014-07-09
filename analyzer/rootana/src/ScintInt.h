#ifndef ScintInt_h__
#define ScintInt_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class ScintInt : public FillHistBase{
 public:
  ScintInt(char *HistogramDirectoryName);
  ~ScintInt();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

