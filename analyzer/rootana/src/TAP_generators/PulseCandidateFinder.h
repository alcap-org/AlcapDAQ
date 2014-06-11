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
  PulseCandidateFinder(TPulseIsland* pulse);
  ~PulseCandidateFinder();

 private:
  TPulseIsland* fPulseIsland;
  std::vector<Location> fPulseCandidateLocations;


 public:
  int GetNPulseCandidates() { return fPulseCandidateLocations.size(); }
  std::vector<TPulseIsland*> GetPulseCandidates();

  // Our algorithms for finding pulse candidates
  void FindCandidatePulses_Fast(int rise);
  void FindCandidatePulses_Slow(int threshold);
};

#endif
