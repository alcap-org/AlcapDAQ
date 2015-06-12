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


/*-- Module declaration --------------------------------------------*/
INT  MPulseLengths_init(void);
INT  MPulseLengths(EVENT_HEADER*, void*);
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static const int NCHANV1724 = 8;
static const int NCHANDT5730 = 8;
static const int NCHANV1720 = 8;

static std::vector<TH1I*> hvPulseLengths;

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
INT MPulseLengths_init() {

  int ihist = 0;
  for (int ich = 0; ich < NCHANV1720; ++ich, ++ihist) {
    char name[32]; sprintf("hPulseLengths_D8%02d", ich);
    char title[32]; sprintf("PulseLengths ")
  }

  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin();
      mapIter != bank_to_detector_map.end(); mapIter++) {

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    std::string histname = "h" + detname + "_Lengths";
    std::string histtitle = "Plot of the pulse lengths for the " + detname + " detector";
    TH1I* hDetLengths = new TH1I(histname.c_str(), histtitle.c_str(), 100,0,100);
    hDetLengths->GetXaxis()->SetTitle("Pulse Lengths [N Samples]");
    hDetLengths->GetYaxis()->SetTitle("Arbitrary Unit");
    hDetLengths->SetBit(TH1::kCanRebin);

    length_histograms_map[bankname] = hDetLengths;
  }

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
	      length_histograms_map[bankname]->Fill((*thePulseIter)->GetPulseLength());
	      total_length_of_pulses += (*thePulseIter)->GetPulseLength();
	    }
	    // Fill the histogram
	    average_length_histogram->Fill(bankname.c_str(), midas_event_number, total_length_of_pulses / thePulses.size());
	  }
	}
	return SUCCESS;
}
