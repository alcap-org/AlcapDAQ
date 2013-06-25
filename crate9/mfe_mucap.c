/********************************************************************\

  Name:         mfe.c
  Created by:   Stefan Ritt

  Contents:     The system part of the MIDAS frontend. Has to be
                linked with user code to form a complete frontend

  ** muCap patches **
  - added frontend_early_init() to allow the frontend to modify its name
    and equipment structure. 
  - added extra transition function pre_begin_of_run().
  - turned off buffer caching, which doesn't make sense for our large
    event sizes and which seemed to lead to instability.
  - turned on "event channel."

  $Log: mfe.c,v $
  Revision 1.71  2004/10/01 23:35:53  midas
  Removed PRE/POST transitions and implemented sequence order of transitions

  Revision 1.70  2004/09/29 19:38:20  midas
  Decreased run stop polling time from 500ms to 100ms

  Revision 1.69  2004/09/29 00:16:21  midas
  Fixed shutdown problem with EB frontends

  Revision 1.68  2004/09/28 23:56:53  midas
  Added -i flag and EQ_EB flag for event building

  Revision 1.67  2004/09/24 21:42:17  midas
  Send manually triggered events in scheduler

  Revision 1.66  2004/09/21 21:09:15  midas
  Updated event statistics to ODB properly at EOR

  Revision 1.65  2004/05/07 19:40:11  midas
  Replaced min/max by MIN/MAX macros

  Revision 1.64  2004/03/26 09:31:56  midas
  Converted pritnf() to cm_msg() for statistics record error

  Revision 1.63  2004/03/19 09:31:43  midas
  Re-formatted comments

  Revision 1.62  2004/01/08 08:40:10  midas
  Implemented standard indentation

  Revision 1.61  2003/11/24 08:22:46  midas
  Changed timeouts from INT to DWORD, added ignore_timeout to cm_cleanup, adde '-f' flag to ODBEdit 'cleanup'

  Revision 1.60  2003/11/20 11:29:44  midas
  Implemented db_check_record and use it in most places instead of db_create_record

  Revision 1.59  2003/11/01 00:48:14  olchansk
  abort if cannot read /runinfo/run number
  abort if run number resets to zero

  Revision 1.58  2003/10/29 15:33:48  midas
  Properly display message about stopped previous frontend

  Revision 1.57  2003/09/30 19:47:57  midas
  Removed tabs

  Revision 1.56  2003/09/30 18:30:07  midas
  Put CNAF functionality under #ifdef HAVE_CAMAC

  Revision 1.55  2003/05/09 07:40:04  midas
  Added extra parameter to cm_get_environment

  Revision 1.54  2003/05/07 10:57:10  midas
  Removed tabs

  Revision 1.53  2003/04/25 10:41:42  midas
  Removed FTCP mode in update_odb()

  Revision 1.52  2003/04/25 07:45:03  midas
  Write first event to ODB only if logger uses ROOT format

  Revision 1.51  2003/04/14 12:40:23  midas
  Create ODB entries for structured banks derived from BANK_LIST in frontend.c

  Revision 1.50  2003/04/14 05:12:54  pierre
  fix send_event for handle=0

  Revision 1.49  2003/04/01 19:22:44  pierre
  fix update_odb for hKeyl copy

  Revision 1.48  2003/03/31 08:27:34  midas
  Fixed alignment bug for strings

  Revision 1.47  2003/03/28 09:13:05  midas
  Added warning for uncreated structured banks

  Revision 1.46  2003/03/28 08:55:11  midas
  Added code for structured banks

  Revision 1.45  2002/10/15 18:44:23  olchansk
  fix printf() and cm_msg() format mismatches

  Revision 1.44  2002/10/15 18:15:25  olchansk
  disable dtsout, stderr buffering,
  catch-ignore SIGPIPE

  Revision 1.43  2002/10/15 18:13:59  olchansk
  always recreate the statistics record.

  Revision 1.42  2002/10/15 18:09:59  olchansk
  catch bm_xxx() errors and bail out from schedule(). This fixes infinite looping after rpc timeouts.

  Revision 1.41  2002/09/13 07:32:47  midas
  Added client name to cm_cleanup()

  Revision 1.40  2002/06/03 06:07:15  midas
  Added extra parameter to ss_daemon_init to keep stdout

  Revision 1.39  2002/05/15 23:43:56  midas
  Added event fragmentation

  Revision 1.38  2002/05/10 01:41:19  midas
  Added optional debug output to cm_transition

  Revision 1.37  2002/05/08 19:54:40  midas
  Added extra parameter to function db_get_value()

  Revision 1.36  2001/11/21 08:37:32  midas
  Took out db_delete_key for statistics again (not needed)

  Revision 1.35  2001/11/20 19:22:07  pierre
  - Add rpc_flush_event in case no periodic eqp
  - Force rpc_flush_event for low trigger rate
  - Patch /Statistics in case it exists but size=0

  Revision 1.34  2001/06/27 12:34:49  midas
  Added -D flag to become a daemon

  Revision 1.33  2001/04/06 04:13:40  midas
  Put cm_cleanup() in init code

  Revision 1.32  2001/01/29 09:50:53  midas
  Changed send_event() parameter from event_id to equipment index

  Revision 1.31  2000/11/14 08:30:03  midas
  SLOW events are not any more sent to the ODB when the history is on

  Revision 1.30  2000/11/06 10:10:23  midas
  Flush events for set-ups with only non-periodic events

  Revision 1.29  2000/10/23 14:19:06  midas
  Added idle period for slow control equipment

  Revision 1.28  2000/09/28 13:16:02  midas
  Fixed bug that MANUAL_TRIG only events are not read out during transitions

  Revision 1.27  2000/09/28 13:02:03  midas
  Added manual triggered events

  Revision 1.26  2000/08/21 11:01:11  midas
  Changed comments

  Revision 1.25  2000/08/21 10:44:58  midas
  Removed reconnect functionality, it's better to restart the frontend via
  /programs/frontend/auto restart.

  Revision 1.24  2000/08/21 10:36:41  midas
  Reworked event and event buffer sizes:
  - Both max_event_size and event_buffer_size must be defined in user code
  - While max_event_size is used for frontend buffers, MAX_EVENT_SIZE is the
    system limit which must be larger than max_event_size
  - Event size returned from user readout routine is now checked against event
    size limits
  - Buffer settings are now always displayed, not only under VxWorks

  Revision 1.23  2000/08/09 12:02:43  midas
  Evaluate return status in tr_xxx functions properly

  Revision 1.22  2000/08/09 11:37:56  midas
  Rearranged serial number increments

  Revision 1.21  2000/07/11 16:43:40  pierre
  - Fix serial number for POLL super event.

  Revision 1.20  2000/04/03 12:27:43  midas
  Changed auto restart to 20 seconds in main loop

  Revision 1.19  2000/03/22 14:42:48  midas
  Fixed bug with invalid pointer

  Revision 1.18  2000/03/17 13:00:05  midas
  Frontends use default timeout fo 60 sec.

  Revision 1.17  2000/03/01 23:29:20  midas
  Fixed bug with wrong event header data size in super eventsvents mfe.c

  Revision 1.16  2000/02/29 21:59:45  midas
  Added auto restart

  Revision 1.15  2000/02/26 01:25:54  midas
  Fixed bug that number of sent events was not cleared at start

  Revision 1.14  2000/02/25 20:22:49  midas
  Added super-event scheme

  Revision 1.13  2000/02/24 22:38:19  midas
  Outcommented USE_EVENT_CHANNEL

  Revision 1.12  2000/02/24 22:29:24  midas
  Added deferred transitions

  Revision 1.11  1999/12/08 00:50:29  pierre
  - fix EVID (ybos: strncpy to strncmp)

  Revision 1.10  1999/11/24 00:49:59  pierre
  - YBOS reject EVID bank in udpate_odb with eb_functions

  Revision 1.9  1999/11/23 18:25:25  pierre
  - YBOS reject EVID bank in udpate_odb

  Revision 1.8  1999/10/18 14:41:51  midas
  Use /programs/<name>/Watchdog timeout in all programs as timeout value. The
  default value can be submitted by calling cm_connect_experiment1(..., timeout)

  Revision 1.7  1999/10/15 12:17:52  midas
  Increased timeout

  Revision 1.6  1999/09/27 13:49:04  midas
  Added bUnique parameter to cm_shutdown

  Revision 1.5  1999/09/23 12:45:48  midas
  Added 32 bit banks

  Revision 1.4  1999/06/23 09:38:51  midas
  - Added D8_BKTYPE
  - Fixed CAMAC server F24 bug
  - cm_synchronize only called for VxWorks

  Revision 1.3  1998/12/10 12:50:47  midas
  Program abort with "!" now works without a return under UNIX

  Revision 1.2  1998/10/12 12:19:01  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <assert.h>
#include "midas.h"
#include "msystem.h"
#include "mcstd.h"

#ifdef YBOS_SUPPORT
#include "ybos.h"
#endif

/*------------------------------------------------------------------*/

/* items defined in frontend.c */

