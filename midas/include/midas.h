/********************************************************************\

  Name:         MIDAS.H
  Created by:   Stefan Ritt

  Contents:     Type definitions and function declarations needed
                for MIDAS applications

  ** muCap patches ** 
  - Added x86-64 test for DWORD and PTYPE definitions.
  - Expanded NET_TCP_SIZE to 524287.
  - Expanded EVENT_BUFFER_SIZE, DEFAULT_EVENT_BUFFER_SIZE, and MAX_EVENT_SIZE 
    by a factor of 16.
  - Expanded DEFAULT_ODB_SIZE by a factor of 8.

  $Log: midas.h,v $
  Revision 1.2  2005/06/21 00:14:50  mucap
  (Fred)
    - Added x86-64 test for DWORD and PTYPE definitions.
    - Expanded NET_TCP_SIZE to 524287.
    - Expanded EVENT_BUFFER_SIZE, DEFAULT_EVENT_BUFFER_SIZE, and MAX_EVENT_SIZE
      by a factor of 16.
    - Expanded DEFAULT_ODB_SIZE by a factor of 8.

  Revision 1.146  2004/10/07 00:54:02  midas
  Implemented templates for histo booking from John O'Donnell

  Revision 1.145  2004/10/06 22:35:37  midas
  Added CM_TOO_MANY_REQUESTS

  Revision 1.144  2004/10/04 23:54:39  midas
  Implemented ODB version

  Revision 1.143  2004/10/01 23:36:11  midas
  Removed PRE/POST transitions and implemented sequence order of transitions

  Revision 1.142  2004/10/01 15:38:18  midas
  Added previous_count to tests

  Revision 1.141  2004/09/29 17:57:22  midas
  Added large file (>2GB) support for linux

  Revision 1.140  2004/09/29 16:45:06  midas
  Added get_frontend_index()

  Revision 1.139  2004/09/29 00:58:35  midas
  Increased various constants

  Revision 1.138  2004/09/28 23:57:08  midas
  Added EQ_EB flag for event building

  Revision 1.137  2004/09/28 20:05:47  midas
  Revised debug logging for mserver

  Revision 1.136  2004/09/28 18:30:12  midas
  Changed rpc_debug_print arguments

  Revision 1.135  2004/09/28 17:13:32  midas
  Added startup debug code for mserver

  Revision 1.134  2004/09/23 19:22:04  midas
  Added histo booking funcitons

  Revision 1.133  2004/09/17 16:10:11  midas
  Added new timeout errors

  Revision 1.132  2004/07/21 06:54:03  pierre
  fix dox doc

  Revision 1.131  2004/07/21 05:43:32  pierre
  fix dox doc

  Revision 1.130  2004/07/09 11:27:57  midas
  Version 1.9.4

  Revision 1.129  2004/05/07 19:40:11  midas
  Replaced min/max by MIN/MAX macros

  Revision 1.128  2004/05/03 11:30:37  midas
  Implemented cm_query_transition()

  Revision 1.127  2004/01/19 16:53:35  olchansk
  add midas_thread_t

  Revision 1.126  2004/01/17 05:35:53  olchansk
  replace #define ALIGN() with ALIGN8() to dodge namespace pollution under macosx
  hide strlcpy() & co #ifdef HAVE_STRLCPY (macosx already has strlcpy())
  correct inconsistent prototype of dbg_malloc() and dbg_calloc()

  Revision 1.125  2004/01/13 00:51:04  pierre
  fix dox comment for vxworks

  Revision 1.124  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.123  2004/01/08 06:29:07  pierre
  Doxygen the file

  Revision 1.122  2003/11/24 08:22:45  midas
  Changed timeouts from INT to DWORD, added ignore_timeout to cm_cleanup, adde '-f' flag to ODBEdit 'cleanup'

  Revision 1.121  2003/11/20 11:28:47  midas
  Added db_check_record

  Revision 1.120  2003/06/12 18:34:37  pierre
  add ss_tape_get_blockn

  Revision 1.119  2003/05/09 07:40:04  midas
  Added extra parameter to cm_get_environment

  Revision 1.118  2003/05/08 19:36:24  midas
  Changed size_t into INT

  Revision 1.117  2003/05/07 10:55:47  midas
  Export strlcpy

  Revision 1.116  2003/05/02 09:04:50  midas
  Version 1.9.3

  Revision 1.115  2003/05/02 09:03:01  midas
  Fixed buffer overflows by strlcpy()

  Revision 1.114  2003/04/22 15:14:09  midas
  Added #undef GetCurrentTime

  Revision 1.113  2003/04/22 12:00:56  midas
  Added rpc_client_check

  Revision 1.112  2003/04/22 10:09:21  midas
  Added RPC_NODELAY option

  Revision 1.111  2003/04/15 10:50:00  midas
  Removed unnecessary elements in KEY structure

  Revision 1.110  2003/04/15 08:16:48  midas
  Increased default ODB size to 1M

  Revision 1.109  2003/04/14 12:58:16  midas
  Added event_descrip to equipment

  Revision 1.108  2003/04/09 13:48:18  midas
  Added ROOT format

  Revision 1.107  2003/03/26 21:09:46  midas
  Corrected version string

  Revision 1.106  2003/03/22 06:56:12  olchansk
  add DB_CORRUPTED error code

  Revision 1.105  2003/03/17 06:36:55  olchansk
  really remove Windows-style "\r" characters

  Revision 1.104  2002/10/22 03:46:33  pierre
  add SS_NO_TAPE

  Revision 1.103  2002/09/19 17:50:22  pierre
  remove ^m

  Revision 1.102  2002/09/18 16:36:24  pierre
  add bk_list()

  Revision 1.101  2002/09/13 07:32:36  midas
  Added client name to cm_cleanup()

  Revision 1.100  2002/06/10 07:06:55  midas
  Added 'lock' parameter to db_remove_open_record() to work also remotely

  Revision 1.99  2002/06/06 07:49:46  midas
  Added DF_xxx flags for device drivers

  Revision 1.98  2002/06/06 07:15:03  midas
  Added demand_priority for SC FE

  Revision 1.97  2002/05/22 22:10:03  midas
  Version 1.9.1

  Revision 1.96  2002/05/22 05:43:32  midas
  Added extra variables to hs_enum_vars for mhist to display array size

  Revision 1.95  2002/05/16 18:01:13  midas
  Added subdir creation in logger and improved program restart scheme

  Revision 1.94  2002/05/15 23:44:47  midas
  Added EVENTID_FRAG for fragmented events

  Revision 1.93  2002/05/13 20:36:43  midas
  Changed type_t to unsigned int for malloc/free

  Revision 1.92  2002/05/13 17:56:36  midas
  Added improved malloc/free functions

  Revision 1.91  2002/05/10 01:41:19  midas
  Added optional debug output to cm_transition

  Revision 1.90  2002/05/09 18:16:55  midas
  Version 1.9.0

  Revision 1.89  2002/05/08 19:54:40  midas
  Added extra parameter to function db_get_value()

  Revision 1.88  2002/03/19 22:15:40  pierre
  Added  ~defined (OS_IRIX)

  Revision 1.87  2002/03/13 08:39:01  midas
  Added periodic alarms

  Revision 1.86  2001/10/03 08:36:02  midas
  Added DB_INVALID_LINK

  Revision 1.85  2001/08/22 13:51:14  midas
  Reorganized directio functions

  Revision 1.84  2001/08/08 11:44:59  midas
  Release 1.8.3

  Revision 1.83  2001/06/27 12:16:30  midas
  Added OS_IRIX

  Revision 1.82  2001/04/05 05:51:03  midas
  Added CMD_NAME

  Revision 1.81  2001/04/04 04:12:08  midas
  changed slow control commands

  Revision 1.80  2001/02/26 11:57:46  midas
  Added CMD_READ and CMD_WRITE

  Revision 1.79  2001/01/29 09:51:54  midas
  Put if defined(__linux__) under #if !defined(OS_LINUX)

  Revision 1.78  2001/01/22 08:26:58  midas
  Added evaluation of __linux__

  Revision 1.77  2001/01/04 11:27:37  midas
  Added bus driver commands and structures

  Revision 1.76  2000/11/20 12:23:17  midas
  Version 1.8.2

  Revision 1.75  2000/11/20 11:26:52  midas
  Added "use tests" in analyzer request

  Revision 1.74  2000/11/14 08:17:04  midas
  Added number of messages for cm_msg_retrieve and in odbedit "old" command

  Revision 1.73  2000/10/23 14:19:05  midas
  Added idle period for slow control equipment

  Revision 1.72  2000/09/29 13:31:10  midas
  ODBEdit cleanup now deletes open record with no client attached to

  Revision 1.71  2000/09/28 13:01:25  midas
  Added manual triggered events

  Revision 1.70  2000/08/21 14:19:11  midas
  bk_close return bank size

  Revision 1.69  2000/08/11 11:43:50  midas
  Added cm_msg1 to produce messages which go to a differnt logging file

  Revision 1.68  2000/06/07 10:49:07  midas
  Release 1.8.1

  Revision 1.67  2000/06/06 13:45:11  midas
  Added locking of histos

  Revision 1.66  2000/05/08 14:29:37  midas
  Added delete option in ELog

  Revision 1.65  2000/04/26 20:23:47  pierre
  - Moved doc++ comments for functions to midas.c (macros still in there)

  Revision 1.64  2000/04/17 17:07:05  pierre
  - new arg in hs_dump and hs_fdump (for mhist)
  - first round of doc++ comments

  Revision 1.63  2000/03/08 17:35:56  midas
  Version 1.8.0

  Revision 1.62  2000/03/02 21:57:31  midas
  Added new slow control commands

  Revision 1.61  2000/03/01 00:52:34  midas
  Added num_subevents into equipment, made events_sent a double

  Revision 1.60  2000/02/29 02:09:33  midas
  Added cm_is_ctrlc_pressed and cm_ack_ctrlc_pressed

  Revision 1.59  2000/02/25 22:22:45  midas
  Added abort feature in db_scan_tree

  Revision 1.58  2000/02/25 20:22:23  midas
  Added super-event scheme

  Revision 1.57  2000/02/24 22:33:16  midas
  Added deferred transitions

  Revision 1.56  2000/02/15 11:07:50  midas
  Changed GET_xxx to bit flags

  Revision 1.55  2000/02/09 08:03:27  midas
  Changed version to 1.7.1

  Revision 1.54  1999/12/08 00:29:34  pierre
  - Add CM_SET_LABEL for cd_gen/dd_epca (chn_acc)

  Revision 1.53  1999/11/19 09:49:36  midas
  Added watchdog_timeout to cm_set_client_info

  Revision 1.52  1999/11/09 13:17:00  midas
  Changed shared memory function names to ss_shm_xxx instead ss_xxx_shm

  Revision 1.51  1999/11/08 13:55:54  midas
  Added AT_xxx

  Revision 1.50  1999/11/04 15:53:54  midas
  Added some slow control commands

  Revision 1.49  1999/10/18 14:41:50  midas
  Use /programs/<name>/Watchdog timeout in all programs as timeout value. The
  default value can be submitted by calling cm_connect_experiment1(..., timeout)

  Revision 1.48  1999/10/08 15:07:04  midas
  Program check creates new internal alarm when triggered

  Revision 1.47  1999/10/07 13:17:34  midas
  Put a few EXPRT im msystem.h to make NT happy, updated NT makefile

  Revision 1.46  1999/10/06 08:56:33  midas
  Added /programs/xxx/required flag

  Revision 1.45  1999/09/27 13:49:03  midas
  Added bUnique parameter to cm_shutdown

  Revision 1.44  1999/09/27 12:54:55  midas
  Finished alarm system

  Revision 1.43  1999/09/23 12:45:48  midas
  Added 32 bit banks

  Revision 1.42  1999/09/22 15:39:36  midas
  Logger won't start run if disk file already exists

  Revision 1.41  1999/09/21 13:49:02  midas
  Added PROGRAM_INFO

  Revision 1.40  1999/09/17 15:06:46  midas
  Moved al_check into cm_yield() and rpc_server_thread

  Revision 1.39  1999/09/17 11:54:43  midas
  OS_LINUX & co automatically define OS_UNIX

  Revision 1.38  1999/09/17 11:48:04  midas
  Alarm system half finished

  Revision 1.37  1999/09/15 13:33:32  midas
  Added remote el_submit functionality

  Revision 1.36  1999/09/14 15:15:43  midas
  Moved el_xxx funtions into midas.c

  Revision 1.35  1999/09/14 11:46:04  midas
  Added EL_FIRST_MESSAGE

  Revision 1.34  1999/08/31 15:12:34  midas
  Added EL_LAST_MSG

  Revision 1.33  1999/08/26 15:18:22  midas
  Added EL_xxx codes

  Revision 1.32  1999/08/20 13:31:18  midas
  Analyzer saves and reloads online histos

  Revision 1.31  1999/08/03 11:15:07  midas
  Added bm_skip_event

  Revision 1.30  1999/07/15 07:35:12  midas
  Added ss_ctrlc_handler

  Revision 1.29  1999/07/13 08:24:27  midas
  Added ANA_TEST

  Revision 1.28  1999/06/28 12:01:33  midas
  Added hs_fdump

  Revision 1.27  1999/06/25 12:02:12  midas
  Added bk_delete function

  Revision 1.26  1999/06/23 09:58:28  midas
  Fixed typo

  Revision 1.25  1999/05/05 12:01:42  midas
  Added and modified hs_xxx functions

  Revision 1.24  1999/04/30 10:58:20  midas
  Added mode to rpc_set_debug

  Revision 1.23  1999/04/27 11:11:39  midas
  Added rpc_register_client

  Revision 1.22  1999/04/23 11:43:15  midas
  Increased version to 1.7.0

  Revision 1.21  1999/04/19 07:46:43  midas
  Added cm_msg_retrieve

  Revision 1.20  1999/04/15 09:59:09  midas
  Added key name to db_get_key_info

  Revision 1.19  1999/04/13 12:20:42  midas
  Added db_get_data1 (for Java)

  Revision 1.18  1999/04/08 15:23:46  midas
  Added CF_ASCII and db_get_key_info

  Revision 1.17  1999/03/02 10:00:07  midas
  Added ANA_CONTINUE and ANA_SKIP

  Revision 1.16  1999/02/18 13:06:27  midas
  Release 1.6.3

  Revision 1.15  1999/02/18 11:20:39  midas
  Added "level" parameter to db_scan_tree and db_scan_tree_link

  Revision 1.14  1999/02/11 13:20:29  midas
  Changed cm_msg from void to INT

  Revision 1.13  1999/02/09 14:38:54  midas
  Added debug logging facility

  Revision 1.12  1999/01/20 09:03:38  midas
  Added LAM_SOURCE_CRATE and LAM_SOURCE_STATION macros

  Revision 1.11  1999/01/19 10:26:39  midas
  Changed CRATE and STATION macros to LAM_SOURCE and LAM_STATION

  Revision 1.10  1999/01/13 09:40:48  midas
  Added db_set_data_index2 function

  Revision 1.9  1998/12/11 11:23:17  midas
  Release 1.6.2

  Revision 1.8  1998/12/10 10:18:38  midas
  Added SS_END_OF_FILE

  Revision 1.7  1998/10/29 15:58:05  midas
  Release 1.6.1

  Revision 1.6  1998/10/23 14:35:16  midas
  Added version comment

  Revision 1.5  1998/10/23 14:21:35  midas
  - Modified version scheme from 1.06 to 1.6.0
  - cm_get_version() now returns versino as string

  Revision 1.4  1998/10/23 13:11:13  midas
  Modified release number to 1.6

  Revision 1.3  1998/10/22 12:40:20  midas
  Added "oflag" to ss_tape_open()

  Revision 1.2  1998/10/12 12:18:59  midas
  Added Log tag in header


  Previous Revision history
  ------------------------------------------------------------------
  date         by    modification
  ---------    ---   ------------------------------------------------
  11-NOV-95    SR    created
  03-JUN-97    SR    increased number of clients to 32

\********************************************************************/

