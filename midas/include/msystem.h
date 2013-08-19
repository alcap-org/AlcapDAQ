/********************************************************************\

  Name:         MSYSTEM.H
  Created by:   Stefan Ritt

  Contents:     Function declarations and constants for internal
                routines

  $Log: msystem.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:15  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.43  2004/10/04 23:54:39  midas
  Implemented ODB version

  Revision 1.42  2004/09/29 00:58:35  midas
  Increased various constants

  Revision 1.41  2004/07/21 05:41:23  pierre
  fix dox doc

  Revision 1.40  2004/05/07 19:40:11  midas
  Replaced min/max by MIN/MAX macros

  Revision 1.39  2004/01/19 20:20:42  midas
  Made ss_timezone() EXPRTable

  Revision 1.38  2004/01/19 16:56:15  olchansk
  add ss_timezone()
  change ss_thread_create() and ss_thread_kill() to use midas_thread_t

  Revision 1.37  2004/01/17 05:35:53  olchansk
  replace #define ALIGN() with ALIGN8() to dodge namespace pollution under macosx
  hide strlcpy() & co #ifdef HAVE_STRLCPY (macosx already has strlcpy())
  correct inconsistent prototype of dbg_malloc() and dbg_calloc()

  Revision 1.36  2004/01/13 00:51:15  pierre
  fix dox comment for vxworks

  Revision 1.35  2004/01/12 23:03:42  pierre
  fix dox

  Revision 1.34  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.33  2004/01/08 06:29:39  pierre
  Doxygen the file

  Revision 1.32  2003/11/24 08:22:45  midas
  Changed timeouts from INT to DWORD, added ignore_timeout to cm_cleanup, adde '-f' flag to ODBEdit 'cleanup'

  Revision 1.31  2003/10/16 22:53:57  midas
  Changed return values for ss_thread_create to return threadId.
  Added ss_thread_kill for killing the passed threadId. (DBM - Triumf)

  Revision 1.30  2003/05/01 18:10:50  midas
  Made rpc_client_check() EXPRT

  Revision 1.29  2003/04/25 11:47:17  midas
  Added ss_force_single_thread()

  Revision 1.28  2003/04/22 12:00:56  midas
  Added rpc_client_check

  Revision 1.27  2003/04/15 08:17:08  midas
  Added verbose flag to db_show_mem

  Revision 1.26  2003/04/14 12:59:51  midas
  Added 'compression' in channel settings

  Revision 1.25  2003/04/07 23:55:55  olchansk
  add c++ wrappers

  Revision 1.24  2002/06/03 06:07:14  midas
  Added extra parameter to ss_daemon_init to keep stdout

  Revision 1.23  2002/05/16 18:01:13  midas
  Added subdir creation in logger and improved program restart scheme

  Revision 1.22  2002/05/15 23:45:46  midas
  Increased number of RPC connections

  Revision 1.21  2001/06/15 09:50:29  midas
  Made recv_string EXPRT (for bl_psi.ch)

  Revision 1.20  2000/10/20 19:03:17  pierre
  - Added ss_exec(...) for pid return
  - Added ss_existpid(...) for pid check

  Revision 1.19  2000/08/21 07:05:47  midas
  Added cm_msg_log1(...,facility) to be compatible with older programs

  Revision 1.18  2000/08/11 11:43:50  midas
  Added cm_msg1 to produce messages which go to a differnt logging file

  Revision 1.17  1999/12/08 00:34:24  pierre
  - EXPRT db_un/lock_database.
  - mod dm_buffer_create arg list.

  Revision 1.16  1999/11/09 13:17:00  midas
  Changed shared memory function names to ss_shm_xxx instead ss_xxx_shm

  Revision 1.15  1999/10/11 14:16:04  midas
  Added ss_daemon_init and ss_system

  Revision 1.14  1999/10/07 13:17:34  midas
  Put a few EXPRT im msystem.h to make NT happy, updated NT makefile

  Revision 1.13  1999/10/04 10:37:51  midas
  Removed reduced MAX_EVENT_SIZE for VxWorks

  Revision 1.12  1999/06/23 09:59:00  midas
  Added dm_task and dm_buffer_time_get

  Revision 1.11  1999/04/30 13:19:53  midas
  Changed inter-process communication (ss_resume, bm_notify_clients, etc)
  to strings so that server process can receive it's own watchdog produced
  messages (pass buffer name insteas buffer handle)

  Revision 1.10  1999/04/30 10:58:20  midas
  Added mode to rpc_set_debug

  Revision 1.9  1999/04/29 12:14:49  midas
  Made db_notify_clients EXPRT

  Revision 1.8  1999/02/11 13:19:44  midas
  Added DR_ASCII

  Revision 1.7  1999/02/09 14:38:54  midas
  Added debug logging facility

  Revision 1.6  1999/01/21 23:01:13  pierre
  - Change ss_create_thread to ss_thread_create.
  - Include VX_TASK_SPAWN struct for ss_thread_create.
  - Adjust dm_async_area_read arg for ss_thread_create.

  Revision 1.5  1999/01/20 08:55:43  midas
  - Renames ss_xxx_mutex to ss_mutex_xxx
  - Added timout flag to ss_mutex_wait_for

  Revision 1.4  1999/01/18 17:28:47  pierre
  - Added prototypes for dm_()

  Revision 1.3  1998/10/27 10:54:03  midas
  Added ss_shell()

  Revision 1.2  1998/10/12 12:19:01  midas
  Added Log tag in header


\********************************************************************/