extern char *frontend_name;
extern char *frontend_file_name;
extern int crate_number;
extern BOOL frontend_call_loop;
extern INT max_event_size;
extern INT max_event_size_frag;
extern INT event_buffer_size;
extern INT display_period;
extern INT frontend_init(void);
extern INT frontend_exit(void);
extern INT frontend_loop(void);
extern INT begin_of_run(INT run_number, char *error);
extern INT end_of_run(INT run_number, char *error);
extern INT pause_run(INT run_number, char *error);
extern INT resume_run(INT run_number, char *error);
extern INT poll_event(INT source, INT count, BOOL test);
extern INT interrupt_configure(INT cmd, INT source, PTYPE adr);

#ifdef PART_OF_MUCAP
extern INT frontend_early_init(void);
extern INT  pre_begin_of_run(INT run_number, char *error);
#endif

/*------------------------------------------------------------------*/

/* globals */

#ifdef PART_OF_MUCAP
#define USE_EVENT_CHANNEL 1
#define SERVER_CACHE_SIZE  0
#else
#undef USE_EVENT_CHANNEL
#define SERVER_CACHE_SIZE  100000       /* event cache before buffer */
#endif


#define ODB_UPDATE_TIME      1000       /* 1 seconds for ODB update */

#define DEFAULT_FE_TIMEOUT  60000       /* 60 seconds for watchdog timeout */

INT run_state;                     /* STATE_RUNNING, STATE_STOPPED, STATE_PAUSED */
INT run_number;
DWORD actual_time;                 /* current time in seconds since 1970 */
DWORD actual_millitime;            /* current time in milliseconds */

char host_name[HOST_NAME_LENGTH];
char exp_name[NAME_LENGTH];
char full_frontend_name[256];

INT max_bytes_per_sec;
INT optimize = 0;                  /* set this to one to opimize TCP buffer size */
INT fe_stop = 0;                   /* stop switch for VxWorks */
BOOL debug;                        /* disable watchdog messages from server */
DWORD auto_restart = 0;            /* restart run after event limit reached stop */
INT manual_trigger_event_id = 0;   /* set from the manual_trigger callback */
INT frontend_index = -1;           /* frontend index for event building */

HNDLE hDB;

#ifdef YBOS_SUPPORT
struct {
   DWORD ybos_type;
   DWORD odb_type;
   INT tsize;
} id_map[] = {
   {
   A1_BKTYPE, TID_CHAR, 1}, {
   I1_BKTYPE, TID_BYTE, 1}, {
   I2_BKTYPE, TID_WORD, 2}, {
   I4_BKTYPE, TID_DWORD, 4}, {
   F4_BKTYPE, TID_FLOAT, 4}, {
   D8_BKTYPE, TID_DOUBLE, 8}, {
   0, 0}
};
#endif

extern EQUIPMENT equipment[];

EQUIPMENT *interrupt_eq = NULL;
EVENT_HEADER *interrupt_odb_buffer;
BOOL interrupt_odb_buffer_valid;

int send_event(INT index);
void send_all_periodic_events(INT transition);
void interrupt_routine(void);
void interrupt_enable(BOOL flag);
void display(BOOL bInit);

/*---- ODB records -------------------------------------------------*/

#define EQUIPMENT_COMMON_STR "\
Event ID = WORD : 0\n\
Trigger mask = WORD : 0\n\
Buffer = STRING : [32] SYSTEM\n\
Type = INT : 0\n\
Source = INT : 0\n\
Format = STRING : [8] FIXED\n\
Enabled = BOOL : 0\n\
Read on = INT : 0\n\
Period = INT : 0\n\
Event limit = DOUBLE : 0\n\
Num subevents = DWORD : 0\n\
Log history = INT : 0\n\
Frontend host = STRING : [32] \n\
Frontend name = STRING : [32] \n\
Frontend file name = STRING : [256] \n\
"

#define EQUIPMENT_STATISTICS_STR "\
Events sent = DOUBLE : 0\n\
Events per sec. = DOUBLE : 0\n\
kBytes per sec. = DOUBLE : 0\n\
"

/*-- transition callbacks ------------------------------------------*/

/*-- start ---------------------------------------------------------*/

#ifdef PART_OF_MUCAP
/*-- prestart ---------------------------------------------------------*/

INT tr_prestart(INT rn, char *error)
{
  INT status;
  status = pre_begin_of_run(rn, error);
  return status;
}
#endif

INT tr_start(INT rn, char *error)
{
   INT i, status;

   /* reset serial numbers */
   for (i = 0; equipment[i].name[0]; i++) {
      equipment[i].serial_number = 1;
      equipment[i].subevent_number = 0;
      equipment[i].stats.events_sent = 0;
      equipment[i].odb_in = equipment[i].odb_out = 0;
   }

   status = begin_of_run(rn, error);

   if (status == CM_SUCCESS) {
      run_state = STATE_RUNNING;
      run_number = rn;

      send_all_periodic_events(TR_START);

      if (display_period) {
         ss_printf(14, 2, "Running ");
         ss_printf(36, 2, "%d", rn);
      }

      /* enable interrupts */
      interrupt_enable(TRUE);
   }

   return status;
}

/*-- prestop -------------------------------------------------------*/

INT tr_stop(INT rn, char *error)
{
   INT status, i;
   EQUIPMENT *eq;

   /* disable interrupts */
   interrupt_enable(FALSE);

   status = end_of_run(rn, error);

   if (status == CM_SUCCESS) {
      /* don't send events if already stopped */
      if (run_state != STATE_STOPPED)
         send_all_periodic_events(TR_STOP);

      run_state = STATE_STOPPED;
      run_number = rn;

      if (display_period)
         ss_printf(14, 2, "Stopped ");
   } else
      interrupt_enable(TRUE);

   /* flush remaining buffered events */
   rpc_flush_event();
   for (i = 0; equipment[i].name[0]; i++)
      if (equipment[i].buffer_handle) {
         INT err = bm_flush_cache(equipment[i].buffer_handle, SYNC);
         if (err != BM_SUCCESS) {
            cm_msg(MERROR, "tr_prestop", "bm_flush_cache(SYNC) error %d", err);
            return err;
         }
      }

   /* update final statistics record in ODB */
   for (i = 0; equipment[i].name[0]; i++) {
      eq = &equipment[i];
      eq->stats.events_sent += eq->events_sent;
      eq->bytes_sent = 0;
      eq->events_sent = 0;
   }

   db_send_changed_records();

   return status;
}

/*-- pause ---------------------------------------------------------*/

INT tr_pause(INT rn, char *error)
{
   INT status;

   /* disable interrupts */
   interrupt_enable(FALSE);

   status = pause_run(rn, error);

   if (status == CM_SUCCESS) {
      run_state = STATE_PAUSED;
      run_number = rn;

      send_all_periodic_events(TR_PAUSE);

      if (display_period)
         ss_printf(14, 2, "Paused  ");
   } else
      interrupt_enable(TRUE);

   return status;
}

/*-- resume --------------------------------------------------------*/

INT tr_resume(INT rn, char *error)
{
   INT status;

   status = resume_run(rn, error);

   if (status == CM_SUCCESS) {
      run_state = STATE_RUNNING;
      run_number = rn;

      send_all_periodic_events(TR_RESUME);

      if (display_period)
         ss_printf(14, 2, "Running ");

      /* enable interrupts */
      interrupt_enable(TRUE);
   }

   return status;
}

/*------------------------------------------------------------------*/

INT manual_trigger(INT index, void *prpc_param[])
{
   manual_trigger_event_id = CWORD(0);
   return SUCCESS;
}

/*------------------------------------------------------------------*/

