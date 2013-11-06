#ifndef FSIMPLEPULSEFINDER_H_
#define FSIMPLEPULSEFINDER_H_

/********************************************************************\

  Name:         FSimpleSiPulseFinder
  Created by:   Andrew Edmonds

  Contents:     A function to find pulses on a TSiSimplePulse island

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <cmath>
#include <iostream>

/* AlCap includes */
#include "TOctalFADCIsland.h"
#include "TSimpleSiPulse.h"

// FSiPulseFinder()
// -- Finds all the pulses on the island and returns them as a vector
std::vector<TSimpleSiPulse*> FSimpleSiPulseFinder(TSimpleSiPulse* island) {
	
	// The vector of pulses that we will return
	std::vector<TSimpleSiPulse*> pulses;
	
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
	    // NB assuming negative pulses !!!
	    if (island->IsPositive() == true) {
	  		if ( *(sampleIter) > pedestal && *(sampleIter+1) > threshold && pulse_found == false) {
	  	
	  			pulse_timestamp = island->GetTime() + (sampleIter - theSamples.begin());
	  			pulse_found = true;
	  		}
	  	}
	  	else {
	  		if ( *(sampleIter) < pedestal && *(sampleIter+1) < threshold && pulse_found == false) {
	  	
	  			pulse_timestamp = island->GetTime() + (sampleIter - theSamples.begin());
	  			pulse_found = true;
	  		}
	  	}
	  		
	  	// If a pulse has been found fill a new sample vector
	  	if (pulse_found == true) {
	  	
	  		pulse_samples.push_back( std::abs(*(sampleIter)-pedestal)); // add this sample to the vector for the pulse
	  		// NB pedestal subtracted and taken the absolute difference to get a positive pulse
	  			
	  		// End the (positive/negative) pulse if:
	  		//  -- the 3-bin mean is less/greater than the pedestal
	  		double mean = ( *(sampleIter) + *(sampleIter+1) + *(sampleIter+2) ) / 3;
	  		if (island->IsPositive() == true) {
	  			if ( (mean < pedestal) ) {
	  				pulse_found = false; // no longer have a pulse
	  			
	  				// Create the TSimpleSiPulse and add it to the vector of pulses
	  				TOctalFADCIsland* octal_pulse = new TOctalFADCIsland(pulse_timestamp, pulse_samples);
					TSimpleSiPulse* pulse = new TSimpleSiPulse(octal_pulse); 
	  				pulses.push_back(pulse);
	  			}
	  		}
	  		else {
	  			if ( (mean > pedestal) ) {
	  				pulse_found = false; // no longer have a pulse
	  			
	  				// Create the TSimpleSiPulse and add it to the vector of pulses
	  				TOctalFADCIsland* octal_pulse = new TOctalFADCIsland(pulse_timestamp, pulse_samples);
					TSimpleSiPulse* pulse = new TSimpleSiPulse(octal_pulse); 
	  				pulses.push_back(pulse);
	  			}
	  		}
	  	}
	}
	
	return pulses;
}


#endif