#ifndef _MIDAS_H_
#define _MIDAS_H_

/*------------------------------------------------------------------*/

/* MIDAS library version number. This value is returned by 
   cm_get_version and will be incremented whenever changes are done
   to the MIDAS library. First digit is major release, second
   digit is minor release, third digit is patch level */

/**dox***************************************************************/
/** @file midas.h
The main include file
*/

/** @defgroup midasincludecode The midas.h & midas.c
 */
/** @defgroup mdefineh Midas Define 
 */
/** @defgroup mmacroh Midas Macros
 */
/** @defgroup mdeferrorh Midas Error definition
 */
/** @defgroup msectionh Midas Structure Declaration
 */

/**dox***************************************************************/
/** @addtogroup midasincludecode
 *  
 *  @{  */

#define MIDAS_VERSION "1.9.5"
#define DATABASE_VERSION 2   /* has to be changed whenenver binary ODB format changes*/

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/*------------------------------------------------------------------*/

/* find out on which operating system we are running */

#if defined( VAX ) || defined( __VMS )
#define OS_VMS
#endif

#if defined( _MSC_VER )
#define OS_WINNT
#endif

#if defined( __MSDOS__ )
#define OS_MSDOS
#endif

#if defined ( vxw )
#define OS_VXWORKS
#undef OS_UNIX
#endif

#if !defined(OS_LINUX)
#if defined ( __linux__ )
#define OS_LINUX
#endif
#endif

#if defined(OS_LINUX) || defined(OS_OSF1) || defined(OS_ULTRIX) || defined(OS_FREEBSD) || defined(OS_SOLARIS) || defined(OS_IRIX) || defined(OS_DARWIN)
#define OS_UNIX
#endif

#if !defined(OS_IRIX) && !defined(OS_VMS) && !defined(OS_MSDOS) && !defined(OS_UNIX) && !defined(OS_VXWORKS) && !defined(OS_WINNT)
#error MIDAS cannot be used on this operating system
#endif

/*------------------------------------------------------------------*/

#ifdef USE_ROOT
#include <TObjArray.h>
#include <TFolder.h>
#include <TCutG.h>
#endif

/* Define basic data types */

#ifndef MIDAS_TYPE_DEFINED
#define MIDAS_TYPE_DEFINED

typedef unsigned char BYTE;
typedef unsigned short int WORD;

#if defined(__alpha) || defined(__x86_64__)
typedef unsigned int DWORD;
#else
typedef unsigned long int DWORD;
#endif

#ifndef OS_WINNT
#ifndef OS_VXWORKS
typedef DWORD BOOL;
#endif
#endif

#endif                          /* MIDAS_TYPE_DEFINED */

/*
  Definitions depending on integer types:

  Note that the alpha chip uses 32 bit integers by default.
  Therefore always use 'INT' instead 'int'.
*/
#if defined(OS_MSDOS)
typedef long int INT;
#elif defined( OS_WINNT )

/* INT predefined in windows.h */
#ifndef _INC_WINDOWS
#include <windows.h>
#endif

#else
typedef int INT;
#endif

typedef INT HNDLE;

/* Include vxWorks for BOOL definition */
#ifdef OS_VXWORKS
#ifndef __INCvxWorksh
#include <vxWorks.h>
#endif
#endif

/*
  Definitions depending on pointer types:

  Note that the alpha chip uses 64 bit pointers by default.
  Therefore, when converting pointer to integers, always
  use the (PTYPE) cast.
*/
#if defined(__alpha) || defined(__x86_64__)
#define PTYPE              long int
#else
#define PTYPE              int
#endif

/* need system-dependant thread type */
#if defined(OS_WINNT)
typedef HANDLE midas_thread_t;
#elif defined(OS_UNIX)
#include <pthread.h>
typedef pthread_t midas_thread_t;
#else
typedef INT midas_thread_t;
#endif

#define TRUE  1
#define FALSE 0

/* directory separator */
#if defined(OS_MSDOS) || defined(OS_WINNT)
#define DIR_SEPARATOR     '\\'
#define DIR_SEPARATOR_STR "\\"
#elif defined(OS_VMS)
#define DIR_SEPARATOR     ']'
#define DIR_SEPARATOR_STR "]"
#else
#define DIR_SEPARATOR     '/'
#define DIR_SEPARATOR_STR "/"
#endif

/* inline functions */
#if defined( _MSC_VER )
#define INLINE __inline
#elif defined(__GNUC__)
#define INLINE __inline__
#else
#define INLINE
#endif

/* large file (>2GB) support */
#ifndef _LARGEFILE64_SOURCE
#define O_LARGEFILE 0
#endif

/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/*------------------------------------------------------------------*/

/* Definition of implementation specific constants */

/* all buffer sizes must be multiples of 4 ! */

#ifdef OS_WINNT
#define TAPE_BUFFER_SIZE       0x100000      /**< buffer size for taping data */
#else
#define TAPE_BUFFER_SIZE       0x8000        /**< buffer size for taping data */
#endif
#define NET_TCP_SIZE           524287        /**< maximum TCP transfer        */
#define OPT_TCP_SIZE           8192          /**< optimal TCP buffer size     */
#define NET_UDP_SIZE           8192          /**< maximum UDP transfer        */

#define EVENT_BUFFER_SIZE      0x1000000     /**< buffer used for events      */
#define EVENT_BUFFER_NAME      "SYSTEM"      /**< buffer name for commands    */
#define MAX_EVENT_SIZE         0x800000      /**< maximum event size 8MB     */
#define DEFAULT_EVENT_BUFFER_SIZE 0x4000000;  /**< 2M                          */
#define DEFAULT_ODB_SIZE       0x800000      /**< online database 1M          */

#define NAME_LENGTH            32            /**< length of names, mult.of 8! */
#define HOST_NAME_LENGTH       256           /**< length of TCP/IP names      */
#define MAX_CLIENTS            64            /**< client processes per buf/db */
#define MAX_EVENT_REQUESTS     10            /**< event requests per client   */
#define MAX_OPEN_RECORDS       256           /**< number of open DB records   */
#define MAX_ODB_PATH           256           /**< length of path in ODB       */
#define MAX_EXPERIMENT         32            /**< number of different exp.    */
#define BANKLIST_MAX           64            /**< max # of banks in event     */
#define STRING_BANKLIST_MAX    BANKLIST_MAX * 4   /**< for bk_list()          */


#define MIDAS_TCP_PORT 1175     /* port under which server is listening */

/**
Timeouts [ms] */
#define DEFAULT_RPC_TIMEOUT    10000
#define WATCHDOG_INTERVAL      1000

#define DEFAULT_WATCHDOG_TIMEOUT 10000    /**< Watchdog */

/*------------------------------------------------------------------*/

/* Enumeration definitions */

/**dox***************************************************************/
/** @addtogroup mdefineh
 *  
 *  @{  */

/**
System states */
#define STATE_STOPPED 1      /**< MIDAS run stopped                  */
#define STATE_PAUSED  2      /**< MIDAS run paused                   */
#define STATE_RUNNING 3      /**< MIDAS run running                  */

/**
Data format */
#define FORMAT_MIDAS  1       /**< MIDAS banks                        */
#define FORMAT_YBOS   2       /**< YBOS  banks                        */
#define FORMAT_ASCII  3       /**< ASCII format                       */
#define FORMAT_FIXED  4       /**< Fixed length binary records        */
#define FORMAT_DUMP   5       /**< Dump (detailed ASCII) format       */
#define FORMAT_HBOOK  6       /**< CERN hbook (rz) format             */
#define FORMAT_ROOT   7       /**< CERN ROOT format                   */

/**
Event Sampling type */
#define GET_ALL   (1<<0)      /**< get all events (consume)           */
#define GET_SOME  (1<<1)      /**< get as much as possible (sampling) */
#define GET_FARM  (1<<2)      /**< distribute events over several
                                 clients (farming)                  */

