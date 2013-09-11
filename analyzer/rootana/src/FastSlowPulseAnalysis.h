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
  
  double GetPulseHeight(TH1* pulse_hist);
  double GetPulseIntegral(TH1* pulse_hist);
  
  std::vector<double> slow_pulse_heights;
  std::vector<double> fast_pulse_integrals;

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

