////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_RunTime
/// \ingroup lldq
/// \author Andrew Edmonds
///
/// \brief
/// Records run time according to the ODB.
///
/// \details
/// Records run time in ::hDQ_RunTime.
/// @{

/// \var hDQ_RunTime
/// \brief
/// Simply a single value representing run time according to ODB.
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
INT  MDQ_RunTime_init(void);
INT  MDQ_RunTime(EVENT_HEADER*, void*);
INT  MDQ_RunTime_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

TH1F* hDQ_RunTime;

ANA_MODULE MDQ_RunTime_module =
{
	"MDQ_RunTime",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_RunTime,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_RunTime_eor,                          /* EOR routine           */
	MDQ_RunTime_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_RunTime_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DQ_Runtime")) {
    
    std::string dir_name("DQ_Runtime/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // hDQ_RunTime
  std::string histname = "hDQ_RunTime";
  std::string histtitle = "The duration of the run";
  hDQ_RunTime = new TH1F(histname.c_str(), histtitle.c_str(), 3,0,3);
  hDQ_RunTime->GetXaxis()->SetTitle("");
  hDQ_RunTime->GetYaxis()->SetTitle("Run Time [s]");


  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_RunTime_eor(INT run_number) {

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

  hDQ_RunTime->Fill(1,duration);

  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_RunTime(EVENT_HEADER *pheader, void *pevent)
{
	// Get the event number
	int midas_event_number = pheader->serial_number;

	// Don't need anything here

	return SUCCESS;
}

/// @}
