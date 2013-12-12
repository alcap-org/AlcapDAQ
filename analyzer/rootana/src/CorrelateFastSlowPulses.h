#ifndef CorrelateFastSlowPulses_h__
#define CorrelateFastSlowPulses_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class CorrelateFastSlowPulses : public FillHistBase{
 public:
  CorrelateFastSlowPulses(char *HistogramDirectoryName);
  ~CorrelateFastSlowPulses();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

