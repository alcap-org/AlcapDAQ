#ifndef CutNonCoincMuSc_h__
#define CutNonCoincMuSc_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class CutNonCoincMuSc : public FillHistBase{
 public:
  CutNonCoincMuSc(char *HistogramDirectoryName);
  ~CutNonCoincMuSc();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

