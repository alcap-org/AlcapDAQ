/********************************************************************\

Name:         MSiPulseHeight
Created by:   Andrew Edmonds

Contents:     A module to plot the pulse heights of the silicon detector

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>

/* AlCap includes */
#include "DetectorMap.h"

#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"
#include "TSimpleSiPulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MSiPulseHeight_init(void);
INT  MSiPulseHeight(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern map<string, vector<TSimpleSiPulse*> > theSimpleSiPulseMap;

static vector<TOctalFADCBankReader*> fadc_bank_readers;

ANA_MODULE MSiPulseHeight_module =
{
	"MSiPulseHeight",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MSiPulseHeight,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MSiPulseHeight_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MSiPulseHeight_init()
{
  // This histogram has the bank names labeled on the X-axis, and the midas
  // block number on the Y-axis.
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.
  int n_bins = 100;
  int x_min = 0;
  int x_max = 100;
  
  // Iterate through the detectors and create a height histogram for each one
  for (detIter aDetIter = DetectorToHeightHistMap.begin(); aDetIter != DetectorToHeightHistMap.end(); aDetIter++) {
  	
  	std::string detname = aDetIter->first;
  	
  	if (detname.substr(0,2) != "Si")
		continue;
				
  	std::string histname = "h" + detname + "_Heights";
  	std::string histtitle = "Plot of the pulse heights for the " + detname + " detector";
  	aDetIter->second = new TH1I(histname.c_str(), histtitle.c_str(), n_bins, x_min, x_max);
  	(aDetIter->second)->SetBit(TH1::kCanRebin);
  }

  vector<string> bank_names = GetAllFADCBankNames();

  for(unsigned int i=0; i<bank_names.size(); i++) {
    fadc_bank_readers.push_back(new TOctalFADCBankReader(bank_names[i]));
  }

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MSiPulseHeight(EVENT_HEADER *pheader, void *pevent)
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
	for (std::map<string, vector<TSimpleSiPulse*> >::iterator simpleSiMapIter = theSimpleSiPulseMap.begin();
			simpleSiMapIter != theSimpleSiPulseMap.end(); 
			simpleSiMapIter++) 
	{
		std::string bankname = simpleSiMapIter->first;
		string detname = ChannelToDetectorMap[bankname];
			
		if (detname.substr(0,2) != "Si")
			continue;
			
		std::vector<TSimpleSiPulse*> theSiPulses = simpleSiMapIter->second;
			
		// Loop over the TSimpleSiPulses and plot the histogram
		for (std::vector<TSimpleSiPulse*>::iterator siPulse = theSiPulses.begin(); siPulse != theSiPulses.end(); siPulse++) {
			
			// Find the detector name
			if (ChannelToDetectorMap.find(bankname) == ChannelToDetectorMap.end())
			{
				// not found, do nothing
			}
			else
			{
				// Get the detector name and use that to fill the corresponding height histogram
				DetectorToHeightHistMap[detname]->Fill((*siPulse)->GetPulseHeight());
				DetectorToHeightHistMap[detname]->GetXaxis()->SetTitle("Pulse Height [ADC value]");
				DetectorToHeightHistMap[detname]->GetYaxis()->SetTitle("Arbitrary Unit");
			}
		}
	}
	return SUCCESS;
}
