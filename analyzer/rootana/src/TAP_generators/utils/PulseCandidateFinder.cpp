#include "PulseCandidateFinder.h"
#include "SetupNavigator.h"
#include "definitions.h"
#include "debug_tools.h"
#include "AlcapExcept.h"

#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include <TSQLiteServer.h>
#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

MAKE_EXCEPTION(PulseCandidateFinder, Base);
MAKE_EXCEPTION(UnspecifiedChannel,PulseCandidateFinder);

/// The constructor just sets all the parameter values
PulseCandidateFinder::PulseCandidateFinder():
    fChannel(IDs::kErrorDetector),
    fParameterValue(definitions::DefaultValue),
    fNoise(definitions::DefaultValue),
    fPedestal(definitions::DefaultValue),
    fNSigma(definitions::DefaultValue){

  // make sure we have default values
  if (fDefaultParameterValues.empty()) {
    SetDefaultParameterValues();
  }

}

/// The constructor just sets all the parameter values
PulseCandidateFinder::PulseCandidateFinder(std::string detname, modules::options* opts) {

  // make sure we have default values
  if (fDefaultParameterValues.empty()) {
    SetDefaultParameterValues();
  }

  SetChannel(detname);

  // Check options to set the values cut
  if(opts->HasOption("n_sigma")){
     SetSigma( opts->GetInt("n_sigma"));
  } else if(opts->HasOption(detname+"_param")){
    fParameterValue = opts->GetDouble(detname+"_param"); 
  }
  
  // print debugging statements
  if (opts->GetFlag("debug")){
    std::cout << "Parameter Value for " << fChannel 
              << " PulseCandidateFinder is " 
              << fParameterValue << std::endl; // would be nice to know which module this is for
  }
}

void PulseCandidateFinder::SetSigma( double sigma){
    fNSigma=sigma;
    fParameterValue=fNSigma*fNoise;
}

void PulseCandidateFinder::SetChannel(const std::string& detname){
  fChannel=detname;
  fNoise = SetupNavigator::Instance()->GetNoise(fChannel);
  fPedestal = SetupNavigator::Instance()->GetPedestal(fChannel);
  fParameterValue = fDefaultParameterValues[fChannel]; // set the parameter value for this channel
}

/// FindPulseCandidates()
/// Finds the pulse candidates on the given TPulseIsland
void PulseCandidateFinder::FindPulseCandidates(const TPulseIsland* pulse) {
    if(!fChannel.isValid()){
        std::cout<<"Channel for PCF is not set, make sure to use "
                   "SetChannel() if you used the defualt constructor"<<std::endl;
        throw Except::UnspecifiedChannel();
    }

  // Clear the vector of pulse candidate locations first since it will still contain them from previous pulses
  fPulseCandidateLocations.clear();

  fPulseIsland = pulse;

  // If we are using the "n_sigma" option, then all channels should use the slow pulse algorithm
  if (fNSigma != 0) {
    FindCandidatePulses_Slow(fParameterValue);
  }
  else { // We have a different algorithm for fast and slow pulses
    if (fChannel.isFast()) {
      if (fChannel.Detector() != IDs::kGe) {
	FindCandidatePulses_Slow(fParameterValue); // use the slow algorithm for the fast silicon pulses because of the noisy pedestal
      }
      else {
	FindCandidatePulses_Fast(fParameterValue);
      }
    }
    else if (fChannel.isSlow()) {
      FindCandidatePulses_Slow(fParameterValue);
    }
    else {
      // this is a scintillator so do the fast pulse analysis
      FindCandidatePulses_Fast(fParameterValue);
    }
  }
}

PulseCandidateFinder::~PulseCandidateFinder() {
}

