/********************************************************************\

  Name:         frontend.c
  Created by:   RA35

  Contents:     Slow control FE for PSI Beam line using EPICS

\********************************************************************/

#include <stdio.h>
#include "midas.h"
//#include "class/generic.h"
//#include "device/epics_ca.h"
//#include "bus/null.h"
#include "generic.h"
#include "epics_ca.h"
#include "null.h"

#ifdef FORTIFY
#include "fortify.h"
#endif

/*-- Globals -------------------------------------------------------*/

//#define DF_NOUPDALL    (1<<5)         /**< do not update all demand values if no change was found */


/* The frontend name (client name) as seen by other MIDAS clients   */
char *frontend_name = "Beamline Frontend";

/* The frontend file name, don't change it */
char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = TRUE;

/* a frontend status page is displayed with this frequency in ms    */
INT display_period = 1000;

/* maximum event size produced by this frontend */
INT max_event_size = 10000;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
INT max_event_size_frag = 5*1024*1024;

/* buffer size to hold events */
INT event_buffer_size = 10*10000;

int crate_number = 0;

/*-- Equipment list ------------------------------------------------*/

/* device driver list */
DEVICE_DRIVER epics_ca_driver[] = {
//{ "EPICSGFA", epics_ca, 111, null, DF_PRIO_DEVICE | DF_ALW_DEVICE | DF_NOUPDALL}, 
//  { "EPICSGFA", epics_ca, 111, null, DF_PRIO_DEVICE                 | DF_NOUPDALL}, 
  { "EPICSGFA", epics_ca, 71, null, DF_PRIO_DEVICE }, 
    /* initial demand value taken from instrument, demand value is read from device */
  { "" }
};


EQUIPMENT equipment[] = {

  { "Beamline",            /* equipment name */
    11, 0,                /* event ID, trigger mask */
    "SYSTEM",             /* event buffer */
    EQ_SLOW,              /* equipment type */
    0,                    /* event source */
    "MIDAS",              /* format */
    TRUE,                 /* enabled */
    RO_ALWAYS,               /* always read, whether running or not */
    30000,                /* read every 30 sec */
    0,                    /* stop run after this event limit */
    0,                    /* number of sub events */
    1,                    /* log history every event */
    "", "", "",
    cd_gen_read,          /* readout routine */
    cd_gen,               /* class driver main routine */
    epics_ca_driver,      /* device driver list */
    NULL,                 /* init string */
  },

  { "" }
};


/*-- Dummy routines ------------------------------------------------*/

INT  poll_event(INT source[], INT count, BOOL test) {return 1;}
INT  interrupt_configure(INT cmd, INT source[], PTYPE adr) {return 1;}
INT frontend_early_init(){ return SUCCESS; };
INT  pre_begin_of_run(INT run_number, char *error){ return SUCCESS; };

/*-- Frontend Init -------------------------------------------------*/

INT frontend_init()
{
#ifdef FORTIFY
    Fortify_CheckAllMemory();
#endif
  return CM_SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/

INT frontend_exit()
{
#ifdef FORTIFY
  Fortify_CheckAllMemory();
  Fortify_OutputStatistics();
#endif
  return CM_SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/

#ifdef FORTIFY
static DWORD last_time_fortify = 0;
#endif

INT frontend_loop()
{
  INT status;

#ifdef FORTIFY
  Fortify_CheckAllMemory();
#endif

#ifdef FORTIFY
  /* time to display fortify output? */
  if (ss_millitime()-last_time_fortify >  600000) {
    Fortify_OutputStatistics();
    last_time_fortify = ss_millitime(); 
  }
#endif

  //status = cm_client_in_list();
  //if (status == SS_ABORT) return status;

  //ss_sleep(2500);
  return CM_SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT begin_of_run(INT run_number, char *error)
{
  return CM_SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT end_of_run(INT run_number, char *error)
{
  return CM_SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT pause_run(INT run_number, char *error)
{
  return CM_SUCCESS;
}

/*-- Resuem Run ----------------------------------------------------*/

INT resume_run(INT run_number, char *error)
{
  return CM_SUCCESS;
}

/*------------------------------------------------------------------*/
