#ifndef SiR2Target_h__
#define SiR2Target_h__

#include "BaseModule.h"

#include <vector>

class TGlobalData;
class TSetupData;
class TH1I;

class SiR2Target : public BaseModule{
 public:
  SiR2Target(char *HistogramDirectoryName);
  ~SiR2Target();

 private:
  TH1I* fhSpectrum;
  double ftCoincidence;

 public:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

