/********************************************************************\

Name:         MCommonOnlineDisplayPlots
Created by:   Andrew Edmonds

Contents:     One module that fills out histograms for the pulse heights, pulse shapes and the raw counts for all digitizer channels. These are all in one module to be more efficient in terms of minimising the number of times we loop through the channels.

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

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MCommonOnlineDisplayPlots_init(void);
INT  MCommonOnlineDisplayPlots(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH1I*> height_histograms_map;
map <std::string, TH1I*> timestamp_histograms_map;
map <std::string, TH2D*> shape_histograms_map;

ANA_MODULE MCommonOnlineDisplayPlots_module =
{
	"MCommonOnlineDisplayPlots",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MCommonOnlineDisplayPlots,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MCommonOnlineDisplayPlots_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MCommonOnlineDisplayPlots_init()
{
  // The following histograms are created for each channel:
  // hPulseHeights: ADC value (x-axis) vs number of pulses (y-axis)
  // hPulseTimestamps: time stamp (x-axis) vs number of pulses (y-axis)
  // hPulseRawCount: number of pulses (y-axis) - channels on x-axis?
  // hPulseShapes: sample number (x-axis) vs ADC value (y-axis) vs pulse (z-axis)

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    int n_digitizer_bits = 0;
    if (TSetupData::IsFADC(bankname))
      n_digitizer_bits = 12;
    else if (TSetupData::IsCAEN(bankname))
      n_digitizer_bits = 14; //?
    
    long max_adc_value = std::pow(2, n_digitizer_bits);

    // hPulseHeights
    std::string histname = "h" + bankname + "_Heights";
    std::string histtitle = "Plot of the pulse heights in the " + bankname + " channels";
    TH1I* hPulseHeights = new TH1I(histname.c_str(), histtitle.c_str(), max_adc_value,0,max_adc_value);
    hPulseHeights->GetXaxis()->SetTitle("Pulse Height [ADC value]");
    hPulseHeights->GetYaxis()->SetTitle("Number of Pulses");
    height_histograms_map[bankname] = hPulseHeights;

    // hPulseTimestamps
    histname = "h" + bankname + "_Timestamps";
    histtitle = "Plot of the pulse timestamps in the " + bankname + " channels";
    TH1I* hPulseTimestamps = new TH1I(histname.c_str(), histtitle.c_str(), 1000,0,1e6);
    hPulseTimestamps->GetXaxis()->SetTitle("Time Stamp");
    hPulseTimestamps->GetYaxis()->SetTitle("Number of Pulses");
    timestamp_histograms_map[bankname] = hPulseTimestamps;

    // hPulseShapes
    histname = "h" + bankname + "_Shapes";
    histtitle = "Plot of the pulse shapes in the " + bankname + " channels";
    TH2D* hPulseShapes = new TH2D(histname.c_str(), histtitle.c_str(), 256,0.5,256.5,max_adc_value,-0.5,max_adc_value-0.5);      
    hPulseShapes->GetXaxis()->SetTitle("Time Stamp");
    hPulseShapes->GetYaxis()->SetTitle("ADC Value");
    shape_histograms_map[bankname] = hPulseShapes;
  }

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MCommonOnlineDisplayPlots(EVENT_HEADER *pheader, void *pevent)
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
	  std::vector<TPulseIsland*> thePulses = theMapIter->second;
			
	  // Loop over the TPulseIslands and plot the histogram
	  for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); pulseIter++) {
			
	    // Make sure the histograms exist and then fill them
	    if (height_histograms_map.find(bankname) != height_histograms_map.end())
	      height_histograms_map[bankname]->Fill((*pulseIter)->GetPulseHeight());

	    if (timestamp_histograms_map.find(bankname) != timestamp_histograms_map.end())
	      timestamp_histograms_map[bankname]->Fill((*pulseIter)->GetTimeStamp());

	    if (shape_histograms_map.find(bankname) != shape_histograms_map.end()) {
	      
	      std::vector<int> theSamples = (*pulseIter)->GetSamples();
	      for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
		shape_histograms_map[bankname]->Fill(sampleIter - theSamples.begin(), (*sampleIter));
	      }
	    }
	  }
	}
	return SUCCESS;
}
