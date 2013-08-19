/********************************************************************\

  Name:         ybos_simfe.c
  Created by:   Stefan Ritt/Pierre-A. Amaudruz

  Contents:     Experiment specific readout code (user part) of
                Midas frontend. This example simulates a "trigger
                event" and a "scaler event" which are filled with
                random data. The trigger event is filled with
                two banks (ADC0 and TDC0), the scaler event with 
                one bank (SCLR).

  $Log: ybos_simfe.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.7  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.6  2001/10/05 22:38:45  pierre
  correct cam_interrupt_xxx() arg list.

  Revision 1.5  2001/04/30 19:55:16  pierre
  - Correct max_event_size, event_buffer_size declaration
  - fix data increment in read_event_trigger for double pointer

  Revision 1.4  2000/03/01 18:04:38  pierre
  - include Super event support

  Revision 1.3  1999/07/16 00:08:50  pierre
  -Added D8 ybos bank, random event size and event length

  Revision 1.2  1998/10/12 12:18:59  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include "midas.h"
#include "msystem.h"
#include "ybos.h"
#include "mcstd.h"
#include "esone.h"

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

/*-- Globals -------------------------------------------------------*/
#define   CRATE    1
#define   LAM_SLOT 1
   INT poll_val = 10;           /* .1% */
   INT tr1 = 16, tr2 = 200;
/* The frontend name (client name) as seen by other MIDAS clients   */
   char *frontend_name = "YbosFE";
/* The frontend file name, don't change it */
   char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
   BOOL frontend_call_loop = FALSE;

/* a frontend status page is displayed with this frequency in ms */
   INT display_period = 3000;

/* maximum event size produced by this frontend */
   INT max_event_size = 10000;

