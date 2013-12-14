#ifndef CreateDetectorPulse_h__
#define CreateDetectorPulse_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class CreateDetectorPulse : public FillHistBase{
 public:
  CreateDetectorPulse(char *HistogramDirectoryName);
  ~CreateDetectorPulse();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

