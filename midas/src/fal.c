/********************************************************************\

  Name:         fal.c
  Created by:   Stefan Ritt

  Contents:     Combined Frontend/Analyzer/Logger for Windows NT.
                Most routines are from mfe.c mana.c and mlogger.c.

  $Log: fal.c,v $
  Revision 1.2  2005/06/21 00:17:57  mucap
  (Fred)
  Update to compile with gcc 4.0, replacing
                     (char *) pdata += key.item_size * key.num_values;
  with
                     pdata = (char *) pdata + key.item_size * key.num_values;

  Revision 1.43  2004/10/01 23:35:53  midas
  Removed PRE/POST transitions and implemented sequence order of transitions

  Revision 1.42  2004/05/07 19:40:11  midas
  Replaced min/max by MIN/MAX macros

  Revision 1.41  2004/01/18 10:10:58  olchansk
  define "f2cFortran" the same way as in mana.c

  Revision 1.40  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.39  2003/12/12 12:32:13  midas
  Fixed HBOOK compiler warnings

  Revision 1.38  2003/12/01 07:51:04  midas
  Added extra parameter to cm_cleanup()

  Revision 1.37  2003/11/14 13:39:08  midas
  Added auto restart to FAL

  Revision 1.36  2003/05/09 07:40:04  midas
  Added extra parameter to cm_get_environment

  Revision 1.35  2003/04/25 14:37:43  midas
  Fixed compiler warnings

  Revision 1.34  2003/04/14 13:32:02  midas
  Updated register_equipment from mfe.c

  Revision 1.33  2003/02/20 13:46:12  midas
  Fixed bug with RO_ODB

  Revision 1.32  2003/02/20 13:10:45  midas
  Fixed wrong channel records

  Revision 1.31  2002/09/17 22:12:25  pierre
  add arg to cm_cleanup

  Revision 1.30  2002/05/10 05:55:06  pierre
  Added optional debug output to cm_transition

  Revision 1.29  2002/05/10 05:22:05  pierre
  add MANA_LITE #ifdef

  Revision 1.28  2002/05/08 19:54:40  midas
  Added extra parameter to function db_get_value()

  Revision 1.27  2002/02/05 09:46:29  midas
  Set /runinfo/online mode flag

  Revision 1.26  2002/02/05 01:26:51  midas
  Re-merged common code of logger and fal

  Revision 1.25  2002/01/16 15:21:38  midas
  Use odb_size from analyzer

  Revision 1.24  2001/04/10 04:30:38  midas
  Fixed crash when FAL was stopping run in logger part and scaler event had
  zero size.

  Revision 1.23  2001/01/31 08:00:48  midas
  Copied some modifications from mfe.c

  Revision 1.22  2001/01/30 09:13:04  midas
  Correct for increased event size

  Revision 1.21  2000/10/30 10:07:23  midas
  Fixed bug that "always true" test was cleared at the BOR

  Revision 1.20  2000/10/29 09:35:11  midas
  Added test system from mana.c

  Revision 1.19  2000/10/23 14:19:06  midas
  Added idle period for slow control equipment

  Revision 1.18  2000/09/28 13:16:01  midas
  Fixed bug that MANUAL_TRIG only events are not read out during transitions

  Revision 1.17  2000/09/28 13:02:02  midas
  Added manual triggered events

  Revision 1.16  2000/08/10 11:37:27  midas
  Made PAW global memory name variable under /analyzer/output/global memeory name
  to run more than one online analyzer instance on one machine

  Revision 1.15  1999/10/18 14:46:04  midas
  fixed compiler warning

  Revision 1.14  1999/10/18 14:41:50  midas
  Use /programs/<name>/Watchdog timeout in all programs as timeout value. The
  default value can be submitted by calling cm_connect_experiment1(..., timeout)

  Revision 1.13  1999/09/23 12:45:48  midas
  Added 32 bit banks

  Revision 1.12  1999/06/23 09:31:18  midas
  Modified error message

  Revision 1.11  1999/02/22 11:55:20  midas
  Fixed bug with rebooking of N-tuples

  Revision 1.10  1999/02/22 11:04:00  midas
  Fixed second bug with ss_getchar()

  Revision 1.9  1999/02/22 11:01:39  midas
  Fixed bug with ss_getchar()

  Revision 1.8  1999/02/22 08:00:03  midas
  Made FAL stop with "!" work under Linux

  Revision 1.7  1999/02/12 11:50:44  midas
  Fixed bug that analyzer statistics didn't get cleared at the beginning
  of a new run.

  Revision 1.6  1999/01/19 12:42:57  midas
  - equipment is registered before analyzer modules are initialized
  - N-tuples are not booked if buffer size is zero in analyzer_request

  Revision 1.5  1999/01/15 12:55:03  midas
  Set /Analyzer/Bank switches/... to FALSE by default to avoid N-tuple
  overflow when an fal is started the first time

  Revision 1.4  1998/11/09 09:15:12  midas
  Merged new code from mana.c, mlogger.c and mfe.c

  Revision 1.3  1998/10/12 12:19:01  midas
  Added Log tag in header


\********************************************************************/

#include "midas.h"
#include "msystem.h"
#include "hardware.h"
#include "ftplib.h"
#include "mcstd.h"

#define INCLUDE_LOGGING
#include "ybos.h"

/* cernlib includes */
#ifdef OS_WINNT
#define VISUAL_CPLUSPLUS
#endif

#ifdef OS_LINUX
#define f2cFortran
#endif


#ifndef MANA_LITE
#include <cfortran.h>
#include <hbook.h>

/* missing in hbook.h */
#ifndef HFNOV
#define HFNOV(A1,A2)  CCALLSFSUB2(HFNOV,hfnov,INT,FLOATV,A1,A2)
#endif
#endif                          // MANA_LITE
/*---- globals -----------------------------------------------------*/

#define FAL_MAIN

#define LOGGER_TIMEOUT      60000

#define SERVER_CACHE_SIZE  100000       /* event cache before buffer */

#define ODB_UPDATE_TIME      1000       /* 1 seconds for ODB update */

#define MAX_CHANNELS 10
#define MAX_EVENTS   10
#define MAX_HISTORY  20

char *fal_name = "FAL";

BOOL in_stop_transition = FALSE;
BOOL auto_restart = FALSE;
BOOL tape_message = TRUE;

LOG_CHN log_chn[MAX_CHANNELS];

struct {
   WORD event_id;
   void *buffer;
   INT buffer_size;
   HNDLE hKeyVar;
   DWORD period;
   DWORD last_log;
} hist_log[MAX_HISTORY];

INT run_state;                  /* STATE_RUNNING, STATE_STOPPED, STATE_PAUSED */
INT run_number;
DWORD actual_time;              /* current time in seconds since 1970 */
DWORD actual_millitime;         /* current time in milliseconds */
char event_buffer[NET_BUFFER_SIZE];
char host_name[HOST_NAME_LENGTH];
char exp_name[NAME_LENGTH];
HNDLE hDB;

/* output file information, maps to /<analyzer>/Output */
struct {
   char filename[256];
   BOOL rwnt;
   BOOL histo_dump;
   char histo_dump_filename[256];
   BOOL clear_histos;
   char last_histo_filename[256];
   BOOL events_to_odb;
   char global_memory_name[8];
} out_info;

/* paw common memory */

#ifdef extname
int *pawc_;
int quest_[100];
#else
int *PAWC;
extern int QUEST[100];
#endif

char *bstr = " ";

/*---- ODB records -------------------------------------------------*/

CHN_SETTINGS_STR(chn_settings_str);

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

#define ANALYZER_REQUEST_STR "\
Event ID = INT : 0\n\
Trigger mask = INT : -1\n\
Sampling type = INT : 1\n\
Buffer = STRING : [32] SYSTEM\n\
Enabled = BOOL : 1\n\
Client name = STRING : [32] \n\
Host = STRING : [32] \n\
"

#define ANALYZER_STATS_STR "\
Events received = DOUBLE : 0\n\
Events per sec. = DOUBLE : 0\n\
Events written = DOUBLE : 0\n\
"

#define OUT_INFO_STR "\
Filename = STRING : [256] run%05d.asc\n\
RWNT = BOOL : 0\n\
Histo Dump = BOOL : 0\n\
Histo Dump Filename = STRING : [256] his%05d.rz\n\
Clear histos = BOOL : 1\n\
Last Histo Filename = STRING : [256] last.rz\n\
Events to ODB = BOOL : 0\n\
Global Memory Name = STRING : [8] ONLN\n\
"

/*---- data structures for MIDAS format ----------------------------*/

typedef struct {
   char *buffer;
   char *write_pointer;
} MIDAS_INFO;

/*---- declarations ------------------------------------------------*/

/* forward declarations */

void send_event(INT index);
void display(BOOL binit);
void send_all_periodic_events(INT transition);
void receive_event(HNDLE hBuf, HNDLE request_id, EVENT_HEADER * pevent);
INT log_write(LOG_CHN * log_chn, EVENT_HEADER * pevent);
void log_system_history(HNDLE hDB, HNDLE hKey, void *info);
int print_message(const char *msg);
void update_stats();
void interrupt_routine(void);
void interrupt_enable(BOOL flag);
INT tr_start2(run_number, error);

/* items defined in frontend.c */

extern char *frontend_name;
extern char *frontend_file_name;
extern BOOL frontend_call_loop;
extern char *frontend_file_name;
extern INT display_period;
extern INT frontend_init(void);
extern INT frontend_exit(void);
extern INT frontend_loop(void);
extern INT begin_of_run(INT run_number, char *error);
extern INT end_of_run(INT run_number, char *error);
extern INT pause_run(INT run_number, char *error);
extern INT resume_run(INT run_number, char *error);
extern EQUIPMENT equipment[];
extern INT poll_event(INT source, INT count, BOOL test);
extern INT interrupt_configure(INT cmd, INT source, PTYPE adr);

/* items defined in analyzer.c */

extern INT pawc_size;
extern char *analyzer_name;
extern INT analyzer_loop_period;
extern INT analyzer_init(void);
extern INT analyzer_exit(void);
extern INT analyzer_loop(void);
extern INT ana_begin_of_run(INT run_number, char *error);
extern INT ana_end_of_run(INT run_number, char *error);
extern INT ana_pause_run(INT run_number, char *error);
extern INT ana_resume_run(INT run_number, char *error);
extern ANALYZE_REQUEST analyze_request[];
extern INT odb_size;

EQUIPMENT *interrupt_eq = NULL;
EVENT_HEADER *interrupt_odb_buffer;
BOOL interrupt_odb_buffer_valid;

/*---- Logging routines --------------------------------------------*/

/*== common code FAL/MLOGGER start =================================*/

/*---- Logging initialization --------------------------------------*/

void logger_init()
{
   INT size, status;
   BOOL flag;
   HNDLE hKey;
   char str[256];

  /*---- create /logger entries -----*/

   cm_get_path(str);
   size = sizeof(str);
   db_get_value(hDB, 0, "/Logger/Data dir", str, &size, TID_STRING, TRUE);

   strcpy(str, "midas.log");
   size = sizeof(str);
   db_get_value(hDB, 0, "/Logger/Message file", str, &size, TID_STRING, TRUE);

   size = sizeof(BOOL);
   flag = TRUE;
   db_get_value(hDB, 0, "/Logger/Write data", &flag, &size, TID_BOOL, TRUE);

   flag = FALSE;
   db_get_value(hDB, 0, "/Logger/ODB Dump", &flag, &size, TID_BOOL, TRUE);

   strcpy(str, "run%05d.odb");
   size = sizeof(str);
   db_get_value(hDB, 0, "/Logger/ODB Dump File", str, &size, TID_STRING, TRUE);

   flag = FALSE;
   size = sizeof(BOOL);
   db_get_value(hDB, 0, "/Logger/Auto restart", &flag, &size, TID_BOOL, TRUE);

   flag = TRUE;
   db_get_value(hDB, 0, "/Logger/Tape message", &flag, &size, TID_BOOL, TRUE);

   /* create at least one logging channel */
   status = db_find_key(hDB, 0, "/Logger/Channels/0", &hKey);
   if (status != DB_SUCCESS) {
      /* if no channels are defined, define at least one */
      status = db_create_record(hDB, 0, "/Logger/Channels/0", strcomb(chn_settings_str));
      if (status != DB_SUCCESS)
         cm_msg(MERROR, "logger_init", "Cannot create channel entry in database");
   }
}

/*---- ODB dump routine --------------------------------------------*/

void log_odb_dump(LOG_CHN * log_chn, short int event_id, INT run_number)
{
   INT status, buffer_size, size;
   EVENT_HEADER *pevent;

   /* write ODB dump */
   buffer_size = 10000;
   do {
      pevent = malloc(buffer_size);
      if (pevent == NULL) {
         cm_msg(MERROR, "log_odb_dump", "Cannot allocate ODB dump buffer");
         break;
      }

      size = buffer_size - sizeof(EVENT_HEADER);
      status = db_copy(hDB, 0, (char *) (pevent + 1), &size, "");
      if (status != DB_TRUNCATED) {
         bm_compose_event(pevent, event_id, MIDAS_MAGIC,
                          buffer_size - sizeof(EVENT_HEADER) - size + 1, run_number);
         log_write(log_chn, pevent);
         free(pevent);
         break;
      }

      /* increase buffer size if truncated */
      free(pevent);
      buffer_size *= 2;
   } while (1);
}

/*---- ODB save routine --------------------------------------------*/

void odb_save(char *filename)
{
   int size;
   char dir[256];
   char path[256];

   if (strchr(filename, DIR_SEPARATOR) == NULL) {
      size = sizeof(dir);
      dir[0] = 0;
      db_get_value(hDB, 0, "/Logger/Data Dir", dir, &size, TID_STRING, TRUE);
      if (dir[0] != 0)
         if (dir[strlen(dir) - 1] != DIR_SEPARATOR)
            strcat(dir, DIR_SEPARATOR_STR);
      strcpy(path, dir);
      strcat(path, filename);
   } else
      strcpy(path, filename);

   db_save(hDB, 0, path, FALSE);
}

/*---- open tape and check for data --------------------------------*/

INT tape_open(char *dev, INT * handle)
{
   INT status, count;
   char buffer[16];

   status = ss_tape_open(dev, O_RDWR | O_CREAT | O_TRUNC, handle);
   if (status != SS_SUCCESS)
      return status;

   /* check if tape contains data */
   count = sizeof(buffer);
   status = ss_tape_read(*handle, buffer, &count);

   if (count == sizeof(buffer)) {
      /* tape contains data -> don't start */
      ss_tape_rskip(*handle, -1);
      cm_msg(MINFO, "tape_open",
             "Tape contains data, please spool tape with 'mtape seod'");
      cm_msg(MINFO, "tape_open",
             "or erase it with 'mtape weof', 'mtape rewind', then try again.");
      ss_tape_close(*handle);
      return SS_TAPE_ERROR;
   }

   return SS_SUCCESS;
}

/*---- open FTP channel --------------------------------------------*/

INT ftp_error(char *message)
{
   cm_msg(MERROR, "ftp_error", message);
   return 1;
}

INT ftp_open(char *destination, FTP_CON ** con)
{
   INT status;
   short port = 0;
   char *token, host_name[HOST_NAME_LENGTH],
       user[32], pass[32], directory[256], file_name[256];

   /*
      destination should have the form:
      host, port, user, password, directory, run%05d.mid
    */

   /* break destination in components */
   token = strtok(destination, ",");
   if (token)
      strcpy(host_name, token);

   token = strtok(NULL, ", ");
   if (token)
      port = atoi(token);

   token = strtok(NULL, ", ");
   if (token)
      strcpy(user, token);

   token = strtok(NULL, ", ");
   if (token)
      strcpy(pass, token);

   token = strtok(NULL, ", ");
   if (token)
      strcpy(directory, token);

   token = strtok(NULL, ", ");
   if (token)
      strcpy(file_name, token);

#ifdef FAL_MAIN
   ftp_debug(NULL, ftp_error);
#else
   ftp_debug((int (*)(char *)) puts, ftp_error);
#endif

   status = ftp_login(con, host_name, port, user, pass, "");
   if (status >= 0)
      return status;

   status = ftp_chdir(*con, directory);
   if (status >= 0)
      return status;

   status = ftp_binary(*con);
   if (status >= 0)
      return status;

   if (ftp_open_write(*con, file_name) >= 0)
      return (*con)->err_no;

   return SS_SUCCESS;
}

/*---- MIDAS format routines ---------------------------------------*/

