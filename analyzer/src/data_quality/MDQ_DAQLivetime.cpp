////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_DAQLivetime
/// \ingroup lldq
/// \author Andrew Edmonds
/// \editor Damien Alexander
///
/// \brief
/// Stores fraction of run time that data was being taken.
/// Creates hDQ_DAQLivetime.
/// \details
/// Assumes every events is 110ms. Multiplies 110 ms by number of
/// MIDAS events, then divides by run time according to ODB.
/// Creates ::hDQ_DAQLivetime.
/// @{

/// \var hDQ_DAQLivetime_event
/// \brief
/// Plots the livetime of the DAQ, calculated from the sum
/// of event times by the total run time

/// \var hDQ_DAQLivetime_pulser
/// \brief
/// Plots the Livetime of the DAQ, calculated from the ratio of
/// of pulser pulses (550 Hz, 1.818 ms) to run time (from odb)

/// \var hDQ_EventTime
/// \brief
/// Plots the time of each event based on the number of pulser
/// hits sen (assumed 10 kHz)
////////////////////////////////////////////////////////////////////////////////

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
INT  MDQ_DAQLivetime_init(void);
INT  MDQ_DAQLivetime(EVENT_HEADER*, void*);
INT  MDQ_DAQLivetime_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

TH1F* hDQ_DAQLivetime_event;
TH1F* hDQ_DAQLivetime_pulser;
TH1F* hDQ_EventTime;


ANA_MODULE MDQ_DAQLivetime_module =
{
	"MDQ_DAQLivetime",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_DAQLivetime,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_DAQLivetime_eor,                          /* EOR routine           */
	MDQ_DAQLivetime_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_DAQLivetime_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DQ_Livetime")) {
    
    std::string dir_name("DQ_Livetime/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // hDQ_DAQLivetime_event
  std::string histname = "hDQ_DAQLivetime_event";
  std::string histtitle = "DAQ Livetime of the Run";
  hDQ_DAQLivetime_event = new TH1F(histname.c_str(), histtitle.c_str(), 3,0,3);
  hDQ_DAQLivetime_event->GetXaxis()->SetTitle("");
  hDQ_DAQLivetime_event->GetYaxis()->SetTitle("DAQ Livetime");

  // hDQ_DAQLivetime_pulser
  histname = "hDQ_DAQLivetime_pulser";
  histtitle = "DAQ Livetime of the Run (pulser based)";
  hDQ_DAQLivetime_pulser = new TH1F(histname.c_str(), histtitle.c_str(), 3,0,3);
  hDQ_DAQLivetime_pulser->GetXaxis()->SetTitle("");
  hDQ_DAQLivetime_pulser->GetYaxis()->SetTitle("DAQ Livetime");

  //hDQ_EventTime
  histname = "hDQ_EventTime";
  histtitle = "Duration of events";
  hDQ_EventTime = new TH1F(histname.c_str(), histtitle.c_str(), 300, 0, 0.3);
  hDQ_EventTime->GetXaxis()->SetTitle("Event duration (ms)");
  hDQ_EventTime->GetYaxis()->SetTitle("");

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_DAQLivetime_eor(INT run_number) {

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

  hDQ_DAQLivetime_event->Scale(1.0/duration);
  hDQ_DAQLivetime_pulser->Scale(1.0/duration);

  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_DAQLivetime(EVENT_HEADER *pheader, void *pevent)
{
	// Get the event number
	int midas_event_number = pheader->serial_number;

	// Some typedefs
	typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
	typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
	typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

	// Add an event
	double gate_length = 110; // ms
	gate_length /= 1000; // convert to seconds since that is what the duration of the run will be calculated in
	hDQ_DAQLivetime_event->Fill(1,gate_length);

	double pulser_sep = 0.001818; //10kHz in seconds
	const std::map<std::string, std::vector<int64_t> >& tdcs_map =
	  gData->fTDCHitsToChannelMap;
	char det[16]; sprintf(det, "TSync");
	std::string bank = gSetup->GetBankName(det);
	int nPulses = 0;
	if(tdcs_map.count(bank))
	  nPulses = tdcs_map.at(bank).size();
	double event_time = nPulses * pulser_sep;
	hDQ_DAQLivetime_pulser->Fill(1, event_time);
	hDQ_EventTime->Fill(event_time);




	return SUCCESS;
}

/// @}
