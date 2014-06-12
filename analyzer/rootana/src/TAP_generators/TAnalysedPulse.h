#ifndef TAnalysedPulse_h
#define TAnalysedPulse_h

#include <TObject.h>
#include "TSetupData.h"
#include "definitions.h"
#include "IdSource.h"
#include <map>
#include <vector>

class TH1F;
class TPulseIsland;

class TAnalysedPulse : public TObject {
  typedef int SourceProxy_t;
  typedef std::vector<IDs::source> ProxyToSourceMap;
  typedef std::map<IDs::source,SourceProxy_t> SourceToProxyMap;

  public:
  TAnalysedPulse();

  public:
  TAnalysedPulse(const IDs::source& sourceID,const TPulseIslandID& parentID, const TPulseIsland* parentTPI);
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
  const IDs::source& GetSource()const{return sProxyToSources.at(fSource);};

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
   void SetSource(const IDs::source& sourceID);

  private:
  TPulseIslandID fParentID;
  SourceProxy_t fSource;
  int fTPILength;
  double fAmplitude;
  double fTime;
  double fIntegral;
  double fEnergy;
  double fPedestal;
  double fTriggerTime;

  static const int fDefaultValue=-99999;
  static ProxyToSourceMap sProxyToSources;
  static SourceToProxyMap sSourceToProxies;

  ClassDef(TAnalysedPulse, 4);
};

#endif
