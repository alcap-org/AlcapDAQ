#ifndef NGammaSmInt_h__
#define NGammaSmInt_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class NGammaSmInt : public FillHistBase{
 public:
  NGammaSmInt(char *HistogramDirectoryName);
  ~NGammaSmInt();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