INT register_equipment(void)
{
   INT index, count, size, status, i, j, k, n;
   char str[256];
   EQUIPMENT_INFO *eq_info;
   EQUIPMENT_STATS *eq_stats;
   DWORD start_time, delta_time;
   HNDLE hKey;
   BOOL manual_trig_flag = FALSE;
   BANK_LIST *bank_list;
   DWORD dummy;

   /* get current ODB run state */
   size = sizeof(run_state);
   run_state = STATE_STOPPED;
   db_get_value(hDB, 0, "/Runinfo/State", &run_state, &size, TID_INT, TRUE);
   size = sizeof(run_number);
   run_number = 1;
   status =
       db_get_value(hDB, 0, "/Runinfo/Run number", &run_number, &size, TID_INT, TRUE);
   assert(status == SUCCESS);

   /* scan EQUIPMENT table from FRONTEND.C */
   for (index = 0; equipment[index].name[0]; index++) {
      eq_info = &equipment[index].info;
      eq_stats = &equipment[index].stats;

      if (eq_info->event_id == 0) {
         printf("\nEvent ID 0 for %s not allowed\n", equipment[index].name);
         cm_disconnect_experiment();
         ss_sleep(5000);
         exit(0);
      }

      /* init status */
      equipment[index].status = FE_SUCCESS;

      /* check for event builder event */
      if (eq_info->eq_type & EQ_EB) {

         if (frontend_index == -1) {
            printf("\nEquipment \"%s\" has EQ_EB set, but no", equipment[index].name); 
            printf(" index specified via \"-i\" flag.\nExiting.");
            cm_disconnect_experiment();
            ss_sleep(5000);
            exit(0);
         }

         /* modify equipment name to <name>xx where xx is the frontend index*/
         sprintf(equipment[index].name+strlen(equipment[index].name), "%02d", frontend_index);

         /* modify event buffer name to <name>xx where xx is the frontend index*/
         sprintf(eq_info->buffer+strlen(eq_info->buffer), "%02d", frontend_index);
      }

      sprintf(str, "/Equipment/%s/Common", equipment[index].name);

      /* get last event limit from ODB */
      if (eq_info->eq_type != EQ_SLOW) {
         db_find_key(hDB, 0, str, &hKey);
         size = sizeof(double);
         if (hKey)
            db_get_value(hDB, hKey, "Event limit", &eq_info->event_limit, &size,
                         TID_DOUBLE, TRUE);
      }

      /* Create common subtree */
      status = db_check_record(hDB, 0, str, EQUIPMENT_COMMON_STR, TRUE);
      if (status != DB_SUCCESS) {
         printf("Cannot check equipment record, status = %d\n", status);
         ss_sleep(3000);
      }
      db_find_key(hDB, 0, str, &hKey);

      if (equal_ustring(eq_info->format, "YBOS"))
         equipment[index].format = FORMAT_YBOS;
      else if (equal_ustring(eq_info->format, "FIXED"))
         equipment[index].format = FORMAT_FIXED;
      else                      /* default format is MIDAS */
         equipment[index].format = FORMAT_MIDAS;

      gethostname(eq_info->frontend_host, sizeof(eq_info->frontend_host));
      strcpy(eq_info->frontend_name, full_frontend_name);
      strcpy(eq_info->frontend_file_name, frontend_file_name);

      /* set record from equipment[] table in frontend.c */
      db_set_record(hDB, hKey, eq_info, sizeof(EQUIPMENT_INFO), 0);

      /* open hot link to equipment info */
      db_open_record(hDB, hKey, eq_info, sizeof(EQUIPMENT_INFO), MODE_READ, NULL, NULL);

    /*---- Create variables record ---------------------------------*/
      sprintf(str, "/Equipment/%s/Variables", equipment[index].name);
      if (equipment[index].event_descrip) {
         if (equipment[index].format == FORMAT_FIXED)
            db_check_record(hDB, 0, str, (char *) equipment[index].event_descrip, TRUE);
         else {
            /* create bank descriptions */
            bank_list = (BANK_LIST *) equipment[index].event_descrip;

            for (; bank_list->name[0]; bank_list++) {
               /* mabye needed later...
                  if (bank_list->output_flag == 0)
                  continue;
                */

               if (bank_list->type == TID_STRUCT) {
                  sprintf(str, "/Equipment/%s/Variables/%s", equipment[index].name,
                          bank_list->name);
                  status =
                      db_check_record(hDB, 0, str, strcomb(bank_list->init_str), TRUE);
                  if (status != DB_SUCCESS) {
                     printf("Cannot check/create record \"%s\", status = %d\n", str,
                            status);
                     ss_sleep(3000);
                  }
               } else {
                  sprintf(str, "/Equipment/%s/Variables/%s", equipment[index].name,
                          bank_list->name);
                  dummy = 0;
                  db_set_value(hDB, 0, str, &dummy, rpc_tid_size(bank_list->type), 1,
                               bank_list->type);
               }
            }
         }
      } else
         db_create_key(hDB, 0, str, TID_KEY);

      sprintf(str, "/Equipment/%s/Variables", equipment[index].name);
      db_find_key(hDB, 0, str, &hKey);
      equipment[index].hkey_variables = hKey;

      /*---- Create and initialize statistics tree -------------------*/
      
      sprintf(str, "/Equipment/%s/Statistics", equipment[index].name);

      status = db_check_record(hDB, 0, str, EQUIPMENT_STATISTICS_STR, TRUE);
      if (status != DB_SUCCESS) {
         printf("Cannot create/check statistics record, error %d\n", status);
         ss_sleep(3000);
      }

      status = db_find_key(hDB, 0, str, &hKey);
      if (status != DB_SUCCESS) {
         printf("Cannot find statistics record, error %d\n", status);
         ss_sleep(3000);
      }

      eq_stats->events_sent = 0;
      eq_stats->events_per_sec = 0;
      eq_stats->kbytes_per_sec = 0;

      /* open hot link to statistics tree */
      status =
          db_open_record(hDB, hKey, eq_stats, sizeof(EQUIPMENT_STATS), MODE_WRITE, NULL,
                         NULL);
      if (status != DB_SUCCESS) {
         cm_msg(MERROR, "register_equipment", 
             "Cannot open statistics record, error %d. Probably other FE is using it",
              status);
         ss_sleep(3000);
      }

      /*---- open event buffer ---------------------------------------*/

      if (eq_info->buffer[0]) {
         status =
             bm_open_buffer(eq_info->buffer, EVENT_BUFFER_SIZE,
                            &equipment[index].buffer_handle);
         if (status != BM_SUCCESS && status != BM_CREATED) {
            cm_msg(MERROR, "register_equipment",
                   "Cannot open event buffer. Try to reduce EVENT_BUFFER_SIZE in midas.h \
and rebuild the system.");
            return 0;
         }

         /* set the default buffer cache size */
         bm_set_cache_size(equipment[index].buffer_handle, 0, SERVER_CACHE_SIZE);
      } else
         equipment[index].buffer_handle = 0;

    /*---- evaluate polling count ----------------------------------*/
      if (eq_info->eq_type & EQ_POLLED) {
         if (display_period)
            printf("\nCalibrating");

         count = 1;
         do {
            if (display_period)
               printf(".");

            start_time = ss_millitime();

            poll_event(equipment[index].info.source, count, TRUE);

            delta_time = ss_millitime() - start_time;

            if (delta_time > 0)
               count = (INT) ((double) count * 100 / delta_time);
            else
               count *= 100;
         } while (delta_time > 120 || delta_time < 80);

         equipment[index].poll_count = (INT) ((double) eq_info->period / 100 * count);

         if (display_period)
            printf("OK\n");
      }

    /*---- initialize interrupt events -----------------------------*/
      if (eq_info->eq_type & EQ_INTERRUPT) {
         /* install interrupt for interrupt events */

         for (i = 0; equipment[i].name[0]; i++)
            if (equipment[i].info.eq_type & EQ_POLLED) {
               equipment[index].status = FE_ERR_DISABLED;
               cm_msg(MINFO, "register_equipment",
                      "Interrupt readout cannot be combined with polled readout");
            }

         if (equipment[index].status != FE_ERR_DISABLED) {
            if (eq_info->enabled) {
               if (interrupt_eq) {
                  equipment[index].status = FE_ERR_DISABLED;
                  cm_msg(MINFO, "register_equipment",
                         "Defined more than one equipment with interrupt readout");
               } else {
                  interrupt_configure(CMD_INTERRUPT_ATTACH, eq_info->source,
                                      (PTYPE) interrupt_routine);
                  interrupt_eq = &equipment[index];
                  interrupt_odb_buffer = malloc(MAX_EVENT_SIZE + sizeof(EVENT_HEADER));
               }
            } else {
               equipment[index].status = FE_ERR_DISABLED;
               cm_msg(MINFO, "register_equipment",
                      "Equipment %s disabled in file \"frontend.c\"",
                      equipment[index].name);
            }
         }
      }

    /*---- initialize slow control equipment -----------------------*/
      if (eq_info->eq_type & EQ_SLOW) {
         /* resolve duplicate device names */
         for (i = 0; equipment[index].driver[i].name[0]; i++)
            for (j = i + 1; equipment[index].driver[j].name[0]; j++)
               if (equal_ustring(equipment[index].driver[i].name,
                                 equipment[index].driver[j].name)) {
                  strcpy(str, equipment[index].driver[i].name);
                  for (k = 0, n = 0; equipment[index].driver[k].name[0]; k++)
                     if (equal_ustring(str, equipment[index].driver[k].name))
                        sprintf(equipment[index].driver[k].name, "%s_%d", str, n++);

                  break;
               }

         /* loop over equipment list and call class driver's init method */
         if (eq_info->enabled)
            equipment[index].status = equipment[index].cd(CMD_INIT, &equipment[index]);
         else {
            equipment[index].status = FE_ERR_DISABLED;
            cm_msg(MINFO, "register_equipment",
                   "Equipment %s disabled in file \"frontend.c\"", equipment[index].name);
         }

         /* let user read error messages */
         if (equipment[index].status != FE_SUCCESS)
            ss_sleep(3000);
      }

      /*---- register callback for manual triggered events -----------*/
      if (eq_info->eq_type & EQ_MANUAL_TRIG) {
         if (!manual_trig_flag)
            cm_register_function(RPC_MANUAL_TRIG, manual_trigger);

         manual_trig_flag = TRUE;
      }
   }

   return SUCCESS;
}

/*------------------------------------------------------------------*/

