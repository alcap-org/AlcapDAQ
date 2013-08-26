#ifndef SimpleHistorgram_h__
#define SimpleHistorgram_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

class SimpleHistograms : public FillHistBase{
 public:
  SimpleHistograms(char *HistogramDirectoryName);
  ~SimpleHistograms();

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

