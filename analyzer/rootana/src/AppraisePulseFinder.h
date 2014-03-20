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

  TVPulseFinder* MakeFinder(const std::string& finderType);
  std::vector<int> AddNoise(const std::vector<int> samples, int RMS);

  std::string fDetName;
  int fPulseNumber;
  std::string fPulseFinderType;
  TVPulseFinder* fPulseFinder;

  TH1F* hPulse;
};

#endif

