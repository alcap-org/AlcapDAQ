/********************************************************************\
Name:         mevb.c
Created by:   Pierre-Andre Amaudruz

Contents:     Main Event builder task.

** muCap patches **
- converted from C to C++: added extern "C" on ybos_swap_event
- added protection on emask to avoid touching crates that are disabled
  in source unbooking and fragment assembly.

$Log: mevb.c,v $
Revision 1.16  2004/10/07 20:08:34  pierre
1.9.5

Revision 1.15  2004/10/04 23:55:28  pierre
move ebuilder into equipment list

Revision 1.14  2004/09/29 17:55:34  pierre
fix speed problem

Revision 1.13  2004/09/29 16:20:31  pierre
change Ebuilder structure

Revision 1.12  2004/01/08 08:40:08  midas
Implemented standard indentation

Revision 1.11  2004/01/08 06:48:26  pierre
Doxygen the file

Revision 1.10  2003/08/19 23:26:36  pierre
fix cm_get_environment arg

Revision 1.9  2002/10/07 17:04:01  pierre
fix tr_stop request

Revision 1.8  2002/09/28 00:48:33  pierre
Add EB_USER_ERROR handling, handFlush()

Revision 1.7  2002/09/25 18:37:37  pierre
correct: header passing, user field, abort run

Revision 1.6  2002/08/29 22:07:47  pierre
fix event header, double task, EOR

Revision 1.5  2002/07/13 05:45:49  pierre
added swap before user function

Revision 1.4  2002/06/14 04:59:08  pierre
revised for ybos 

Revision 1.3  2002/05/08 20:51:41  midas
Added extra parameter to function db_get_value()

Revision 1.2  2002/01/17 23:34:14  pierre
doc++ format

Revision 1.1.1.1  2002/01/17 19:49:54  pierre
Initial Version

\********************************************************************/

/**dox***************************************************************/
/* @file mevb.c
The Event builder main file 
*/

#include <stdio.h>
#include "midas.h"
#include "mevb.h"
#include "msystem.h"
#include "ybos.h"

#define SERVER_CACHE_SIZE  100000       /* event cache before buffer */

#define ODB_UPDATE_TIME      1000       /* 1 seconds for ODB update */

#define DEFAULT_FE_TIMEOUT  60000       /* 60 seconds for watchdog timeout */

EBUILDER_SETTINGS ebset;
EBUILDER_CHANNEL ebch[MAX_CHANNELS];

INT run_state;                     /* STATE_RUNNING, STATE_STOPPED, STATE_PAUSED */
INT run_number;
DWORD last_time;
DWORD actual_time;                 /* current time in seconds since 1970 */
DWORD actual_millitime;            /* current time in milliseconds */

char host_name[HOST_NAME_LENGTH];
char expt_name[NAME_LENGTH];
char full_frontend_name[256];
char buffer_name[NAME_LENGTH];
INT   nfragment;
char *dest_event;
HNDLE hDB, hKey, hStatKey, hSubkey, hEqKey, hESetKey;
BOOL debug = FALSE, debug1 = FALSE;

BOOL wheel = FALSE;
char bars[] = "|\\-/";
int i_bar;
BOOL abort_requested = FALSE, stop_requested = TRUE;
DWORD stop_time = 0, request_stop_time = 0;

INT(*meb_fragment_add) (char *, char *, INT *);
INT handFlush(void);
INT source_booking(void);
INT source_unbooking(void);
INT close_buffers(void);
INT source_scan(INT fmt, EQUIPMENT_INFO *eq_info);
INT eb_mfragment_add(char *pdest, char *psrce, INT * size);
INT eb_yfragment_add(char *pdest, char *psrce, INT * size);

INT eb_begin_of_run(INT, char *, char *);
INT eb_end_of_run(INT, char *);
INT eb_user(INT, BOOL mismatch, EBUILDER_CHANNEL *, EVENT_HEADER *, void *, INT *);
INT load_fragment(void);
INT scan_fragment(void);
extern char *frontend_name;
extern char *frontend_file_name;
extern BOOL frontend_call_loop;

extern INT max_event_size;
extern INT max_event_size_frag;
extern INT event_buffer_size;
extern INT display_period;
extern INT ebuilder_init(void);
extern INT ebuilder_exit(void);
extern INT ebuilder_loop(void);

extern EQUIPMENT equipment[];
extern "C" {
  extern INT ybos_event_swap(DWORD * pevt);
};

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

