/********************************************************************\

  Name:         frontend.c
  Created by:   Stefan Ritt

  Contents:     Example Slow Control Frontend for equipment control
                through EPICS channel access.

  $Log: frontend.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.8  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.7  2002/05/10 00:42:32  pierre
  additional arg for BUS_DRIVER

  Revision 1.6  2000/03/02 21:54:48  midas
  Added comments concerning channel names and possibility to disable functions

  Revision 1.5  1999/12/21 09:37:00  midas
  Use new driver names

  Revision 1.3  1999/10/08 14:04:45  midas
  Set initial channel number to 10

  Revision 1.2  1999/09/22 12:01:09  midas
  Fixed compiler warning

  Revision 1.1  1999/09/22 09:19:25  midas
  Added sources

  Revision 1.3  1998/10/23 08:46:26  midas
  Added #include "null.h"

  Revision 1.2  1998/10/12 12:18:59  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include "midas.h"
#include "class/generic.h"
#include "device/epics_ca.h"

/*-- Globals -------------------------------------------------------*/

/* The frontend name (client name) as seen by other MIDAS clients   */
char *frontend_name = "BL Frontend";
/* The frontend file name, don't change it */
char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = TRUE;

/* a frontend status page is displayed with this frequency in ms    */
INT display_period = 1000;

/* maximum event size produced by this frontend */
INT max_event_size = 10000;

/* buffer size to hold events */
INT event_buffer_size = 10 * 10000;

/*-- Equipment list ------------------------------------------------*/

/* 
The following statement allocates 10 channels for the beamline
control through the epics channel access device driver. The 
EPICS channel names are stored under 
   
  /Equipment/Beamline/Settings/Devices/Beamline

while the channel names as the midas slow control sees them are
under 

  /Equipment/Bemaline/Settings/Names

An example set of channel names is saved in the triumf.odb file
in this directory and can be loaded into the ODB with the odbedit
command

  load triumf.odb

before the frontend is started. The CMD_SET_LABEL statement 
actually defines who determines the label name. If this flag is
set, the CMD_SET_LABEL command in the device driver is disabled,
therefore the label is taken from EPICS, otherwise the label is
taken from MIDAS and set in EPICS.

The same can be done with the demand values. If the command
CMD_SET_DEMAND is disabled, the demand value is always determied
by EPICS.
*/

/* device driver list */
DEVICE_DRIVER epics_driver[] = {
   {"Beamline", epics_ca, 10, NULL, 0, CMD_SET_LABEL},  /* disable CMD_SET_LABEL */
   {""}
};

EQUIPMENT equipment[] = {

   {"Beamline",                 /* equipment name */
    3, 0,                       /* event ID, trigger mask */
    "SYSTEM",                   /* event buffer */
    EQ_SLOW,                    /* equipment type */
    0,                          /* event source */
    "FIXED",                    /* format */
    TRUE,                       /* enabled */
    RO_RUNNING | RO_TRANSITIONS,        /* read when running and on transitions */
    60000,                      /* read every 60 sec */
    0,                          /* stop run after this event limit */
    0,                          /* number of sub events */
    1,                          /* log history every event */
    "", "", "",
    cd_gen_read,                /* readout routine */
    cd_gen,                     /* class driver main routine */
    epics_driver,               /* device driver list */
    NULL,                       /* init string */
    },

   {""}
};

/*-- Dummy routines ------------------------------------------------*/

INT poll_event(INT source[], INT count, BOOL test)
{
   return 1;
};
INT interrupt_configure(INT cmd, INT source[], PTYPE adr)
{
   return 1;
};

/*-- Frontend Init -------------------------------------------------*/

INT frontend_init()
{
   /* anable/disable certain command in device driver */


   return CM_SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/

INT frontend_exit()
{
   return CM_SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/

INT frontend_loop()
{
   /* slow down frontend not to eat all CPU cycles */
   ss_sleep(200);

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
