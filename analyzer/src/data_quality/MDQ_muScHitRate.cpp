/********************************************************************\

Name:         MDQ_muScHitRate
Created by:   Andrew Edmonds

Contents:     hDQ_muScHitRate_muSc
               - Plots: the number of muSc hits as seen in the TDC (parameter = 6011)

              hDQ_muScHitRate_muScA
               - Plots: the number of muScA hits as seen in the TDC (parameter = 6002)

              hDQ_muScHitRate_muPC
               - Plots: the number of muPC hits as seen in the TDC (4001 <= parameter <= 4074)

              hDQ_muScHitRate_Unknown
               - Plots: the number of hits in the TDC with any other parameter

              hDQ_muScHitRate_muSc_time
               - Plots: the time of the muSc hits as seen in the TDC

              hDQ_muScHitRate_TDiff
               - Plots: the time difference between muSc hits in the TDC and the BU CAEN

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

/* MuCap includes */
#include "../muSC_muPC/common.h" // for channel_hit

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
INT  MDQ_muScHitRate_init(void);
INT  MDQ_muScHitRate(EVENT_HEADER*, void*);
INT  MDQ_muScHitRate_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

TH1F* hDQ_muScHitRate;

ANA_MODULE MDQ_muScHitRate_module =
{
	"MDQ_muScHitRate",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_muScHitRate,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_muScHitRate_eor,                          /* EOR routine           */
	MDQ_muScHitRate_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_muScHitRate_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Create some histograms
  hDQ_muScHitRate = new TH1F("hDQ_muScHitRate", "Number of hits in muSc", 3,0,3);
  hDQ_muScHitRate->GetXaxis()->SetTitle("muSc");
  hDQ_muScHitRate->GetYaxis()->SetTitle("Hit Rate [#mu s^{-1}]");

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_muScHitRate_eor(INT run_number) {

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

  hDQ_muScHitRate->Scale(1.0/duration);

  return SUCCESS;
}
/** This method fills the histograms
 */
INT MDQ_muScHitRate(EVENT_HEADER *pheader, void *pevent)
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

	// Get a pointer to the hit data in the TDC
	// NB copied and modified from MMuSCAnalysisC.cpp
	channel_hit *hit_bank;
	int hit_bank_size = bk_locate(pevent, "HITS", (DWORD *) &hit_bank);
	hit_bank_size = hit_bank_size * sizeof(DWORD) / sizeof(channel_hit);

	// Parameter values have been obtained from MMuSCAnalysisMQL.cpp and MMuPC1AnalysisMQL.cpp
	for (int i = 0; i < hit_bank_size; ++i) {
	  if (hit_bank[i].parameter == 6011) {
	    hDQ_muScHitRate->Fill(1);
	  }
	}

	return SUCCESS;
}