INT midas_flush_buffer(LOG_CHN * log_chn)
{
   INT status, size, written;
   MIDAS_INFO *info;

   info = (MIDAS_INFO *) log_chn->format_info;
   size = (INT) info->write_pointer - (INT) info->buffer;

   if (size == 0)
      return SS_SUCCESS;

   /* write record to device */
   if (log_chn->type == LOG_TYPE_TAPE)
      status = ss_tape_write(log_chn->handle, info->buffer, size);
   else if (log_chn->type == LOG_TYPE_FTP)
      status =
          ftp_send(((FTP_CON *) log_chn->ftp_con)->data, info->buffer,
                   size) == size ? SS_SUCCESS : SS_FILE_ERROR;
   else {
#ifdef OS_WINNT
      WriteFile((HANDLE) log_chn->handle, info->buffer, size, &written, NULL);
#else
      written = write(log_chn->handle, info->buffer, size);
#endif
      status = written == size ? SS_SUCCESS : SS_FILE_ERROR;
   }

   info->write_pointer = info->buffer;

   return status;
}


/*------------------------------------------------------------------*/

INT midas_write(LOG_CHN * log_chn, EVENT_HEADER * pevent, INT evt_size)
{
   INT status, size_left;
   MIDAS_INFO *info;

   info = (MIDAS_INFO *) log_chn->format_info;

   /* check if event fits into buffer */
   size_left = TAPE_BUFFER_SIZE - ((INT) info->write_pointer - (INT) info->buffer);

   if (size_left < evt_size) {
      /* copy first part of event */
      memcpy(info->write_pointer, pevent, size_left);
      info->write_pointer += size_left;

      /* flush buffer */
      status = midas_flush_buffer(log_chn);
      if (status != SS_SUCCESS)
         return status;

      /* several writes for large events */
      while (evt_size - size_left >= TAPE_BUFFER_SIZE) {
         memcpy(info->buffer, (char *) pevent + size_left, TAPE_BUFFER_SIZE);
         info->write_pointer += TAPE_BUFFER_SIZE;
         size_left += TAPE_BUFFER_SIZE;

         status = midas_flush_buffer(log_chn);
         if (status != SS_SUCCESS)
            return status;
      }

      /* copy remaining part of event */
      memcpy(info->buffer, (char *) pevent + size_left, evt_size - size_left);
      info->write_pointer = info->buffer + (evt_size - size_left);
   } else {
      /* copy event to buffer */
      memcpy(info->write_pointer, pevent, evt_size);
      info->write_pointer += evt_size;
   }

   /* update statistics */
   log_chn->statistics.events_written++;
   log_chn->statistics.bytes_written += pevent->data_size + sizeof(EVENT_HEADER);
   log_chn->statistics.bytes_written_total += pevent->data_size + sizeof(EVENT_HEADER);

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/

INT midas_log_open(LOG_CHN * log_chn, INT run_number)
{
   MIDAS_INFO *info;
   INT status;

   /* allocate MIDAS buffer info */
   log_chn->format_info = malloc(sizeof(MIDAS_INFO));

   info = (MIDAS_INFO *) log_chn->format_info;
   if (info == NULL) {
      log_chn->handle = 0;
      return SS_NO_MEMORY;
   }

   /* allocate full ring buffer for that channel */
   if ((info->buffer = (char *) malloc(TAPE_BUFFER_SIZE)) == NULL) {
      free(info);
      log_chn->handle = 0;
      return SS_NO_MEMORY;
   }

   info->write_pointer = info->buffer;

   /* Create device channel */
   if (log_chn->type == LOG_TYPE_TAPE) {
      status = tape_open(log_chn->path, &log_chn->handle);
      if (status != SS_SUCCESS) {
         free(info->buffer);
         free(info);
         log_chn->handle = 0;
         return status;
      }
   } else if (log_chn->type == LOG_TYPE_FTP) {
      status = ftp_open(log_chn->path, (FTP_CON **) (&log_chn->ftp_con));
      if (status != SS_SUCCESS) {
         free(info->buffer);
         free(info);
         log_chn->handle = 0;
         return status;
      } else
         log_chn->handle = 1;
   } else {
      /* check if file exists */
      if (strstr(log_chn->path, "null") == NULL) {
         log_chn->handle = open(log_chn->path, O_RDONLY);
         if (log_chn->handle > 0) {
            /* check if file length is nonzero */
            if (lseek(log_chn->handle, 0, SEEK_END) > 0) {
               close(log_chn->handle);
               free(info->buffer);
               free(info);
               log_chn->handle = 0;
               return SS_FILE_EXISTS;
            }
         }
      }
#ifdef OS_WINNT
      log_chn->handle =
          (int) CreateFile(log_chn->path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                           CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH |
                           FILE_FLAG_SEQUENTIAL_SCAN, 0);

#else
      log_chn->handle =
          open(log_chn->path, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0644);
#endif

      if (log_chn->handle < 0) {
         free(info->buffer);
         free(info);
         log_chn->handle = 0;
         return SS_FILE_ERROR;
      }
   }

   /* write ODB dump */
   if (log_chn->settings.odb_dump)
      log_odb_dump(log_chn, EVENTID_BOR, run_number);

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/

INT midas_log_close(LOG_CHN * log_chn, INT run_number)
{
   /* write ODB dump */
   if (log_chn->settings.odb_dump)
      log_odb_dump(log_chn, EVENTID_EOR, run_number);

   midas_flush_buffer(log_chn);

   /* Write EOF if Tape */
   if (log_chn->type == LOG_TYPE_TAPE) {
      /* writing EOF mark on tape Fonly */
      ss_tape_write_eof(log_chn->handle);
      ss_tape_close(log_chn->handle);
   } else if (log_chn->type == LOG_TYPE_FTP) {
      ftp_close((FTP_CON *) log_chn->ftp_con);
      ftp_bye((FTP_CON *) log_chn->ftp_con);
   } else
#ifdef OS_WINNT
      CloseHandle((HANDLE) log_chn->handle);
#else
      close(log_chn->handle);
#endif

   free(((MIDAS_INFO *) log_chn->format_info)->buffer);
   free(log_chn->format_info);

   return SS_SUCCESS;
}

/*-- db_get_event_definition ---------------------------------------*/

typedef struct {
   short int event_id;
   WORD format;
   HNDLE hDefKey;
} EVENT_DEF;

EVENT_DEF *db_get_event_definition(short int event_id)
{
   INT i, index, status, size;
   char str[80];
   HNDLE hKey, hKeyRoot;
   WORD id;

#define EVENT_DEF_CACHE_SIZE 30
   static EVENT_DEF *event_def = NULL;

   /* allocate memory for cache */
   if (event_def == NULL)
      event_def = calloc(EVENT_DEF_CACHE_SIZE, sizeof(EVENT_DEF));

   /* lookup if event definition in cache */
   for (i = 0; event_def[i].event_id; i++)
      if (event_def[i].event_id == event_id)
         return &event_def[i];

   /* search free cache entry */
   for (index = 0; index < EVENT_DEF_CACHE_SIZE; index++)
      if (event_def[index].event_id == 0)
         break;

   if (index == EVENT_DEF_CACHE_SIZE) {
      cm_msg(MERROR, "db_get_event_definition", "too many event definitions");
      return NULL;
   }

   /* check for system events */
   if (event_id < 0) {
      event_def[index].event_id = event_id;
      event_def[index].format = FORMAT_ASCII;
      event_def[index].hDefKey = 0;
      return &event_def[index];
   }

   status = db_find_key(hDB, 0, "/equipment", &hKeyRoot);
   if (status != DB_SUCCESS) {
      cm_msg(MERROR, "db_get_event_definition", "cannot find /equipment entry in ODB");
      return NULL;
   }

   for (i = 0;; i++) {
      /* search for client with specific name */
      status = db_enum_key(hDB, hKeyRoot, i, &hKey);
      if (status == DB_NO_MORE_SUBKEYS) {
         sprintf(str, "Cannot find event id %d under /equipment", event_id);
         cm_msg(MERROR, "db_get_event_definition", str);
         return NULL;
      }

      size = sizeof(id);
      status = db_get_value(hDB, hKey, "Common/Event ID", &id, &size, TID_WORD, TRUE);
      if (status != DB_SUCCESS)
         continue;

      if (id == event_id) {
         /* set cache entry */
         event_def[index].event_id = id;

         size = sizeof(str);
         str[0] = 0;
         db_get_value(hDB, hKey, "Common/Format", str, &size, TID_STRING, TRUE);

         if (equal_ustring(str, "Fixed"))
            event_def[index].format = FORMAT_FIXED;
         else if (equal_ustring(str, "ASCII"))
            event_def[index].format = FORMAT_ASCII;
         else if (equal_ustring(str, "MIDAS"))
            event_def[index].format = FORMAT_MIDAS;
         else if (equal_ustring(str, "YBOS"))
            event_def[index].format = FORMAT_YBOS;
         else if (equal_ustring(str, "DUMP"))
            event_def[index].format = FORMAT_DUMP;
         else {
            cm_msg(MERROR, "db_get_event_definition", "unknown data format");
            event_def[index].event_id = 0;
            return NULL;
         }

         db_find_key(hDB, hKey, "Variables", &event_def[index].hDefKey);
         return &event_def[index];
      }
   }
}

/*---- DUMP format routines ----------------------------------------*/

#define STR_INC(p,base) { p+=strlen(p); \
                          if (p > base+sizeof(base)) \
                            cm_msg(MERROR, "STR_INC", "ASCII buffer too small"); }


INT dump_write(LOG_CHN * log_chn, EVENT_HEADER * pevent, INT evt_size)
{
   INT status, size, i, j;
   EVENT_DEF *event_def;
   BANK_HEADER *pbh;
   BANK *pbk;
   BANK32 *pbk32;
   void *pdata;
   char buffer[100000], *pbuf, name[5], type_name[10];
   HNDLE hKey;
   KEY key;
   DWORD bkname;
   WORD bktype;

   event_def = db_get_event_definition(pevent->event_id);
   if (event_def == NULL)
      return SS_SUCCESS;

   /* write event header */
   pbuf = buffer;
   name[4] = 0;

   if (pevent->event_id == EVENTID_BOR)
      sprintf(pbuf, "%%ID BOR NR %ld\n", pevent->serial_number);
   else if (pevent->event_id == EVENTID_EOR)
      sprintf(pbuf, "%%ID EOR NR %ld\n", pevent->serial_number);
   else
      sprintf(pbuf, "%%ID %d TR %d NR %ld\n", pevent->event_id, pevent->trigger_mask,
              pevent->serial_number);
   STR_INC(pbuf, buffer);

  /*---- MIDAS format ----------------------------------------------*/
   if (event_def->format == FORMAT_MIDAS) {
      LRS1882_DATA *lrs1882;
      LRS1877_DATA *lrs1877;
      LRS1877_HEADER *lrs1877_header;

      pbh = (BANK_HEADER *) (pevent + 1);
      bk_swap(pbh, FALSE);

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

         if (rpc_tid_size(bktype & 0xFF))
            size /= rpc_tid_size(bktype & 0xFF);

         lrs1882 = (LRS1882_DATA *) pdata;
         lrs1877 = (LRS1877_DATA *) pdata;
         lrs1877_header = (LRS1877_HEADER *) pdata;

         /* write bank header */
         *((DWORD *) name) = bkname;

         if ((bktype & 0xFF00) == 0)
            strcpy(type_name, rpc_tid_name(bktype & 0xFF));
         else if ((bktype & 0xFF00) == TID_LRS1882)
            strcpy(type_name, "LRS1882");
         else if ((bktype & 0xFF00) == TID_LRS1877)
            strcpy(type_name, "LRS1877");
         else if ((bktype & 0xFF00) == TID_PCOS3)
            strcpy(type_name, "PCOS3");
         else
            strcpy(type_name, "unknown");

         sprintf(pbuf, "BK %s TP %s SZ %d\n", name, type_name, size);
         STR_INC(pbuf, buffer);

         /* write data */
         for (i = 0; i < size; i++) {
            if ((bktype & 0xFF00) == 0)
               db_sprintf(pbuf, pdata, size, i, bktype & 0xFF);

            else if ((bktype & 0xFF00) == TID_LRS1882)
               sprintf(pbuf, "GA %d CH %02d DA %d",
                       lrs1882[i].geo_addr, lrs1882[i].channel, lrs1882[i].data);

            else if ((bktype & 0xFF00) == TID_LRS1877) {
               if (i == 0)      /* header */
                  sprintf(pbuf, "GA %d BF %d CN %d",
                          lrs1877_header[i].geo_addr, lrs1877_header[i].buffer,
                          lrs1877_header[i].count);
               else             /* data */
                  sprintf(pbuf, "GA %d CH %02d ED %d DA %1.1lf",
                          lrs1877[i].geo_addr, lrs1877[i].channel, lrs1877[i].edge,
                          lrs1877[i].data * 0.5);
            }

            else if ((bktype & 0xFF00) == TID_PCOS3)
               sprintf(pbuf, " ");
            else
               db_sprintf(pbuf, pdata, size, i, bktype & 0xFF);

            strcat(pbuf, "\n");
            STR_INC(pbuf, buffer);
         }

      } while (1);
   }

  /*---- FIXED format ----------------------------------------------*/
   if (event_def->format == FORMAT_FIXED) {
      if (event_def->hDefKey == 0)
         cm_msg(MERROR, "dump_write", "cannot find event definition");
      else {
         pdata = (char *) (pevent + 1);
         for (i = 0;; i++) {
            status = db_enum_key(hDB, event_def->hDefKey, i, &hKey);
            if (status == DB_NO_MORE_SUBKEYS)
               break;

            db_get_key(hDB, hKey, &key);
            sprintf(pbuf, "%s\n", key.name);
            STR_INC(pbuf, buffer);

            /* adjust for alignment */
            pdata = (void *) VALIGN(pdata, MIN(ss_get_struct_align(), key.item_size));

            for (j = 0; j < key.num_values; j++) {
               db_sprintf(pbuf, pdata, key.item_size, j, key.type);
               strcat(pbuf, "\n");
               STR_INC(pbuf, buffer);
            }

            /* shift data pointer to next item */
            pdata = (char *) pdata + key.item_size * key.num_values;
         }
      }
   }

  /*---- ASCII format ----------------------------------------------*/
   if (event_def->format == FORMAT_ASCII) {
      /* write event header to device */
      size = strlen(buffer);
      if (log_chn->type == LOG_TYPE_TAPE)
         status = ss_tape_write(log_chn->handle, buffer, size);
      else
         status = write(log_chn->handle, buffer, size) ==
             size ? SS_SUCCESS : SS_FILE_ERROR;

      /* write event directly to device */
      size = strlen((char *) (pevent + 1));
      if (log_chn->type == LOG_TYPE_TAPE)
         status = ss_tape_write(log_chn->handle, (char *) (pevent + 1), size);
      else if (log_chn->type == LOG_TYPE_FTP)
         status = ftp_send(((FTP_CON *) log_chn->ftp_con)->data, buffer, size) == size ?
             SS_SUCCESS : SS_FILE_ERROR;
      else
         status = write(log_chn->handle, (char *) (pevent + 1), size) ==
             size ? SS_SUCCESS : SS_FILE_ERROR;
   } else {
      /* non-ascii format: only write buffer */

      /* insert empty line after each event */
      strcat(pbuf, "\n");
      size = strlen(buffer);

      /* write record to device */
      if (log_chn->type == LOG_TYPE_TAPE)
         status = ss_tape_write(log_chn->handle, buffer, size);
      else if (log_chn->type == LOG_TYPE_FTP)
         status = ftp_send(((FTP_CON *) log_chn->ftp_con)->data, buffer, size) == size ?
             SS_SUCCESS : SS_FILE_ERROR;
      else
         status = write(log_chn->handle, buffer, size) ==
             size ? SS_SUCCESS : SS_FILE_ERROR;
   }

   /* update statistics */
   log_chn->statistics.events_written++;
   log_chn->statistics.bytes_written += size;
   log_chn->statistics.bytes_written_total += size;

   return status;
}

/*------------------------------------------------------------------*/

