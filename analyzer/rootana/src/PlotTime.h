#ifndef PlotTime_h__
#define PlotTime_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class PlotTime : public FillHistBase{
 public:
  PlotTime(char *HistogramDirectoryName);
  ~PlotTime();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

