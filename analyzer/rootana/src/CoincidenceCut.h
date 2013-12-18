#ifndef CoincidenceCut_h__
#define CoincidenceCut_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class CoincidenceCut : public FillHistBase{
 public:
  CoincidenceCut(char *HistogramDirectoryName, std::string corr_det_name, double start_window, double stop_window);
  ~CoincidenceCut();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  std::string fCorrDetName;
  double fStartWindow;
  double fStopWindow;
};

#endif

