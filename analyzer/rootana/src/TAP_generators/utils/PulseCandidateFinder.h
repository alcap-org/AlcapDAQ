#ifndef PulseCandidateFinder_h_
#define PulseCandidateFinder_h_

#include "ModulesOptions.h"
#include "TPulseIsland.h"
#include "definitions.h"

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Andrew Edmonds
///
/// \brief
/// Utility class to find pulse candidates on a TPulseIsland
///
/// \details
/// This utility class stores the location of candidate pulses of a TPulseIsland.
/// These candidate pulses can be retrieved later by the calling module.
/// 
/// Note that there are two algorithms used to find candidate pulses. For slow pulses, we 
/// simply look for when a sample has gone above a certain threshold and for fast pulses, we
/// look to see if there's a sudden rise in sample value between consecutive pulses.
///
/// The final thing this class does is to fill a given histogram with the relevant parameters
/// (either sample height or sample difference). This ability is used by the 
/// PulseCandidateFinder_InvestigateParameters module
////////////////////////////////////////////////////////////////////////////////
class PulseCandidateFinder {

 private:
  /// \brief
  /// Location is a struct containing the start and stop location (in terms of sample number) of a candidate pulse
  struct Location {
    int start;
    int stop;
  };

 public:
  /// \brief
  /// The constructor
  ///
  /// \detailed
  /// The constructor looks in the opts for any changes to the defailt parameter, "n_sigma" and "debug"
  ///
  /// \param[in] opts Describe the options this module takes.
  PulseCandidateFinder(std::string detname, modules::options* opts);

  /// \brief
  /// Empty destructor
  ~PulseCandidateFinder();

 private:
  /// \brief
  /// The TPulseIsland that we are looking for candidates on
  TPulseIsland* fPulseIsland;
  /// \brief
  /// The vector that we store the pulse candidate locations in
  std::vector<Location> fPulseCandidateLocations;


 public:
  /// \brief
  /// Find the pulse candidates on the given TPulseIsland
  void FindPulseCandidates(TPulseIsland* pulse);
  /// \brief
  /// Returns the number of pulse candidates that were found
  int GetNPulseCandidates() { return fPulseCandidateLocations.size(); }
  /// \brief
  /// Returns the actual TPulseIsland of each candidate
  //std::vector<TPulseIsland*> GetPulseCandidates();

  void GetPulseCandidates(std::vector<TPulseIsland*>&)const;

 private:
  /// \brief
  /// The algorithm for finding fast pulses
  ///
  /// \detailed
  /// Check the difference between consecutive samples and if it's above a certain value
  /// then it's that start of a candidate pulse. The end of the candidate pulse is when
  /// the samples go below pedestal
  void FindCandidatePulses_Fast(int rise);
  /// \brief
  /// The algorithm for finding slow pulses
  ///
  /// \detailed
  /// Check the height above pedestal of each sample and if it's above a certain value
  /// then it's that start of a candidate pulse. The end of the candidate pulse is when
  /// the samples go below pedestal
  void FindCandidatePulses_Slow(int threshold);

 public:
  /// \brief
  /// Takes a histogram and fills it with the parameters (either sample height above pedestals 
  /// or sample differences between consecutive samples)
  void FillParameterHistogram(TH1D* histogram);

 private:
  /// \brief
  /// Fills the given histogram with the sample differences between consecutive pulses
  void FillSampleDifferencesHistogram(TH1D* histogram);
  /// \brief
  /// Fills the given histogram with the sample's height above pedestal
  void FillSampleHeightsHistogram(TH1D* histogram);

 private:
  /// \brief
  /// The channel that this PulseCandidateFinder works on
  IDs::channel fChannel;
  /// \brief
  /// The value of the parameter to start a candidate pulse
  double fParameterValue;

  /// \brief
  /// The map that stores the default parameter values in case there isn't one specified in the modules file
  static std::map<IDs::channel, int> fDefaultParameterValues;
  /// \brief
  /// Called in the constructor if fDefaultParameterValues is empty, so that it can be filled
  void SetDefaultParameterValues();

  /// \brief
  /// The number of sigma (i.e. noise) that we want the threshold to be set to
  int fNSigma;
  /// \brief
  /// The map that stores the one sigma values that we get from the SQLite database
  static std::map<IDs::channel, double> fOneSigmaValues;
  /// \brief
  /// Called if "n_sigma" is specified as an option in the modules file and if fOneSigmaValues is empty
  void SetOneSigmaValues();
};

#endif