/// GetPulseCandidates()
/// Fills a vector of the TPulseIslands of the pulse candidates that have been found
void PulseCandidateFinder::GetPulseCandidates(std::vector<TPulseIsland*>& pulse_candidates)const {

  // Loop through the pulse candidate locations
  std::vector<int>::const_iterator start_new, stop_new;
  for (std::vector<Location>::const_iterator locationIter = fPulseCandidateLocations.begin();
          locationIter != fPulseCandidateLocations.end(); ++locationIter) {
    
    // Get iterator to the start locations
    start_new=fPulseIsland->GetSamples().begin();
    std::advance(start_new,locationIter->start);

    // Get iterator to the stop locations
    stop_new=fPulseIsland->GetSamples().begin();
    std::advance(stop_new,locationIter->stop);

    // Get the bank name and work out what the new timestamp will be
    std::string bankname = fPulseIsland->GetBankName();
    int new_timestamp = fPulseIsland->GetTimeStamp() + locationIter->start;

    // Make the new TPI
    int index=locationIter-fPulseCandidateLocations.begin();
    if((int)pulse_candidates.size() <= index || !pulse_candidates.at(index)){
        TPulseIsland* pulse_island = new TPulseIsland(new_timestamp, start_new, stop_new, bankname);
        pulse_candidates.push_back(pulse_island);
    }else{
        TPulseIsland* pulse_island=pulse_candidates.at(index);
        pulse_island->SetSamples(start_new,stop_new);
        pulse_island->SetTimeStamp(new_timestamp);
        pulse_island->SetBankName(bankname);
    }
  }
  for(unsigned int i=fPulseCandidateLocations.size();i< pulse_candidates.size();++i){
      pulse_candidates.at(i)->Reset();
  }
}

/// FindCandidatePulses_Fast
/// Finds pulse candidates on fast pulse islands by looking for a sudden rise between consecutive samples
void PulseCandidateFinder::FindCandidatePulses_Fast(int rise) {

  const std::vector<int>& samples = fPulseIsland->GetSamples();
  unsigned int n_samples = samples.size();

  std::string bankname = fPulseIsland->GetBankName();
  int polarity = fPulseIsland->GetTriggerPolarity();

  int s1, s2, ds; // this sample value, the previous sample value and the change in the sample value
  bool found = false;
  Location location;

  // Loop through the samples
  int n_before_start_samples = 2; // take a few samples from before the official start
  for (unsigned int i = 1; i < n_samples; ++i) {
    s1 = polarity * (samples[i-1] - fPedestal);
    s2 = polarity * (samples[i] - fPedestal);
    ds = s2 - s1;

    if (found) {

      if (s2 < fNoise) { // stop if the sample goes below pedestal
	location.stop = (int)i;
	if (location.stop >= (int)samples.size()) {
	  location.stop = samples.size() - 1;
	}

	fPulseCandidateLocations.push_back(location);
	found = false;
      }
      else if (i == n_samples-1) { // Also stop if we are at the last sample (only do this check if we failed the above one)
	location.stop = (int) i;

	fPulseCandidateLocations.push_back(location);
	found = false;
      }
    } else {
      if (ds > rise) {
	found = true;
	location.start = (int)(i - 1) - n_before_start_samples;

	if (location.start < 0) {
	  location.start = 0;
	}
      }
    }
  }
}

/// FindCandidatePulses_Slow
/// Finds pulse candidates on slow pulse islands by seeing if a sample goes above a threshold
void PulseCandidateFinder::FindCandidatePulses_Slow(int threshold) {

  const std::vector<int>& samples = fPulseIsland->GetSamples();
  unsigned int n_samples = samples.size();

  std::string bankname = fPulseIsland->GetBankName();
  int polarity = fPulseIsland->GetTriggerPolarity();

  int sample_height; // the sample's height above pedestal
  int start, stop; // the start and stop location
  bool found = false;
  Location location;

  // Loop through the samples
  for (unsigned int i = 0; i < n_samples; ++i) {
    sample_height = polarity * (samples[i] - fPedestal);

    if (found) {
      if (sample_height < fNoise) { // stop if the sample goes below pedestal
	location.stop = (int)i;
	if (location.stop >=(int) samples.size()) {
	  location.stop = samples.size() - 1;
	}

	start = stop = 0;
	fPulseCandidateLocations.push_back(location);
	found = false;
      }
      else if (i == n_samples-1) { // Also stop if we are at the last sample (only do this check if we failed the above one)
	location.stop = (int) i;

	fPulseCandidateLocations.push_back(location);
	found = false;
      }
    } else {
      if (sample_height > threshold) {
	found = true;

	// Track back until we get to the pedestal again so that we get the whole pulse (Ge-S pulses were being cut-off at the start)
	location.start = (int)(i);
	for (int j = i; j > 0; --j) {

	  sample_height = polarity * (samples[j] - fPedestal);
	  if (sample_height <= 0) {
	    location.start = (int)(j);
	    break;
	  }
	  else if (j == 1) { // if we are at the last iteration
	    location.start = 0; // set the location to the start since we haven't gone below pedestal again
	  }
	}
	if (location.start < 0) {
	  location.start = 0;
	}
      }
    }
  }
}


