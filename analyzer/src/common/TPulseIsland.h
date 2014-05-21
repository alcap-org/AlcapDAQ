#ifndef TPulseIsland_h
#define TPulseIsland_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>
#include "TSetupData.h"

class TPulseIsland : public TObject {
  private:
  std::vector<int> fSamples; // adc samples
  int fTimeStamp; // time stamp in clock ticks
  std::string fBankName; // FADC Bank name of this island's channel

  // These values are the same for all pulses coming from the same channel in a
  // run so at some point we should remove them and get them from the run's
  // TSetupData.
  // Make sure to use the getters for them in the meantime
  //double fClockTickInNs; // clock tick in nanoseconds
  //double fADCValueInMeV; // ADC value in MeV (assuming linear calibration)
  //  int fPolarity; // Trigger threshold polarity (i.e. the polarity of the initial pulse coming out of the detector)

  public:
  TPulseIsland();
  TPulseIsland(int timestamp, const std::vector<int>& samples_vector, 
               std::string bank_name);

  void Reset(Option_t* o = "");

  std::vector<int> GetSamples() const { return fSamples; }
  int GetTimeStamp() const { return fTimeStamp; }
  std::string GetBankName() const { return fBankName; }

  double GetClockTickInNs() const { return TSetupData::Instance()->GetClockTick(fBankName); }
  //int GetADCValueInMeV() const { return fADCValueInMeV; }
  int GetTriggerPolarity() const {return TSetupData::Instance()->GetTriggerPolarity(fBankName);};

  double GetAmplitude() const;
  double GetFastPulseAmplitude() const;
  double GetSlowPulseAmplitude() const;

  double GetPulseHeight() const;
  double GetPulseTime() const;  
  TH1I* GetPulseWaveform(std::string histname, std::string histtitle) const;
  int GetPulseLength() const { return fSamples.size(); }

  int GetPeakSample() const;
  double GetPedestal(int nPedSamples) const;

 private:
  TPulseIsland(const TPulseIsland& src);
  TPulseIsland operator=(const TPulseIsland& rhs); 

  ClassDef(TPulseIsland, 1);
};

#endif
