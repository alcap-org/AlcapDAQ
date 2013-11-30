/********************************************************************\

Name:         MExpectedIslands
Created by:   Andrew Edmonds

Contents:     A module to fill a 2D histogram of the number of expected pulses per event from each channel

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
#include <TH2.h>

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
INT  MExpectedIslands_init(void);
INT  MExpectedIslands(EVENT_HEADER*, void*);
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static vector<TOctalFADCBankReader*> fadc_bank_readers;
static TH2I* hExpectedNumberOfIslands;

ANA_MODULE MExpectedIslands_module =
{
	"MExpectedIslands",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MExpectedIslands,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MExpectedIslands_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MExpectedIslands_init()
{
  // This histogram has the pulse counters on the X-axis and the number of pulses on the Y-axis
  // One histogram is created for each detector


  hExpectedNumberOfIslands = new TH2I("hExpectedNumberOfIslands", "Expected Number of Islands per Channel per Event", 1,0,1, 10000,0,10000);
  hExpectedNumberOfIslands->GetXaxis()->SetTitle("Bank Name");
  hExpectedNumberOfIslands->GetYaxis()->SetTitle("MIDAS Event Number");
  hExpectedNumberOfIslands->SetBit(TH1::kCanRebin);

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MExpectedIslands(EVENT_HEADER *pheader, void *pevent)
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
	
	  double pulse_frequency = 65; // Hz
	  double pulse_period = (1 / pulse_frequency) * 1e3; // ms
	  double gate_width = 110; // ms

	  if (thePulses.size() != 0) {
	    double initial_pulse_time = (*(thePulses.begin()))->GetTimeStamp() * (*(thePulses.begin()))->GetClockTickInNs() * 1e-6; // ms
	    
	    // Work out how many more pulses we expect for the given pulser period and the given gate width
	    double remaining_time = gate_width - initial_pulse_time;
	    int expected_pulses = remaining_time / pulse_period;

	    // If the initial pulse_time is greater than the pulse period then a pulse should have been seen before now
	    if (initial_pulse_time > pulse_period) {
	      expected_pulses += initial_pulse_time / pulse_period; // add the number that were missed at the start
	    }
	    printf("%f\n", (*(thePulses.begin()))->GetClockTickInNs());
	    printf("%s: Pulse Period = %f ms\nInitial time stamp: %d\nInitial pulse time: %f ms\nRemaining time: %f ms\nExpected number of pulses = %d\n\n", bankname.c_str(), pulse_period, (*(thePulses.begin()))->GetTimeStamp(), initial_pulse_time, remaining_time, expected_pulses);

	    // Fill the histogram
	    hExpectedNumberOfIslands->Fill(bankname.c_str(), midas_event_number, expected_pulses + 1); // extra +1 so that we count the initial pulse
	  }		     
	  else {
	    hExpectedNumberOfIslands->Fill(bankname.c_str(), midas_event_number, 0);
	  }
	}
	return SUCCESS;
}
