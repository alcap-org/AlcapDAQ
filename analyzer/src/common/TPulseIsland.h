#ifndef TPulseIsland_h
#define TPulseIsland_h

#include <vector>
#include <string>

#include <TObject.h>

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

  ClassDef(TPulseIsland, 1);
};

#endif
