#include "PulseCandidateFinder.h"
#include "SetupNavigator.h"
#include "definitions.h"
#include "debug_tools.h"

#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include <TSQLiteServer.h>
#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

/// PulseCandidateFinder()
/// The constructor just sets all the parameter values
PulseCandidateFinder::PulseCandidateFinder(std::string detname, modules::options* opts): fChannel(detname) {

  if (fDefaultParameterValues.empty()) {
    SetDefaultParameterValues();
  }

  fNSigma = opts->GetInt("n_sigma", 0);
  if (fNSigma == 0) {
    fParameterValue = opts->GetInt(detname, fDefaultParameterValues[fChannel]); // set the parameter value for this channel
  }
  else {
    if (fOneSigmaValues.empty()) {
      SetOneSigmaValues();
    }
    fParameterValue = fNSigma * fOneSigmaValues[IDs::channel(detname)];
  }

  if (opts->HasOption("debug") && (opts->GetOption("debug").empty() || opts->GetBool("debug"))) {
    std::cout << "Parameter Value for " << fChannel << " PulseCandidateFinder is " << fParameterValue << std::endl; // would be nice to know which module this is for
  }
}

/// FindPulseCandidates()
/// Finds the pulse candidates on the given TPulseIsland
void PulseCandidateFinder::FindPulseCandidates(TPulseIsland* pulse) {

  // Clear the vector of pulse candidate locations first since it will still contain them from previous pulses
  fPulseCandidateLocations.clear();

  fPulseIsland = pulse;

  // We have a different algorithm for fast and slow pulses
  if (fChannel.isFast()) {
    FindCandidatePulses_Fast(fParameterValue);
  }
  else if (fChannel.isSlow()) {
    FindCandidatePulses_Slow(fParameterValue);
  }
  else {
    // this is a scintillator so do the fast pulse analysis
    FindCandidatePulses_Fast(fParameterValue);
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
  double pedestal = SetupNavigator::Instance()->GetPedestal(bankname);
  double noise = SetupNavigator::Instance()->GetPedestalError(bankname);
  int polarity = fPulseIsland->GetTriggerPolarity();

  int s1, s2, ds; // this sample value, the previous sample value and the change in the sample value
  bool found = false;
  Location location;

  // Loop through the samples
  int n_before_start_samples = 2; // take a few samples from before the official start
  for (unsigned int i = 1; i < n_samples; ++i) {
    s1 = polarity * (samples[i-1] - pedestal);
    s2 = polarity * (samples[i] - pedestal);
    ds = s2 - s1;

    if (found) {

      if (s2 < noise) { // stop if the sample goes below pedestal
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
  double pedestal = SetupNavigator::Instance()->GetPedestal(bankname);
  double noise = SetupNavigator::Instance()->GetPedestalError(bankname);
  int polarity = fPulseIsland->GetTriggerPolarity();

  int sample_height; // the sample's height above pedestal
  int start, stop; // the start and stop location
  bool found = false;
  Location location;

  // Loop through the samples
  for (unsigned int i = 0; i < n_samples; ++i) {
    sample_height = polarity * (samples[i] - pedestal);
    std::cout << "i = " << i << ", sample_height = " << sample_height << std::endl;
    if (found) {
      if (sample_height < noise) { // stop if the sample goes below pedestal
	location.stop = (int)i;
	if (location.stop >=(int) samples.size()) {
	  location.stop = samples.size() - 1;
	}
	std::cout << "Stop: location = " << location.stop << ", sample_height = " << sample_height << std::endl;
	start = stop = 0;
	fPulseCandidateLocations.push_back(location);
	found = false;
      }
      else if (i == n_samples-1) { // Also stop if we are at the last sample (only do this check if we failed the above one)
	location.stop = (int) i;
	std::cout << "Stop: location = " << location.stop << ", sample_height = " << sample_height << std::endl;
	fPulseCandidateLocations.push_back(location);
	found = false;
      }
    } else {
      if (sample_height > threshold) {
	found = true;

	// Track back until we get to the pedestal again so that we get the whole pulse (Ge-S pulses were being cut-off at the start)
	location.start = (int)(i);
	for (int j = i; j > 0; --j) {
	  std::cout << "j = " << j << std::endl;
	  sample_height = polarity * (samples[j] - pedestal);
	  if (sample_height <= 0) {
	    location.start = (int)(j);
	    break;
	  }
	}
	if (location.start < 0) {
	  location.start = 0;
	}
	std::cout << "Start: location = " << location.start << ", sample_height = " << sample_height << std::endl;
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
  if (theChannel.isFast()) {
    parameter_name = "SampleDifference";
    FillSampleDifferencesHistogram(histogram);
  }
  else if (theChannel.isSlow()) {
    parameter_name = "SampleHeight";
    FillSampleHeightsHistogram(histogram);
  }
  else {
    // it's a scintillator so plot the fast parameters
    parameter_name = "SampleDifference";
    FillSampleDifferencesHistogram(histogram);
  }

  std::stringstream histtitle;
  histtitle << "Plot of " << parameter_name << " for " << detname << " for Run " << SetupNavigator::Instance()->GetRunNumber();
  histogram->SetTitle(histtitle.str().c_str());
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

/// Need to declare this outside of any method
std::map<IDs::channel, int> PulseCandidateFinder::fDefaultParameterValues;

/// SetDefaultParameterValues
/// Sets the default parameter values
void PulseCandidateFinder::SetDefaultParameterValues() {

  // Set all the default values for the fast channels
  fDefaultParameterValues[IDs::channel("muSc")] = 300;
  fDefaultParameterValues[IDs::channel("muScA")] = 100;
  fDefaultParameterValues[IDs::channel("NDet")] = 100;

  fDefaultParameterValues[IDs::channel("Ge-F")] = 40;

  fDefaultParameterValues[IDs::channel("ScL")] = 15;
  fDefaultParameterValues[IDs::channel("ScR")] = 20;
  fDefaultParameterValues[IDs::channel("ScGe")] = 20;
  fDefaultParameterValues[IDs::channel("ScVe")] = 30;

  fDefaultParameterValues[IDs::channel("SiL2-F")] = 80;
  fDefaultParameterValues[IDs::channel("SiL1-1-F")] = 40;
  fDefaultParameterValues[IDs::channel("SiL1-2-F")] = 20;
  fDefaultParameterValues[IDs::channel("SiL1-3-F")] = 20;
  fDefaultParameterValues[IDs::channel("SiL1-4-F")] = 80;

  fDefaultParameterValues[IDs::channel("SiR2-F")] = 20;
  fDefaultParameterValues[IDs::channel("SiR1-1-F")] = 40;
  fDefaultParameterValues[IDs::channel("SiR1-2-F")] = 60;
  fDefaultParameterValues[IDs::channel("SiR1-3-F")] = 80;
  fDefaultParameterValues[IDs::channel("SiR1-4-F")] = 40;

  // Set all the default values for the slow parameters
  fDefaultParameterValues[IDs::channel("Ge-S")] = 500;

  fDefaultParameterValues[IDs::channel("SiL2-S")] = 20;
  fDefaultParameterValues[IDs::channel("SiL1-1-S")] = 20;
  fDefaultParameterValues[IDs::channel("SiL1-2-S")] = 150;
  fDefaultParameterValues[IDs::channel("SiL1-3-S")] = 20;
  fDefaultParameterValues[IDs::channel("SiL1-4-S")] = 40;

  fDefaultParameterValues[IDs::channel("SiR2-S")] = 50;
  fDefaultParameterValues[IDs::channel("SiR1-1-S")] = 50;
  fDefaultParameterValues[IDs::channel("SiR1-2-S")] = 30;
  fDefaultParameterValues[IDs::channel("SiR1-3-S")] = 40;
  fDefaultParameterValues[IDs::channel("SiR1-4-S")] = 40;
}

/// Need to declare this outside of any method
std::map<IDs::channel, double> PulseCandidateFinder::fOneSigmaValues;

/// SetOneSigmaValues()
/// Sets the one sigma values that we get from the text file
void PulseCandidateFinder::SetOneSigmaValues() {

  // The values that we will read in
  std::string run_number;
  std::string detname, bankname;
  std::string pedestal, noise;

  // Get the SQLite database file
  TSQLiteServer* server = new TSQLiteServer("sqlite://pedestals-and-noises.sqlite");

  std::stringstream query; 
  std::string tablename = "pedestals_and_noises";
  if (server) {

    query << "SELECT * FROM " << tablename << ";"; // get all the pedestals and noises
    TSQLiteResult* result = (TSQLiteResult*) server->Query(query.str().c_str());  // get the result of this query
    query.str(""); // clear the stringstream after use

    TSQLiteRow* row = (TSQLiteRow*) result->Next(); // get the first row
    while (row != NULL) {
      //      std::cout << row->GetField(0) << " " << row->GetField(1) << " " << row->GetField(2) << " " << row->GetField(3) << " " << row->GetField(4) << std::endl;
      run_number = row->GetField(0);
      detname = row->GetField(1);
      bankname = row->GetField(2);
      pedestal = row->GetField(3);
      noise = row->GetField(4);

      fOneSigmaValues[IDs::channel(detname)] = atof(noise.c_str());
      
      delete row;
      row = (TSQLiteRow*) result->Next(); // get the next row
    }
    delete result; // user has to delete the result
  }
  else {
    std::cout << "Error: Couldn't connect to SQLite database" << std::endl;
  }
}
