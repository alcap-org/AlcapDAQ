/********************************************************************\

  Name:         mserver.c
  Created by:   Stefan Ritt

  Contents:     Server program for midas RPC calls

  $Log: mserver.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:36  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.54  2004/09/30 16:13:02  midas
  Added more debugging info

  Revision 1.53  2004/09/28 21:02:12  midas
  Become a daemon before registering the socket server

  Revision 1.52  2004/09/28 20:46:44  midas
  Fixed bug in debug_print

  Revision 1.51  2004/09/28 20:37:03  midas
  Added more debugging info

  Revision 1.50  2004/09/28 20:21:05  midas
  Fixed missing LF in debug output

  Revision 1.49  2004/09/28 20:05:59  midas
  Revised debug logging for mserver

  Revision 1.48  2004/09/28 18:11:35  midas
  Added -D daemon mode

  Revision 1.47  2004/09/28 17:13:21  midas
  Added startup debug code for mserver

  Revision 1.46  2004/09/28 16:51:14  midas
  Put mserver.log into /tmp under Linux

  Revision 1.45  2004/01/18 09:56:32  olchansk
  ss_thread_create() does not return SS_NO_THREAD

  Revision 1.44  2004/01/08 08:40:10  midas
  Implemented standard indentation

  Revision 1.43  2003/12/12 10:54:14  midas
  Fixed wrong index in call to db_remove_open_record()

  Revision 1.42  2003/12/12 09:19:08  midas
  Fixed wrong parameter in call to cm_get_watchdog_info()

  Revision 1.41  2003/11/24 08:22:46  midas
  Changed timeouts from INT to DWORD, added ignore_timeout to cm_cleanup, adde '-f' flag to ODBEdit 'cleanup'

  Revision 1.40  2003/11/20 11:29:44  midas
  Implemented db_check_record and use it in most places instead of db_create_record

  Revision 1.39  2003/04/24 13:47:38  midas
  Fixed compiler warning

  Revision 1.38  2003/04/15 12:52:34  midas
  Removed db_show_mem

  Revision 1.37  2003/01/14 11:24:07  midas
  Write error to midas.log if user does not exist

  Revision 1.36  2002/10/21 00:25:14  olchansk
  catch/ignore SIGPIPE

  Revision 1.35  2002/09/26 09:34:37  midas
  Log startup messages only in debugging mode

  Revision 1.34  2002/09/13 07:32:48  midas
  Added client name to cm_cleanup()

  Revision 1.33  2002/06/10 22:58:07  pierre
  add lock flag in remove_record

  Revision 1.32  2002/05/22 05:43:33  midas
  Added extra variables to hs_enum_vars for mhist to display array size

  Revision 1.31  2002/05/10 01:41:19  midas
  Added optional debug output to cm_transition

  Revision 1.30  2002/05/08 19:54:41  midas
  Added extra parameter to function db_get_value()

  Revision 1.29  2002/03/14 14:15:44  midas
  Fixed bug with setuid

  Revision 1.28  2001/08/07 12:51:50  midas
  If no full path is given as argv[0] (line under xinetd), use
  /usr/local/bin/mserver as default

  Revision 1.27  2001/04/23 08:25:07  midas
  Only call setuid() when started as root

  Revision 1.26  2000/11/14 08:17:05  midas
  Added number of messages for cm_msg_retrieve and in odbedit "old" command

  Revision 1.25  2000/09/29 16:29:52  pierre
  - add BOOL arg in db_get_open_records

  Revision 1.24  2000/08/21 07:05:48  midas
  Added cm_msg_log1(...,facility) to be compatible with older programs

  Revision 1.23  2000/08/11 11:43:51  midas
  Added cm_msg1 to produce messages which go to a differnt logging file

  Revision 1.22  1999/11/22 08:44:25  midas
  Fixed missing parameter in cm_set_client_info

  Revision 1.21  1999/11/08 15:06:26  midas
  Added new parameters to al_trigger_alarm

  Revision 1.20  1999/10/08 22:15:04  midas
  Added ftruncate for LINUX

  Revision 1.19  1999/09/27 13:02:32  midas
  Added new el_submit in mserver

  Revision 1.18  1999/09/17 11:48:07  midas
  Alarm system half finished

  Revision 1.17  1999/09/15 13:33:36  midas
  Added remote el_submit functionality

  Revision 1.16  1999/08/03 11:15:07  midas
  Added bm_skip_event

  Revision 1.15  1999/05/05 12:02:34  midas
  Added and modified history functions, added db_set_num_values

  Revision 1.14  1999/05/03 10:45:06  midas
  Fixed compiler warning with rpc_set_debug

  Revision 1.13  1999/04/30 10:58:59  midas
  Added -D debug to screen for mserver

  Revision 1.12  1999/04/28 15:27:29  midas
  Made hs_read working for Java

  Revision 1.11  1999/04/19 07:47:39  midas
  Added cm_msg_retrieve

  Revision 1.10  1999/04/15 09:57:52  midas
  Added cm_exist, modified db_get_key_info

  Revision 1.9  1999/04/13 12:20:44  midas
  Added db_get_data1 (for Java)

  Revision 1.8  1999/04/08 15:26:55  midas
  Added cm_transition and db_get_key_info

  Revision 1.7  1999/02/11 13:15:51  midas
  Made cm_msg callable from a remote client (for Java)

  Revision 1.6  1999/02/09 14:38:11  midas
  Added debug logging facility

  Revision 1.5  1999/02/05 23:39:39  pierre
  - Add commented function print_rpc_string(index)

  Revision 1.4  1999/01/21 23:10:23  pierre
  - change ss_create_thread() to ss_thread_create()

  Revision 1.3  1999/01/13 09:40:48  midas
  Added db_set_data_index2 function

  Revision 1.2  1998/10/12 12:19:02  midas
  Added Log tag in header


\********************************************************************/