/********************************************************************/
INT register_equipment(void)
{
  INT index, size, status;
  char str[256];
  EQUIPMENT_INFO *eq_info;
  EQUIPMENT_STATS *eq_stats;
  HNDLE hKey;

  /* get current ODB run state */
  size = sizeof(run_state);
  run_state = STATE_STOPPED;
  db_get_value(hDB, 0, "/Runinfo/State", &run_state, &size, TID_INT, TRUE);
  size = sizeof(run_number);
  run_number = 1;
  status = db_get_value(hDB, 0, "/Runinfo/Run number", &run_number, &size, TID_INT, TRUE);
  assert(status == SUCCESS);

  /* scan EQUIPMENT table from mevb.C */
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
    equipment[index].status = EB_SUCCESS;

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
    else    /* default format is MIDAS */
      equipment[index].format = FORMAT_MIDAS;

    gethostname(eq_info->frontend_host, sizeof(eq_info->frontend_host));
    strcpy(eq_info->frontend_name, full_frontend_name);
    strcpy(eq_info->frontend_file_name, frontend_file_name);

    /* set record from equipment[] table in frontend.c */
    db_set_record(hDB, hKey, eq_info, sizeof(EQUIPMENT_INFO), 0);

    /* get record once at the start equipment info */
    size = sizeof(EQUIPMENT_INFO);
    db_get_record(hDB, hKey, eq_info, &size, 0);

    /*---- Create just the key , leave it empty ---------------------------------*/
    sprintf(str, "/Equipment/%s/Variables", equipment[index].name);
    db_create_key(hDB, 0, str, TID_KEY);
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
    status = db_open_record(hDB, hKey, eq_stats, sizeof(EQUIPMENT_STATS)
      , MODE_WRITE, NULL, NULL);
    if (status != DB_SUCCESS) {
      cm_msg(MERROR, "register_equipment", 
        "Cannot open statistics record, error %d. Probably other FE is using it",
        status);
      ss_sleep(3000);
    }

    /*---- open event buffer ---------------------------------------*/
    if (eq_info->buffer[0]) {
      status = bm_open_buffer(eq_info->buffer, EVENT_BUFFER_SIZE,
        &equipment[index].buffer_handle);
      if (status != BM_SUCCESS && status != BM_CREATED) {
        cm_msg(MERROR, "register_equipment",
          "Cannot open event buffer. Try to reduce EVENT_BUFFER_SIZE in midas.h \
          and rebuild the system.");
        return 0;
      }

      /* set the default buffer cache size */
      bm_set_cache_size(equipment[index].buffer_handle, 0, SERVER_CACHE_SIZE);
    } else {
      cm_msg(MERROR, "register_equipment", "Destination buffer must be present");
      ss_sleep(3000);
      exit(0);
    }
  }
  return SUCCESS;
}

/********************************************************************/
INT load_fragment(void)
{
  INT i, size, type;
  HNDLE hEqKey, hSubkey;
  EQUIPMENT_INFO *eq_info;
  KEY  key;
  char  buffer[NAME_LENGTH];
  char  format[8];

  /* Get equipment pointer, only one eqp for now */
  eq_info = &equipment[0].info;

  /* Scan Equipment/Common listing */
  if (db_find_key(hDB, 0, "Equipment", &hEqKey) != DB_SUCCESS) {
    cm_msg(MINFO, "load_fragment", "Equipment listing not found");
    return EB_ERROR;
  }

  /* Scan the Equipment list for fragment info collection */
  for (i = 0, nfragment=0 ; ; i++) {
    db_enum_key(hDB, hEqKey, i, &hSubkey);
    if (!hSubkey)
      break;
    db_get_key(hDB, hSubkey, &key);
    if (key.type == TID_KEY) {
      /* Equipment name */
      if (debug) printf("Equipment name:%s\n", key.name);
      /* Check if equipment is EQ_EB */
      size = sizeof(INT);
      db_get_value(hDB, hSubkey, "common/type", &type, &size, TID_INT, 0);
      size = sizeof(buffer);
      db_get_value(hDB, hSubkey, "common/Buffer", buffer, &size, TID_STRING, 0);
      size = sizeof(format);
      db_get_value(hDB, hSubkey, "common/Format", format, &size, TID_STRING, 0);
      /* Check if equipment match EB requirements */
      if ((type & EQ_EB)
        && (strncmp(buffer, buffer_name, strlen(buffer_name)) == 0)
        && (strncmp(format, eq_info->format, strlen(format)) == 0)) {
          /* match=> fill internal eb structure */
          strcpy(ebch[nfragment].format, format);
          strcpy(ebch[nfragment].buffer, buffer);
          size = sizeof(WORD);
          db_get_value(hDB, hSubkey, "common/Trigger Mask", &ebch[nfragment].trigger_mask, &size, TID_WORD, 0);
          size = sizeof(WORD);
          db_get_value(hDB, hSubkey, "common/Event ID", &ebch[nfragment].event_id, &size, TID_WORD, 0);
          nfragment++;
        }
    }
  }

  printf("Found %d fragment matching EB setting\n", nfragment);
  /* Point to the Ebuilder settings */
  /* Set fragment_add function based on the format */
  if (equipment[0].format == FORMAT_MIDAS)
    meb_fragment_add = eb_mfragment_add;
  else if (equipment[0].format == FORMAT_YBOS)
    meb_fragment_add = eb_yfragment_add;
  else {
    cm_msg(MERROR, "mevb", "Unknown data format :%d", format);
    return EB_ERROR;
  }

  /* allocate destination event buffer */
  dest_event = (char *) malloc(nfragment * (max_event_size + sizeof(EVENT_HEADER)));
  memset(dest_event, 0, nfragment * (max_event_size + sizeof(EVENT_HEADER)));
  if (dest_event == NULL) {
    cm_msg(MERROR, "EBuilder", "%s: Not enough memory for event buffer", full_frontend_name);
    return EB_ERROR;
  }
  return EB_SUCCESS;
}

