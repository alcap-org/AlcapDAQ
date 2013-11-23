#include "TPulseIsland.h"

#include <cmath>

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

double TPulseIsland::GetPulseHeight() const {
  
  double pedestal = GetPedestal(10); // get the pedestal

  // Go through the samples and get the samples with the largest difference between it and the pedestal
  // (should take into account both positive and negative pulses)
  double pulseheight = 0;
  for (std::vector<int>::const_iterator sampleIter = fSamples.begin(); sampleIter != fSamples.end(); sampleIter++) {
  
    double this_height = std::abs(*(sampleIter) - pedestal);
    if ( this_height > pulseheight )
      pulseheight = this_height;
  }

  return pulseheight;
}

double TPulseIsland::GetPulseTime() const {

  return fTimeStamp * fClockTickInNs;
}

double TPulseIsland::GetPedestal(int nPedSamples) const {

  double pedestal = 0;
  for (int iSample = 0; iSample < nPedSamples; iSample++) {
    pedestal += fSamples.at(iSample);
  }

  pedestal /= nPedSamples;

  return pedestal;
}