/**
Data types Definition                         min      max    */
#define TID_BYTE      1       /**< unsigned byte         0       255    */
#define TID_SBYTE     2       /**< signed byte         -128      127    */
#define TID_CHAR      3       /**< single character      0       255    */
#define TID_WORD      4       /**< two bytes             0      65535   */
#define TID_SHORT     5       /**< signed word        -32768    32767   */
#define TID_DWORD     6       /**< four bytes            0      2^32-1  */
#define TID_INT       7       /**< signed dword        -2^31    2^31-1  */
#define TID_BOOL      8       /**< four bytes bool       0        1     */
#define TID_FLOAT     9       /**< 4 Byte float format                  */
#define TID_DOUBLE   10       /**< 8 Byte float format                  */
#define TID_BITFIELD 11       /**< 32 Bits Bitfield      0  111... (32) */
#define TID_STRING   12       /**< zero terminated string               */
#define TID_ARRAY    13       /**< array with unknown contents          */
#define TID_STRUCT   14       /**< structure with fixed length          */
#define TID_KEY      15       /**< key in online database               */
#define TID_LINK     16       /**< link in online database              */
#define TID_LAST     17       /**< end of TID list indicator            */

/**
Synchronous / Asynchronous flags */
#define SYNC          0
#define ASYNC         1

/**
Access modes */
#define MODE_READ      (1<<0)
#define MODE_WRITE     (1<<1)
#define MODE_DELETE    (1<<2)
#define MODE_EXCLUSIVE (1<<3)
#define MODE_ALLOC     (1<<7)

/**
RPC options */
#define RPC_OTIMEOUT       1
#define RPC_OTRANSPORT     2
#define RPC_OCONVERT_FLAG  3
#define RPC_OHW_TYPE       4
#define RPC_OSERVER_TYPE   5
#define RPC_OSERVER_NAME   6
#define RPC_CONVERT_FLAGS  7
#define RPC_ODB_HANDLE     8
#define RPC_CLIENT_HANDLE  9
#define RPC_SEND_SOCK      10
#define RPC_WATCHDOG_TIMEOUT 11
#define RPC_NODELAY        12

#define RPC_TCP            0
#define RPC_FTCP           1

/**
Watchdog flags */
#define WF_WATCH_ME   (1<<0)    /* see cm_set_watchdog_flags   */
#define WF_CALL_WD    (1<<1)

/**
Transitions values */
#define TR_START      (1<<0)  /**< Start transition  */
#define TR_STOP       (1<<1)  /**< Stop transition  */
#define TR_PAUSE      (1<<2)  /**< Pause transition */
#define TR_RESUME     (1<<3)  /**< Resume transition  */
#define TR_DEFERRED   (1<<12)

/** 
Equipment types */
#define EQ_PERIODIC   (1<<0)   /**< Periodic Event */
#define EQ_POLLED     (1<<1)   /**< Polling Event */
#define EQ_INTERRUPT  (1<<2)   /**< Interrupt Event */
#define EQ_SLOW       (1<<3)   /**< Slow Control Event */
#define EQ_MANUAL_TRIG (1<<4)  /**< Manual triggered Event */
#define EQ_FRAGMENTED (1<<5)   /**< Fragmented Event */
#define EQ_EB         (1<<6)   /**< Event run through the event builder */


/** 
Read - On flags */
#define RO_RUNNING    (1<<0)   /**< While running */
#define RO_STOPPED    (1<<1)   /**< Before stopping the run */
#define RO_PAUSED     (1<<2)   /**< ??? */
#define RO_BOR        (1<<3)   /**< At the Begin of run */
#define RO_EOR        (1<<4)   /**< At the End of run */
#define RO_PAUSE      (1<<5)   /**< Before pausing the run */
#define RO_RESUME     (1<<6)   /**< Before resuming the run */

#define RO_TRANSITIONS (RO_BOR|RO_EOR|RO_PAUSE|RO_RESUME)      /**< At all transitions */
#define RO_ALWAYS      (0xFF)      /**<  Always (independent of the run status) */

#define RO_ODB        (1<<8)   /**< Submit data to ODB only */

/**dox***************************************************************/
/** @} */ /* end of mdefineh */

/**
special characters */
#define CH_BS             8
#define CH_TAB            9
#define CH_CR            13

#define CH_EXT 0x100

#define CH_HOME   (CH_EXT+0)
#define CH_INSERT (CH_EXT+1)
#define CH_DELETE (CH_EXT+2)
#define CH_END    (CH_EXT+3)
#define CH_PUP    (CH_EXT+4)
#define CH_PDOWN  (CH_EXT+5)
#define CH_UP     (CH_EXT+6)
#define CH_DOWN   (CH_EXT+7)
#define CH_RIGHT  (CH_EXT+8)
#define CH_LEFT   (CH_EXT+9)

/* event sources in equipment */
/**
Code the LAM crate and LAM station into a bitwise register.
@param c Crate number
@param s Slot number
*/
#define LAM_SOURCE(c, s)         (c<<24 | ((s) & 0xFFFFFF))

/** 
Code the Station number bitwise for the LAM source.
@param s Slot number
*/
#define LAM_STATION(s)           (1<<(s-1))

/** 
Convert the coded LAM crate to Crate number.
@param c coded crate
*/
#define LAM_SOURCE_CRATE(c)      (c>>24)

/** 
Convert the coded LAM station to Station number.
@param s Slot number
*/
#define LAM_SOURCE_STATION(s)    ((s) & 0xFFFFFF)

/**
CNAF commands */
#define CNAF        0x1         /* normal read/write                */
#define CNAF_nQ     0x2         /* Repeat read until noQ            */

#define CNAF_INHIBIT_SET         0x100
#define CNAF_INHIBIT_CLEAR       0x101
#define CNAF_CRATE_CLEAR         0x102
#define CNAF_CRATE_ZINIT         0x103
#define CNAF_TEST                0x110

/**dox***************************************************************/
/** @addtogroup mmacroh
 *  
 *  @{
 */

/**
MAX */
#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

/**
MIN */
#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

/*------------------------------------------------------------------*/

/**
Align macro for data alignment on 8-byte boundary */
#define ALIGN8(x)  (((x)+7) & ~7)

/**
Align macro for variable data alignment */
#define VALIGN(adr,align)  (((PTYPE) (adr)+align-1) & ~(align-1))

/**dox***************************************************************/
/** @} */ /* end of mmacroh */
 
/**dox***************************************************************/
/** @addtogroup mdefineh
 *  
 *  @{  */
/*
* Bit flags */
#define EVENTID_ALL        -1   /* any event id                   */
#define TRIGGER_ALL        -1   /* any type of trigger            */

/**
System message types */
#define MT_ERROR           (1<<0)     /**< - */
#define MT_INFO            (1<<1)     /**< - */
#define MT_DEBUG           (1<<2)     /**< - */
#define MT_USER            (1<<3)     /**< - */
#define MT_LOG             (1<<4)     /**< - */
#define MT_TALK            (1<<5)     /**< - */
#define MT_CALL            (1<<6)     /**< - */
#define MT_ALL              0xFF      /**< - */

#define MERROR             MT_ERROR, __FILE__, __LINE__ /**< - */
#define MINFO              MT_INFO,  __FILE__, __LINE__ /**< - */
#define MDEBUG             MT_DEBUG, __FILE__, __LINE__ /**< - */
#define MUSER              MT_USER,  __FILE__, __LINE__ /**< produced by interactive user */
#define MLOG               MT_LOG,   __FILE__, __LINE__ /**< info message which is only logged */
#define MTALK              MT_TALK,  __FILE__, __LINE__ /**< info message for speech system */
#define MCALL              MT_CALL,  __FILE__, __LINE__ /**< info message for telephone call */

/**dox***************************************************************/
/** @} */ /* end of mdefineh */


/**dox***************************************************************/
/** @addtogroup mdeferrorh
 *  
 *  @{
 */

/**dox***************************************************************/
/**
 @defgroup err21 Status and error codes
 @{ */
#define SUCCESS                       1 /**< Success */
#define CM_SUCCESS                    1 /**< Same  */
#define CM_SET_ERROR                102 /**< set  */
#define CM_NO_CLIENT                103 /**< nobody */
#define CM_DB_ERROR                 104 /**< db access error */
#define CM_UNDEF_EXP                105 /**< - */
#define CM_VERSION_MISMATCH         106 /**< - */
#define CM_SHUTDOWN                 107 /**< - */
#define CM_WRONG_PASSWORD           108 /**< - */
#define CM_UNDEF_ENVIRON            109 /**< - */
#define CM_DEFERRED_TRANSITION      110 /**< - */
#define CM_TRANSITION_IN_PROGRESS   111 /**< - */
#define CM_TIMEOUT                  112 /**< - */
#define CM_INVALID_TRANSITION       113 /**< - */
#define CM_TOO_MANY_REQUESTS        114 /**< - */
/**dox***************************************************************/
/** @} */ /* end of err21 */

/**dox***************************************************************/
/**
 @defgroup err22 Buffer Manager error codes
 @{ */
#define BM_SUCCESS                    1   /**< - */
#define BM_CREATED                  202   /**< - */
#define BM_NO_MEMORY                203   /**< - */
#define BM_INVALID_NAME             204   /**< - */
#define BM_INVALID_HANDLE           205   /**< - */
#define BM_NO_SLOT                  206   /**< - */
#define BM_NO_MUTEX                 207   /**< - */
#define BM_NOT_FOUND                208   /**< - */
#define BM_ASYNC_RETURN             209   /**< - */
#define BM_TRUNCATED                210   /**< - */
#define BM_MULTIPLE_HOSTS           211   /**< - */
#define BM_MEMSIZE_MISMATCH         212   /**< - */
#define BM_CONFLICT                 213   /**< - */
#define BM_EXIT                     214   /**< - */
#define BM_INVALID_PARAM            215   /**< - */
#define BM_MORE_EVENTS              216   /**< - */
#define BM_INVALID_MIXING           217   /**< - */
#define BM_NO_SHM                   218   /**< - */
/**dox***************************************************************/
/** @} */ /* end of group 22 */

/**dox***************************************************************/
/**  @defgroup err23 Online Database error codes 
@{ */
#define DB_SUCCESS                    1   /**< - */
#define DB_CREATED                  302   /**< - */
#define DB_NO_MEMORY                303   /**< - */
#define DB_INVALID_NAME             304   /**< - */
#define DB_INVALID_HANDLE           305   /**< - */
#define DB_NO_SLOT                  306   /**< - */
#define DB_NO_MUTEX                 307   /**< - */
#define DB_MEMSIZE_MISMATCH         308   /**< - */
#define DB_INVALID_PARAM            309   /**< - */
#define DB_FULL                     310   /**< - */
#define DB_KEY_EXIST                311   /**< - */
#define DB_NO_KEY                   312   /**< - */
#define DB_KEY_CREATED              313   /**< - */
#define DB_TRUNCATED                314   /**< - */
#define DB_TYPE_MISMATCH            315   /**< - */
#define DB_NO_MORE_SUBKEYS          316   /**< - */
#define DB_FILE_ERROR               317   /**< - */
#define DB_NO_ACCESS                318   /**< - */
#define DB_STRUCT_SIZE_MISMATCH     319   /**< - */
#define DB_OPEN_RECORD              320   /**< - */
#define DB_OUT_OF_RANGE             321   /**< - */
#define DB_INVALID_LINK             322   /**< - */
#define DB_CORRUPTED                323   /**< - */
#define DB_STRUCT_MISMATCH          324   /**< - */
#define DB_TIMEOUT                  325   /**< - */
#define DB_VERSION_MISMATCH         326   /**< - */
/**dox***************************************************************/
/** @} */ /* end of group 23 */

/**dox***************************************************************/
/**  @defgroup err24 System Services error code
@{ */
#define SS_SUCCESS                    1   /**< - */
#define SS_CREATED                  402   /**< - */
#define SS_NO_MEMORY                403   /**< - */
#define SS_INVALID_NAME             404   /**< - */
#define SS_INVALID_HANDLE           405   /**< - */
#define SS_INVALID_ADDRESS          406   /**< - */
#define SS_FILE_ERROR               407   /**< - */
#define SS_NO_MUTEX                 408   /**< - */
#define SS_NO_PROCESS               409   /**< - */
#define SS_NO_THREAD                410   /**< - */
#define SS_SOCKET_ERROR             411   /**< - */
#define SS_TIMEOUT                  412   /**< - */
#define SS_SERVER_RECV              413   /**< - */
#define SS_CLIENT_RECV              414   /**< - */
#define SS_ABORT                    415   /**< - */
#define SS_EXIT                     416   /**< - */
#define SS_NO_TAPE                  417   /**< - */
#define SS_DEV_BUSY                 418   /**< - */
#define SS_IO_ERROR                 419   /**< - */
#define SS_TAPE_ERROR               420   /**< - */
#define SS_NO_DRIVER                421   /**< - */
#define SS_END_OF_TAPE              422   /**< - */
#define SS_END_OF_FILE              423   /**< - */
#define SS_FILE_EXISTS              424   /**< - */
#define SS_NO_SPACE                 425   /**< - */
#define SS_INVALID_FORMAT           426   /**< - */
#define SS_NO_ROOT                  427   /**< - */
/**dox***************************************************************/
/** @} */ /* end of group 24 */

