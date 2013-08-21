#include "TPulseIsland.h"

using std::vector;
using std::string;

TPulseIsland::TPulseIsland()
{
  Reset();
}

TPulseIsland::TPulseIsland(
  int timestamp, const vector<int>& samples_vector,
  double clock_tick_in_ns, string bank_name)
{
  Reset();
  fTimeStamp = timestamp;
  fSamples = samples_vector;
  fClockTickInNs = clock_tick_in_ns;
  fBankName = bank_name;
}

void TPulseIsland::Reset(Option_t* o)
{
  fTimeStamp = 0;
  fSamples.clear();
  fClockTickInNs = 0.0;
  fBankName = "";
}
