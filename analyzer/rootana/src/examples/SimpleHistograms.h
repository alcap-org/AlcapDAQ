#ifndef SimpleHistorgram_h__
#define SimpleHistorgram_h__

#include "BaseModule.h"
#include "TGlobalData.h"

class SimpleHistograms : public BaseModule{
 public:
  SimpleHistograms(const char *HistogramDirectoryName);
  ~SimpleHistograms();

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

