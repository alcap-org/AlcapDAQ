#ifndef FastSlowPulseAnalysis_h__
#define FastSlowPulseAnalysis_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

#include "TH1.h"

class FastSlowPulseAnalysis : public FillHistBase{
 public:
  FastSlowPulseAnalysis(char *HistogramDirectoryName);
  ~FastSlowPulseAnalysis();
  
  TH1* Calibrate(TH1* hist, double x, double new_x);
  TH1* RemovePedestal(TH1* hist);

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

