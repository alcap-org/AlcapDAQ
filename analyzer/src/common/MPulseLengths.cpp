/********************************************************************\

Name:         MPulseLengths
Created by:   Andrew Edmonds

Contents:     A module to fill a histogram of the pulse lengths from each channel

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
INT  MPulseLengths_init(void);
INT  MPulseLengths(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static vector<TOctalFADCBankReader*> fadc_bank_readers;
static TH2I* average_length_histogram;

ANA_MODULE MPulseLengths_module =
{
	"MPulseLengths",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MPulseLengths,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MPulseLengths_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MPulseLengths_init()
{
  // This histogram has the pulse lengths on the X-axis and the number of pulses on the Y-axis
  // One histogram is created for each detector
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.
  vector<string> bank_names = GetAllFADCBankNames();

  for(unsigned int i=0; i<bank_names.size(); i++) {
    fadc_bank_readers.push_back(new TOctalFADCBankReader(bank_names[i]));    
  }

  std::string histname = "hAvgPulseLengthsPerChannel";
  std::string histtitle = "Plot of the average pulse lengths per event for the each channel";
  average_length_histogram = new TH2I(histname.c_str(), histtitle.c_str(), 1,0,1, 5000,0,5000);
  average_length_histogram->GetXaxis()->SetTitle("Bank Name");
  average_length_histogram->GetYaxis()->SetTitle("MIDAS Event Number");
  average_length_histogram->SetBit(TH1::kCanRebin);

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MPulseLengths(EVENT_HEADER *pheader, void *pevent)
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
	  
	  if (thePulses.size() != 0) {
	    // Loop over the TPulseIslands and plot the histogram
	    double total_length_of_pulses = 0;
	    for (std::vector<TPulseIsland*>::iterator thePulseIter = thePulses.begin(); thePulseIter != thePulses.end(); thePulseIter++) {
	      
	      total_length_of_pulses += (*thePulseIter)->GetPulseLength();
	    }
	    // Fill the histogram
	    average_length_histogram->Fill(bankname.c_str(), midas_event_number, total_length_of_pulses / thePulses.size());
	  }
	}
	return SUCCESS;
}
