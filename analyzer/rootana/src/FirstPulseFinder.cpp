#include "FirstPulseFinder.h"

#include <iostream>
#include <cmath>
#include <sstream>

#include "TH1.h"

// FindPulses()
// -- Finds all pulses on an island and returns the vector of sub pulses
// -- Gets called for each channel in every event
FirstPulseFinder::PulseIslandList_t FirstPulseFinder::FindPulses(const PulseIslandList_t& theIslands) {

  // Get the output ready
  PulseIslandList_t output;

  bool plot_pulses = false;

  // Because FindPulses() is called for each channel individually, we can get the TSetupData variables here, before we go into the loop
  fBankName = (*(theIslands.begin()))->GetBankName();
  fDetName = TSetupData::Instance()->GetDetectorName(fBankName);
  fPedestal = TSetupData::Instance()->GetPedestal(fBankName);
  fTriggerPolarity = TSetupData::Instance()->GetTriggerPolarity(fBankName);
  fNBits = TSetupData::Instance()->GetNBits(fBankName);

  for (PulseIslandList_t::const_iterator islandIter = theIslands.begin(); islandIter != theIslands.end(); ++islandIter) {

    ++fPulseCounter; // increase the pulse counter here
    
    // Check that the pulse island passes the sanity checks
    if (!PassesSanityChecks(*(islandIter)))
      continue;

    std::vector<int> theSamples = (*islandIter)->GetSamples();
    int RMS = 20; // hard-coded for the time being

    // Histograms
    TH1F* new_pulses = NULL;
    TH1F* old_pulse = NULL;
    
    if (plot_pulses) {
      std::stringstream histname;
      histname << fBankName << "_" << fDetName << "_SubPulses_" << fPulseCounter;
      new_pulses = new TH1F(histname.str().c_str(), histname.str().c_str(), theSamples.size(),0,theSamples.size());
      new_pulses->SetLineColor(kMagenta);
      histname.str("");

      histname << fBankName << "_" << fDetName << "_OriginalIsland_" << fPulseCounter;
      old_pulse = new TH1F(histname.str().c_str(), histname.str().c_str(), theSamples.size(),0,theSamples.size());
    }
    
    // What we need for the new pulses
    int timestamp = 0;
    std::vector<int> newSamples;
    
    // Loop through the samples
    fPulseStarted = false;
    for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {

      fCurrentSample = *(sampleIter);

      int height = CalculateTestValue(); // get the test value that we will use for the start/stop conditions

      if (plot_pulses)
	old_pulse->Fill(sampleIter - theSamples.begin(), fCurrentSample);
      
      // If the pulse hasn't started yet, then check the current height against the start condition
      if (!fPulseStarted) {
	if (PassesStartCondition(height)) {
	  timestamp = sampleIter - theSamples.begin();
	  fPulseStarted = true;
	}
      }
      
      // If the pulse has started...
      if (fPulseStarted) {
	// ... check against the stop condition 
	if (PassesStopCondition(height)) {
	  fPulseStarted = false; // the pulse is over
	  
	  // Add the TPulseIsland
	  output.push_back(new TPulseIsland(timestamp, newSamples, fBankName));
	  
	  // Clear the old information
	  newSamples.clear();
	  timestamp = 0;
	}
	else {
	  // carry on adding samples to the pulse
	  newSamples.push_back(fCurrentSample);
	  if (plot_pulses)
	    new_pulses->Fill(sampleIter - theSamples.begin(), fCurrentSample);
	}
      }
    }
  }

  std::cout << "Bank Name: " << fBankName << " DetName: " << fDetName << std::endl;
  std::cout << "Total number of pulse islands: " << theIslands.size() << std::endl;
  std::cout << "Total number of sub-pulses found: " << output.size() << std::endl;
  std::cout << std::endl;

  return output;
}

// PassesSanityChecks()
// -- Checks if the pulse island passes the following sanity checks:
//      1. overflowed the digitizer (i.e. any sample is >= 2^n, where n is the number of bits in the digitiser)
// -- Returns: true if the pulse island passes and false if it fails
bool FirstPulseFinder::PassesSanityChecks(const TPulseIsland* island) {

  //////////////////
  // Sanity Check 1
  // See if any samples are above the max digitisation value
  int max_digitised_value = std::pow(2, fNBits);
  std::vector<int> theSamples = island->GetSamples();

  for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
    if (*sampleIter >= max_digitised_value) {
      std::cout << "Pulse #" << fPulseCounter << ": has a sample with value " << *sampleIter << " which is greater than or equal to the maximum digitised value of " 
		<< max_digitised_value << " and so will be ignored." << std::endl;
      return false;
    }
  }
  // End Sanity Check 1
  ///////////////////////////////////

  return true; // passed everything
}

// CalculateTestValue()
// -- Calculates the test value that we will use to check against the start/stop conditions
// -- Returns the test value
int FirstPulseFinder::CalculateTestValue() {

  // Return the current height (taking into account pulse polarity)
  return fTriggerPolarity*(fCurrentSample - fPedestal);
}


// PassesStartCondition()
// -- Checks to see if the value given has passed the start condition
// -- Returns true if it does pass and false if it doesn't
bool FirstPulseFinder::PassesStartCondition(int val) {

  int RMS = 20; // hard-coded for the time being

  if (val > 0 + RMS)
    return true;
  else
    return false;
}


// PassesStopCondition()
// -- Checks to see if the value given has passed the stop condition
// -- Returns true if it does pass and false if it doesn't
bool FirstPulseFinder::PassesStopCondition(int val) {

  if (val < 0)
    return true;
  else
    return false;
}
