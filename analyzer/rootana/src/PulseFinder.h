#ifndef PulseFinder_h__
#define PulseFinder_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

class PulseFinder : public FillHistBase{
 public:
  PulseFinder(char *HistogramDirectoryName);
  ~PulseFinder();
  
  void GetPedestalAndRMS(std::vector<int> samples, double& pedestal, double& RMS);

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

