#ifndef CreateDetectorPulses_h__
#define CreateDetectorPulses_h__

#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class CreateDetectorPulse : public BaseModule{
 public:
  CreateDetectorPulse(char *HistogramDirectoryName);
  ~CreateDetectorPulse();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif // CreateDetectorPulses_h__
