#ifndef SiR2Target_h__
#define SiR2Target_h__

#include "FillHistBase.h"

#include <vector>

class TGlobalData;
class TSetupData;
class TH2I;

class SiR2Target : public FillHistBase{
 public:
  SiR2Target(char *HistogramDirectoryName);
  ~SiR2Target();

 private:
  TH2I* fhEvdE;
  double ftCoinc_Musc_Stop[2];    // Time window after muSc to look for stop
  double ftCoinc_Stop_Decay[2];   // Time window after stop to look for decay
  double ftCoinc_Decay_ArmHit[2]; // Time window after decay to look for arm hit

 public:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
};

#endif

