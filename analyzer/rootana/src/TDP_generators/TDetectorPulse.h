#ifndef TDetectorPulse_h
#define TDetectorPulse_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>

#include "TAnalysedPulse.h"
#include "TSetupData.h"

//extern TSetupData* gSetup;

class TDetectorPulse : public TObject {
  public:
  TDetectorPulse();
  TDetectorPulse(TAnalysedPulse* fast_pulse, TAnalysedPulse* slow_pulse, std::string det_name);
  virtual ~TDetectorPulse();

  void Reset(Option_t* o = "");

  double GetFastPulseAmplitude() const { if (fFastPulse) return fFastPulse->GetAmplitude(); else return -9999; }
  double GetSlowPulseAmplitude() const { if (fSlowPulse) return fSlowPulse->GetAmplitude(); else return -9999; }

  double GetFastPulseTime() const { if (fFastPulse) return fFastPulse->GetTime(); else return -9999; }
  double GetSlowPulseTime() const { if (fSlowPulse) return fSlowPulse->GetTime(); else return -9999; }

  double GetFastPulseIntegral() const { if (fFastPulse) return fFastPulse->GetIntegral(); else return -9999; }
  double GetSlowPulseIntegral() const { if (fSlowPulse) return fSlowPulse->GetIntegral(); else return -9999; }

  double GetTime()const{return fFastPulse?GetFastPulseTime():GetSlowPulseTime();};
  double GetAmplitude()const{return fSlowPulse?GetSlowPulseAmplitude():GetFastPulseAmplitude();};

  std::string GetDetName() const { return fDetName; }

  bool IsPileUpSafe()const{return fPileUpSafe&&fCheckedForPileUp;};
  void SetPileUpSafe(const bool& val=true){fPileUpSafe=val;fCheckedForPileUp=true;};
  bool WasPileUpChecked()const{return fCheckedForPileUp;};
  void SetPileUpChecked(const bool& val=true){ fCheckedForPileUp=val;};

  private:
  TAnalysedPulse* fFastPulse;
  TAnalysedPulse* fSlowPulse;
  std::string fDetName;
  bool fCheckedForPileUp;
  bool fPileUpSafe;

  ClassDef(TDetectorPulse, 1);
};

#endif
