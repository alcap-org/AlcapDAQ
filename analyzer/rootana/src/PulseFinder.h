#ifndef PulseFinder_h__
#define PulseFinder_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

class PulseFinder : public FillHistBase{
 public:
  PulseFinder(char *HistogramDirectoryName);
  ~PulseFinder();
  
  double GetPedestal(std::vector<int> samples);

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

