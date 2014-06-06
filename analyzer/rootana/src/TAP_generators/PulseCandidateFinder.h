#ifndef PulseCandidateFinder_h_
#define PulseCandidateFinder_h_

#include "TPulseIsland.h"

class PulseCandidateFinder {

 private:
  /// Location is a struct containing the start and stop location (in terms of sample number) of a candidate pulse
  struct Location {
    int start;
    int stop;
  };

 public:
  PulseCandidateFinder(TPulseIsland* pulse, int rise, int fall);
  ~PulseCandidateFinder();

 private:
  std::vector<Location> fPulseCandidateLocations;

 public:
  int GetNPulseCandidates() { return fPulseCandidateLocations.size(); }
};

#endif
