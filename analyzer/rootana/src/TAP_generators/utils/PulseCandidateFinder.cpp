#include "PulseCandidateFinder.h"

#include "definitions.h"

#include <iostream>
#include <sstream>

/// PulseCandidateFinder()
/// The constructor just sets all the parameter values
PulseCandidateFinder::PulseCandidateFinder(std::string detname, modules::options* opts): fDetName(detname) {

  // Set all the rise parameter values
  /*  fRiseValues[IDs::kMuSc] = opts->GetInt("muSc", 300);
  fRiseValues[IDs::kMuScA] = opts->GetInt("muScA", 100);
  fRiseValues[IDs::kNDet] = opts->GetInt("NDet", 100);

  fRiseValues[IDs::kGe] = opts->GetInt("Ge-F", 40);

  fRiseValues[IDs::kScL] = opts->GetInt("ScL", 20);
  fRiseValues[IDs::kScR] = opts->GetInt("ScR", 20);
  fRiseValues[IDs::kScGe] = opts->GetInt("ScGe", 20);
  fRiseValues[IDs::kScVe] = opts->GetInt("ScVe", 40);

  fRiseValues[IDs::kSiL2] = opts->GetInt("SiL2-F", 80);
  fRiseValues[IDs::kSiL1_1] = opts->GetInt("SiL1-1-F", 40);
  fRiseValues[IDs::kSiL1_2] = opts->GetInt("SiL1-2-F", 20);
  fRiseValues[IDs::kSiL1_3] = opts->GetInt("SiL1-3-F", 20);
  fRiseValues[IDs::kSiL1_4] = opts->GetInt("SiL1-4-F", 80);

  fRiseValues[IDs::kSiR2] = opts->GetInt("SiR2-F", 20);
  fRiseValues[IDs::kSiR1_1] = opts->GetInt("SiR1-1-F", 40);
  fRiseValues[IDs::kSiR1_2] = opts->GetInt("SiR1-2-F", 60);
  fRiseValues[IDs::kSiR1_3] = opts->GetInt("SiR1-3-F", 80);
  fRiseValues[IDs::kSiR1_4] = opts->GetInt("SiR1-4-F", 40);

  // Set all the threshold parameter values
  fThresholdValues[IDs::kGe] = opts->GetInt("Ge-S", 500);

  fThresholdValues[IDs::kSiL2] = opts->GetInt("SiL2-S", 20);
  fThresholdValues[IDs::kSiL1_1] = opts->GetInt("SiL1-1-S", 20);
  fThresholdValues[IDs::kSiL1_2] = opts->GetInt("SiL1-2-S", 150);
  fThresholdValues[IDs::kSiL1_3] = opts->GetInt("SiL1-3-S", 20);
  fThresholdValues[IDs::kSiL1_4] = opts->GetInt("SiL1-4-S", 40);

  fThresholdValues[IDs::kSiR2] = opts->GetInt("SiR2-S", 50);
  fThresholdValues[IDs::kSiR1_1] = opts->GetInt("SiR1-1-S", 50);
  fThresholdValues[IDs::kSiR1_2] = opts->GetInt("SiR1-2-S", 30);
  fThresholdValues[IDs::kSiR1_3] = opts->GetInt("SiR1-3-S", 40);
  fThresholdValues[IDs::kSiR1_4] = opts->GetInt("SiR1-4-S", 40);

  if (opts->HasOption("debug") && (opts->GetOption("debug").empty() || opts->GetBool("debug"))) {
    std::cout << "Parameter Values for PulseCandidateFinder" << std::endl; // would be nice to know which module this is for
    for (int iDet = IDs::kGe; iDet != IDs::kMuScA; ++iDet) {
      std::cout << IDs::channel::GetDetectorString(static_cast<IDs::Detector_t>(iDet)) << std::endl;
      std::cout << "\tThreshold = " << fThresholdValues[iDet] << std::endl;
      std::cout << "\tRise = " << fRiseValues[iDet] << std::endl;
    }
  }
  */
}

