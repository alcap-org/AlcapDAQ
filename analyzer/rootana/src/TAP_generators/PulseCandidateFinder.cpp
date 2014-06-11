#include "PulseCandidateFinder.h"

#include "definitions.h"

#include <iostream>

/// PulseCandidateFinder()
/// This passes the TPulseIsland to the relevant pulse candidate finder
PulseCandidateFinder::PulseCandidateFinder(TPulseIsland* pulse): fPulseIsland(pulse) {

  IDs::channel theChannel = fPulseIsland->GetBankName();

  // We have a different algorithm for fast and slow pulses
  if (theChannel == IDs::Fast) {

    int rise = 0; // the parameter for this algorithm
    switch(theChannel.GetDetectorEnum()) {
    case kGe:
      rise = 100;
      break;
    }

    FindPulseCandidates_Fast(rise);
  }
  else if (theChannel == IDs::Slow) {

    int threshold = 0; // the parameter for this algorithm
    switch(theChannel.GetDetectorEnum()) {
    case kGe:
      threshold = 0;
      break;
    }

    FindPulseCandidates_Slow(threshold);
  }
}

PulseCandidateFinder::~PulseCandidateFinder() {
}

/// GetPulseCandidates()
/// Fills a vector of the TPulseIslands of the pulse candidates that have been found
std::vector<TPulseIsland*> PulseCandidateFinder::GetPulseCandidates() {

  // Get the output ready
  std::vector<TPulseIsland*> pulse_candidates;

  // Loop through the pulse candidate locations
  for (std::vector<Location>::iterator locationIter = fPulseCandidateLocations.begin(); locationIter != fPulseCandidateLocations.end(); ++locationIter) {
    
    // Get the start and stop locations
    int start_location = (*locationIter).start;
    int stop_location = (*locationIter).stop;

    // Get the bank name and work out what the new timestamp will be
    std::string bankname = fPulseIsland->GetBankName();
    int new_timestamp = fPulseIsland->GetTimeStamp() + start_location;

    // Get the samples from the original TPI and have a new one ready for the pulse candidate
    const std::vector<int>& theSamples = fPulseIsland->GetSamples();
    std::vector<int> theNewSamples;

    // Loop through the samples between the start and stop locations and add them to the new samples vector
    for (int iSample = start_location; iSample <= stop_location; ++iSample) {
      theNewSamples.push_back(theSamples.at(iSample));
    }

    TPulseIsland* pulse_island = new TPulseIsland(new_timestamp, theNewSamples, bankname);
    pulse_candidates.push_back(pulse_island);
  }

  return pulse_candidates;
}

/// FindPulseCandidates_Fast
/// Finds pulse candidates on fast pulse islands by looking for a sudden rise between consecutive samples
void PulseCandidateFinder::FindPulseCandidates_Fast(int rise) {

  const std::vector<int>& samples = fPulseIsland->GetSamples();
  unsigned int n_samples = samples.size();
  int pedestal = fPulseIsland->GetPedestal(0);
  int polarity = fPulseIsland->GetTriggerPolarity();

  int s1, s2, ds; // this sample value, the previous sample value and the change in the sample value
  int start, stop; // the start and stop location
  bool found = false;
  Location location;

  // Loop through the samples
  for (unsigned int i = 1; i < n_samples; ++i) {
    s1 = polarity * (samples[i-1] - pedestal);
    s2 = polarity * (samples[i] - pedestal);
    ds = s2 - s1;

    if (found) {
      if (s2 < 0) { // stop if the sample goes below pedestal
	location.stop = (int)i;
	start = stop = 0;
	fPulseCandidateLocations.push_back(location);
	found = false;
      }
    } else {
      if (ds > rise) {
	found = true;
	location.start = (int)(i - 1);
      }
    }
  }
}

/// FindPulseCandidates_Slow
/// Finds pulse candidates on slow pulse islands by seeing if a sample goes above a threshold
void PulseCandidateFinder::FindCandidatePulses_Slow(int threshold) {

  const std::vector<int>& samples = fPulseIsland->GetSamples();
  unsigned int n_samples = samples.size();
  int pedestal = fPulseIsland->GetPedestal(0);
  int polarity = fPulseIsland->GetTriggerPolarity();

  int sample_height; // the sample's height above pedestal
  int start, stop; // the start and stop location
  bool found = false;
  Location location;

  // Loop through the samples
  for (unsigned int i = 0; i < n_samples; ++i) {
    sample_height = polarity * (samples[i] - pedestal);

    if (found) {
      if (sample_height < 0) { // stop if the sample goes below pedestal
	location.stop = (int)i;
	start = stop = 0;
	fPulseCandidateLocations.push_back(location);
	found = false;
      }
    } else {
      if (sample_height > threshold) {
	found = true;
	location.start = (int)(i);
      }
    }
  }

}
