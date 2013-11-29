/********************************************************************\

Name:         MPulseTimeSeparation
Created by:   Joe Grange

Contents:     A module to fill a histogram of any pulses with only 4 samples

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>

/* AlCap includes */
#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MShortPulseTimes_init(void);
INT  MShortPulseTimes(EVENT_HEADER*, void*);
vector<string> GetAllBankNames();
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static vector<TOctalFADCBankReader*> fadc_bank_readers;

static std::map<std::string, TH1*>  time_short_pulse_histogram_map;

ANA_MODULE MShortPulseTimes_module =
{
	"MShortPulseTimes",                    /* module name           */
	"Joe Grange",              /* author                */
	MShortPulseTimes,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MShortPulseTimes_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MShortPulseTimes_init()
{
  // This histogram has the pulse times on the X-axis and the number of pulses on the Y-axis
  // One histogram is created for each detector
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin();
      mapIter != bank_to_detector_map.end(); mapIter++) {

    std::string detname = gSetup->GetDetectorName(mapIter->first);
    std::string histname = "h" + detname + "_ShortPulseTimes";
    std::string histtitle = "Plot of the short pulse times for the " + detname + " detector";
    TH1D* hShortPulseTime = new TH1D(histname.c_str(),histtitle.c_str(),100,0,100);
    hShortPulseTime->GetXaxis()->SetTitle("Time Stamp");
    hShortPulseTime->GetYaxis()->SetTitle("Number of 4-sample pulses");
    hShortPulseTime->SetBit(TH1::kCanRebin);

    time_short_pulse_histogram_map[mapIter->first] = hShortPulseTime;
  }

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MShortPulseTimes(EVENT_HEADER *pheader, void *pevent)
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
	  for (std::vector<TPulseIsland*>::iterator thePulseIter = thePulses.begin(); thePulseIter != thePulses.end(); thePulseIter++) {

        //samples = (*thePulseIter)->GetSamples();
        if ((*thePulseIter)->GetSamples().size()==4) time_short_pulse_histogram_map[bankname]->Fill((*thePulseIter)->GetPulseTime());        
	    
	  }

	}
	return SUCCESS;
}
