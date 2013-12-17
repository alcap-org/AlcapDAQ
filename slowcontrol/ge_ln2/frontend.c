#include <stdio.h>
#include <time.h>
#include "midas.h"


/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

INT crate_number=11;

HNDLE hDB;

INT ge_ln2_read(char *pevent, INT off);

/*-- Globals -------------------------------------------------------*/

/* The frontend name (client name) as seen by other MIDAS clients   */
char *frontend_name = "Germanium";
/* The frontend file name, don't change it */
char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = TRUE;

/* a frontend status page is displayed with this frequency in ms    */
INT display_period = 1000;

/* maximum event size produced by this frontend */
INT max_event_size = 10000;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
INT max_event_size_frag = 5 * 1024 * 1024;

/* buffer size to hold events */
INT event_buffer_size = 10 * 10000;

// Globals to this file only
static char* sLastFilled = "/Equipment/Germanium/Settings/Alarm/LastFilled";
static char* sJustFilled = "/Equipment/Germanium/Settings/Alarm/JustFilled";
static char* sTimeLimit  = "/Equipment/Germanium/Settings/Alarm/TimeLimit";
static char* sAlarmName  = "Germanium";
static HNDLE kLastFilled;
static HNDLE kJustFilled;
static HNDLE kTimeLimit;
static HNDLE kAlarm;

/*-- Equipment list ------------------------------------------------*/

BANK_LIST ge_ln2_bank_list[] = {
  { "LN2F", TID_DWORD, 1, NULL },
  { "" },
};

#undef USE_INT

EQUIPMENT equipment[] = {
   {"Germanium",       /* equipment name */
     { 24, 0,              /* event ID, trigger mask */
       "SYSTEM",           /* event buffer */
       EQ_PERIODIC,        /* equipment type */
       0,                  /* event source */
       "MIDAS",            /* format */
       TRUE,               /* enabled */
       RO_ALWAYS | RO_ODB,          /* read all the time */
       1000,              /* reads spaced by this many ms */
       0,                  /* stop run after this event limit */
       0,                  /* number of sub events */
       0,                  /* log history every event */
       "", "", "", },
    ge_ln2_read,     /* readout routine */
    NULL, NULL, 
    ge_ln2_bank_list,
   },

   {""}
};



#ifdef __cplusplus
}
#endif

/*-- Dummy routines ------------------------------------------------*/

INT frontend_early_init()
{
  return CM_SUCCESS;
}

INT pre_begin_of_run()
{
  return CM_SUCCESS;
}

INT poll_event(INT source[], INT count, BOOL test)
{
   return 1;
};
INT interrupt_configure(INT cmd, INT source[], PTYPE adr)
{
   return 1;
};

/*-- Frontend Init -------------------------------------------------*/

