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
