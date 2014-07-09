#ifndef PlotNEnergy_h__
#define PlotNEnergy_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class PlotNEnergy : public FillHistBase{
 public:
  PlotNEnergy(char *HistogramDirectoryName);
  ~PlotNEnergy();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