/********************************************************************/
INT scan_fragment(void)
{
  INT fragn, status;
  EQUIPMENT *eq;
  EQUIPMENT_INFO *eq_info;

  /* Get equipment pointer, only one eqp for now */
  eq_info = &equipment[0].info;

  /* Main event loop */
  do {
    switch (run_state) {
      case STATE_STOPPED:
      case STATE_PAUSED:
        /* skip the source scan and yield */
        status = cm_yield(500);
        if (wheel) {
          printf("...%c Snoring\r", bars[i_bar++ % 4]);
          fflush(stdout);
        }
        break;
      case STATE_RUNNING:
        status = source_scan(equipment[0].format, eq_info);
        switch (status) {
            case BM_ASYNC_RETURN:  // No event found for now, Check for timeout 
              for (fragn = 0; fragn < nfragment; fragn++) {
                if (ebch[fragn].timeout > TIMEOUT) {        /* Timeout */
                  if (stop_requested) {                     /* Stop */
                    if (debug) printf("Stop requested on timeout %d\n", status);
                      status = close_buffers();
                    break;
                  } 
                  else {         /* No stop requested  but timeout */
                    if (wheel) {
                      printf("...%c Timoing on %1.0lf\r", bars[i_bar++ % 4],
                        eq->stats.events_sent);
                      fflush(stdout);
                      status = cm_yield(50);
                    }
                  }
                }
                //else { /* No timeout loop back */
              } // for loop over all fragments
              break;
            case EB_ERROR:
            case EB_USER_ERROR:
              abort_requested = TRUE;
              if (status == EB_USER_ERROR)
                cm_msg(MTALK, "EBuilder", "%s: Error signaled by user code - stopping run...", full_frontend_name);
              else
                cm_msg(MTALK, "EBuilder", "%s: Event mismatch - Stopping run...", full_frontend_name);
              if (cm_transition(TR_STOP, 0, NULL, 0, ASYNC, 0) != CM_SUCCESS) {
                cm_msg(MERROR, "EBuilder", "%s: Stop Transition request failed", full_frontend_name);
                return status;
              }
              if (debug) printf("Stop requested on Error %d\n", status);
              status = close_buffers();
              return status;
              break;
            case EB_SUCCESS:
            case EB_SKIP:
              //   Normal path if event has been assembled
              //   No yield in this case.
              break;
            default:
              cm_msg(MERROR, "Source_scan", "unexpected return %d", status);
              status = SS_ABORT;
              }  // switch scan_source
        break;
    }
    /* EB job done, update statistics if its time */
    /* Check if it's time to do statistics job */
    if ((actual_millitime = ss_millitime()) - last_time > 1000) {
      /* Force event to appear at the destination if Ebuilder is remote */
      rpc_flush_event();
      /* Force event ot appear at the destination if Ebuilder is local */
      bm_flush_cache(equipment[0].buffer_handle, ASYNC);

      status = cm_yield(10);

      eq = &equipment[0];
      eq->stats.events_sent += eq->events_sent;
      eq->stats.events_per_sec =
        eq->events_sent / ((actual_millitime - last_time) / 1000.0);
      eq->stats.kbytes_per_sec =
        eq->bytes_sent / 1024.0 / ((actual_millitime - last_time) /
        1000.0);
      eq->bytes_sent = 0;
      eq->events_sent = 0;
      /* update destination statistics */
      db_send_changed_records();
      /* Keep track of last ODB update */
      last_time = ss_millitime();
    }
  } while (status != RPC_SHUTDOWN && status != SS_ABORT);

  return status;
  }

