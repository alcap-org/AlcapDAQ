#ifndef CombineFastSlowPulses_h__
#define CombineFastSlowPulses_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class CombineFastSlowPulses : public FillHistBase{
 public:
  CombineFastSlowPulses(char *HistogramDirectoryName);
  ~CombineFastSlowPulses();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

