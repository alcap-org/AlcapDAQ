#ifndef FastVsSlow_h__
#define FastVsSlow_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class FastVsSlow : public FillHistBase{
 public:
  FastVsSlow(char *HistogramDirectoryName);
  ~FastVsSlow();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