/// FillParameterHistogram
/// Fills the given histogram with the relevant parameter based on the channel
void PulseCandidateFinder::FillParameterHistogram(TH2D* histogram) {

  std::string detname = TSetupData::Instance()->GetDetectorName(fPulseIsland->GetBankName());
  IDs::channel theChannel = detname;

  // Get some information from the TPI
  const std::vector<int>& samples = fPulseIsland->GetSamples();
  unsigned int n_samples = samples.size();
  int polarity = fPulseIsland->GetTriggerPolarity();

  int s1, s2, ds; // this sample value, the previous sample value and the change in the sample value

  // Loop through the samples and plot the 2D plot
  for (unsigned int i = 1; i < n_samples; ++i) {
    s1 = polarity * (samples[i-1] - fPedestal);
    s2 = polarity * (samples[i] - fPedestal);
    ds = s2 - s1;

    histogram->Fill(ds, s1);
  }

  std::stringstream histtitle;
  histtitle << "Plot of sample differences vs sample heights for " << detname << " for Run " << SetupNavigator::Instance()->GetRunNumber();
  histogram->SetTitle(histtitle.str().c_str());
  histogram->GetYaxis()->SetTitle("Sample Heights");
  histogram->GetXaxis()->SetTitle("Sample Differences");
}

/// Need to declare this outside of any method
std::map<IDs::channel, int> PulseCandidateFinder::fDefaultParameterValues;

/// SetDefaultParameterValues
/// Sets the default parameter values
void PulseCandidateFinder::SetDefaultParameterValues() {

  // Set all the default values for the fast channels
  fDefaultParameterValues[IDs::channel("muSc")] = 230;
  fDefaultParameterValues[IDs::channel("muScA")] = 100;
  fDefaultParameterValues[IDs::channel("NDet")] = 100;
  fDefaultParameterValues[IDs::channel("NDet2")] = 100;

  fDefaultParameterValues[IDs::channel("Ge-F")] = 40;

  fDefaultParameterValues[IDs::channel("ScL")] = 100;
  fDefaultParameterValues[IDs::channel("ScR")] = 100;
  fDefaultParameterValues[IDs::channel("ScGe")] = 20;
  fDefaultParameterValues[IDs::channel("ScVe")] = 100;

  fDefaultParameterValues[IDs::channel("SiL2-F")] = 0;
  fDefaultParameterValues[IDs::channel("SiL1-1-F")] = 0;
  fDefaultParameterValues[IDs::channel("SiL1-2-F")] = 0;
  fDefaultParameterValues[IDs::channel("SiL1-3-F")] = 0;
  fDefaultParameterValues[IDs::channel("SiL1-4-F")] = 0;

  fDefaultParameterValues[IDs::channel("SiR2-F")] = 0;
  fDefaultParameterValues[IDs::channel("SiR1-1-F")] = 0;
  fDefaultParameterValues[IDs::channel("SiR1-2-F")] = 0;
  fDefaultParameterValues[IDs::channel("SiR1-3-F")] = 0;
  fDefaultParameterValues[IDs::channel("SiR1-4-F")] = 0;

  // Set all the default values for the slow parameters
  fDefaultParameterValues[IDs::channel("Ge-S")] = 500;

  fDefaultParameterValues[IDs::channel("SiL2-S")] = 0;
  fDefaultParameterValues[IDs::channel("SiL1-1-S")] = 0;
  fDefaultParameterValues[IDs::channel("SiL1-2-S")] = 0;
  fDefaultParameterValues[IDs::channel("SiL1-3-S")] = 0;
  fDefaultParameterValues[IDs::channel("SiL1-4-S")] = 0;

  fDefaultParameterValues[IDs::channel("SiR2-S")] = 0;
  fDefaultParameterValues[IDs::channel("SiR1-1-S")] = 0;
  fDefaultParameterValues[IDs::channel("SiR1-2-S")] = 0;
  fDefaultParameterValues[IDs::channel("SiR1-3-S")] = 0;
  fDefaultParameterValues[IDs::channel("SiR1-4-S")] = 0;
}
