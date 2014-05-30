#ifndef Lifetime_h__
#define Lifetime_h__

#include "FillHistBase.h"

class TGlobalData;
class TSetupData;

class Lifetime : public FillHistBase{
 public:
  Lifetime(char *HistogramDirectoryName);
  ~Lifetime();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

