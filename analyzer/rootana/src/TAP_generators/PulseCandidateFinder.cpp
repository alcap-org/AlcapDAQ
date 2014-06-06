#include "PulseCandidateFinder.h"

/// PulseCandidateFinder()
/// -- Pass it a TPulseIsland* and a rise and fall parameter to find the locations of the candidate pulses
PulseCandidateFinder::PulseCandidateFinder(TPulseIsland* pulse, int rise, int fall) {

  Location location;
  location.start = 0;
  location.stop = 1;

  fPulseCandidateLocations.push_back(location);

}

PulseCandidateFinder::~PulseCandidateFinder() {
}
