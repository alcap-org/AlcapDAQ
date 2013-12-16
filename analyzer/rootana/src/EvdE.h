#ifndef EvdE_h__
#define EvdE_h__

#include "FillHistBase.h"

class TGlobalData;
class TSetupData;

class EvdE : public FillHistBase{
 public:
  EvdE(char *HistogramDirectoryName);
  ~EvdE();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

