/********************************************************************\

  Name:         analyzer.c
  Created by:   Stefan Ritt

  Contents:     System part of Analyzer code for sample experiment

\********************************************************************/

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

/* midas includes */
#include "midas.h"

/* cernlib includes */
#ifdef OS_WINNT
#define VISUAL_CPLUSPLUS
#endif
#ifdef __linux__
#define f2cFortran
#endif

#ifdef HAVE_HBOOK
#include <cfortran.h>
#include <hbook.h>

PAWC_DEFINE(1000000);
#endif /* HAVE_HBOOK */

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

/*-- Globals -------------------------------------------------------*/

/* The analyzer name (client name) as seen by other MIDAS clients   */
char *analyzer_name = "Analyzer";

/* analyzer_loop is called with this interval in ms (0 to disable)  */
INT  analyzer_loop_period = 0;

/* default ODB size */
INT  odb_size = DEFAULT_ODB_SIZE;

/* Global repository for data produced by modules to be used by
 * downstream modules
 */
TGlobalData* gData;
TSetupData* gSetup;

void UpdateDetectorBankNameMap(TSetupData *gSetup);

/*-- Module declarations -------------------------------------------*/

#include "MODULES.h"

/*-- Bank definitions ----------------------------------------------*/

BANK_LIST ana_trigger_bank_list[] = {

  { "" },
};

/*-- Event request list --------------------------------------------*/

ANALYZE_REQUEST analyze_request[] = {
  { "Trigger",            /* equipment name */
    { 1,                    /* event ID */
      TRIGGER_ALL,          /* trigger mask */
      GET_SOME,             /* get some events */
      "SYSTEM",             /* event buffer */
      TRUE,                 /* enabled */
      "", "", },
    NULL,                 /* analyzer routine */
    trigger_module,       /* module list */
    ana_trigger_bank_list,/* bank list */
    1000,                 /* RWNT buffer size */
    TRUE,                 /* Use tests for this event */
  },

  { "" },
};

/*-- Analyzer Init -------------------------------------------------*/

static void catastrophe(int signum)
{
#if 0
  static bool previously_caught_signal = false;
#endif

  printf("Caught signal %d, exiting...", signum);

#if 0
  if(!previously_caught_signal) {
    previously_caught_signal = true;

#ifdef HAVE_ROOT
    printf("Attempting to save histograms...\n");
    extern void CloseRootOutputFile();
    CloseRootOutputFile();
#endif
  }
#endif

  exit(-signum);
}

INT analyzer_init()
{
  // Initialize gData
  gData = new TGlobalData();

  // Initialize gSetup
  gSetup = new TSetupData();
  UpdateDetectorBankNameMap(gSetup);
  // Override ROOT's handling of signals
  signal(SIGHUP , catastrophe);
  signal(SIGINT , catastrophe);
  signal(SIGQUIT , catastrophe);
  signal(SIGILL , catastrophe);
  signal(SIGABRT , catastrophe);
  signal(SIGFPE , catastrophe);
  signal(SIGKILL , catastrophe);
  signal(SIGSEGV , catastrophe);
  signal(SIGPIPE , catastrophe);
  signal(SIGTERM , catastrophe);

  return SUCCESS;
}

/*-- Analyzer Exit -------------------------------------------------*/

