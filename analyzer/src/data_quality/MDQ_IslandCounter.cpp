/********************************************************************\

Name:         MDQ_IslandCounter
Created by:   Andrew Edmonds

Contents:     hDQ_IslandCounter_[DetName]_[BankName]
              - plots the number of TPulseIslands in each event for each detector

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
INT  MDQ_IslandCounter_init(void);
INT  MDQ_IslandCounter(EVENT_HEADER*, void*);
INT  MDQ_IslandCounter_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

extern TH1F* hDQ_TDCCheck_muSc;

map <std::string, TH1F*> DQ_IslandCounter_histograms_map;

ANA_MODULE MDQ_IslandCounter_module =
{
	"MDQ_IslandCounter",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_IslandCounter,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_IslandCounter_eor,                          /* EOR routine           */
	MDQ_IslandCounter_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_IslandCounter_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Create a histogram for each detector
  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    // hDQ_IslandCounter_[DetName]_[BankName]
    std::string histname = "hDQ_IslandCounter_" + detname + "_" + bankname;
    std::string histtitle = "Distribution of the number of islands per event per muSc TDC Hit in " + detname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(), 3500, 0, 3500);
    hDQ_Histogram->GetXaxis()->SetTitle("Number of TPulseIslands");
    hDQ_Histogram->GetYaxis()->SetTitle("Number of Events per muSc TDC Hit");
    DQ_IslandCounter_histograms_map[bankname] = hDQ_Histogram;
  }

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_IslandCounter_eor(INT run_number) {

  // Some typedefs
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;
  
  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map =
    gData->fPulseIslandToChannelMap;

  // Loop over the map and get each bankname, vector pair
  for (map_iterator mapIter = pulse_islands_map.begin(); mapIter != pulse_islands_map.end(); ++mapIter) {

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
      
    // Make sure the histograms exist and then fill them
    if (DQ_IslandCounter_histograms_map.find(bankname) != DQ_IslandCounter_histograms_map.end()) {
      DQ_IslandCounter_histograms_map[bankname]->Scale(1./hDQ_TDCCheck_muSc->GetEntries());
    }
  }

  return SUCCESS;
}


/** This method fills the histograms
 */
INT MDQ_IslandCounter(EVENT_HEADER *pheader, void *pevent)
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
	  std::string detname = gSetup->GetDetectorName(bankname);
	  std::vector<TPulseIsland*> thePulses = mapIter->second;
			
	  // Make sure the histograms exist and then fill them
	  if (DQ_IslandCounter_histograms_map.find(bankname) != DQ_IslandCounter_histograms_map.end()) {
	    int n_pulse_islands = thePulses.size();
	    
	    DQ_IslandCounter_histograms_map[bankname]->Fill(n_pulse_islands);
	  }
	}
	return SUCCESS;
}
