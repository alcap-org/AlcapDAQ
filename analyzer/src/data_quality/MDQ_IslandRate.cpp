/********************************************************************\

Name:         MDQ_IslandRate
Created by:   Andrew Edmonds

Contents:     hDQ_IslandRate
              - plots the total number of TPulseIslands per unit time in each detector

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
INT  MDQ_IslandRate_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

TH1F* hDQ_IslandRate;

extern TH1F* hDQ_TDCCheck_muSc;

ANA_MODULE MDQ_IslandRate_module =
{
	"MDQ_IslandRate",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_IslandRate,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_IslandRate_eor,                          /* EOR routine           */
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
  hDQ_IslandRate->GetYaxis()->SetTitle("Island Rate [TPI per second per muSc TDC Hit]");
  hDQ_IslandRate->SetBit(TH1::kCanRebin);


  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_IslandRate_eor(INT run_number) {

  // Get the run duration to scale the histogram
  HNDLE hDB, hKey;
  char keyName[200];

  if(cm_get_experiment_database(&hDB, NULL) != CM_SUCCESS){
    printf("Warning: Could not connect to ODB database!\n");
    return false;
  }

  sprintf(keyName, "/Runinfo/Start time binary");
  if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return false;
  }
  KEY start_time_key;
  if(db_get_key(hDB, hKey, &start_time_key) != DB_SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return false;
  }
  DWORD StartTimes[start_time_key.num_values];
  int size = sizeof(StartTimes);
  if(db_get_value(hDB, 0, keyName, StartTimes, &size, TID_DWORD, 0) != DB_SUCCESS){
    printf("Warning: Could not retrieve values for key %s\n", keyName);
    return false;
  }

  sprintf(keyName, "/Runinfo/Stop time binary");
  if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return false;
  }
  KEY stop_time_key;
  if(db_get_key(hDB, hKey, &stop_time_key) != DB_SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return false;
  }
  DWORD StopTimes[stop_time_key.num_values];
  size = sizeof(StopTimes);
  if(db_get_value(hDB, 0, keyName, StopTimes, &size, TID_DWORD, 0) != DB_SUCCESS){
    printf("Warning: Could not retrieve values for key %s\n", keyName);
    return false;
  }

  int duration = StopTimes[0] - StartTimes[0]; // length of run in seconds (checked against run #2600)

  hDQ_IslandRate->Scale(1.0/duration);

  hDQ_IslandRate->Scale(1.0/hDQ_TDCCheck_muSc->GetEntries()); // also normalise to the number of muSc hits in the TDC

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
