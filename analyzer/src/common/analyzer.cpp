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
  UpdateDetectorBankNameMap(gSetup);

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
  // Want to go through the /Analyzer/WireMap and map detector names and 
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
    if(strcmp(DetectorNames[i], "") == 0) printf("Warning: No detector name associated with this bank %s!\n", BankNames[i]);
    
    std::string bank(BankNames[i]), detector(DetectorNames[i]);
    gSetup->fBankToDetectorMap.insert(std::pair<std::string, std::string>(bank, detector));
  }
  
}
