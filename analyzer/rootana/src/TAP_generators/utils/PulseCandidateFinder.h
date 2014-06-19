#ifndef PulseCandidateFinder_h_
#define PulseCandidateFinder_h_

#include "ModulesOptions.h"
#include "TPulseIsland.h"
#include "definitions.h"

class PulseCandidateFinder {

 private:
  /// Location is a struct containing the start and stop location (in terms of sample number) of a candidate pulse
  struct Location {
    int start;
    int stop;
  };

 public:
  PulseCandidateFinder(std::string detname, modules::options* opts);
  ~PulseCandidateFinder();

 private:
  /// Store the TPulseIsland and pulse candidate locations for one pulse at a time
  TPulseIsland* fPulseIsland;
  std::vector<Location> fPulseCandidateLocations;


 public:
  void FindPulseCandidates(TPulseIsland* pulse);
  int GetNPulseCandidates() { return fPulseCandidateLocations.size(); }
  std::vector<TPulseIsland*> GetPulseCandidates();

 private:
  // Our algorithms for finding pulse candidates
  void FindCandidatePulses_Fast(int rise);
  void FindCandidatePulses_Slow(int threshold);

 public:
  void FillParameterHistogram(TH1D* histogram);

 private:
  // Fill a histogram with either the sample differences or the 
  void FillSampleDifferencesHistogram(TH1D* histogram);
  void FillSampleHeightsHistogram(TH1D* histogram);

 private:
  /// The detector name and parameter value to use
  std::string fDetName;
  int fParameterValue;

};

#endif
