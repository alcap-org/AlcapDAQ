#ifndef TAnalysedPulse_h
#define TAnalysedPulse_h

#include <TObject.h>
#include "TSetupData.h"
#include "definitions.h"

class TH1F;
class TPulseIsland;

class TAnalysedPulse : public TObject {

  private:
  TAnalysedPulse();

  public:
  TAnalysedPulse(const TPulseIslandID& parentID, const TPulseIsland* parentTPI);
  virtual ~TAnalysedPulse() {};

  void Reset(Option_t* o = "");

  virtual void Draw(const TH1F* tpi_pulse)const;

  TPulseIslandID GetParentID()const{return fParentID;};
  int GetTPILength()const{return fTPILength;};
  double GetAmplitude()const{return fAmplitude;};
  double GetTime()const{return fTime;};
  double GetIntegral()const{return fIntegral;};
  double GetEnergy()const{return fEnergy;};
  double GetPedestal()const{return fPedestal;};
  double GetTriggerTime()const{return fTriggerTime;};

  void SetParentID(const TPulseIslandID& val){ fParentID=val;};
  void SetTPILength(const int& val){ fTPILength=val;};
  void SetAmplitude(const double& val){ fAmplitude=val;};
  void SetTime(const double& val){ fTime=val;};
  void SetIntegral(const double& val){ fIntegral=val;};
  void SetEnergy(const double& val){ fEnergy=val;};
  void SetPedestal(const double& val){ fPedestal=val;};
  void SetTriggerTime(const double& val){ fTriggerTime=val;};
  void SetParentTPIProperties(const TPulseIslandID& id, 
                              const TPulseIsland* parent);

  public:
  bool IsParentIDSet()const{return fParentID!=fDefaultValue;};
  bool IsTPILengthSet()const{return fTPILength!=fDefaultValue;};
  bool IsAmplitudeSet()const{return fAmplitude!=fDefaultValue;};
  bool IsTimeSet()const{return fTime!=fDefaultValue;};
  bool IsIntegralSet()const{return fIntegral!=fDefaultValue;};
  bool IsEnergySet()const{return fEnergy!=fDefaultValue;};
  bool IsPedestalSet()const{return fPedestal!=fDefaultValue;};
  bool IsTriggerTimeSet()const{return fTriggerTime!=fDefaultValue;};

  private:
  TPulseIslandID fParentID;
  int fTPILength;
  double fAmplitude;
  double fTime;
  double fIntegral;
  double fEnergy;
  double fPedestal;
  double fTriggerTime;

  static const int fDefaultValue=-99999;

  ClassDef(TAnalysedPulse, 4);
};

#endif
