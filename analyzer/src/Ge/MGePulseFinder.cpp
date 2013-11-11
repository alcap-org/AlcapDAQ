/********************************************************************\

  Name:         MGePulseFinder
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
#include "TGlobalData.h"
#include "TSimpleGePulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MGePulseFinder_init(void);
INT  MGePulseFinder(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

vector<TSimpleGePulse*> GetPulsesFromIsland(TSimpleGePulse* island);

extern HNDLE hDB;
extern TGlobalData* gData;

static vector<TOctalFADCBankReader*> fadc_bank_readers;
map<string, int> theNSubGePulseMap;
map<string, std::vector<TSimpleGePulse*> > theSimpleGePulseMap;

ANA_MODULE MGePulseFinder_module =
{
  "MGePulseFinder",                    /* module name           */
  "Andrew Edmonds",              /* author                */
  MGePulseFinder,                      /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MGePulseFinder_init,                 /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MGePulseFinder_init()
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
INT MGePulseFinder(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;

  // Some typedefs
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map =
    gData->fPulseIslandToChannelMap;
  
  // Clear the pulses from theSimpleGePulseMap
  for (std::map<string, vector<TSimpleGePulse*> >::iterator simpleGeMapIter = theSimpleGePulseMap.begin();
		simpleGeMapIter != theSimpleGePulseMap.end(); 
		simpleGeMapIter++) {
    
    	vector<TSimpleGePulse*>& pulses = simpleGeMapIter->second;
    	// Delete the pointers to TSiSimplePulses, then clear the vector
    	for(unsigned int j=0; j<pulses.size(); j++) {
      	if(pulses[j]) { delete pulses[j]; pulses[j] = NULL; }
    	}
    	pulses.clear();
  }
  theSimpleGePulseMap.clear(); // clear the pulse map so more than the first event is read
  
  theNSubGePulseMap.clear(); // need to clear the NSubPulses map

  // Iterate through the banks readers
  for (std::vector<TOctalFADCBankReader*>::iterator bankReaderIter = fadc_bank_readers.begin();
  	   bankReaderIter != fadc_bank_readers.end(); bankReaderIter++) {
  	   
  	   (*bankReaderIter)->ProcessEvent(pheader, pevent);
  	   std::vector<TOctalFADCIsland*> theOctalFADCIslands = (*bankReaderIter)->GetIslandVectorCopy();
  	   std::string bankname = (*bankReaderIter)->GetBankName();
  	   
  	   // Have a vector ready for all the simple_ge_pulses
  	   vector<TSimpleGePulse*> simple_ge_pulses;
  	   
  	   // Loop over the islands
  	   for (std::vector<TOctalFADCIsland*>::iterator octalFADCIslandIter = theOctalFADCIslands.begin();
  	   		octalFADCIslandIter != theOctalFADCIslands.end(); octalFADCIslandIter++) {
  	   		
  	   		// Create a TSimpleGePulse for the island
  	   		unsigned int nped = 10;
  	   		TSimpleGePulse* simple_ge_island = new TSimpleGePulse((*octalFADCIslandIter), nped);
    		
    		// Find all the pulses in this island
  	   		vector<TSimpleGePulse*> simple_ge_pulses_on_island = GetPulsesFromIsland(simple_ge_island);
  	   		
  	   		// Create a bank and island name "bank" to store the number of sub pulses
  	   		std::stringstream bankislandname;
  	   		bankislandname << bankname << (octalFADCIslandIter - theOctalFADCIslands.begin() + 1);
  	   		theNSubGePulseMap.insert(std::pair<string, int>(bankislandname.str(), simple_ge_pulses_on_island.size())); // set the number of sub pulses on the island
  	   		
  	   		// Loop through and add the pulses to the main pulses vector
  	   		for (std::vector<TSimpleGePulse*>::iterator iter = simple_ge_pulses_on_island.begin(); 
  	   				iter != simple_ge_pulses_on_island.end(); iter++) {
  	   		
  	   			simple_ge_pulses.push_back(*iter);
  	   		}
  	   		
  	   }
  	   
  	   // Create a new "bank" to store the pulses that are on that island
       std::pair<std::string, std::vector<TSimpleGePulse*> > thePair (bankname, simple_ge_pulses);
       theSimpleGePulseMap.insert(thePair);
  	   
  }

  return SUCCESS;
}

// GetPulsesFromIsland()
// -- Finds all the pulses on the island and returns them as a vector
vector<TSimpleGePulse*> GetPulsesFromIsland(TSimpleGePulse* island) {
	
	// The vector of pulses that we will return
	std::vector<TSimpleGePulse*> pulses;
	
	// Get the samples
	std::vector<int> theSamples = island->GetSampleVector();
	
	// Get the pedestal and threshold
	double pedestal = island->GetPedestal(); 
	double threshold = island->GetThreshold();
	
	// Loop through the samples
	bool pulse_found = false; // haven't found a pulse yet
	int pulse_timestamp = 0;
	std::vector<int> pulse_samples;
	
	// NB end an element before the end so that the iterator isn't inspecting some random bit of memory
	for (std::vector<int>::iterator	 sampleIter = theSamples.begin(); sampleIter != theSamples.end()-1; sampleIter++) {
	    
	    // Start (positive/negative) pulse if:
	    //  - the current sample is greater/less than the pedestal
	    //  - the next sample is greater/less then the threshold
	    //  - we haven't already started a pulse
	    if (island->IsPositive() == true) {
	  		if ( *(sampleIter) > pedestal && *(sampleIter+1) > pedestal + threshold && pulse_found == false) {
	  	
	  			pulse_timestamp = island->GetTime() + (sampleIter - theSamples.begin());
	  			pulse_found = true;
	  		}
	  	}
	  	else {
	  		if ( *(sampleIter) < pedestal && *(sampleIter+1) < pedestal - threshold && pulse_found == false) {
	  	
	  			pulse_timestamp = island->GetTime() + (sampleIter - theSamples.begin());
	  			pulse_found = true;
	  		}
	  	}
	  		
	  	// If a pulse has been found fill a new sample vector
	  	if (pulse_found == true) {
	  	
	  		pulse_samples.push_back(*sampleIter); // add this sample to the vector for the pulse
	  			
	  		// End the (positive/negative) pulse if:
	  		//  -- the 3-bin mean is less/greater than the pedestal
	  		double mean = ( *(sampleIter) + *(sampleIter+1) + *(sampleIter+2) ) / 3;
	  		if (island->IsPositive() == true) {
	  			if ( (mean < pedestal) ) {
	  				pulse_found = false; // no longer have a pulse
	  				
	  				// Create the TSimpleGePulse and add it to the vector of pulses
	  				TOctalFADCIsland* octal_pulse = new TOctalFADCIsland(pulse_timestamp, pulse_samples);
					TSimpleGePulse* pulse = new TSimpleGePulse(octal_pulse, island->GetPedestal()); 
	  				pulses.push_back(pulse);
	  			}
	  		}
	  		else {
	  			if ( (mean > pedestal) ) {
	  				pulse_found = false; // no longer have a pulse
	  				
	  				// Create the TSimpleGePulse and add it to the vector of pulses
	  				TOctalFADCIsland* octal_pulse = new TOctalFADCIsland(pulse_timestamp, pulse_samples);
					TSimpleGePulse* pulse = new TSimpleGePulse(octal_pulse, island->GetPedestal()); 
	  				pulses.push_back(pulse);
	  				pulse_samples.clear();
	  			}
	  		}
	  	}
	}
	
	return pulses;
}
