/********************************************************************\

  Name:         analyzer.c
  Created by:   Stefan Ritt

  Contents:     Experiment specific analyzer code (user part).

                This is the "simple" version of an analyzer which
                does not use any HBOOK functionality nor a module
                concept.

                This program can be used as a framework for user
                written analyzers that don't use the CERN library.

  $Log: analyzer.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include "midas.h"

/*-- Globals -------------------------------------------------------*/

/* The analyzer name (client name) as seen by other MIDAS clients   */
char *analyzer_name = "Analyzer";

/* analyzer_loop is called with this interval in ms (0 to disable)  */
INT analyzer_loop_period = 0;

/*-- Function declarations -----------------------------------------*/

INT analyze_trigger_event(char *);
INT analyze_scaler_event(char *);

/*-- Event request list --------------------------------------------*/

ANALYZE_REQUEST analyze_request[] = {

   {"Trigger",                  /* event name */
    1,                          /* event ID */
    TRIGGER_ALL,                /* trigger mask */
    GET_SOME,                   /* get some events */
    "SYSTEM",                   /* event buffer */
    TRUE,                       /* enabled */
    "", "",
    analyze_trigger_event,      /* analyzer routine */
    },

   {"Scaler",                   /* equipment name */
    2,                          /* event ID */
    TRIGGER_ALL,                /* trigger mask */
    GET_ALL,                    /* get all events */
    "SYSTEM",                   /* event buffer */
    TRUE,                       /* enabled */
    "", "",
    analyze_scaler_event        /* analyzer routine */
    },

   {""}
};

/********************************************************************\
              Callback routines for system transitions

  These routines are called whenever a system transition like start/
  stop of a run occurs. The routines are called on the following
  occations:

  analyzer_init:  When the analyzer program is started. This routine
                  should initialize global variables.
  
  analyzer_exit:  When the analyzer program is shut down. Can be used
                  to write final variables etc.

  begin_of_run:   When a new run is started.

  end_of_run:     Called on a request to stop a run.

  pause_run:      When a run is paused.

  resume_run:     When a run is resumed.

\********************************************************************/

/*-- Analyzer Init -------------------------------------------------*/

INT analyzer_init()
{
   return CM_SUCCESS;
}

/*-- Analyzer Exit -------------------------------------------------*/

INT analyzer_exit()
{
   return CM_SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT ana_begin_of_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT ana_end_of_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT ana_pause_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- Resuem Run ----------------------------------------------------*/

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

/********************************************************************\
  
  Analyzer routines for different events

\********************************************************************/

/*-- Trigger event -------------------------------------------------*/

INT analyze_trigger_event(char *pevent)
{
   if (SERIAL_NUMBER(pevent) % 1000 == 0)
      printf("Trigger: %d\r", SERIAL_NUMBER(pevent));

   return CM_SUCCESS;
}

/*-- Scaler event --------------------------------------------------*/

INT analyze_scaler_event(char *pevent)
{
   printf("\nScaler: %d\n", SERIAL_NUMBER(pevent));

   return CM_SUCCESS;
}
