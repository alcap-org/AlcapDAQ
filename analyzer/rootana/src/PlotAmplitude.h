#ifndef PlotAmplitude_h__
#define PlotAmplitude_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class PlotAmplitude : public FillHistBase{
 public:
  PlotAmplitude(char *HistogramDirectoryName);
  ~PlotAmplitude();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

