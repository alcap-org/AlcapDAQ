#ifndef TPulseIsland_h
#define TPulseIsland_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>
#include "TSetupData.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// A uniform format for data from many different detectors.
/// \ingroup alcapana
/// \ingroup rootana
///
/// \details
/// The %TPulseIsland (TPI) class is a simple format for pulses
/// from many different detectors. The pulse shape is stored, the time stamp,
/// and the bank (digitizer channel) the pulse is from.
/// More information is stored in TSetupData (TSD) which TPIs depend on.
////////////////////////////////////////////////////////////////////////////////
class TPulseIsland : public TObject {
 private:
  /// Time ordered ADC samples of the pulse.
  std::vector<int> fSamples;
  /// The time, in sample clock ticks, of the first samples
  int fTimeStamp;
  /// The name describing the digitizer channel the pulse originated
  std::string fBankName;

 public:
  /// This defaultconstructor I believe exists so ROOT can load these from file and is not used explcitly.
  TPulseIsland();
  /// The constructor we should use.
  ///
  /// @param[in] timestamp The time in units of clock ticks of the first sample in the pulse
  /// @param[in] samples_vector Consecutive ADC values representing the pulse
  /// @param[in] bank_name The (4 character long) name identifying the digitizer channel
  /// the pulse originated.
  TPulseIsland(int timestamp, const std::vector<int>& samples_vector, 
               std::string bank_name);

  /// The TPI goes to the state the default constructor would set it to. Not generally used.
  void Reset(Option_t* o = "");

  /// \name Getters
  /// Return copies of all fields in the TPI and TSD.
  //@{
  std::vector<int> GetSamples() const { return fSamples; }
  int GetTimeStamp() const { return fTimeStamp; }
  std::string GetBankName() const { return fBankName; }

  double GetClockTickInNs() const { return TSetupData::Instance()->GetClockTick(fBankName); }
  int GetTriggerPolarity() const {return TSetupData::Instance()->GetTriggerPolarity(fBankName);};

  /// Deprecated
  double GetAmplitude() const;
  /// Deprecated
  double GetFastPulseAmplitude() const;
  /// Deprecated
  double GetSlowPulseAmplitude() const;

  /// @return Pedestal subtracted value of largest (smallest) sample for positive (negative) pulses.
  double GetPulseHeight() const;
  /// @return Time in nanoseconds of the peak of the pulse.
  double GetPulseTime() const;  
  /// @return An histogram representing the pulse for drawing or fitting.
  TH1I* GetPulseWaveform(std::string histname, std::string histtitle) const;
  int GetPulseLength() const { return fSamples.size(); }

  /// @return Index in samples vector corresponding to pulse peak.
  int GetPeakSample() const;
  /// @param[in] nPedSamples Deprecated
  /// @return Pedestal as stored in ODB
  double GetPedestal(int nPedSamples) const;
  //@}

 private:
  /// Copying is made explicitly private since we do not need it yet.
  TPulseIsland(const TPulseIsland& src);
  /// Assignment is made explicitly private since we do not need it yet.
  TPulseIsland operator=(const TPulseIsland& rhs); 

  ClassDef(TPulseIsland, 1);
};

#endif

/** @}*/