INT frontend_init() {

  INT   status;
  BOOL  default_justfilled = FALSE; // Assuming the worst
  DWORD default_lastfilled = 0;     // Assuming the worst
  DWORD default_timelimit  = 28000; // 8 hours between fills
  // Get handles to database keys if they exist,
  // and create otherwise with default worst-case-scenario values
  status = cm_get_experiment_database(&hDB, NULL);
  if (status != CM_SUCCESS) {
    printf("Warning: Could not connect to ODB database!\n");
    return FE_ERR_HW;
  }

  status = db_find_key(hDB, 0, sLastFilled, &kLastFilled);
  if (status == DB_NO_KEY) {
    db_create_key(hDB, 0, sLastFilled, TID_DWORD);
    db_find_key(hDB, 0, sLastFilled, &kLastFilled);
    status = db_set_value(hDB, 0, sLastFilled, &default_lastfilled, sizeof(default_lastfilled), 1, TID_DWORD);
  }
  if (status != DB_SUCCESS) {
    printf("Warning: Could not access key %s!\n", sLastFilled);
    return FE_ERR_HW;
  }

  status = db_find_key(hDB, 0, sJustFilled, &kJustFilled);
  if (status == DB_NO_KEY) {
    db_create_key(hDB, 0, sJustFilled, TID_BOOL);
    db_find_key(hDB, 0, sJustFilled, &kJustFilled);
    status = db_set_value(hDB, 0, sJustFilled, &default_justfilled, sizeof(default_justfilled), 1, TID_BOOL);
  }
  if (status != DB_SUCCESS) {
    printf("Warning: Could not access key %s!\n", sJustFilled);
    return FE_ERR_HW;
  }

  status = db_find_key(hDB, 0, sTimeLimit, &kTimeLimit);
  if (status == DB_NO_KEY) {
    db_create_key(hDB, 0, sTimeLimit, TID_DWORD);
    db_find_key(hDB, 0, sTimeLimit, &kTimeLimit);
    status = db_set_value(hDB, 0, sTimeLimit, &default_timelimit, sizeof(default_timelimit), 1, TID_DWORD);
  }
  if (status != DB_SUCCESS) {
    printf("Warning: Could not access key %s!\n", sTimeLimit);
    return FE_ERR_HW;
  }

  /*
  // Check if alarm exists and, if not,create it
  // Copied from midas.c which should take care of this, but doesn't
  char str[256];
  sprintf(str, "/Alarms/Alarms/%s", sAlarmName);
  db_find_key(hDB, 0, str, &kAlarm);
  if (!kAlarm) {
  ALARM_ODB_STR(alarm_odb_str); // The initial "run number too large" settings for a default alarm
  status = db_create_record(hDB, 0, str, strcomb(alarm_odb_str));
    db_find_key(hDB, 0, str, &kAlarm);
    if (!kAlarm) {
      cm_msg(MERROR, "ge_ln2_init", "Cannot create alarm record");
      return FE_ERR_HW;
    }
    BOOL al_active    = TRUE;                        // Alarm should be on
    INT  al_type      = AT_EVALUATED;                // Alarm type is evaluated; it looks for a certain ODB value to be too large
    char al_cond[256] = "";                          // Not a "conditional" alarm in the MIDAS sense
    char al_class[32] = "Alarm";                     // When triggered, the alarm will have black letters on red background
    char al_msg[80]   = "Germanium must be filled!"; // Message on banner
    db_set_value(hDB, kAlarm, "Active",        &al_active, sizeof(al_active), 1, TID_BOOL);
    db_set_value(hDB, kAlarm, "Type",          &al_type,   sizeof(al_type),   1, TID_INT);
    db_set_value(hDB, kAlarm, "Condition",      al_cond,   sizeof(al_cond),   1, TID_STRING);
    db_set_value(hDB, kAlarm, "Alarm Class",    al_class,  sizeof(al_class),  1, TID_STRING);
    db_set_value(hDB, kAlarm, "Alarm Message",  al_msg,    sizeof(al_msg),    1, TID_STRING);
    
  }
  */
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

INT ge_ln2_read(char *pevent, INT off) {

  static INT    status, size;
  static DWORD  lastfilled, now, timelimit;
  static BOOL   justfilled;
  static DWORD  *timesincefill;

  bk_init(pevent);
  timesincefill = NULL;

  // Get recent values
  size = sizeof(lastfilled);
  status = db_get_value(hDB, 0, sLastFilled, &lastfilled, &size, TID_DWORD, FALSE);
  if (status != DB_SUCCESS) {
    cm_msg(MERROR, "ge_ln2_read", "Error getting last filled time");
    return 0;
  }

  size = sizeof(justfilled);
  status = db_get_value(hDB, 0, sJustFilled, &justfilled, &size, TID_BOOL, FALSE);
  if (status != DB_SUCCESS) {
    cm_msg(MERROR, "ge_ln2_read", "Error getting just filled status");
    return 0;
  }

  size = sizeof(timelimit);
  status = db_get_value(hDB, 0, sTimeLimit, &timelimit, &size, TID_DWORD, FALSE);
  if (status != DB_SUCCESS) {
    cm_msg(MERROR, "ge_ln2_read", "Error getting time limit between fills");
    return 0;
  }

  // If just filled, write time to ODB
  if (justfilled == TRUE) {
    lastfilled = (DWORD)time(NULL);
    status = db_set_value(hDB, 0, sLastFilled, &lastfilled, sizeof(lastfilled), 1, TID_DWORD);
    if (status != DB_SUCCESS) {
      cm_msg(MERROR, "gn_ln2_read", "Error setting last filled time");
      return 0;
    }
    justfilled = FALSE;
    status = db_set_value(hDB, 0, sJustFilled, &justfilled, sizeof(justfilled), 1, TID_BOOL);
    if (status != DB_SUCCESS) {
      cm_msg(MERROR, "gn_ln2_read", "Error setting just filled status");
      return 0;
    }

    al_reset_alarm(sAlarmName);

    bk_create(pevent, "LN2F", TID_DWORD, &timesincefill);
    *timesincefill = 0;
    bk_close(pevent, ++timesincefill);

    return bk_size(pevent);
  }

  // Check the status
  bk_create(pevent, "LN2F", TID_DWORD, &timesincefill);
  now = (DWORD) time(NULL);
  *timesincefill = now - lastfilled;
  if (*timesincefill > timelimit) {
    al_trigger_alarm(sAlarmName, "Germanium must be filled!", "Alarm", "", AT_INTERNAL);
    printf("Alarm!\n");
  }
  bk_close(pevent, ++timesincefill);

  return bk_size(pevent);
}
