#ifndef AppraisePulseFinder_h__
#define AppraisePulseFinder_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"

#include "TVPulseFinder.h"
#include "FirstPulseFinder.h"
#include "NullPulseFinder.h"

class AppraisePulseFinder : public FillHistBase{
 public:
  AppraisePulseFinder(char *HistogramDirectoryName);
  AppraisePulseFinder(modules::options* opts);
  ~AppraisePulseFinder();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  std::string fDetName;
  int fPulseNumber;

  TH1F* hPulse;
};

#endif

