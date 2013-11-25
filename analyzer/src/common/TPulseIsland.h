#ifndef TPulseIsland_h
#define TPulseIsland_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>

class TPulseIsland : public TObject {
  private:
  std::vector<int> fSamples; // adc samples
  double fClockTickInNs; // clock tick in nanoseconds
  int fTimeStamp; // time stamp in clock ticks
  std::string fBankName; // FADC Bank name of this island's channel

  public:
  TPulseIsland();
  TPulseIsland(int timestamp, const std::vector<int>& samples_vector,
    double clock_tick_in_ns, std::string bank_name);

  void Reset(Option_t* o = "");

  std::vector<int> GetSamples() const { return fSamples; }
  double GetClockTickInNs() const { return fClockTickInNs; }
  int GetTimeStamp() const { return fTimeStamp; }
  std::string GetBankName() const { return fBankName; }
  
  double GetPulseHeight() const;
  double GetPulseTime() const;  
  TH1I* GetPulseWaveform(std::string histname, std::string histtitle) const;

  int GetPeakSample() const;
  double GetPedestal(int nPedSamples) const;
  ClassDef(TPulseIsland, 1);
};

#endif
