#ifndef CombineFastSlowPulses_h__
#define CombineFastSlowPulses_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include <iostream>

class CombineFastSlowPulses : public FillHistBase{
 public:
  CombineFastSlowPulses(char *HistogramDirectoryName,bool verbose=true);
  ~CombineFastSlowPulses();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

};

#endif

