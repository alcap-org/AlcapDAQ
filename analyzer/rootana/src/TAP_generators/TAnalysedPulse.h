#ifndef TAnalysedPulse_h
#define TAnalysedPulse_h

#include <TObject.h>
#include "TSetupData.h"

class TH1F;

class TAnalysedPulse : public TObject {
  typedef int TPulseIslandID;

  public:
  TAnalysedPulse();

  virtual ~TAnalysedPulse() {};

  void Reset(Option_t* o = "");

  virtual void Draw(const TH1F* tpi_pulse)const;

  TPulselandID GetParentTPI()const{return fParentTPI;};
  int GetTPILength()const{return fTPILength;};
  double GetAmplitude()const{return fAmplitude;};
  double GetTime()const{return fTime;};
  double GetIntegral()const{return fIntegral;};
  double GetEnergy()const{return fEnergy;};
  double GetPedestal()const{return fPedestal;};
  double GetTriggerTime()const{return fTriggerTime;};

  void SetParentTPI(const TPulseIslandID& val){ fParentTPI=val;};
  void SetTPILength(const int& val){ fTPILength=val;};
  void SetAmplitude(const double& val){ fAmplitude=val;};
  void SetTime(const double& val){ fTime=val;};
  void SetIntegral(const double& val){ fIntegral=val;};
  void SetEnergy(const double& val){ fEnergy=val;};
  void SetPedestal(const double& val){ fPedestal=val;};
  void SetTriggerTime(const double& val){ fTriggerTime=val;};
  void SetParentTPIProperties(const TPulseIslandID& id, 
                              const int& length, const double& trigger);

  public:
  bool IsParentTPISet()const{return fParentTPI!=defaultValue;};
  bool IsTPILengthSet()const{return fTPILength!=defaultValue;};
  bool IsAmplitudeSet()const{return fAmplitude!=defaultValue;};
  bool IsTimeSet()const{return fTime!=defaultValue;};
  bool IsIntegralSet()const{return fIntegral!=defaultValue;};
  bool IsEnergySet()const{return fEnergy!=defaultValue;};
  bool IsPedestalSet()const{return fPedestal!=defaultValue;};
  bool IsTriggerTimeSet()const{return fTriggerTime!=defaultValue;};

  private:
  TPulseIslandID fParentTPI;
  int fTPILength;
  double fAmplitude;
  double fTime;
  double fIntegral;
  double fEnergy;
  double fPedestal;
  double fTriggerTime;

  static const int fDefaultValue=-999;

  ClassDef(TAnalysedPulse, 4);
};

inline TAnalysedPulse::TAnalysedPulse():
  fParentTPI(fDefaultValue),
  fTPILength(fDefaultValue),
  fAmplitude(fDefaultValue),
  fTime(fDefaultValue),
  fIntegral(fDefaultValue),
  fEnergy(fDefaultValue),
  fPedestal(fDefaultValue),
  fTriggerTime(fDefaultValue){
    Reset();
}

#endif