/* buffer size to hold events */
   INT event_buffer_size = 10 * 10000;

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
   INT files_dump(char *pevent, INT off);
   INT gbl_run_number;
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
       0,                       /* event source */
       "YBOS",                  /* format */
       TRUE,                    /* enabled */
       RO_RUNNING,              /* read only when running */
       500,                     /* poll for 500ms */
       0,                       /* stop run after this event limit */
       0,                       /* number of sub event */
       0,                       /* don't log history */
       "", "", "",
       read_trigger_event,      /* readout routine */
       NULL,                    /* keep null */
       NULL                     /* init string */
       },

      {"Scaler",                /* equipment name */
       2, 0,                    /* event ID, trigger mask */
       "SYSTEM",                /* event buffer */
       EQ_PERIODIC,             /* equipment type */
       0,                       /* event source */
       "YBOS",                  /* format */
       TRUE,                    /* enabled */
       RO_RUNNING | RO_TRANSITIONS |    /* read when running and on transitions */
       RO_ODB,                  /* and update ODB */
       10000,                   /* read every 10 sec */
       0,                       /* stop run after this event limit */
       0,                       /* number of sub event */
       0,                       /* log history */
       "", "", "",
       read_scaler_event,       /* readout routine */
       NULL,                    /* keep null */
       NULL                     /* init string */
       },

      {"File",                  /* equipment name */
       7, 0x0000,               /* event ID, mask */
       "SYSTEM",                /* event buffer */
       EQ_PERIODIC,             /* equipment type */
       0,                       /* event source */
       "YBOS",                  /* format */
       FALSE,                   /* enabled */
       RO_BOR,                  /* read only at BOR */
       10000,                   /* read every 10 sec */
       0,                       /* stop run after this event limit */
       0,                       /* number of sub event */
       0,                       /* don't log history */
       "", "", "",
       files_dump,              /* file dump */
       NULL,                    /* keep null */
       NULL                     /* init string */
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
   /* put here hardware initialization */
   cam_init();
   return SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/

INT frontend_exit()
{
   cam_exit();
   return SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT begin_of_run(INT run_number, char *error)
{
   int ext, q;
   unsigned long d;
   /* put here clear scalers etc. */
   gbl_run_number = run_number;

   cdreg(&ext, 1, 2, 3, 4);
   cfsa(16, ext, &d, &q);
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
      the frontend is idle or between every event */
   return SUCCESS;
}

/*------------------------------------------------------------------*/

/********************************************************************\
  
  Readout routines for different events

\********************************************************************/

/*-- Trigger event routines ----------------------------------------*/

INT poll_event(INT source, INT count, BOOL test)
{
   INT i;

   for (i = 0; i < count; i++) {
      /* read the LAM register of the Crate controller to find out if
         ANY station has raised the LAM line.
       */

      if (rand() % 10000 < poll_val)    /* Ignore LAM if not LAM_STATION */
         if (!test)
            return TRUE;
   }
   return FALSE;
}

/*-- Interrupt configuration for trigger event ---------------------*/
INT interrupt_configure(INT cmd, INT source[], PTYPE adr)
{
   switch (cmd) {
   case CMD_INTERRUPT_ENABLE:
      cam_interrupt_enable(CRATE);
      break;
   case CMD_INTERRUPT_DISABLE:
      cam_interrupt_disable(CRATE);
      break;
   case CMD_INTERRUPT_ATTACH:
      cam_interrupt_attach(CRATE, LAM_SLOT, (void (*)(void)) adr);
      break;
   case CMD_INTERRUPT_DETACH:
      cam_interrupt_detach(CRATE, LAM_SLOT);
      break;
   }
   return SUCCESS;
}

/*-- Event readout -------------------------------------------------*/

INT read_trigger_event(char *pevent, INT off)
{
   INT i, j;
   double *pbkdat;
   ybk_init((DWORD *) pevent);

   j = rand() % 100;
   if (j < tr1)
      j = tr1;
   else if (j > tr2)
      j = tr2;
   ybk_create((DWORD *) pevent, "ADC0", I4_BKTYPE, (DWORD *) (&pbkdat));
   for (i = 0; i < j; i++)
      *((DWORD *) pbkdat)++ = i & 0xFFF;
   ybk_close((DWORD *) pevent, pbkdat);

   ybk_create((DWORD *) pevent, "TDC0", I2_BKTYPE, (DWORD *) (&pbkdat));
   for (i = 0; i < 16; i++)
      *((WORD *) pbkdat)++ = (WORD) (0x10 + i) & 0xFFFF;
   ybk_close((DWORD *) pevent, pbkdat);

   ybk_create_chaos((DWORD *) pevent, "OTHR", I2_BKTYPE, (DWORD *) (&pbkdat));
   for (i = 0; i < 16; i++)
      *((WORD *) pbkdat)++ = (WORD) (0x20 + i + 1) & 0xFFFF;
   ybk_close_chaos((DWORD *) pevent, I2_BKTYPE, pbkdat);

   ybk_create((DWORD *) pevent, "EFGH", D8_BKTYPE, (DWORD *) (&pbkdat));
   for (i = 0; i < 6; i++)
      *pbkdat++ = (double) i + 10.;
   ybk_close((DWORD *) pevent, pbkdat);
   /* END OF EVENT */

   return (ybk_size((DWORD *) pevent));
}

/*-- Scaler event --------------------------------------------------*/

INT read_scaler_event(char *pevent, INT off)
{
   DWORD i;
   DWORD *pbkdat;

   ybk_init((DWORD *) pevent);

   /* collect user hardware data */
  /*---- USER Stuff ----*/
   ybk_create((DWORD *) pevent, "SCAL", I4_BKTYPE, (DWORD *) (&pbkdat));
   for (i = 0; i < 16; i++)
      *pbkdat++ = (DWORD) (0xff + i) & 0xFFF;
   ybk_close((DWORD *) pevent, pbkdat);

   return (ybk_size((DWORD *) pevent));
}

/*-- File event -------------------------------------------------*/
INT files_dump(char *pevent, INT off)
{
   feodb_file_dump(equipment, "File", pevent, gbl_run_number, "trigger");

   /* return 0 because I handle the "event send" by myself */
   return 0;
}
