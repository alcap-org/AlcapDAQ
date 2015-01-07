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
  virtual int AfterLastEntry(TGlobalData *gData);

  int neutCount = 0, gammaCount = 0;

  std::string fDetNameA;
  double fStartWindow;
  double fStopWindow;
  char *fHistName;
};

#endif
