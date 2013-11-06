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

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>

/* AlCap includes */
#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MSiPulseFinder_init(void);
INT  MSiPulseFinder(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

vector<TOctalFADCIsland*> GetPulsesFromIsland(TOctalFADCIsland* island);
void GetPedestalAndRMS(std::vector<int> samples, double& pedestal, double& RMS);

extern HNDLE hDB;
extern TGlobalData* gData;

static vector<TOctalFADCBankReader*> fadc_bank_readers;

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

  // Iterate through the banks readers
  for (std::vector<TOctalFADCBankReader*>::iterator bankReaderIter = fadc_bank_readers.begin();
  	   bankReaderIter != fadc_bank_readers.end(); bankReaderIter++) {
  	   
  	   (*bankReaderIter)->ProcessEvent(pheader, pevent);
  	   std::vector<TOctalFADCIsland*> theOctalFADCIslands = (*bankReaderIter)->GetIslandVectorCopy();
  	   
  	   // Loop over the islands and fill the relevant histogram with the peak height
  	   for (std::vector<TOctalFADCIsland*>::iterator octalFADCIslandIter = theOctalFADCIslands.begin();
  	   		octalFADCIslandIter != theOctalFADCIslands.end(); octalFADCIslandIter++) {
  	   		
  	   		// Create a new "bank" to store the pulses
  	   		std:;string old_bank_name = (*bankReaderIter)->GetBankName();
  	   		std::string new_bank_name = old_bank_name + "P";
    		
    		// Find all the pulses in this island
  	   		vector<TOctalFADCIsland*> octal_fadc_pulses = GetPulsesFromIsland(*octalFADCIslandIter);
  	   		
  	   		// The pulses need converting from TOctalFADCIslands to TPulseIslands
  	   		std::vector<TPulseIsland*> pulses;
  	   		for (std::vector<TOctalFADCIsland*>::iterator theOctalPulseIter = octal_fadc_pulses.begin(); 
  	   			theOctalPulseIter != octal_fadc_pulses.end(); theOctalPulseIter++) {
  	   		
  	   			TPulseIsland* pulse = new TPulseIsland((*theOctalPulseIter)->GetTime(), (*theOctalPulseIter)->GetSampleVector(), GetClockTickForChannel(old_bank_name), new_bank_name);
  	   			
  	   			pulses.push_back(pulse);
  	   		}
  	   		
  	   		// Now add the new "banks" to gData for use by other modules
    		std::pair<std::string, std::vector<TPulseIsland*> > thePair (new_bank_name, pulses);
    		gData->fPulseIslandToChannelMap.insert(thePair);
  	   }
  	   
  }
  return SUCCESS;
}

// GetPulsesFromIsland()
// -- Finds all the pulses on the island and returns them as a vector
vector<TOctalFADCIsland*> GetPulsesFromIsland(TOctalFADCIsland* island) {
	
	// The vector of pulses that we will return
	std::vector<TOctalFADCIsland*> pulses;
	
	// Get the samples
	std::vector<int> theSamples = island->GetSampleVector();
	
	// Get the pedestal and RMS
	double pedestal = 0; double RMS = 0;
	GetPedestalAndRMS(theSamples, pedestal, RMS);
	
	// Set the level of sigma above pedestal that a sample must achieve for a pulse to be found
	int threshold_n_sigma = 5;
	
	// Loop through the samples
	bool pulse_found = false; // haven't found a pulse yet
	int pulse_timestamp = 0;
	std::vector<int> pulse_samples;
	
	// NB end an element before the end so that the iterator isn't inspecting some random bit of memory
	for (std::vector<int>::iterator	 sampleIter = theSamples.begin(); sampleIter != theSamples.end()-1; sampleIter++) {
	    
	    // Start pulse if:
	    //  - the current sample is less than the pedestal
	    //  - the next sample is greater then a certain sigma from pedestal
	    //  - we haven't already started a pulse
	    // NB assuming negative pulses !!!
	  	if ( *(sampleIter) < pedestal && *(sampleIter+1) < (pedestal - threshold_n_sigma*RMS) && pulse_found == false) {
	  	
	  		pulse_timestamp = island->GetTime() + (sampleIter - theSamples.begin());
	  		pulse_found = true;
	  	}
	  		
	  	// If a pulse has been found fill a new sample vector
	  	if (pulse_found == true) {
	  	
	  		pulse_samples.push_back( std::abs(*(sampleIter)-pedestal)); // add this sample to the vector for the pulse
	  		// NB pedestal subtracted and taken the absolute difference to get a positive pulse
	  			
	  		// End the pulse if:
	  		//  -- the 3-bin mean of this sample and the next two is less than the pedestal
	  		double mean = ( *(sampleIter) + *(sampleIter+1) + *(sampleIter+2) ) / 3;
	  		if ( (mean > pedestal - 1*RMS) ) {
	  			pulse_found = false; // no longer have a pulse
	  			
	  			// Create the TPulseIsland and add it to the vector of pulses
				TOctalFADCIsland* pulse = new TOctalFADCIsland(pulse_timestamp, pulse_samples); 
	  			pulses.push_back(pulse);
	  		}
	  	}
	}
	
	return pulses;
}

// GetPedestalAndRMS()
// -- Gets the pedestal as the mean of the first 10 bins
// -- and the RMS (i.e. standard deviation, sigma)
void GetPedestalAndRMS(std::vector<int> samples, double& pedestal, double& RMS) {

  int nBinsForMean = 10; // the number of bins to use for the mean

  // Get the mean
  pedestal = 0.0;
  for (int i = 0; i < nBinsForMean; i++) {
    pedestal += samples[i];
  }
  pedestal /= nBinsForMean;
  
  // Get the standard deviation
  RMS = 0.0;
  for (int i = 0; i < nBinsForMean; i++) {
    RMS += (samples[i] - pedestal)*(samples[i] - pedestal);
  }
  RMS /= nBinsForMean;
  RMS = std::sqrt(RMS);
}
