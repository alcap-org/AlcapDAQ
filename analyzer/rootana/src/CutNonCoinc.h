#ifndef CutNonCoinc_h__
#define CutNonCoinc_h__

#include "FillHistBase.h"
#include "TGlobalData.h"
#include "TSetupData.h"

class CutNonCoinc : public FillHistBase{
 public:
  CutNonCoinc(char *HistogramDirectoryName, std::string corr_det_name, double time_difference);
  ~CutNonCoinc();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

  std::string fCorrDetName;
  double fTimeDifference;
};

#endif

