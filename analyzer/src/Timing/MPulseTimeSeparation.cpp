/********************************************************************\

Name:         MPulseTimeSeparation
Created by:   Joe Grange

Contents:     A module to fill a histogram of the pulse times from each channel

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
INT  MPulseTimeSeparation_init(void);
INT  MPulseTimeSeparation(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static vector<TOctalFADCBankReader*> fadc_bank_readers;
static TH1* hPulseTimeDiff;

ANA_MODULE MPulseTimeSeparation_module =
{
	"MPulseTimeSeparation",                    /* module name           */
	"Joe Grange",              /* author                */
	MPulseTimeSeparation,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MPulseTimeSeparation_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MPulseTimeSeparation_init()
{
  // This histogram has the pulse times on the X-axis and the number of pulses on the Y-axis
  // One histogram is created for each detector
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.

  hPulseTimeDiff = new TH1I("pulseIslandTimeDiffs","Time difference between consecutive pulses in all channels",100,0,100);
  hPulseTimeDiff->GetXaxis()->SetTitle("time [ns]");
  hPulseTimeDiff->GetYaxis()->SetTitle("Number of pulse pairs");
  hPulseTimeDiff->SetBit(TH1::kCanRebin);

  vector<string> bank_names = GetAllFADCBankNames();

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MPulseTimeSeparation(EVENT_HEADER *pheader, void *pevent)
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
	    
	    if (thePulseIter!=thePulses.begin()) 
	      hPulseTimeDiff->Fill((*thePulseIter)->GetPulseTime() - (*(thePulseIter-1))->GetPulseTime());	      	  }
	}
	return SUCCESS;
}
