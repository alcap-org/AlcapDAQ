#ifndef Normalization_h__
#define Normalization_h__

#include "FillHistBase.h"

#include <vector>

class TGlobalData;
class TSetupData;

class Normalization : public FillHistBase{
 public:
  Normalization(char *HistogramDirectoryName, int threshold);
  ~Normalization();

 private:
  static unsigned int N_MU;

  unsigned int fNMuons;
  int fThreshold;

 public:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  static unsigned int GetMuonCount();

 private:
  bool IsOverThreshold(std::vector<TPulseIsland*>::iterator&);
};

#endif