/**dox***************************************************************/
/**  @defgroup err25 Remote Procedure Calls error codes
@{ */
#define RPC_SUCCESS                   1   /**< - */
#define RPC_ABORT              SS_ABORT   /**< - */
#define RPC_NO_CONNECTION           502   /**< - */
#define RPC_NET_ERROR               503   /**< - */
#define RPC_TIMEOUT                 504   /**< - */
#define RPC_EXCEED_BUFFER           505   /**< - */
#define RPC_NOT_REGISTERED          506   /**< - */
#define RPC_CONNCLOSED              507   /**< - */
#define RPC_INVALID_ID              508   /**< - */
#define RPC_SHUTDOWN                509   /**< - */
#define RPC_NO_MEMORY               510   /**< - */
#define RPC_DOUBLE_DEFINED          511   /**< - */
/**dox***************************************************************/
/** @} */ /* end of group 25 */

/**dox***************************************************************/
/**  @defgroup err26 Other errors
@{ */
#define FE_SUCCESS                    1   /**< - */
#define FE_ERR_ODB                  602   /**< - */
#define FE_ERR_HW                   603   /**< - */
#define FE_ERR_DISABLED             604   /**< - */
#define FE_ERR_DRIVER               605   /**< - */

/** 
History error code */
#define HS_SUCCESS                    1   /**< - */
#define HS_FILE_ERROR               702   /**< - */
#define HS_NO_MEMORY                703   /**< - */
#define HS_TRUNCATED                704   /**< - */
#define HS_WRONG_INDEX              705   /**< - */
#define HS_UNDEFINED_EVENT          706   /**< - */
#define HS_UNDEFINED_VAR            707   /**< - */

/** 
FTP error code */
#define FTP_SUCCESS                   1   /**< - */
#define FTP_NET_ERROR               802   /**< - */
#define FTP_FILE_ERROR              803   /**< - */
#define FTP_RESPONSE_ERROR          804   /**< - */
#define FTP_INVALID_ARG             805   /**< - */

/** 
ELog error code */
#define EL_SUCCESS                    1   /**< - */
#define EL_FILE_ERROR               902   /**< - */
#define EL_NO_MESSAGE               903   /**< - */
#define EL_TRUNCATED                904   /**< - */
#define EL_FIRST_MSG                905   /**< - */
#define EL_LAST_MSG                 906   /**< - */

/** 
Alarm error code */
#define AL_SUCCESS                    1   /**< - */
#define AL_INVALID_NAME            1002   /**< - */
#define AL_ERROR_ODB               1003   /**< - */
#define AL_RESET                   1004   /**< - */

/** 
Slow control commands error code */
#define CMD_INIT                    (1<<0)
#define CMD_EXIT                    (1<<1)
#define CMD_IDLE                    (1<<2)
#define CMD_SET                     (1<<3)
#define CMD_SET_ALL                 (1<<4)
#define CMD_GET                     (1<<5)
#define CMD_GET_ALL                 (1<<6)
#define CMD_GET_CURRENT             (1<<7)
#define CMD_GET_CURRENT_ALL         (1<<8)
#define CMD_SET_CURRENT_LIMIT       (1<<9)
#define CMD_SET_CURRENT_LIMIT_ALL  (1<<10)
#define CMD_GET_DEMAND             (1<<11)
#define CMD_GET_DEFAULT_NAME       (1<<12)
#define CMD_GET_DEFAULT_THRESHOLD  (1<<13)
#define CMD_SET_LABEL              (1<<14)
#define CMD_ENABLE_COMMAND         (1<<15)
#define CMD_DISABLE_COMMAND        (1<<16)

/** 
Bus driver commands */
#define CMD_WRITE                   100
#define CMD_READ                    101
#define CMD_PUTS                    102
#define CMD_GETS                    103
#define CMD_DEBUG                   104
#define CMD_NAME                    105

/** 
Commands for interrupt events error code */
#define CMD_INTERRUPT_ENABLE        100
#define CMD_INTERRUPT_DISABLE       101
#define CMD_INTERRUPT_ATTACH        102
#define CMD_INTERRUPT_DETACH        103

/**
macros for bus driver access */
#define BD_GETS(s,z,p,t)   info->bd(CMD_GETS, info->bd_info, s, z, p, t)
#define BD_READS(s,z,p,t)  info->bd(CMD_READ, info->bd_info, s, z, p, t)
#define BD_PUTS(s)         info->bd(CMD_PUTS, info->bd_info, s)
#define BD_WRITES(s)       info->bd(CMD_WRITE, info->bd_info, s)

/**dox***************************************************************/
/** @} */ /* end of 26 */

/**dox***************************************************************/
/** @} */ /* end of mdeferrorh */


#define ANA_CONTINUE                  1
#define ANA_SKIP                      0


/*---- Buffer manager structures -----------------------------------*/

/**dox***************************************************************/
/** @addtogroup msectionh
 *  
 *  @{  */

/**dox***************************************************************/
/** @defgroup mbufferh Buffer Section
 *  @{  */
/**
Event header */
typedef struct {
   short int event_id;           /**< event ID starting from one      */
   short int trigger_mask;       /**< hardware trigger mask           */
   DWORD serial_number;          /**< serial number starting from one */
   DWORD time_stamp;             /**< time of production of event     */
   DWORD data_size;              /**< size of event in bytes w/o header */
} EVENT_HEADER;

/**
TRIGGER_MASK
Extract or set the trigger mask field pointed by the argument.
@param e pointer to the midas event (pevent)
*/
#define TRIGGER_MASK(e)    ((((EVENT_HEADER *) e)-1)->trigger_mask)

/**
EVENT_ID
Extract or set the event ID field pointed by the argument..
@param e pointer to the midas event (pevent)
*/
#define EVENT_ID(e)        ((((EVENT_HEADER *) e)-1)->event_id)

/**
SERIAL_NUMBER
Extract or set/reset the serial number field pointed by the argument.
@param e pointer to the midas event (pevent)
*/
#define SERIAL_NUMBER(e)   ((((EVENT_HEADER *) e)-1)->serial_number)

/**
TIME_STAMP
Extract or set/reset the time stamp field pointed by the argument.
@param e pointer to the midas event (pevent)
*/
#define TIME_STAMP(e)      ((((EVENT_HEADER *) e)-1)->time_stamp)
#define EVENT_SOURCE(e,o)  (* (INT*) (e+o))

/**
system event IDs */
#define EVENTID_BOR      ((short int) 0x8000)  /**< Begin-of-run      */
#define EVENTID_EOR      ((short int) 0x8001)  /**< End-of-run        */
#define EVENTID_MESSAGE  ((short int) 0x8002)  /**< Message events    */

/**
fragmented events */
#define EVENTID_FRAG1    ((unsigned short) 0xC000)      /* first fragment */
#define EVENTID_FRAG     ((unsigned short) 0xD000)      /* added to real event-id */

/**
magic number used in trigger_mask for BOR event */
#define MIDAS_MAGIC      0x494d            /**< 'MI' */


/**
Buffer structure */
typedef struct {
   INT id;                       /**< request id                      */
   BOOL valid;                   /**< indicating a valid entry        */
   short int event_id;           /**< event ID                        */
   short int trigger_mask;       /**< trigger mask                    */
   INT sampling_type;            /**< GET_ALL, GET_SOME, GET_FARM     */
                                 /**< dispatch function */
   void (*dispatch) (HNDLE, HNDLE, EVENT_HEADER *, void *);
} EVENT_REQUEST;

typedef struct {
   char name[NAME_LENGTH];            /**< name of client             */
   INT pid;                           /**< process ID                 */
   INT tid;                           /**< thread ID                  */
   INT thandle;                       /**< thread handle              */
   INT port;                          /**< UDP port for wake up       */
   INT read_pointer;                  /**< read pointer to buffer     */
   INT max_request_index;             /**< index of last request      */
   INT num_received_events;           /**< no of received events      */
   INT num_sent_events;               /**< no of sent events          */
   INT num_waiting_events;            /**< no of waiting events       */
   float data_rate;                   /**< data rate in kB/sec        */
   BOOL read_wait;                    /**< wait for read - flag       */
   INT write_wait;                    /**< wait for write # bytes     */
   BOOL wake_up;                      /**< client got a wake-up msg   */
   BOOL all_flag;                     /**< at least one GET_ALL request */
   DWORD last_activity;               /**< time of last activity      */
   DWORD watchdog_timeout;            /**< timeout in ms              */

   EVENT_REQUEST event_request[MAX_EVENT_REQUESTS];

} BUFFER_CLIENT;

typedef struct {
   char name[NAME_LENGTH];            /**< name of buffer             */
   INT num_clients;                   /**< no of active clients       */
   INT max_client_index;              /**< index of last client       */
   INT size;                          /**< size of data area in bytes */
   INT read_pointer;                  /**< read pointer               */
   INT write_pointer;                 /**< read pointer               */
   INT num_in_events;                 /**< no of received events      */
   INT num_out_events;                /**< no of distributed events   */

   BUFFER_CLIENT client[MAX_CLIENTS]; /**< entries for clients        */

} BUFFER_HEADER;

/* Per-process buffer access structure (descriptor) */

typedef struct {
   BOOL attached;                   /**< TRUE if buffer is attached   */
   INT client_index;                /**< index to CLIENT str. in buf. */
   BUFFER_HEADER *buffer_header;    /**< pointer to buffer header     */
   void *buffer_data;               /**< pointer to buffer data       */
   char *read_cache;                /**< cache for burst read         */
   INT read_cache_size;             /**< cache size in bytes          */
   INT read_cache_rp;               /**< cache read pointer           */
   INT read_cache_wp;               /**< cache write pointer          */
   char *write_cache;               /**< cache for burst read         */
   INT write_cache_size;            /**< cache size in bytes          */
   INT write_cache_rp;              /**< cache read pointer           */
   INT write_cache_wp;              /**< cache write pointer          */
   HNDLE mutex;                     /**< mutex/semaphore handle       */
   INT shm_handle;                  /**< handle to shared memory      */
   INT index;                       /**< connection index / tid       */
   BOOL callback;                   /**< callback defined for this buffer */

} BUFFER;

typedef struct {
   DWORD type;                        /**< TID_xxx type                      */
   INT num_values;                    /**< number of values                  */
   char name[NAME_LENGTH];            /**< name of variable                  */
   INT data;                          /**< Address of variable (offset)      */
   INT total_size;                    /**< Total size of data block          */
   INT item_size;                     /**< Size of single data item          */
   WORD access_mode;                  /**< Access mode                       */
   WORD notify_count;                 /**< Notify counter                    */
   INT next_key;                      /**< Address of next key               */
   INT parent_keylist;                /**< keylist to which this key belongs */
   INT last_written;                  /**< Time of last write action  */
} KEY;

typedef struct {
   INT parent;                        /**< Address of parent key      */
   INT num_keys;                      /**< number of keys             */
   INT first_key;                     /**< Address of first key       */
} KEYLIST;

/**dox***************************************************************/
/** @} */ /* end of msectionh */

/*---- Equipment ---------------------------------------------------*/

/**dox***************************************************************/
/** @defgroup mequipment Equipment related
 *  @{  */

#define DF_INPUT       (1<<0)         /**< channel is input           */
#define DF_OUTPUT      (1<<1)         /**< channel is output          */
#define DF_PRIO_DEVICE (1<<2)         /**< get demand values from device instead of ODB */
#define DF_READ_ONLY   (1<<3)         /**< never write demand values to device */

