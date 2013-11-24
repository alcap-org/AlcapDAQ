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

// GetPulseHeight()
// -- Gets the position of the peak from GetPeakSample() and then returns the pulse height
double TPulseIsland::GetPulseHeight() const {
  
  double pedestal = GetPedestal(10); // get the pedestal

  // Go through the samples and get the samples with the largest difference between it and the pedestal
  // (should take into account both positive and negative pulses)
 
  return std::abs(fSamples.at(GetPeakSample()) - pedestal);
}

// GetPulseTime()
// -- Gets the position of the peak from GetPeakSample() and adds that number of elements to the time stamp
// -- It also calibrates the time to ns using fClockTickInNs
double TPulseIsland::GetPulseTime() const {

  return (fTimeStamp + GetPeakSample()) * fClockTickInNs;
}


// GetPeakSample()
// -- Goes through the samples and finds the element with the largest difference from the pedestal
//    so it should take into account both positive and negative pulses
// -- It returns its position in the fSamples vector
int TPulseIsland::GetPeakSample() const {

  double pedestal = GetPedestal(10);
  double peak_sample_value = 0;
  double peak_sample_pos = 0;
  for (std::vector<int>::const_iterator sampleIter = fSamples.begin(); sampleIter != fSamples.end(); sampleIter++) {
  
    double this_height = std::abs(*(sampleIter) - pedestal);
    if ( this_height > peak_sample_value ) {
      peak_sample_value = this_height;
      peak_sample_pos = sampleIter - fSamples.begin();
    }
  }

  return peak_sample_pos;
}

// GetPedestal()
// -- Calculates the pedestal for this TPulseIsland using the given number of bins
double TPulseIsland::GetPedestal(int nPedSamples) const {

  double pedestal = 0;
  for (int iSample = 0; iSample < nPedSamples; iSample++) {
    pedestal += fSamples.at(iSample);
  }

  pedestal /= nPedSamples;

  return pedestal;
}
