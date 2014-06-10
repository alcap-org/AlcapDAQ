#include "PulseCandidateFinder.h"

#include <iostream>

/// PulseCandidateFinder()
/// Passed a TPulseIsland* and will look through it for samples that increase by as much as "rise"
PulseCandidateFinder::PulseCandidateFinder(TPulseIsland* pulse, int rise): fPulseIsland(pulse) {

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
      if (s2 < pedestal) { // stop if the sample goes below pedestal
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

/// FillSampleDifferencesHistogram()
/// For investigating what values to use for the rise and fall parameters use this function
/// to fill a histogram with all the sample differences
void PulseCandidateFinder::FillSampleDifferencesHistogram(TH1D* h_sample_differences) {

  const std::vector<int>& samples = fPulseIsland->GetSamples();
  unsigned int n_samples = samples.size();
  int pedestal = fPulseIsland->GetPedestal(0);
  int polarity = fPulseIsland->GetTriggerPolarity();

  int s1, s2, ds; // this sample value, the previous sample value and the change in the sample value
  int start, stop; // the start and stop location
  
  // Loop through the samples
  for (unsigned int i = 1; i < n_samples; ++i) {
    s1 = polarity * (samples[i-1] - pedestal);
    s2 = polarity * (samples[i] - pedestal);
    ds = s2 - s1;

    h_sample_differences->Fill(ds);
  }
}