void update_odb(EVENT_HEADER * pevent, HNDLE hKey, INT format)
{
   INT size, i, ni4, tsize, status, n_data;
   void *pdata;
   char name[5];
   BANK_HEADER *pbh;
   BANK *pbk;
   BANK32 *pbk32;
   void *pydata;
   DWORD odb_type;
   DWORD *pyevt, bkname;
   WORD bktype;
   HNDLE hKeyRoot, hKeyl;
   KEY key;

   /* outcommented sind db_find_key does not work in FTCP mode, SR 25.4.03
      rpc_set_option(-1, RPC_OTRANSPORT, RPC_FTCP); */

   if (format == FORMAT_FIXED) {
      if (db_set_record(hDB, hKey, (char *) (pevent + 1),
                        pevent->data_size, 0) != DB_SUCCESS)
         cm_msg(MERROR, "update_odb", "event #%d size mismatch", pevent->event_id);
   } else if (format == FORMAT_MIDAS) {
      pbh = (BANK_HEADER *) (pevent + 1);
      pbk = NULL;
      pbk32 = NULL;
      do {
         /* scan all banks */
         if (bk_is32(pbh)) {
            size = bk_iterate32(pbh, &pbk32, &pdata);
            if (pbk32 == NULL)
               break;
            bkname = *((DWORD *) pbk32->name);
            bktype = (WORD) pbk32->type;
         } else {
            size = bk_iterate(pbh, &pbk, &pdata);
            if (pbk == NULL)
               break;
            bkname = *((DWORD *) pbk->name);
            bktype = (WORD) pbk->type;
         }

         n_data = size;
         if (rpc_tid_size(bktype & 0xFF))
            n_data /= rpc_tid_size(bktype & 0xFF);

         /* get bank key */
         *((DWORD *) name) = bkname;
         name[4] = 0;

         if (bktype == TID_STRUCT) {
            status = db_find_key(hDB, hKey, name, &hKeyRoot);
            if (status != DB_SUCCESS) {
               cm_msg(MERROR, "update_odb",
                      "please define bank %s in BANK_LIST in frontend.c", name);
               continue;
            }

            /* write structured bank */
            for (i = 0;; i++) {
               status = db_enum_key(hDB, hKeyRoot, i, &hKeyl);
               if (status == DB_NO_MORE_SUBKEYS)
                  break;

               db_get_key(hDB, hKeyl, &key);

               /* adjust for alignment */
               if (key.type != TID_STRING && key.type != TID_LINK)
                  pdata =
                      (void *) VALIGN(pdata, MIN(ss_get_struct_align(), key.item_size));

               status = db_set_data(hDB, hKeyl, pdata, key.item_size * key.num_values,
                                    key.num_values, key.type);
               if (status != DB_SUCCESS) {
                  cm_msg(MERROR, "update_odb", "cannot write %s to ODB", name);
                  continue;
               }

               /* shift data pointer to next item */
               pdata = (char *) pdata + key.item_size * key.num_values;
            }
         } else {
            /* write variable length bank  */
            if (n_data > 0)
               db_set_value(hDB, hKey, name, pdata, size, n_data, bktype & 0xFF);
         }

      } while (1);
   } else if (format == FORMAT_YBOS) {
#ifdef YBOS_SUPPORT
      YBOS_BANK_HEADER *pybkh;

      /* skip the lrl (4 bytes per event) */
      pyevt = (DWORD *) (pevent + 1);
      pybkh = NULL;
      do {
         /* scan all banks */
         ni4 = ybk_iterate(pyevt, &pybkh, &pydata);
         if (pybkh == NULL || ni4 == 0)
            break;

         /* find the corresponding odb type */
         tsize = odb_type = 0;
         for (i = 0; id_map[0].ybos_type > 0; i++) {
            if (pybkh->type == id_map[i].ybos_type) {
               odb_type = id_map[i].odb_type;
               tsize = id_map[i].tsize;
               break;
            }
         }

         /* extract bank name (key name) */
         *((DWORD *) name) = pybkh->name;
         name[4] = 0;

         /* reject EVID bank */
         if (strncmp(name, "EVID", 4) == 0)
            continue;

         /* correct YBS number of entries */
         if (pybkh->type == D8_BKTYPE)
            ni4 /= 2;
         if (pybkh->type == I2_BKTYPE)
            ni4 *= 2;
         if (pybkh->type == I1_BKTYPE || pybkh->type == A1_BKTYPE)
            ni4 *= 4;

         /* write bank to ODB, ni4 always in I*4 */
         size = ni4 * tsize;
         if ((status =
              db_set_value(hDB, hKey, name, pydata, size, ni4,
                           odb_type & 0xFF)) != DB_SUCCESS) {
            printf("status:%i odb_type:%li name:%s ni4:%i size:%i tsize:%i\n", status,
                   odb_type, name, ni4, size, tsize);
            for (i = 0; i < 6; i++) {
               float * fpydata = (float *) pydata;
               printf("data: %f\n", *(fpydata)++);
            }
         }
      } while (1);
#endif                          /* YBOS_SUPPORT */
   }

   rpc_set_option(-1, RPC_OTRANSPORT, RPC_TCP);
}

/*------------------------------------------------------------------*/

int send_event(INT index)
{
   EQUIPMENT_INFO *eq_info;
   EVENT_HEADER *pevent, *pfragment;
   char *pdata;
   unsigned char *pd;
   INT i, status;
   DWORD sent, size;
   static void *frag_buffer = NULL;

   eq_info = &equipment[index].info;

   /* check for fragmented event */
   if (eq_info->eq_type & EQ_FRAGMENTED) {
      if (frag_buffer == NULL)
         frag_buffer = malloc(max_event_size_frag);

      if (frag_buffer == NULL) {
         cm_msg(MERROR, "send_event",
                "Not enough memory to allocate buffer for fragmented events");
         return SS_NO_MEMORY;
      }

      pevent = frag_buffer;
   } else {
      /* return value should be valid pointer. if NULL BIG error ==> abort  */
      pevent = dm_pointer_get();
      if (pevent == NULL) {
         cm_msg(MERROR, "send_event", "dm_pointer_get not returning valid pointer");
         return SS_NO_MEMORY;
      }
   }

   /* compose MIDAS event header */
   pevent->event_id = eq_info->event_id;
   pevent->trigger_mask = eq_info->trigger_mask;
   pevent->data_size = 0;
   pevent->time_stamp = ss_time();
   pevent->serial_number = equipment[index].serial_number++;

   equipment[index].last_called = ss_millitime();

   /* call user readout routine */
   *((EQUIPMENT **) (pevent + 1)) = &equipment[index];
   pevent->data_size = equipment[index].readout((char *) (pevent + 1), 0);

   /* send event */
   if (pevent->data_size) {
      if (eq_info->eq_type & EQ_FRAGMENTED) {
         /* fragment event */
         if (pevent->data_size + sizeof(EVENT_HEADER) > (DWORD) max_event_size_frag) {
            cm_msg(MERROR, "send_event",
                   "Event size %ld larger than maximum size %d for frag. ev.",
                   (long) (pevent->data_size + sizeof(EVENT_HEADER)),
                   max_event_size_frag);
            return SS_NO_MEMORY;
         }

         /* compose fragments */
         pfragment = dm_pointer_get();
         if (pfragment == NULL) {
            cm_msg(MERROR, "send_event", "dm_pointer_get not returning valid pointer");
            return SS_NO_MEMORY;
         }

         /* compose MIDAS event header */
         memcpy(pfragment, pevent, sizeof(EVENT_HEADER));
         pfragment->event_id |= EVENTID_FRAG1;

         /* store total event size */
         pd = (char *) (pfragment + 1);
         size = pevent->data_size;
         for (i = 0; i < 4; i++) {
            pd[i] = (unsigned char) (size & 0xFF);      /* little endian, please! */
            size >>= 8;
         }

         pfragment->data_size = sizeof(DWORD);

         pdata = (char *) (pevent + 1);

         for (i = 0, sent = 0; sent < pevent->data_size; i++) {
            if (i > 0) {
               pfragment = dm_pointer_get();

               /* compose MIDAS event header */
               memcpy(pfragment, pevent, sizeof(EVENT_HEADER));
               pfragment->event_id |= EVENTID_FRAG;

               /* copy portion of event */
               size = pevent->data_size - sent;
               if (size > max_event_size - sizeof(EVENT_HEADER))
                  size = max_event_size - sizeof(EVENT_HEADER);

               memcpy(pfragment + 1, pdata, size);
               pfragment->data_size = size;
               sent += size;
               pdata += size;
            }

            /* send event to buffer */
            if (equipment[index].buffer_handle) {
#ifdef USE_EVENT_CHANNEL
               dm_pointer_increment(equipment[index].buffer_handle,
                                    pfragment->data_size + sizeof(EVENT_HEADER));
#else
               rpc_flush_event();
               status = bm_send_event(equipment[index].buffer_handle, pfragment,
                                      pfragment->data_size + sizeof(EVENT_HEADER), SYNC);
               if (status != BM_SUCCESS) {
                  cm_msg(MERROR, "send_event", "bm_send_event(SYNC) error %d", status);
                  return status;
               }
#endif
            }
         }

         if (equipment[index].buffer_handle) {
#ifndef USE_EVENT_CHANNEL
            status = bm_flush_cache(equipment[index].buffer_handle, SYNC);
            if (status != BM_SUCCESS) {
               cm_msg(MERROR, "send_event", "bm_flush_cache(SYNC) error %d", status);
               return status;
            }
#endif
         }
      } else {
         /* send unfragmented event */

         if (pevent->data_size + sizeof(EVENT_HEADER) > (DWORD) max_event_size) {
            cm_msg(MERROR, "send_event", "Event size %ld larger than maximum size %d",
                   (long) (pevent->data_size + sizeof(EVENT_HEADER)), max_event_size);
            return SS_NO_MEMORY;
         }

         /* send event to buffer */
         if (equipment[index].buffer_handle) {
#ifdef USE_EVENT_CHANNEL
            dm_pointer_increment(equipment[index].buffer_handle,
                                 pevent->data_size + sizeof(EVENT_HEADER));
#else
            rpc_flush_event();
            status = bm_send_event(equipment[index].buffer_handle, pevent,
                                   pevent->data_size + sizeof(EVENT_HEADER), SYNC);
            if (status != BM_SUCCESS) {
               cm_msg(MERROR, "send_event", "bm_send_event(SYNC) error %d", status);
               return status;
            }
            status = bm_flush_cache(equipment[index].buffer_handle, SYNC);
            if (status != BM_SUCCESS) {
               cm_msg(MERROR, "send_event", "bm_flush_cache(SYNC) error %d", status);
               return status;
            }
#endif
         }

         /* send event to ODB if RO_ODB flag is set or history is on. Do not
            send SLOW events since the class driver does that */
         if ((eq_info->read_on & RO_ODB) ||
             (eq_info->history > 0 && (eq_info->eq_type & ~EQ_SLOW))) {
            update_odb(pevent, equipment[index].hkey_variables, equipment[index].format);
            equipment[index].odb_out++;
         }
      }

      equipment[index].bytes_sent += pevent->data_size + sizeof(EVENT_HEADER);
      equipment[index].events_sent++;

      equipment[index].stats.events_sent += equipment[index].events_sent;
      equipment[index].events_sent = 0;
   } else
      equipment[index].serial_number--;

   /* emtpy event buffer */
#ifdef USE_EVENT_CHANNEL
   if ((status = dm_area_flush()) != CM_SUCCESS)
      cm_msg(MERROR, "send_event", "dm_area_flush: %i", status);
#endif

   for (i = 0; equipment[i].name[0]; i++)
      if (equipment[i].buffer_handle) {
         status = bm_flush_cache(equipment[i].buffer_handle, SYNC);
         if (status != BM_SUCCESS) {
            cm_msg(MERROR, "send_event", "bm_flush_cache(SYNC) error %d", status);
            return status;
         }
      }

   return CM_SUCCESS;
}

