#ifndef TVPULSEFINDER_H__
#define TVPULSEFINDER_H__

#include "TSetupData.h"
#include "TPulseIsland.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

class TVPulseFinder {
 protected:
  typedef std::vector<TPulseIsland*> PulseIslandList_t;
  typedef std::map<std::string, PulseIslandList_t > BankPulseList_t;

 public:
  TVPulseFinder(): fBankName(""), fDetName(""), fPedestal(0), fTriggerPolarity(0), fNBits(0), fPulseCounter(0),
    fCurrentSampleIter(0), fPulseStarted(false), fDoSanityChecks(true){};
  virtual ~TVPulseFinder(){};

  // The information from TSetupData
  std::string fBankName;
  std::string fDetName;
  int fPedestal;
  int fTriggerPolarity;
  int fNBits;

  long fPulseCounter;
  
  std::vector<int>::iterator fCurrentSampleIter;
  bool fPulseStarted;
  bool fDoSanityChecks; // set to false if you don't want to do the sanity checks (e.g. in the NullPulseFinder)


  virtual int CalculateTestValue()=0;
  virtual bool PassesStartCondition(int val)=0;
  virtual bool PassesStopCondition(int val)=0;

 public:

  // FindPulses()
  // -- Finds all pulses on an island and returns the vector of sub pulses
  // -- Gets called for each channel in every event
  PulseIslandList_t FindPulses(const PulseIslandList_t& theIslands, bool plot_pulses = false) {

    // Get the output ready
    PulseIslandList_t output;

    // Because FindPulses() is called for each channel individually, we can get the TSetupData variables here, before we go into the loop
    fBankName = (*(theIslands.begin()))->GetBankName();
    fDetName = TSetupData::Instance()->GetDetectorName(fBankName);
    fPedestal = TSetupData::Instance()->GetPedestal(fBankName);
    fTriggerPolarity = TSetupData::Instance()->GetTriggerPolarity(fBankName);
    fNBits = TSetupData::Instance()->GetNBits(fBankName);
    
    for (PulseIslandList_t::const_iterator islandIter = theIslands.begin(); islandIter != theIslands.end(); ++islandIter) {
      
      ++fPulseCounter; // increase the pulse counter here
    
      // Check that the pulse island passes the sanity checks
      if (fDoSanityChecks) {
	if (!PassesSanityChecks(*(islandIter)))
	  continue;
      }
      
      std::vector<int> theSamples = (*islandIter)->GetSamples();
      int island_timestamp = (*islandIter)->GetTimeStamp();

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
	
	fCurrentSampleIter = sampleIter;

	int test_value = CalculateTestValue(); // get the test value that we will use for the start/stop conditions
	
	if (plot_pulses)
	  old_pulse->Fill(sampleIter - theSamples.begin(), *fCurrentSampleIter);
	
	// If the pulse hasn't started yet, then check the current test_value against the start condition
	if (!fPulseStarted) {
	  if (PassesStartCondition(test_value)) {
	    timestamp = island_timestamp + (sampleIter - theSamples.begin());
	    fPulseStarted = true;
	  }
	}
	
	// If the pulse has started...
	if (fPulseStarted) {
	  // ... check against the stop condition 
	  // also want to make sure we aren't at the final sample (otherwise we will lose the last sub pulse if we are at the final sample)
	  if (PassesStopCondition(test_value) || (sampleIter == theSamples.end()-1)) {
	    fPulseStarted = false; // the pulse is over
	    
	    // Add the TPulseIsland
	    output.push_back(new TPulseIsland(timestamp, newSamples, fBankName));
	    
	    // Clear the old information
	    newSamples.clear();
	    timestamp = 0;
	  }
	  else {
	    // carry on adding samples to the pulse
	    newSamples.push_back(*fCurrentSampleIter);
	    if (plot_pulses)
	      new_pulses->Fill(sampleIter - theSamples.begin(), *fCurrentSampleIter);
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
  bool PassesSanityChecks(const TPulseIsland* island) {
    
    //////////////////
    // Sanity Check 1
    // See if any samples are above the max digitisation value
    int max_digitised_value = std::pow(2, fNBits) - 1;
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

};

#endif // TVPULSEFINDER_H__
