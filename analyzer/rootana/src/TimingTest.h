#ifndef TimingTest_h__
#define TimingTest_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

#include "TH1.h"

class TimingTest : public FillHistBase{
 public:
  TimingTest(char *HistogramDirectoryName);
  ~TimingTest();
  
  TH1* Calibrate(TH1* hist, double x, double new_x);
  TH1* RemovePedestal(TH1* hist);
  
  double GetPulseTime(TH1* pulse_hist);

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

