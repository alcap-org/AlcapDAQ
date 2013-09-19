#ifndef PlotPulses_h__
#define PlotPulses_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

#include "TH1.h"

class PlotPulses : public FillHistBase{
 public:
  PlotPulses(char *HistogramDirectoryName);
  ~PlotPulses();
  
  TH1* Calibrate(TH1* hist, double x, double new_x);
  TH1* RemovePedestal(TH1* hist);

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

