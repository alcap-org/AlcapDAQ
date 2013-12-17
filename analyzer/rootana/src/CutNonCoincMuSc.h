#ifndef CutNonCoincMuSc_h__
#define CutNonCoincMuSc_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class CutNonCoincMuSc : public FillHistBase{
 public:
  CutNonCoincMuSc(char *HistogramDirectoryName, std::string corr_det_name, double time_difference);
  ~CutNonCoincMuSc();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  std::string fCorrDetName;
  double fTimeDifference;
};

#endif