/********************************************************************/
INT eb_mfragment_add(char *pdest, char *psrce, INT * size)
{
   BANK_HEADER *psbh, *pdbh;
   char *psdata, *pddata;
   INT bksize;

   /* Condition for new EVENT the data_size should be ZERO */
   *size = ((EVENT_HEADER *) pdest)->data_size;

   /* destination pointer */
   pddata = pdest + *size + sizeof(EVENT_HEADER);

   if (*size) {
      /* NOT the first fragment */

      /* Swap event source if necessary */
      psbh = (BANK_HEADER *) (((EVENT_HEADER *) psrce) + 1);
      bk_swap(psbh, FALSE);

      /* source pointer */
      psbh = (BANK_HEADER *) (((EVENT_HEADER *) psrce) + 1);
      psdata = (char *) (psbh + 1);

      /* copy all banks without the bank header */
      bksize = psbh->data_size;

      /* copy */
      memcpy(pddata, psdata, bksize);

      /* update event size */
      ((EVENT_HEADER *) pdest)->data_size += bksize;

      /* update bank size */
      pdbh = (BANK_HEADER *) (((EVENT_HEADER *) pdest) + 1);
      pdbh->data_size += bksize;

      *size = ((EVENT_HEADER *) pdest)->data_size;
   } else {
      /* First event without the event header but with the 
         bank header as the size is zero */
      *size = ((EVENT_HEADER *) psrce)->data_size;

      /* Swap event if necessary */
      psbh = (BANK_HEADER *) (((EVENT_HEADER *) psrce) + 1);
      bk_swap(psbh, FALSE);

      /* copy first fragment */
      memcpy(pddata, psbh, *size);

      /* update destination event size */
      ((EVENT_HEADER *) pdest)->data_size = *size;
   }
   return CM_SUCCESS;
}

/*--------------------------------------------------------------------*/
INT eb_yfragment_add(char *pdest, char *psrce, INT * size)
{
   /* pdest : EVENT_HEADER pointer
      psrce : EVENT_HEADER pointer
      Keep pbkh for later incrementation
    */
   char *psdata, *pddata;
   DWORD *pslrl, *pdlrl;
   INT i4frgsize, i1frgsize, status;

   /* Condition for new EVENT the data_size should be ZERO */
   *size = ((EVENT_HEADER *) pdest)->data_size;

   /* destination pointer skip the header as it has been already
      composed and the usere may have modified it on purpose (Midas Control) */
   pddata = pdest + *size + sizeof(EVENT_HEADER);

   /* the Midas header is present for logger */
   if (*size) {                 /* already filled with a fragment */

      /* source pointer: number of DWORD (lrl included) */
      pslrl = (DWORD *) (((EVENT_HEADER *) psrce) + 1);

      /* Swap event if necessary */
      status = ybos_event_swap(pslrl);

      /* copy done in bytes, do not include LRL */
      psdata = (char *) (pslrl + 1);

      /* copy size in I*4 (lrl included, remove it) */
      i4frgsize = (*pslrl);
      i1frgsize = 4 * i4frgsize;

      /* append fragment */
      memcpy(pddata, psdata, i1frgsize);

      /* update Midas header event size */
      ((EVENT_HEADER *) pdest)->data_size += i1frgsize;

      /* update LRL size (I*4) */
      pdlrl = (DWORD *) (((EVENT_HEADER *) pdest) + 1);
      *pdlrl += i4frgsize;

      /* Return event size in bytes */
      *size = ((EVENT_HEADER *) pdest)->data_size;
   } else {                     /* new destination event */
      /* The composed event has already the MIDAS header.
         which may have been modified by the user in ebuser.c
         Will be stripped by the logger (YBOS).
         Copy the first full event ( no EVID suppression )
         First event (without the event header) */

      /* source pointer */
      pslrl = (DWORD *) (((EVENT_HEADER *) psrce) + 1);

      /* Swap event if necessary */
      status = ybos_event_swap(pslrl);

      /* size in byte from the source midas header */
      *size = ((EVENT_HEADER *) psrce)->data_size;

      /* copy first fragment */
      memcpy(pddata, (char *) pslrl, *size);

      /* update destination Midas header event size */
      ((EVENT_HEADER *) pdest)->data_size += *size;

   }
   return CM_SUCCESS;
}