/* MIDAS library version number. This value is returned by 
   cm_get_version and will be incremented whenever changes are done
   to the MIDAS library. First digit is major release, second
   digit is minor release, third digit is patch level */

/**dox***************************************************************/
/** @file msystem.h
The Midas System include file
*/

/** @defgroup msystemincludecode The msystem.h & system.c
 */
/** @defgroup msdefineh System Define
 */
/** @defgroup msmacroh System Macros
 */
/** @defgroup mssectionh System Structure Declaration
 */

/**dox***************************************************************/
/** @addtogroup msystemincludecode
 *  
 *  @{  */

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "midasinc.h"

/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/**dox***************************************************************/
/** @addtogroup msdefineh
 *  
 *  @{  */

/**
data representations
*/
#define DRI_16              (1<<0)  /**< - */
#define DRI_32              (1<<1)  /**< - */
#define DRI_64              (1<<2)  /**< - */
#define DRI_LITTLE_ENDIAN   (1<<3)  /**< - */
#define DRI_BIG_ENDIAN      (1<<4)  /**< - */
#define DRF_IEEE            (1<<5)  /**< - */
#define DRF_G_FLOAT         (1<<6)  /**< - */
#define DR_ASCII            (1<<7)  /**< - */

/**dox***************************************************************/
/** @} */ /* end of msdefineh */

/**dox***************************************************************/
/** @addtogroup msmacroh
 *  
 *  @{  */

/* Byte and Word swapping big endian <-> little endian */
/**
SWAP WORD macro */
#define WORD_SWAP(x) { BYTE _tmp;                               \
                       _tmp= *((BYTE *)(x));                    \
                       *((BYTE *)(x)) = *(((BYTE *)(x))+1);     \
                       *(((BYTE *)(x))+1) = _tmp; }

/**
SWAP DWORD macro */
#define DWORD_SWAP(x) { BYTE _tmp;                              \
                       _tmp= *((BYTE *)(x));                    \
                       *((BYTE *)(x)) = *(((BYTE *)(x))+3);     \
                       *(((BYTE *)(x))+3) = _tmp;               \
                       _tmp= *(((BYTE *)(x))+1);                \
                       *(((BYTE *)(x))+1) = *(((BYTE *)(x))+2); \
                       *(((BYTE *)(x))+2) = _tmp; }