INT dump_log_open(LOG_CHN * log_chn, INT run_number)
{
   INT status;

   /* Create device channel */
   if (log_chn->type == LOG_TYPE_TAPE) {
      status = tape_open(log_chn->path, &log_chn->handle);
      if (status != SS_SUCCESS) {
         log_chn->handle = 0;
         return status;
      }
   } else if (log_chn->type == LOG_TYPE_FTP) {
      status = ftp_open(log_chn->path, (FTP_CON **) (&log_chn->ftp_con));
      if (status != SS_SUCCESS) {
         log_chn->handle = 0;
         return status;
      } else
         log_chn->handle = 1;
   } else {
      log_chn->handle = open(log_chn->path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

      if (log_chn->handle < 0) {
         log_chn->handle = 0;
         return SS_FILE_ERROR;
      }
   }

   /* write ODB dump */
   if (log_chn->settings.odb_dump)
      log_odb_dump(log_chn, EVENTID_BOR, run_number);

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/

INT dump_log_close(LOG_CHN * log_chn, INT run_number)
{
   /* write ODB dump */
   if (log_chn->settings.odb_dump)
      log_odb_dump(log_chn, EVENTID_EOR, run_number);

   /* Write EOF if Tape */
   if (log_chn->type == LOG_TYPE_TAPE) {
      /* writing EOF mark on tape only */
      ss_tape_write_eof(log_chn->handle);
      ss_tape_close(log_chn->handle);
   } else if (log_chn->type == LOG_TYPE_FTP) {
      ftp_close((FTP_CON *) log_chn->ftp_con);
      ftp_bye((FTP_CON *) log_chn->ftp_con);
   } else
      close(log_chn->handle);

   return SS_SUCCESS;
}

/*---- ASCII format routines ---------------------------------------*/

INT ascii_write(LOG_CHN * log_chn, EVENT_HEADER * pevent, INT evt_size)
{
   INT status, size, i, j;
   EVENT_DEF *event_def;
   BANK_HEADER *pbh;
   BANK *pbk;
   BANK32 *pbk32;
   void *pdata;
   char data[1000];
   char buffer[10000], name[5], type_name[10];
   char *ph, header_line[10000];
   char *pd, data_line[10000];
   HNDLE hKey, hKeyRoot;
   KEY key;
   static short int last_event_id = -1;
   DWORD bkname;
   WORD bktype;

   if (pevent->serial_number == 1)
      last_event_id = -1;

   event_def = db_get_event_definition(pevent->event_id);
   if (event_def == NULL)
      return SS_SUCCESS;

   name[4] = 0;
   header_line[0] = 0;
   data_line[0] = 0;
   ph = header_line;
   pd = data_line;

   /* write run parameter at begin of run */
   if (pevent->event_id == EVENTID_BOR) {
      sprintf(header_line, "%%Run\t");
      sprintf(data_line, "%ld\t", pevent->serial_number);
      STR_INC(ph, header_line);
      STR_INC(pd, data_line);

      status = db_find_key(hDB, 0, "/Experiment/Run parameters", &hKeyRoot);
      if (status == DB_SUCCESS) {
         for (i = 0;; i++) {
            status = db_enum_key(hDB, hKeyRoot, i, &hKey);
            if (status == DB_NO_MORE_SUBKEYS)
               break;

            db_get_key(hDB, hKey, &key);
            size = sizeof(data);
            db_get_data(hDB, hKey, data, &size, key.type);

            for (j = 0; j < key.num_values; j++) {
               if (key.num_values == 1)
                  sprintf(ph, "%s\t", key.name);
               else
                  sprintf(ph, "%s%d\t", key.name, j);

               STR_INC(ph, header_line);

               db_sprintf(pd, data, key.item_size, j, key.type);
               strcat(pd, "\t");
               STR_INC(pd, data_line);
            }
         }
      }
   } else {
      sprintf(ph, "%d", pevent->event_id);
      STR_INC(ph, header_line);
   }

  /*---- MIDAS format ----------------------------------------------*/
   if (event_def->format == FORMAT_MIDAS) {
      LRS1882_DATA *lrs1882;
      LRS1877_DATA *lrs1877;
      LRS1877_HEADER *lrs1877_header;

      pbh = (BANK_HEADER *) (pevent + 1);
      bk_swap(pbh, FALSE);

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

         if (rpc_tid_size(bktype & 0xFF))
            size /= rpc_tid_size(bktype & 0xFF);

         lrs1882 = (LRS1882_DATA *) pdata;
         lrs1877 = (LRS1877_DATA *) pdata;
         lrs1877_header = (LRS1877_HEADER *) pdata;

         /* write bank header */
         *((DWORD *) name) = bkname;

         if ((bktype & 0xFF00) == 0)
            strcpy(type_name, rpc_tid_name(bktype & 0xFF));
         else if ((bktype & 0xFF00) == TID_LRS1882)
            strcpy(type_name, "LRS1882");
         else if ((bktype & 0xFF00) == TID_LRS1877)
            strcpy(type_name, "LRS1877");
         else if ((bktype & 0xFF00) == TID_PCOS3)
            strcpy(type_name, "PCOS3");
         else
            strcpy(type_name, "unknown");

         sprintf(ph, "%s[%d]\t", name, size);
         STR_INC(ph, header_line);

         /* write data */
         for (i = 0; i < size; i++) {
            db_sprintf(pd, pdata, size, i, bktype & 0xFF);
            strcat(pd, "\t");
            STR_INC(pd, data_line);
         }

      } while (1);
   }

  /*---- FIXED format ----------------------------------------------*/
   if (event_def->format == FORMAT_FIXED) {
      if (event_def->hDefKey == 0)
         cm_msg(MERROR, "ascii_write", "cannot find event definition");
      else {
         pdata = (char *) (pevent + 1);
         for (i = 0;; i++) {
            status = db_enum_key(hDB, event_def->hDefKey, i, &hKey);
            if (status == DB_NO_MORE_SUBKEYS)
               break;

            db_get_key(hDB, hKey, &key);

            /* adjust for alignment */
            pdata = (void *) VALIGN(pdata, MIN(ss_get_struct_align(), key.item_size));

            for (j = 0; j < key.num_values; j++) {
               if (pevent->event_id != last_event_id) {
                  if (key.num_values == 1)
                     sprintf(ph, "%s\t", key.name);
                  else
                     sprintf(ph, "%s%d\t", key.name, j);

                  STR_INC(ph, header_line);
               }

               db_sprintf(pd, pdata, key.item_size, j, key.type);
               strcat(pd, "\t");
               STR_INC(pd, data_line);
            }

            /* shift data pointer to next item */
            pdata = (char *) pdata + key.item_size * key.num_values;
         }
      }
   }

   if (*(pd - 1) == '\t')
      *(pd - 1) = '\n';

   if (last_event_id != pevent->event_id) {
      if (*(ph - 1) == '\t')
         *(ph - 1) = '\n';
      last_event_id = pevent->event_id;
      strcpy(buffer, header_line);
      strcat(buffer, data_line);
   } else
      strcpy(buffer, data_line);

   /* write buffer to device */
   size = strlen(buffer);

   if (log_chn->type == LOG_TYPE_TAPE)
      status = ss_tape_write(log_chn->handle, buffer, size);
   else if (log_chn->type == LOG_TYPE_FTP)
      status = ftp_send(((FTP_CON *) log_chn->ftp_con)->data, buffer, size) == size ?
          SS_SUCCESS : SS_FILE_ERROR;
   else
      status = write(log_chn->handle, buffer, size) == size ? SS_SUCCESS : SS_FILE_ERROR;

   /* update statistics */
   log_chn->statistics.events_written++;
   log_chn->statistics.bytes_written += size;
   log_chn->statistics.bytes_written_total += size;

   return status;
}

/*------------------------------------------------------------------*/

INT ascii_log_open(LOG_CHN * log_chn, INT run_number)
{
   INT status;
   EVENT_HEADER event;

   /* Create device channel */
   if (log_chn->type == LOG_TYPE_TAPE) {
      status = tape_open(log_chn->path, &log_chn->handle);
      if (status != SS_SUCCESS) {
         log_chn->handle = 0;
         return status;
      }
   } else if (log_chn->type == LOG_TYPE_FTP) {
      status = ftp_open(log_chn->path, (FTP_CON **) (&log_chn->ftp_con));
      if (status != SS_SUCCESS) {
         log_chn->handle = 0;
         return status;
      } else
         log_chn->handle = 1;
   } else {
      log_chn->handle = open(log_chn->path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

      if (log_chn->handle < 0) {
         log_chn->handle = 0;
         return SS_FILE_ERROR;
      }
   }

   /* write ODB dump */
   if (log_chn->settings.odb_dump) {
      event.event_id = EVENTID_BOR;
      event.data_size = 0;
      event.serial_number = run_number;

      ascii_write(log_chn, &event, sizeof(EVENT_HEADER));
   }

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/

INT ascii_log_close(LOG_CHN * log_chn, INT run_number)
{
   /* Write EOF if Tape */
   if (log_chn->type == LOG_TYPE_TAPE) {
      /* writing EOF mark on tape only */
      ss_tape_write_eof(log_chn->handle);
      ss_tape_close(log_chn->handle);
   } else if (log_chn->type == LOG_TYPE_FTP) {
      ftp_close((FTP_CON *) log_chn->ftp_con);
      ftp_bye((FTP_CON *) log_chn->ftp_con);
   } else
      close(log_chn->handle);

   return SS_SUCCESS;
}

/*---- log_open ----------------------------------------------------*/

INT log_open(LOG_CHN * log_chn, INT run_number)
{
   INT status;

   if (equal_ustring(log_chn->settings.format, "YBOS")) {
      log_chn->format = FORMAT_YBOS;
      status = ybos_log_open(log_chn, run_number);
   } else if (equal_ustring(log_chn->settings.format, "ASCII")) {
      log_chn->format = FORMAT_ASCII;
      status = ascii_log_open(log_chn, run_number);
   } else if (equal_ustring(log_chn->settings.format, "DUMP")) {
      log_chn->format = FORMAT_DUMP;
      status = dump_log_open(log_chn, run_number);
   } else {                     /* default format is MIDAS */

      log_chn->format = FORMAT_MIDAS;
      status = midas_log_open(log_chn, run_number);
   }

   return status;
}

/*---- log_close ---------------------------------------------------*/

INT log_close(LOG_CHN * log_chn, INT run_number)
{
   if (log_chn->format == FORMAT_YBOS)
      ybos_log_close(log_chn, run_number);

   if (log_chn->format == FORMAT_ASCII)
      ascii_log_close(log_chn, run_number);

   if (log_chn->format == FORMAT_DUMP)
      dump_log_close(log_chn, run_number);

   if (log_chn->format == FORMAT_MIDAS)
      midas_log_close(log_chn, run_number);

   log_chn->statistics.files_written++;
   log_chn->handle = 0;
   log_chn->ftp_con = NULL;

   return SS_SUCCESS;
}

/*---- log_write ---------------------------------------------------*/

INT log_write(LOG_CHN * log_chn, EVENT_HEADER * pevent)
{
   INT status = 0, size, izero;
   DWORD actual_time, start_time, watchdog_timeout;
   BOOL watchdog_flag, flag;
   static BOOL stop_requested = FALSE;
   static DWORD last_checked = 0;
   HNDLE htape, stats_hkey;
   char tape_name[256];
   double dzero;

   start_time = ss_millitime();

   if (log_chn->format == FORMAT_YBOS)
      status = ybos_write(log_chn, pevent, pevent->data_size + sizeof(EVENT_HEADER));

   if (log_chn->format == FORMAT_ASCII)
      status = ascii_write(log_chn, pevent, pevent->data_size + sizeof(EVENT_HEADER));

   if (log_chn->format == FORMAT_DUMP)
      status = dump_write(log_chn, pevent, pevent->data_size + sizeof(EVENT_HEADER));

   if (log_chn->format == FORMAT_MIDAS)
      status = midas_write(log_chn, pevent, pevent->data_size + sizeof(EVENT_HEADER));

   actual_time = ss_millitime();
   if ((int) actual_time - (int) start_time > 3000)
      cm_msg(MINFO, "log_write", "Write operation took %d ms", actual_time - start_time);

   if (status != SS_SUCCESS && !stop_requested) {
      if (status == SS_IO_ERROR)
         cm_msg(MTALK, "log_write", "Physical IO error on %s, stopping run",
                log_chn->path);
      else
         cm_msg(MTALK, "log_write", "Error writing to %s, stopping run", log_chn->path);

      stop_requested = TRUE;
      cm_transition(TR_STOP, 0, NULL, 0, ASYNC, FALSE);
      stop_requested = FALSE;

      return status;
   }

   /* check if event limit is reached */
   if (!stop_requested && !in_stop_transition &&
       log_chn->settings.event_limit > 0 &&
       log_chn->statistics.events_written >= log_chn->settings.event_limit) {
      stop_requested = TRUE;

      cm_msg(MTALK, "log_write", "stopping run after having received %d events",
             log_chn->settings.event_limit);

      status = cm_transition(TR_STOP, 0, NULL, 0, ASYNC, FALSE);
      if (status != CM_SUCCESS)
         cm_msg(MERROR, "log_write", "cannot stop run after reaching event limit");
      stop_requested = FALSE;

      /* check if autorestart, main loop will take care of it */
      size = sizeof(BOOL);
      flag = FALSE;
      db_get_value(hDB, 0, "/Logger/Auto restart", &flag, &size, TID_BOOL, TRUE);

      if (flag)
         auto_restart = ss_time() + 20; /* start in 20 sec. */

      return status;
   }

   /* check if byte limit is reached */
   if (!stop_requested && !in_stop_transition &&
       log_chn->settings.byte_limit > 0 &&
       log_chn->statistics.bytes_written >= log_chn->settings.byte_limit) {
      stop_requested = TRUE;

      cm_msg(MTALK, "log_write", "stopping run after having received %1.0lf mega bytes",
             log_chn->statistics.bytes_written / 1E6);

      status = cm_transition(TR_STOP, 0, NULL, 0, ASYNC, FALSE);
      if (status != CM_SUCCESS)
         cm_msg(MERROR, "log_write", "cannot stop run after reaching bytes limit");
      stop_requested = FALSE;

      /* check if autorestart, main loop will take care of it */
      size = sizeof(BOOL);
      flag = FALSE;
      db_get_value(hDB, 0, "/Logger/Auto restart", &flag, &size, TID_BOOL, TRUE);

      if (flag)
         auto_restart = ss_time() + 20; /* start in 20 sec. */

      return status;
   }

   /* check if capacity is reached for tapes */
   if (!stop_requested && !in_stop_transition &&
       log_chn->type == LOG_TYPE_TAPE &&
       log_chn->settings.tape_capacity > 0 &&
       log_chn->statistics.bytes_written_total >= log_chn->settings.tape_capacity) {
      stop_requested = TRUE;
      cm_msg(MTALK, "log_write", "tape capacity reached, stopping run");

      /* remember tape name */
      strcpy(tape_name, log_chn->path);
      stats_hkey = log_chn->stats_hkey;

      status = cm_transition(TR_STOP, 0, NULL, 0, ASYNC, FALSE);
      if (status != CM_SUCCESS)
         cm_msg(MERROR, "log_write", "cannot stop run after reaching tape capacity");
      stop_requested = FALSE;

      /* rewind tape */
      ss_tape_open(tape_name, O_RDONLY, &htape);
      cm_msg(MTALK, "log_write", "rewinding tape %s, please wait", log_chn->path);

      cm_get_watchdog_params(&watchdog_flag, &watchdog_timeout);
      cm_set_watchdog_params(watchdog_flag, 300000);    /* 5 min for tape rewind */
      ss_tape_unmount(htape);
      ss_tape_close(htape);
      cm_set_watchdog_params(watchdog_flag, watchdog_timeout);

      /* zero statistics */
      dzero = izero = 0;
      db_set_value(hDB, stats_hkey, "Bytes written total", &dzero,
                   sizeof(dzero), 1, TID_DOUBLE);
      db_set_value(hDB, stats_hkey, "Files written", &izero, sizeof(izero), 1, TID_INT);

      cm_msg(MTALK, "log_write", "Please insert new tape and start new run.");

      return status;
   }

   /* stop run if less than 10MB free disk space */
   actual_time = ss_millitime();
   if (log_chn->type == LOG_TYPE_DISK && actual_time - last_checked > 1000) {
      last_checked = actual_time;

      if (ss_disk_free(log_chn->path) < 1E7 && !stop_requested && !in_stop_transition) {
         stop_requested = TRUE;
         cm_msg(MTALK, "log_write", "disk nearly full, stopping run");

         status = cm_transition(TR_STOP, 0, NULL, 0, ASYNC, FALSE);
         if (status != CM_SUCCESS)
            cm_msg(MERROR, "log_write", "cannot stop run after reaching byte limit");
         stop_requested = FALSE;
      }
   }

   return status;
}

/*---- open_history ------------------------------------------------*/

void log_history(HNDLE hDB, HNDLE hKey, void *info);

INT open_history()
{
   INT size, index, i_tag, status, i, j, li, max_event_id;
   INT n_var, n_tags, n_names = 0;
   HNDLE hKeyRoot, hKeyVar, hKeyNames, hLinkKey, hVarKey, hKeyEq, hHistKey, hKey;
   DWORD history;
   TAG *tag;
   KEY key, varkey, linkkey, histkey;
   WORD event_id;
   char str[256], eq_name[NAME_LENGTH], hist_name[NAME_LENGTH];
   BOOL single_names;

   /* set direcotry for history files */
   size = sizeof(str);
   str[0] = 0;
   status = db_get_value(hDB, 0, "/Logger/History Dir", str, &size, TID_STRING, FALSE);
   if (status != DB_SUCCESS)
      db_get_value(hDB, 0, "/Logger/Data Dir", str, &size, TID_STRING, TRUE);

   if (str[0] != 0)
      hs_set_path(str);

   if (db_find_key(hDB, 0, "/History/Links", &hKeyRoot) != DB_SUCCESS) {
      /* create default history keys */
      db_create_key(hDB, 0, "/History/Links", TID_KEY);

      if (db_find_key(hDB, 0, "/Equipment/Trigger/Statistics/Events per sec.", &hKeyEq) ==
          DB_SUCCESS)
         db_create_link(hDB, 0, "/History/Links/System/Trigger per sec.",
                        "/Equipment/Trigger/Statistics/Events per sec.");

      if (db_find_key(hDB, 0, "/Equipment/Trigger/Statistics/kBytes per sec.", &hKeyEq) ==
          DB_SUCCESS)
         db_create_link(hDB, 0, "/History/Links/System/Trigger kB per sec.",
                        "/Equipment/Trigger/Statistics/kBytes per sec.");
   }


  /*---- define equipment events as history ------------------------*/

   max_event_id = 0;

   status = db_find_key(hDB, 0, "/Equipment", &hKeyRoot);
   if (status != DB_SUCCESS) {
      cm_msg(MERROR, "open_history", "Cannot find Equipment entry in database");
      return 0;
   }

   /* loop over equipment */
   for (index = 0; index < MAX_HISTORY; index++) {
      status = db_enum_key(hDB, hKeyRoot, index, &hKeyEq);
      if (status != DB_SUCCESS)
         break;

      /* check history flag */
      size = sizeof(history);
      db_get_value(hDB, hKeyEq, "Common/Log history", &history, &size, TID_INT, TRUE);

      /* define history tags only if log history flag is on */
      if (history > 0) {
         /* get equipment name */
         db_get_key(hDB, hKeyEq, &key);
         strcpy(eq_name, key.name);

         status = db_find_key(hDB, hKeyEq, "Variables", &hKeyVar);
         if (status != DB_SUCCESS) {
            cm_msg(MERROR, "open_history",
                   "Cannot find /Equipment/%s/Variables entry in database", eq_name);
            return 0;
         }

         size = sizeof(event_id);
         db_get_value(hDB, hKeyEq, "Common/Event ID", &event_id, &size, TID_WORD, TRUE);

         /* count keys in variables tree */
         for (n_var = 0, n_tags = 0;; n_var++) {
            status = db_enum_key(hDB, hKeyVar, n_var, &hKey);
            if (status == DB_NO_MORE_SUBKEYS)
               break;
            db_get_key(hDB, hKey, &key);
            n_tags += key.num_values;
         }

         if (n_var == 0)
            cm_msg(MERROR, "open_history", "defined event %d with no variables in ODB",
                   event_id);

         /* create tag array */
         tag = malloc(sizeof(TAG) * n_tags);

         for (i = 0, i_tag = 0;; i++) {
            status = db_enum_key(hDB, hKeyVar, i, &hKey);
            if (status == DB_NO_MORE_SUBKEYS)
               break;

            /* get variable key */
            db_get_key(hDB, hKey, &varkey);

            /* look for names */
            db_find_key(hDB, hKeyEq, "Settings/Names", &hKeyNames);
            single_names = (hKeyNames > 0);
            if (hKeyNames) {
               /* define tags from names list */
               db_get_key(hDB, hKeyNames, &key);
               n_names = key.num_values;
            } else {
               sprintf(str, "Settings/Names %s", varkey.name);
               db_find_key(hDB, hKeyEq, str, &hKeyNames);
               if (hKeyNames) {
                  /* define tags from names list */
                  db_get_key(hDB, hKeyNames, &key);
                  n_names = key.num_values;
               }
            }

            if (hKeyNames && varkey.num_values > n_names)
               cm_msg(MERROR, "open_history",
                      "/Equipment/%s/Settings/%s contains only %d entries", eq_name,
                      key.name, n_names);

            if (hKeyNames) {
               /* loop over array elements */
               for (j = 0; j < key.num_values; j++) {
                  /* get name #j */
                  size = NAME_LENGTH;
                  db_get_data_index(hDB, hKeyNames, tag[i_tag].name, &size, j,
                                    TID_STRING);

                  /* append variable key name for single name array */
                  if (single_names) {
                     if (strlen(tag[i_tag].name) + 1 + strlen(varkey.name) >= NAME_LENGTH) {
                        cm_msg(MERROR, "open_history",
                               "Name for history entry \"%s %s\" too long",
                               tag[i_tag].name, varkey.name);
                        free(tag);
                        return 0;
                     }
                     strcat(tag[i_tag].name, " ");
                     strcat(tag[i_tag].name, varkey.name);
                  }

                  tag[i_tag].type = varkey.type;
                  tag[i_tag].n_data = 1;
                  i_tag++;
               }
            } else {
               strcpy(tag[i_tag].name, varkey.name);
               tag[i_tag].type = varkey.type;
               tag[i_tag].n_data = varkey.num_values;
               i_tag++;
            }
         }

         hs_define_event(event_id, eq_name, tag, sizeof(TAG) * i_tag);
         free(tag);

         /* setup hist_log structure for this event */
         hist_log[index].event_id = event_id;
         hist_log[index].hKeyVar = hKeyVar;
         db_get_record_size(hDB, hKeyVar, 0, &size);
         hist_log[index].buffer_size = size;
         hist_log[index].buffer = malloc(size);
         hist_log[index].period = history;
         hist_log[index].last_log = 0;
         if (hist_log[index].buffer == NULL) {
            cm_msg(MERROR, "open_history", "cannot allocate history buffer");
            return 0;
         }

         /* open hot link to variables */
         status = db_open_record(hDB, hKeyVar, hist_log[index].buffer,
                                 size, MODE_READ, log_history, NULL);
         if (status != DB_SUCCESS)
            cm_msg(MERROR, "open_history",
                   "cannot open variable record for history logging");

         /* remember maximum event id for later use with system events */
         if (event_id > max_event_id)
            max_event_id = event_id;
      } else {
         hist_log[index].event_id = 0;
         hist_log[index].hKeyVar = 0;
         hist_log[index].buffer = NULL;
         hist_log[index].buffer_size = 0;
         hist_log[index].period = 0;
      }
   }

   if (index == MAX_HISTORY) {
      cm_msg(MERROR, "open_history", "too many equipments for history");
      return 0;
   }

  /*---- define linked trees ---------------------------------------*/

   /* round up event id */
   max_event_id = ((int) ((max_event_id + 1) / 10) + 1) * 10;

   status = db_find_key(hDB, 0, "/History/Links", &hKeyRoot);
   if (status == DB_SUCCESS) {
      for (li = 0;; li++) {
         status = db_enum_link(hDB, hKeyRoot, li, &hHistKey);
         if (status == DB_NO_MORE_SUBKEYS)
            break;

         db_get_key(hDB, hHistKey, &histkey);
         strcpy(hist_name, histkey.name);
         db_enum_key(hDB, hKeyRoot, li, &hHistKey);

         db_get_key(hDB, hHistKey, &key);
         if (key.type != TID_KEY) {
            cm_msg(MERROR, "open_history", "Only subkeys allows in /history/links");
            continue;
         }

         /* count subkeys in link */
         for (i = n_var = 0;; i++) {
            status = db_enum_key(hDB, hHistKey, i, &hKey);
            if (status == DB_NO_MORE_SUBKEYS)
               break;
            if (db_get_key(hDB, hKey, &key) == DB_SUCCESS) {
               if (key.type != TID_KEY)
                  n_var++;
            } else {
               db_enum_link(hDB, hHistKey, i, &hKey);
               db_get_key(hDB, hKey, &key);
               cm_msg(MERROR, "open_history",
                      "History link /History/Links/%s/%s is invalid", hist_name,
                      key.name);
            }
         }

         if (n_var == 0)
            cm_msg(MERROR, "open_history", "History event %s has no variables in ODB",
                   hist_name);
         else {
            /* create tag array */
            tag = malloc(sizeof(TAG) * n_var);

            for (i = 0, size = 0, n_var = 0;; i++) {
               status = db_enum_link(hDB, hHistKey, i, &hLinkKey);
               if (status == DB_NO_MORE_SUBKEYS)
                  break;

               /* get link key */
               db_get_key(hDB, hLinkKey, &linkkey);

               if (linkkey.type == TID_KEY)
                  continue;

               /* get link target */
               db_enum_key(hDB, hHistKey, i, &hVarKey);
               if (db_get_key(hDB, hVarKey, &varkey) == DB_SUCCESS) {
                  /* hot-link individual values */
                  if (histkey.type == TID_KEY)
                     db_open_record(hDB, hVarKey, NULL, varkey.total_size, MODE_READ,
                                    log_system_history, (void *) index);

                  strcpy(tag[n_var].name, linkkey.name);
                  tag[n_var].type = varkey.type;
                  tag[n_var].n_data = varkey.num_values;
                  size += varkey.total_size;
                  n_var++;
               }
            }

            /* hot-link whole subtree */
            if (histkey.type == TID_LINK)
               db_open_record(hDB, hHistKey, NULL, size, MODE_READ, log_system_history,
                              (void *) index);

            hs_define_event(max_event_id, hist_name, tag, sizeof(TAG) * n_var);
            free(tag);

            /* define system history */

            hist_log[index].event_id = max_event_id;
            hist_log[index].hKeyVar = hHistKey;
            hist_log[index].buffer_size = size;
            hist_log[index].buffer = malloc(size);
            hist_log[index].period = 10;        /* 10 sec default period */
            hist_log[index].last_log = 0;
            if (hist_log[index].buffer == NULL) {
               cm_msg(MERROR, "open_history", "cannot allocate history buffer");
               return 0;
            }

            index++;
            max_event_id++;

            if (index == MAX_HISTORY) {
               cm_msg(MERROR, "open_history", "too many equipments for history");
               return 0;
            }
         }
      }
   }

  /*---- define run start/stop event -------------------------------*/

   tag = malloc(sizeof(TAG) * 2);

   strcpy(tag[0].name, "State");
   tag[0].type = TID_DWORD;
   tag[0].n_data = 1;

   strcpy(tag[1].name, "Run number");
   tag[1].type = TID_DWORD;
   tag[1].n_data = 1;

   hs_define_event(0, "Run transitions", tag, sizeof(TAG) * 2);
   free(tag);

   return CM_SUCCESS;
}

/*---- close_history -----------------------------------------------*/

void close_history()
{
   INT i, status;
   HNDLE hKeyRoot, hKey;

   /* close system history */
   status = db_find_key(hDB, 0, "/History/Links", &hKeyRoot);
   if (status != DB_SUCCESS) {
      for (i = 0;; i++) {
         status = db_enum_key(hDB, hKeyRoot, i, &hKey);
         if (status == DB_NO_MORE_SUBKEYS)
            break;
         db_close_record(hDB, hKey);
      }
   }

   /* close event history */
   for (i = 1; i < MAX_HISTORY; i++)
      if (hist_log[i].hKeyVar) {
         db_close_record(hDB, hist_log[i].hKeyVar);
         free(hist_log[i].buffer);
      }
}

/*---- log_history -------------------------------------------------*/

void log_history(HNDLE hDB, HNDLE hKey, void *info)
{
   INT i, size;

   for (i = 0; i < MAX_HISTORY; i++)
      if (hist_log[i].hKeyVar == hKey)
         break;

   if (i == MAX_HISTORY)
      return;

   /* check if over period */
   if (ss_time() - hist_log[i].last_log < hist_log[i].period)
      return;

   /* check if event size has changed */
   db_get_record_size(hDB, hKey, 0, &size);
   if (size != hist_log[i].buffer_size) {
      close_history();
      open_history();
   }

   hs_write_event(hist_log[i].event_id, hist_log[i].buffer, hist_log[i].buffer_size);
   hist_log[i].last_log = ss_time();
}

/*------------------------------------------------------------------*/

void log_system_history(HNDLE hDB, HNDLE hKey, void *info)
{
   INT i, size, total_size, status, index;
   KEY key;

   index = (int) info;

   /* check if over period */
   if (ss_time() - hist_log[index].last_log < hist_log[index].period)
      return;

   for (i = 0, total_size = 0;; i++) {
      status = db_enum_key(hDB, hist_log[index].hKeyVar, i, &hKey);
      if (status == DB_NO_MORE_SUBKEYS)
         break;

      db_get_key(hDB, hKey, &key);
      size = key.total_size;
      db_get_data(hDB, hKey, (char *) hist_log[index].buffer + total_size, &size,
                  key.type);
      total_size += size;
   }

   if (total_size != hist_log[index].buffer_size) {
      close_history();
      open_history();
   } else
      hs_write_event(hist_log[index].event_id, hist_log[index].buffer,
                     hist_log[index].buffer_size);

   hist_log[index].last_log = ss_time();

   /* simulate odb key update for hot links connected to system history */
   db_lock_database(hDB);
   db_notify_clients(hDB, hist_log[index].hKeyVar, FALSE);
   db_unlock_database(hDB);

}

/*------------------------------------------------------------------*/

INT log_callback(INT index, void *prpc_param[])
{
   HNDLE hKeyRoot, hKeyChannel;
   INT i, status, size, channel, izero, htape, online_mode;
   DWORD watchdog_timeout;
   BOOL watchdog_flag;
   char str[256];
   double dzero;

   /* rewind tapes */
   if (index == RPC_LOG_REWIND) {
      channel = *((INT *) prpc_param[0]);

      /* loop over all channels */
      status = db_find_key(hDB, 0, "/Logger/Channels", &hKeyRoot);
      if (status != DB_SUCCESS) {
         cm_msg(MERROR, "log_callback", "cannot find Logger/Channels entry in database");
         return 0;
      }

      /* check online mode */
      online_mode = 0;
      size = sizeof(online_mode);
      db_get_value(hDB, 0, "/Runinfo/online mode", &online_mode, &size, TID_INT, TRUE);

      for (i = 0; i < MAX_CHANNELS; i++) {
         status = db_enum_key(hDB, hKeyRoot, i, &hKeyChannel);
         if (status == DB_NO_MORE_SUBKEYS)
            break;

         /* skip if wrong channel, -1 means rewind all channels */
         if (channel != i && channel != -1)
            continue;

         if (status == DB_SUCCESS) {
            size = sizeof(str);
            status =
                db_get_value(hDB, hKeyChannel, "Settings/Type", str, &size, TID_STRING,
                             TRUE);
            if (status != DB_SUCCESS)
               continue;

            if (equal_ustring(str, "Tape")) {
               size = sizeof(str);
               status =
                   db_get_value(hDB, hKeyChannel, "Settings/Filename", str, &size,
                                TID_STRING, TRUE);
               if (status != DB_SUCCESS)
                  continue;

               if (ss_tape_open(str, O_RDONLY, &htape) == SS_SUCCESS) {
                  cm_msg(MTALK, "log_callback", "rewinding tape #%d, please wait", i);

                  cm_get_watchdog_params(&watchdog_flag, &watchdog_timeout);
                  cm_set_watchdog_params(watchdog_flag, 300000);        /* 5 min for tape rewind */
                  ss_tape_rewind(htape);
                  if (online_mode)
                     ss_tape_unmount(htape);
                  cm_set_watchdog_params(watchdog_flag, watchdog_timeout);

                  cm_msg(MINFO, "log_callback", "Tape %s rewound sucessfully", str);
               } else
                  cm_msg(MERROR, "log_callback", "Cannot rewind tape %s", str);

               ss_tape_close(htape);

               /* clear statistics */
               dzero = izero = 0;
               log_chn[i].statistics.bytes_written_total = 0;
               log_chn[i].statistics.files_written = 0;
               db_set_value(hDB, hKeyChannel, "Statistics/Bytes written total", &dzero,
                            sizeof(dzero), 1, TID_DOUBLE);
               db_set_value(hDB, hKeyChannel, "Statistics/Files written", &izero,
                            sizeof(izero), 1, TID_INT);
            }
         }
      }

      cm_msg(MTALK, "log_callback", "tape rewind finished");
   }

   return RPC_SUCCESS;
}

/*------------------------------------------------------------------*/

/********************************************************************\

                         transition callbacks

\********************************************************************/

struct {
   DWORD transition;
   DWORD run_number;
} eb;

/*------------------------------------------------------------------*/

INT tr_start1(INT run_number, char *error)
/********************************************************************\

   Prestart:

     Loop through channels defined in /logger/channels.
     Neglect channels with are not active.
     If "filename" contains a "%", substitute it by the
     current run number. Open logging channel and
     corresponding buffer. Place a event request
     into the buffer.

\********************************************************************/
{
   INT size, index, status;
   HNDLE hKeyRoot, hKeyChannel;
   char str[256], path[256], dir[256];
   CHN_SETTINGS *chn_settings;
   KEY key;
   BOOL write_data, tape_flag = FALSE;

   /* save current ODB */
   odb_save("last.odb");

   /* read global logging flag */
   size = sizeof(BOOL);
   write_data = TRUE;
   db_get_value(hDB, 0, "/Logger/Write data", &write_data, &size, TID_BOOL, TRUE);

   /* read tape message flag */
   size = sizeof(tape_message);
   db_get_value(hDB, 0, "/Logger/Tape message", &tape_message, &size, TID_BOOL, TRUE);

   /* loop over all channels */
   status = db_find_key(hDB, 0, "/Logger/Channels", &hKeyRoot);
   if (status != DB_SUCCESS) {
      /* if no channels are defined, define at least one */
      status = db_create_record(hDB, 0, "/Logger/Channels/0/", strcomb(chn_settings_str));
      if (status != DB_SUCCESS) {
         strcpy(error, "Cannot create channel entry in database");
         cm_msg(MERROR, "tr_prestart", error);
         return 0;
      }

      status = db_find_key(hDB, 0, "/Logger/Channels", &hKeyRoot);
      if (status != DB_SUCCESS) {
         strcpy(error, "Cannot create channel entry in database");
         cm_msg(MERROR, "tr_prestart", error);
         return 0;
      }
   }

   for (index = 0; index < MAX_CHANNELS; index++) {
      status = db_enum_key(hDB, hKeyRoot, index, &hKeyChannel);
      if (status == DB_NO_MORE_SUBKEYS)
         break;

      /* correct channel record */
      db_get_key(hDB, hKeyChannel, &key);
      status = db_create_record(hDB, hKeyRoot, key.name, strcomb(chn_settings_str));
      if (status != DB_SUCCESS && status != DB_OPEN_RECORD) {
         cm_msg(MERROR, "tr_prestart", "Cannot create channel record");
         break;
      }

      if (status == DB_SUCCESS || status == DB_OPEN_RECORD) {
         /* check if channel is already open */
         if (log_chn[index].handle || log_chn[index].ftp_con)
            continue;

         /* save settings key */
         status =
             db_find_key(hDB, hKeyChannel, "Settings", &log_chn[index].settings_hkey);
         if (status != DB_SUCCESS) {
            strcpy(error, "Cannot find channel settings info");
            cm_msg(MERROR, "tr_prestart", error);
            return 0;
         }

         /* save statistics key */
         status = db_find_key(hDB, hKeyChannel, "Statistics", &log_chn[index].stats_hkey);
         if (status != DB_SUCCESS) {
            strcpy(error, "Cannot find channel statistics info");
            cm_msg(MERROR, "tr_prestart", error);
            return 0;
         }

         /* clear statistics */
         size = sizeof(CHN_STATISTICS);
         db_get_record(hDB, log_chn[index].stats_hkey, &log_chn[index].statistics, &size,
                       0);

         log_chn[index].statistics.events_written = 0;
         log_chn[index].statistics.bytes_written = 0;

         db_set_record(hDB, log_chn[index].stats_hkey, &log_chn[index].statistics, size,
                       0);

         /* get channel info structure */
         chn_settings = &log_chn[index].settings;
         size = sizeof(CHN_SETTINGS);
         status =
             db_get_record(hDB, log_chn[index].settings_hkey, chn_settings, &size, 0);
         if (status != DB_SUCCESS) {
            strcpy(error, "Cannot read channel info");
            cm_msg(MERROR, "tr_prestart", error);
            return 0;
         }

         /* don't start run if tape is full */
         if (log_chn[index].type == LOG_TYPE_TAPE &&
             chn_settings->tape_capacity > 0 &&
             log_chn[index].statistics.bytes_written_total >= chn_settings->tape_capacity)
         {
            strcpy(error, "Tape capacity reached. Please load new tape");
            cm_msg(MERROR, "tr_prestart", error);
            return 0;
         }

         /* check if active */
         if (!chn_settings->active || !write_data)
            continue;

         /* check for type */
         if (equal_ustring(chn_settings->type, "Tape"))
            log_chn[index].type = LOG_TYPE_TAPE;
         else if (equal_ustring(chn_settings->type, "FTP"))
            log_chn[index].type = LOG_TYPE_FTP;
         else
            log_chn[index].type = LOG_TYPE_DISK;

         /* if disk, precede filename with directory if not already there */
         if (log_chn[index].type == LOG_TYPE_DISK &&
             strchr(chn_settings->filename, DIR_SEPARATOR) == NULL) {
            size = sizeof(dir);
            dir[0] = 0;
            db_get_value(hDB, 0, "/Logger/Data Dir", dir, &size, TID_STRING, TRUE);
            if (dir[0] != 0)
               if (dir[strlen(dir) - 1] != DIR_SEPARATOR)
                  strcat(dir, DIR_SEPARATOR_STR);
            strcpy(str, dir);
         } else
            str[0] = 0;
         strcat(str, chn_settings->filename);

         /* substitue "%d" by current run number */
         if (strchr(str, '%'))
            sprintf(path, str, run_number);
         else
            strcpy(path, str);

         strcpy(log_chn[index].path, path);

         if (log_chn[index].type == LOG_TYPE_TAPE &&
             log_chn[index].statistics.bytes_written_total == 0 && tape_message) {
            tape_flag = TRUE;
            cm_msg(MTALK, "tr_prestart", "mounting tape #%d, please wait", index);
         }

         /* open logging channel */
         status = log_open(&log_chn[index], run_number);

         /* return if logging channel couldn't be opened */
         if (status != SS_SUCCESS) {
            if (status == SS_FILE_ERROR)
               sprintf(error, "cannot open file %s (Disk full?)", path);
            if (status == SS_FILE_EXISTS)
               sprintf(error, "file %s exists already, run start aborted", path);
            if (status == SS_NO_TAPE)
               sprintf(error, "no tape in device %s", path);
            if (status == SS_TAPE_ERROR)
               sprintf(error, "tape error, cannot start run");
            if (status == SS_DEV_BUSY)
               sprintf(error, "device %s used by someone else", path);
            if (status == FTP_NET_ERROR || status == FTP_RESPONSE_ERROR)
               sprintf(error, "cannot open FTP channel to [%s]", path);

            cm_msg(MERROR, "tr_prestart", error);
            return 0;
         }

         /* open hot link to statistics tree */
         status =
             db_open_record(hDB, log_chn[index].stats_hkey, &log_chn[index].statistics,
                            sizeof(CHN_STATISTICS), MODE_WRITE, NULL, NULL);
         if (status != DB_SUCCESS)
            cm_msg(MERROR, "tr_prestart",
                   "cannot open statistics record, probably other logger is using it");

         /* open hot link to settings tree */
         status =
             db_open_record(hDB, log_chn[index].settings_hkey, &log_chn[index].settings,
                            sizeof(CHN_SETTINGS), MODE_READ, NULL, NULL);
         if (status != DB_SUCCESS)
            cm_msg(MERROR, "tr_prestart",
                   "cannot open channel settings record, probably other logger is using it");

#ifndef FAL_MAIN
         /* open buffer */
         status =
             bm_open_buffer(chn_settings->buffer, EVENT_BUFFER_SIZE,
                            &log_chn[index].buffer_handle);
         if (status != BM_SUCCESS && status != BM_CREATED) {
            sprintf(error, "Cannot open buffer %s", str);
            cm_msg(MERROR, "tr_prestart", error);
            return 0;
         }
         bm_set_cache_size(log_chn[index].buffer_handle, 100000, 0);

         /* place event request */
         status = bm_request_event(log_chn[index].buffer_handle,
                                   (short) chn_settings->event_id,
                                   (short) chn_settings->trigger_mask,
                                   GET_ALL, &log_chn[index].request_id, receive_event);

         if (status != BM_SUCCESS) {
            sprintf(error, "Cannot place event request");
            cm_msg(MERROR, "tr_prestart", error);
            return 0;
         }

         /* open message buffer if requested */
         if (chn_settings->log_messages) {
            status =
                bm_open_buffer(MESSAGE_BUFFER_NAME, MESSAGE_BUFFER_SIZE,
                               &log_chn[index].msg_buffer_handle);
            if (status != BM_SUCCESS && status != BM_CREATED) {
               sprintf(error, "Cannot open buffer %s", MESSAGE_BUFFER_NAME);
               cm_msg(MERROR, "tr_prestart", error);
               return 0;
            }

            /* place event request */
            status = bm_request_event(log_chn[index].msg_buffer_handle,
                                      (short) EVENTID_MESSAGE,
                                      (short) chn_settings->log_messages,
                                      GET_ALL, &log_chn[index].msg_request_id,
                                      receive_event);

            if (status != BM_SUCCESS) {
               sprintf(error, "Cannot place event request");
               cm_msg(MERROR, "tr_prestart", error);
               return 0;
            }
         }
#endif
      }
   }

   if (tape_flag && tape_message)
      cm_msg(MTALK, "tr_prestart", "tape mounting finished");

   /* reopen history channels if event definition has changed */
   close_history();
   open_history();

   /* write transition event into history */
   eb.transition = STATE_RUNNING;
   eb.run_number = run_number;
   hs_write_event(0, &eb, sizeof(eb));

#ifdef FAL_MAIN
   return tr_start2(run_number, error);
#endif

   return CM_SUCCESS;
}

/*-- poststop ------------------------------------------------------*/

INT tr_poststop(INT run_number, char *error)
/********************************************************************\

   Poststop:

     Wait until buffers are empty, then close logging channels

\********************************************************************/
{
   INT i, size;
   BOOL flag, tape_flag = FALSE;
   char filename[256];
   char str[256];

   if (in_stop_transition)
      return CM_SUCCESS;

   in_stop_transition = TRUE;
   for (i = 0; i < MAX_CHANNELS; i++) {
      if (log_chn[i].handle || log_chn[i].ftp_con) {
         /* generate MTALK message */
         if (log_chn[i].type == LOG_TYPE_TAPE && tape_message) {
            tape_flag = TRUE;
            cm_msg(MTALK, "tr_poststop", "closing tape channel #%d, please wait", i);
         }
#ifndef FAL_MAIN
         /* wait until buffer is empty */
         if (log_chn[i].buffer_handle) {
#ifdef DELAYED_STOP
            DWORD start_time;

            start_time = ss_millitime();
            do {
               cm_yield(100);
            } while (ss_millitime() - start_time < DELAYED_STOP);
#else
            INT n_bytes;
            do {
               bm_get_buffer_level(log_chn[i].buffer_handle, &n_bytes);
               if (n_bytes > 0)
                  cm_yield(100);
            } while (n_bytes > 0);
#endif
         }
#endif                          /* FAL_MAIN */

         /* close logging channel */
         log_close(&log_chn[i], run_number);

         /* close statistics record */
         db_set_record(hDB, log_chn[i].stats_hkey, &log_chn[i].statistics,
                       sizeof(CHN_STATISTICS), 0);
         db_close_record(hDB, log_chn[i].stats_hkey);
         db_close_record(hDB, log_chn[i].settings_hkey);
      }
   }

   /* close buffers */
   for (i = 0; i < MAX_CHANNELS; i++) {
#ifndef FAL_MAIN
      if (log_chn[i].buffer_handle) {
         INT j;

         bm_close_buffer(log_chn[i].buffer_handle);
         for (j = i + 1; j < MAX_CHANNELS; j++)
            if (log_chn[j].buffer_handle == log_chn[i].buffer_handle)
               log_chn[j].buffer_handle = 0;
      }

      if (log_chn[i].msg_request_id)
         bm_delete_request(log_chn[i].msg_request_id);
#endif

      /* clear channel info */
      memset(&log_chn[i].handle, 0, sizeof(LOG_CHN));
   }

   /* ODB dump if requested */
   size = sizeof(flag);
   flag = 0;
   db_get_value(hDB, 0, "/Logger/ODB Dump", &flag, &size, TID_BOOL, TRUE);
   if (flag) {
      strcpy(str, "run%d.odb");
      size = sizeof(str);
      str[0] = 0;
      db_get_value(hDB, 0, "/Logger/ODB Dump File", str, &size, TID_STRING, TRUE);
      if (str[0] == 0)
         strcpy(str, "run%d.odb");

      /* substitue "%d" by current run number */
      if (strchr(str, '%'))
         sprintf(filename, str, run_number);
      else
         strcpy(filename, str);

      odb_save(filename);
   }

   in_stop_transition = FALSE;

   if (tape_flag & tape_message)
      cm_msg(MTALK, "tr_poststop", "all tape channels closed");

   /* write transition event into history */
   eb.transition = STATE_STOPPED;
   eb.run_number = run_number;
   hs_write_event(0, &eb, sizeof(eb));

   return CM_SUCCESS;
}

/*== common code FAL/MLOGGER end ===================================*/

/*-- add </logger/data dir> before filename ------------------------*/

void add_data_dir(char *result, char *file)
{
   HNDLE hDB, hkey;
   char str[256];
   int size;

   cm_get_experiment_database(&hDB, NULL);
   db_find_key(hDB, 0, "/Logger/Data dir", &hkey);

   if (hkey) {
      size = sizeof(str);
      db_get_data(hDB, hkey, str, &size, TID_STRING);
      if (str[strlen(str) - 1] != DIR_SEPARATOR)
         strcat(str, DIR_SEPARATOR_STR);
      strcat(str, file);
      strcpy(result, str);
   } else
      strcpy(result, file);
}

/*-- functions for internal tests ----------------------------------*/

ANA_TEST **tl;
int n_test = 0;

void test_register(ANA_TEST * t)
{
   int i;

   /* check if test already registered */
   for (i = 0; i < n_test; i++)
      if (tl[i] == t)
         break;
   if (i < n_test) {
      t->registered = TRUE;
      return;
   }

   /* allocate space for pointer to test */
   if (n_test == 0) {
      tl = malloc(2 * sizeof(void *));

      /* define "always true" test */
      tl[0] = malloc(sizeof(ANA_TEST));
      strcpy(tl[0]->name, "Always true");
      tl[0]->count = 0;
      tl[0]->value = TRUE;
      tl[0]->registered = TRUE;
      n_test++;
   } else
      tl = realloc(tl, (n_test + 1) * sizeof(void *));

   tl[n_test] = t;
   t->count = 0;
   t->value = FALSE;
   t->registered = TRUE;

   n_test++;
}

void test_clear()
{
   int i;

   /* clear all tests in interal list */
   for (i = 0; i < n_test; i++) {
      tl[i]->count = 0;
      tl[i]->value = FALSE;
   }

   /* set "always true" test */
   if (n_test > 0)
      tl[0]->value = TRUE;
}

void test_increment()
{
   int i;

   /* increment test counters based on their value and reset them */
   for (i = 0; i < n_test; i++) {
      if (tl[i]->value)
         tl[i]->count++;
      if (i > 0)
         tl[i]->value = FALSE;
   }
}

void test_write()
{
   int i;
   char str[256];

   /* write all test counts to /analyzer/tests/<name> */
   for (i = 0; i < n_test; i++) {
      sprintf(str, "/%s/Tests/%s", analyzer_name, tl[i]->name);
      db_set_value(hDB, 0, str, &tl[i]->count, sizeof(DWORD), 1, TID_DWORD);
   }
}

/*-- start ---------------------------------------------------------*/

INT tr_start2(INT rn, char *error)
/********************************************************************\

     Initialize serial numbers, update display

\********************************************************************/
{
   ANA_MODULE **module;
   INT i, j, status, size;
   char str[256];
   BANK_LIST *bank_list;

   /* reset serial numbers */
   for (i = 0; equipment[i].name[0]; i++) {
      equipment[i].serial_number = 1;
      equipment[i].odb_in = equipment[i].odb_out = 0;
   }

   /* reset counters */
   for (i = 0; analyze_request[i].event_name[0]; i++) {
      analyze_request[i].events_received = 0;
      analyze_request[i].ar_stats.events_received = 0;
   }

   for (i = 0; analyze_request[i].event_name[0]; i++) {
      /* copy output flag from ODB to bank_list */
      bank_list = analyze_request[i].bank_list;

      if (bank_list != NULL)
         for (; bank_list->name[0]; bank_list++) {
            sprintf(str, "/%s/Bank switches/%s", analyzer_name, bank_list->name);
            bank_list->output_flag = FALSE;
            size = sizeof(DWORD);
            db_get_value(hDB, 0, str, &bank_list->output_flag, &size, TID_DWORD, TRUE);
         }

      /* copy module enabled flag to ana_module */
      module = analyze_request[i].ana_module;
      for (j = 0; module != NULL && module[j] != NULL; j++) {
         sprintf(str, "/%s/Module switches/%s", analyzer_name, module[j]->name);
         module[j]->enabled = TRUE;
         size = sizeof(BOOL);
         db_get_value(hDB, 0, str, &module[j]->enabled, &size, TID_BOOL, TRUE);
      }
   }
#ifndef MANA_LITE
   /* clear histos and N-tuples */
   if (out_info.clear_histos) {
      for (i = 0; analyze_request[i].event_name[0]; i++)
         if (analyze_request[i].bank_list != NULL)
            if (HEXIST(analyze_request[i].ar_info.event_id))
               HRESET(analyze_request[i].ar_info.event_id, bstr);
      HRESET(0, bstr);

      test_clear();
   }
#endif
   /* call bor for modules */
   for (i = 0; analyze_request[i].event_name[0]; i++) {
      module = analyze_request[i].ana_module;
      for (j = 0; module != NULL && module[j] != NULL; j++)
         if (module[j]->bor != NULL && module[j]->enabled)
            module[j]->bor(run_number);
   }


   /* call frontend BOR routine */
   status = begin_of_run(rn, error);
   if (status != CM_SUCCESS)
      return status;

   /* call main analyzer BOR routine */
   status = ana_begin_of_run(rn, error);
   if (status != CM_SUCCESS)
      return status;

   run_state = STATE_RUNNING;
   run_number = rn;

   send_all_periodic_events(TR_START);

   ss_printf(14, 2, "Running");
   ss_printf(36, 2, "%d", rn);

   return status;
}

/*-- stop ----------------------------------------------------------*/

INT tr_stop(INT rn, char *error)
/********************************************************************\

     Send all periodic events, update display

\********************************************************************/
{
   ANA_MODULE **module;
   INT i, j, size, status;
   char str[256], file_name[256];

   /* call frontend EOR routine */
   status = end_of_run(rn, error);
   if (status != CM_SUCCESS)
      return status;

   /* don't send events if already stopped */
   if (run_state != STATE_STOPPED)
      send_all_periodic_events(TR_STOP);

   /* call EOR routines modules */
   for (i = 0; analyze_request[i].event_name[0]; i++) {
      module = analyze_request[i].ana_module;
      for (j = 0; module != NULL && module[j] != NULL; j++)
         if (module[j]->eor != NULL && module[j]->enabled)
            module[j]->eor(run_number);
   }

   /* call main analyzer BOR routine */
   status = ana_end_of_run(run_number, error);

   /* write tests to ODB */
   update_stats();

#ifndef MANA_LITE
   /* save histos if requested */
   if (out_info.histo_dump) {
      size = sizeof(str);
      str[0] = 0;
      db_get_value(hDB, 0, "/Logger/Data Dir", str, &size, TID_STRING, TRUE);
      if (str[0] != 0)
         if (str[strlen(str) - 1] != DIR_SEPARATOR)
            strcat(str, DIR_SEPARATOR_STR);

      strcat(str, out_info.histo_dump_filename);
      if (strchr(str, '%') != NULL)
         sprintf(file_name, str, run_number);
      else
         strcpy(file_name, str);

      strcpy(str, "NT");
      HRPUT(0, file_name, str);
   }
#endif
   run_state = STATE_STOPPED;
   run_number = rn;

   ss_printf(14, 2, "Stopped");

   return status;
}

/*-- pause ---------------------------------------------------------*/

INT tr_pause(INT rn, char *error)
{
   INT status;

   status = pause_run(rn, error);
   if (status != CM_SUCCESS)
      return status;

   status = ana_pause_run(rn, error);
   if (status != CM_SUCCESS)
      return status;

   run_state = STATE_PAUSED;
   run_number = rn;

   send_all_periodic_events(TR_PAUSE);

   ss_printf(14, 2, "Paused");

   /* write transition event into history */
   eb.transition = STATE_PAUSED;
   eb.run_number = run_number;
   hs_write_event(0, &eb, sizeof(eb));

   return status;
}

/*-- resume --------------------------------------------------------*/

INT tr_resume(INT rn, char *error)
{
   INT status;

   status = resume_run(rn, error);
   if (status != CM_SUCCESS)
      return status;

   status = ana_resume_run(rn, error);
   if (status != CM_SUCCESS)
      return status;

   run_state = STATE_RUNNING;
   run_number = rn;

   send_all_periodic_events(TR_RESUME);

   ss_printf(14, 2, "Running");

   /* write transition event into history */
   eb.transition = STATE_RUNNING;
   eb.run_number = run_number;
   hs_write_event(0, &eb, sizeof(eb));

   return status;
}

/********************************************************************/

/*---- ODB output --------------------------------------------------*/

INT write_event_odb(EVENT_HEADER * pevent, ANALYZE_REQUEST * par)
{
   INT status, size, n_data, i;
   BANK_HEADER *pbh;
   EVENT_DEF *event_def;
   BANK *pbk;
   BANK32 *pbk32;
   void *pdata;
   char name[5];
   HNDLE hKeyRoot, hKey;
   KEY key;
   DWORD bkname;
   WORD bktype;

   event_def = db_get_event_definition(pevent->event_id);
   if (event_def == NULL)
      return SS_SUCCESS;

  /*---- MIDAS format ----------------------------------------------*/
   if (event_def->format == FORMAT_MIDAS) {
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
         if (rpc_tid_size(pbk->type & 0xFF))
            n_data /= rpc_tid_size(pbk->type & 0xFF);

         /* get bank key */
         *((DWORD *) name) = bkname;
         name[4] = 0;

         status = db_find_key(hDB, event_def->hDefKey, name, &hKeyRoot);
         if (status != DB_SUCCESS) {
            cm_msg(MERROR, "write_event_odb", "received unknown bank %s", name);
            continue;
         }

         if (bktype == TID_STRUCT) {
            /* write structured bank */
            for (i = 0;; i++) {
               status = db_enum_key(hDB, hKeyRoot, i, &hKey);
               if (status == DB_NO_MORE_SUBKEYS)
                  break;

               db_get_key(hDB, hKey, &key);

               /* adjust for alignment */
               pdata = (void *) VALIGN(pdata, MIN(ss_get_struct_align(), key.item_size));

               status = db_set_data(hDB, hKey, pdata, key.item_size * key.num_values,
                                    key.num_values, key.type);
               if (status != DB_SUCCESS) {
                  cm_msg(MERROR, "write_event_odb", "cannot write %s to ODB", name);
                  continue;
               }

               /* shift data pointer to next item */
               pdata = (char *) pdata + key.item_size * key.num_values;
            }
         } else {
            db_get_key(hDB, hKeyRoot, &key);

            /* write variable length bank  */
            if (n_data > 0) {
               status = db_set_data(hDB, hKeyRoot, pdata, size, n_data, key.type);
               if (status != DB_SUCCESS) {
                  cm_msg(MERROR, "write_event_odb", "cannot write %s to ODB", name);
                  continue;
               }
            }
         }
      } while (1);
   }

  /*---- FIXED format ----------------------------------------------*/
   if (event_def->format == FORMAT_FIXED) {
      if (db_set_record(hDB, event_def->hDefKey, (char *) (pevent + 1),
                        pevent->data_size, 0) != DB_SUCCESS)
         cm_msg(MERROR, "write_event_odb", "event #%d size mismatch", pevent->event_id);
   }

   return SUCCESS;
}

/*-- book N-tuples from ODB bank structures ------------------------*/

char hbook_types[][8] = {
   "",
   ":U:8",                      /* TID_BYTE      */
   ":I:8",                      /* TID_SBYTE     */
   ":I:8",                      /* TID_CHAR      */
   ":U:16",                     /* TID_WORD      */
   ":I:16",                     /* TID_SHORT     */
   ":U*4",                      /* TID_DWORD     */
   ":I*4",                      /* TID_INT       */
   ":I*4",                      /* TID_BOOL      */
   ":R*4",                      /* TID_FLOAT     */
   ":R*8",                      /* TID_DOUBLE    */
   ":U:8",                      /* TID_BITFIELD  */
   ":C:32",                     /* TID_STRING    */
   "",                          /* TID_ARRAY     */
   "",                          /* TID_STRUCT    */
   "",                          /* TID_KEY       */
   "",                          /* TID_LINK      */
   "",                          /* TID_LAST      */

};

#ifndef MANA_LITE
INT book_ntuples(void);

void banks_changed(INT hDB, INT hKey, void *info)
{
   char str[80];
   HNDLE hkey;

   /* close previously opened hot link */
   sprintf(str, "/%s/Bank switches", analyzer_name);
   db_find_key(hDB, 0, str, &hkey);
   db_close_record(hDB, hkey);

   book_ntuples();
   print_message("N-tuples rebooked");
}

INT book_ntuples(void)
{
   INT index, i, j, status, n_tag, size, id;
   HNDLE hkey;
   KEY key;
   char rw_tag[512][8];
   char str[80], key_name[NAME_LENGTH], block_name[NAME_LENGTH];
   BANK_LIST *bank_list;
   EVENT_DEF *event_def;

   /* copy output flag from ODB to bank_list */
   for (i = 0; analyze_request[i].event_name[0]; i++) {
      bank_list = analyze_request[i].bank_list;

      if (bank_list != NULL)
         for (; bank_list->name[0]; bank_list++) {
            sprintf(str, "/%s/Bank switches/%s", analyzer_name, bank_list->name);
            bank_list->output_flag = FALSE;
            size = sizeof(DWORD);
            db_get_value(hDB, 0, str, &bank_list->output_flag, &size, TID_DWORD, TRUE);
         }
   }

   /* hot link bank switches to N-tuple re-booking */
   sprintf(str, "/%s/Bank switches", analyzer_name);
   db_find_key(hDB, 0, str, &hkey);
   db_open_record(hDB, hkey, NULL, 0, MODE_READ, banks_changed, NULL);

   /* book RW N-tuples */

   /* go through all analyzer requests (events) */
   for (index = 0; analyze_request[index].event_name[0]; index++) {
      /* don't book N-tuples if buffer size is zero */
      if (analyze_request[index].rwnt_buffer_size == 0)
         continue;

      n_tag = 0;

      strcpy(rw_tag[n_tag++], "Run");
      strcpy(rw_tag[n_tag++], "Number");
      strcpy(rw_tag[n_tag++], "Time");

      bank_list = analyze_request[index].bank_list;
      if (bank_list == NULL) {
         /* book fixed event */
         event_def =
             db_get_event_definition((short int) analyze_request[index].ar_info.event_id);

         for (i = 0;; i++) {
            status = db_enum_key(hDB, event_def->hDefKey, i, &hkey);
            if (status == DB_NO_MORE_SUBKEYS)
               break;

            db_get_key(hDB, hkey, &key);

            /* convert blanks etc to '_' */
            strcpy(key_name, key.name);
            for (j = 0; key_name[j]; j++) {
               if (!(key_name[j] >= 'a' && key_name[j] <= 'z') &&
                   !(key_name[j] >= 'A' && key_name[j] <= 'Z') &&
                   !(key_name[j] >= '0' && key_name[j] <= '9'))
                  key_name[j] = '_';
            }

            if (key.num_values > 1)
               for (j = 0; j < key.num_values; j++) {
                  sprintf(str, "%s%d", key_name, j);
                  strncpy(rw_tag[n_tag++], str, 8);
                  if (n_tag >= 512) {
                     cm_msg(MERROR, "book_ntuples",
                            "Too much tags for RW N-tupeles (512 maximum)");
                     return 0;
                  }
            } else
               strncpy(rw_tag[n_tag++], key_name, 8);
            if (n_tag >= 512) {
               cm_msg(MERROR, "book_ntuples",
                      "Too much tags for RW N-tupeles (512 maximum)");
               return 0;
            }
         }
      } else {
         /* go through all banks in bank_list */
         for (; bank_list->name[0]; bank_list++) {
            /* remember tag offset in n_data variable */
            bank_list->n_data = n_tag;

            if (bank_list->output_flag == 0)
               continue;

            if (bank_list->type != TID_STRUCT) {
               for (i = 0; i < (INT) bank_list->size; i++) {
                  sprintf(str, "%s%d", bank_list->name, i);
                  strncpy(rw_tag[n_tag++], str, 8);
               }
            } else {
               /* define structured bank */
               for (i = 0;; i++) {
                  status = db_enum_key(hDB, bank_list->def_key, i, &hkey);
                  if (status == DB_NO_MORE_SUBKEYS)
                     break;

                  db_get_key(hDB, hkey, &key);

                  /* convert blanks etc to '_' */
                  strcpy(key_name, key.name);
                  for (j = 0; key_name[j]; j++) {
                     if (!(key_name[j] >= 'a' && key_name[j] <= 'z') &&
                         !(key_name[j] >= 'A' && key_name[j] <= 'Z') &&
                         !(key_name[j] >= '0' && key_name[j] <= '9'))
                        key_name[j] = '_';
                  }

                  if (key.num_values > 1)
                     for (j = 0; j < key.num_values; j++) {
                        sprintf(str, "%s%d", key_name, j);
                        strncpy(rw_tag[n_tag++], str, 8);
                        if (n_tag >= 512) {
                           cm_msg(MERROR, "book_ntuples",
                                  "Too much tags for RW N-tupeles (512 maximum)");
                           return 0;
                        }
                  } else
                     strncpy(rw_tag[n_tag++], key_name, 8);
                  if (n_tag >= 512) {
                     cm_msg(MERROR, "book_ntuples",
                            "Too much tags for RW N-tupeles (512 maximum)");
                     return 0;
                  }
               }
            }
         }
      }

      /* book N-tuple with evend ID */
      strcpy(block_name, analyze_request[index].event_name);
      block_name[8] = 0;
#ifndef MANA_LITE
      id = analyze_request[index].ar_info.event_id;
      if (HEXIST(id))
         HDELET(id);

      HBOOKN(id, block_name, n_tag, bstr,
             n_tag * analyze_request[index].rwnt_buffer_size, rw_tag);

      if (!HEXIST(id)) {
         printf("\n");
         cm_msg(MINFO, "book_ntuples",
                "Cannot book N-tuple #%d. Increase PAWC size via the -s flag or switch off banks",
                id);
      }
#endif
   }

   return SUCCESS;
}

/*---- HBOOK output ------------------------------------------------*/

INT write_event_hbook(EVENT_HEADER * pevent, ANALYZE_REQUEST * par)
{
   INT i, j, k, n, size, item_size, status;
   BANK *pbk;
   BANK32 *pbk32;
   BANK_LIST *pbl;
   BANK_HEADER *pbh;
   void *pdata;
   BOOL exclude, exclude_all;
   char block_name[5];
   float rwnt[512];
   EVENT_DEF *event_def;
   HNDLE hkey;
   KEY key;
   DWORD bkname;
   WORD bktype;

   event_def = db_get_event_definition(pevent->event_id);
   if (event_def == NULL)
      return SS_SUCCESS;

   /* fill number info */
   memset(rwnt, 0, sizeof(rwnt));
   rwnt[0] = (float) run_number;
   rwnt[1] = (float) pevent->serial_number;
   rwnt[2] = (float) pevent->time_stamp;

  /*---- MIDAS format ----------------------------------------------*/

   if (event_def->format == FORMAT_MIDAS) {
      /* first fill number block */
      pbk = NULL;
      pbk32 = NULL;
      exclude_all = TRUE;
      do {
         pbh = (BANK_HEADER *) (pevent + 1);
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

         /* look if bank is in exclude list */
         *((DWORD *) block_name) = bkname;
         block_name[4] = 0;

         exclude = FALSE;
         pbl = NULL;
         if (par->bank_list != NULL) {
            for (i = 0; par->bank_list[i].name[0]; i++)
               if (*((DWORD *) par->bank_list[i].name) == bkname) {
                  pbl = &par->bank_list[i];
                  exclude = (pbl->output_flag == 0);
                  break;
               }
            if (par->bank_list[i].name[0] == 0)
               cm_msg(MERROR, "write_event_hbook", "Received unknown bank %s",
                      block_name);
         }

         /* fill RW N-tuple */
         if (!exclude && pbl != NULL) {
            exclude_all = FALSE;

            item_size = rpc_tid_size(bktype & 0xFF);
            /* set array size in bank list */
            if ((bktype & 0xFF) != TID_STRUCT) {
               n = size / item_size;

               /* check bank size */
               if (n > (INT) pbl->size) {
                  cm_msg(MERROR, "write_event_hbook",
                         "Bank %s has more (%d) entries than maximum value (%d)",
                         block_name, n, pbl->size);
                  continue;
               }

               /* convert bank to float values */
               for (i = 0; i < n; i++) {
                  switch (bktype & 0xFF) {
                  case TID_BYTE:
                     rwnt[pbl->n_data + i] = (float) (*((BYTE *) pdata + i));
                     break;
                  case TID_WORD:
                     rwnt[pbl->n_data + i] = (float) (*((WORD *) pdata + i));
                     break;
                  case TID_DWORD:
                     rwnt[pbl->n_data + i] = (float) (*((DWORD *) pdata + i));
                     break;
                  case TID_FLOAT:
                     rwnt[pbl->n_data + i] = (float) (*((float *) pdata + i));
                     break;
                  case TID_DOUBLE:
                     rwnt[pbl->n_data + i] = (float) (*((double *) pdata + i));
                     break;
                  }
               }

               /* zero padding */
               for (; i < (INT) pbl->size; i++)
                  rwnt[pbl->n_data + i] = 0.f;
            } else {
               /* fill N-tuple from structured bank */
               k = pbl->n_data;

               for (i = 0;; i++) {
                  status = db_enum_key(hDB, pbl->def_key, i, &hkey);
                  if (status == DB_NO_MORE_SUBKEYS)
                     break;

                  db_get_key(hDB, hkey, &key);

                  /* align data pointer */
                  pdata =
                      (void *) VALIGN(pdata, MIN(ss_get_struct_align(), key.item_size));

                  for (j = 0; j < key.num_values; j++) {
                     switch (key.type & 0xFF) {
                     case TID_BYTE:
                        rwnt[k++] = (float) (*((BYTE *) pdata + j));
                        break;
                     case TID_WORD:
                        rwnt[k++] = (float) (*((WORD *) pdata + j));
                        break;
                     case TID_SHORT:
                        rwnt[k++] = (float) (*((short int *) pdata + j));
                        break;
                     case TID_INT:
                        rwnt[k++] = (float) (*((INT *) pdata + j));
                        break;
                     case TID_DWORD:
                        rwnt[k++] = (float) (*((DWORD *) pdata + j));
                        break;
                     case TID_BOOL:
                        rwnt[k++] = (float) (*((BOOL *) pdata + j));
                        break;
                     case TID_FLOAT:
                        rwnt[k++] = (float) (*((float *) pdata + j));
                        break;
                     case TID_DOUBLE:
                        rwnt[k++] = (float) (*((double *) pdata + j));
                        break;
                     }
                  }

                  /* shift data pointer to next item */
                  pdata = (char *) pdata + key.item_size * key.num_values;
               }
            }
         }

      } while (TRUE);

      /* fill shared memory */
      HFNOV(pevent->event_id, rwnt);

   }


   /* if (event_def->format == FORMAT_MIDAS) */
 /*---- FIXED format ----------------------------------------------*/
   if (event_def->format == FORMAT_FIXED) {
      /* fill N-tuple from structured bank */
      pdata = pevent + 1;
      k = 3;                    /* index 0..2 for run/serial/time */

      for (i = 0;; i++) {
         status = db_enum_key(hDB, event_def->hDefKey, i, &hkey);
         if (status == DB_NO_MORE_SUBKEYS)
            break;

         db_get_key(hDB, hkey, &key);

         /* align data pointer */
         pdata = (void *) VALIGN(pdata, MIN(ss_get_struct_align(), key.item_size));

         for (j = 0; j < key.num_values; j++) {
            switch (key.type & 0xFF) {
            case TID_BYTE:
               rwnt[k++] = (float) (*((BYTE *) pdata + j));
               break;
            case TID_WORD:
               rwnt[k++] = (float) (*((WORD *) pdata + j));
               break;
            case TID_SHORT:
               rwnt[k++] = (float) (*((short int *) pdata + j));
               break;
            case TID_INT:
               rwnt[k++] = (float) (*((INT *) pdata + j));
               break;
            case TID_DWORD:
               rwnt[k++] = (float) (*((DWORD *) pdata + j));
               break;
            case TID_BOOL:
               rwnt[k++] = (float) (*((BOOL *) pdata + j));
               break;
            case TID_FLOAT:
               rwnt[k++] = (float) (*((float *) pdata + j));
               break;
            case TID_DOUBLE:
               rwnt[k++] = (float) (*((double *) pdata + j));
               break;
            }
         }

         /* shift data pointer to next item */
         pdata = (char *) pdata + key.item_size * key.num_values;
      }

      /* fill shared memory */
      HFNOV(pevent->event_id, rwnt);
   }

   return SUCCESS;
}
#endif

/*-- analyzer init routine -----------------------------------------*/

INT mana_init()
{
   ANA_MODULE **module;
   INT i, j, status, size;
   HNDLE hkey;
   char str[256], block_name[32];
   BANK_LIST *bank_list;
   double dummy;

   /* create ODB structure for output */
   sprintf(str, "/%s/Output", analyzer_name);
   db_create_record(hDB, 0, str, OUT_INFO_STR);
   db_find_key(hDB, 0, str, &hkey);
   status = db_open_record(hDB, hkey, &out_info, sizeof(out_info), MODE_READ, NULL, NULL);
   if (status != DB_SUCCESS) {
      cm_msg(MERROR, "bor", "Cannot read output info record");
      return 0;
   }

   /* create ODB structures for banks */
   for (i = 0; analyze_request[i].event_name[0]; i++) {
      bank_list = analyze_request[i].bank_list;

      if (bank_list == NULL)
         continue;

      for (; bank_list->name[0]; bank_list++) {
         strncpy(block_name, bank_list->name, 4);
         block_name[4] = 0;

         if (bank_list->type == TID_STRUCT) {
            sprintf(str, "/Equipment/%s/Variables/%s", analyze_request[i].event_name,
                    block_name);
            db_create_record(hDB, 0, str, strcomb(bank_list->init_str));
            db_find_key(hDB, 0, str, &hkey);
            bank_list->def_key = hkey;
         } else {
            sprintf(str, "/Equipment/%s/Variables/%s", analyze_request[i].event_name,
                    block_name);
            dummy = 0;
            db_set_value(hDB, 0, str, &dummy, rpc_tid_size(bank_list->type), 1,
                         bank_list->type);
            db_find_key(hDB, 0, str, &hkey);
            bank_list->def_key = hkey;
         }
      }
   }

   /* create ODB structures for fixed events */
   for (i = 0; analyze_request[i].event_name[0]; i++) {
      if (analyze_request[i].init_string) {
         sprintf(str, "/Equipment/%s/Variables", analyze_request[i].event_name);
         db_create_record(hDB, 0, str, strcomb(analyze_request[i].init_string));
      }
   }
#ifndef MANA_LITE
   /* create global section */
   HLIMAP(pawc_size / 4, out_info.global_memory_name);
   printf("\nGLOBAL MEMORY NAME = %s\n", out_info.global_memory_name);

   /* book online N-tuples only once when online */
   status = book_ntuples();
   if (status != SUCCESS)
      return status;
#endif
   /* call main analyzer init routine */
   status = analyzer_init();
   if (status != SUCCESS)
      return status;

   /* initialize modules */
   for (i = 0; analyze_request[i].event_name[0]; i++) {
      module = analyze_request[i].ana_module;
      for (j = 0; module != NULL && module[j] != NULL; j++) {
         /* copy module enabled flag to ana_module */
         sprintf(str, "/%s/Module switches/%s", analyzer_name, module[j]->name);
         module[j]->enabled = TRUE;
         size = sizeof(BOOL);
         db_get_value(hDB, 0, str, &module[j]->enabled, &size, TID_BOOL, TRUE);

         if (module[j]->init != NULL && module[j]->enabled)
            module[j]->init();
      }
   }

   return SUCCESS;
}

/*-- init_module_parameters ----------------------------------------*/

INT init_module_parameters()
{
   INT i, j;
   ANA_MODULE **module;
   char str[80];
   HNDLE hkey;

   for (i = 0; analyze_request[i].event_name[0]; i++) {
      module = analyze_request[i].ana_module;
      for (j = 0; module != NULL && module[j] != NULL; j++) {
         if (module[j]->parameters != NULL) {
            sprintf(str, "/%s/Parameters/%s", analyzer_name, module[j]->name);

            if (module[j]->init_str)
               db_create_record(hDB, 0, str, strcomb(module[j]->init_str));

            db_find_key(hDB, 0, str, &hkey);
            if (db_open_record(hDB, hkey, module[j]->parameters, module[j]->param_size,
                               MODE_READ, NULL, NULL) != DB_SUCCESS) {
               cm_msg(MERROR, "init_module_parameters",
                      "Cannot open \"%s\" parameters in ODB", str);
               return 0;
            }
         }
      }
   }

   return SUCCESS;
}

/*-- exit routine --------------------------------------------------*/

INT mana_exit()
{
   ANA_MODULE **module;
   INT i, j;

   /* call exit routines from modules */
   for (i = 0; analyze_request[i].event_name[0]; i++) {
      module = analyze_request[i].ana_module;
      for (j = 0; module != NULL && module[j] != NULL; j++)
         if (module[j]->exit != NULL && module[j]->enabled)
            module[j]->exit();
   }

   /* call main analyzer exit routine */
   return analyzer_exit();
}

/*------------------------------------------------------------------*/

struct {
   short int event_id;
   DWORD last_time;
} last_time_event[50];

void process_event(EVENT_HEADER * pevent)
{
   INT i, status;
   ANA_MODULE **module;
   ANALYZE_REQUEST *par;
   DWORD actual_time;
   EVENT_DEF *event_def;

   /* log event to all channels  */
   for (i = 0; i < MAX_CHANNELS; i++) {
      if (log_chn[i].handle == 0)
         continue;

      log_write(&log_chn[i], pevent);
   }

   /* return for zero event size */
   if (pevent->data_size == 0)
      return;

   /* analyze event */
   par = analyze_request;

   for (i = 0; par->event_name[0]; par++)
      if (par->ar_info.event_id == pevent->event_id) {
         par->events_received++;

         /* call non-modular analyzer if defined */
         status = CM_SUCCESS;
         if (par->analyzer)
            status = par->analyzer(pevent, (void *) (pevent + 1));

         /* don't continue if event was rejected */
         if (status == 0)
            return;

         /* loop over analyzer modules */
         module = par->ana_module;
         for (i = 0; module != NULL && module[i] != NULL; i++) {
            if (module[i]->enabled) {
               status = module[i]->analyzer(pevent, (void *) (pevent + 1));

               /* don't continue if event was rejected */
               if (status == 0)
                  return;
            }
         }

         /* correct for increased event size */
         event_def = db_get_event_definition(pevent->event_id);
         if (event_def == NULL)
            return;

         if (event_def->format == FORMAT_MIDAS)
            pevent->data_size = bk_size((void *) (pevent + 1));
         if (event_def->format == FORMAT_YBOS)
            pevent->data_size = ybk_size((void *) (pevent + 1));

         /* increment tests */
         test_increment();
#ifndef MANA_LITE
         write_event_hbook(pevent, par);
#endif

         for (i = 0; equipment[i].name[0]; i++)
            if (equipment[i].info.event_id == pevent->event_id)
               break;

         if ((equipment[i].info.read_on & RO_ODB) || equipment[i].info.history) {
            /* put event in ODB once every second */
            actual_time = ss_millitime();
            for (i = 0; i < 50; i++) {
               if (last_time_event[i].event_id == pevent->event_id) {
                  if (actual_time - last_time_event[i].last_time > 1000) {
                     last_time_event[i].last_time = actual_time;
                     write_event_odb(pevent, par);
                  }
                  break;
               }
               if (last_time_event[i].event_id == 0) {
                  last_time_event[i].event_id = pevent->event_id;
                  last_time_event[i].last_time = actual_time;
                  write_event_odb(pevent, par);
                  break;
               }
            }
         }

      }
}

/*------------------------------------------------------------------*/

INT manual_trigger(INT index, void *prpc_param[])
{
   WORD event_id;
   INT i;

   event_id = CWORD(0);

   for (i = 0; equipment[i].name[0]; i++)
      if (equipment[i].info.event_id == event_id)
         send_event(i);

   if (display_period)
      display(FALSE);

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
   db_get_value(hDB, 0, "/Runinfo/Run number", &run_number, &size, TID_INT, TRUE);

   /* scan EQUIPMENT table from FRONTEND.C */
   for (index = 0; equipment[index].name[0]; index++) {
      eq_info = &equipment[index].info;
      eq_stats = &equipment[index].stats;

      if (eq_info->event_id == 0) {
         printf("Event ID 0 for %s not allowed\n", equipment[index].name);
         ss_sleep(5000);
      }

      /* init status */
      equipment[index].status = FE_SUCCESS;

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
      status = db_create_record(hDB, 0, str, EQUIPMENT_COMMON_STR);
      if (status != DB_SUCCESS) {
         printf("Cannot init equipment record, probably other FE is using it\n");
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
      strcpy(eq_info->frontend_name, frontend_name);
      strcpy(eq_info->frontend_file_name, frontend_file_name);

      /* set record from equipment[] table in frontend.c */
      db_set_record(hDB, hKey, eq_info, sizeof(EQUIPMENT_INFO), 0);

      /* open hot link to equipment info */
      db_open_record(hDB, hKey, eq_info, sizeof(EQUIPMENT_INFO), MODE_READ, NULL, NULL);

    /*---- Create variables record ---------------------------------*/
      sprintf(str, "/Equipment/%s/Variables", equipment[index].name);
      if (equipment[index].event_descrip) {
         if (equipment[index].format == FORMAT_FIXED)
            db_create_record(hDB, 0, str, (char *) equipment[index].event_descrip);
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
                  db_create_record(hDB, 0, str, strcomb(bank_list->init_str));
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

    /*-PAA- Needed in case Statistics exists but size = 0 */
    /*-SR- Not needed since db_create_record does a delete already */

      status = db_find_key(hDB, 0, str, &hKey);
      if (status == DB_SUCCESS) {
         status = db_delete_key(hDB, hKey, FALSE);
         if (status != DB_SUCCESS) {
            printf("Cannot delete statistics record, error %d\n", status);
            ss_sleep(3000);
         }
      }

      status = db_create_record(hDB, 0, str, EQUIPMENT_STATISTICS_STR);
      if (status != DB_SUCCESS) {
         printf("Cannot create statistics record, error %d\n", status);
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
         printf
             ("Cannot open statistics record, error %d. Probably other FE is using it\n",
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

void register_requests(void)
{
   INT index, status;
   char str[256];
   AR_INFO *ar_info;
   AR_STATS *ar_stats;
   HNDLE hKey;

   /* scan ANALYZE_REQUEST table from ANALYZE.C */
   for (index = 0; analyze_request[index].event_name[0]; index++) {
      ar_info = &analyze_request[index].ar_info;
      ar_stats = &analyze_request[index].ar_stats;

      /* create common subtree from analyze_request table in analyze.c */
      sprintf(str, "/%s/%s/Common", analyzer_name, analyze_request[index].event_name);
      db_create_record(hDB, 0, str, ANALYZER_REQUEST_STR);
      db_find_key(hDB, 0, str, &hKey);
      analyze_request[index].hkey_common = hKey;

      strcpy(ar_info->client_name, analyzer_name);
      gethostname(ar_info->host, sizeof(ar_info->host));
      db_set_record(hDB, hKey, ar_info, sizeof(AR_INFO), 0);

      /* create statistics tree */
      sprintf(str, "/%s/%s/Statistics", analyzer_name, analyze_request[index].event_name);
      db_create_record(hDB, 0, str, ANALYZER_STATS_STR);
      db_find_key(hDB, 0, str, &hKey);

      ar_stats->events_received = 0;
      ar_stats->events_per_sec = 0;
      ar_stats->events_written = 0;

      /* open hot link to statistics tree */
      status =
          db_open_record(hDB, hKey, ar_stats, sizeof(AR_STATS), MODE_WRITE, NULL, NULL);
      if (status != DB_SUCCESS)
         printf("Cannot open statistics record, probably other analyzer is using it\n");
   }
}

/*------------------------------------------------------------------*/

void update_stats()
{
   int i;
   AR_STATS *ar_stats;
   static DWORD last_time = 0;
   DWORD actual_time;

   actual_time = ss_millitime();

   if (last_time == 0)
      last_time = actual_time;

   if (actual_time - last_time == 0)
      return;

   for (i = 0; analyze_request[i].event_name[0]; i++) {
      ar_stats = &analyze_request[i].ar_stats;
      ar_stats->events_received += analyze_request[i].events_received;
      ar_stats->events_written += analyze_request[i].events_written;
      ar_stats->events_per_sec =
          (DWORD) (analyze_request[i].events_received /
                   ((actual_time - last_time) / 1000.0));
      analyze_request[i].events_received = 0;
      analyze_request[i].events_written = 0;
   }

   /* propagate new statistics to ODB */
   db_send_changed_records();

   /* save tests in ODB */
   test_write();

   last_time = actual_time;
}

/*-- Clear histos --------------------------------------------------*/

INT clear_histos(INT id1, INT id2)
{
   INT i;
#ifndef MANA_LITE
   if (id1 != id2) {
      printf("Clear ID %d to ID %d\n", id1, id2);
      for (i = id1; i <= id2; i++)
         if (HEXIST(i))
            HRESET(i, bstr);
   } else {
      printf("Clear ID %d\n", id1);
      HRESET(id1, bstr);
   }
#endif
   return SUCCESS;
}

/*------------------------------------------------------------------*/

INT ana_callback(INT index, void *prpc_param[])
{
   if (index == RPC_ANA_CLEAR_HISTOS)
      clear_histos(CINT(0), CINT(1));

   return RPC_SUCCESS;
}

/*------------------------------------------------------------------*/

void send_event(INT index)
{
   EQUIPMENT_INFO *eq_info;
   EVENT_HEADER *pevent;
   INT i;

   pevent = (EVENT_HEADER *) event_buffer;

   eq_info = &equipment[index].info;

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
      equipment[index].bytes_sent += pevent->data_size + sizeof(EVENT_HEADER);
      equipment[index].events_sent++;

      equipment[index].stats.events_sent += equipment[index].events_sent;
      equipment[index].events_sent = 0;

      /* process event locally */
      process_event(pevent);

      /* send event to buffer */
      if (equipment[index].buffer_handle) {
         bm_send_event(equipment[index].buffer_handle, pevent,
                       pevent->data_size + sizeof(EVENT_HEADER), SYNC);

         /* flush buffer cache */
         bm_flush_cache(equipment[index].buffer_handle, SYNC);
      }
   } else
      equipment[index].serial_number--;

   for (i = 0; equipment[i].name[0]; i++)
      if (equipment[i].buffer_handle)
         bm_flush_cache(equipment[i].buffer_handle, SYNC);
}

/*------------------------------------------------------------------*/

void send_all_periodic_events(INT transition)
{
   EQUIPMENT_INFO *eq_info;
   INT i;

   for (i = 0; equipment[i].name[0]; i++) {
      eq_info = &equipment[i].info;

      if (!eq_info->enabled)
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

int print_message(const char *msg)
{
   char str[256];

   memset(str, ' ', 159);
   str[159] = 0;

   if (msg[0] == '[')
      msg = strchr(msg, ']') + 2;

   memcpy(str, msg, strlen(msg));
   ss_printf(0, 20, str);

   return 0;
}

/*------------------------------------------------------------------*/

void receive_message(HNDLE hBuf, HNDLE id, EVENT_HEADER * pheader, void *message)
{
   /* display message on screen */
   print_message(message);
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

      ss_printf(0, 0, "%s connected to %s. Press \"!\" to exit", fal_name, str);
      ss_printf(0, 1,
                "================================================================================");
      ss_printf(0, 2, "Run status:   %s",
                run_state == STATE_STOPPED ? "Stopped" : run_state ==
                STATE_RUNNING ? "Running" : "Paused");
      ss_printf(25, 2, "Run number %ld   ", run_number);
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

INT scheduler(void)
{
   EQUIPMENT_INFO *eq_info;
   EQUIPMENT *eq;
   ANALYZE_REQUEST *ar;
   EVENT_HEADER *pevent;
   DWORD actual_time, actual_millitime,
       last_time_network = 0, last_time_display = 0, readout_start, source;
   INT i, j, index, status, ch, size, flag;
   char str[80];

   pevent = (EVENT_HEADER *) event_buffer;

  /*----------------- MAIN equipment loop ------------------------------*/

   do {
      actual_millitime = ss_millitime();
      actual_time = ss_millitime();

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

            /* if period over, call readout routine */
            if (actual_time - eq->last_called >= (DWORD) eq_info->period) {
               eq->last_called = actual_time;

               /* compose MIDAS event header */
               pevent->event_id = eq_info->event_id;
               pevent->trigger_mask = eq_info->trigger_mask;
               pevent->data_size = 0;
               pevent->time_stamp = actual_time;
               pevent->serial_number = eq->serial_number++;

               /* call user readout routine */
               *((EQUIPMENT **) (pevent + 1)) = eq;
               pevent->data_size = eq->readout((char *) (pevent + 1), 0);

               /* send event */
               if (pevent->data_size) {
                  eq->bytes_sent += pevent->data_size;
                  eq->events_sent++;

                  /* process event locally */
                  process_event(pevent);

                  /* send event to buffer for remote consumer */
                  if (eq->buffer_handle) {
                     bm_send_event(eq->buffer_handle, pevent,
                                   pevent->data_size + sizeof(EVENT_HEADER), SYNC);

                     /* flush buffer cache */
                     bm_flush_cache(eq->buffer_handle, SYNC);
                  }

               } else
                  eq->serial_number--;

               eq->last_called = actual_time;
            }
         }

      /*---- check polled events ----*/
         if (eq_info->eq_type & EQ_POLLED) {
            /* compose MIDAS event header */
            pevent->event_id = eq_info->event_id;
            pevent->trigger_mask = eq_info->trigger_mask;
            pevent->data_size = 0;
            readout_start = actual_time;

            while ((source = poll_event(eq_info->source, eq->poll_count, FALSE))) {
               pevent->time_stamp = actual_time;
               pevent->serial_number = eq->serial_number++;

               /* put source at beginning of event */
               *(INT *) (pevent + 1) = source;

               /* call user readout routine */
               pevent->data_size = eq->readout((char *) (pevent + 1), 0);

               /* send event */
               if (pevent->data_size) {
                  if (eq->buffer_handle)
                     bm_send_event(eq->buffer_handle, pevent,
                                   pevent->data_size + sizeof(EVENT_HEADER), SYNC);

                  eq->bytes_sent += pevent->data_size;
                  eq->events_sent++;

                  /* analyze and log event */
                  process_event(pevent);
               } else
                  eq->serial_number--;

               actual_time = ss_millitime();

               /* repeat no more than 100 ms */
               if (actual_time - readout_start > 100)
                  break;

               /* quit if event limit is reached */
               if (eq_info->event_limit && eq->serial_number > eq_info->event_limit)
                  break;
            }

         }

      /*---- check if event limit is reached ----*/
         if (eq_info->eq_type != EQ_SLOW &&
             eq_info->event_limit &&
             eq->serial_number > eq_info->event_limit && run_state == STATE_RUNNING) {
            /* stop run */
            if (cm_transition(TR_STOP, 0, str, sizeof(str), SYNC, FALSE) != CM_SUCCESS) {
               cm_msg(MERROR, "Cannot stop run: %s", str);
            }

            /* check if autorestart, main loop will take care of it */
            size = sizeof(BOOL);
            flag = FALSE;
            db_get_value(hDB, 0, "/Logger/Auto restart", &flag, &size, TID_BOOL, TRUE);

            if (flag)
               auto_restart = ss_time() + 5;    /* start in 5 sec. */
         }
      }

    /*---- call frontend_loop periodically -------------------------*/
      if (frontend_call_loop) {
         status = frontend_loop();
         if (status != CM_SUCCESS)
            status = RPC_SHUTDOWN;
      }

    /*---- calculate rates and update status page periodically -----*/
      if (display_period && actual_time - last_time_display > (DWORD) display_period) {
         /* calculate rates */
         if (actual_time != last_time_display) {
            for (i = 0; equipment[i].name[0]; i++) {
               eq = &equipment[i];
               j = eq->serial_number;
               if (j > 0)
                  j--;
               eq->stats.events_sent = j;
               eq->stats.events_per_sec =
                   (DWORD) (eq->events_sent /
                            ((actual_time - last_time_display) / 1000.0));
               eq->stats.kbytes_per_sec =
                   eq->bytes_sent / 1024.0 / ((actual_time - last_time_display) / 1000.0);

               eq->bytes_sent = 0;
               eq->events_sent = 0;
            }

            for (i = 0; analyze_request[i].event_name[0]; i++) {
               ar = &analyze_request[i];
               ar->ar_stats.events_received += ar->events_received;
               ar->ar_stats.events_per_sec =
                   (DWORD) (ar->events_received /
                            ((actual_time - last_time_display) / 1000.0));
               ar->events_received = 0;
            }

         }

         /* propagate changes in equipment to ODB */
         db_send_changed_records();

         /* update statistics */
         update_stats();

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

         last_time_display = actual_time;
      }

    /*---- check network messages ----------------------------------*/
      if (run_state == STATE_RUNNING) {
         /* only call yield once every 100ms when running */
         if (actual_time - last_time_network > 100) {
            status = cm_yield(0);
            last_time_network = actual_time;
         } else
            status = RPC_SUCCESS;
      } else
         /* when run is stopped, call yield with 100ms timeout */
         status = cm_yield(100);

    /*---- check auto restart --------------------------------------*/
      if (auto_restart > 0 && (int) ss_time() > auto_restart) {
         auto_restart = FALSE;
         size = sizeof(run_number);
         db_get_value(hDB, 0, "/Runinfo/Run number", &run_number, &size, TID_INT, TRUE);

         cm_msg(MTALK, "main", "starting new run");
         status = cm_transition(TR_START, run_number + 1, NULL, 0, ASYNC, FALSE);
         if (status != CM_SUCCESS)
            cm_msg(MERROR, "main", "cannot restart run");
      }

   } while (status != RPC_SHUTDOWN && status != SS_ABORT);

   return status;
}

/*------------------------------------------------------------------*/

INT cnaf_callback(INT index, void *prpc_param[])
{
   DWORD cmd, b, c, n, a, f, *pdword, *size, *x, *q;
   WORD *pword, *pdata;
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
               cam16i_q(c, n, a, f, pword, (int *) x, (int *) q);
            else
               cam16o_q(c, n, a, f, pword[i], (int *) x, (int *) q);
      } else {
         for (i = 0; i < count; i++)
            if (f < 16)
               cam24i_q(c, n, a, f, pdword, (int *) x, (int *) q);
            else
               cam24o_q(c, n, a, f, pdword[i], (int *) x, (int *) q);
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
      printf("cnaf: Unknown command 0x%lX\n", cmd);
   }

   printf("cmd=%ld c=%ld n=%ld a=%ld f=%ld d=%lX\n", cmd, c, n, a, f, pdword[0]);

   return RPC_SUCCESS;
}

/*------------------------------------------------------------------*/

#ifdef OS_VXWORKS
int mfe(char *ahost_name, char *aexp_name, BOOL adebug)
#else
int main(int argc, char *argv[])
#endif
{
   INT status, i;
   BOOL debug;

   host_name[0] = 0;
   exp_name[0] = 0;
   debug = FALSE;

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
      else if (argv[i][0] == '-') {
         if (i + 1 >= argc || argv[i + 1][0] == '-')
            goto usage;
         if (argv[i][1] == 'e')
            strcpy(exp_name, argv[++i]);
         else if (argv[i][1] == 'h')
            strcpy(host_name, argv[++i]);
#ifndef MANA_LITE
         else if (argv[i][1] == 's')
            pawc_size = atoi(argv[++i]);
#endif
         else {
          usage:
            printf("usage: fal [-h Hostname] [-e Experiment] [-s pawc_size] [-d]\n\n");
            printf("-d(ebug) should be set if FAL is started inside a debugger\n");
            return 1;
         }
      }
   }
#endif

   /* now connect to server */
   if (host_name[0])
      printf("Connect to experiment %s on host %s...", exp_name, host_name);
   else
      printf("Connect to experiment %s...", exp_name);

   status = cm_connect_experiment1(host_name, exp_name, fal_name, NULL, odb_size,
                                   LOGGER_TIMEOUT);
   if (status != CM_SUCCESS) {
      /* let user read message before window might close */
      ss_sleep(5000);
      return 1;
   }

   printf("OK\n");

   /* set own message print function */
   cm_set_msg_print(MT_ALL, MT_ALL, print_message);

   /* check if FAL already running */
   status = cm_exist(fal_name, FALSE);
   if (status == CM_SUCCESS) {
      /* try to cleanup hanging FAL */
      cm_cleanup(fal_name, FALSE);

      /* let user read message */
      ss_sleep(2000);

      /* now check again */
      status = cm_exist(fal_name, FALSE);
      if (status == CM_SUCCESS) {
         printf("This program runs already and cannot be started twice.\n");
         printf("<Please hit return to exit>");
         getchar();
         cm_disconnect_experiment();
         return 1;
      }

      /* now reconnect to get client name FAL, not FAL1 */
      cm_disconnect_experiment();
      cm_connect_experiment1(host_name, exp_name, fal_name, NULL, odb_size,
                             LOGGER_TIMEOUT);
   }

   if (cm_register_transition(TR_START, tr_start1, 500) != CM_SUCCESS ||
       cm_register_transition(TR_STOP, tr_stop, 500) != CM_SUCCESS ||
       cm_register_transition(TR_PAUSE, tr_pause, 500) != CM_SUCCESS ||
       cm_register_transition(TR_RESUME, tr_resume, 500) != CM_SUCCESS) {
      printf("Failed to start local RPC server");
      return 1;
   }

   cm_get_experiment_database(&hDB, NULL);

   /* set online mode */
   i = 1;
   db_set_value(hDB, 0, "/Runinfo/Online Mode", &i, sizeof(int), 1, TID_INT);

   /* turn off watchdog if in debug mode */
   if (debug)
      cm_set_watchdog_params(TRUE, 0);

   /* set own message print function */
   cm_set_msg_print(MT_ALL, MT_ALL, print_message);

   /* register message receiver */
   cm_msg_register(receive_message);

   /* register CNAF callback */
   cm_register_function(RPC_CNAF16, cnaf_callback);
   cm_register_function(RPC_CNAF24, cnaf_callback);

   /* register callback for clearing histos */
   cm_register_function(RPC_ANA_CLEAR_HISTOS, ana_callback);

   /* register callback for rewinding tapes */
   cm_register_function(RPC_LOG_REWIND, log_callback);

   /* call frontend init function */
   printf("Init hardware...");
   if (frontend_init() != CM_SUCCESS) {
      printf("Cannot initialize hardware.\n");
      printf("<Please hit return to exit>");
      getchar();
      cm_disconnect_experiment();
      return 1;
   }
   printf("OK\n");

   /* reqister equipment in ODB */
   register_equipment();

   /* initialize module parameters */
   if (init_module_parameters() != CM_SUCCESS) {
      cm_disconnect_experiment();
      return 1;
   }

   /* init logger */
   logger_init();

   /* call analyzer init function */
   printf("Init analyzer...");
   if (mana_init() != CM_SUCCESS) {
      printf("Cannot initialize analyzer.\n");
      printf("<Please hit return to exit>");
      getchar();
      cm_disconnect_experiment();
      return 0;
   }
   printf("OK\n");

   /* reqister requests in ODB */
   register_requests();

   /* open history logging */
   open_history();

   /* initialize screen display */
   ss_sleep(2000);
   display(TRUE);

   /* initialize ss_getchar */
   ss_getchar(0);

   /* call main scheduler loop */
   scheduler();

   /* call user exit function */
   frontend_exit();

   /* close slow control drivers */
   for (i = 0; equipment[i].name[0]; i++)
      if ((equipment[i].info.eq_type & EQ_SLOW) && equipment[i].status == FE_SUCCESS)
         equipment[i].cd(CMD_EXIT, &equipment[i]);

   /* close history logging */
   close_history();

   /* close network connection to server */
   cm_disconnect_experiment();

   ss_clear_screen();
   ss_printf(0, 0, "%s shut down.", fal_name);
   ss_printf(0, 1, "");

   return 0;
}