#include "midas.h"
#include "msystem.h"
#include "strlcpy.h"

#ifdef OS_UNIX
#include <grp.h>
#include <sys/types.h>
#endif

#ifdef OS_WINNT
/* critical section object for open/close buffer */
CRITICAL_SECTION buffer_critial_section;
#endif

struct callback_addr callback;

INT rpc_server_dispatch(INT index, void *prpc_param[]);

/*---- msg_print ---------------------------------------------------*/

INT msg_print(const char *msg)
{
   /* print message to system log */
   ss_syslog(msg);

   /* print message to stdout */
   return puts(msg);
}

/*---- debug_print -------------------------------------------------*/

void debug_print(char *msg)
{
   FILE *f;
   char file_name[256], str[1000];
   static char client_name[NAME_LENGTH] = "";
   static DWORD start_time = 0;

   if (!start_time)
      start_time = ss_millitime();

   /* print message to file */
#ifdef OS_LINUX
   strcpy(file_name, "/tmp/mserver.log");
#else
   getcwd(file_name, sizeof(file_name));
   if (file_name[strlen(file_name)-1] != DIR_SEPARATOR)
      strcat(file_name, DIR_SEPARATOR_STR);
   strcat(file_name, "mserver.log");
#endif
   f = fopen(file_name, "a");

   if (!client_name[0])
      cm_get_client_info(client_name);

   sprintf(str, "%10.3lf [%d,%s,%s] ", (ss_millitime()-start_time)/1000.0, 
      ss_getpid(), callback.host_name, client_name);
   strlcat(str, msg, sizeof(str));
   strlcat(str, "\n", sizeof(str));

   if (f != NULL) {
      fprintf(f, str);
      fclose(f);
   } else {
      printf("Cannot open \"%s\": %s\n", file_name, strerror(errno));
   }
}

/*---- main --------------------------------------------------------*/

