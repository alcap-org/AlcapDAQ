#ifndef PulseCandidateFinder_TSpectrum_h_
#define PulseCandidateFinder_TSpectrum_h_

#include "ModulesOptions.h"
#include "TPulseIsland.h"
#include "definitions.h"

#include "TSpectrum.h"

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author Andrew Edmonds
///
/// \brief
/// Utility class to find pulse candidates on a TPulseIsland using TSpectrum
///
/// \details
////////////////////////////////////////////////////////////////////////////////
class PulseCandidateFinder_TSpectrum {

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
  /// \brief Default constructor, make sure to set the channel name with
  /// SetChannel if you use this constructor
  PulseCandidateFinder_TSpectrum();
  PulseCandidateFinder_TSpectrum(std::string detname, modules::options* opts);

  /// \brief
  /// Empty destructor
  ~PulseCandidateFinder_TSpectrum();

  /// 
  void SetChannel(const std::string& detname);
  void SetSigma(double sigma);

  struct PeakLocation {
    double time;
    double amplitude;
  };

 private:
  /// \brief
  /// The TPulseIsland that we are looking for candidates on
  const TPulseIsland* fPulseIsland;
  /// \brief
  /// The vector that we store the pulse candidate locations in
  std::vector<Location> fPulseCandidateLocations;

  std::vector<PeakLocation> fPulseCandidatePeakLocations;

 public:
  /// \brief
  /// Find the pulse candidates on the given TPulseIsland
  void FindPulseCandidates(const TPulseIsland* pulse, TH1* hPulse = NULL);
  /// \brief
  /// Returns the number of pulse candidates that were found
  int GetNPulseCandidates() { return fPulseCandidateLocations.size(); }
  /// \brief
  /// Returns the actual TPulseIsland of each candidate
  //std::vector<TPulseIsland*> GetPulseCandidates();

  void GetPulseCandidates(std::vector<TPulseIsland*>&)const;

  const std::vector<PeakLocation>& GetPeakLocations()const {
    return fPulseCandidatePeakLocations;
  }

 private:
  /// \brief
  /// The channel that this PulseCandidateFinder_TSpectrum works on
  IDs::channel fChannel;

  TSpectrum* fTSpectrum;

  /// \brief
  /// Check if the pulse is overflowed
  ///
  /// \detailed
  /// Checks each sample and if the value is equal to the maximum ADC value then we've 
  /// overflowed the digitiser and can ignore this pulse
  bool CheckDigitiserOverflow();

};

#endif
