/********************************************************************\

Name:         MDQ_IslandLength
Created by:   NT

Contents:     hDQ_IslandLength_[DetName] 

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
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MDQ_IslandLength_init(void);
INT  MDQ_IslandLength(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH1F*> DQ_IslandLength_histograms_map;

ANA_MODULE MDQ_IslandLength_module =
{
	"MDQ_IslandLength",                    /* module name           */
	"Nam Tran",              /* author                */
	MDQ_IslandLength,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MDQ_IslandLength_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_IslandLength_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
	
	std::string dir_name("DataQuality_LowLevel/");
	dir_name += "IslandLength/";
  if (!gDirectory->Cd(dir_name.c_str())) {
		gDirectory->mkdir(dir_name.c_str());
		gDirectory->Cd(dir_name.c_str());
  }


  // Create a histogram for each detector
  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    // hDQ_IslandLength_[DetName]
    std::string histname = "hDQ_IslandLength_" + detname;
    std::string histtitle = "Pulse shape of " + detname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(), 
				4096, -0.5, 4095.5);
    hDQ_Histogram->GetXaxis()->SetTitle("Length [samples]");
    hDQ_Histogram->GetYaxis()->SetTitle("Number of islands");
    DQ_IslandLength_histograms_map[bankname] = hDQ_Histogram;
  }

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_IslandLength(EVENT_HEADER *pheader, void *pevent)
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
	for (map_iterator mapIter = pulse_islands_map.begin(); 
			mapIter != pulse_islands_map.end(); ++mapIter) 
	{
	  std::string bankname = mapIter->first;
	  std::string detname = gSetup->GetDetectorName(bankname);
	  std::vector<TPulseIsland*> thePulses = mapIter->second;
			
	  // Loop over the TPulseIslands and plot the histogram
		for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin();
				pulseIter != thePulses.end(); ++pulseIter) {

	    // Make sure the histograms exist and then fill them
			if (DQ_IslandLength_histograms_map.find(bankname) !=
					DQ_IslandLength_histograms_map.end()) 
			{ 
				std::vector<int> theSamples = (*pulseIter)->GetSamples();
				DQ_IslandLength_histograms_map[bankname]->Fill(theSamples.size());
	    }
	  }
	}
	return SUCCESS;
}
