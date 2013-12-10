/********************************************************************\

Name:         MAnalysePulseIsland
Created by:   Andrew Edmonds

Contents:     This module takes a vector of TPulseIsland* and analyses them into a vector of TAnalysedPulse* which can then be used for further analysis

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>

/* MIDAS includes */
#include "midas.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MAnalysePulseIsland_init(void);
INT  MAnalysePulseIsland_bor(INT);
INT  MAnalysePulseIsland(EVENT_HEADER*, void*);

// All Parameter Algorithms
void GetAllParameters_MaxBin(const TPulseIsland* pulse, double& amplitude, double& time, double& integral);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

ANA_MODULE MAnalysePulseIsland_module =
{
	"MAnalysePulseIsland",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MAnalysePulseIsland,                      /* event routine         */
	MAnalysePulseIsland_bor,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MAnalysePulseIsland_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MAnalysePulseIsland_init()
{
  // The following histograms are created for each channel:
  // hPulseHeights: ADC value (x-axis) vs number of pulses (y-axis)
  // hPulseTimes: time stamp (x-axis) vs number of pulses (y-axis)
  // hPulseRawCount: number of pulses (y-axis) - channels on x-axis?
  // hPulseShapes: sample number (x-axis) vs ADC value (y-axis) vs pulse (z-axis)

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

  }

  return SUCCESS;
}

// Resets the histograms at the beginning of each run so that the online display updates
INT MAnalysePulseIsland_bor(INT run_number) {
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MAnalysePulseIsland(EVENT_HEADER *pheader, void *pevent)
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

	// Loop over the map and get each bankname, vector pair
	for (map_iterator theMapIter = pulse_islands_map.begin(); theMapIter != pulse_islands_map.end(); theMapIter++) 
	{
	  std::string bankname = theMapIter->first;
	  std::string detname = gSetup->GetDetectorName(bankname);
	  std::vector<TPulseIsland*> thePulses = theMapIter->second;
			
	  // Loop over the TPulseIslands and analyse the TPulseIslands into TAnalysedPulses
	  for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); pulseIter++) {

	    // For the time being assume one TAnalysedPulse per TPulseIsland
	    double amplitude = 0;
	    double time = 0;
	    double integral = 0;

	    // If this is a slow pulse
	    if ( *(detname.end() - 1) == 'S' ) {
	      GetAllParameters_MaxBin( *pulseIter, amplitude, time, integral);
	    }
	    else if ( *(detname.end() -1) == 'F') {
	      GetAllParameters_MaxBin( *pulseIter, amplitude, time, integral);
	    }

	    TAnalysedPulse* analysedPulse = new TAnalysedPulse(amplitude, time, integral, detname);

	  }

	}
	return SUCCESS;
}


// GetAllParameters_MaxBin()
// -- Gets all the parameters for the pulse using the max bin method
void GetAllParameters_MaxBin(const TPulseIsland* pulse, double& amplitude, double& time, double& integral) {

  std::string bankname = pulse->GetBankName();
  double pedestal = gSetup->GetPedestal(bankname);
  int trigger_polarity = gSetup->GetTriggerPolarity(bankname);

  // First find the position of the peak
  std::vector<int> pulseSamples = pulse->GetSamples();
  int peak_sample_value = 0;
  int peak_sample_pos = 0;
  for (std::vector<int>::const_iterator sampleIter = pulseSamples.begin(); sampleIter != pulseSamples.end(); sampleIter++) {
    
    int this_height = trigger_polarity*(*(sampleIter) - pedestal);
    if ( this_height > peak_sample_value ) {
      peak_sample_value = this_height;
      peak_sample_pos = sampleIter - pulseSamples.begin();
    }
  }

  // Now assign the parameters
  amplitude = peak_sample_value;
  time = pulse->GetTimeStamp() + peak_sample_pos;
  integral = 0;
}