/*------------------------------------------------------------------*/

void send_all_periodic_events(INT transition)
{
   EQUIPMENT_INFO *eq_info;
   INT i;

   for (i = 0; equipment[i].name[0]; i++) {
      eq_info = &equipment[i].info;

      if (!eq_info->enabled || equipment[i].status != FE_SUCCESS)
         continue;

      if (transition == TR_START && (eq_info->read_on & RO_BOR) == 0)
         continue;
      if (transition == TR_STOP && (eq_info->read_on & RO_EOR) == 0)
         continue;
      if (transition == TR_PAUSE && (eq_info->read_on & RO_PAUSE) == 0)
         continue;
      if (transition == TR_RESUME && (eq_info->read_on & RO_RESUME) == 0)
         continue;

      send_event(i);
   }
}

/*------------------------------------------------------------------*/

BOOL interrupt_enabled;

void interrupt_enable(BOOL flag)
{
   interrupt_enabled = flag;

   if (interrupt_eq) {
      if (interrupt_enabled)
         interrupt_configure(CMD_INTERRUPT_ENABLE, 0, 0);
      else
         interrupt_configure(CMD_INTERRUPT_DISABLE, 0, 0);
   }
}

/*------------------------------------------------------------------*/

void interrupt_routine(void)
{
   EVENT_HEADER *pevent;

   /* get pointer for upcoming event.
      This is a blocking call if no space available */
   if ((pevent = dm_pointer_get()) == NULL)
      cm_msg(MERROR, "interrupt_routine", "interrupt, dm_pointer_get returned NULL");

   /* compose MIDAS event header */
   pevent->event_id = interrupt_eq->info.event_id;
   pevent->trigger_mask = interrupt_eq->info.trigger_mask;
   pevent->data_size = 0;
   pevent->time_stamp = actual_time;
   pevent->serial_number = interrupt_eq->serial_number++;

   /* call user readout routine */
   pevent->data_size = interrupt_eq->readout((char *) (pevent + 1), 0);

   /* send event */
   if (pevent->data_size) {
      interrupt_eq->bytes_sent += pevent->data_size + sizeof(EVENT_HEADER);
      interrupt_eq->events_sent++;

      if (interrupt_eq->buffer_handle) {
#ifdef USE_EVENT_CHANNEL
         dm_pointer_increment(interrupt_eq->buffer_handle,
                              pevent->data_size + sizeof(EVENT_HEADER));
#else
         rpc_send_event(interrupt_eq->buffer_handle, pevent,
                        pevent->data_size + sizeof(EVENT_HEADER), SYNC);
#endif
      }

      /* send event to ODB */
      if (interrupt_eq->info.read_on & RO_ODB || interrupt_eq->info.history) {
         if (actual_millitime - interrupt_eq->last_called > ODB_UPDATE_TIME) {
            interrupt_eq->last_called = actual_millitime;
            memcpy(interrupt_odb_buffer, pevent,
                   pevent->data_size + sizeof(EVENT_HEADER));
            interrupt_odb_buffer_valid = TRUE;
            interrupt_eq->odb_out++;
         }
      }
   } else
      interrupt_eq->serial_number--;

}

/*------------------------------------------------------------------*/

int message_print(const char *msg)
{
   char str[160];

   memset(str, ' ', 159);
   str[159] = 0;

   if (msg[0] == '[')
      msg = strchr(msg, ']') + 2;

   memcpy(str, msg, strlen(msg));
   ss_printf(0, 20, str);

   return 0;
}

/*------------------------------------------------------------------*/

void display(BOOL bInit)
{
   INT i, status;
   time_t full_time;
   char str[30];

   if (bInit) {
      ss_clear_screen();

      if (host_name[0])
         strcpy(str, host_name);
      else
         strcpy(str, "<local>");

      ss_printf(0, 0, "%s connected to %s. Press \"!\" to exit", full_frontend_name, str);
      ss_printf(0, 1,
                "================================================================================");
      ss_printf(0, 2, "Run status:   %s",
                run_state == STATE_STOPPED ? "Stopped" : run_state ==
                STATE_RUNNING ? "Running" : "Paused");
      ss_printf(25, 2, "Run number %d   ", run_number);
      ss_printf(0, 3,
                "================================================================================");
      ss_printf(0, 4,
                "Equipment     Status     Events     Events/sec Rate[kB/s] ODB->FE    FE->ODB");
      ss_printf(0, 5,
                "--------------------------------------------------------------------------------");
      for (i = 0; equipment[i].name[0]; i++)
         ss_printf(0, i + 6, "%s", equipment[i].name);
   }

   /* display time */
   time(&full_time);
   strcpy(str, ctime(&full_time) + 11);
   str[8] = 0;
   ss_printf(72, 0, "%s", str);

   for (i = 0; equipment[i].name[0]; i++) {
      status = equipment[i].status;

      if ((status == 0 || status == FE_SUCCESS) && equipment[i].info.enabled)
         ss_printf(14, i + 6, "OK       ");
      else if (!equipment[i].info.enabled)
         ss_printf(14, i + 6, "Disabled ");
      else if (status == FE_ERR_ODB)
         ss_printf(14, i + 6, "ODB Error");
      else if (status == FE_ERR_HW)
         ss_printf(14, i + 6, "HW Error ");
      else if (status == FE_ERR_DISABLED)
         ss_printf(14, i + 6, "Disabled ");
      else
         ss_printf(14, i + 6, "Unknown  ");

      if (equipment[i].stats.events_sent > 1E9)
         ss_printf(25, i + 6, "%1.3lfG     ", equipment[i].stats.events_sent / 1E9);
      else if (equipment[i].stats.events_sent > 1E6)
         ss_printf(25, i + 6, "%1.3lfM     ", equipment[i].stats.events_sent / 1E6);
      else
         ss_printf(25, i + 6, "%1.0lf      ", equipment[i].stats.events_sent);
      ss_printf(36, i + 6, "%1.1lf      ", equipment[i].stats.events_per_sec);
      ss_printf(47, i + 6, "%1.1lf      ", equipment[i].stats.kbytes_per_sec);
      ss_printf(58, i + 6, "%ld       ", equipment[i].odb_in);
      ss_printf(69, i + 6, "%ld       ", equipment[i].odb_out);
   }

   /* go to next line */
   ss_printf(0, i + 6, "");
}

/*------------------------------------------------------------------*/

BOOL logger_root()
/* check if logger uses ROOT format */
{
   int size, i, status;
   char str[80];
   HNDLE hKeyRoot, hKey;

   if (db_find_key(hDB, 0, "/Logger/Channels", &hKeyRoot) == DB_SUCCESS) {
      for (i = 0;; i++) {
         status = db_enum_key(hDB, hKeyRoot, i, &hKey);
         if (status == DB_NO_MORE_SUBKEYS)
            break;

         strcpy(str, "MIDAS");
         size = sizeof(str);
         db_get_value(hDB, hKey, "Settings/Format", str, &size, TID_STRING, TRUE);

         if (equal_ustring(str, "ROOT"))
            return TRUE;
      }
   }

   return FALSE;
}

/*------------------------------------------------------------------*/

