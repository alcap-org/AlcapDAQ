#ifndef NGIntTCut2_h__
#define NGIntTCut2_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class NGIntTCut2 : public FillHistBase{
 public:
  NGIntTCut2(char *HistogramDirectoryName, std::string detnameA, double start_window, double stop_window);
  ~NGIntTCut2();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData *gData);

  int neutCount = 0, gammaCount = 0;

  std::string fDetNameA;
  double fStartWindow;
  double fStopWindow;
  char *fHistName;
};

#endif
