#ifndef NGammaLanInt_h__
#define NGammaLanInt_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class NGammaLanInt : public FillHistBase{
 public:
  NGammaLanInt(char *HistogramDirectoryName);
  ~NGammaLanInt();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

