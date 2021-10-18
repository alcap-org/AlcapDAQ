#ifndef TAnalysedPulse_h
#define TAnalysedPulse_h

#include <TObject.h>
#include "TSetupData.h"
#include "definitions.h"
#include "IdSource.h"
#include <map>
#include <vector>
#include "FlyWeight.h"
class TH1F;
class TPulseIsland;

typedef int TAnalysedPulseID;

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana
///
/// \brief
/// A %TAnalysedPulse (TAP) is the product of the second level of analysis
/// in the framework.
///
/// \details
/// A TAP is generated from a TPulseIsland and represents the next higher level,
/// more intuitive representation of a physical hit in a detector. It has
/// properties more commonly associated with a physics event, such as time
/// and energy, that are derived from the TPI using the methods in
/// generator (derived from TVAnalysedPulseGenerator). Different generators
/// can calculate the same quantitites using different techniques, leading to
/// multiple TAPs coming from a single TPI. 
////////////////////////////////////////////////////////////////////////////////
class TAnalysedPulse : public TObject {
  public:
  struct Tag{};

  public:
  /// \brief
  /// This constructor must exist for ROOT I/O, but should not be used as
  /// you cannot set the source outside of the constructor.
  TAnalysedPulse();

  virtual void Copy(TObject& rhs)const{
    TObject::Copy(rhs);
    if(rhs.InheritsFrom(Class())){
      TAnalysedPulse* tap=static_cast<TAnalysedPulse*>(&rhs);
      tap->fParentID=fParentID;
      tap->fTAPID=fTAPID;
      tap->fTPILength=fTPILength;
      tap->fAmplitude=fAmplitude;
      tap->fTime=fTime;
      tap->fIntegral=fIntegral;
      tap->fEnergy=fEnergy;
      tap->fPedestal=fPedestal;
      tap->fTriggerTime=fTriggerTime;
      tap->fSource=fSource;
    }
  }

  public:
  /// \brief
  /// The constructor one should use if we need use one.
  ///
  /// \details
  /// This constructor, which is the only constructor the user should use,
  /// takes all the information neessary to identify the genesis of this TAP.
  /// Generally a user would not use this constructor, but would instead call
  /// TVAnalysedPulseGenerator::MakeNewTAP which determines the source and
  /// TPI from the TPulseIslandID and where in the code it's being called.
  ///
  /// \param[in] sourceID The source identifying the detector the TPI is from
  /// and what generator processed it.
  /// \param[in] parentID The position in the TGD vector of the TPI
  /// used to make this TAP.
  /// \param[in] parentTPI The pulse that was used to produce this TAP.
  TAnalysedPulse(const IDs::source& sourceID,const TPulseIslandID& parentID, const TAnalysedPulseID& tapID, const TPulseIsland* parentTPI);
  virtual ~TAnalysedPulse() {};

  /// \brief
  /// Clear the TAP. This will probably not be used.
  void Reset(Option_t* o = "");

  /// \brief
  /// Visualize the TAP interpretation of the TPI.
  ///
  /// \details
  /// Since there are different methods for determining the physical
  /// values in the TAP from the TPI, here we can get a visual
  /// feel for how right those values might be. If passed a TH1F
  /// representation of the TPI, Draw makes a new histogram
  /// with the same histogram dimensions and a single bin
  /// at the determined time weighted with the determined amplitude.
  /// 
  /// \todo
  /// Clean this up
  ///
  /// \param[in] tpi_pulse A histogram representation of the parent pulse
  virtual void Draw(const TH1F* tpi_pulse)const;

  /// \name Getters
  //@{
  TPulseIslandID GetParentID()const{return fParentID;};
  TAnalysedPulseID GetTAPID()const{return fTAPID;};
  int GetTPILength()const{return fTPILength;};
  double GetAmplitude()const{return fAmplitude;};
  double GetTime()const{return fTime;};
  double GetIntegral()const{return fIntegral;};
  double GetEnergy()const{return fEnergy;};
  double GetPedestal()const{return fPedestal;};
  double GetTriggerTime()const{return fTriggerTime;};
  const IDs::source& GetSource()const{return fSource.GetValue();};
  //  const std::vector<int>& GetSamples() const{ return fSamples; }
  //@}

  /// \name Setters
  ///
  /// \details
  /// The TAP generators will set whatever TAP values it must
  /// through these methods. Not all need be set; the default
  /// values are obviously unphysical and several Sanity
  /// methods are provided to check \em if a value was set
  /// later in the analysis.
  //@{
  void SetParentID(const TPulseIslandID& val){ fParentID=val;};
  void SetTAPID(const TAnalysedPulseID& val){ fTAPID=val;};
  void SetTPILength(const int& val){ fTPILength=val;};
  void SetAmplitude(const double& val){ fAmplitude=val;};
  void SetTime(const double& val){ fTime=val;};
  void SetIntegral(const double& val){ fIntegral=val;};
  void SetEnergy(const double& val){ fEnergy=val;};
  void SetPedestal(const double& val){ fPedestal=val;};
  void SetTriggerTime(const double& val){ fTriggerTime=val;};
  void SetParentTPIProperties(const TPulseIslandID& id, 
                              const TPulseIsland* parent);
  //@}

 public:
  /// \name Sanity
  //@{
  bool IsParentIDSet()const{return fParentID!=fDefaultValue;};
  bool IsTPILengthSet()const{return fTPILength!=fDefaultValue;};
  bool IsAmplitudeSet()const{return fAmplitude!=fDefaultValue;};
  bool IsTimeSet()const{return fTime!=fDefaultValue;};
  bool IsIntegralSet()const{return fIntegral!=fDefaultValue;};
  bool IsEnergySet()const{return fEnergy!=fDefaultValue;};
  bool IsPedestalSet()const{return fPedestal!=fDefaultValue;};
  bool IsTriggerTimeSet()const{return fTriggerTime!=fDefaultValue;};
  //@}

  private:
  

  private:
  TPulseIslandID fParentID;
  TAnalysedPulseID fTAPID;
  int fTPILength;
  double fAmplitude;
  double fTime;
  double fIntegral;
  double fEnergy;
  double fPedestal;
  double fTriggerTime;
  //IDs::source fSource;
  FlyWeight<IDs::source,Tag> fSource;
  //  std::vector<int> fSamples;

  /// \brief
  /// To enable sanity checks, we have an unphysical value that all
  /// fields are set to.
  static const int fDefaultValue = definitions::DefaultValue;

  ClassDef(TAnalysedPulse, 6);
};

#endif