typedef struct {
   char name[NAME_LENGTH];            /**< Driver name                       */
    INT(*bd) (INT cmd, ...);          /**< Device driver entry point         */
   void *bd_info;                     /**< Private info for bus driver       */
} BUS_DRIVER;

typedef struct {
   char name[NAME_LENGTH];            /**< Driver name                       */
    INT(*dd) (INT cmd, ...);          /**< Device driver entry point         */
   INT channels;                      /**< Number of channels                */
    INT(*bd) (INT cmd, ...);          /**< Bus driver entry point            */
   DWORD flags;                       /**< Combination of DF_xx              */
   void *dd_info;                     /**< Private info for device driver    */
} DEVICE_DRIVER;

typedef struct {
   WORD event_id;                     /**< Event ID associated with equipm.  */
   WORD trigger_mask;                 /**< Trigger mask                      */
   char buffer[NAME_LENGTH];          /**< Event buffer to send events into  */
   INT eq_type;                       /**< One of EQ_xxx                     */
   INT source;                        /**< Event source (LAM/IRQ)            */
   char format[8];                    /**< Data format to produce            */
   BOOL enabled;                      /**< Enable flag                       */
   INT read_on;                       /**< Combination of Read-On flags RO_xxx */
   INT period;                        /**< Readout interval/Polling time in ms */
   double event_limit;                /**< Stop run when limit is reached    */
   DWORD num_subevents;               /**< Number of events in super event */
   INT history;                       /**< Log history                       */
   char frontend_host[NAME_LENGTH];   /**< Host on which FE is running       */
   char frontend_name[NAME_LENGTH];   /**< Frontend name                     */
   char frontend_file_name[256];      /**< Source file used for user FE      */
} EQUIPMENT_INFO;

typedef struct {
   double events_sent;
   double events_per_sec;
   double kbytes_per_sec;
} EQUIPMENT_STATS;

typedef struct eqpmnt *PEQUIPMENT;

typedef struct eqpmnt {
   char name[NAME_LENGTH];              /**< Equipment name                            */
   EQUIPMENT_INFO info;                 /**< From above                                */
    INT(*readout) (char *, INT);        /**< Pointer to user readout routine           */
    INT(*cd) (INT cmd, PEQUIPMENT);     /**< Class driver routine                      */
   DEVICE_DRIVER *driver;               /**< Device driver list                        */
   void *event_descrip;                 /**< Init string for fixed events or bank list */
   void *cd_info;                       /**< private data for class driver             */
   INT status;                          /**< One of FE_xxx                             */
   DWORD last_called;                   /**< Last time event was read                  */
   DWORD last_idle;                     /**< Last time idle func was called            */
   DWORD poll_count;                    /**< Needed to poll 'period'                   */
   INT format;                          /**< FORMAT_xxx                                */
   HNDLE buffer_handle;                 /**< MIDAS buffer handle                       */
   HNDLE hkey_variables;                /**< Key to variables subtree in ODB           */
   DWORD serial_number;                 /**< event serial number                       */
   DWORD subevent_number;               /**< subevent number                           */
   DWORD odb_out;                       /**< # updates FE -> ODB                       */
   DWORD odb_in;                        /**< # updated ODB -> FE                       */
   DWORD bytes_sent;                    /**< number of bytes sent                      */
   DWORD events_sent;                   /**< number of events sent                     */
   EQUIPMENT_STATS stats;
} EQUIPMENT;
/**dox***************************************************************/
/** @} */ /* end of mequipmenth */

/*---- Banks -------------------------------------------------------*/

/**dox***************************************************************/
/** @defgroup mbank Bank related
 *  @{  */

#define BANK_FORMAT_VERSION     1      /**< - */
#define BANK_FORMAT_32BIT   (1<<4)     /**< - */

typedef struct {
   DWORD data_size;                    /**< Size in bytes */
   DWORD flags;                        /**< internal flag */
} BANK_HEADER;

typedef struct {
   char name[4];                       /**< - */
   WORD type;                          /**< - */
   WORD data_size;                     /**< - */
} BANK;

typedef struct {
   char name[4];                       /**< - */
   DWORD type;                         /**< - */
   DWORD data_size;                    /**< - */
} BANK32;

typedef struct {
   char name[NAME_LENGTH];             /**< - */
   DWORD type;                         /**< - */
   DWORD n_data;                       /**< - */
} TAG;

typedef struct {
   char name[9];                       /**< - */
   WORD type;                          /**< - */
   DWORD size;                         /**< - */
   char **init_str;                    /**< - */
   BOOL output_flag;                   /**< - */
   void *addr;                         /**< - */
   DWORD n_data;                       /**< - */
   HNDLE def_key;                      /**< - */
} BANK_LIST;
/**dox***************************************************************/
/** @} */ /* end of mbank */

/*---- Analyzer request --------------------------------------------*/
/**dox***************************************************************/
/** @defgroup manalyzer Analyzer related
 *  @{  */

typedef struct {
   char name[NAME_LENGTH];            /**< Module name                       */
   char author[NAME_LENGTH];          /**< Author                            */
    INT(*analyzer) (EVENT_HEADER *, void *);
                                           /**< Pointer to user analyzer routine  */
    INT(*bor) (INT run_number);       /**< Pointer to begin-of-run routine   */
    INT(*eor) (INT run_number);       /**< Pointer to end-of-run routine     */
    INT(*init) ();                    /**< Pointer to init routine           */
    INT(*exit) ();                    /**< Pointer to exit routine           */
   void *parameters;                  /**< Pointer to parameter structure    */
   INT param_size;                    /**< Size of parameter structure       */
   char **init_str;                   /**< Parameter init string             */
   BOOL enabled;                      /**< Enabled flag                      */
   void *histo_folder;
} ANA_MODULE;

typedef struct {
   INT event_id;                      /**< Event ID associated with equipm.  */
   INT trigger_mask;                  /**< Trigger mask                      */
   INT sampling_type;                 /**< GET_ALL/GET_SOME                  */
   char buffer[NAME_LENGTH];          /**< Event buffer to send events into  */
   BOOL enabled;                      /**< Enable flag                       */
   char client_name[NAME_LENGTH];     /**< Analyzer name                     */
   char host[NAME_LENGTH];            /**< Host on which analyzer is running */
} AR_INFO;

typedef struct {
   double events_received;
   double events_per_sec;
   double events_written;
} AR_STATS;

typedef struct {
   char event_name[NAME_LENGTH];      /**< Event name                        */
   AR_INFO ar_info;                   /**< From above                        */
   INT(*analyzer) (EVENT_HEADER *, void *); /**< Pointer to user analyzer routine  */
   ANA_MODULE **ana_module;           /**< List of analyzer modules          */
   BANK_LIST *bank_list;              /**< List of banks for event           */
   INT rwnt_buffer_size;              /**< Size in events of RW N-tuple buf  */
   BOOL use_tests;                    /**< Use tests for this event          */
   char **init_string;
   INT status;                        /**< One of FE_xxx                     */
   HNDLE buffer_handle;               /**< MIDAS buffer handle               */
   HNDLE request_id;                  /**< Event request handle              */
   HNDLE hkey_variables;              /**< Key to variables subtree in ODB   */
   HNDLE hkey_common;                 /**< Key to common subtree             */
   void *addr;                        /**< Buffer for CWNT filling           */
   struct {                          
      DWORD run;
      DWORD serial;
      DWORD time;
   } number;                          /**< Buffer for event number for CWNT  */    
   DWORD events_received;             /**< number of events sent             */
   DWORD events_written;              /**< number of events written          */
   AR_STATS ar_stats;

} ANALYZE_REQUEST;

/*---- Tests -------------------------------------------------------*/

typedef struct {
   char name[80];
   BOOL registered;
   DWORD count;
   DWORD previous_count;
   BOOL value;
} ANA_TEST;

#define SET_TEST(t, v) { if (!t.registered) test_register(&t); t.value = (v); }
#define TEST(t) (t.value)

#ifdef DEFINE_TESTS
#define DEF_TEST(t) ANA_TEST t = { #t, 0, 0, FALSE };
#else
#define DEF_TEST(t) extern ANA_TEST t;
#endif
/**dox***************************************************************/
/** @} */ /* end of mbank */

/*---- History structures ------------------------------------------*/

/**dox***************************************************************/
/** @defgroup mhistoryh History related
 *  @{  */

#define RT_DATA (*((DWORD *) "HSDA"))
#define RT_DEF  (*((DWORD *) "HSDF"))

typedef struct {
   DWORD record_type;           /* RT_DATA or RT_DEF */
   DWORD event_id;
   DWORD time;
   DWORD def_offset;            /* place of definition */
   DWORD data_size;             /* data following this header in bytes */
} HIST_RECORD;

typedef struct {
   DWORD event_id;
   char event_name[NAME_LENGTH];
   DWORD def_offset;
} DEF_RECORD;

typedef struct {
   DWORD event_id;
   DWORD time;
   DWORD offset;
} INDEX_RECORD;

typedef struct {
   DWORD event_id;
   char event_name[NAME_LENGTH];
   DWORD n_tag;
   TAG *tag;
   DWORD hist_fh;
   DWORD index_fh;
   DWORD def_fh;
   DWORD base_time;
   DWORD def_offset;
} HISTORY;
/**dox***************************************************************/
/** @} */ /* end of mhistory */

/*---- ODB runinfo -------------------------------------------------*/

/**dox***************************************************************/
/** @defgroup modbh ODB runinfo related
 *  @{  */
/** Contains the main parameters regarding the run status.
    The containt reflects the current system ONLY if Midas clients
    are connected. Otherwise the status is erroneous.
*/
typedef struct {
   INT state;                         /**< Current run condition  */
   INT online_mode;                   /**< Mode of operation online/offline */
   INT run_number;                    /**< Current processing run number      */
   INT transition_in_progress;        /**< Intermediate state during transition */
   INT requested_transition;          /**< Deferred transition request */
   char start_time[32];               /**< ASCII of the last start time */
   DWORD start_time_binary;           /**< Bin of the last start time */
   char stop_time[32];                /**< ASCII of the last stop time */
   DWORD stop_time_binary;            /**< ASCII of the last stop time */
} RUNINFO;

#define RUNINFO_STR(_name) char *_name[] = {\
"[.]",\
"State = INT : 1",\
"Online Mode = INT : 1",\
"Run number = INT : 0",\
"Transition in progress = INT : 0",\
"Requested transition = INT : 0",\
"Start time = STRING : [32] Tue Sep 09 15:04:42 1997",\
"Start time binary = DWORD : 0",\
"Stop time = STRING : [32] Tue Sep 09 15:04:42 1997",\
"Stop time binary = DWORD : 0",\
"",\
NULL }
/**dox***************************************************************/
/** @} */ /* end of mhistory */

/*---- Alarm system ------------------------------------------------*/
/**dox***************************************************************/
/** @defgroup malarmh Alarm related
 * Alarm structre. 
 *  @{  */

/********************************************************************/
/**
Program information structure */
typedef struct {
   BOOL required;
   INT watchdog_timeout;
   DWORD check_interval;
   char start_command[256];
   BOOL auto_start;
   BOOL auto_stop;
   BOOL auto_restart;
   char alarm_class[32];
   DWORD first_failed;
} PROGRAM_INFO;

#define AT_INTERNAL   1 /**< - */
#define AT_PROGRAM    2 /**< - */
#define AT_EVALUATED  3 /**< - */
#define AT_PERIODIC   4 /**< - */
#define AT_LAST       4 /**< - */

#define PROGRAM_INFO_STR(_name) char *_name[] = {\
"[.]",\
"Required = BOOL : n",\
"Watchdog timeout = INT : 10000",\
"Check interval = DWORD : 180000",\
"Start command = STRING : [256] ",\
"Auto start = BOOL : n",\
"Auto stop = BOOL : n",\
"Auto restart = BOOL : n",\
"Alarm class = STRING : [32] ",\
"First failed = DWORD : 0",\
"",\
NULL }

