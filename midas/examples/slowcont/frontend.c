/********************************************************************\

  Name:         frontend.c
  Created by:   Stefan Ritt

  Contents:     Example Slow Control Frontend program. Defines two
                slow control equipments, one for a HV device and one
                for a multimeter (usually a general purpose PC plug-in
                card with A/D inputs/outputs. As a device driver,
                the "null" driver is used which simulates a device
                without accessing any hardware. The used class drivers
                cd_hv and cd_multi act as a link between the ODB and
                the equipment and contain some functionality like
                ramping etc. To form a fully functional frontend,
                the device driver "null" has to be replaces with
                real device drivers.


  $Log: frontend.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.11  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.10  2002/06/10 07:15:17  midas
  Use new DF_xxx flags

  Revision 1.9  2002/05/29 13:35:47  midas
  Added max_event_size_frag

  Revision 1.8  2002/05/13 22:21:08  midas
  Fixed typo

  Revision 1.7  2002/03/13 08:39:28  midas
  Use bus drivers in examples

  Revision 1.6  2000/08/21 10:49:11  midas
  Added max_event_size

  Revision 1.5  2000/03/02 22:00:47  midas
  Added number of subevents in equipment list

  Revision 1.4  1999/12/21 09:38:23  midas
  Use new driver names

  Revision 1.3  1998/10/23 08:46:26  midas
  Added #include "null.h"

  Revision 1.2  1998/10/12 12:18:59  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include "midas.h"
#include "class/hv.h"
#include "class/multi.h"
#include "device/nulldev.h"
#include "bus/null.h"

/*-- Globals -------------------------------------------------------*/

/* The frontend name (client name) as seen by other MIDAS clients   */
char *frontend_name = "SC Frontend";
/* The frontend file name, don't change it */
char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = FALSE;

/* a frontend status page is displayed with this frequency in ms    */
INT display_period = 1000;

/* maximum event size produced by this frontend */
INT max_event_size = 10000;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
INT max_event_size_frag = 5 * 1024 * 1024;

/* buffer size to hold events */
INT event_buffer_size = 10 * 10000;

/*-- Equipment list ------------------------------------------------*/

/* device driver list */
DEVICE_DRIVER hv_driver[] = {
   {"Dummy Device", nulldev, 16, null},
   {""}
};

DEVICE_DRIVER multi_driver[] = {
   {"Input", nulldev, 2, null, DF_INPUT},
   {"Output", nulldev, 2, null, DF_OUTPUT},
   {""}
};


EQUIPMENT equipment[] = {

   {"HV",                       /* equipment name */
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
    cd_hv_read,                 /* readout routine */
    cd_hv,                      /* class driver main routine */
    hv_driver,                  /* device driver list */
    NULL,                       /* init string */
    },

   {"Environment",              /* equipment name */
    4, 0,                       /* event ID, trigger mask */
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
    cd_multi_read,              /* readout routine */
    cd_multi,                   /* class driver main routine */
    multi_driver,               /* device driver list */
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