/**
SWAP QWORD macro */
#define QWORD_SWAP(x) { BYTE _tmp;                              \
                       _tmp= *((BYTE *)(x));                    \
                       *((BYTE *)(x)) = *(((BYTE *)(x))+7);     \
                       *(((BYTE *)(x))+7) = _tmp;               \
                       _tmp= *(((BYTE *)(x))+1);                \
                       *(((BYTE *)(x))+1) = *(((BYTE *)(x))+6); \
                       *(((BYTE *)(x))+6) = _tmp;               \
                       _tmp= *(((BYTE *)(x))+2);                \
                       *(((BYTE *)(x))+2) = *(((BYTE *)(x))+5); \
                       *(((BYTE *)(x))+5) = _tmp;               \
                       _tmp= *(((BYTE *)(x))+3);                \
                       *(((BYTE *)(x))+3) = *(((BYTE *)(x))+4); \
                       *(((BYTE *)(x))+4) = _tmp; }

/**dox***************************************************************/
/** @} */ /* end of msmacroh */

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
Definition of implementation specific constants */
#define MESSAGE_BUFFER_SIZE    100000   /**< buffer used for messages */
#define MESSAGE_BUFFER_NAME    "SYSMSG" /**< buffer name for messages */
#define MAX_RPC_CONNECTION     64       /**< server/client connections   */
#define MAX_STRING_LENGTH      256      /**< max string length for odb */
#define NET_BUFFER_SIZE        (ALIGN8(MAX_EVENT_SIZE)+sizeof(EVENT_HEADER)+\
4*8 + sizeof(NET_COMMAND_HEADER))

/*------------------------------------------------------------------*/
/* flag for conditional compilation of debug messages */
#undef  DEBUG_MSG

/* flag for local routines (not for pure network clients) */
#if !defined ( OS_MSDOS ) && !defined ( OS_VXWORKS )
#define LOCAL_ROUTINES
#endif

/* YBOS support not in MSDOS */
#if !defined ( OS_MSDOS )
#define YBOS_SUPPORT
#endif

/*------------------------------------------------------------------*/

/* Mapping of function names for socket operations */

#ifdef OS_MSDOS

#define closesocket(s) close(s)
#define ioctlsocket(s,c,d) ioctl(s,c,d)
#define malloc(i) farmalloc(i)

#undef NET_TCP_SIZE
#define NET_TCP_SIZE 0x7FFF

#undef MAX_EVENT_SIZE
#define MAX_EVENT_SIZE 4096

#endif                          /* OS_MSDOS */

#ifdef OS_VMS

#define closesocket(s) close(s)
#define ioctlsocket(s,c,d)

#ifndef FD_SET
typedef struct {
   INT fds_bits;
} fd_set;

#define FD_SET(n, p)    ((p)->fds_bits |= (1 << (n)))
#define FD_CLR(n, p)    ((p)->fds_bits &= ~(1 << (n)))
#define FD_ISSET(n, p)  ((p)->fds_bits & (1 << (n)))
#define FD_ZERO(p)      ((p)->fds_bits = 0)
#endif                          /* FD_SET */

#endif                          /* OS_VMS */

/* Missing #defines in VMS */

#ifdef OS_VMS

#define P_WAIT   0
#define P_NOWAIT 1
#define P_DETACH 4

#endif

/* and for UNIX */

#ifdef OS_UNIX

#define closesocket(s) close(s)
#define ioctlsocket(s,c,d) ioctl(s,c,d)

#define P_WAIT   0
#define P_NOWAIT 1
#define P_DETACH 4

extern char **environ;

#endif

#ifndef FD_SETSIZE
#define FD_SETSIZE 32
#endif

/* and VXWORKS */

#ifdef OS_VXWORKS

#define P_NOWAIT 1
#define closesocket(s) close(s)
#define ioctlsocket(s,c,d) ioctl(s,c,d)

#endif

/* missing O_BINARY for non-PC */
#ifndef O_BINARY
#define O_BINARY 0
#define O_TEXT   0
#endif

/* min/max/abs macros */
#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a)              (((a) < 0)   ? -(a) : (a))
#endif

/* missing tell() for some operating systems */
#define TELL(fh) lseek(fh, 0, SEEK_CUR)

/* reduced shared memory size */
#ifdef OS_SOLARIS
#define MAX_SHM_SIZE      0x20000       /* 128k */
#endif

