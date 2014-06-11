#include "PulseCandidateFinder.h"

#include <iostream>

/// PulseCandidateFinder()
/// This passes the TPulseIsland to the relevant pulse candidate finder
PulseCandidateFinder::PulseCandidateFinder(TPulseIsland* pulse): fPulseIsland(pulse) {

  IDs::channel theChannel = fPulseIsland->GetBankName();

  // We have a different algorithm for fast and slow pulses
  if (theChannel == IDs::Fast) {

    double parameter; // the parameter for this algorithm
    switch(theChannel.GetDetectorEnum()) {
    case kGe:
      parameter = 0;
      break;
    }

    FindPulseCandidates_Fast(parameter);
  }
  else if (theChannel == IDs::Slow) {

    double parameter; // the parameter for this algorithm
    switch(theChannel.GetDetectorEnum()) {
    case kGe:
      parameter = 0;
      break;
    }

    FindPulseCandidates_Slow(parameter);
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
