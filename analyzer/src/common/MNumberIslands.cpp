/********************************************************************\

Name:         MNumberIslands
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
INT  MNumberIslands_init(void);
INT  MNumberIslands(EVENT_HEADER*, void*);
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static vector<TOctalFADCBankReader*> fadc_bank_readers;
static TH2I* hNumberOfIslands;

ANA_MODULE MNumberIslands_module =
{
	"MNumberIslands",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MNumberIslands,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MNumberIslands_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MNumberIslands_init()
{
  // This histogram has the pulse counters on the X-axis and the number of pulses on the Y-axis
  // One histogram is created for each detector


  hNumberOfIslands = new TH2I("hNumberOfIslands", "Expected Number of Islands per Channel per Event", 1,0,1, 10000,0,10000);
  hNumberOfIslands->GetXaxis()->SetTitle("Bank Name");
  hNumberOfIslands->GetYaxis()->SetTitle("MIDAS Event Number");
  hNumberOfIslands->SetBit(TH1::kCanRebin);

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MNumberIslands(EVENT_HEADER *pheader, void *pevent)
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
	
	  hNumberOfIslands->Fill(bankname.c_str(), midas_event_number, thePulses.size());

	}
	return SUCCESS;
}