/*------------------------------------------------------------------*/

/* Network structures */

typedef struct {
   DWORD routine_id;            /* routine ID like ID_BM_xxx    */
   DWORD param_size;            /* size in Bytes of parameter   */
} NET_COMMAND_HEADER;

typedef struct {
   NET_COMMAND_HEADER header;
   char param[32];              /* parameter array              */
} NET_COMMAND;


typedef struct {
   DWORD serial_number;
   DWORD sequence_number;
} UDP_HEADER;

#define UDP_FIRST 0x80000000l
#define TCP_FAST  0x80000000l

#define MSG_BM       1
#define MSG_ODB      2
#define MSG_CLIENT   3
#define MSG_SERVER   4
#define MSG_LISTEN   5
#define MSG_WATCHDOG 6

/* RPC structures */

struct callback_addr {
   char host_name[HOST_NAME_LENGTH];
   short host_port1;
   short host_port2;
   short host_port3;
   int debug;
   char experiment[NAME_LENGTH];
   char directory[MAX_STRING_LENGTH];
   char user[NAME_LENGTH];
   INT index;
};

typedef struct {
   char host_name[HOST_NAME_LENGTH];    /*  server name        */
   INT port;                    /*  ip port                 */
   char exp_name[NAME_LENGTH];  /*  experiment to connect   */
   int send_sock;               /*  tcp send socket         */
   INT remote_hw_type;          /*  remote hardware type    */
   char client_name[NAME_LENGTH];       /* name of remote client    */
   INT transport;               /*  RPC_TCP/RPC_FTCP        */
   INT rpc_timeout;             /*  in milliseconds         */

} RPC_CLIENT_CONNECTION;

typedef struct {
   char host_name[HOST_NAME_LENGTH];    /*  server name        */
   INT port;                    /*  ip port                 */
   char exp_name[NAME_LENGTH];  /*  experiment to connect   */
   int send_sock;               /*  tcp send socket         */
   int recv_sock;               /*  tcp receive socket      */
   int event_sock;              /*  event socket            */
   INT remote_hw_type;          /*  remote hardware type    */
   INT transport;               /*  RPC_TCP/RPC_FTCP        */
   INT rpc_timeout;             /*  in milliseconds         */

} RPC_SERVER_CONNECTION;

typedef struct {
   INT tid;                     /*  thread id               */
   char prog_name[NAME_LENGTH]; /*  client program name     */
   char host_name[HOST_NAME_LENGTH];    /*  client name        */
   int send_sock;               /*  tcp send socket         */
   int recv_sock;               /*  tcp receive socket      */
   int event_sock;              /*  tcp event socket        */
   INT remote_hw_type;          /*  hardware type           */
   INT transport;               /*  RPC_TCP/RPC_FTCP        */
   INT watchdog_timeout;        /*  in milliseconds         */
   DWORD last_activity;         /*  time of last recv       */
   INT convert_flags;           /*  convertion flags        */
   char *net_buffer;            /*  TCP cache buffer        */
   char *ev_net_buffer;
   INT net_buffer_size;         /*  size of TCP cache       */
   INT write_ptr, read_ptr, misalign;   /* pointers for cache */
   INT ev_write_ptr, ev_read_ptr, ev_misalign;
   HNDLE odb_handle;            /*  handle to online datab. */
   HNDLE client_handle;         /*  client key handle .     */

} RPC_SERVER_ACCEPTION;

/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/**dox***************************************************************/
/** @addtogroup mssectionh
 *  
 *  @{  */

typedef struct {
   INT size;                          /**< size in bytes              */
   INT next_free;                     /**< Address of next free block */
} FREE_DESCRIP;

typedef struct {
   INT handle;                        /**< Handle of record base key  */
   WORD access_mode;                  /**< R/W flags                  */
   WORD flags;                        /**< Data format, ...           */

} OPEN_RECORD;