/*--------------------------------------------------------------------*/
INT tr_start(INT rn, char *error)
{
  EBUILDER(ebuilder_str);
  INT status, size, i;
  char   str[128];
  KEY    key;
  HNDLE  hKey, hEqkey, hEqFRkey;
  EQUIPMENT_INFO *eq_info;
  

  eq_info = &equipment[0].info;

  /* Get update eq_info from ODB */
  sprintf(str, "/Equipment/%s/Common", equipment[0].name);
  status = db_find_key(hDB, 0, str, &hKey);
  size = sizeof(EQUIPMENT_INFO);
  db_get_record(hDB, hKey, eq_info, &size, 0);

  ebset.nfragment = nfragment;

  /* reset serial numbers */
  for (i = 0; equipment[i].name[0]; i++) {
    equipment[i].serial_number = 1;
    equipment[i].subevent_number = 0;
    equipment[i].stats.events_sent = 0;
    equipment[i].odb_in = equipment[i].odb_out = 0;
  }

  /* Get / Set Settings */
  sprintf(str, "/Equipment/%s/Settings", equipment[0].name);
  if (db_find_key(hDB, 0, str, &hEqkey) != DB_SUCCESS) {
    status = db_create_record(hDB, 0, str, strcomb(ebuilder_str));
  }

  /* Keep Key on Ebuilder/Settings */
  sprintf(str, "/Equipment/%s/Settings", equipment[0].name);
  if (db_find_key(hDB, 0, str, &hEqkey) != DB_SUCCESS) {
    cm_msg(MINFO, "load_fragment", "/Equipment/%s/Settings not found", equipment[0].name);
  }
  
  /* Update or Create User_field */
  size = sizeof(ebset.user_field);
  status = db_get_value(hDB, hEqkey, "User Field", ebset.user_field, &size, TID_STRING, TRUE);

  /* Update or Create User_Build */
  size = sizeof(ebset.user_build);
  status = db_get_value(hDB, hEqkey, "User Build", &ebset.user_build, &size, TID_BOOL, TRUE);

  /* update ODB */
  size = sizeof(INT);
  status = db_set_value(hDB, hEqkey, "Number of Fragment", &ebset.nfragment, size, 1, TID_INT);  

    /* Create or update the fragment request list */
  status = db_find_key(hDB, hEqkey, "Fragment Required", &hEqFRkey);
  status = db_get_key (hDB, hEqFRkey, &key);
  if (key.num_values != ebset.nfragment) {
    cm_msg(MINFO, "mevb", "Number of Fragment mismatch ODB:%d - CUR:%d", key.num_values, ebset.nfragment);
    free (ebset.preqfrag);
    size = ebset.nfragment*sizeof(BOOL);
    ebset.preqfrag = malloc(size);
    for (i=0 ; i<ebset.nfragment ; i++)
      ebset.preqfrag[i] = TRUE;
    status = db_set_value(hDB, hEqkey, "Fragment Required", ebset.preqfrag, size, ebset.nfragment, TID_BOOL);  
  } else {  // Take from ODBedit
    size = key.total_size;
    free (ebset.preqfrag);
    ebset.preqfrag = malloc(size);
    status = db_get_data(hDB, hEqkey, ebset.preqfrag, &size, TID_BOOL);
  }
  /* Cleanup fragment flags */
  free (ebset.received);
  ebset.received = malloc(size);
  for (i=0 ; i < ebset.nfragment ; i++) 
    ebset.received[i] = FALSE;

  /* Call BOR user function */
  status = eb_begin_of_run(run_number, ebset.user_field, error);
  if (status != EB_SUCCESS) {
    cm_msg(MERROR, "eb_prestart", "run start aborted due to eb_begin_of_run (%d)",
      status);
    return status;
  }

  /* Book all fragment */
  status = source_booking();
  if (status != SUCCESS)
    return status;

  /* local run state */
  run_state = STATE_RUNNING;
  run_number = rn;
  stop_requested = FALSE;
  abort_requested = FALSE;
  printf("%s-Starting New Run: %d\n", full_frontend_name, rn);

  /* Reset global trigger mask */
  return CM_SUCCESS;
}

/*--------------------------------------------------------------------*/
INT tr_stop(INT rn, char *error)
{
   printf("\n%s-Stopping Run: %d detected\n", full_frontend_name, rn);

   /* local stop */
   stop_requested = TRUE;

   /* local stop time */
   request_stop_time = ss_millitime();
   return CM_SUCCESS;
}

/*--------------------------------------------------------------------*/
void free_event_buffer(INT nfrag)
{
   INT i;
   for (i = 0; i < nfrag; i++) {
      if (ebch[i].pfragment) {
         free(ebch[i].pfragment);
         ebch[i].pfragment = NULL;
      }
   }
}

/*--------------------------------------------------------------------*/
INT handFlush()
{
   int i, size, status;
   char strout[256];

   /* Do Hand flush until better way to  garantee the input buffer to be empty */
   if (debug)
      printf("Hand flushing system buffer... \n");
   for (i = 0; i < nfragment; i++) {
      do {
         size = max_event_size;
         status = bm_receive_event(ebch[i].hBuf, ebch[i].pfragment, &size, ASYNC);
         if (debug1) {
            sprintf(strout,
                    "booking:Hand flush bm_receive_event[%d] hndle:%d stat:%d  Last Ser:%d",
                    i, ebch[i].hBuf, status,
                    ((EVENT_HEADER *) ebch[i].pfragment)->serial_number);
            printf("%s\n", strout);
         }
      } while (status == BM_SUCCESS);
   }

   /* Empty source buffer */
   status = bm_empty_buffers();
   if (status != BM_SUCCESS)
      cm_msg(MERROR, "handFlush", "bm_empty_buffers failure [%d]", status);
   run_state = STATE_STOPPED;
   return status;
}