/**
Alarm class structure */
typedef struct {
   BOOL write_system_message;
   BOOL write_elog_message;
   INT system_message_interval;
   DWORD system_message_last;
   char execute_command[256];
   INT execute_interval;
   DWORD execute_last;
   BOOL stop_run;
   char display_bgcolor[32];
   char display_fgcolor[32];
} ALARM_CLASS;

#define ALARM_CLASS_STR(_name) char *_name[] = {\
"[.]",\
"Write system message = BOOL : y",\
"Write Elog message = BOOL : n",\
"System message interval = INT : 60",\
"System message last = DWORD : 0",\
"Execute command = STRING : [256] ",\
"Execute interval = INT : 0",\
"Execute last = DWORD : 0",\
"Stop run = BOOL : n",\
"Display BGColor = STRING : [32] red",\
"Display FGColor = STRING : [32] black",\
"",\
NULL }

/**
Alarm structure */
typedef struct {
   BOOL active;
   INT triggered;
   INT type;
   INT check_interval;
   DWORD checked_last;
   char time_triggered_first[32];
   char time_triggered_last[32];
   char condition[256];
   char alarm_class[32];
   char alarm_message[80];
} ALARM;

#define ALARM_ODB_STR(_name) char *_name[] = {\
"[.]",\
"Active = BOOL : n",\
"Triggered = INT : 0",\
"Type = INT : 3",\
"Check interval = INT : 60",\
"Checked last = DWORD : 0",\
"Time triggered first = STRING : [32] ",\
"Time triggered last = STRING : [32] ",\
"Condition = STRING : [256] /Runinfo/Run number > 100",\
"Alarm Class = STRING : [32] Alarm",\
"Alarm Message = STRING : [80] Run number became too large",\
"",\
NULL }

#define ALARM_PERIODIC_STR(_name) char *_name[] = {\
"[.]",\
"Active = BOOL : n",\
"Triggered = INT : 0",\
"Type = INT : 4",\
"Check interval = INT : 28800",\
"Checked last = DWORD : 0",\
"Time triggered first = STRING : [32] ",\
"Time triggered last = STRING : [32] ",\
"Condition = STRING : [256] ",\
"Alarm Class = STRING : [32] Warning",\
"Alarm Message = STRING : [80] Please do your shift checks",\
"",\
NULL }

/**dox***************************************************************/
/** @} */ /* end of malarmh */

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/*---- malloc/free routines for debugging --------------------------*/

#ifdef _MEM_DBG
#define M_MALLOC(x)   dbg_malloc((x), __FILE__, __LINE__)
#define M_CALLOC(x,y) dbg_calloc((x), (y), __FILE__, __LINE__)
#define M_FREE(x)     dbg_free  ((x), __FILE__, __LINE__)
#else
#define M_MALLOC(x) malloc(x)
#define M_CALLOC(x,y) calloc(x,y)
#define M_FREE(x) free(x)
#endif

void *dbg_malloc(unsigned int size, char *file, int line);
void *dbg_calloc(unsigned int size, unsigned int count, char *file, int line);
void dbg_free(void *adr, char *file, int line);

/*---- CERN libray -------------------------------------------------*/

#ifdef extname
#define PAWC_NAME pawc_
#else
#define PAWC_NAME PAWC
#endif

#define PAWC_DEFINE(size) \
INT PAWC_NAME[size/4];    \
INT pawc_size = size

/* bug in ROOT include files */
#undef GetCurrentTime

/*---- RPC ---------------------------------------------------------*/

/**
flags */
#define RPC_IN       (1 << 0)
#define RPC_OUT      (1 << 1)
#define RPC_POINTER  (1 << 2)
#define RPC_FIXARRAY (1 << 3)
#define RPC_VARARRAY (1 << 4)
#define RPC_OUTGOING (1 << 5)

/**
Server types */
#define ST_NONE            0
#define ST_SINGLE          1
#define ST_MTHREAD         2
#define ST_MPROCESS        3
#define ST_SUBPROCESS      4
#define ST_REMOTE          5

/**
function list */
typedef struct {
   WORD tid;
   WORD flags;
   INT n;
} RPC_PARAM;

typedef struct {
   INT id;
   char *name;
   RPC_PARAM param[20];
    INT(*dispatch) (INT, void **);
} RPC_LIST;

/**
IDs allow for users */
#define RPC_MIN_ID    1
#define RPC_MAX_ID 9999

/**
Data conversion flags */
#define CF_ENDIAN          (1<<0)
#define CF_IEEE2VAX        (1<<1)
#define CF_VAX2IEEE        (1<<2)
#define CF_ASCII           (1<<3)

#define CBYTE(_i)        (* ((BYTE *)       prpc_param[_i]))
#define CPBYTE(_i)       (  ((BYTE *)       prpc_param[_i]))

#define CSHORT(_i)       (* ((short *)      prpc_param[_i]))
#define CPSHORT(_i)      (  ((short *)      prpc_param[_i]))

#define CINT(_i)         (* ((INT *)        prpc_param[_i]))
#define CPINT(_i)        (  ((INT *)        prpc_param[_i]))

#define CWORD(_i)        (* ((WORD *)       prpc_param[_i]))
#define CPWORD(_i)       (  ((WORD *)       prpc_param[_i]))

#define CLONG(_i)        (* ((long *)       prpc_param[_i]))
#define CPLONG(_i)       (  ((long *)       prpc_param[_i]))

#define CDWORD(_i)       (* ((DWORD *)      prpc_param[_i]))
#define CPDWORD(_i)      (  ((DWORD *)      prpc_param[_i]))

#define CHNDLE(_i)       (* ((HNDLE *)      prpc_param[_i]))
#define CPHNDLE(_i)      (  ((HNDLE *)      prpc_param[_i]))

#define CBOOL(_i)        (* ((BOOL *)       prpc_param[_i]))
#define CPBOOL(_i)       (  ((BOOL *)       prpc_param[_i]))

#define CFLOAT(_i)       (* ((float *)      prpc_param[_i]))
#define CPFLOAT(_i)      (  ((float *)      prpc_param[_i]))

#define CDOUBLE(_i)      (* ((double *)     prpc_param[_i]))
#define CPDOUBLE(_i)     (  ((double *)     prpc_param[_i]))

#define CSTRING(_i)      (  ((char *)       prpc_param[_i]))
#define CARRAY(_i)       (  ((void *)       prpc_param[_i]))

#define CBYTE(_i)        (* ((BYTE *)       prpc_param[_i]))
#define CPBYTE(_i)       (  ((BYTE *)       prpc_param[_i]))

#define CSHORT(_i)       (* ((short *)      prpc_param[_i]))
#define CPSHORT(_i)      (  ((short *)      prpc_param[_i]))

#define CINT(_i)         (* ((INT *)        prpc_param[_i]))
#define CPINT(_i)        (  ((INT *)        prpc_param[_i]))

#define CWORD(_i)        (* ((WORD *)       prpc_param[_i]))
#define CPWORD(_i)       (  ((WORD *)       prpc_param[_i]))

#define CLONG(_i)        (* ((long *)       prpc_param[_i]))
#define CPLONG(_i)       (  ((long *)       prpc_param[_i]))

#define CDWORD(_i)       (* ((DWORD *)      prpc_param[_i]))
#define CPDWORD(_i)      (  ((DWORD *)      prpc_param[_i]))

#define CHNDLE(_i)       (* ((HNDLE *)      prpc_param[_i]))
#define CPHNDLE(_i)      (  ((HNDLE *)      prpc_param[_i]))

#define CBOOL(_i)        (* ((BOOL *)       prpc_param[_i]))
#define CPBOOL(_i)       (  ((BOOL *)       prpc_param[_i]))

#define CFLOAT(_i)       (* ((float *)      prpc_param[_i]))
#define CPFLOAT(_i)      (  ((float *)      prpc_param[_i]))

#define CDOUBLE(_i)      (* ((double *)     prpc_param[_i]))
#define CPDOUBLE(_i)     (  ((double *)     prpc_param[_i]))

#define CSTRING(_i)      (  ((char *)       prpc_param[_i]))
#define CARRAY(_i)       (  ((void *)       prpc_param[_i]))



#define cBYTE            (* ((BYTE *)       prpc_param[--n_param]))
#define cPBYTE           (  ((BYTE *)       prpc_param[--n_param]))

#define cSHORT           (* ((short *)      prpc_param[--n_param]))
#define cPSHORT          (  ((short *)      prpc_param[--n_param]))

#define cINT             (* ((INT *)        prpc_param[--n_param]))
#define cPINT            (  ((INT *)        prpc_param[--n_param]))

#define cWORD            (* ((WORD *)       prpc_param[--n_param]))
#define cPWORD           (  ((WORD *)       prpc_param[--n_param]))

#define cLONG            (* ((long *)       prpc_param[--n_param]))
#define cPLONG           (  ((long *)       prpc_param[--n_param]))

#define cDWORD           (* ((DWORD *)      prpc_param[--n_param]))
#define cPDWORD          (  ((DWORD *)      prpc_param[--n_param]))

#define cHNDLE           (* ((HNDLE *)      prpc_param[--n_param]))
#define cPHNDLE          (  ((HNDLE *)      prpc_param[--n_param]))

#define cBOOL            (* ((BOOL *)       prpc_param[--n_param]))
#define cPBOOL           (  ((BOOL *)       prpc_param[--n_param]))

#define cFLOAT           (* ((float *)      prpc_param[--n_param]))
#define cPFLOAT          (  ((float *)      prpc_param[--n_param]))

#define cDOUBLE          (* ((double *)     prpc_param[--n_param]))
#define cPDOUBLE         (  ((double *)     prpc_param[--n_param]))

#define cSTRING          (  ((char *)       prpc_param[--n_param]))
#define cARRAY           (  ((void *)       prpc_param[--n_param]))

/*---- Function declarations ---------------------------------------*/