typedef struct {
   char name[NAME_LENGTH];      /* name of client             */
   INT pid;                     /* process ID                 */
   INT tid;                     /* thread ID                  */
   INT thandle;                 /* thread handle              */
   INT port;                    /* UDP port for wake up       */
   INT num_open_records;        /* number of open records     */
   DWORD last_activity;         /* time of last activity      */
   DWORD watchdog_timeout;      /* timeout in ms              */
   INT max_index;               /* index of last opren record */

   OPEN_RECORD open_record[MAX_OPEN_RECORDS];

} DATABASE_CLIENT;

typedef struct {
   char name[NAME_LENGTH];      /* name of database           */
   INT version;                 /* database version           */
   INT num_clients;             /* no of active clients       */
   INT max_client_index;        /* index of last client       */
   INT key_size;                /* size of key area in bytes  */
   INT data_size;               /* size of data area in bytes */
   INT root_key;                /* root key offset            */
   INT first_free_key;          /* first free key memory      */
   INT first_free_data;         /* first free data memory     */

   DATABASE_CLIENT client[MAX_CLIENTS]; /* entries for clients        */

} DATABASE_HEADER;

/* Per-process buffer access structure (descriptor) */

typedef struct {
   char name[NAME_LENGTH];      /* Name of database             */
   BOOL attached;               /* TRUE if database is attached */
   INT client_index;            /* index to CLIENT str. in buf. */
   DATABASE_HEADER *database_header;    /* pointer to database header   */
   void *database_data;         /* pointer to database data     */
   HNDLE mutex;                 /* mutex/semaphore handle       */
   INT lock_cnt;                /* flag to avoid multiple locks */
   HNDLE shm_handle;            /* handle (id) to shared memory */
   INT index;                   /* connection index / tid       */
   BOOL protect;                /* read/write protection        */

} DATABASE;

/* Open record descriptor */

typedef struct {
   HNDLE handle;                /* Handle of record base key  */
   HNDLE hDB;                   /* Handle of record's database */
   WORD access_mode;            /* R/W flags                  */
   void *data;                  /* Pointer to local data      */
   void *copy;                  /* Pointer of copy to data    */
   INT buf_size;                /* Record size in bytes       */
   void (*dispatcher) (INT, INT, void *);       /* Pointer to dispatcher func. */
   void *info;                  /* addtl. info for dispatcher */

} RECORD_LIST;

/* Event request descriptor */

typedef struct {
   INT buffer_handle;           /* Buffer handle */
   short int event_id;          /* same as in EVENT_HEADER */
   short int trigger_mask;
   void (*dispatcher) (HNDLE, HNDLE, EVENT_HEADER *, void *);   /* Dispatcher func. */

} REQUEST_LIST;

/**dox***************************************************************/
/** @} */ /* end of mssectionh */

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/*---- Logging channel information ---------------------------------*/

#define CHN_SETTINGS_STR(_name) char *_name[] = {\
"[Settings]",\
"Active = BOOL : 1",\
"Type = STRING : [8] Disk",\
"Filename = STRING : [256] run%05d.mid",\
"Format = STRING : [8] MIDAS",\
"Compression = INT : 0",\
"ODB dump = BOOL : 1",\
"Log messages = DWORD : 0",\
"Buffer = STRING : [32] SYSTEM",\
"Event ID = INT : -1",\
"Trigger mask = INT : -1",\
"Event limit = DWORD : 0",\
"Byte limit = DOUBLE : 0",\
"Tape capacity = DOUBLE : 0",\
"Subdir format = STRING : [32]",\
"Current filename = STRING : [256]",\
"",\
"[Statistics]",\
"Events written = DOUBLE : 0",\
"Bytes written = DOUBLE : 0",\
"Bytes written total = DOUBLE : 0",\
"Files written = INT : 0",\
"",\
NULL}

typedef struct {
   BOOL active;
   char type[8];
   char filename[256];
   char format[8];
   INT compression;
   BOOL odb_dump;
   DWORD log_messages;
   char buffer[32];
   INT event_id;
   INT trigger_mask;
   DWORD event_limit;
   double byte_limit;
   double tape_capacity;
   char subdir_format[32];
   char current_filename[256];
} CHN_SETTINGS;

