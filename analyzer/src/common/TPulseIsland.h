#ifndef TPulseIsland_h
#define TPulseIsland_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>
#include "TSetupData.h"

extern TSetupData* gSetup;

class TPulseIsland : public TObject {
  private:
  std::vector<int> fSamples; // adc samples
  int fTimeStamp; // time stamp in clock ticks
  std::string fBankName; // FADC Bank name of this island's channel

  public:
  TPulseIsland();
  TPulseIsland(int timestamp, const std::vector<int>& samples_vector, std::string bank_name);

  void Reset(Option_t* o = "");

  std::vector<int> GetSamples() const { return fSamples; }
  int GetTimeStamp() const { return fTimeStamp; }
  std::string GetBankName() const { return fBankName; }

  ClassDef(TPulseIsland, 1);
};

#endif
