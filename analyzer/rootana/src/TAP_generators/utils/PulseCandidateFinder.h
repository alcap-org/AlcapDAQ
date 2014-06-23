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
  /// The channel and parameter value to use
  IDs::channel fChannel;
  int fParameterValue;

  /// The map that stores the default parameter values in case there isn't one specified in the modules file
  static std::map<IDs::channel, int> fDefaultParameterValues;
  void SetDefaultParameterValues();

  /// The number of sigma (i.e. noise) that we want the threshold to be set to
  /// and the map that stores the one sigma values that we get from the text file
  int fNSigma;
  static std::map<IDs::channel, double> fOneSigmaValues;
  void SetOneSigmaValues();
};

#endif