typedef struct {
   double events_written;
   double bytes_written;
   double bytes_written_total;
   INT files_written;
} CHN_STATISTICS;

typedef struct {
   INT handle;
   char path[256];
   INT type;
   INT format;
   INT buffer_handle;
   INT msg_buffer_handle;
   INT request_id;
   INT msg_request_id;
   HNDLE stats_hkey;
   HNDLE settings_hkey;
   CHN_SETTINGS settings;
   CHN_STATISTICS statistics;
   void **format_info;
   void *ftp_con;
} LOG_CHN;

#define LOG_TYPE_DISK 1
#define LOG_TYPE_TAPE 2
#define LOG_TYPE_FTP  3

/*---- VxWorks specific taskSpawn arguments ----------------------*/

typedef struct {
   char name[32];
   int priority;
   int options;
   int stackSize;
   int arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10;
} VX_TASK_SPAWN;

/*---- Channels for ss_suspend_set_dispatch() ----------------------*/

#define CH_IPC        1
#define CH_CLIENT     2
#define CH_SERVER     3
#define CH_LISTEN     4

/*---- Function declarations ---------------------------------------*/

/* make functions callable from a C++ program */
#ifdef __cplusplus
extern "C" {
#endif

/*---- common function ----*/
   INT EXPRT cm_set_path(char *path);
   INT EXPRT cm_get_path(char *path);
   INT cm_dispatch_ipc(char *message, int socket);
   INT EXPRT cm_msg_log(INT message_type, const char *message);
   INT EXPRT cm_msg_log1(INT message_type, const char *message, const char *facility);
   void EXPRT name2c(char *str);

/*---- buffer manager ----*/
   INT bm_lock_buffer(INT buffer_handle);
   INT bm_unlock_buffer(INT buffer_handle);
   INT bm_notify_client(char *buffer_name, int socket);
   INT EXPRT bm_mark_read_waiting(BOOL flag);
   INT bm_push_event(char *buffer_name);
   INT bm_check_buffers(void);
   INT EXPRT bm_remove_event_request(INT buffer_handle, INT request_id);
   void EXPRT bm_defragment_event(HNDLE buffer_handle, HNDLE request_id,
                                  EVENT_HEADER * pevent, void *pdata,
                                  void (*dispatcher) (HNDLE, HNDLE,
                                                      EVENT_HEADER *, void *));

/*---- online database ----*/
   INT EXPRT db_lock_database(HNDLE database_handle);
   INT EXPRT db_unlock_database(HNDLE database_handle);
   INT db_update_record(INT hDB, INT hKey, int socket);
   INT db_close_all_records(void);
   INT EXPRT db_flush_database(HNDLE hDB);
   INT EXPRT db_notify_clients(HNDLE hDB, HNDLE hKey, BOOL bWalk);
   INT EXPRT db_set_client_name(HNDLE hDB, char *client_name);
   INT db_delete_key1(HNDLE hDB, HNDLE hKey, INT level, BOOL follow_links);
   INT EXPRT db_show_mem(HNDLE hDB, char *result, INT buf_size, BOOL verbose);

/*---- rpc functions -----*/
   RPC_LIST EXPRT *rpc_get_internal_list(INT flag);
   INT rpc_server_receive(INT index, int sock, BOOL check);
   INT rpc_server_callback(struct callback_addr *callback);
   INT EXPRT rpc_server_accept(int sock);
   INT rpc_client_accept(int sock);
   INT rpc_get_server_acception(void);
   INT rpc_set_server_acception(INT index);
   INT EXPRT rpc_set_server_option(INT item, PTYPE value);
   PTYPE EXPRT rpc_get_server_option(INT item);
   INT recv_tcp_check(int sock);
   INT recv_event_check(int sock);
   INT rpc_deregister_functions(void);
   INT rpc_check_channels(void);
   void EXPRT rpc_client_check();
   INT rpc_server_disconnect(void);
   int EXPRT rpc_get_send_sock(void);
   int EXPRT rpc_get_event_sock(void);
   INT EXPRT rpc_set_opt_tcp_size(INT tcp_size);
   INT EXPRT rpc_get_opt_tcp_size(void);

/*---- system services ----*/
   INT ss_shm_open(char *name, INT size, void **adr, HNDLE * handle);
   INT ss_shm_close(char *name, void *adr, HNDLE handle, INT destroy_flag);
   INT ss_shm_flush(char *name, void *adr, INT size);
   INT ss_shm_protect(HNDLE handle, void *adr);
   INT ss_shm_unprotect(HNDLE handle, void **adr);
   INT ss_spawnv(INT mode, char *cmdname, char *argv[]);
   INT ss_shell(int sock);
   INT EXPRT ss_daemon_init(BOOL keep_stdout);
   INT EXPRT ss_system(char *command);
   INT EXPRT ss_exec(char *cmd, INT * child_pid);
   BOOL EXPRT ss_existpid(INT pid);
   INT EXPRT ss_getpid(void);
   INT EXPRT ss_gettid(void);
   INT ss_getthandle(void);
   INT ss_set_async_flag(INT flag);
   INT EXPRT ss_mutex_create(char *mutex_name, HNDLE * mutex_handle);
   INT EXPRT ss_mutex_wait_for(HNDLE mutex_handle, INT timeout);
   INT EXPRT ss_mutex_release(HNDLE mutex_handle);
   INT EXPRT ss_mutex_delete(HNDLE mutex_handle, INT destroy_flag);
   INT ss_wake(INT pid, INT tid, INT thandle);
   INT ss_alarm(INT millitime, void (*func) (int));
   INT ss_suspend_get_port(INT * port);
   INT ss_suspend_set_dispatch(INT channel, void *connection, INT(*dispatch) ());
   INT ss_resume(INT port, char *message);
   INT ss_suspend_exit(void);
   INT ss_exception_handler(void (*func) ());
   void EXPRT ss_force_single_thread();
   INT EXPRT ss_suspend(INT millisec, INT msg);
   midas_thread_t EXPRT ss_thread_create(INT(*func) (void *), void *param);
   INT EXPRT ss_thread_kill(midas_thread_t thread_id);
   INT EXPRT ss_get_struct_align(void);
   INT EXPRT ss_timezone(void);

/*---- socket routines ----*/
   INT EXPRT send_tcp(int sock, char *buffer, DWORD buffer_size, INT flags);
   INT EXPRT recv_tcp(int sock, char *buffer, DWORD buffer_size, INT flags);
   INT send_udp(int sock, char *buffer, DWORD buffer_size, INT flags);
   INT recv_udp(int sock, char *buffer, DWORD buffer_size, INT flags);
   INT EXPRT recv_string(int sock, char *buffer, DWORD buffer_size, INT flags);

/*---- system logging ----*/
   INT EXPRT ss_syslog(const char *message);

/*---- event buffer routines ----*/
   INT EXPRT eb_create_buffer(INT size);
   INT EXPRT eb_free_buffer(void);
   BOOL EXPRT eb_buffer_full(void);
   BOOL EXPRT eb_buffer_empty(void);
   EVENT_HEADER EXPRT *eb_get_pointer(void);
   INT EXPRT eb_increment_pointer(INT buffer_handle, INT event_size);
   INT EXPRT eb_send_events(BOOL send_all);

/*---- dual memory event buffer routines ----*/
   INT EXPRT dm_buffer_create(INT size, INT usize);
   INT EXPRT dm_buffer_release(void);
   BOOL EXPRT dm_area_full(void);
   EVENT_HEADER EXPRT *dm_pointer_get(void);
   INT EXPRT dm_pointer_increment(INT buffer_handle, INT event_size);
   INT EXPRT dm_area_send(void);
   INT EXPRT dm_area_flush(void);
   INT EXPRT dm_task(void *pointer);
   DWORD EXPRT dm_buffer_time_get(void);
   INT EXPRT dm_async_area_send(void *pointer);

/*---- Include RPC identifiers -------------------------------------*/

#include "mrpc.h"

#ifdef __cplusplus
}
#endif
/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/**dox***************************************************************/
/** @} */ /* end of msystemincludecode */
