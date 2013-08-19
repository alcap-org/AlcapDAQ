/********************************************************************\

  Name:         ebfe.c
  Created by:   Stefan Ritt / Pierre-Andre Amaudruz

  Contents:     Frontend code for mevb example.
                Source code taken from the frontend.c example.
		Modification of:
		*frontend_name
		Trigger eqp name : Trigger1
		Scaler  eqp name : Scaler1
		Trigger eqp buffer name : BUF1
		Added cm_yield(10) in the poll_event()
		data rate reduction.
		
		Simplified the event generator and should be linked
		with the camacnul driver for the mevb example.
		But nothing prevent to generate real event using
		proper driver.
		
  $Log: ebfe.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.1  2004/09/29 16:19:47  pierre
  for new mevb

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  2002/07/13 05:46:25  pierre
  added frag declaration

  Revision 1.1.1.1  2002/01/17 20:22:16  pierre
  Initial Version

\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "midas.h"
#include "mcstd.h"

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

/*-- Globals -------------------------------------------------------*/

  /* The frontend name (client name) as seen by other MIDAS clients   */
  char *frontend_name = "ebfe";
  /* The frontend file name, don't change it */
  char *frontend_file_name = __FILE__;

  /* frontend_loop is called periodically if this variable is TRUE    */
  BOOL frontend_call_loop = FALSE;

  /* a frontend status page is displayed with this frequency in ms */
  INT display_period = 3000;

  /* maximum event size produced by this frontend */
  INT max_event_size = 1000;
  INT max_event_size_frag = 0;

  /* buffer size to hold events */
  INT event_buffer_size = 10 * 1000;

  /* bank name list */
  char Adcx[5], Tdcx[5], Sclx[5];

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
   extern INT get_frontend_index();

/*-- Equipment list ------------------------------------------------*/

#undef USE_INT

   EQUIPMENT equipment[] = {

      {"Trigger",              /* equipment name */
       1, TRIGGER_ALL,         /* event ID, trigger mask */
       "BUF",                  /* event buffer */
#ifdef USE_INT
       EQ_INTERRUPT,            /* equipment type */
#else
       EQ_POLLED | EQ_EB,       /* equipment type */
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

      {"Scaler",               /* equipment name */
       2, TRIGGER_ALL,         /* event ID, trigger mask */
       "BIF",                  /* event buffer */
       EQ_PERIODIC | EQ_EB | 
       EQ_MANUAL_TRIG,    /* equipment type */
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
\********************************************************************/
INT frontend_init()
{
  /* Compose expected bank name under Ebuilder */
  sprintf(Adcx, "AD%02d", get_frontend_index());
  sprintf(Tdcx, "TC%02d", get_frontend_index());
  sprintf(Sclx, "SC%02d", get_frontend_index());

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

/*-- Resume Run ----------------------------------------------------*/
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
    cm_yield(10);
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
   bk_create(pevent, Adcx, TID_WORD, &pdata);

   /* read ADC bank */
   for (a = 0; a < N_ADC; a++)
      *pdata++ = a + 10;

   /* clear ADC */
   camc(CRATE, SLOT_ADC, 0, 9);

   bk_close(pevent, pdata);

   /* create TDC bank */
   bk_create(pevent, Tdcx, TID_WORD, &pdata);

   /* read TDC bank */
   for (a = 0; a < N_TDC; a++)
      *pdata++ = a + 20;

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
   bk_create(pevent, Sclx, TID_DWORD, &pdata);

   /* read scaler bank */
   for (a = 0; a < N_SCLR; a++)
      cam24i(CRATE, SLOT_SCLR, a, 0, pdata++);

   bk_close(pevent, pdata);

   return bk_size(pevent);
}
