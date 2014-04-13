/********************************************************************\

Name:         MDQ_IslandRate
Created by:   Andrew Edmonds

Contents:     hDQ_IslandRate
               - Plots: the total number of TPulseIslands per unit time in each detector

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
INT  MDQ_IslandRate_init(void);
INT  MDQ_IslandRate(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

TH1F* hDQ_IslandRate;

ANA_MODULE MDQ_IslandRate_module =
{
	"MDQ_IslandRate",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_IslandRate,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MDQ_IslandRate_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_IslandRate_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // hDQ_IslandRate
  std::string histname = "hDQ_IslandRate";
  std::string histtitle = "The total island rate in each channel";
  hDQ_IslandRate = new TH1F(histname.c_str(), histtitle.c_str(), 1,0,1);
  hDQ_IslandRate->GetXaxis()->SetTitle("Detector (Bank)");
  hDQ_IslandRate->GetYaxis()->SetTitle("Island Rate [TPI s^{-1}]");
  hDQ_IslandRate->SetBit(TH1::kCanRebin);


  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_IslandRate(EVENT_HEADER *pheader, void *pevent)
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
	for (map_iterator mapIter = pulse_islands_map.begin(); mapIter != pulse_islands_map.end(); ++mapIter) 
	{
	  std::string bankname = mapIter->first;
	  std::vector<TPulseIsland*> thePulses = mapIter->second;
	  std::string detname = gSetup->GetDetectorName(bankname);

	  std::string binname = detname + "(" + bankname + ")";

	  hDQ_IslandRate->Fill(binname.c_str(), thePulses.size()); // keep a count of the total number of TPIs
	}
	return SUCCESS;
}