/*--------------------------------------------------------------------*/
INT source_booking()
{
   INT j, i, status, status1, status2;

   if (debug)
      printf("Entering booking\n");

   /* Book all the source channels */
   for (i = 0; i < nfragment; i++) {
      /* Book only the requested event mask */
     if (ebset.preqfrag[i]) {
         /* Connect channel to source buffer */
         status1 = bm_open_buffer(ebch[i].buffer, EVENT_BUFFER_SIZE, &(ebch[i].hBuf));

         if (debug)
           printf("bm_open_buffer frag:%d buf:%s handle:%d stat:%d\n",
                   i, ebch[i].buffer, ebch[i].hBuf, status1);
         /* Register for specified channel event ID and Trigger mask */
         status2 =
             bm_request_event(ebch[i].hBuf, ebch[i].event_id,
                              ebch[i].trigger_mask, GET_ALL, &ebch[i].req_id, NULL);
         if (debug)
           printf("bm_request_event frag:%d id:%d msk:%d req_id:%d stat:%d\n",
                   i, ebch[i].event_id, ebch[i].trigger_mask, ebch[i].req_id, status2);
         if (((status1 != BM_SUCCESS) && (status1 != BM_CREATED)) ||
             ((status2 != BM_SUCCESS) && (status2 != BM_CREATED))) {
            cm_msg(MERROR, "source_booking",
                   "Open buffer/event request failure [%d %d %d]", i, status1, status2);
            return BM_CONFLICT;
         }

         /* allocate local source event buffer */
         if (ebch[i].pfragment)
            free(ebch[i].pfragment);
         ebch[i].pfragment = (char *) malloc(max_event_size + sizeof(EVENT_HEADER));
         if (debug)
            printf("malloc pevent frag:%d pevent:%p\n", i, ebch[i].pfragment);
         if (ebch[i].pfragment == NULL) {
            free_event_buffer(nfragment);
            cm_msg(MERROR, "source_booking", "Can't allocate space for buffer");
            return BM_NO_MEMORY;
         }
      }
   }

   /* Empty source buffer */
   status = bm_empty_buffers();
   if (status != BM_SUCCESS) {
      cm_msg(MERROR, "source_booking", "bm_empty_buffers failure [%d]", status);
      return status;
   }

   if (debug) {
      printf("bm_empty_buffers stat:%d\n", status);
      for (j = 0; j < ebset.nfragment; j++) {
         printf(" buff:%s", ebch[j].buffer);
         printf(" ser#:%d", ebch[j].serial);
         printf(" hbuf:%2d", ebch[j].hBuf);
         printf(" rqid:%2d", ebch[j].req_id);
         printf(" opst:%d", status1);
         printf(" rqst:%d", status2);
         printf(" evid:%2d", ebch[j].event_id);
         printf(" tmsk:0x%4.4x\n", ebch[j].trigger_mask);
      }
   }

   return SUCCESS;
}

/*--------------------------------------------------------------------*/
INT source_unbooking()
{
   INT i, status;

#if 0
   /* Skip unbooking if already done */
   if (ebch[0].pfragment == NULL)
      return EB_SUCCESS;
#endif

   /* unbook all source channels */
   for (i = 0; i< nfragment; i++) {
      bm_empty_buffers();

      if (ebch[i].set.emask) {
        /* Remove event ID registration */
        status = bm_delete_request(ebch[i].req_id);
        if (debug)
           printf("unbook: bm_delete_req[%d] req_id:%d stat:%d\n", i, ebch[i].req_id,
                  status);

        /* Close source buffer */
        status = bm_close_buffer(ebch[i].hBuf);
        if (debug)
           printf("unbook: bm_close_buffer[%d] hndle:%d stat:%d\n", i, ebch[i].hBuf,
                  status);
        if (status != BM_SUCCESS) {
           cm_msg(MERROR, "source_unbooking", "Close buffer[%d] stat:", i, status);
           return status;
        }
     }
   }

   /* release local event buffer memory */
   free_event_buffer(nfragment);

   return EB_SUCCESS;
}

/*--------------------------------------------------------------------*/
INT close_buffers(void)
{
  INT status;
  char error[256];
  EQUIPMENT *eq;

  eq = &equipment[0];

  /* Flush local destination cache */
  bm_flush_cache(equipment[0].buffer_handle, SYNC);
  /* Call user function */
  eb_end_of_run(run_number, error);
  /* Cleanup buffers */
  handFlush();
  /* Detach all source from midas */
  status = source_unbooking();

  /* Compose message */
  stop_time = ss_millitime() - request_stop_time;
  sprintf(error, "Run %d Stop after %1.0lf events sent DT:%d[ms]",
    run_number, eq->stats.events_sent, stop_time);
  cm_msg(MINFO, "EBuilder", "%s", error);

  run_state = STATE_STOPPED;
  abort_requested = FALSE;
  return status;
}

