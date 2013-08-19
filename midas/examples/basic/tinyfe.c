/********************************************************************\

  Name:         tinyfe.c
  Created by:   Stefan Ritt

  Contents:     Experiment specific readout code (user part) of
                Midas frontend. This example demonstrate the
                implementation of the sub-events packing.
 
  $Log: tinyfe.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.1  2003/05/17 17:59:09  midas
  demo code

  Revision 1.1  2003/05/06 16:49:05  pierre
  super event demo

  Revision 1.16  2003/04/14 13:31:17  midas
  Enabled trigger_bank_list

  Revision 1.15  2003/04/14 13:17:01  midas
  Added bank description

  Revision 1.14  2002/05/16 21:09:53  midas
  Added max_event_size_frag

  Revision 1.11  2000/08/21 10:32:51  midas
  Added max_event_size, set event_buffer_size = 10*max_event_size

  Revision 1.10  2000/03/13 18:53:29  pierre
  - Added 2nd arg in readout functions (offset for Super event)

  Revision 1.9  2000/03/02 22:00:00  midas
  Added number of subevents as zero

  Revision 1.8  1999/02/24 16:27:01  midas
  Added some "real" readout code

  Revision 1.7  1999/01/20 09:03:38  midas
  Added LAM_SOURCE_CRATE and LAM_SOURCE_STATION macros

  Revision 1.6  1999/01/19 10:27:30  midas
  Use new LAM_SOURCE and LAM_STATION macros

  Revision 1.5  1998/11/09 09:14:41  midas
  Added code to simulate random data

  Revision 1.4  1998/10/29 14:27:46  midas
  Added note about FE_ERR_HW in frontend_init()

  Revision 1.3  1998/10/28 15:50:58  midas
  Changed lam to DWORD

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "midas.h"
#include "msystem.h"
#include "mcstd.h"

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

/*-- Globals -------------------------------------------------------*/
/* The frontend name (client name) as seen by other MIDAS clients   */
   char *frontend_name = "tinyfe";

/* The frontend file name, don't change it */
   char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
   BOOL frontend_call_loop = TRUE;

/* a frontend status page is displayed with this frequency in ms */
   INT display_period = 0000;

/* maximum event size produced by this frontend */
   INT max_event_size = 10000;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
   INT max_event_size_frag = 5 * 1024 * 1024;

/* buffer size to hold events */
   INT event_buffer_size = 10 * 10000;


/* MY HV structure */
   HNDLE hDB, hKey;

/*-- Function declarations -----------------------------------------*/

   INT frontend_init();
   INT frontend_exit();
   INT begin_of_run(INT run_number, char *error);
   INT end_of_run(INT run_number, char *error);
   INT pause_run(INT run_number, char *error);
   INT resume_run(INT run_number, char *error);
   INT frontend_loop();
   INT read_tiny_event(char *pevent, INT off);
/*-- Equipment list ------------------------------------------------*/

#undef USE_INT

   EQUIPMENT equipment[] = {

      {"Tiny",                  /* equipment name */
       3, 0,                    /* event ID, trigger mask */
       "SYSTEM",                /* event buffer */
       EQ_POLLED,               /* equipment type */
       1,                       /* event source */
       "MIDAS",                 /* format */
       TRUE,                    /* enabled */
       RO_RUNNING,              /* read when running and on transitions */
       500,                     /* poll 500ms */
       0,                       /* stop run after this event limit */
       10,                      /* number of sub events */
       0,                       /* log history */
       "", "", "",
       read_tiny_event,         /* readout routine */
       NULL, NULL,              /* keep null */
       NULL,                    /* init string */
       },

      {""}
   };

#ifdef __cplusplus
}
#endif
/********************************************************************\
              Callback routines for system transitions

  These routines are called whenever a system transition like start/
  stop of a run occurs. The routines are called on the following
  occations:

  frontend_init:  When the frontend program is started. This routine
                  should initialize the hardware.
  
  frontend_exit:  When the frontend program is shut down. Can be used
                  to releas any locked resources like memory, commu-
                  nications ports etc.

  begin_of_run:   When a new run is started. Clear scalers, open
                  rungates, etc.

  end_of_run:     Called on a request to stop a run. Can send 
                  end-of-run event and close run gates.

  pause_run:      When a run is paused. Should disable trigger events.

  resume_run:     When a run is resumed. Should enable trigger events.

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \********************************************************************//*-- Frontend Init -------------------------------------------------*/
    INT frontend_init()
{
   return SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/

INT frontend_exit()
{
   return SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT begin_of_run(INT run_number, char *error)
{
   return SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT end_of_run(INT run_number, char *error)
{
   return SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT pause_run(INT run_number, char *error)
{
   return SUCCESS;
}

/*-- Resuem Run ----------------------------------------------------*/

INT resume_run(INT run_number, char *error)
{
   return SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/
INT frontend_loop()
{
   return SUCCESS;
}

/*------------------------------------------------------------------*/

/********************************************************************\
  
  Readout routines for different events

\********************************************************************/

/*-- Trigger event routines ----------------------------------------*/

INT poll_event(INT source, INT count, BOOL test)
/* Polling routine for events. Returns TRUE if event
   is available. If test equals TRUE, don't return. The test
   flag is used to time the polling */
{
   int i;
   DWORD lam;

   for (i = 0; i < count; i++) {
      cam_lam_read(LAM_SOURCE_CRATE(source), &lam);

//  To force a LAM independently of the driver.
      lam = 1;
      if (lam & LAM_SOURCE_STATION(source))
         if (!test)
            return lam;
   }

   return 0;
}

/*-- Interrupt configuration ---------------------------------------*/

INT interrupt_configure(INT cmd, INT source, PTYPE adr)
{
   switch (cmd) {
   case CMD_INTERRUPT_ENABLE:
      break;
   case CMD_INTERRUPT_DISABLE:
      break;
   case CMD_INTERRUPT_ATTACH:
      break;
   case CMD_INTERRUPT_DETACH:
      break;
   }
   return SUCCESS;
}


/*-- Tiny event --------------------------------------------------*/
#define NWORDS 3

INT read_tiny_event(char *pevent, INT offset)
{
   static WORD *pdata = NULL;
   static WORD sub_counter = 0;
   // Super event structure 
   if (offset == 0) {           // FIRST event of the Super event 
      bk_init(pevent);
      bk_create(pevent, "SUPR", TID_WORD, &pdata);
      sub_counter = 1;
   } else if (offset == -1) {   // CLOSE Super event
      bk_close(pevent, pdata);
      return bk_size(pevent);
   }
   // READ event
   *pdata++ = 0xB0E;
   *pdata++ = sub_counter++;
   *pdata++ = 0xE0E;

   if (offset == 0) {
      // Compute the proper event length on the FIRST event in the Super Event
      // NWORDS correspond to the !! NWORDS WORD above !!
      // sizeof(BANK_HEADER) + sizeof(BANK) will make the 16 bytes header
      // sizeof(WORD) is defined by the TID_WORD in bk_create()
      return NWORDS * sizeof(WORD) + sizeof(BANK_HEADER) + sizeof(BANK);
   } else {
      // Return the data section size only
      // sizeof(WORD) is defined by the TID_WORD in bk_create()
      return NWORDS * sizeof(WORD);
   }
}
