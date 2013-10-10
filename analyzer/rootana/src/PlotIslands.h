#ifndef PlotIslands_h__
#define PlotIslands_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

#include "TH1.h"

class PlotIslands : public FillHistBase{
 public:
  PlotIslands(char *HistogramDirectoryName);
  ~PlotIslands();
  
  TH1* Calibrate(TH1* hist, double x, double new_x);
  TH1* RemovePedestal(TH1* hist);

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