/********************************************************************/
/**
Scan all the fragment source once per call.

-# This will retrieve the full midas event not swapped (except the
MIDAS_HEADER) for each fragment if possible. The fragment will
be stored in the channel event pointer.
-# if after a full nfrag path some frag are still not cellected, it
returns with the frag# missing for timeout check.
-# If ALL fragments are present it will check the midas serial#
for a full match across all the fragments.
-# If the serial check fails it returns with "event mismatch"
and will abort the event builder but not stop the run for now.
-# If the serial check is passed, it will call the user_build function
where the destination event is going to be composed.

@param fmt Fragment format type 
@param eq_info Equipement pointer
@param dest_hBuf  Destination buffer handle
@return   EB_NO_MORE_EVENT, EB_COMPOSE_TIMEOUT
if different then SUCCESS (bm_compose, rpc_sent error)
*/
INT source_scan(INT fmt, EQUIPMENT_INFO *eq_info)
{
   static char bars[] = "|/-\\";
   static int i_bar;
   static DWORD serial;
   DWORD *plrl;
   BOOL   complete;
   INT i, j, status, size;
   INT act_size;
   BOOL found, event_mismatch;
   BANK_HEADER *psbh;

   status = SUCCESS;

   /* Scan all channels at least once */
   for (i = 0; i < nfragment; i++) {
      /* Check if current channel needs to be received */
     if (ebset.preqfrag[i] && !ebset.received[i]) {
         /* Get fragment and store it in ebch[i].pfragment */
         size = max_event_size;
         status = bm_receive_event(ebch[i].hBuf, ebch[i].pfragment, &size, ASYNC);
         switch (status) {
         case BM_SUCCESS:      /* event received */
            /* Mask event */
            ebset.received[i] = TRUE;
            /* Keep local serial */
            ebch[i].serial = ((EVENT_HEADER *) ebch[i].pfragment)->serial_number;

            /* Swap event depending on data format */
            switch (fmt) {
            case FORMAT_YBOS:
               plrl = (DWORD *) (((EVENT_HEADER *) ebch[i].pfragment) + 1);
               ybos_event_swap(plrl);
               break;
            case FORMAT_MIDAS:
               psbh = (BANK_HEADER *) (((EVENT_HEADER *) ebch[i].pfragment) + 1);
               bk_swap(psbh, FALSE);
               break;
            }

            if (debug1) {
              printf("SUCC: ch:%d ser:%d rec:%d sz:%d\n", i,
                      ebch[i].serial, ebset.received[i], size);
            }
            break;
         case BM_ASYNC_RETURN: /* timeout */
            ebch[i].timeout++;
            if (debug1) {
              printf("ASYNC: ch:%d ser:%d rec:%d sz:%d\n", i,
                      ebch[i].serial, ebset.received[i], size);
            }
            break;
         default:              /* Error */
            cm_msg(MERROR, "event_scan", "bm_receive_event error %d", status);
            return status;
            break;
         }
      }    /* next channel */
   }

   /* Check if all fragments have been received */
   complete = FALSE;
   for (i = 0; i < nfragment;i++) {
     if (ebset.preqfrag[i] && !ebset.received[i])
       break;
   }
   if (i == nfragment) {
     complete = TRUE;
     /* Check if serial matches */
     found = event_mismatch = FALSE;
     /* Check Serial, mark first serial */
     for (i = 0; i < nfragment; i++) {
         if (ebset.preqfrag[i] && ebset.received[i] && !found) {
            serial = ebch[i].serial;
            found = TRUE;
         } else {
            if (ebset.preqfrag[i] && ebset.received[i] && (serial != ebch[i].serial)) {
               /* Event mismatch */
               event_mismatch = TRUE;
            }
         }
      }

    /* internal action in case of event mismatch */
     if (event_mismatch && debug) {
       char str[256];
       char strsub[128];
       strcpy(str, "event mismatch: ");
       for (j = 0; j < nfragment; j++) {
         sprintf(strsub, "Ser[%d]:%d ", j, ebch[j].serial);
         strcat(str, strsub);
       }
       printf("event serial mismatch %s\n", str);
     }

     /* In any case reset destination buffer */
     memset(dest_event, 0, sizeof(EVENT_HEADER));
     act_size = 0;

     /* Fill reserved header space of destination event with
     final header information */
     bm_compose_event((EVENT_HEADER *) dest_event, eq_info->event_id, eq_info->trigger_mask,
       act_size, ebch[0].serial);

     /* Pass fragments to user with mismatch flag, for final check before assembly */
     status = eb_user(nfragment, event_mismatch, ebch
       , (EVENT_HEADER *) dest_event,(void *) ((EVENT_HEADER *) dest_event + 1), &act_size);
     if (status != EB_SUCCESS) {
       if (status == EB_SKIP) {
         /* Reset mask and timeouts as even thave been succesfully send */
         for (i = 0; i < nfragment; i++) {
           ebch[i].timeout = 0;
           ebset.received[i] = FALSE;
         }
       }
       return status;        // Event mark as EB_SKIP or EB_ABORT by user
     }

     /* Allow bypass of fragment assembly if user did it on its own */
     if (!ebset.user_build) {
       for (j = 0; j < nfragment; j++) {
         if (ebch[j].set.emask) {
           status = meb_fragment_add(dest_event, ebch[j].pfragment, &act_size);
           if (status != EB_SUCCESS) {
             cm_msg(MERROR, "source_scan",
               "compose fragment:%d current size:%d (%d)", j, act_size, status);
             return EB_ERROR;
           }
         }
       }
     }

     /* Overall event to be sent */
     act_size = ((EVENT_HEADER *) dest_event)->data_size + sizeof(EVENT_HEADER);

     /* Send event and wait for completion */
     status = rpc_send_event(equipment[0].buffer_handle, dest_event, act_size, SYNC);
     if (status != BM_SUCCESS) {
       if (debug)
         printf("rpc_send_event returned error %d, event_size %d\n",
         status, act_size);
       cm_msg(MERROR, "EBuilder", "%s: rpc_send_event returned error %d", full_frontend_name, status);
       return EB_ERROR;
     }

     /* Keep track of the total byte count */
     equipment[0].bytes_sent += act_size;

     /* update destination event count */
     equipment[0].events_sent++;

     /* Reset mask and timeouts as even thave been succesfully send */
     for (i = 0; i < nfragment; i++) {
       ebch[i].timeout = 0;
       ebset.received[i] = FALSE;
     }
   } // all fragment recieved for this event

   return status;
}

