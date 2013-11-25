/********************************************************************\

  Name:         MSiPulseFinder
  Created by:   Andrew Edmonds

  Contents:     A module to plot the pulse heights of the silicon detector

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <cmath>
#include <iostream>
#include <sstream>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>

/* AlCap includes */
#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TSimpleSiPulse.h"
#include "DetectorMap.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MSiPulseFinder_init(void);
INT  MSiPulseFinder(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

vector<TSimpleSiPulse*> GetPulsesFromIsland(TSimpleSiPulse* island, string bankname);

extern HNDLE hDB;

static vector<TOctalFADCBankReader*> fadc_bank_readers;
map<string, int> theNSubSiPulseMap;
map<string, std::vector<TSimpleSiPulse*> > theSimpleSiPulseMap;

ANA_MODULE MSiPulseFinder_module =
{
  "MSiPulseFinder",                    /* module name           */
  "Andrew Edmonds",              /* author                */
  MSiPulseFinder,                      /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MSiPulseFinder_init,                 /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MSiPulseFinder_init()
{
  vector<string> bank_names = GetAllFADCBankNames();

  for(unsigned int i=0; i<bank_names.size(); i++) {
    fadc_bank_readers.push_back(new TOctalFADCBankReader(bank_names[i]));
  }

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
  * of TOctalFADCIsland objects from the raw Octal FADC data.
  */
INT MSiPulseFinder(EVENT_HEADER *pheader, void *pevent)
{
  // Clear the pulses from theSimpleSiPulseMap
  for (std::map<string, vector<TSimpleSiPulse*> >::iterator simpleSiMapIter = theSimpleSiPulseMap.begin();
		simpleSiMapIter != theSimpleSiPulseMap.end(); 
		simpleSiMapIter++) {
    
    	vector<TSimpleSiPulse*>& pulses = simpleSiMapIter->second;
    	// Delete the pointers to TSiSimplePulses, then clear the vector
    	for(unsigned int j=0; j<pulses.size(); j++) {
      	if(pulses[j]) { delete pulses[j]; pulses[j] = NULL; }
    	}
    	pulses.clear();
  }
  theSimpleSiPulseMap.clear(); // clear the pulse map so more than the first event is read
  
  theNSubSiPulseMap.clear(); // need to clear the NSubPulses map

  // Iterate through the banks readers
  for (std::vector<TOctalFADCBankReader*>::iterator bankReaderIter = fadc_bank_readers.begin();
  	   bankReaderIter != fadc_bank_readers.end(); bankReaderIter++) {
  	   
  	   (*bankReaderIter)->ProcessEvent(pheader, pevent);
  	   std::vector<TOctalFADCIsland*> theOctalFADCIslands = (*bankReaderIter)->GetIslandVectorCopy();
  	   std::string bankname = (*bankReaderIter)->GetBankName();
  	   
  	   // Have a vector ready for all the simple_si_pulses
  	   vector<TSimpleSiPulse*> simple_si_pulses;
  	   
  	   // Loop over the islands
  	   for (std::vector<TOctalFADCIsland*>::iterator octalFADCIslandIter = theOctalFADCIslands.begin();
  	   		octalFADCIslandIter != theOctalFADCIslands.end(); octalFADCIslandIter++) {
  	   		
  	   		// Create a TSimpleSiPulse for the island
  	   		unsigned int nped = 10;
  	   		TSimpleSiPulse* simple_si_island = new TSimpleSiPulse((*octalFADCIslandIter), ChannelToDetectorMap[bankname], nped);
    		
    		// Find all the pulses in this island
  	   		vector<TSimpleSiPulse*> simple_si_pulses_on_island = GetPulsesFromIsland(simple_si_island, bankname);
  	   		
  	   		// Create a bank and island name "bank" to store the number of sub pulses
  	   		std::stringstream bankislandname;
  	   		bankislandname << bankname << (octalFADCIslandIter - theOctalFADCIslands.begin() + 1);
  	   		theNSubSiPulseMap.insert(std::pair<string, int>(bankislandname.str(), simple_si_pulses_on_island.size())); // set the number of sub pulses on the island
  	   		
  	   		// Loop through and add the pulses to the main pulses vector
  	   		for (std::vector<TSimpleSiPulse*>::iterator iter = simple_si_pulses_on_island.begin(); 
  	   				iter != simple_si_pulses_on_island.end(); iter++) {
  	   		
  	   			simple_si_pulses.push_back(*iter);
  	   		}
  	   		
  	   }
  	   
  	   // Create a new "bank" to store the pulses that are on that island
       std::pair<std::string, std::vector<TSimpleSiPulse*> > thePair (bankname, simple_si_pulses);
       theSimpleSiPulseMap.insert(thePair);
  	   
  }

  return SUCCESS;
}

// GetPulsesFromIsland()
// -- Finds all the pulses on the island and returns them as a vector
vector<TSimpleSiPulse*> GetPulsesFromIsland(TSimpleSiPulse* island, string bankname) {
	
	// The vector of pulses that we will return
	std::vector<TSimpleSiPulse*> pulses;
	
	// Get the samples
	std::vector<int> theSamples = island->GetSampleVector();
	
	// Get the pedestal and threshold
	double pedestal = island->GetPedestal(); 
	double threshold = island->GetThreshold();
	
	// Loop through the samples
	bool pulse_start = false; // haven't started a pulse yet
	bool pulse_finish = false; // haven't finihsed a pulse yet
	int pulse_timestamp = 0;
	std::vector<int> pulse_samples;
	
	// NB end an element before the end so that the iterator isn't inspecting some random bit of memory
	for (std::vector<int>::iterator	 sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
	
		// If we get to the samples with value 0 we are at the end of the island and should exit the loop
		if ( (*sampleIter) == 0)
			break;
	    
	    // Start (positive/negative) pulse if:
	    //  - the current sample is greater/less than the pedestal
	    //  - the next sample is greater/less then the threshold
	    //  - we haven't already started a pulse
	    if (island->IsPositive() == true) {
	  		if ( *(sampleIter) > pedestal && *(sampleIter+1) > (pedestal + threshold) && pulse_start == false) {
	  	
	  			pulse_timestamp = island->GetTime() + (sampleIter - theSamples.begin());
	  			pulse_start = true;
	  		}
	  	}
	  	else {
	  		if ( *(sampleIter) < pedestal && *(sampleIter+1) < (pedestal - threshold) && pulse_start == false) {
	  	
	  			pulse_timestamp = island->GetTime() + (sampleIter - theSamples.begin());
	  			pulse_start = true;
	  		}
	  	}
	  		
	  	// If a pulse has been started, fill a new sample vector
	  	if (pulse_start == true) {
	  	
	  		pulse_samples.push_back(*sampleIter); // add this sample to the vector for the pulse
	  			
	  		// End the (positive/negative) pulse if:
	  		//  -- if the next bin is 0 (we'll be at the end of the island and it will mess up the mean)
	  		//  -- or the 3-bin mean is less/greater than the pedestal
	  		
	  		if ( *(sampleIter+1) == 0 || sampleIter == theSamples.end()-1) {
	  			pulse_start = false;
	  			pulse_finish = true;
	  		}
	  		else {
		  		double mean = ( *(sampleIter) + *(sampleIter+1) + *(sampleIter+2) ) / 3;
		  		if (island->IsPositive() == true) {
		  			if ( (mean < (pedestal - island->GetRMSNoise()) ) ) {
		  				pulse_start = false; // no longer have a pulse
	  					pulse_finish = true;
		  			}
		  		}
		  		else {
		  			if ( (mean > (pedestal + island->GetRMSNoise()) ) ) {
		  				pulse_start = false; // no longer have a pulse
	  					pulse_finish = true;
		  			}
		  		}
		  	}
	  	}
	  	
	  	if (pulse_finish == true) {
	  	
	  		// If there's only one sample then there's something wrong and it's not a pulse
	  		if (pulse_samples.size() > 1) {
	  			// Create the TSimpleSiPulse and add it to the vector of pulses
		  		TOctalFADCIsland* octal_pulse = new TOctalFADCIsland(pulse_timestamp, pulse_samples);
				TSimpleSiPulse* pulse = new TSimpleSiPulse(octal_pulse, ChannelToDetectorMap[bankname], island->GetPedestal()); 
		  		pulses.push_back(pulse);
		  		pulse_samples.clear();
		  		pulse_finish = false;
		  	}
		  	else {
		  		pulse_samples.clear();
		  		pulse_finish = false;
		  	}
		}
	}
	
	return pulses;
}
