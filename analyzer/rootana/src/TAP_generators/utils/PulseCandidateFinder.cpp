#include "PulseCandidateFinder.h"

#include "definitions.h"

#include <iostream>
#include <sstream>

/// PulseCandidateFinder()
/// The constructor just sets all the parameter values
PulseCandidateFinder::PulseCandidateFinder() {
}

/// FindPulseCandidates()
/// Finds the pulse candidates on the given TPulseIsland
void PulseCandidateFinder::FindPulseCandidates(TPulseIsland* pulse) {

  fPulseIsland = pulse;
  std::string detname = TSetupData::Instance()->GetDetectorName(fPulseIsland->GetBankName());
  IDs::channel theChannel = detname;

  // We have a different algorithm for fast and slow pulses
  if (theChannel == IDs::Fast) {

    int rise = 0; // the parameter for this algorithm
    switch(theChannel.Detector()) {
    case IDs::kMuSc:
      rise = 300;
      break;

    case IDs::kMuScA:
      rise = 100;
      break;

    case IDs::kNDet:
      rise = 100;
      break;

    case IDs::kGe:
      rise = 40;
      break;

    case IDs::kScL:
      rise = 20;
      break;

    case IDs::kScR:
      rise = 20;
      break;

    case IDs::kScGe:
      rise = 20;
      break;

    case IDs::kScVe:
      rise = 40;
      break;

    case IDs::kSiL2:
      rise = 80;
      break;

    case IDs::kSiL1_1:
      rise = 40;
      break;

    case IDs::kSiL1_2:
      rise = 20;
      break;

    case IDs::kSiL1_3:
      rise = 20;
      break;

    case IDs::kSiL1_4:
      rise = 80;
      break;

    case IDs::kSiR2:
      rise = 20;
      break;

    case IDs::kSiR1_1:
      rise = 40;
      break;

    case IDs::kSiR1_2:
      rise = 60;
      break;

    case IDs::kSiR1_3:
      rise = 60;
      break;

    case IDs::kSiR1_4:
      rise = 40;
      break;

    default:
      std::cout << "Error: PulseCandidateFinder currently does not have a rise parameter for " << detname << std::endl;
      break;
    }

    FindCandidatePulses_Fast(rise);
  }
  else if (theChannel == IDs::Slow) {

    int threshold = 0; // the parameter for this algorithm
    switch(theChannel.Detector()) {
    case IDs::kGe:
      threshold = 500;
      break;

    case IDs::kSiL2:
      threshold = 20;
      break;

    case IDs::kSiL1_1:
      threshold = 20;
      break;

    case IDs::kSiL1_2:
      threshold = 150;
      break;

    case IDs::kSiL1_3:
      threshold = 20;
      break;

    case IDs::kSiL1_4:
      threshold = 40;
      break;

    case IDs::kSiR2:
      threshold = 50;
      break;

    case IDs::kSiR1_1:
      threshold = 50;
      break;

    case IDs::kSiR1_2:
      threshold = 30;
      break;

    case IDs::kSiR1_3:
      threshold = 40;
      break;

    case IDs::kSiR1_4:
      threshold = 40;
      break;

    default:
      std::cout << "Error: PulseCandidateFinder currently does not have a threshold parameter for " << detname << std::endl;
      break;
    }

    FindCandidatePulses_Slow(threshold);
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