/// FindPulseCandidates()
/// Finds the pulse candidates on the given TPulseIsland
void PulseCandidateFinder::FindPulseCandidates(TPulseIsland* pulse) {

  // Clear the vector of pulse candidate locations first since it will still contain them from previous pulses
  fPulseCandidateLocations.clear();

  fPulseIsland = pulse;
  std::string detname = TSetupData::Instance()->GetDetectorName(fPulseIsland->GetBankName());
  IDs::channel theChannel = detname;

  // We have a different algorithm for fast and slow pulses
  if (theChannel == IDs::Fast) {

    int rise = 0;//fRiseValues[theChannel.Detector()]; // the parameter for this algorithm
    if (rise == 0) {
      std::cout << "Error: PulseCandidateFinder currently does not have a rise parameter for " << detname << std::endl;
    }
    else {
      FindCandidatePulses_Fast(rise);
    }
  }
  else if (theChannel == IDs::Slow) {

    int threshold = 0;//fThresholdValues[theChannel.Detector()]; // the parameter for this algorithm
    if (threshold == 0) {
      std::cout << "Error: PulseCandidateFinder currently does not have a threshold parameter for " << detname << std::endl;
    }
    else {
      FindCandidatePulses_Slow(threshold);
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

/// FindCandidatePulses_Fast
/// Finds pulse candidates on fast pulse islands by looking for a sudden rise between consecutive samples
void PulseCandidateFinder::FindCandidatePulses_Fast(int rise) {

  const std::vector<int>& samples = fPulseIsland->GetSamples();
  unsigned int n_samples = samples.size();
  int pedestal = fPulseIsland->GetPedestal(0);
  int polarity = fPulseIsland->GetTriggerPolarity();

  int s1, s2, ds; // this sample value, the previous sample value and the change in the sample value
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

/// FindCandidatePulses_Slow
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


/// FillParameterHistogram
/// Fills the given histogram with the relevant parameter based on the channel
void PulseCandidateFinder::FillParameterHistogram(TH1D* histogram) {

  std::string detname = TSetupData::Instance()->GetDetectorName(fPulseIsland->GetBankName());
  IDs::channel theChannel = detname;

  std::string parameter_name = "Unknown";

  // We have a different algorithm for fast and slow pulses
  if (theChannel == IDs::Fast) {
    parameter_name = "SampleDifference";
    FillSampleDifferencesHistogram(histogram);
  }
  else if (theChannel == IDs::Slow) {
    parameter_name = "SampleHeight";
    FillSampleHeightsHistogram(histogram);
  }

  std::string histtitle = "Plot of " + parameter_name + " for " + detname + " for Run 2808";
  histogram->SetTitle(histtitle.c_str());
  histogram->GetYaxis()->SetTitle("");
  histogram->GetXaxis()->SetTitle(parameter_name.c_str());
}

/// FillSampleDifferencesHistogram
/// Fills the given histogram with the differences between consecutive samples
void PulseCandidateFinder::FillSampleDifferencesHistogram(TH1D* histogram) {

  const std::vector<int>& samples = fPulseIsland->GetSamples();
  unsigned int n_samples = samples.size();
  int pedestal = fPulseIsland->GetPedestal(0);
  int polarity = fPulseIsland->GetTriggerPolarity();

  int s1, s2, ds; // this sample value, the previous sample value and the change in the sample value

  // Loop through the samples
  for (unsigned int i = 1; i < n_samples; ++i) {
    s1 = polarity * (samples[i-1] - pedestal);
    s2 = polarity * (samples[i] - pedestal);
    ds = s2 - s1;

    histogram->Fill(ds);
  }
}

/// FillSampleHeightsHistogram
/// Fills the given histogram with the heights between consecutive samples
void PulseCandidateFinder::FillSampleHeightsHistogram(TH1D* histogram) {

  const std::vector<int>& samples = fPulseIsland->GetSamples();
  unsigned int n_samples = samples.size();
  int pedestal = fPulseIsland->GetPedestal(0);
  int polarity = fPulseIsland->GetTriggerPolarity();

  int sample_height; // the height of this sample above pedestal

  // Loop through the samples
  for (unsigned int i = 0; i < n_samples; ++i) {
    sample_height = polarity * (samples[i] - pedestal);

    histogram->Fill(sample_height);
  }
}
