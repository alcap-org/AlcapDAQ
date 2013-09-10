#ifndef FastSlowPulseAnalysis_h__
#define FastSlowPulseAnalysis_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

class FastSlowPulseAnalysis : public FillHistBase{
 public:
  FastSlowPulseAnalysis(char *HistogramDirectoryName);
  ~FastSlowPulseAnalysis();

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

