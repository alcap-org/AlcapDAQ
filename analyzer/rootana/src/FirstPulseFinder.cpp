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

  // Because FindPulses() is called for each channel individually, we can get some variables before we go into the loop
  std::string bankname = (*(theIslands.begin()))->GetBankName();
  std::string detname = TSetupData::Instance()->GetDetectorName(bankname);
  int pedestal = TSetupData::Instance()->GetPedestal(bankname);
  int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
  int max_digitised_value = std::pow(2, TSetupData::Instance()->GetNBits(bankname));

  for (PulseIslandList_t::const_iterator islandIter = theIslands.begin(); islandIter != theIslands.end(); ++islandIter) {

    ++fPulseCounter; // increase the pulse counter here

    // Things we need from the old pulse island
    std::vector<int> theSamples = (*islandIter)->GetSamples();
    
    // See if any samples are above the max digitisation value and if they are, we will skip to the next island
    bool ignore_island = false;
    for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
      if (*sampleIter >= max_digitised_value) {
	std::cout << "Pulse #" << fPulseCounter << ": has a sample with value " << *sampleIter << " which is greater than or equal to the maximum digitised value of " 
		  << max_digitised_value << " and so will be ignored." << std::endl;
	ignore_island = true;
	break;
      }
    }
    if (ignore_island)
      continue;

    int RMS = 20; // hard-coded for the time being

    // Histograms
    TH1F* new_pulses = NULL;
    TH1F* old_pulse = NULL;
    
    if (plot_pulses) {
      std::stringstream histname;
      histname << bankname << "_" << detname << "_SubPulses_" << fPulseCounter;
      new_pulses = new TH1F(histname.str().c_str(), histname.str().c_str(), theSamples.size(),0,theSamples.size());
      new_pulses->SetLineColor(kMagenta);
      histname.str("");

      histname << bankname << "_" << detname << "_OriginalIsland_" << fPulseCounter;
      old_pulse = new TH1F(histname.str().c_str(), histname.str().c_str(), theSamples.size(),0,theSamples.size());
    }
    
    // What we need for the new pulses
    int timestamp = 0;
    std::vector<int> newSamples;
    
    // Loop through the samples
    bool start_pulse = false;
    for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
      
      // Get the current height (taking into account pulse polarity)
      int height = trigger_polarity*(*(sampleIter) - pedestal);

      if (plot_pulses)
	old_pulse->Fill(sampleIter - theSamples.begin(), *sampleIter);
      
      if (start_pulse == false) { // if we haven't found a pulse yet then see if we do
	if (height > 0 + RMS) {
	  timestamp = sampleIter - theSamples.begin();
	  start_pulse = true;
	}
      }
      
      if (start_pulse == true) {
	// see if the pulse has ended
	if (height < 0) {
	  start_pulse = false; // the pulse is over
	  
	  // Add the TPulseIsland
	  output.push_back(new TPulseIsland(timestamp, newSamples, bankname));
	  
	  // Clear the old information
	  newSamples.clear();
	  timestamp = 0;
	}
	else {
	  newSamples.push_back(*sampleIter);
	  if (plot_pulses)
	    new_pulses->Fill(sampleIter - theSamples.begin(), *sampleIter);
	}
      }
    }
  }

  std::cout << "Bank Name: " << bankname << " DetName: " << detname << std::endl;
  std::cout << "Total number of pulse islands: " << theIslands.size() << std::endl;
  std::cout << "Total number of sub-pulses found: " << output.size() << std::endl;
  std::cout << std::endl;

  return output;
}