/* make functions callable from a C++ program */
#ifdef __cplusplus
extern "C" {
#endif

/* make functions under WinNT dll exportable */
#if defined(OS_WINNT) && defined(MIDAS_DLL)
#define EXPRT __declspec(dllexport)
#else
#define EXPRT
#endif

   /*---- common routines ----*/
   INT EXPRT cm_get_error(INT code, char *string);
   char EXPRT *cm_get_version(void);
   INT EXPRT cm_get_environment(char *host_name, int host_name_size,
                                char *exp_name, int exp_name_size);
   INT EXPRT cm_list_experiments(char *host_name,
                                 char exp_name[MAX_EXPERIMENT][NAME_LENGTH]);
   INT EXPRT cm_select_experiment(char *host_name, char *exp_name);
   INT EXPRT cm_connect_experiment(char *host_name, char *exp_name,
                                   char *client_name, void (*func) (char *));
   INT EXPRT cm_connect_experiment1(char *host_name, char *exp_name,
                                    char *client_name,
                                    void (*func) (char *), INT odb_size,
                                    DWORD watchdog_timeout);
   INT EXPRT cm_disconnect_experiment(void);
   INT EXPRT cm_register_transition(INT transition, INT(*func) (INT, char *), int sequence_number);
   INT EXPRT cm_set_transition_sequence(INT transition, INT sequence_number);
   INT EXPRT cm_query_transition(int *transition, int *run_number, int *trans_time);
   INT EXPRT cm_register_deferred_transition(INT transition, BOOL(*func) (INT, BOOL));
   INT EXPRT cm_check_deferred_transition(void);
   INT EXPRT cm_transition(INT transition, INT run_number, char *error,
                           INT strsize, INT async_flag, INT debug_flag);
   INT EXPRT cm_register_server(void);
   INT EXPRT cm_register_function(INT id, INT(*func) (INT, void **));
   INT EXPRT cm_connect_client(char *client_name, HNDLE * hConn);
   INT EXPRT cm_disconnect_client(HNDLE hConn, BOOL bShutdown);
   INT EXPRT cm_set_experiment_database(HNDLE hDB, HNDLE hKeyClient);
   INT EXPRT cm_get_experiment_database(HNDLE * hDB, HNDLE * hKeyClient);
   INT EXPRT cm_set_experiment_mutex(INT mutex_alarm, INT mutex_elog);
   INT EXPRT cm_get_experiment_mutex(INT * mutex_alarm, INT * mutex_elog);
   INT EXPRT cm_set_client_info(HNDLE hDB, HNDLE * hKeyClient,
                                char *host_name, char *client_name,
                                INT computer_id, char *password, DWORD watchdog_timeout);
   INT EXPRT cm_get_client_info(char *client_name);
   INT EXPRT cm_set_watchdog_params(BOOL call_watchdog, DWORD timeout);
   INT EXPRT cm_get_watchdog_params(BOOL * call_watchdog, DWORD * timeout);
   INT EXPRT cm_get_watchdog_info(HNDLE hDB, char *client_name,
                                  DWORD * timeout, DWORD * last);
   INT EXPRT cm_enable_watchdog(BOOL flag);
   void EXPRT cm_watchdog(int);
   INT EXPRT cm_shutdown(char *name, BOOL bUnique);
   INT EXPRT cm_exist(char *name, BOOL bUnique);
   INT EXPRT cm_cleanup(char *client_name, BOOL ignore_timeout);
   INT EXPRT cm_yield(INT millisec);
   INT EXPRT cm_execute(char *command, char *result, INT buf_size);
   INT EXPRT cm_synchronize(DWORD * sec);
   INT EXPRT cm_asctime(char *str, INT buf_size);
   INT EXPRT cm_time(DWORD * time);
   BOOL EXPRT cm_is_ctrlc_pressed();
   void EXPRT cm_ack_ctrlc_pressed();

   INT EXPRT cm_set_msg_print(INT system_mask, INT user_mask, int (*func) (const char *));
   INT EXPRT cm_msg(INT message_type, char *filename, INT line,
                    const char *routine, const char *format, ...);
   INT EXPRT cm_msg1(INT message_type, char *filename, INT line,
                     const char *facility, const char *routine, const char *format, ...);
   INT EXPRT cm_msg_register(void (*func)
                              (HNDLE, HNDLE, EVENT_HEADER *, void *));
   INT EXPRT cm_msg_retrieve(INT n_message, char *message, INT * buf_size);

   BOOL EXPRT equal_ustring(char *str1, char *str2);

   /*---- buffer manager ----*/
   INT EXPRT bm_open_buffer(char *buffer_name, INT buffer_size, INT * buffer_handle);
   INT EXPRT bm_close_buffer(INT buffer_handle);
   INT EXPRT bm_close_all_buffers(void);
   INT EXPRT bm_init_buffer_counters(INT buffer_handle);
   INT EXPRT bm_get_buffer_info(INT buffer_handle, BUFFER_HEADER * buffer_header);
   INT EXPRT bm_get_buffer_level(INT buffer_handle, INT * n_bytes);
   INT EXPRT bm_set_cache_size(INT buffer_handle, INT read_size, INT write_size);
   INT EXPRT bm_compose_event(EVENT_HEADER * event_header,
                              short int event_id, short int trigger_mask,
                              DWORD size, DWORD serial);
   INT EXPRT bm_request_event(INT buffer_handle, short int event_id,
                              short int trigger_mask, INT sampling_type,
                              INT * request_id, void (*func) (HNDLE, HNDLE,
                                                              EVENT_HEADER *, void *));
   INT EXPRT bm_add_event_request(INT buffer_handle, short int event_id,
                                  short int trigger_mask,
                                  INT sampling_type, void (*func) (HNDLE,
                                                                   HNDLE,
                                                                   EVENT_HEADER
                                                                   *,
                                                                   void *),
                                  INT request_id);
   INT EXPRT bm_delete_request(INT request_id);
   INT EXPRT bm_send_event(INT buffer_handle, void *event, INT buf_size, INT async_flag);
   INT EXPRT bm_receive_event(INT buffer_handle, void *destination,
                              INT * buf_size, INT async_flag);
   INT EXPRT bm_skip_event(INT buffer_handle);
   INT EXPRT bm_flush_cache(INT buffer_handle, INT async_flag);
   INT EXPRT bm_poll_event(INT flag);
   INT EXPRT bm_empty_buffers(void);

   /*---- online database functions -----*/
   INT EXPRT db_open_database(char *database_name, INT database_size,
                              HNDLE * hdb, char *client_name);
   INT EXPRT db_close_database(HNDLE database_handle);
   INT EXPRT db_close_all_databases(void);
   INT EXPRT db_protect_database(HNDLE database_handle);

   INT EXPRT db_create_key(HNDLE hdb, HNDLE key_handle, char *key_name, DWORD type);
   INT EXPRT db_create_link(HNDLE hdb, HNDLE key_handle, char *link_name,
                            char *destination);
   INT EXPRT db_set_value(HNDLE hdb, HNDLE hKeyRoot, char *key_name,
                          void *data, INT size, INT num_values, DWORD type);
   INT EXPRT db_get_value(HNDLE hdb, HNDLE hKeyRoot, char *key_name,
                          void *data, INT * size, DWORD type, BOOL create);
   INT EXPRT db_find_key(HNDLE hdb, HNDLE hkey, char *name, HNDLE * hsubkey);
   INT EXPRT db_find_link(HNDLE hDB, HNDLE hKey, char *key_name, HNDLE * subhKey);
   INT EXPRT db_find_key1(HNDLE hdb, HNDLE hkey, char *name, HNDLE * hsubkey);
   INT EXPRT db_find_link1(HNDLE hDB, HNDLE hKey, char *key_name, HNDLE * subhKey);
   INT EXPRT db_scan_tree(HNDLE hDB, HNDLE hKey, int level,
                          INT(*callback) (HNDLE, HNDLE, KEY *, INT, void *), void *info);
   INT EXPRT db_scan_tree_link(HNDLE hDB, HNDLE hKey, int level,
                               void (*callback) (HNDLE, HNDLE, KEY *, INT,
                                                 void *), void *info);
   INT EXPRT db_get_path(HNDLE hDB, HNDLE hKey, char *path, INT buf_size);
   INT EXPRT db_delete_key(HNDLE database_handle, HNDLE key_handle, BOOL follow_links);
   INT EXPRT db_enum_key(HNDLE hdb, HNDLE key_handle, INT index, HNDLE * subkey_handle);
   INT EXPRT db_enum_link(HNDLE hdb, HNDLE key_handle, INT index, HNDLE * subkey_handle);
   INT EXPRT db_get_next_link(HNDLE hdb, HNDLE key_handle, HNDLE * subkey_handle);
   INT EXPRT db_get_key(HNDLE hdb, HNDLE key_handle, KEY * key);
   INT EXPRT db_get_key_info(HNDLE hDB, HNDLE hKey, char *name,
                             INT name_size, INT * type, INT * num_values,
                             INT * item_size);
   INT EXPRT db_get_key_time(HNDLE hdb, HNDLE key_handle, DWORD * delta);
   INT EXPRT db_rename_key(HNDLE hDB, HNDLE hKey, char *name);
   INT EXPRT db_reorder_key(HNDLE hDB, HNDLE hKey, INT index);
   INT EXPRT db_get_data(HNDLE hdb, HNDLE key_handle, void *data,
                         INT * buf_size, DWORD type);
   INT EXPRT db_get_data1(HNDLE hDB, HNDLE hKey, void *data,
                          INT * buf_size, DWORD type, INT * num_values);
   INT EXPRT db_get_data_index(HNDLE hDB, HNDLE hKey, void *data,
                               INT * buf_size, INT index, DWORD type);
   INT EXPRT db_set_data(HNDLE hdb, HNDLE hKey, void *data, INT buf_size,
                         INT num_values, DWORD type);
   INT EXPRT db_set_data_index(HNDLE hDB, HNDLE hKey, void *data, INT size,
                               INT index, DWORD type);
   INT EXPRT db_set_data_index2(HNDLE hDB, HNDLE hKey, void *data,
                                INT size, INT index, DWORD type, BOOL bNotify);
   INT EXPRT db_set_num_values(HNDLE hDB, HNDLE hKey, INT num_values);
   INT EXPRT db_merge_data(HNDLE hDB, HNDLE hKeyRoot, char *name,
                           void *data, INT data_size, INT num_values, INT type);
   INT EXPRT db_set_mode(HNDLE hdb, HNDLE key_handle, WORD mode, BOOL recurse);
   INT EXPRT db_create_record(HNDLE hdb, HNDLE hkey, char *name, char *init_str);
   INT EXPRT db_check_record(HNDLE hDB, HNDLE hKey, char *key_name,
                             char *rec_str, BOOL correct);
   INT EXPRT db_open_record(HNDLE hdb, HNDLE hkey, void *ptr, INT rec_size,
                            WORD access, void (*dispatcher) (INT, INT,
                                                             void *), void *info);
   INT EXPRT db_close_record(HNDLE hdb, HNDLE hkey);
   INT EXPRT db_get_record(HNDLE hdb, HNDLE hKey, void *data, INT * buf_size, INT align);
   INT EXPRT db_get_record_size(HNDLE hdb, HNDLE hKey, INT align, INT * buf_size);
   INT EXPRT db_set_record(HNDLE hdb, HNDLE hKey, void *data, INT buf_size, INT align);
   INT EXPRT db_send_changed_records(void);
   INT EXPRT db_get_open_records(HNDLE hDB, HNDLE hKey, char *str,
                                 INT buf_size, BOOL fix);

   INT EXPRT db_add_open_record(HNDLE hDB, HNDLE hKey, WORD access_mode);
   INT EXPRT db_remove_open_record(HNDLE hDB, HNDLE hKey, BOOL lock);

   INT EXPRT db_load(HNDLE hdb, HNDLE key_handle, char *filename, BOOL bRemote);
   INT EXPRT db_save(HNDLE hdb, HNDLE key_handle, char *filename, BOOL bRemote);
   INT EXPRT db_copy(HNDLE hDB, HNDLE hKey, char *buffer, INT * buffer_size, char *path);
   INT EXPRT db_paste(HNDLE hDB, HNDLE hKeyRoot, char *buffer);
   INT EXPRT db_save_struct(HNDLE hDB, HNDLE hKey, char *file_name,
                            char *struct_name, BOOL append);
   INT EXPRT db_save_string(HNDLE hDB, HNDLE hKey, char *file_name,
                            char *string_name, BOOL append);
   INT EXPRT db_save_xml(HNDLE hDB, HNDLE hKey, char *file_name);

   INT EXPRT db_sprintf(char *string, void *data, INT data_size, INT index, DWORD type);
   INT EXPRT db_sprintfh(char *string, void *data, INT data_size, INT index, DWORD type);
   INT EXPRT db_sscanf(char *string, void *data, INT * data_size, INT index, DWORD type);
   char EXPRT *strcomb(char **list);

   /*---- Bank routines ----*/
   void EXPRT bk_init(void *pbh);
   void EXPRT bk_init32(void *event);
   BOOL EXPRT bk_is32(void *event);
   INT EXPRT bk_size(void *pbh);
   void EXPRT bk_create(void *pbh, const char *name, WORD type, void *pdata);
   INT EXPRT bk_delete(void *event, const char *name);
   INT EXPRT bk_close(void *pbh, void *pdata);
   INT EXPRT bk_list(void *pbh, char *bklist);
   INT EXPRT bk_locate(void *pbh, const char *name, void *pdata);
   INT EXPRT bk_iterate(void *pbh, BANK ** pbk, void *pdata);
   INT EXPRT bk_iterate32(void *pbh, BANK32 ** pbk, void *pdata);
   INT EXPRT bk_swap(void *event, BOOL force);
   INT EXPRT bk_find(BANK_HEADER * pbkh, const char *name, DWORD * bklen,
                     DWORD * bktype, void **pdata);

   /*---- RPC routines ----*/
   INT EXPRT rpc_register_functions(RPC_LIST * new_list, INT(*func) (INT, void **));
   INT EXPRT rpc_register_function(INT id, INT(*func) (INT, void **));
   INT EXPRT rpc_get_option(HNDLE hConn, INT item);
   INT EXPRT rpc_set_option(HNDLE hConn, INT item, INT value);
   INT EXPRT rpc_set_name(char *name);
   INT EXPRT rpc_get_name(char *name);
   INT EXPRT rpc_is_remote(void);
   INT EXPRT rpc_set_debug(void (*func) (char *), INT mode);
   void EXPRT rpc_debug_printf(char *format, ...);

   INT EXPRT rpc_register_server(INT server_type, char *name, INT * port,
                                 INT(*func) (INT, void **));
   INT EXPRT rpc_register_client(char *name, RPC_LIST * list);
   INT EXPRT rpc_server_thread(void *pointer);
   INT EXPRT rpc_server_shutdown(void);
   INT EXPRT rpc_client_call(HNDLE hConn, const INT routine_id, ...);
   INT EXPRT rpc_call(const INT routine_id, ...);
   INT EXPRT rpc_tid_size(INT id);
   char EXPRT *rpc_tid_name(INT id);
   INT EXPRT rpc_server_connect(char *host_name, char *exp_name);
   INT EXPRT rpc_client_connect(char *host_name, INT midas_port,
                                char *client_name, HNDLE * hConnection);
   INT EXPRT rpc_client_disconnect(HNDLE hConn, BOOL bShutdown);

   INT EXPRT rpc_send_event(INT buffer_handle, void *source, INT buf_size,
                            INT async_flag);
   INT EXPRT rpc_flush_event(void);

   void EXPRT rpc_get_convert_flags(INT * convert_flags);
   void EXPRT rpc_convert_single(void *data, INT tid, INT flags, INT convert_flags);
   void EXPRT rpc_convert_data(void *data, INT tid, INT flags, INT size,
                               INT convert_flags);

   /*---- system services ----*/
   DWORD EXPRT ss_millitime(void);
   DWORD EXPRT ss_time(void);
   DWORD EXPRT ss_settime(DWORD seconds);
   char EXPRT *ss_asctime(void);
   INT EXPRT ss_sleep(INT millisec);
   BOOL EXPRT ss_kbhit(void);

   void EXPRT ss_clear_screen(void);
   void EXPRT ss_printf(INT x, INT y, const char *format, ...);
   void ss_set_screen_size(int x, int y);

   char EXPRT *ss_getpass(char *prompt);
   INT EXPRT ss_getchar(BOOL reset);
   char EXPRT *ss_crypt(char *key, char *salt);
   char EXPRT *ss_gets(char *string, int size);

   void EXPRT *ss_ctrlc_handler(void (*func) (int));

   /*---- direct io routines ----*/
   INT EXPRT ss_directio_give_port(INT start, INT end);
   INT EXPRT ss_directio_lock_port(INT start, INT end);

   /*---- tape routines ----*/
   INT EXPRT ss_tape_open(char *path, INT oflag, INT * channel);
   INT EXPRT ss_tape_close(INT channel);
   INT EXPRT ss_tape_status(char *path);
   INT EXPRT ss_tape_read(INT channel, void *pdata, INT * count);
   INT EXPRT ss_tape_write(INT channel, void *pdata, INT count);
   INT EXPRT ss_tape_write_eof(INT channel);
   INT EXPRT ss_tape_fskip(INT channel, INT count);
   INT EXPRT ss_tape_rskip(INT channel, INT count);
   INT EXPRT ss_tape_rewind(INT channel);
   INT EXPRT ss_tape_spool(INT channel);
   INT EXPRT ss_tape_mount(INT channel);
   INT EXPRT ss_tape_unmount(INT channel);
   INT EXPRT ss_tape_get_blockn(INT channel);

   /*---- disk routines ----*/
   double EXPRT ss_disk_free(char *path);
   double EXPRT ss_file_size(char *path);
   INT EXPRT ss_file_remove(char *path);
   INT EXPRT ss_file_find(char *path, char *pattern, char **plist);
   double EXPRT ss_disk_size(char *path);

   /*---- history routines ----*/
   INT EXPRT hs_set_path(char *path);
   INT EXPRT hs_define_event(DWORD event_id, char *name, TAG * tag, DWORD size);
   INT EXPRT hs_write_event(DWORD event_id, void *data, DWORD size);
   INT EXPRT hs_count_events(DWORD ltime, DWORD * count);
   INT EXPRT hs_enum_events(DWORD ltime, char *event_name,
                            DWORD * name_size, INT event_id[], DWORD * id_size);
   INT EXPRT hs_count_vars(DWORD ltime, DWORD event_id, DWORD * count);
   INT EXPRT hs_enum_vars(DWORD ltime, DWORD event_id, char *var_name,
                          DWORD * size, DWORD * var_n, DWORD * n_size);
   INT EXPRT hs_get_var(DWORD ltime, DWORD event_id, char *var_name,
                        DWORD * type, INT * n_data);
   INT EXPRT hs_get_event_id(DWORD ltime, char *name, DWORD * id);
   INT EXPRT hs_read(DWORD event_id, DWORD start_time, DWORD end_time,
                     DWORD interval, char *tag_name, DWORD var_index,
                     DWORD * time_buffer, DWORD * tbsize,
                     void *data_buffer, DWORD * dbsize, DWORD * type, DWORD * n);
   INT EXPRT hs_dump(DWORD event_id, DWORD start_time, DWORD end_time,
                     DWORD interval, BOOL binary_time);
   INT EXPRT hs_fdump(char *file_name, DWORD id, BOOL binary_time);

   /*---- ELog functions ----*/
   INT EXPRT el_retrieve(char *tag, char *date, int *run, char *author,
                         char *type, char *system, char *subject,
                         char *text, int *textsize, char *orig_tag,
                         char *reply_tag, char *attachment1,
                         char *attachment2, char *attachment3, char *encoding);
   INT EXPRT el_submit(int run, char *author, char *type, char *system,
                       char *subject, char *text, char *reply_to,
                       char *encoding, char *afilename1, char *buffer1,
                       INT buffer_size1, char *afilename2, char *buffer2,
                       INT buffer_size2, char *afilename3, char *buffer3,
                       INT buffer_size3, char *tag, INT tag_size);
   INT EXPRT el_search_message(char *tag, int *fh, BOOL walk);
   INT EXPRT el_search_run(int run, char *return_tag);
   INT EXPRT el_delete_message(char *tag);

   /*---- alarm functions ----*/
   INT EXPRT al_check();
   INT EXPRT al_trigger_alarm(char *alarm_name, char *alarm_message,
                              char *default_class, char *cond_str, INT type);
   INT EXPRT al_trigger_class(char *alarm_class, char *alarm_message, BOOL first);
   INT EXPRT al_reset_alarm(char *alarm_name);
   BOOL EXPRT al_evaluate_condition(char *condition, char *value);

   /*---- frontend functions ----*/
   INT get_frontend_index();

   /*---- analyzer functions ----*/
   void EXPRT test_register(ANA_TEST * t);
   void EXPRT add_data_dir(char *result, char *file);
   void EXPRT lock_histo(INT id);

   void EXPRT open_subfolder(char *name);
   void EXPRT close_subfolder();
#ifdef __cplusplus
}

