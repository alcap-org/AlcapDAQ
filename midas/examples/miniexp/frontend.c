/********************************************************************\

  Name:         frontend.c
  Created by:   Stefan Ritt

  Contents:     Experiment specific readout code (user part) of
                Midas frontend. This example simulates a "trigger
                event" and a "scaler event" which are filled with
                CAMAC or random data. The trigger event is filled 
                with two banks (ADC0 and TDC0), the scaler event 
                with one bank (SCLR).

  $Log: frontend.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.6  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.5  2003/08/20 21:35:20  pierre
  fix max_event_size

  Revision 1.4  2001/12/17 18:35:35  pierre
  update for newer version

  Revision 1.3  2000/08/21 10:49:11  midas
  Added max_event_size

  Revision 1.2  1999/02/24 16:26:46  midas
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
#include "mcstd.h"
#include "experim.h"

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

/*-- Globals -------------------------------------------------------*/

/* The frontend name (client name) as seen by other MIDAS clients   */
   char *frontend_name = "Sample Frontend";
/* The frontend file name, don't change it */
   char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
   BOOL frontend_call_loop = FALSE;

/* a frontend status page is displayed with this frequency in ms */
   INT display_period = 3000;

/* maximum event size produced by this frontend */
   INT max_event_size = 10000;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
   INT max_event_size_frag = 5 * 1024 * 1024;

/* buffer size to hold events */
   INT event_buffer_size = 10 * 10000;

/* number of channels */
#define N_ADC  8
#define N_TDC  8
#define N_SCLR 8

/* CAMAC crate and slots */
#define CRATE      0
#define SLOT_IO   23
#define SLOT_ADC   1
#define SLOT_TDC   2
#define SLOT_SCLR  3

/*-- Function declarations -----------------------------------------*/

   INT frontend_init();
   INT frontend_exit();
   INT begin_of_run(INT run_number, char *error);
   INT end_of_run(INT run_number, char *error);
   INT pause_run(INT run_number, char *error);
   INT resume_run(INT run_number, char *error);
   INT frontend_loop();

   INT read_trigger_event(char *pevent, INT off);
   INT read_scaler_event(char *pevent, INT off);

/*-- Equipment list ------------------------------------------------*/

#undef USE_INT

   EQUIPMENT equipment[] = {

      {"Trigger",               /* equipment name */
       1, 0,                    /* event ID, trigger mask */
       "SYSTEM",                /* event buffer */
#ifdef USE_INT
       EQ_INTERRUPT,            /* equipment type */
#else
       EQ_POLLED,               /* equipment type */
#endif
       LAM_SOURCE(0, 0xFFFFFF), /* event source crate 0, all stations */
       "MIDAS",                 /* format */
       TRUE,                    /* enabled */
       RO_RUNNING |             /* read only when running */
       RO_ODB,                  /* and update ODB */
       500,                     /* poll for 500ms */
       0,                       /* stop run after this event limit */
       0,                       /* number of sub events */
       0,                       /* don't log history */
       "", "", "",
       read_trigger_event,      /* readout routine */
       },

      {"Scaler",                /* equipment name */
       2, 0,                    /* event ID, trigger mask */
       "SYSTEM",                /* event buffer */
       EQ_PERIODIC,             /* equipment type */
       0,                       /* event source */
       "MIDAS",                 /* format */
       TRUE,                    /* enabled */
       RO_RUNNING | RO_TRANSITIONS |    /* read when running and on transitions */
       RO_ODB,                  /* and update ODB */
       10000,                   /* read every 10 sec */
       0,                       /* stop run after this event limit */
       0,                       /* number of sub events */
       0,                       /* log history */
       "", "", "",
       read_scaler_event,       /* readout routine */
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
   /* hardware initialization */

   cam_init();
   cam_crate_clear(CRATE);
   cam_crate_zinit(CRATE);

   /* enable LAM in IO unit */
   camc(CRATE, SLOT_IO, 0, 26);

   /* enable LAM in crate controller */
   cam_lam_enable(CRATE, SLOT_IO);

   /* reset external LAM Flip-Flop */
   camo(CRATE, SLOT_IO, 1, 16, 0xFF);
   camo(CRATE, SLOT_IO, 1, 16, 0);

   /* print message and return FE_ERR_HW if frontend should not be started */

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
   /* put here clear scalers etc. */

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
   /* if frontend_call_loop is true, this routine gets called when
      the frontend is idle or once between every event */
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

/*-- Event readout -------------------------------------------------*/

INT read_trigger_event(char *pevent, INT off)
{
   WORD *pdata, a;
   INT q, timeout;

   /* init bank structure */
   bk_init(pevent);

   /* create ADC bank */
   bk_create(pevent, "ADC0", TID_WORD, &pdata);

   /* wait for ADC conversion */
   for (timeout = 100; timeout > 0; timeout--) {
      camc_q(CRATE, SLOT_ADC, 0, 8, &q);
      if (q)
         break;
   }
   if (timeout == 0)
      ss_printf(0, 10, "No ADC gate!");

   /* read ADC bank */
   for (a = 0; a < N_ADC; a++)
      cami(CRATE, SLOT_ADC, a, 0, pdata++);

   /* instead of using cami(), use following line to "simulate" data */
   /* *pdata++ = rand() % 1024; */

   /* clear ADC */
   camc(CRATE, SLOT_ADC, 0, 9);

   bk_close(pevent, pdata);

   /* create TDC bank */
   bk_create(pevent, "TDC0", TID_WORD, &pdata);

   /* read TDC bank */
   for (a = 0; a < N_TDC; a++)
      cami(CRATE, SLOT_TDC, a, 0, pdata++);

   /* clear TDC */
   camc(CRATE, SLOT_TDC, 0, 9);

   bk_close(pevent, pdata);

   /* clear IO unit LAM */
   camc(CRATE, SLOT_IO, 0, 10);

   /* clear LAM in crate controller */
   cam_lam_clear(CRATE, SLOT_IO);

   /* reset external LAM Flip-Flop */
   camo(CRATE, SLOT_IO, 1, 16, 0xFF);
   camo(CRATE, SLOT_IO, 1, 16, 0);

   return bk_size(pevent);
}

/*-- Scaler event --------------------------------------------------*/

INT read_scaler_event(char *pevent, INT off)
{
   DWORD *pdata, a;

   /* init bank structure */
   bk_init(pevent);

   /* create SCLR bank */
   bk_create(pevent, "SCLR", TID_DWORD, &pdata);

   /* read scaler bank */
   for (a = 0; a < N_SCLR; a++)
      cam24i(CRATE, SLOT_SCLR, a, 0, pdata++);

   bk_close(pevent, pdata);

   return bk_size(pevent);
}
