#ifndef TAnalysedPulse_h
#define TAnalysedPulse_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>
#include "TSetupData.h"

//extern TSetupData* gSetup;

class TAnalysedPulse : public TObject {
  private:
  double fAmplitude;
  double fTime;
  double fIntegral;
  std::string fDetName;

  public:
  TAnalysedPulse();
  TAnalysedPulse(double amplitude, double time, double integral, std::string det_name);
  virtual ~TAnalysedPulse() {};

  void Reset(Option_t* o = "");

  double GetAmplitude() const { return fAmplitude; }
  double GetTime() const { return fTime; }
  double GetIntegral() const { return fIntegral; }
  std::string GetDetName() const { return fDetName; }

  ClassDef(TAnalysedPulse, 1);
};

#endif