#ifdef USE_ROOT
   /* root functions really are C++ functions */
   extern TFolder *gManaHistosFolder;
   extern TObjArray *gHistoFolderStack;

   // book functions put a root object in a suitable folder
   // for histos, there are a lot of types, so we use templates.
   // for other objects we have one function per object
   template<typename TH1X>
   TH1X EXPRT *h1_book(const char *name, const char *title,
		       int bins, double min, double max)
   {
      TH1X *hist;

      /* check if histo already exists */
      if (!gHistoFolderStack->Last())
         hist = (TH1X *) gManaHistosFolder->FindObjectAny(name);
      else
         hist = (TH1X *) ((TFolder *)gHistoFolderStack->Last())->FindObjectAny(name);

      if (hist == NULL) {
         hist = new TH1X(name, title, bins, min, max);
         if (!gHistoFolderStack->Last())
            gManaHistosFolder->Add(hist);
         else
            ((TFolder *)gHistoFolderStack->Last())->Add(hist);
      }

      return hist;
   }

   template<typename TH1X>
   TH1X EXPRT *h1_book(const char *name, const char *title,
		       int bins, double edges[])
   {
      TH1X *hist;

      /* check if histo already exists */
      if (!gHistoFolderStack->Last())
         hist = (TH1X *) gManaHistosFolder->FindObjectAny(name);
      else
         hist = (TH1X *) ((TFolder *)gHistoFolderStack->Last())->FindObjectAny(name);

      if (hist == NULL) {
         hist = new TH1X(name, title, bins, edges);
         if (!gHistoFolderStack->Last())
            gManaHistosFolder->Add(hist);
         else
            ((TFolder *)gHistoFolderStack->Last())->Add(hist);
      }

      return hist;
   }

   template<typename TH2X>
   TH2X EXPRT *h2_book(const char *name, const char *title,
	     	       int xbins, double xmin, double xmax,
                       int ybins, double ymin, double ymax)
   {
      TH2X *hist;

      /* check if histo already exists */
      if (!gHistoFolderStack->Last())
         hist = (TH2X *) gManaHistosFolder->FindObjectAny(name);
      else
         hist = (TH2X *) ((TFolder *)gHistoFolderStack->Last())->FindObjectAny(name);

      if (hist == NULL) {
         hist = new TH2X(name, title, xbins, xmin, xmax, ybins, ymin, ymax);
         if (!gHistoFolderStack->Last())
            gManaHistosFolder->Add(hist);
         else
            ((TFolder *)gHistoFolderStack->Last())->Add(hist);
      }

      return hist;
   }

   template<typename TH2X>
   TH2X EXPRT *h2_book(const char *name, const char *title,
	  	       int xbins, double xmin, double xmax,
                       int ybins, double yedges[])
   {
      TH2X *hist;

      /* check if histo already exists */
      if (!gHistoFolderStack->Last())
         hist = (TH2X *) gManaHistosFolder->FindObjectAny(name);
      else
         hist = (TH2X *) ((TFolder *)gHistoFolderStack->Last())->FindObjectAny(name);

      if (hist == NULL) {
         hist = new TH2X(name, title, xbins, xmin, xmax, ybins, yedges);
         if (!gHistoFolderStack->Last())
            gManaHistosFolder->Add(hist);
         else
            ((TFolder *)gHistoFolderStack->Last())->Add(hist);
      }

      return hist;
   }

   template<typename TH2X>
   TH2X EXPRT *h2_book(const char *name, const char *title,
		       int xbins, double xedges[],
                       int ybins, double ymin, double ymax)
   {
      TH2X *hist;

      /* check if histo already exists */
      if (!gHistoFolderStack->Last())
         hist = (TH2X *) gManaHistosFolder->FindObjectAny(name);
      else
         hist = (TH2X *) ((TFolder *)gHistoFolderStack->Last())->FindObjectAny(name);

      if (hist == NULL) {
         hist = new TH2X(name, title, xbins, xedges, ybins, ymin, ymax);
         if (!gHistoFolderStack->Last())
            gManaHistosFolder->Add(hist);
         else
            ((TFolder *)gHistoFolderStack->Last())->Add(hist);
      }

      return hist;
   }

   template<typename TH2X>
   TH2X EXPRT *h2_book(const char *name, const char *title,
		       int xbins, double xedges[],
                       int ybins, double yedges[])
   {
      TH2X *hist;

      /* check if histo already exists */
      if (!gHistoFolderStack->Last())
         hist = (TH2X *) gManaHistosFolder->FindObjectAny(name);
      else
         hist = (TH2X *) ((TFolder *)gHistoFolderStack->Last())->FindObjectAny(name);

      if (hist == NULL) {
         hist = new TH2X(name, title, xbins, xedges, ybins, yedges);
         if (!gHistoFolderStack->Last())
            gManaHistosFolder->Add(hist);
         else
            ((TFolder *)gHistoFolderStack->Last())->Add(hist);
      }

      return hist;
   }

   /*
    * the following two macros allow for simple fortran like usage
    * for the most common histo types
    */
   #define H1_BOOK(n,t,b,min,max) (h1_book<TH1F>(n,t,b,min,max))
   #define H2_BOOK(n,t,xb,xmin,xmax,yb,ymin,ymax) (h2_book<TH2F>(n,t,xb,xmin,xmax,yb,ymin,ymax))
#endif /* USE_ROOT */

#endif
#endif                          /* _MIDAS_H */
/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/**dox***************************************************************/
/** @} */ /* end of midasincludecode */