int main(int argc, char **argv)
/********************************************************************\

  Routine: main (server.exe)

  Purpose: Main routine for MIDAS server process. If called one
           parameter, it listens for a connection under
           MIDAS_TCP_PORT. If a connection is requested, the action
           depends on the parameter:

           0: Single process server: This process executes RPC calls
              even if there are several connections.

           1: Multi thread server (only Windows NT): For each conn-
              ection, a seperate thread is started which servers this
              connection.

           2: Multi process server (Not MS-DOS):  For each conn-
              ection, a seperate subprocess is started which servers 
              this connection. The subprocess is again server.exe, now
              with four parameters: the IP address of the calling 
              host, the two port numbers used by the calling
              host and optionally the name of the experiment to connect
              to. With this information it calles back the client.

           This technique is necessary since the usual fork() doesn't
           work on some of the supported systems.

  Input:
    int  argc               Number of parameters in command line
    char **argv             Command line parameters:

                            ip-addr     callback address as longword
                            port-no     port number
                            program     program name as string
                            experiment  experiment name as string


  Output:
    none

  Function value:
    BM_SUCCESS              Successful completion

\********************************************************************/
{
   int i, flag, size, server_type;
   char name[256], str[1000];
   BOOL inetd, daemon, debug;

#ifdef OS_WINNT
   /* init critical section object for open/close buffer */
   InitializeCriticalSection(&buffer_critial_section);
#endif

#if defined(SIGPIPE) && defined(SIG_IGN)
   signal(SIGPIPE, SIG_IGN);
#endif

   /* save executable file name */
   if (argv[0] == NULL || argv[0][0] == 0)
      strcpy(name, "mserver");
   else
      strcpy(name, argv[0]);

#ifdef OS_UNIX
   /* if no full path given, assume /usr/local/bin */
   if (strchr(name, '/') == 0)
      strcpy(name, "/usr/local/bin/mserver");
#endif

   rpc_set_server_option(RPC_OSERVER_NAME, (PTYPE) name);

   /* redirect message print */
   cm_set_msg_print(MT_ALL, MT_ALL, msg_print);

   /* find out if we were started by inetd */
   size = sizeof(int);
   inetd = (getsockopt(0, SOL_SOCKET, SO_TYPE, (void *) &flag, (void *) &size) == 0);

   if (argc < 7 && inetd) {
      /* accept connection from stdin */
      rpc_set_server_option(RPC_OSERVER_TYPE, ST_MPROCESS);
      rpc_server_accept(0);

      return 0;
   }

   if (!inetd && argc < 7)
      printf("%s started interactively\n", argv[0]);

   if (argc < 7) {
      debug = daemon = FALSE;
      server_type = ST_MPROCESS;

      /* parse command line parameters */
      for (i = 1; i < argc; i++) {
         if (argv[i][0] == '-' && argv[i][1] == 'd')
            debug = TRUE;
         else if (argv[i][0] == '-' && argv[i][1] == 'D')
            daemon = TRUE;
         else if (argv[i][0] == '-' && argv[i][1] == 's')
            server_type = ST_SINGLE;
         else if (argv[i][0] == '-' && argv[i][1] == 't')
            server_type = ST_MTHREAD;
         else if (argv[i][0] == '-' && argv[i][1] == 'm')
            server_type = ST_MPROCESS;
         else if (argv[i][0] == '-') {
            if (i + 1 >= argc || argv[i + 1][0] == '-')
               goto usage;
            else {
             usage:
               printf("usage: mserver [-s][-t][-m][-d]\n");
               printf("               -s    Single process server\n");
               printf("               -t    Multi threaded server\n");
               printf("               -m    Multi process server (default)\n");
#ifdef OS_LINUX
               printf("               -D    Become a daemon\n");
               printf("               -d    Write debug info to stdout or to \"/tmp/mserver.log\"\n\n");
#else
               printf("               -d    Write debug info\"\n\n");
#endif
               return 0;
            }
         }
      }

      /* turn on debugging */
      if (debug) {
         if (daemon || inetd)
            rpc_set_debug(debug_print, 1);
         else
            rpc_set_debug((void (*)(char *)) puts, 1);

         sprintf(str, "Arguments: ");
         for (i=0 ; i<argc ; i++)
            sprintf(str+strlen(str), " %s", argv[i]);
         rpc_debug_printf(str);

         rpc_debug_printf("Debugging mode is on");
      }

      /* if command line parameter given, start according server type */
      if (server_type == ST_MTHREAD) {
         if (ss_thread_create(NULL, NULL) == 0) {
            printf("MIDAS doesn't support threads on this OS.\n");
            return 0;
         }

         printf
             ("NOTE: THE MULTI THREADED SERVER IS BUGGY, ONLY USE IT FOR TEST PURPOSES\n");
         printf("Multi thread server started\n");
      }

      /* become a daemon */
      if (daemon) {
         printf("Becoming a daemon...\n");
         ss_daemon_init(FALSE);
      }

      /* register server */
      if (rpc_register_server(server_type, argv[0],
                              NULL, rpc_server_dispatch) != RPC_SUCCESS) {
         printf("Cannot start server\n");
         return 0;
      }

      /* register MIDAS library functions */
      rpc_register_functions(rpc_get_internal_list(1), rpc_server_dispatch);

      /* run forever */
      while (ss_suspend(5000, 0) != RPC_SHUTDOWN);
   } else {
      
      /* here we come if this program is started as a subprocess */

      memset(&callback, 0, sizeof(callback));

      /* extract callback arguments and start receiver */
#ifdef OS_VMS
      strcpy(callback.host_name, argv[2]);
      callback.host_port1 = atoi(argv[3]);
      callback.host_port2 = atoi(argv[4]);
      callback.host_port3 = atoi(argv[5]);
      callback.debug = atoi(argv[6]);
      if (argc > 7)
         strcpy(callback.experiment, argv[7]);
      if (argc > 8)
         strcpy(callback.directory, argv[8]);
      if (argc > 9)
         strcpy(callback.user, argv[9]);
#else
      strcpy(callback.host_name, argv[1]);
      callback.host_port1 = atoi(argv[2]);
      callback.host_port2 = atoi(argv[3]);
      callback.host_port3 = atoi(argv[4]);
      callback.debug = atoi(argv[5]);
      if (argc > 6)
         strcpy(callback.experiment, argv[6]);
      if (argc > 7)
         strcpy(callback.directory, argv[7]);
      if (argc > 8)
         strcpy(callback.user, argv[8]);
#endif
      callback.index = 0;

      if (callback.debug) {
         rpc_set_debug(debug_print, 1);
         if (callback.directory[0]) {
            if (callback.user[0])
               rpc_debug_printf("Start subprocess in %s under user %s",
                      callback.directory, callback.user);
            else
               rpc_debug_printf("Start subprocess in %s", callback.directory);

         } else
            rpc_debug_printf("Start subprocess in current directory");
      }

      /* change the directory and uid */
      if (callback.directory[0])
         if (chdir(callback.directory) != 0)
            rpc_debug_printf("Cannot change to directory \"%s\"",
                             callback.directory);

#ifdef OS_UNIX

      /* under UNIX, change user and group ID if started under root */
      if (callback.user[0] && geteuid() == 0) {
         struct passwd *pw;

         pw = getpwnam(callback.user);
         if (pw == NULL) {
            rpc_debug_printf("Cannot change UID, unknown user \"%s\"",
                             callback.user);
            cm_msg(MERROR, "main", "Cannot change UID, unknown user \"%s\"",
                   callback.user);
         } else {
            if (setgid(pw->pw_gid) < 0 || initgroups(pw->pw_name, pw->pw_gid) < 0) {
               rpc_debug_printf("Unable to set group premission for user %s",
                                callback.user);
               cm_msg(MERROR, "main", "Unable to set group premission for user %s",
                      callback.user);
            } else {
               if (setuid(pw->pw_uid) < 0) {
                  rpc_debug_printf("Unable to set user ID for %s", callback.user);
                  cm_msg(MERROR, "main", "Unable to set user ID for %s", callback.user);
               } else {
                  rpc_debug_printf("Changed UID to user %s (GID %d, UID %d)",
                         callback.user, pw->pw_gid, pw->pw_uid);
                  cm_msg(MLOG, "main", "Changed UID to user %s (GID %d, UID %d)",
                         callback.user, pw->pw_gid, pw->pw_uid);
               }
            }
         }
      }
#endif

      rpc_register_server(ST_SUBPROCESS, NULL, NULL, rpc_server_dispatch);

      /* register MIDAS library functions */
      rpc_register_functions(rpc_get_internal_list(1), rpc_server_dispatch);

      rpc_server_thread(&callback);
   }

   return BM_SUCCESS;
}

