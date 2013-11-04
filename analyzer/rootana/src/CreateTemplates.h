#ifndef CreateTemplates_h__
#define CreateTemplates_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

class CreateTemplates : public FillHistBase{
 public:
  CreateTemplates(char *HistogramDirectoryName);
  ~CreateTemplates();
  
  void GaussianFit(TPulseIsland* pulse, int pulse_number); // for slow pulses
  void PseudotimeDistributions(TPulseIsland* pulse, int pulse_number); // for fast pulses
  void PseudotimeTemplate();

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

