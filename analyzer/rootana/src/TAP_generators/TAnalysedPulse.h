#ifndef TAnalysedPulse_h
#define TAnalysedPulse_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>
#include "TSetupData.h"

class TH1F;

//extern TSetupData* gSetup;

class TAnalysedPulse : public TObject {
  private:
  typedef int TPulseIslandID;
  TPulseIslandID fParentTPI;
  double fAmplitude;
  double fTime;
  double fIntegral;
  double fEnergy;
  std::string fDetName;
  std::string fAlgo;

  public:
  TAnalysedPulse();
  TAnalysedPulse(double amplitude, double time, double integral, 
			double energy, std::string det_name);
  TAnalysedPulse(double amplitude, double time, double integral, 
			double energy, std::string det_name, std::string algo);
  virtual ~TAnalysedPulse() {};

  void Reset(Option_t* o = "");

  double GetAmplitude() const { return fAmplitude; }
  double GetTime() const { return fTime; }
  double GetIntegral() const { return fIntegral; }
  double GetEnergy() const { return fEnergy; }
  std::string GetDetName() const { return fDetName; }
  std::string GetAlgorithmStamp() const { return fAlgo; }
  TPulseIslandID GetPulseIslandID()const{return fParentTPI;};
  void SetPulseIslandID(const TPulseIslandID& id){ fParentTPI=id;};
  virtual void Draw(TH1F* tpi_pulse)const{};

  ClassDef(TAnalysedPulse, 3);
};

#endif