INT scheduler(void)
{
   EQUIPMENT_INFO *eq_info;
   EQUIPMENT *eq;
   EVENT_HEADER *pevent;
   DWORD last_time_network = 0, last_time_display = 0, last_time_flush = 0, readout_start;
   INT i, j, index, status, ch, source, size, state;
   char str[80];
   BOOL buffer_done, flag, force_update = FALSE;

   INT opt_max = 0, opt_index = 0, opt_tcp_size = 128, opt_cnt = 0;
   INT err;

#ifdef OS_VXWORKS
   rpc_set_opt_tcp_size(1024);
#ifdef PPCxxx
   rpc_set_opt_tcp_size(NET_TCP_SIZE);
#endif
#endif

   /*----------------- MAIN equipment loop ------------------------------*/

   do {
      actual_millitime = ss_millitime();
      actual_time = ss_time();

      /*---- loop over equipment table -------------------------------*/
      for (index = 0;; index++) {
         eq = &equipment[index];
         eq_info = &eq->info;

         /* check if end of equipment list */
         if (!eq->name[0])
            break;

         if (!eq_info->enabled)
            continue;

         if (eq->status != FE_SUCCESS)
            continue;

         /*---- call idle routine for slow control equipment ----*/
         if ((eq_info->eq_type & EQ_SLOW) && eq->status == FE_SUCCESS) {
            if (eq_info->event_limit > 0 && run_state == STATE_RUNNING) {
               if (actual_time - eq->last_idle >= (DWORD) eq_info->event_limit) {
                  eq->cd(CMD_IDLE, eq);
                  eq->last_idle = actual_time;
               }
            } else
               eq->cd(CMD_IDLE, eq);
         }

         if (run_state == STATE_STOPPED && (eq_info->read_on & RO_STOPPED) == 0)
            continue;
         if (run_state == STATE_PAUSED && (eq_info->read_on & RO_PAUSED) == 0)
            continue;
         if (run_state == STATE_RUNNING && (eq_info->read_on & RO_RUNNING) == 0)
            continue;

         /*---- check periodic events ----*/
         if ((eq_info->eq_type & EQ_PERIODIC) || (eq_info->eq_type & EQ_SLOW)) {
            if (eq_info->period == 0)
               continue;

            /* check if period over */
            if (actual_millitime - eq->last_called >= (DWORD) eq_info->period) {
               /* disable interrupts during readout */
               interrupt_enable(FALSE);

               /* readout and send event */
               status = send_event(index);

               if (status != CM_SUCCESS) {
                  cm_msg(MERROR, "scheduler", "send_event error %d", status);
                  goto net_error;
               }

               /* re-enable the interrupt after periodic */
               interrupt_enable(TRUE);
            }
         }

         /* end of periodic equipments */
         /*---- check polled events ----*/
         if (eq_info->eq_type & EQ_POLLED) {
            readout_start = actual_millitime;
            pevent = NULL;

            while ((source = poll_event(eq_info->source, eq->poll_count, FALSE)) > 0) {
               pevent = dm_pointer_get();
               if (pevent == NULL) {
                  cm_msg(MERROR, "scheduler",
                         "polled, dm_pointer_get not returning valid pointer");
                  status = SS_NO_MEMORY;
                  goto net_error;
               }

               /* compose MIDAS event header */
               pevent->event_id = eq_info->event_id;
               pevent->trigger_mask = eq_info->trigger_mask;
               pevent->data_size = 0;
               pevent->time_stamp = actual_time;
               pevent->serial_number = eq->serial_number;

               /* put source at beginning of event, will be overwritten by 
                  user readout code, just a special feature used by some 
                  multi-source applications */
               *(INT *) (pevent + 1) = source;

               if (eq->info.num_subevents) {
                  eq->subevent_number = 0;
                  do {
                     *(INT *) ((char *) (pevent + 1) + pevent->data_size) = source;

                     /* call user readout routine for subevent indicating offset */
                     size = eq->readout((char *) (pevent + 1), pevent->data_size);
                     pevent->data_size += size;
                     if (size > 0) {
                        if (pevent->data_size + sizeof(EVENT_HEADER) >
                            (DWORD) max_event_size) {
                           cm_msg(MERROR, "scheduler",
                                  "Event size %ld larger than maximum size %d",
                                  (long) (pevent->data_size + sizeof(EVENT_HEADER)),
                                  max_event_size);
                        }

                        eq->subevent_number++;
                        eq->serial_number++;
                     }

                     /* wait for next event */
                     do {
                        source = poll_event(eq_info->source, eq->poll_count, FALSE);

                        if (source == FALSE) {
                           actual_millitime = ss_millitime();

                           /* repeat no more than period */
                           if (actual_millitime - readout_start > (DWORD) eq_info->period)
                              break;
                        }
                     } while (source == FALSE);

                  } while (eq->subevent_number < eq->info.num_subevents && source);

                  /* notify readout routine about end of super-event */
                  pevent->data_size = eq->readout((char *) (pevent + 1), -1);
               } else {
                  /* call user readout routine indicating event source */
                  pevent->data_size = eq->readout((char *) (pevent + 1), 0);

                  /* check event size */
                  if (pevent->data_size + sizeof(EVENT_HEADER) > (DWORD) max_event_size) {
                     cm_msg(MERROR, "scheduler",
                            "Event size %ld larger than maximum size %d",
                            (long) (pevent->data_size + sizeof(EVENT_HEADER)),
                            max_event_size);
                  }

                  /* increment serial number if event read out sucessfully */
                  if (pevent->data_size)
                     eq->serial_number++;
               }

               /* send event */
               if (pevent->data_size) {
                  if (eq->buffer_handle) {
                     /* send first event to ODB if logger writes in root format */
                     if (pevent->serial_number == 1)
                        if (logger_root())
                           update_odb(pevent, eq->hkey_variables, eq->format);

#ifdef USE_EVENT_CHANNEL
                     dm_pointer_increment(eq->buffer_handle,
                                          pevent->data_size + sizeof(EVENT_HEADER));
#else
                     status = rpc_send_event(eq->buffer_handle, pevent,
                                             pevent->data_size + sizeof(EVENT_HEADER),
                                             SYNC);

                     if (status != SUCCESS) {
                        cm_msg(MERROR, "scheduler", "rpc_send_event error %d", status);
                        goto net_error;
                     }
#endif

                     eq->bytes_sent += pevent->data_size + sizeof(EVENT_HEADER);

                     if (eq->info.num_subevents)
                        eq->events_sent += eq->subevent_number;
                     else
                        eq->events_sent++;
                  }
               }

               actual_millitime = ss_millitime();

               /* repeat no more than period */
               if (actual_millitime - readout_start > (DWORD) eq_info->period)
                  break;

               /* quit if event limit is reached */
               if (eq_info->event_limit > 0 &&
                   eq->stats.events_sent + eq->events_sent >= eq_info->event_limit)
                  break;
            }

            /* send event to ODB */
            if (pevent && (eq_info->read_on & RO_ODB || eq_info->history)) {
               if (actual_millitime - eq->last_called > ODB_UPDATE_TIME && pevent != NULL) {
                  eq->last_called = actual_millitime;
                  update_odb(pevent, eq->hkey_variables, eq->format);
                  eq->odb_out++;
               }
            }
         }

         /*---- send interrupt events ----*/
         if (eq_info->eq_type & EQ_INTERRUPT) {
            /* not much to do as work being done independently in interrupt_routine() */

            /* update ODB */
            if (interrupt_odb_buffer_valid) {
               update_odb(interrupt_odb_buffer, interrupt_eq->hkey_variables,
                          interrupt_eq->format);
               interrupt_odb_buffer_valid = FALSE;
            }

         }

         /*---- check if event limit is reached ----*/
         if (eq_info->eq_type != EQ_SLOW &&
             eq_info->event_limit > 0 &&
             eq->stats.events_sent + eq->events_sent >= eq_info->event_limit &&
             run_state == STATE_RUNNING) {
            /* stop run */
            if (cm_transition(TR_STOP, 0, str, sizeof(str), SYNC, FALSE) != CM_SUCCESS)
               cm_msg(MERROR, "scheduler", "cannot stop run: %s", str);

            /* check if autorestart, main loop will take care of it */
            size = sizeof(BOOL);
            flag = FALSE;
            db_get_value(hDB, 0, "/Logger/Auto restart", &flag, &size, TID_BOOL, TRUE);

            if (flag)
               auto_restart = ss_time() + 20;   /* restart in 20 sec. */

            /* update event display correctly */
            force_update = TRUE;
         }
      }

      /*---- call frontend_loop periodically -------------------------*/
      if (frontend_call_loop) {
         status = frontend_loop();
         if (status != CM_SUCCESS)
            status = RPC_SHUTDOWN;
      }

      /*---- check for deferred transitions --------------------------*/
      cm_check_deferred_transition();

      /*---- check for manual triggered events -----------------------*/
      if (manual_trigger_event_id) {
         interrupt_enable(FALSE);

         /* readout and send event */
         status = BM_INVALID_PARAM; 
         for (i = 0; equipment[i].name[0]; i++)
            if (equipment[i].info.event_id == manual_trigger_event_id) {
               status = send_event(i);
               break;
            }

         manual_trigger_event_id = 0;

         if (status != CM_SUCCESS) {
            cm_msg(MERROR, "scheduler", "send_event error %d", status);
            goto net_error;
         }

         /* re-enable the interrupt after periodic */
         interrupt_enable(TRUE);
      }

      /*---- calculate rates and update status page periodically -----*/
      if (force_update ||
          (display_period
           && actual_millitime - last_time_display > (DWORD) display_period)
          || (!display_period && actual_millitime - last_time_display > 3000)) {
         force_update = FALSE;

         /* calculate rates */
         if (actual_millitime != last_time_display) {
            max_bytes_per_sec = 0;
            for (i = 0; equipment[i].name[0]; i++) {
               eq = &equipment[i];
               eq->stats.events_sent += eq->events_sent;
               eq->stats.events_per_sec =
                   eq->events_sent / ((actual_millitime - last_time_display) / 1000.0);
               eq->stats.kbytes_per_sec =
                   eq->bytes_sent / 1024.0 / ((actual_millitime - last_time_display) /
                                              1000.0);

               if ((INT) eq->bytes_sent > max_bytes_per_sec)
                  max_bytes_per_sec = eq->bytes_sent;

               eq->bytes_sent = 0;
               eq->events_sent = 0;
            }

            max_bytes_per_sec = (DWORD)
                ((double) max_bytes_per_sec /
                 ((actual_millitime - last_time_display) / 1000.0));

            /* tcp buffer size evaluation */
            if (optimize) {
               opt_max = MAX(opt_max, (INT) max_bytes_per_sec);
               ss_printf(0, opt_index, "%6d : %5.1lf %5.1lf", opt_tcp_size,
                         opt_max / 1024.0, max_bytes_per_sec / 1024.0);
               if (++opt_cnt == 10) {
                  opt_cnt = 0;
                  opt_max = 0;
                  opt_index++;
                  opt_tcp_size = 1 << (opt_index + 7);
                  rpc_set_opt_tcp_size(opt_tcp_size);
                  if (1 << (opt_index + 7) > 0x8000) {
                     opt_index = 0;
                     opt_tcp_size = 1 << 7;
                     rpc_set_opt_tcp_size(opt_tcp_size);
                  }
               }
            }

         }

         /* propagate changes in equipment to ODB */
         rpc_set_option(-1, RPC_OTRANSPORT, RPC_FTCP);
         db_send_changed_records();
         rpc_set_option(-1, RPC_OTRANSPORT, RPC_TCP);

         if (display_period) {
            display(FALSE);

            /* check keyboard */
            ch = 0;
            status = 0;
            while (ss_kbhit()) {
               ch = ss_getchar(0);
               if (ch == -1)
                  ch = getchar();

               if (ch == '!')
                  status = RPC_SHUTDOWN;
            }

            if (ch > 0)
               display(TRUE);
            if (status == RPC_SHUTDOWN)
               break;
         }

         last_time_display = actual_millitime;
      }

      /*---- check to flush cache ------------------------------------*/
      if (actual_millitime - last_time_flush > 1000) {
         last_time_flush = actual_millitime;

         /* if cache on server is not filled in one second at current
            data rate, flush it now to make events available to consumers */

         if (max_bytes_per_sec < SERVER_CACHE_SIZE) {
            interrupt_enable(FALSE);

#ifdef USE_EVENT_CHANNEL
            if ((status = dm_area_flush()) != CM_SUCCESS)
               cm_msg(MERROR, "scheduler", "dm_area_flush: %i", status);
#endif

            for (i = 0; equipment[i].name[0]; i++) {
               if (equipment[i].buffer_handle) {
                  /* check if buffer already flushed */
                  buffer_done = FALSE;
                  for (j = 0; j < i; j++)
                     if (equipment[i].buffer_handle == equipment[j].buffer_handle) {
                        buffer_done = TRUE;
                        break;
                     }

                  if (!buffer_done) {
                     rpc_set_option(-1, RPC_OTRANSPORT, RPC_FTCP);
                     rpc_flush_event();
                     err = bm_flush_cache(equipment[i].buffer_handle, ASYNC);
                     if (err != BM_SUCCESS) {
                        cm_msg(MERROR, "scheduler", "bm_flush_cache(ASYNC) error %d",
                               err);
                        return err;
                     }
                     rpc_set_option(-1, RPC_OTRANSPORT, RPC_TCP);
                  }
               }
            }
            interrupt_enable(TRUE);
         }
      }

      /*---- check for auto restart --------------------------------*/
      if (auto_restart > 0 && ss_time() > auto_restart) {
         /* check if really stopped */
         size = sizeof(state);
         status = db_get_value(hDB, 0, "Runinfo/State", &state, &size, TID_INT, TRUE);
         if (status != DB_SUCCESS)
            cm_msg(MERROR, "scheduler", "cannot get Runinfo/State in database");

         if (state == STATE_STOPPED) {
            auto_restart = 0;
            size = sizeof(run_number);
            status =
                db_get_value(hDB, 0, "/Runinfo/Run number", &run_number, &size, TID_INT,
                             TRUE);
            assert(status == SUCCESS);

            if (run_number <= 0) {
               cm_msg(MERROR, "main", "aborting on attempt to use invalid run number %d",
                      run_number);
               abort();
            }

            cm_msg(MTALK, "main", "starting new run");
            status = cm_transition(TR_START, run_number + 1, NULL, 0, SYNC, FALSE);
            if (status != CM_SUCCESS)
               cm_msg(MERROR, "main", "cannot restart run");
         }
      }

      /*---- check network messages ----------------------------------*/
      if (run_state == STATE_RUNNING && interrupt_eq == NULL) {
         /* only call yield once every 100ms when running */
         if (actual_millitime - last_time_network > 100) {
            status = cm_yield(0);
            last_time_network = actual_millitime;
         } else
            status = RPC_SUCCESS;
      } else
         /* when run is stopped or interrupts used, 
            call yield with 100ms timeout */
         status = cm_yield(100);

      /* exit for VxWorks */
      if (fe_stop)
         status = RPC_SHUTDOWN;

   } while (status != RPC_SHUTDOWN && status != SS_ABORT);

 net_error:

   return status;
}