INT analyzer_exit()
{
  if(gData) {
    delete gData;
    gData = NULL;
  }

  if(gSetup) {
    delete gSetup;
    gSetup = NULL;
  }

  return CM_SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT ana_begin_of_run(INT run_number, char *error)
{
  printf("Analyzer saw beginning of run %d\n", run_number);
  return CM_SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT ana_end_of_run(INT run_number, char *error)
{
  printf("Analyzer saw end of run %d\n", run_number);
  return CM_SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT ana_pause_run(INT run_number, char *error)
{
  return CM_SUCCESS;
}

/*-- Resume Run ----------------------------------------------------*/

INT ana_resume_run(INT run_number, char *error)
{
  return CM_SUCCESS;
}

/*-- Analyzer Loop -------------------------------------------------*/

INT analyzer_loop()
{
  return CM_SUCCESS;
}

/*------------------------------------------------------------------*/
//}

void UpdateDetectorBankNameMap(TSetupData *gSetup){
  // Want to go through the /Analyzer/WireMap and map bank names and detector names 
  HNDLE hDB, hKey;
  char keyName[200];
  
  if(cm_get_experiment_database(&hDB, NULL) != CM_SUCCESS){
    printf("Warning: Could not connect to ODB database!\n");
    return;
  }
  
  sprintf(keyName, "/Analyzer/WireMap/BankName");
  if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return;
  }
  KEY bk_key;
  if(db_get_key(hDB, hKey, &bk_key) != DB_SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return;
  }    
  char BankNames[bk_key.num_values][bk_key.item_size];
  int size = sizeof(BankNames);
  if(db_get_value(hDB, 0, keyName , BankNames, &size, TID_STRING, 0) != DB_SUCCESS){
    printf("Warning: Could not retrieve values for key %s\n", keyName);
    return;
  }
  
  sprintf(keyName, "/Analyzer/WireMap/DetectorName");
  if(db_find_key(hDB,0,keyName, &hKey) != SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return;
  }  
  KEY det_key;
  if(db_get_key(hDB, hKey, &det_key) != DB_SUCCESS){
    printf("Warning: Could not find key %s\n", keyName);
    return;
  }
  char DetectorNames[det_key.num_values][det_key.item_size];
  size = sizeof(DetectorNames);
  if(db_get_value(hDB, 0, keyName , DetectorNames, &size, TID_STRING, 0) != DB_SUCCESS){
    printf("Warning: Could not retrieve values for key %s\n", keyName);
    return;
  }
  
  if(det_key.num_values != bk_key.num_values){
    printf("Warning: Key sizes are not equal for banks and detectors in /Analyzer/WireMap/\n");
    return;
  }
  else printf("sizes are %d\n", det_key.num_values);
  for(int i=0; i<det_key.num_values; i++){
    if(strcmp(BankNames[i], "") == 0) continue;
    if(strcmp(DetectorNames[i], "") == 0) printf("Warning: No detector name associated with bank %s!\n", BankNames[i]);
    
    std::string bank(BankNames[i]), detector(DetectorNames[i]);
    gSetup->fBankToDetectorMap[bank] = detector;

    if(BankNames[i][0] == 'N'){
      std::string iAddr(BankNames[i]);
      int iChn = (int)(*(iAddr.substr(1,1).c_str()) - 97);
      iAddr = iAddr.substr(2, 2);

      char wireKey[100];
      bool enabled = false;
      int size = sizeof(int);
      sprintf(wireKey, "/Analyzer/WireMap/Enabled");
      if(db_find_key(hDB,0,wireKey, &hKey) == SUCCESS){
	// Let's first reset /Analyzer/WireMap/Enabled for this channel to 'n'
	db_set_data_index(hDB, hKey, &enabled, size, i, TID_BOOL);

	sprintf(keyName, "/Equipment/Crate 9/Settings/NFADC %s/Enabled", iAddr.c_str());
	if(db_get_value(hDB, 0, keyName , &enabled, &size, TID_BOOL, 0) == DB_SUCCESS) {
	  if(enabled == true){
	    sprintf(keyName, "/Equipment/Crate 9/Settings/NFADC %s/Channel %d/Trigger mask", iAddr.c_str(), iChn);
	    int trigger_mask;
	    size = sizeof(trigger_mask);
	    if(db_get_value(hDB, 0, keyName , &trigger_mask, &size, TID_INT, 0) == DB_SUCCESS) {
	      if (trigger_mask == 1){ // if this channel is taking data
		enabled = true;
		db_set_data_index(hDB, hKey, &enabled, size, i, TID_BOOL);	
	      }
	    } // We found the channel 'Trigger mask' key in ODB
	  } // This module is enabled
	} // We got the value of the module 'Enable' key in ODB
      } // We found the 'Enabled' key in the ODB
    } // end if bank is starting with letter 'N' 
  } // end loop over all non empty banks
  
 for(int i=0; i<det_key.num_values; i++){
   // Calculate the clock ticks for each bank and update TSetupData
   if(BankNames[i][0] == 'N'){ // FADC banks
     std::string bank_name(BankNames[i]);
     int iChn = (int)(*(bank_name.substr(1,1).c_str()) - 97);
     std::string iAddr = bank_name.substr(2, 2);
    
     sprintf(keyName, "/Equipment/Crate 9/Settings/NFADC %s/Channel %d/DCM phase", iAddr.c_str(), iChn);
    
     int DCMPhase;
     double clockTickInNs;
     double FADC_frequency = 170e6; // 170 MHz

     if(db_find_key(hDB,0,keyName, &hKey) == SUCCESS){
       db_get_key(hDB, hKey, &bk_key);

       int size = sizeof(DCMPhase);
       if(db_get_value(hDB, 0, keyName , &DCMPhase, &size, TID_INT, 0) == DB_SUCCESS){
	 double true_frequency = FADC_frequency / DCMPhase;
	 clockTickInNs = (1/true_frequency) * 1e9;

	 gSetup->fBankToClockTickMap[bank_name] = clockTickInNs;
       }
     }
   }
   else if (BankNames[i][0] == 'C') { // CAEN banks
     std::string bank_name(BankNames[i]);

     double CAEN_frequency = 100e6; // 100 MHz (internal clock)
     double clockTickInNs = (1 / CAEN_frequency) * 1e9;

     gSetup->fBankToClockTickMap[bank_name] = clockTickInNs;
   }
 }
}
