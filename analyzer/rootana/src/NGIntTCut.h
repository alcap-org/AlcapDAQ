#ifndef NGIntTCut_h__
#define NGIntTCut_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class NGIntTCut : public FillHistBase{
 public:
  NGIntTCut(char *HistogramDirectoryName, std::string detnameA, double start_window, double stop_window);
  ~NGIntTCut();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  std::string fDetNameA;
  double fStartWindow;
  double fStopWindow;
};

#endif