/*------------------------------------------------------------------*/

#ifdef HAVE_CAMAC

INT cnaf_callback(INT index, void *prpc_param[])
{
   DWORD cmd, b, c, n, a, f, *pdword, *size, *x, *q, dtemp;
   WORD *pword, *pdata, temp;
   INT i, count;

   /* Decode parameters */
   cmd = CDWORD(0);
   b = CDWORD(1);
   c = CDWORD(2);
   n = CDWORD(3);
   a = CDWORD(4);
   f = CDWORD(5);
   pdword = CPDWORD(6);
   pword = CPWORD(6);
   pdata = CPWORD(6);
   size = CPDWORD(7);
   x = CPDWORD(8);
   q = CPDWORD(9);

   /* determine repeat count */
   if (index == RPC_CNAF16)
      count = *size / sizeof(WORD);     /* 16 bit */
   else
      count = *size / sizeof(DWORD);    /* 24 bit */

   switch (cmd) {
    /*---- special commands ----*/

   case CNAF_INHIBIT_SET:
      cam_inhibit_set(c);
      break;
   case CNAF_INHIBIT_CLEAR:
      cam_inhibit_clear(c);
      break;
   case CNAF_CRATE_CLEAR:
      cam_crate_clear(c);
      break;
   case CNAF_CRATE_ZINIT:
      cam_crate_zinit(c);
      break;

   case CNAF_TEST:
      break;

   case CNAF:
      if (index == RPC_CNAF16) {
         for (i = 0; i < count; i++)
            if (f < 16)
               cam16i_q(c, n, a, f, pword++, (int *) x, (int *) q);
            else if (f < 24)
               cam16o_q(c, n, a, f, pword[i], (int *) x, (int *) q);
            else
               cam16i_q(c, n, a, f, &temp, (int *) x, (int *) q);
      } else {
         for (i = 0; i < count; i++)
            if (f < 16)
               cam24i_q(c, n, a, f, pdword++, (int *) x, (int *) q);
            else if (f < 24)
               cam24o_q(c, n, a, f, pdword[i], (int *) x, (int *) q);
            else
               cam24i_q(c, n, a, f, &dtemp, (int *) x, (int *) q);
      }

      break;

   case CNAF_nQ:
      if (index == RPC_CNAF16) {
         if (f < 16)
            cam16i_rq(c, n, a, f, (WORD **) & pdword, count);
      } else {
         if (f < 16)
            cam24i_rq(c, n, a, f, &pdword, count);
      }

      /* return reduced return size */
      *size = (int) pdword - (int) pdata;
      break;

   default:
      printf("cnaf: Unknown command 0x%X\n", (unsigned int) cmd);
   }

   if (debug) {
      if (index == RPC_CNAF16)
         printf("cmd=%d r=%d c=%d n=%d a=%d f=%d d=%X x=%d q=%d\n",
                (int) cmd, (int) count, (int) c, (int) n, (int) a, (int) f,
                (int) pword[0], (int) *x, (int) *q);
      else if (index == RPC_CNAF24)
         printf("cmd=%d r=%d c=%d n=%d a=%d f=%d d=%X x=%d q=%d\n",
                (int) cmd, (int) count, (int) c, (int) n, (int) a, (int) f,
                (int) pdword[0], (int) *x, (int) *q);
   }

   return RPC_SUCCESS;
}

