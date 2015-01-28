#ifndef NGammaInt_h__
#define NGammaInt_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class NGammaInt : public FillHistBase{
 public:
  NGammaInt(char *HistogramDirectoryName);
  ~NGammaInt();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int AfterLastEntry(TGlobalData *gData);

  int neutCount = 0, gammaCount = 0;

  char *fHistName;
};

#endif