/*--------------------------------------------------------------------*/
int main(unsigned int argc, char **argv)
{
  INT status;
  unsigned int i;
  BOOL daemon = FALSE;
  
  /* init structure */
  memset(&ebch[0], 0, sizeof(ebch));

  /* set default */
  cm_get_environment(host_name, sizeof(host_name), expt_name, sizeof(expt_name));

  /* get parameters */
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] == 'd')
      debug = TRUE;
    else if (argv[i][0] == '-' && argv[i][1] == 'D')
      daemon = TRUE;
    else if (argv[i][0] == '-' && argv[i][1] == 'w')
      wheel = TRUE;
    else if (argv[i][0] == '-') {
      if (i + 1 >= argc || argv[i + 1][0] == '-')
        goto usage;
      if (strncmp(argv[i], "-e", 2) == 0)
        strcpy(expt_name, argv[++i]);
      else if (strncmp(argv[i], "-h", 2) == 0)
        strcpy(host_name, argv[++i]);
      else if (strncmp(argv[i], "-b", 2) == 0)
        strcpy(buffer_name, argv[++i]);
    } else {
usage:
      printf("usage: mevb [-h <Hostname>] [-e <Experiment>] -b <buffername> [-d debug]\n");
      printf("             -w show wheel -D to start as a daemon\n\n");
      return 0;
    }
  }

  printf("Program mevb version 5 started\n\n");
  if (daemon) {
    printf("Becoming a daemon...\n");
    ss_daemon_init(FALSE);
  }

  /* Check buffer arg */
  if (buffer_name[0] == 0) {
    printf("Buffer name must be specified with -b argument\n");
    goto exit;
  }

  /* Compose frontend name */
   strcpy(full_frontend_name, frontend_name);

  /* Connect to experiment */
  status = cm_connect_experiment(host_name, expt_name, full_frontend_name, NULL);
  if (status != CM_SUCCESS) {
    ss_sleep(5000);
    goto exit;
  }

  if (debug)
    cm_set_watchdog_params(TRUE, 0);

  /* Connect to ODB */
  status = cm_get_experiment_database(&hDB, &hKey);
  if (status != EB_SUCCESS) {
    ss_sleep(5000);
    goto exit;
  }
  
  /* check if Ebuilder is already running */
  status = cm_exist(full_frontend_name, TRUE);
  if (status == CM_SUCCESS) {
    cm_msg(MERROR, "Ebuilder", "%s running already!.", full_frontend_name);
    cm_disconnect_experiment();
    goto exit;
  }

  if (ebuilder_init() != SUCCESS) {
    cm_disconnect_experiment();
    /* let user read message before window might close */
    ss_sleep(5000);
    goto exit;
  }

  /* Register single equipment */
  status = register_equipment();
  if (status != EB_SUCCESS) {
    ss_sleep(5000);
    goto exit;
  }

  /* Load Fragment info */
  status = load_fragment();
  if (status != EB_SUCCESS) {
    ss_sleep(5000);
    goto exit;
  }

  /* Register transition for reset counters */
  if (cm_register_transition(TR_START, tr_start, 300) != CM_SUCCESS)
    return status;
  if (cm_register_transition(TR_STOP, tr_stop, 700) != CM_SUCCESS)
    goto exit;

  /* Scan fragments... will stay in */
  status = scan_fragment();
  printf("%s-Out of scan_fragment\n", full_frontend_name);

  /* Detach all source from midas */
  printf("%s-Unbooking\n", full_frontend_name);
  source_unbooking();
  
  ebuilder_exit();

exit:
  /* Free local memory */
  free_event_buffer(ebset.nfragment);

  /* Clean disconnect from midas */
  cm_disconnect_experiment();
  return 0;
}