#endif                          /* HAVE_CAMAC */


/*------------------------------------------------------------------*/

INT get_frontend_index()
{
   return frontend_index;
}

/*------------------------------------------------------------------*/

#ifdef OS_VXWORKS
int mfe(char *ahost_name, char *aexp_name, BOOL adebug)
#else
int main(int argc, char *argv[])
#endif
{
   INT status, i, dm_size;
   INT daemon;

   host_name[0] = 0;
   exp_name[0] = 0;
   debug = FALSE;
   daemon = 0;

   setbuf(stdout, 0);
   setbuf(stderr, 0);

#ifdef SIGPIPE
   signal(SIGPIPE, SIG_IGN);
#endif

#ifdef OS_VXWORKS
   if (ahost_name)
      strcpy(host_name, ahost_name);
   if (aexp_name)
      strcpy(exp_name, aexp_name);
   debug = adebug;
#else

   /* get default from environment */
   cm_get_environment(host_name, sizeof(host_name), exp_name, sizeof(exp_name));

   /* parse command line parameters */
   for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-' && argv[i][1] == 'd')
         debug = TRUE;
      else if (argv[i][0] == '-' && argv[i][1] == 'D')
         daemon = 1;
      else if (argv[i][0] == '-' && argv[i][1] == 'O')
         daemon = 2;
      else if (argv[i][0] == '-') {
         if (i + 1 >= argc || argv[i + 1][0] == '-')
            goto usage;
         if (argv[i][1] == 'e')
            strcpy(exp_name, argv[++i]);
         else if (argv[i][1] == 'h')
            strcpy(host_name, argv[++i]);
         else if (argv[i][1] == 'i')
            frontend_index = atoi(argv[++i]);
         else {
          usage:
            printf("usage: frontend [-h Hostname] [-e Experiment] [-d] [-D] [-O] [-i n]\n");
            printf("         [-d]     Used to debug the frontend\n");
            printf("         [-D]     Become a daemon\n");
            printf("         [-O]     Become a daemon but keep stdout\n");
            printf("         [-i n]   Set frontend index (used for event building)\n");
            return 0;
         }
      }
   }
#endif

#ifdef PART_OF_MUCAP
  frontend_early_init();
#endif

   /* check event and buffer sizes */
   if (event_buffer_size < 2 * max_event_size) {
      printf("event_buffer_size too small for max. event size\n");
      ss_sleep(5000);
      return 1;
   }

   if (max_event_size > MAX_EVENT_SIZE) {
      printf("Requested max_event_size (%d) exceeds max. system event size (%d)",
             max_event_size, MAX_EVENT_SIZE);
      ss_sleep(5000);
      return 1;
   }

   dm_size = event_buffer_size;

#ifdef OS_VXWORKS
   /* override dm_size in case of VxWorks
      take remaining free memory and use 20% of it for dm_ */
   dm_size = 2 * 10 * (max_event_size + sizeof(EVENT_HEADER) + sizeof(INT));
   if (dm_size > memFindMax()) {
      cm_msg(MERROR, "mainFE", "Not enough mem space for event size");
      return 0;
   }
   /* takes overall 20% of the available memory resource for dm_() */
   dm_size = 0.2 * memFindMax();

   /* there are two buffers */
   dm_size /= 2;
#endif

   /* reduce memory size for MS-DOS */
#ifdef OS_MSDOS
   if (dm_size > 0x4000)
      dm_size = 0x4000;         /* 16k */
#endif

   /* add frontend index to frontend name if present */
   strcpy(full_frontend_name, frontend_name);
   if (frontend_index >= 0)
      sprintf(full_frontend_name+strlen(full_frontend_name), "%02d", frontend_index);

   /* inform user of settings */
   printf("Frontend name          :     %s\n", full_frontend_name);
   printf("Event buffer size      :     %d\n", event_buffer_size);
   printf("Buffer allocation      : 2 x %d\n", dm_size);
   printf("System max event size  :     %d\n", MAX_EVENT_SIZE);
   printf("User max event size    :     %d\n", max_event_size);
   if (max_event_size_frag > 0)
      printf("User max frag. size    :     %d\n", max_event_size_frag);
   printf("# of events per buffer :     %d\n\n", dm_size / max_event_size);

   if (daemon) {
      printf("\nBecoming a daemon...\n");
      ss_daemon_init(daemon == 2);
   }

   /* now connect to server */
   if (display_period) {
      if (host_name[0])
         printf("Connect to experiment %s on host %s...", exp_name, host_name);
      else
         printf("Connect to experiment %s...", exp_name);
   }

   status = cm_connect_experiment1(host_name, exp_name, full_frontend_name,
                                   NULL, DEFAULT_ODB_SIZE, DEFAULT_FE_TIMEOUT);
   if (status != CM_SUCCESS) {
      /* let user read message before window might close */
      ss_sleep(5000);
      return 1;
   }

   if (display_period)
      printf("OK\n");

   /* book buffer space */
   status = dm_buffer_create(dm_size, max_event_size);
   if (status != CM_SUCCESS) {
      printf("dm_buffer_create: Not enough memory or event too big\n");
      return 1;
   }

   /* remomve any dead frontend */
   cm_cleanup(full_frontend_name, FALSE);

   /* shutdown previous frontend */
   status = cm_shutdown(full_frontend_name, FALSE);
   if (status == CM_SUCCESS && display_period) {
      printf("Previous frontend stopped\n");

      /* let user read message */
      ss_sleep(3000);
   }

   /* register transition callbacks */
   if (cm_register_transition(TR_START, tr_prestart, 400+crate_number) != CM_SUCCESS ||
       cm_register_transition(TR_START, tr_start, 500+crate_number) != CM_SUCCESS ||
       cm_register_transition(TR_STOP, tr_stop, 500+crate_number) != CM_SUCCESS ||
       cm_register_transition(TR_PAUSE, tr_pause, 500+crate_number) != CM_SUCCESS ||
       cm_register_transition(TR_RESUME, tr_resume, 500+crate_number) != CM_SUCCESS) {
      printf("Failed to start local RPC server");
      cm_disconnect_experiment();
      dm_buffer_release();

      /* let user read message before window might close */
      ss_sleep(5000);
      return 1;
   }
#ifdef HAVE_CAMAC

   /* register CNAF callback */
   cm_register_function(RPC_CNAF16, cnaf_callback);
   cm_register_function(RPC_CNAF24, cnaf_callback);

#endif

   cm_get_experiment_database(&hDB, &status);

   /* set time from server */
#ifdef OS_VXWORKS
   cm_synchronize(NULL);
#endif

   /* turn off watchdog if in debug mode */
   if (debug)
      cm_set_watchdog_params(TRUE, 0);

   /* increase RPC timeout to 2min for logger with exabyte or blocked disk */
   rpc_set_option(-1, RPC_OTIMEOUT, 120000);

   /* set own message print function */
   if (display_period)
      cm_set_msg_print(MT_ALL, MT_ALL, message_print);

   /* call user init function */
   if (display_period)
      printf("Init hardware...");
   if (frontend_init() != SUCCESS) {
      if (display_period)
         printf("\n");
      cm_disconnect_experiment();
      dm_buffer_release();

      /* let user read message before window might close */
      ss_sleep(5000);
      return 1;
   }

   /* reqister equipment in ODB */
   if (register_equipment() != SUCCESS) {
      if (display_period)
         printf("\n");
      cm_disconnect_experiment();
      dm_buffer_release();

      /* let user read message before window might close */
      ss_sleep(5000);
      return 1;
   }

   if (display_period)
      printf("OK\n");

   /* initialize screen display */
   if (display_period) {
      ss_sleep(1000);
      display(TRUE);
   }

   /* switch on interrupts if running */
   if (interrupt_eq && run_state == STATE_RUNNING)
      interrupt_enable(TRUE);

   /* initialize ss_getchar */
   ss_getchar(0);

   /* call main scheduler loop */
   status = scheduler();

   /* reset terminal */
   ss_getchar(TRUE);

   /* switch off interrupts */
   if (interrupt_eq) {
      interrupt_configure(CMD_INTERRUPT_DISABLE, 0, 0);
      interrupt_configure(CMD_INTERRUPT_DETACH, 0, 0);
      if (interrupt_odb_buffer)
         free(interrupt_odb_buffer);
   }

   /* detach interrupts */
   if (interrupt_eq != NULL)
      interrupt_configure(CMD_INTERRUPT_DETACH, interrupt_eq->info.source, 0);

   /* call user exit function */
   frontend_exit();

   /* close slow control drivers */
   for (i = 0; equipment[i].name[0]; i++)
      if ((equipment[i].info.eq_type & EQ_SLOW) && equipment[i].status == FE_SUCCESS)
         equipment[i].cd(CMD_EXIT, &equipment[i]);

   /* close network connection to server */
   cm_disconnect_experiment();

   if (display_period) {
      if (status == RPC_SHUTDOWN) {
         ss_clear_screen();
         ss_printf(0, 0, "Frontend shut down.");
         ss_printf(0, 1, "");
      }
   }

   if (status != RPC_SHUTDOWN)
      printf("Network connection aborted.\n");

   dm_buffer_release();

   return 0;
}
