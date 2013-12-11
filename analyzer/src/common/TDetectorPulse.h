#ifndef TDetectorPulse_h
#define TDetectorPulse_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>

#include "TAnalysedPulse.h"
#include "TSetupData.h"

extern TSetupData* gSetup;

class TDetectorPulse : public TObject {
  private:
  TAnalysedPulse* fFastPulse;
  TAnalysedPulse* fSlowPulse;
  std::string fDetName;

  public:
  TDetectorPulse();
  TDetectorPulse(TAnalysedPulse* fast_pulse, TAnalysedPulse* slow_pulse, std::string det_name);
  virtual ~TDetectorPulse() {};

  void Reset(Option_t* o = "");

  TAnalysedPulse* GetFastPulse() const { return fFastPulse; }
  TAnalysedPulse* GetSlowPulse() const { return fSlowPulse; }
  std::string GetDetName() const { return fDetName; }

  ClassDef(TDetectorPulse, 1);
};

#endif