/*------------------------------------------------------------------*/

/* just a small test routine which doubles numbers */
INT rpc_test(BYTE b, WORD w, INT i, float f, double d,
             BYTE * b1, WORD * w1, INT * i1, float *f1, double *d1)
{
   printf("rpc_test: %d %d %d %1.1f %1.1lf\n", b, w, i, f, d);

   *b1 = b * 2;
   *w1 = w * 2;
   *i1 = i * 2;
   *f1 = f * 2;
   *d1 = d * 2;

   return 1;
}

/*----- rpc_server_dispatch ----------------------------------------*/

INT rpc_server_dispatch(INT index, void *prpc_param[])
/********************************************************************\

  Routine: rpc_server_dispatch

  Purpose: This routine gets registered as the callback function
           for all RPC calls via rpc_server_register. It acts as a
           stub to call the local midas subroutines. It uses the
           Cxxx(i) macros to cast parameters in prpc_param to
           their appropriate types.

  Input:
    int    index            RPC index defined in RPC.H
    void   *prpc_param      pointer to rpc parameter array

  Output:
    none

  Function value:
    status                  status returned from local midas function

\********************************************************************/
{
   INT status = 0;
   INT convert_flags;

   convert_flags = rpc_get_server_option(RPC_CONVERT_FLAGS);

   switch (index) {
      /* common functions */

   case RPC_CM_SET_CLIENT_INFO:
      status = cm_set_client_info(CHNDLE(0), CPHNDLE(1), CSTRING(2),
                                  CSTRING(3), CINT(4), CSTRING(5), CINT(6));
      break;

   case RPC_CM_SET_WATCHDOG_PARAMS:
      status = cm_set_watchdog_params(CBOOL(0), CINT(1));
      break;

   case RPC_CM_CLEANUP:
      status = cm_cleanup(CSTRING(0), CBOOL(1));
      break;

   case RPC_CM_GET_WATCHDOG_INFO:
      status = cm_get_watchdog_info(CHNDLE(0), CSTRING(1), CPDWORD(2), CPDWORD(3));
      break;

   case RPC_CM_MSG:
      status = cm_msg(CINT(0), CSTRING(1), CINT(2), CSTRING(3), CSTRING(4));
      break;

   case RPC_CM_MSG_LOG:
      status = cm_msg_log(CINT(0), CSTRING(1));
      break;

   case RPC_CM_MSG_LOG1:
      status = cm_msg_log1(CINT(0), CSTRING(1), CSTRING(2));
      break;

   case RPC_CM_EXECUTE:
      status = cm_execute(CSTRING(0), CSTRING(1), CINT(2));
      break;

   case RPC_CM_EXIST:
      status = cm_exist(CSTRING(0), CBOOL(1));
      break;

   case RPC_CM_SYNCHRONIZE:
      status = cm_synchronize(CPDWORD(0));
      break;

   case RPC_CM_ASCTIME:
      status = cm_asctime(CSTRING(0), CINT(1));
      break;

   case RPC_CM_TIME:
      status = cm_time(CPDWORD(0));
      break;

   case RPC_CM_MSG_RETRIEVE:
      status = cm_msg_retrieve(CINT(0), CSTRING(1), CPINT(2));
      break;

      /* buffer manager functions */

   case RPC_BM_OPEN_BUFFER:

#ifdef OS_WINNT
      /*
         bm_open_buffer may only be called from one thread at a time,
         so use critical section object for synchronization.
       */
      EnterCriticalSection(&buffer_critial_section);
#endif

      status = bm_open_buffer(CSTRING(0), CINT(1), CPINT(2));

#ifdef OS_WINNT
      LeaveCriticalSection(&buffer_critial_section);
#endif

      break;

   case RPC_BM_CLOSE_BUFFER:

#ifdef OS_WINNT
      /*
         bm_close_buffer may only be called from one thread at a time,
         so use critical section object for synchronization.
       */
      EnterCriticalSection(&buffer_critial_section);
#endif

      status = bm_close_buffer(CINT(0));

#ifdef OS_WINNT
      LeaveCriticalSection(&buffer_critial_section);
#endif

      break;

   case RPC_BM_CLOSE_ALL_BUFFERS:

#ifdef OS_WINNT
      /*
         bm_close_all_buffers may only be called from one thread at a time,
         so use critical section object for synchronization.
       */
      EnterCriticalSection(&buffer_critial_section);
#endif

      status = bm_close_all_buffers();

#ifdef OS_WINNT
      LeaveCriticalSection(&buffer_critial_section);
#endif

      break;

   case RPC_BM_GET_BUFFER_INFO:
      status = bm_get_buffer_info(CINT(0), CARRAY(1));
      if (convert_flags) {
         BUFFER_HEADER *pb;

         /* convert event header */
         pb = (BUFFER_HEADER *) CARRAY(1);
         rpc_convert_single(&pb->num_clients, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pb->max_client_index, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pb->size, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pb->read_pointer, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pb->write_pointer, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pb->num_in_events, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pb->num_out_events, TID_INT, RPC_OUTGOING, convert_flags);
      }
      break;

   case RPC_BM_GET_BUFFER_LEVEL:
      status = bm_get_buffer_level(CINT(0), CPINT(1));
      break;

   case RPC_BM_INIT_BUFFER_COUNTERS:
      status = bm_init_buffer_counters(CINT(0));
      break;

   case RPC_BM_SET_CACHE_SIZE:
      status = bm_set_cache_size(CINT(0), CINT(1), CINT(2));
      break;

   case RPC_BM_ADD_EVENT_REQUEST:
      status = bm_add_event_request(CINT(0), CSHORT(1),
                                    CSHORT(2), CINT(3),
                                    (void (*)(HNDLE, HNDLE, EVENT_HEADER *, void *))
                                    CINT(4), CINT(5));
      break;

   case RPC_BM_REMOVE_EVENT_REQUEST:
      status = bm_remove_event_request(CINT(0), CINT(1));
      break;

   case RPC_BM_SEND_EVENT:
      if (convert_flags) {
         EVENT_HEADER *pevent;

         /* convert event header */
         pevent = (EVENT_HEADER *) CARRAY(1);
         rpc_convert_single(&pevent->event_id, TID_SHORT, 0, convert_flags);
         rpc_convert_single(&pevent->trigger_mask, TID_SHORT, 0, convert_flags);
         rpc_convert_single(&pevent->serial_number, TID_DWORD, 0, convert_flags);
         rpc_convert_single(&pevent->time_stamp, TID_DWORD, 0, convert_flags);
         rpc_convert_single(&pevent->data_size, TID_DWORD, 0, convert_flags);
      }

      status = bm_send_event(CINT(0), CARRAY(1), CINT(2), CINT(3));
      break;

   case RPC_BM_RECEIVE_EVENT:
      status = bm_receive_event(CINT(0), CARRAY(1), CPINT(2), CINT(3));
      break;

   case RPC_BM_SKIP_EVENT:
      status = bm_skip_event(CINT(0));
      break;

   case RPC_BM_FLUSH_CACHE:
      status = bm_flush_cache(CINT(0), CINT(1));
      break;

   case RPC_BM_MARK_READ_WAITING:
      status = bm_mark_read_waiting(CBOOL(0));
      break;

   case RPC_BM_EMPTY_BUFFERS:
      status = bm_empty_buffers();
      break;

      /* database functions */

   case RPC_DB_OPEN_DATABASE:

#ifdef OS_WINNT
      /*
         db_open_database may only be called from one thread at a time,
         so use critical section object for synchronization.
       */
      EnterCriticalSection(&buffer_critial_section);
#endif

      status = db_open_database(CSTRING(0), CINT(1), CPHNDLE(2), CSTRING(3));

#ifdef OS_WINNT
      LeaveCriticalSection(&buffer_critial_section);
#endif

      break;

   case RPC_DB_CLOSE_DATABASE:

#ifdef OS_WINNT
      /*
         db_close_database may only be called from one thread at a time,
         so use critical section object for synchronization.
       */
      EnterCriticalSection(&buffer_critial_section);
#endif

      status = db_close_database(CINT(0));

#ifdef OS_WINNT
      LeaveCriticalSection(&buffer_critial_section);
#endif

      break;

   case RPC_DB_FLUSH_DATABASE:
      status = db_flush_database(CINT(0));
      break;

   case RPC_DB_CLOSE_ALL_DATABASES:

#ifdef OS_WINNT
      /*
         db_close_allo_databases may only be called from one thread at a time,
         so use critical section object for synchronization.
       */
      EnterCriticalSection(&buffer_critial_section);
#endif

      status = db_close_all_databases();

#ifdef OS_WINNT
      LeaveCriticalSection(&buffer_critial_section);
#endif

      break;

   case RPC_DB_CREATE_KEY:
      status = db_create_key(CHNDLE(0), CHNDLE(1), CSTRING(2), CDWORD(3));
      break;

   case RPC_DB_CREATE_LINK:
      status = db_create_link(CHNDLE(0), CHNDLE(1), CSTRING(2), CSTRING(3));
      break;

   case RPC_DB_SET_VALUE:
      rpc_convert_data(CARRAY(3), CDWORD(6), RPC_FIXARRAY, CINT(4), convert_flags);
      status =
          db_set_value(CHNDLE(0), CHNDLE(1), CSTRING(2), CARRAY(3), CINT(4), CINT(5),
                       CDWORD(6));
      break;

   case RPC_DB_GET_VALUE:
      rpc_convert_data(CARRAY(3), CDWORD(5), RPC_FIXARRAY, CINT(4), convert_flags);
      status =
          db_get_value(CHNDLE(0), CHNDLE(1), CSTRING(2), CARRAY(3), CPINT(4), CDWORD(5),
                       CBOOL(6));
      rpc_convert_data(CARRAY(3), CDWORD(5), RPC_FIXARRAY | RPC_OUTGOING, CINT(4),
                       convert_flags);
      break;

   case RPC_DB_FIND_KEY:
      status = db_find_key(CHNDLE(0), CHNDLE(1), CSTRING(2), CPHNDLE(3));
      break;

   case RPC_DB_FIND_LINK:
      status = db_find_link(CHNDLE(0), CHNDLE(1), CSTRING(2), CPHNDLE(3));
      break;

   case RPC_DB_GET_PATH:
      status = db_get_path(CHNDLE(0), CHNDLE(1), CSTRING(2), CINT(3));
      break;

   case RPC_DB_DELETE_KEY:
      status = db_delete_key(CHNDLE(0), CHNDLE(1), CBOOL(2));
      break;

   case RPC_DB_ENUM_KEY:
      status = db_enum_key(CHNDLE(0), CHNDLE(1), CINT(2), CPHNDLE(3));
      break;

   case RPC_DB_ENUM_LINK:
      status = db_enum_link(CHNDLE(0), CHNDLE(1), CINT(2), CPHNDLE(3));
      break;

   case RPC_DB_GET_NEXT_LINK:
      status = db_get_next_link(CHNDLE(0), CHNDLE(1), CPHNDLE(2));
      break;

   case RPC_DB_GET_KEY:
      status = db_get_key(CHNDLE(0), CHNDLE(1), CARRAY(2));
      if (convert_flags) {
         KEY *pkey;

         pkey = (KEY *) CARRAY(2);
         rpc_convert_single(&pkey->type, TID_DWORD, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pkey->num_values, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pkey->data, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pkey->total_size, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pkey->item_size, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pkey->access_mode, TID_WORD, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pkey->notify_count, TID_WORD, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pkey->next_key, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pkey->parent_keylist, TID_INT, RPC_OUTGOING, convert_flags);
         rpc_convert_single(&pkey->last_written, TID_INT, RPC_OUTGOING, convert_flags);
      }
      break;

   case RPC_DB_GET_KEY_INFO:
      status =
          db_get_key_info(CHNDLE(0), CHNDLE(1), CSTRING(2), CINT(3), CPINT(4), CPINT(5),
                          CPINT(6));
      break;

   case RPC_DB_GET_KEY_TIME:
      status = db_get_key_time(CHNDLE(0), CHNDLE(1), CPDWORD(2));
      break;

   case RPC_DB_RENAME_KEY:
      status = db_rename_key(CHNDLE(0), CHNDLE(1), CSTRING(2));
      break;

   case RPC_DB_REORDER_KEY:
      status = db_reorder_key(CHNDLE(0), CHNDLE(1), CINT(2));
      break;

   case RPC_DB_GET_DATA:
      status = db_get_data(CHNDLE(0), CHNDLE(1), CARRAY(2), CPINT(3), CDWORD(4));
      rpc_convert_data(CARRAY(2), CDWORD(4), RPC_FIXARRAY | RPC_OUTGOING, CINT(3),
                       convert_flags);
      break;

   case RPC_DB_GET_DATA1:
      status =
          db_get_data1(CHNDLE(0), CHNDLE(1), CARRAY(2), CPINT(3), CDWORD(4), CPINT(5));
      rpc_convert_data(CARRAY(2), CDWORD(4), RPC_FIXARRAY | RPC_OUTGOING, CINT(3),
                       convert_flags);
      break;

   case RPC_DB_GET_DATA_INDEX:
      status =
          db_get_data_index(CHNDLE(0), CHNDLE(1), CARRAY(2), CPINT(3), CINT(4),
                            CDWORD(5));
      rpc_convert_single(CARRAY(2), CDWORD(5), RPC_OUTGOING, convert_flags);
      break;

   case RPC_DB_SET_DATA:
      rpc_convert_data(CARRAY(2), CDWORD(5), RPC_FIXARRAY, CINT(3), convert_flags);
      status = db_set_data(CHNDLE(0), CHNDLE(1), CARRAY(2), CINT(3), CINT(4), CDWORD(5));
      break;

   case RPC_DB_SET_DATA_INDEX:
      rpc_convert_single(CARRAY(2), CDWORD(5), 0, convert_flags);
      status =
          db_set_data_index(CHNDLE(0), CHNDLE(1), CARRAY(2), CINT(3), CINT(4), CDWORD(5));
      break;

   case RPC_DB_SET_DATA_INDEX2:
      rpc_convert_single(CARRAY(2), CDWORD(5), 0, convert_flags);
      status =
          db_set_data_index2(CHNDLE(0), CHNDLE(1), CARRAY(2), CINT(3), CINT(4), CDWORD(5),
                             CBOOL(6));
      break;

   case RPC_DB_SET_NUM_VALUES:
      status = db_set_num_values(CHNDLE(0), CHNDLE(1), CINT(2));
      break;

   case RPC_DB_SET_MODE:
      status = db_set_mode(CHNDLE(0), CHNDLE(1), CWORD(2), CBOOL(3));
      break;

   case RPC_DB_GET_RECORD:
      status = db_get_record(CHNDLE(0), CHNDLE(1), CARRAY(2), CPINT(3), CINT(4));
      break;

   case RPC_DB_SET_RECORD:
      status = db_set_record(CHNDLE(0), CHNDLE(1), CARRAY(2), CINT(3), CINT(4));
      break;

   case RPC_DB_GET_RECORD_SIZE:
      status = db_get_record_size(CHNDLE(0), CHNDLE(1), CINT(2), CPINT(3));
      break;

   case RPC_DB_CREATE_RECORD:
      status = db_create_record(CHNDLE(0), CHNDLE(1), CSTRING(2), CSTRING(3));
      break;

   case RPC_DB_CHECK_RECORD:
      status = db_check_record(CHNDLE(0), CHNDLE(1), CSTRING(2), CSTRING(3), CBOOL(4));
      break;

   case RPC_DB_ADD_OPEN_RECORD:
      status = db_add_open_record(CHNDLE(0), CHNDLE(1), CWORD(2));
      break;

   case RPC_DB_REMOVE_OPEN_RECORD:
      status = db_remove_open_record(CHNDLE(0), CHNDLE(1), CBOOL(2));
      break;

   case RPC_DB_LOAD:
      status = db_load(CHNDLE(0), CHNDLE(1), CSTRING(2), CBOOL(3));
      break;

   case RPC_DB_SAVE:
      status = db_save(CHNDLE(0), CHNDLE(1), CSTRING(2), CBOOL(3));
      break;

   case RPC_DB_SET_CLIENT_NAME:
      status = db_set_client_name(CHNDLE(0), CSTRING(1));
      break;

   case RPC_DB_GET_OPEN_RECORDS:
      status = db_get_open_records(CHNDLE(0), CHNDLE(1), CSTRING(2), CINT(3), CBOOL(4));
      break;

      /* history functions */

   case RPC_HS_SET_PATH:
      status = hs_set_path(CSTRING(0));
      break;

   case RPC_HS_DEFINE_EVENT:
      if (convert_flags) {
         TAG *tag;
         INT i;

         /* convert tags */
         tag = (TAG *) CARRAY(2);
         for (i = 0; i < CINT(3); i++) {
            rpc_convert_single(&tag[i].type, TID_DWORD, 0, convert_flags);
            rpc_convert_single(&tag[i].n_data, TID_DWORD, 0, convert_flags);
         }
      }

      status = hs_define_event(CDWORD(0), CSTRING(1), CARRAY(2), CDWORD(3));
      break;

   case RPC_HS_WRITE_EVENT:
      status = hs_write_event(CDWORD(0), CARRAY(1), CDWORD(2));
      break;

   case RPC_HS_COUNT_EVENTS:
      status = hs_count_events(CDWORD(0), CPDWORD(1));
      break;

   case RPC_HS_ENUM_EVENTS:
      status = hs_enum_events(CDWORD(0), CSTRING(1), CPDWORD(2), CPINT(3), CPDWORD(4));
      break;

   case RPC_HS_COUNT_VARS:
      status = hs_count_vars(CDWORD(0), CDWORD(1), CPDWORD(2));
      break;

   case RPC_HS_ENUM_VARS:
      status =
          hs_enum_vars(CDWORD(0), CDWORD(1), CSTRING(2), CPDWORD(3), CPDWORD(4),
                       CPDWORD(5));
      break;

   case RPC_HS_GET_VAR:
      status = hs_get_var(CDWORD(0), CDWORD(1), CSTRING(2), CPDWORD(3), CPINT(4));
      break;

   case RPC_HS_GET_EVENT_ID:
      status = hs_get_event_id(CDWORD(0), CSTRING(1), CPDWORD(2));
      break;

   case RPC_HS_READ:
      status = hs_read(CDWORD(0), CDWORD(1), CDWORD(2), CDWORD(3), CSTRING(4),
                       CDWORD(5), CARRAY(6), CPDWORD(7), CARRAY(8), CPDWORD(9),
                       CPDWORD(10), CPDWORD(11));
      if (convert_flags && rpc_tid_size(CDWORD(10)) > 0) {
         rpc_convert_data(CARRAY(6), TID_DWORD, RPC_FIXARRAY | RPC_OUTGOING,
                          CDWORD(7) / sizeof(DWORD), convert_flags);
         rpc_convert_data(CARRAY(8), CDWORD(10), RPC_FIXARRAY | RPC_OUTGOING,
                          CDWORD(11) / rpc_tid_size(CDWORD(10)), convert_flags);
      }
      break;

   case RPC_EL_SUBMIT:
      status = el_submit(CINT(0), CSTRING(1), CSTRING(2), CSTRING(3), CSTRING(4),
                         CSTRING(5), CSTRING(6), CSTRING(7),
                         CSTRING(8), CARRAY(9), CINT(10),
                         CSTRING(11), CARRAY(12), CINT(13),
                         CSTRING(14), CARRAY(15), CINT(16), CSTRING(17), CINT(18));
      break;

   case RPC_AL_CHECK:
      status = al_check();
      break;

   case RPC_AL_TRIGGER_ALARM:
      status = al_trigger_alarm(CSTRING(0), CSTRING(1), CSTRING(2), CSTRING(3), CINT(4));
      break;

      /* exit functions */
   case RPC_ID_EXIT:
   case RPC_ID_SHUTDOWN:
      status = RPC_SUCCESS;
      break;

      /* various functions */

   case RPC_TEST:
      status = rpc_test(CBYTE(0), CWORD(1), CINT(2), CFLOAT(3), CDOUBLE(4),
                        CPBYTE(5), CPWORD(6), CPINT(7), CPFLOAT(8), CPDOUBLE(9));
      break;

   default:
      cm_msg(MERROR, "rpc_server_dispatch", "received unrecognized command %d", index);
   }

   return status;
}
