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
INT  MCommonOnlineDisplayPlots_bor(INT);
INT  MCommonOnlineDisplayPlots(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH1I*> height_histograms_map;
map <std::string, TH1I*> time_histograms_map;
map <std::string, TH2D*> shape_histograms_map;
map <std::string, TH1I*> latest_pulse_histograms_map;
static TH1I* hPulseRawCount;

ANA_MODULE MCommonOnlineDisplayPlots_module =
{
	"MCommonOnlineDisplayPlots",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MCommonOnlineDisplayPlots,                      /* event routine         */
	MCommonOnlineDisplayPlots_bor,                          /* BOR routine           */
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
  // hPulseTimes: time stamp (x-axis) vs number of pulses (y-axis)
  // hPulseRawCount: number of pulses (y-axis) - channels on x-axis?
  // hPulseShapes: sample number (x-axis) vs ADC value (y-axis) vs pulse (z-axis)

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    int n_digitizer_bits = 0;
    if (TSetupData::IsFADC(bankname))
      n_digitizer_bits = 12;
    else if (TSetupData::IsHoustonCAEN(bankname))
      n_digitizer_bits = 14; //?
    else if (TSetupData::IsBostonCAEN(bankname))
      n_digitizer_bits = 12;
    
    long max_adc_value = std::pow(2, n_digitizer_bits);

    // hPulseHeights
    std::string histname = "h" + bankname + "_Heights";
    std::string histtitle = "Plot of the pulse heights in the " + bankname + " channels";
    TH1I* hPulseHeights = new TH1I(histname.c_str(), histtitle.c_str(), max_adc_value,0,max_adc_value);
    hPulseHeights->GetXaxis()->SetTitle("Pulse Height [ADC value]");
    hPulseHeights->GetYaxis()->SetTitle("Number of Pulses");
    height_histograms_map[bankname] = hPulseHeights;

    // hPulseTimes
    histname = "h" + bankname + "_Times";
    histtitle = "Plot of the pulse times in the " + bankname + " channels";
    TH1I* hPulseTimes = new TH1I(histname.c_str(), histtitle.c_str(), 1000,0,1e6);
    hPulseTimes->GetXaxis()->SetTitle("Time");
    hPulseTimes->GetYaxis()->SetTitle("Number of Pulses");
    time_histograms_map[bankname] = hPulseTimes;

    // hPulseShapes
    histname = "h" + bankname + "_Shapes";
    histtitle = "Plot of the pulse shapes in the " + bankname + " channels";
    TH2D* hPulseShapes = new TH2D(histname.c_str(), histtitle.c_str(), 64,-0.5,63.5,max_adc_value,-0.5,max_adc_value-0.5);      
    hPulseShapes->GetXaxis()->SetTitle("Time");
    hPulseShapes->GetYaxis()->SetTitle("ADC Value");
    shape_histograms_map[bankname] = hPulseShapes;

    //hLatestPulse
    histname = "h" + bankname + "_LatestPulse";
    histtitle = "Plot of the latest pulse in the " + bankname + " channels";
    TH1I* hLatestPulse = new TH1I(histname.c_str(), histtitle.c_str(), 64,-0.5,63.5);
    hLatestPulse->GetXaxis()->SetTitle("Time");
    hLatestPulse->GetYaxis()->SetTitle("ADC Value");
    latest_pulse_histograms_map[bankname] = hLatestPulse;

  }

  // hPulseRawCount
  std::string histname = "hPulseRawCount";
  std::string histtitle = "Plot of the raw counts in each channels";
  hPulseRawCount = new TH1I(histname.c_str(), histtitle.c_str(), 1,0,1);
  hPulseRawCount->GetXaxis()->SetTitle("Channel");
  hPulseRawCount->GetYaxis()->SetTitle("Number of Pulses");
  hPulseRawCount->SetBit(TH1::kCanRebin);

  return SUCCESS;
}

// Resets the histograms at the beginning of each run so that the online display updates
INT MCommonOnlineDisplayPlots_bor(INT run_number) {

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 
  
    std::string bankname = mapIter->first;
    height_histograms_map[bankname]->Reset();
    time_histograms_map[bankname]->Reset();
    shape_histograms_map[bankname]->Reset();
    latest_pulse_histograms_map[bankname]->Reset();

  }

  hPulseRawCount->Reset();
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

	    if (time_histograms_map.find(bankname) != time_histograms_map.end())
	      time_histograms_map[bankname]->Fill((*pulseIter)->GetPulseTime());

	    // only fill this histogram every 10 events
	    if (midas_event_number % 10 == 0) {
	      if (shape_histograms_map.find(bankname) != shape_histograms_map.end()) {

		latest_pulse_histograms_map[bankname]->Reset();	
	
		std::vector<int> theSamples = (*pulseIter)->GetSamples();
		for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
		  shape_histograms_map[bankname]->Fill(sampleIter - theSamples.begin(), (*sampleIter));
                  latest_pulse_histograms_map[bankname]->SetBinContent(sampleIter - theSamples.begin(),(*sampleIter));
		}
	      }	    
	    }
	  }

	  hPulseRawCount->Fill(bankname.c_str(), thePulses.size());
	}
	return SUCCESS;
}
