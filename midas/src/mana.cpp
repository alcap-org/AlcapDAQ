/********************************************************************\

  Name:         mana.c
  Created by:   Stefan Ritt

  Contents:     The system part of the MIDAS analyzer. Has to be
                linked with analyze.c to form a complete analyzer

  $Log: mana.cpp,v $
  Revision 1.3  2011-06-04 04:33:01  mhmurray
  MM: resolved an ambiguity in calls to TTree::Branch() that is not resolved by recent versions of gcc. I used static_cast<> to enforce proper typing

  Revision 1.2  2011-03-26 09:09:32  mhmurray
  MM: Added support for a directory structure in the mu histogram output file. This required adding the function WriteObjectList which recurses on directories that it finds while writing all objects to file. See the function CloseRootOutputFile for the implementation.

  Also added a "return NULL" to root_server_loop, which should return a (void*). PPreviously, this function returned nothing explicitly, and the NCSA compiler doesn't want to fill in the blanks and just return NULL.

  Revision 1.1.1.1  2009-01-14 21:21:41  winter
  DAQ software

  Revision 1.2  2008/11/24 09:09:38  l_data
  (Peter Winter) No module name output

  Revision 1.1.1.1  2008/11/14 14:22:16  l_data
  (Peter Winter) Starting version of midas for MuSun

  Revision 1.2  2006/01/26 20:27:17  fegray
  (Fred)
  - Add Steve's gMiasTreeOutputFileName option.
  - Expand size of memory used for event buffer.

  Revision 1.1  2006/01/25 01:34:46  mucap
  (Fred)
  Online analyzer framework used during run 9.  It is basically the run 8
  analyzer updated so that it compiles with the new version of MIDAS.
  (The analyzer that comes with the new version is broken in quite a few ways.)

  Revision 1.3  2005/06/16 23:15:08  fegray
  (Fred)
  Adjust calculation of data_size slightly so that mdump doesn't complain of
  "FIXED format with Midas Header" on skimmed files.

  Revision 1.2  2005/06/15 20:00:33  fegray
  (Fred)
  Modified mana.c to add a "-O <filename>" command line option (capital O)
  to specify a secondary ROOT output file.  This makes it possible to
  write skimmed data to a MIDAS file (with the usual "-o" option) and
  the associated histograms to a ROOT file.

  Revision 1.1  2005/06/14 19:24:01  fegray
  (Slightly) customized version of the MIDAS analyzer framework that is
  needed for mu.

  Revision 1.118  2004/05/07 19:40:11  midas
  Replaced min/max by MIN/MAX macros

  Revision 1.117  2004/02/12 08:21:22  midas
  Print ROOT server port on startup

  Revision 1.116  2004/02/12 08:06:35  midas
  Added -s parameter for ROOT server

  Revision 1.115  2004/01/18 08:25:10  olchansk
  replace PVM with HAVE_PVM to dodge namespace pollution on macosx

  Revision 1.114  2004/01/17 05:35:53  olchansk
  replace #define ALIGN() with ALIGN8() to dodge namespace pollution under macosx
  hide strlcpy() & co #ifdef HAVE_STRLCPY (macosx already has strlcpy())
  correct inconsistent prototype of dbg_malloc() and dbg_calloc()

  Revision 1.113  2004/01/08 08:40:10  midas
  Implemented standard indentation

  Revision 1.112  2003/12/18 17:42:12  midas
  Added 'P' to HROPEN to work with uppercase directories in offline mode

  Revision 1.111  2003/12/17 07:59:50  midas
  Improved previous error display

  Revision 1.110  2003/12/17 07:52:32  midas
  Added error display if experiment not defined

  Revision 1.109  2003/12/12 12:54:35  midas
  Warn about uppercase characters in HBOOK directory names

  Revision 1.108  2003/12/12 12:52:24  midas
  Warn about uppercase characters in HBOOK directory names

  Revision 1.107  2003/12/12 12:32:09  midas
  Fixed HBOOK compiler warnings

  Revision 1.106  2003/12/04 12:51:18  midas
  Fixed problem when output file does not contain a '.'

  Revision 1.105  2003/11/24 08:22:45  midas
  Changed timeouts from INT to DWORD, added ignore_timeout to cm_cleanup, adde '-f' flag to ODBEdit 'cleanup'

  Revision 1.104  2003/11/20 11:29:44  midas
  Implemented db_check_record and use it in most places instead of db_create_record

  Revision 1.103  2003/11/01 23:21:30  olchansk
  per Stephan Ritt's request, remove #error Please defeine either -DHAVE_HBOOK or -DHAVE_ROOT

  Revision 1.102  2003/11/01 01:25:11  olchansk
  abort if cannto read /runinfo/run number
  abort on invalid run numbers
  abort if cannot write /runinfo/run number

  Revision 1.101  2003/10/29 14:37:14  midas
  Don't book NTUPLES for unknown events

  Revision 1.100  2003/10/29 14:33:46  midas
  Test for either HAVE_HBOOK or HAVE_ROOT

  Revision 1.99  2003/10/03 18:57:40  pierre
  fix error msg

  Revision 1.98  2003/05/09 07:40:04  midas
  Added extra parameter to cm_get_environment

  Revision 1.97  2003/05/02 09:03:01  midas
  Fixed buffer overflows by strlcpy()

  Revision 1.96  2003/04/30 12:58:50  midas
  Load HBOOK histos after booking

  Revision 1.95  2003/04/28 11:13:18  midas
  Moved ss_force_single_thread() befor cm_connect_experiment

  Revision 1.94  2003/04/25 14:04:26  midas
  Fixed wrong #ifdef HAVE_ROOT

  Revision 1.93  2003/04/25 13:29:23  midas
  Added ss_force_single_thread()

  Revision 1.92  2003/04/25 11:53:22  midas
  Added ROOT histo server

  Revision 1.91  2003/04/23 15:30:17  midas
  Fixed compiler warnings under g++

  Revision 1.90  2003/04/23 15:08:02  midas
  Added TTree output

  Revision 1.89  2003/04/22 14:58:17  midas
  Made histogram loading from last.root work

  Revision 1.88  2003/04/22 12:07:22  midas
  Made mana.c compile under Visual C++, added TApplication to make select() work

  Revision 1.87  2003/04/21 03:22:59  olchansk
  remove \t
  fix missing calls to CloseRootOutputFile()
  fix call to gManaOutputTree->Fill();

  Revision 1.86  2003/04/20 03:07:12  olchansk
  fix botched #ifdef HAVE_HBOOK around ana_callback()

  Revision 1.85  2003/04/20 02:59:13  olchansk
  merge ROOT code into mana.c
  remove MANA_LITE, replaced with HAVE_HBOOK, HAVE_ROOT
  ROOT histogramming support almost complete,
  ROOT TTree filling is missing
  all ROOT code is untested, but compiles with RH-8.0, GCC-3.2, ROOT v3.05.03

  Revision 1.84  2003/04/07 23:59:41  olchansk
  make mana.c compilable with g++ 3.2
  replace all '\t' with spaces

  Revision 1.83  2003/03/31 08:27:34  midas
  Fixed alignment bug for strings

  Revision 1.82  2002/11/28 09:43:46  midas
  Added '-o OFLN' option to produce PAW shared memory in offline mode

  Revision 1.81  2002/09/17 22:12:10  pierre
  add arg to cm_cleanup

  Revision 1.80  2002/08/13 14:34:18  midas
  Added event header byte swapping for offline analysis on RS6000 system

  Revision 1.79  2002/06/03 06:07:15  midas
  Added extra parameter to ss_daemon_init to keep stdout

  Revision 1.78  2002/05/10 05:21:50  pierre
  add MANA_LITE #ifdef

  Revision 1.77  2002/05/09 01:57:37  midas
  Set 'events to ODB' TRUE by default

  Revision 1.76  2002/05/08 22:15:03  pierre
  add db_get_value arg

  Revision 1.75  2002/05/08 19:54:40  midas
  Added extra parameter to function db_get_value()

  Revision 1.74  2001/11/09 20:27:53  pierre
  Fix replay for YBOS format

  Revision 1.73  2001/01/30 09:13:04  midas
  Correct for increased event size

  Revision 1.72  2001/01/29 15:27:43  midas
  Added some \n for parallel version

  Revision 1.71  2000/11/20 12:18:58  midas
  Merge more than 10 RZ files

  Revision 1.70  2000/11/20 11:26:53  midas
  Added "use tests" in analyzer request

  Revision 1.69  2000/11/17 08:25:45  midas
  Added disable_shm_write flag for Linux cluster applications

  Revision 1.68  2000/11/15 14:02:08  midas
  Some more work on PVM, but not yet finished

  Revision 1.67  2000/11/08 15:03:00  midas
  Fixed bug that elapsed time was zero after EOR event

  Revision 1.66  2000/11/08 14:47:40  midas
  Analyzer now stops with '!' also in -r (multi run) mode

  Revision 1.65  2000/10/30 10:07:24  midas
  Fixed bug that "always true" test was cleared at the BOR

  Revision 1.64  2000/09/18 09:22:23  midas
  Fixed bug which limited output event size to 64k

  Revision 1.63  2000/08/10 07:44:50  midas
  Made PAW global memory name variable under /analyzer/output/global memeory name
  to run more than one online analyzer instance on one machine

  Revision 1.62  2000/06/06 13:41:09  midas
  Added lock_histo to prevent histos from being cleared

  Revision 1.61  2000/06/06 11:22:02  midas
  Added EXT_EVENT_SIZE, partially implemented PVM stuff

  Revision 1.60  2000/05/09 08:42:58  midas
  Call analyzer_init after ODB load to correct record structures

  Revision 1.59  2000/05/09 08:00:58  midas
  Fixed bug with file not found in ma_open

  Revision 1.58  2000/05/05 08:44:09  midas
  Added ybos_get_tid_size

  Revision 1.57  2000/03/23 08:50:37  midas
  Changed comments

  Revision 1.56  2000/03/13 16:37:10  midas
  Add data directory to last.rz if no directory present already in ODB key

  Revision 1.55  2000/03/08 17:39:35  midas
  Added "/analyzer/output/events to odb" flag

  Revision 1.54  2000/03/06 22:39:06  midas
  Moved last.rz to /analyzer/output so that it can be changed, including a path

  Revision 1.53  2000/03/01 23:04:18  midas
  Added correct_num_events

  Revision 1.52  2000/02/15 11:07:50  midas
  Changed GET_xxx to bit flags

  Revision 1.51  2000/02/09 09:32:00  midas
  Speed up analyzer start

  Revision 1.50  2000/02/01 08:26:09  midas
  Added -P (protect) flag

  Revision 1.49  2000/01/21 08:38:47  midas
  Added event size check for analyzer event extension

  Revision 1.48  2000/01/20 12:04:31  midas
  Fixed bug when opening offline files

  Revision 1.47  1999/12/21 09:40:27  midas
  Ajdusted spaces

  Revision 1.46  1999/12/20 22:12:15  pierre
  - moved yb_tid_size[] in ybos.c, declared extern

  Revision 1.45  1999/12/17 00:10:47  pierre
  - Add YBOS support.

  Revision 1.44  1999/11/29 14:04:31  midas
  Fixed bug that with RWNT all events were filled, even if buffer size = 0

  Revision 1.43  1999/11/29 11:27:23  midas
  Made HBOOK record size LREC changable via command line parameter -L

  Revision 1.42  1999/11/29 09:14:45  midas
  Do a automatic cleanup if running offline

  Revision 1.41  1999/11/26 12:04:49  midas
  Added additional error message when creating module parameters in ODB

  Revision 1.40  1999/11/23 15:49:41  midas
  Allocate local event buffers according to MAX_EVENT_SIZE

  Revision 1.39  1999/10/18 14:41:51  midas
  Use /programs/<name>/Watchdog timeout in all programs as timeout value. The
  default value can be submitted by calling cm_connect_experiment1(..., timeout)

  Revision 1.38  1999/10/11 14:59:24  midas
  Moved the daemonizing before the cm_connect_experiment

  Revision 1.37  1999/10/11 14:45:01  midas
  Added note about becoming a daemon

  Revision 1.36  1999/10/11 14:40:04  midas
  Added -D flag (become daemon)

  Revision 1.35  1999/10/08 13:17:11  midas
  Fixed wrongly booked N-tuple #100000 when loading from last.rz

  Revision 1.34  1999/10/06 10:42:40  midas
  Added -l flag not to load histos from last.rz

  Revision 1.33  1999/09/23 12:45:48  midas
  Added 32 bit banks

  Revision 1.32  1999/08/20 14:26:24  midas
  Do last.rz existence checking before loading

  Revision 1.31  1999/08/20 13:31:18  midas
  Analyzer saves and reloads online histos

  Revision 1.30  1999/08/16 08:28:42  midas
  Fixed bug with wrong run number extraction from tape1/run123456.mid

  Revision 1.29  1999/08/13 13:49:14  midas
  Extract current_run_number from BOR event

  Revision 1.28  1999/08/12 14:39:10  midas
  Fixed bug where from serveral input files specified via "-i xxx.mid yyy.mid"
  only the last one was contained in the output file.

  Revision 1.27  1999/08/12 14:09:31  midas
  Changed -f flag from filter ID to "copy original event if analyer accepts it"

  Revision 1.26  1999/07/22 07:04:56  midas
  Only dump histos in online mode

  Revision 1.25  1999/07/21 09:22:01  midas
  Added Ctrl-C handler to cm_connect_experiment and cm_yield

  Revision 1.24  1999/07/15 07:35:25  midas
  Added Ctrl-C handling

  Revision 1.23  1999/07/15 06:51:17  midas
  Don't call ss_getchar in "quiet" mode not to block tty

  Revision 1.22  1999/07/13 08:16:45  midas
  -q flag makes now analyzer really quiet (not to block the terminal in BG)

  Revision 1.21  1999/07/12 11:22:51  midas
  Added tests

  Revision 1.20  1999/07/02 13:40:53  midas
  Write periodic and slow events always to ODB, not only every second

  Revision 1.19  1999/07/02 13:27:26  midas
  Removed *par from write_event_odb (for external use)

  Revision 1.18  1999/06/24 11:59:17  midas
  Fixed filling of N-tuples if RWNT_SIZE equals zero

  Revision 1.17  1999/06/23 09:37:26  midas
  Print time for offline analysis

  Revision 1.16  1999/05/20 07:37:22  midas
  Fixed bug with ss_getchar() when running online

  Revision 1.15  1999/04/22 07:53:37  midas
  Added event size in -verbose output

  Revision 1.14  1999/03/02 10:00:37  midas
  Used ANA_SKIP/CONTINUE for skipping events

  Revision 1.13  1999/02/22 11:55:20  midas
  Fixed bug with rebooking of N-tuples

  Revision 1.12  1999/02/03 16:36:50  midas
  Added -f flag to filter events

  Revision 1.11  1999/02/01 15:47:47  midas
  - removed ASCII read function (only historical for pibeta)
  - analyze_file puts current offline run number into ODB

  Revision 1.10  1999/01/15 12:50:04  midas
  - Set /Analyzer/Bank switches/... to FALSE by default to avoid N-tuple
    overflow when an analyzer is started the first time
  - Assume MIDAS data format on /dev/... input

  Revision 1.9  1999/01/08 15:00:53  midas
  Analyzer does not stop if a file in a range of files is missing

  Revision 1.8  1998/12/16 11:07:41  midas
  - verbose output (via -v flag) for N-Tuple booking
  - error message if histo dump is on when running online

  Revision 1.7  1998/12/10 12:50:47  midas
  Program abort with "!" now works without a return under UNIX

  Revision 1.6  1998/12/10 05:28:18  midas
  Removed convert-only flag, added -v (verbose) flag

  Revision 1.5  1998/10/29 14:37:58  midas
  Fixed problem with '!' for stopping analyzer

  Revision 1.4  1998/10/22 07:11:15  midas
  *** empty log message ***

  Revision 1.3  1998/10/19 16:40:44  midas
  ASCII output of multi-module LRS1877 now possible

  Revision 1.2  1998/10/12 12:19:01  midas
  Added Log tag in header


\********************************************************************/

#include <assert.h>
#include "midas.h"
#include "msystem.h"
#include "hardware.h"
#include "zlib.h"
#include "ybos.h"
#include <stdio.h>

/*------------------------------------------------------------------*/

/* cernlib includes */
#ifdef OS_WINNT
#define VISUAL_CPLUSPLUS
#endif

#ifdef OS_LINUX
#define f2cFortran
#endif

#ifdef HAVE_HBOOK

#include <cfortran.h>
#include <hbook.h>

/* missing in hbook.h */
#ifndef HFNOV
#define HFNOV(A1,A2)  CCALLSFSUB2(HFNOV,hfnov,INT,FLOATV,A1,A2)
#endif

#ifndef HMERGE
#define HMERGE(A1,A2,A3)  CCALLSFSUB3(HMERGE,hmerge,INT,STRINGV,STRING,A1,A2,A3)
#endif

#endif                          /* HAVE_HBOOK */

/*------------------------------------------------------------------*/

#ifdef HAVE_ROOT

#undef abs

#include <assert.h>
#include <TApplication.h>
#include <TKey.h>
#include <TROOT.h>
#include <TH1.h>
#include <TFile.h>
#include <TTree.h>
#include <TLeaf.h>
#include <TSocket.h>
#include <TServerSocket.h>
#include <TMessage.h>
#include <TObjString.h>
#include <TSystem.h>

#ifdef OS_LINUX
#include <TThread.h>
#endif

/* Our own ROOT global objects */

TApplication *manaApp;
TDirectory *gManaHistsDir = NULL;       // Container for all histograms
TFile *gManaOutputFile = NULL;  // MIDAS output file
char *gMiasTreeOutputFileName = NULL;  // MIAS ROOT Tree output file

/* MIDAS output tree structure holing one tree per event type */

typedef struct {
   int event_id;
   TTree *tree;
   int n_branch;
   char *branch_name;
   int *branch_filled;
   int *branch_len;
   TBranch **branch;
} EVENT_TREE;

typedef struct {
   TFile *f;
   int n_tree;
   EVENT_TREE *event_tree;
} TREE_STRUCT;

TREE_STRUCT tree_struct;

#endif                          /* HAVE_ROOT */

/*------------------------------------------------------------------*/

/* PVM include */

#ifdef HAVE_PVM

INT pvm_start_time = 0;

extern BOOL disable_shm_write;

void pvm_debug(char *format, ...)
{
   va_list argptr;
   char msg[256], str[256];

   if (pvm_start_time == 0)
      pvm_start_time = ss_millitime();

   va_start(argptr, format);
   vsprintf(msg, (char *) format, argptr);
   va_end(argptr);
   sprintf(str, "%1.3lf:  %s", (ss_millitime() - pvm_start_time) / 1000.0, msg);
   puts(str);
#ifdef OS_LINUX
   {
      char cmd[256];

      sprintf(cmd, "echo > /dev/console \"%s\"", str);
      system(cmd);
   }
#endif
}

#undef STRICT
#include <pvm3.h>
#endif

/*----- PVM TAGS and data ------------------------------------------*/

/* buffer size must be larger than largest event (ODB dump!) */
#define PVM_BUFFER_SIZE (1024*1024)

#ifdef HAVE_PVM
#define TAG_DATA  1
#define TAG_BOR   2
#define TAG_EOR   3
#define TAG_EXIT  4
#define TAG_INIT  5

int pvm_n_task;
int pvm_myparent;
int pvm_client_index;

typedef struct {
   int tid;
   char host[80];
   char *buffer;
   int wp;
   DWORD n_events;
   DWORD time;
   BOOL eor_sent;
} PVM_CLIENT;

PVM_CLIENT *pvmc;

int pvm_eor(int);
int pvm_merge(void);
void pvm_debug(char *format, ...);
int pvm_distribute(ANALYZE_REQUEST * par, EVENT_HEADER * pevent);

//#define PVM_DEBUG pvm_debug
#define PVM_DEBUG

#endif                          /* PVM */

BOOL pvm_master = FALSE, pvm_slave = FALSE;

char *bstr = " ";

/*------------------------------------------------------------------*/

/* items defined in analyzer.c */
extern char *analyzer_name;
extern INT analyzer_loop_period;
extern INT analyzer_init(void);
extern INT analyzer_exit(void);
extern INT analyzer_loop(void);
extern INT ana_begin_of_run(INT run_number, char *error);
extern INT ana_end_of_run(INT run_number, char *error);
extern INT ana_pause_run(INT run_number, char *error);
extern INT ana_resume_run(INT run_number, char *error);
extern INT odb_size;

/*---- globals -----------------------------------------------------*/

/* ODB handle */
HNDLE hDB;

/* run number */
DWORD current_run_number;

/* analyze_request defined in analyze.c or anasys.c */
extern ANALYZE_REQUEST analyze_request[];

/* N-tupel booking and filling flag */
BOOL ntuple_flag;

/* list of locked histos (won't get cleared) */
INT lock_list[10000];

/* flag to indicate whether the queue is being cleared after a stop request */
BOOL now_stopping = FALSE;

#ifdef extname
int quest_[100];
#else
extern int QUEST[100];
#endif

extern INT pawc_size;

/* default HBOOK record size */
#define HBOOK_LREC 8190

/* maximum extended event size */
#ifndef EXT_EVENT_SIZE
#define EXT_EVENT_SIZE (5*(MAX_EVENT_SIZE+sizeof(EVENT_HEADER)))
#endif

/* command line parameters */
struct {
   INT online;
   char host_name[HOST_NAME_LENGTH];
   char exp_name[NAME_LENGTH];
   char input_file_name[10][256];
   char output_file_name[256];
   char secondary_output_file_name[256];
   char mias_tree_output_file_name[256];
   INT run_number[2];
   DWORD n[4];
   BOOL filter;
   BOOL graphics;
   char config_file_name[10][256];
   char param[10][256];
   char protect[10][256];
   BOOL rwnt;
   INT lrec;
   BOOL debug;
   BOOL verbose;
   BOOL quiet;
   BOOL no_load;
   BOOL daemon;
   INT n_task;
   INT pvm_buf_size;
   INT root_port;
} clp;

struct {
   char flag_char;
   char description[1000];
   void *data;
   INT type;
   INT n;
   INT index;
} clp_descrip[] = {

   {
   'c', "<filename1>   Configuration file name(s). May contain a '%05d' to be\n\
     <filename2>   replaced by the run number. Up to ten files can be\n\
         ...       specified in one \"-c\" statement", clp.config_file_name, TID_STRING, 10}, {
   'd', "              Debug flag when started the analyzer fron a debugger.\n\
                   Prevents the system to kill the analyzer when the\n\
                   debugger stops at a breakpoint", &clp.debug, TID_BOOL, 0}, {
   'D', "              Start analyzer as a daemon in the background (UNIX only).",
          &clp.daemon, TID_BOOL, 0}, {
   'e', "<experiment>  MIDAS experiment to connect to", clp.exp_name, TID_STRING, 1}, {
   'f', "              Filter mode. Write original events to output file\n\
                   only if analyzer accepts them (doesn't return ANA_SKIP).\n", &clp.filter, TID_BOOL, 0}, {
   'g', "              Graphics mode. Runs ROOT in non-batch mode.\n", &clp.graphics, TID_BOOL, 0}, {
   'h', "<hostname>    MIDAS host to connect to when running the analyzer online",
          clp.host_name, TID_STRING, 1}, {
   'i', "<filename1>   Input file name. May contain a '%05d' to be replaced by\n\
     <filename2>   the run number. Up to ten input files can be specified\n\
         ...       in one \"-i\" statement", clp.input_file_name, TID_STRING, 10}, {
   'l', "              If set, don't load histos from last histo file when\n\
                   running online.", &clp.no_load, TID_BOOL, 0}, {
   'L', "              HBOOK LREC size. Default is 8190.", &clp.lrec, TID_INT, 0}, {
   'n', "<count>       Analyze only \"count\" events.\n\
     <first> <last>\n\
                   Analyze only events from \"first\" to \"last\".\n\
     <first> <last> <n>\n\
                   Analyze every n-th event from \"first\" to \"last\".", clp.n, TID_INT, 4}, {
   'o', "<filename>    Output file name. Extension may be .mid (MIDAS binary),\n\
                   .asc (ASCII) or .rz (HBOOK). If the name contains a '%05d',\n\
                   one output file is generated for each run. Use \"OFLN\" as\n\
                   output file name to creaate a HBOOK shared memory instead\n\
                   of a file.", clp.output_file_name, TID_STRING, 1}, 
#ifdef HAVE_ROOT
  {'O', "<filename>    Secondary output file name, which can be used to generate\n\
                   a .root histogram file in addition to the main output file.",
                   clp.secondary_output_file_name, TID_STRING, 1}, 
  {'T', "<filename>    MIAS Tree output file name, which can be used to generate\n\
                   a .root Tree file in addition to the main output file.",
                   clp.mias_tree_output_file_name, TID_STRING, 1}, 
#endif
  {'p', "<param=value> Set individual parameters to a specific value.\n\
                   Overrides any setting in configuration files", clp.param, TID_STRING, 10}, {
   'P', "<ODB tree>    Protect an ODB subtree from being overwritten\n\
                   with the online data when ODB gets loaded from .mid file", clp.protect, TID_STRING, 10}, {
   'q', "              Quiet flag. If set, don't display run progress in\n\
                   offline mode.", &clp.quiet, TID_BOOL, 0}, {
   'r', "<range>       Range of run numbers to analyzer like \"-r 120 125\"\n\
                   to analyze runs 120 to 125 (inclusive). The \"-r\"\n\
                   flag must be used with a '%05d' in the input file name.", clp.run_number, TID_INT, 2},
#ifdef HAVE_PVM
   {
   't', "<n>           Parallelize analyzer using <n> tasks with PVM.",
          &clp.n_task, TID_INT, 1}, {
   'b', "<n>           Buffer size for parallelization in kB.",
          &clp.pvm_buf_size, TID_INT, 1},
#endif

#ifdef HAVE_ROOT
   {
   's', "<port>        Start ROOT histo server under <port>. If port==0, don't start server.",
          &clp.root_port, TID_INT, 1},
#endif
   
   {
   'v', "              Verbose output.", &clp.verbose, TID_BOOL, 0}, {
   'w', "              Produce row-wise N-tuples in outpur .rz file. By\n\
                   default, column-wise N-tuples are used.", &clp.rwnt, TID_BOOL, 0}, {
   0}
};

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

FILE *out_file;
BOOL out_gzip;
INT out_format;
BOOL out_append;

void update_stats();
void odb_load(EVENT_HEADER * pevent);

/*---- ODB records -------------------------------------------------*/

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
Events to ODB = BOOL : 1\n\
Global Memory Name = STRING : [8] ONLN\n\
"

/*-- interprete command line parameters ----------------------------*/

INT getparam(int argc, char **argv)
{
   INT index, i, j, size;

   /* parse command line parameters */
   for (index = 1; index < argc;) {
      /* search flag in parameter description */
      if (argv[index][0] == '-') {
         for (j = 0; clp_descrip[j].flag_char; j++)
            if (argv[index][1] == clp_descrip[j].flag_char)
               break;

         if (!clp_descrip[j].flag_char)
            goto usage;

         if (clp_descrip[j].n > 0 && index >= argc - 1)
            goto usage;
         index++;

         if (clp_descrip[j].type == TID_BOOL) {
            *((BOOL *) clp_descrip[j].data) = TRUE;
            continue;
         }

         do {
            if (clp_descrip[j].type == TID_STRING)
               strcpy((char *) clp_descrip[j].data + clp_descrip[j].index * 256,
                      argv[index]);
            else
               db_sscanf(argv[index], clp_descrip[j].data,
                         &size, clp_descrip[j].index, clp_descrip[j].type);

            if (clp_descrip[j].n > 1)
               clp_descrip[j].index++;

            if (clp_descrip[j].index > clp_descrip[j].n) {
               printf("Note more than %d options possible for flag -%c\n",
                      clp_descrip[j].n, clp_descrip[j].flag_char);
               return 0;
            }

            index++;

         } while (index < argc && argv[index][0] != '-');

      } else
         goto usage;
   }

   return SUCCESS;

 usage:

   printf("usage: analyzer [options]\n\n");
   printf("valid options are:\n");
   for (i = 0; clp_descrip[i].flag_char; i++)
      printf("  -%c %s\n", clp_descrip[i].flag_char, clp_descrip[i].description);

   return 0;
}

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

/*-- db_get_event_definition ---------------------------------------*/

typedef struct {
   short int event_id;
   int type;
   WORD format;
   HNDLE hDefKey;
   BOOL disabled;
} EVENT_DEF;

EVENT_DEF *db_get_event_definition(short int event_id)
{
   INT i, index, status, size, type;
   char str[80];
   HNDLE hKey, hKeyRoot;
   WORD id;

#define EVENT_DEF_CACHE_SIZE 30
   static EVENT_DEF *event_def = NULL;

   /* allocate memory for cache */
   if (event_def == NULL)
      event_def = (EVENT_DEF *) calloc(EVENT_DEF_CACHE_SIZE, sizeof(EVENT_DEF));

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
      event_def[index].disabled = FALSE;
      return &event_def[index];
   }

   status = db_find_key(hDB, 0, "/equipment", &hKeyRoot);
   if (status != DB_SUCCESS) {
      cm_msg(MERROR, "db_get_event_definition", "cannot find /equipment entry in ODB");
      return NULL;
   }

   for (i = 0;; i++) {
      /* search for equipment with specific name */
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

      size = sizeof(type);
      status = db_get_value(hDB, hKey, "Common/Type", &type, &size, TID_INT, TRUE);
      if (status != DB_SUCCESS)
         continue;

      if (id == event_id) {
         /* set cache entry */
         event_def[index].event_id = id;
         event_def[index].type = type;

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
      tl = (ANA_TEST **) malloc(2 * sizeof(void *));

      /* define "always true" test */
      tl[0] = (ANA_TEST *) malloc(sizeof(ANA_TEST));
      strcpy(tl[0]->name, "Always true");
      tl[0]->count = 0;
      tl[0]->value = TRUE;
      tl[0]->registered = TRUE;
      n_test++;
   } else
      tl = (ANA_TEST **) realloc(tl, (n_test + 1) * sizeof(void *));

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

/*-- load parameters specified on command line ---------------------*/

INT load_parameters(INT run_number)
{
   INT i, size, index, status;
   HNDLE hkey;
   char file_name[256], str[80], value_string[80], param_string[80];
   char data[32];
   KEY key;

   /* loop over configutation file names */
   for (i = 0; clp.config_file_name[i][0] && i < 10; i++) {
      if (strchr(clp.config_file_name[i], '%') != NULL)
         sprintf(file_name, clp.config_file_name[i], run_number);
      else
         strcpy(file_name, clp.config_file_name[i]);

      /* load file under "/" */
      if (db_load(hDB, 0, file_name, FALSE) == DB_SUCCESS)
         printf("Configuration file \"%s\" loaded\n", file_name);
   }

   /* loop over parameters */
   for (i = 0; clp.param[i][0] && i < 10; i++) {
      if (strchr(clp.param[i], '=') == NULL) {
         printf("Error: parameter %s contains no value\n", clp.param[i]);
      } else {
         strcpy(value_string, strchr(clp.param[i], '=') + 1);
         strcpy(param_string, clp.param[i]);
         *strchr(param_string, '=') = 0;

         index = 0;
         if (strchr(param_string, '[') != NULL) {
            index = atoi(strchr(param_string, '[') + 1);
            *strchr(param_string, '[') = 0;
         }

         if (param_string[0] == '/')
            strcpy(str, param_string);
         else
            sprintf(str, "/%s/Parameters/%s", analyzer_name, param_string);
         db_find_key(hDB, 0, str, &hkey);
         if (hkey == 0) {
            printf("Error: cannot find parameter %s in ODB\n", str);
         } else {
            db_get_key(hDB, hkey, &key);
            db_sscanf(value_string, data, &size, 0, key.type);

            status = db_set_data_index(hDB, hkey, data, size, index, key.type);
            if (status == DB_SUCCESS)
               printf("Parameter %s changed to %s\n", str, value_string);
            else
               printf("Cannot change parameter %s\n", str);
         }
      }
   }

   /* let parameter changes propagate to modules */
   cm_yield(0);

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

INT book_ntuples(void);
INT book_ttree(void);

void banks_changed(INT hDB, INT hKey, void *info)
{
   char str[80];
   HNDLE hkey;

   /* close previously opened hot link */
   sprintf(str, "/%s/Bank switches", analyzer_name);
   db_find_key(hDB, 0, str, &hkey);
   db_close_record(hDB, hkey);

#ifdef HAVE_HBOOK
   book_ntuples();
   printf("N-tuples rebooked\n");
#endif
#ifdef HAVE_ROOT
   book_ttree();
   printf("ROOT TTree rebooked\n");
#endif
}

#ifdef HAVE_HBOOK
INT book_ntuples(void)
{
   INT index, i, j, status, n_tag, size, id;
   HNDLE hkey;
   KEY key;
   char ch_tags[2000];
   char rw_tag[512][8];
   char str[80], str2[80], key_name[NAME_LENGTH], block_name[NAME_LENGTH];
   BANK_LIST *bank_list;
   EVENT_DEF *event_def;

   /* check global N-tuple flag */
   ntuple_flag = 1;
   size = sizeof(ntuple_flag);
   sprintf(str, "/%s/Book N-tuples", analyzer_name);
   db_get_value(hDB, 0, str, &ntuple_flag, &size, TID_BOOL, TRUE);

   if (!ntuple_flag)
      return SUCCESS;

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

   if (!clp.rwnt) {
      /* book CW N-tuples */

      /* go through all analyzer requests (events) */
      for (index = 0; analyze_request[index].event_name[0]; index++) {
         /* book N-tuple with evend ID */
         HBNT(analyze_request[index].ar_info.event_id, analyze_request[index].event_name,
              bstr);

         /* book run number/event number/time */
         strcpy(str, "Number");
         strcpy(str2, "Run:U*4,Number:U*4,Time:U*4");
         HBNAME(analyze_request[index].ar_info.event_id, str,
                (int *) &analyze_request[index].number, str2);

         bank_list = analyze_request[index].bank_list;
         if (bank_list == NULL) {
            /* book fixed event */
            event_def =
                db_get_event_definition((short int) analyze_request[index].ar_info.
                                        event_id);
            if (event_def == NULL) {
               cm_msg(MERROR, "book_ntuples", "Cannot find definition of event %s in ODB",
                      analyze_request[index].event_name);
               return 0;
            }

            ch_tags[0] = 0;
            for (i = 0;; i++) {
               status = db_enum_key(hDB, event_def->hDefKey, i, &hkey);
               if (status == DB_NO_MORE_SUBKEYS)
                  break;

               db_get_key(hDB, hkey, &key);

               /* convert blanks etc to '_' */
               strcpy(str, key.name);
               for (j = 0; str[j]; j++) {
                  if (!(str[j] >= 'a' && str[j] <= 'z') &&
                      !(str[j] >= 'A' && str[j] <= 'Z') &&
                      !(str[j] >= '0' && str[j] <= '9'))
                     str[j] = '_';
               }
               strcat(ch_tags, str);
               str[0] = 0;

               if (key.num_values > 1)
                  sprintf(str, "(%d)", key.num_values);

               if (hbook_types[key.type] != NULL)
                  strcat(str, hbook_types[key.type]);
               else {
                  cm_msg(MERROR, "book_ntuples",
                         "Key %s in event %s is of type %s with no HBOOK correspondence",
                         key.name, analyze_request[index].event_name,
                         rpc_tid_name(key.type));
                  return 0;
               }
               strcat(ch_tags, str);
               strcat(ch_tags, ",");
            }

            ch_tags[strlen(ch_tags) - 1] = 0;
            db_get_record_size(hDB, event_def->hDefKey, 0, &size);
            analyze_request[index].addr = calloc(1, size);

            strcpy(block_name, analyze_request[index].event_name);
            block_name[8] = 0;

            HBNAME(analyze_request[index].ar_info.event_id, block_name,
                   analyze_request[index].addr, ch_tags);
         } else {
            /* go thorough all banks in bank_list */
            for (; bank_list->name[0]; bank_list++) {
               if (bank_list->output_flag == 0)
                  continue;

               if (bank_list->type != TID_STRUCT) {
                  sprintf(str, "N%s[0,%ld]", bank_list->name, bank_list->size);
                  HBNAME(analyze_request[index].ar_info.event_id,
                         bank_list->name, (INT *) & bank_list->n_data, str);

                  sprintf(str, "%s(N%s)", bank_list->name, bank_list->name);

                  /* define variable length array */
                  if (hbook_types[bank_list->type] != NULL)
                     strcat(str, hbook_types[bank_list->type]);
                  else {
                     cm_msg(MERROR, "book_ntuples",
                            "Bank %s is of type %s with no HBOOK correspondence",
                            bank_list->name, rpc_tid_name(bank_list->type));
                     return 0;
                  }

                  if (rpc_tid_size(bank_list->type) == 0) {
                     cm_msg(MERROR, "book_ntuples",
                            "Bank %s is of type with unknown size", bank_list->name);
                     return 0;
                  }

                  bank_list->addr =
                      calloc(bank_list->size, MAX(4, rpc_tid_size(bank_list->type)));

                  HBNAME(analyze_request[index].ar_info.event_id,
                         bank_list->name, bank_list->addr, str);
               } else {
                  /* define structured bank */
                  ch_tags[0] = 0;
                  for (i = 0;; i++) {
                     status = db_enum_key(hDB, bank_list->def_key, i, &hkey);
                     if (status == DB_NO_MORE_SUBKEYS)
                        break;

                     db_get_key(hDB, hkey, &key);

                     /* convert blanks etc to '_' */
                     strcpy(str, key.name);
                     for (j = 0; str[j]; j++) {
                        if (!(str[j] >= 'a' && str[j] <= 'z') &&
                            !(str[j] >= 'A' && str[j] <= 'Z') &&
                            !(str[j] >= '0' && str[j] <= '9'))
                           str[j] = '_';
                     }
                     strcat(ch_tags, str);
                     str[0] = 0;

                     if (key.num_values > 1)
                        sprintf(str, "(%d)", key.num_values);

                     if (hbook_types[key.type] != NULL)
                        strcat(str, hbook_types[key.type]);
                     else {
                        cm_msg(MERROR, "book_ntuples",
                               "Key %s in bank %s is of type %s with no HBOOK correspondence",
                               key.name, bank_list->name, rpc_tid_name(key.type));
                        return 0;
                     }
                     strcat(ch_tags, str);
                     strcat(ch_tags, ",");
                  }

                  ch_tags[strlen(ch_tags) - 1] = 0;
                  bank_list->addr = calloc(1, bank_list->size);

                  HBNAME(analyze_request[index].ar_info.event_id,
                         bank_list->name, bank_list->addr, ch_tags);
               }
            }
         }

         /* HPRNT(analyze_request[index].ar_info.event_id); */
      }
   } else {
      /* book RW N-tuples */

      /* go through all analyzer requests (events) */
      for (index = 0; analyze_request[index].event_name[0]; index++) {
         /* get pointer to event definition */
         event_def =
             db_get_event_definition((short int) analyze_request[index].ar_info.event_id);

         /* skip if not found */
         if (!event_def)
            continue;

         /* don't book NT if not requested */
         if (analyze_request[index].rwnt_buffer_size == 0) {
            event_def->disabled = TRUE;
            continue;
         }

         n_tag = 0;

         strcpy(rw_tag[n_tag++], "Run");
         strcpy(rw_tag[n_tag++], "Number");
         strcpy(rw_tag[n_tag++], "Time");

         if (clp.verbose) {
            printf("NT #%d-1: Run\n", analyze_request[index].ar_info.event_id);
            printf("NT #%d-2: Number\n", analyze_request[index].ar_info.event_id);
            printf("NT #%d-3: Time\n", analyze_request[index].ar_info.event_id);
         }

         bank_list = analyze_request[index].bank_list;
         if (bank_list == NULL) {
            /* book fixed event */

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

                     if (clp.verbose)
                        printf("NT #%d-%d: %s\n", analyze_request[index].ar_info.event_id,
                               n_tag + 1, str);

                     if (n_tag >= 512) {
                        cm_msg(MERROR, "book_ntuples",
                               "Too much tags for RW N-tupeles (512 maximum)");
                        return 0;
                     }
               } else {
                  strncpy(rw_tag[n_tag++], key_name, 8);

                  if (clp.verbose)
                     printf("NT #%d-%d: %s\n", analyze_request[index].ar_info.event_id,
                            n_tag, key_name);
               }

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

                     if (clp.verbose)
                        printf("NT #%d-%d: %s\n", analyze_request[index].ar_info.event_id,
                               n_tag, str);
                     if (n_tag >= 512) {
                        cm_msg(MERROR, "book_ntuples",
                               "Too much tags for RW N-tupeles (512 maximum)");
                        return 0;
                     }
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

                           if (clp.verbose)
                              printf("NT #%d-%d: %s\n",
                                     analyze_request[index].ar_info.event_id, n_tag, str);

                           if (n_tag >= 512) {
                              cm_msg(MERROR, "book_ntuples",
                                     "Too much tags for RW N-tupeles (512 maximum)");
                              return 0;
                           }
                     } else {
                        strncpy(rw_tag[n_tag++], key_name, 8);
                        if (clp.verbose)
                           printf("NT #%d-%d: %s\n",
                                  analyze_request[index].ar_info.event_id, n_tag,
                                  key_name);
                     }

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

         id = analyze_request[index].ar_info.event_id;
         if (HEXIST(id))
            HDELET(id);

         if (clp.online || equal_ustring(clp.output_file_name, "OFLN"))
            HBOOKN(id, block_name, n_tag, bstr,
                   n_tag * analyze_request[index].rwnt_buffer_size, rw_tag);
         else {
            strcpy(str, "//OFFLINE");
            HBOOKN(id, block_name, n_tag, str, 5120, rw_tag);
         }

         if (!HEXIST(id)) {
            printf("\n");
            cm_msg(MINFO, "book_ntuples",
                   "Cannot book N-tuple #%d. Increase PAWC size via the -s flag or switch off banks",
                   id);
         }
      }
   }

   return SUCCESS;
}
#endif                          /* HAVE_HBOOK */

/*-- book TTree from ODB bank structures ---------------------------*/

#ifdef HAVE_ROOT

char ttree_types[][8] = {
   "",
   "b",                         /* TID_BYTE      */
   "B",                         /* TID_SBYTE     */
   "b",                         /* TID_CHAR      */
   "s",                         /* TID_WORD      */
   "S",                         /* TID_SHORT     */
   "i",                         /* TID_DWORD     */
   "I",                         /* TID_INT       */
   "I",                         /* TID_BOOL      */
   "F",                         /* TID_FLOAT     */
   "D",                         /* TID_DOUBLE    */
   "b",                         /* TID_BITFIELD  */
   "C",                         /* TID_STRING    */
   "",                          /* TID_ARRAY     */
   "",                          /* TID_STRUCT    */
   "",                          /* TID_KEY       */
   "",                          /* TID_LINK      */
   "",                          /* TID_LAST      */

};

INT book_ttree()
{
   INT index, i, status, size;
   HNDLE hkey;
   KEY key;
   char leaf_tags[2000];
   char str[80];
   BANK_LIST *bank_list;
   EVENT_DEF *event_def;
   EVENT_TREE *et;

   /* check global N-tuple flag */
   ntuple_flag = 1;
   size = sizeof(ntuple_flag);
   sprintf(str, "/%s/Book TTree", analyzer_name);
   db_get_value(hDB, 0, str, &ntuple_flag, &size, TID_BOOL, TRUE);

   if (!ntuple_flag)
      return SUCCESS;

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

   /* go through all analyzer requests (events) */
   for (index = 0; analyze_request[index].event_name[0]; index++) {
      /* create tree */
      tree_struct.n_tree++;
      if (tree_struct.n_tree == 1)
         tree_struct.event_tree = (EVENT_TREE *) malloc(sizeof(EVENT_TREE));
      else
         tree_struct.event_tree =
             (EVENT_TREE *) realloc(tree_struct.event_tree,
                                    sizeof(EVENT_TREE) * tree_struct.n_tree);

      et = tree_struct.event_tree + (tree_struct.n_tree - 1);

      et->event_id = analyze_request[index].ar_info.event_id;
      et->n_branch = 0;

      /* create tree */
      sprintf(str, "Event \"%s\", ID %d", analyze_request[index].event_name,
              et->event_id);
      et->tree = new TTree(analyze_request[index].event_name, str);

      /* book run number/event number/time */
      et->branch = (TBranch **) malloc(sizeof(TBranch *));
      et->branch_name = (char *) malloc(NAME_LENGTH);
      et->branch_filled = (int *) malloc(sizeof(int));
      et->branch_len = (int *) malloc(sizeof(int));

      et->branch[et->n_branch] =
          et->tree->Branch("Number", &analyze_request[index].number,
                           "Run/I:Number/I:Time/i");
      strcpy(et->branch_name, "Number");
      et->n_branch++;

      bank_list = analyze_request[index].bank_list;
      if (bank_list == NULL) {
         /* book fixed event */
         event_def =
             db_get_event_definition((short int) analyze_request[index].ar_info.event_id);
         if (event_def == NULL) {
            cm_msg(MERROR, "book_ttree", "Cannot find definition of event %s in ODB",
                   analyze_request[index].event_name);
            return 0;
         }

         leaf_tags[0] = 0;
         for (i = 0;; i++) {
            status = db_enum_key(hDB, event_def->hDefKey, i, &hkey);
            if (status == DB_NO_MORE_SUBKEYS)
               break;

            db_get_key(hDB, hkey, &key);

            strcat(leaf_tags, key.name);

            if (key.num_values > 1)
               sprintf(leaf_tags + strlen(leaf_tags), "[%d]", key.num_values);

            strcat(leaf_tags, "/");

            if (ttree_types[key.type] != NULL)
               strcat(leaf_tags, ttree_types[key.type]);
            else {
               cm_msg(MERROR, "book_ttree",
                      "Key %s in event %s is of type %s with no TTREE correspondence",
                      key.name, analyze_request[index].event_name,
                      rpc_tid_name(key.type));
               return 0;
            }
            strcat(leaf_tags, ":");
         }

         leaf_tags[strlen(leaf_tags) - 1] = 0;  /* delete last ':' */

         et->branch =
             (TBranch **) realloc(et->branch, sizeof(TBranch *) * (et->n_branch + 1));
         et->branch_name =
             (char *) realloc(et->branch_name, NAME_LENGTH * (et->n_branch + 1));
         et->branch_filled =
             (int *) realloc(et->branch_filled, sizeof(int) * (et->n_branch + 1));
         et->branch_len =
             (int *) realloc(et->branch_len, sizeof(int) * (et->n_branch + 1));

         et->branch[et->n_branch] =
             et->tree->Branch(
                static_cast<const char*>(analyze_request[index].event_name), 0, 
                static_cast<const char*>(leaf_tags));
         strcpy(&et->branch_name[et->n_branch * NAME_LENGTH],
                analyze_request[index].event_name);
         et->n_branch++;
      } else {
         /* go thorough all banks in bank_list */
         for (; bank_list->name[0]; bank_list++) {
            if (bank_list->output_flag == 0)
               continue;

            if (bank_list->type != TID_STRUCT) {
               sprintf(leaf_tags, "n%s/I:%s[n%s]/", bank_list->name, bank_list->name,
                       bank_list->name);

               /* define variable length array */
               if (ttree_types[bank_list->type] != NULL)
                  strcat(leaf_tags, ttree_types[bank_list->type]);
               else {
                  cm_msg(MERROR, "book_ttree",
                         "Bank %s is of type %s with no TTREE correspondence",
                         bank_list->name, rpc_tid_name(bank_list->type));
                  return 0;
               }

               if (rpc_tid_size(bank_list->type) == 0) {
                  cm_msg(MERROR, "book_ttree", "Bank %s is of type with unknown size",
                         bank_list->name);
                  return 0;
               }

               et->branch =
                   (TBranch **) realloc(et->branch,
                                        sizeof(TBranch *) * (et->n_branch + 1));
               et->branch_name =
                   (char *) realloc(et->branch_name, NAME_LENGTH * (et->n_branch + 1));
               et->branch_filled =
                   (int *) realloc(et->branch_filled, sizeof(int) * (et->n_branch + 1));
               et->branch_len =
                   (int *) realloc(et->branch_len, sizeof(int) * (et->n_branch + 1));

               et->branch[et->n_branch] =
                   et->tree->Branch(static_cast<const char*>(bank_list->name), 
                                    0, static_cast<const char*>(leaf_tags));
               strcpy(&et->branch_name[et->n_branch * NAME_LENGTH], bank_list->name);
               et->n_branch++;
            } else {
               /* define structured bank */
               leaf_tags[0] = 0;
               for (i = 0;; i++) {
                  status = db_enum_key(hDB, bank_list->def_key, i, &hkey);
                  if (status == DB_NO_MORE_SUBKEYS)
                     break;

                  db_get_key(hDB, hkey, &key);

                  strcat(leaf_tags, key.name);

                  if (key.num_values > 1)
                     sprintf(leaf_tags + strlen(leaf_tags), "[%d]", key.num_values);

                  strcat(leaf_tags, "/");

                  if (ttree_types[key.type] != NULL)
                     strcat(leaf_tags, ttree_types[key.type]);
                  else {
                     cm_msg(MERROR, "book_ttree",
                            "Key %s in bank %s is of type %s with no HBOOK correspondence",
                            key.name, bank_list->name, rpc_tid_name(key.type));
                     return 0;
                  }
                  strcat(leaf_tags, ":");
               }

               leaf_tags[strlen(leaf_tags) - 1] = 0;

               et->branch =
                   (TBranch **) realloc(et->branch,
                                        sizeof(TBranch *) * (et->n_branch + 1));
               et->branch_name =
                   (char *) realloc(et->branch_name, NAME_LENGTH * (et->n_branch + 1));
               et->branch_filled =
                   (int *) realloc(et->branch_filled, sizeof(int) * (et->n_branch + 1));
               et->branch_len =
                   (int *) realloc(et->branch_len, sizeof(int) * (et->n_branch + 1));

               et->branch[et->n_branch] =
                   et->tree->Branch(static_cast<const char*>(bank_list->name), 
                   0, static_cast<const char*>(leaf_tags));
               strcpy(&et->branch_name[et->n_branch * NAME_LENGTH], bank_list->name);
               et->n_branch++;
            }
         }
      }
   }

   return SUCCESS;
}

/*-- root histogram routines ---------------------------------------*/

// Save all objects from given directory into given file
INT SaveRootHistograms(TDirectory * dir, const char *filename)
{
   TDirectory *savedir = gDirectory;
   TFile *outf = new TFile(filename, "RECREATE", "Midas Analyzer Histograms");
   if (outf == 0) {
      cm_msg(MERROR, "SaveRootHistograms", "Cannot create output file %s", filename);
      return 0;
   }

   outf->cd();
   TIter next(dir->GetList());
   while (TObject * obj = next())
      obj->Write();

   outf->Close();
   delete outf;
   // restore current directory
   savedir->cd();
   return SUCCESS;
}

/*------------------------------------------------------------------*/

// Load all objects from given file into given directory
INT LoadRootHistograms(TDirectory * dir, const char *filename)
{
   TDirectory *savedir = gDirectory;
   dir->cd();
   TFile *inf = new TFile(filename, "READ");
   if (inf == NULL)
      printf("Error: File \"%s\" not found", filename);
   else {
      TIter next(inf->GetListOfKeys());

      while (TObject * obj = next()) {
         //if (obj->InheritsFrom("TH1")) // does not work???
         {
            dir->cd();
            inf->Get(obj->GetName())->Clone();
            savedir->cd();
         }
      }
      inf->Close();
      delete inf;
   }
   // restore current directory
   savedir->cd();
   return SUCCESS;
}

/*------------------------------------------------------------------*/

// Clear all TH1 objects in the given directory
INT ClearRootHistograms(TDirectory * dir)
{
   TIter next(dir->GetList());
   while (TObject * obj = next())
      if (obj->InheritsFrom("TH1"))
         ((TH1 *) obj)->Reset();
   return SUCCESS;
}

/*------------------------------------------------------------------*/


// Added by Michael M 26 Mar 2011 to support a directory structure
// for ROOT histograms at mu level.
INT WriteObjectList(TList* list)
{
  TIter next(list);
  while (TObject* obj = next()) {
    // if obj is a directory, recurse.
    if( obj->InheritsFrom( TDirectory::Class() ) ) {
      TDirectory* currentdir = gDirectory; // save to switch back
      TDirectory* newdir = gDirectory->mkdir(((TDirectory*)obj)->GetName(),
                                             ((TDirectory*)obj)->GetName());
      newdir->cd();
      WriteObjectList( ((TDirectory*)obj)->GetList() );
      //switch back to this directory, and continue writing objects
      currentdir->cd();
      continue; // TDirectory::Write() does nothing.
    }
    obj->Write();
  }
  return SUCCESS;
}

/*------------------------------------------------------------------*/

INT CloseRootOutputFile()
{
   int i;

   // ensure that we do have an open file
   assert(gManaOutputFile != NULL);

   // save the histograms
   gManaOutputFile->cd();
   /* 
   TIter next(gManaHistsDir->GetList());
   while (TObject * obj = next())
      obj->Write();
   */ // MM: replaced with WriteObjectList() for directories, below.
  
   WriteObjectList(gManaHistsDir->GetList());

   // close the output file
   gManaOutputFile->Write();
   gManaOutputFile->Close();
   delete gManaOutputFile;
   gManaOutputFile = NULL;

   // delete the output trees
   for (i = 0; i < tree_struct.n_tree; i++)
      if (tree_struct.event_tree[i].branch) {
         free(tree_struct.event_tree[i].branch);
         free(tree_struct.event_tree[i].branch_name);
         free(tree_struct.event_tree[i].branch_filled);
         free(tree_struct.event_tree[i].branch_len);
         tree_struct.event_tree[i].branch = NULL;
      }

   /* delete event tree */
   free(tree_struct.event_tree);
   tree_struct.event_tree = NULL;

   // go to ROOT root directory
   gROOT->cd();

   return SUCCESS;
}

#endif                          /* HAVE_ROOT */

/*-- analyzer init routine -----------------------------------------*/

INT mana_init()
{
   ANA_MODULE **module;
   INT i, j, status, size;
   HNDLE hkey;
   char str[256], block_name[32];
   BANK_LIST *bank_list;
   double dummy;

   sprintf(str, "/%s/Output", analyzer_name);
   db_find_key(hDB, 0, str, &hkey);

   if (clp.online) {
      status =
          db_open_record(hDB, hkey, &out_info, sizeof(out_info), MODE_READ, NULL, NULL);
      if (status != DB_SUCCESS) {
         cm_msg(MERROR, "bor", "Cannot read output info record");
         return 0;
      }
   }

#if HAVE_ROOT
   /* MIAS Tree output file opened in src/uiuc/EventTree/TEventTreeAnalysis.cpp. */
   gMiasTreeOutputFileName = clp.mias_tree_output_file_name;
   if(gMiasTreeOutputFileName) {
     printf("gMiasTreeOutputFileName = %s\n", gMiasTreeOutputFileName);
   } else {
     printf("gMiasTreeOutputFileName == NULL\n");
   }
#endif

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
            db_check_record(hDB, 0, str, strcomb(bank_list->init_str), TRUE);
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
         db_check_record(hDB, 0, str, strcomb(analyze_request[i].init_string), TRUE);
      }
   }

   /* delete tests in ODB */
   sprintf(str, "/%s/Tests", analyzer_name);
   db_find_key(hDB, 0, str, &hkey);
   if (hkey)
      db_delete_key(hDB, hkey, FALSE);

#ifdef HAVE_HBOOK
   /* create global memory */
   if (clp.online) {
      /* book online N-tuples only once when online */
      status = book_ntuples();
      if (status != SUCCESS)
         return status;
   } else {
      if (equal_ustring(clp.output_file_name, "OFLN")) {
         /* book online N-tuples only once when online */
         status = book_ntuples();
         if (status != SUCCESS)
            return status;
      }
   }
#endif                          /* HAVE_HBOOK */

#ifdef HAVE_ROOT
   if (clp.online) {
      /* book online N-tuples only once when online */
      status = book_ttree();
      if (status != SUCCESS)
         return status;
   }
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

/*-- BOR routine ---------------------------------------------------*/

INT bor(INT run_number, char *error)
{
   ANA_MODULE **module;
   INT i, j, size;
   char str[256], file_name[256], *ext_str;
   BANK_LIST *bank_list;

   /* load parameters */
   load_parameters(run_number);

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

   /* clear histos, N-tuples and tests */
   if (clp.online && out_info.clear_histos) {
#ifdef HAVE_HBOOK
      int hid[10000];
      int n;

      for (i = 0; analyze_request[i].event_name[0]; i++)
         if (analyze_request[i].bank_list != NULL)
            if (HEXIST(analyze_request[i].ar_info.event_id))
               HRESET(analyze_request[i].ar_info.event_id, bstr);

      /* get list of all histos */
      HIDALL(hid, n);
      for (i = 0; i < n; i++) {
         for (j = 0; j < 10000; j++)
            if (lock_list[j] == 0 || lock_list[j] == hid[i])
               break;

         /* clear histo if not locked */
         if (lock_list[j] != hid[i])
            HRESET(hid[i], bstr);
      }
#endif                          /* HAVE_HBOOK */

#ifdef HAVE_ROOT
      /* clear histos */
      if (clp.online && out_info.clear_histos)
         ClearRootHistograms(gManaHistsDir);
#endif                          /* HAVE_ROOT */

      /* clear tests */
      test_clear();
   }

   /* open output file if not already open (append mode) and in offline mode */
   if (!clp.online && out_file == NULL && !pvm_master
       && !equal_ustring(clp.output_file_name, "OFLN")) {
      if (out_info.filename[0]) {
         strcpy(str, out_info.filename);
         if (strchr(str, '%') != NULL)
            sprintf(file_name, str, run_number);
         else
            strcpy(file_name, str);

         /* check output file extension */
         out_gzip = FALSE;
         if (strchr(file_name, '.')) {
            ext_str = file_name + strlen(file_name) - 1;
            while (*ext_str != '.')
               ext_str--;

            if (strncmp(ext_str, ".gz", 3) == 0) {
               out_gzip = TRUE;
               ext_str--;
               while (*ext_str != '.' && ext_str > file_name)
                  ext_str--;
            }

            if (strncmp(ext_str, ".asc", 4) == 0)
               out_format = FORMAT_ASCII;
            else if (strncmp(ext_str, ".mid", 4) == 0)
               out_format = FORMAT_MIDAS;
            else if (strncmp(ext_str, ".rz", 3) == 0)
               out_format = FORMAT_HBOOK;
            else if (strncmp(ext_str, ".root", 5) == 0)
               out_format = FORMAT_ROOT;
            else {
               strcpy(error,
                      "Unknown output data format. Please use file extension .asc, .mid, .rz or .root.\n");
               cm_msg(MERROR, "bor", error);
               return 0;
            }
         } else
            out_format = FORMAT_ASCII;

#ifdef HAVE_PVM
         /* use node name as filename if PVM slave */
         if (pvm_slave) {
            /* extract extension */
            if (strchr(file_name, '.')) {
               strcpy(str, strchr(file_name, '.') + 1);
               sprintf(file_name, "n%d", pvm_client_index);
               strcat(file_name, ".");
               strcat(file_name, str);
            } else {
               sprintf(file_name, "n%d", pvm_client_index);
            }

            PVM_DEBUG("BOR: file_name = %s", file_name);
         }
#endif

         /* open output file */
         if (out_format == FORMAT_HBOOK) {
#ifdef HAVE_HBOOK
            int status, lrec;
            char str2[80];

            lrec = clp.lrec;
#ifdef extname
            quest_[9] = 65000;
#else
            QUEST[9] = 65000;
#endif

            strcpy(str, "BSIZE");
            HBSET(str, HBOOK_LREC, status);
            strcpy(str, "OFFLINE");
            strcpy(str2, "NQP");
            HROPEN(1, str, file_name, str2, lrec, status);
            if (status != 0) {
               sprintf(error, "Cannot open output file %s", out_info.filename);
               cm_msg(MERROR, "bor", error);
               out_file = NULL;
               return 0;
            } else
               out_file = (FILE *) 1;
#else
            cm_msg(MERROR, "bor", "HBOOK support is not compiled in");
#endif                          /* HAVE_HBOOK */
         }

         else if (out_format == FORMAT_ROOT) {
#ifdef HAVE_ROOT
            // ensure the output file is closed
            assert(gManaOutputFile == NULL);

            gManaOutputFile =
                new TFile(file_name, "RECREATE", "Midas Analyzer output file");
            if (gManaOutputFile == NULL) {
               sprintf(error, "Cannot open output file %s", out_info.filename);
               cm_msg(MERROR, "bor", error);
               out_file = NULL;
               return 0;
            }
            // make all ROOT objects created by user module bor() functions
            // go into the output file
            gManaOutputFile->cd();

            out_file = (FILE *) 1;
#else
            cm_msg(MERROR, "bor", "ROOT support is not compiled in");
#endif                          /* HAVE_ROOT */
         }

         else {
            if (out_gzip)
               out_file = (FILE *) gzopen(file_name, "wb");
            else if (out_format == FORMAT_ASCII)
               out_file = fopen(file_name, "wt");
            else
               out_file = fopen(file_name, "wb");
            if (out_file == NULL) {
               sprintf(error, "Cannot open output file %s", file_name);
               cm_msg(MERROR, "bor", error);
               return 0;
            }
         }
      } else
         out_file = NULL;

#ifdef HAVE_HBOOK
      /* book N-tuples */
      if (out_format == FORMAT_HBOOK) {
         int status = book_ntuples();
         if (status != SUCCESS)
            return status;
      }
#endif                          /* HAVE_HBOOK */

#ifdef HAVE_ROOT
      /* book ROOT TTree */
      if (out_format == FORMAT_ROOT) {
         int status = book_ttree();
         if (status != SUCCESS)
            return status;
      }
#endif                          /* HAVE_ROOT */

   }

#ifdef HAVE_ROOT
   /* open secondary output file if applicable. */
   if (out_format != FORMAT_ROOT && clp.secondary_output_file_name[0] != '\0') {
    // ensure the output file is closed
    assert(gManaOutputFile == NULL);

    gManaOutputFile =
        new TFile(clp.secondary_output_file_name, "RECREATE", 
			"Midas Analyzer output file");
    if (gManaOutputFile == NULL) {
       sprintf(error, "Cannot open secondary output file %s", 
		       clp.secondary_output_file_name);
       cm_msg(MERROR, "bor", error);
       return 0;
    }
    // make all ROOT objects created by user module bor() functions
    // go into the output file
    gManaOutputFile->cd();
   }

#endif                          /* HAVE_ROOT */

   /* if (out_file == NULL) */
   /* save run number */
   current_run_number = run_number;

   /* call bor for modules */
   for (i = 0; analyze_request[i].event_name[0]; i++) {
      module = analyze_request[i].ana_module;
      for (j = 0; module != NULL && module[j] != NULL; j++)
         if (module[j]->bor != NULL && module[j]->enabled)
            module[j]->bor(run_number);
   }

   /* call main analyzer BOR routine */
   return ana_begin_of_run(run_number, error);
}

/*-- EOR routine ---------------------------------------------------*/

INT eor(INT run_number, char *error)
{
   ANA_MODULE **module;
   BANK_LIST *bank_list;
   INT i, j, status;
   char str[256], file_name[256];

   /* call EOR routines modules */
   for (i = 0; analyze_request[i].event_name[0]; i++) {
      module = analyze_request[i].ana_module;
      for (j = 0; module != NULL && module[j] != NULL; j++)
         if (module[j]->eor != NULL && module[j]->enabled)
            module[j]->eor(run_number);
   }

   /* call main analyzer BOR routine */
   status = ana_end_of_run(run_number, error);

   /* save histos if requested */
   if (out_info.histo_dump && clp.online) {
      strcpy(str, out_info.histo_dump_filename);
      if (strchr(str, '%') != NULL)
         sprintf(file_name, str, run_number);
      else
         strcpy(file_name, str);

      add_data_dir(str, file_name);
#ifdef HAVE_HBOOK
      for (i = 0; i < (int) strlen(str); i++)
         if (isupper(str[i]))
            break;

      if (i < (int) strlen(str)) {
         printf
             ("Error: Due to a limitation in HBOOK, directoy names may not contain uppercase\n");
         printf("       characters. Histogram saving to %s will not work.\n", str);
      } else {
         char str2[256];
         strcpy(str2, "NT");
         HRPUT(0, str, str2);
      }
#endif                          /* HAVE_HBOOK */

#ifdef HAVE_ROOT
      SaveRootHistograms(gManaHistsDir, str);
#endif                          /* HAVE_ROOT */
   }

   /* close output file */
   if (out_file && !out_append) {
      if (out_format == FORMAT_HBOOK) {
#ifdef HAVE_HBOOK
         HROUT(0, i, bstr);
         strcpy(str, "OFFLINE");
         HREND(str);
#else
         cm_msg(MERROR, "eor", "HBOOK support is not compiled in");
#endif                          /* HAVE_HBOOK */
      } else if (out_format == FORMAT_ROOT) {
#ifdef HAVE_ROOT
         CloseRootOutputFile();
#else
         cm_msg(MERROR, "eor", "ROOT support is not compiled in");
#endif                          /* HAVE_ROOT */
      } else {
         if (out_gzip)
            gzclose(out_file);
         else
            fclose(out_file);
      }

      out_file = NULL;

      /* free CWNT buffer */
      for (i = 0; analyze_request[i].event_name[0]; i++) {
         bank_list = analyze_request[i].bank_list;

         if (bank_list == NULL) {
            if (analyze_request[i].addr) {
               free(analyze_request[i].addr);
               analyze_request[i].addr = NULL;
            }
         } else {
            for (; bank_list->name[0]; bank_list++)
               if (bank_list->addr) {
                  free(bank_list->addr);
                  bank_list->addr = NULL;
               }
         }
      }
   }

#ifdef HAVE_ROOT
   /* close secondary output file */
   if(clp.secondary_output_file_name[0] != '\0') {
     CloseRootOutputFile();
   }
#endif

   return status;
}

/*-- transition callbacks ------------------------------------------*/

/*-- start ---------------------------------------------------------*/

INT tr_prestart(INT rn, char *error)
{
   INT status, i;

   now_stopping = FALSE;

   /* reset counters */
   for (i = 0; analyze_request[i].event_name[0]; i++) {
      analyze_request[i].ar_stats.events_received = 0;
      analyze_request[i].ar_stats.events_per_sec = 0;
      analyze_request[i].ar_stats.events_written = 0;
      analyze_request[i].events_received = 0;
      analyze_request[i].events_written = 0;
   }

   status = bor(rn, error);
   if (status != SUCCESS)
      return status;

   return SUCCESS;
}

/*-- stop ----------------------------------------------------------*/

INT tr_stop(INT rn, char *error)
{
   INT i, status, n_bytes;

   /* wait until all events in buffers are analyzed */

  now_stopping = TRUE;

  printf("*** Received stop request ***\n");

   if (rpc_is_remote())
      while (bm_poll_event(TRUE));
   else
      for (i = 0; analyze_request[i].event_name[0]; i++) {
         do {
            bm_get_buffer_level(analyze_request[i].buffer_handle, &n_bytes);
            if (n_bytes > 0)
               cm_yield(100);
         } while (n_bytes > 0);
      }

   /* update statistics */
   update_stats();

   status = eor(rn, error);
   if (status != SUCCESS)
      return status;

   return CM_SUCCESS;
}

/*-- pause ---------------------------------------------------------*/

INT tr_pause(INT rn, char *error)
{
   INT status;

   status = ana_pause_run(rn, error);
   if (status != CM_SUCCESS)
      return status;

   return CM_SUCCESS;
}

/*-- resume --------------------------------------------------------*/

INT tr_resume(INT rn, char *error)
{
   INT status;

   status = ana_resume_run(rn, error);
   if (status != CM_SUCCESS)
      return status;

   return CM_SUCCESS;
}

/*---- ASCII output ------------------------------------------------*/

#define STR_INC(p,base) { p+=strlen(p); \
                          if (p > base+sizeof(base)) \
                            cm_msg(MERROR, "STR_INC", "ASCII buffer too small"); }


INT write_event_ascii(FILE * file, EVENT_HEADER * pevent, ANALYZE_REQUEST * par)
{
   INT status, size, i, j, count;
   BOOL exclude;
   BANK_HEADER *pbh;
   BANK_LIST *pbl;
   EVENT_DEF *event_def;
   BANK *pbk;
   BANK32 *pbk32;
   void *pdata;
   char *pbuf, name[5], type_name[10];
   LRS1882_DATA *lrs1882;
   LRS1877_DATA *lrs1877;
   LRS1877_HEADER *lrs1877_header;
   HNDLE hKey;
   KEY key;
   char buffer[100000];
   DWORD bkname;
   WORD bktype;

   event_def = db_get_event_definition(pevent->event_id);
   if (event_def == NULL)
      return SS_SUCCESS;

   /* write event header */
   pbuf = buffer;
   name[4] = 0;

   if (pevent->event_id == EVENTID_BOR)
      sprintf(pbuf, "%%ID BOR NR %d\n", (int) pevent->serial_number);
   else if (pevent->event_id == EVENTID_EOR)
      sprintf(pbuf, "%%ID EOR NR %d\n", (int) pevent->serial_number);
   else
      sprintf(pbuf, "%%ID %d TR %d NR %d\n", pevent->event_id, pevent->trigger_mask,
              (int) pevent->serial_number);
   STR_INC(pbuf, buffer);

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

         /* look if bank is in exclude list */
         exclude = FALSE;
         pbl = NULL;
         if (par->bank_list != NULL)
            for (i = 0; par->bank_list[i].name[0]; i++)
               if (*((DWORD *) par->bank_list[i].name) == bkname) {
                  pbl = &par->bank_list[i];
                  exclude = (pbl->output_flag == 0);
                  break;
               }

         if (!exclude) {
            if (rpc_tid_size(bktype & 0xFF))
               size /= rpc_tid_size(bktype & 0xFF);

            lrs1882 = (LRS1882_DATA *) pdata;
            lrs1877 = (LRS1877_DATA *) pdata;

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

            if (bktype == TID_STRUCT) {
               if (pbl == NULL)
                  cm_msg(MERROR, "write_event_ascii", "received unknown bank %s", name);
               else
                  /* write structured bank */
                  for (i = 0;; i++) {
                     status = db_enum_key(hDB, pbl->def_key, i, &hKey);
                     if (status == DB_NO_MORE_SUBKEYS)
                        break;

                     db_get_key(hDB, hKey, &key);
                     sprintf(pbuf, "%s:\n", key.name);
                     STR_INC(pbuf, buffer);

                     /* adjust for alignment */
                     pdata =
                         (void *) VALIGN(pdata,
                                         MIN(ss_get_struct_align(), key.item_size));

                     for (j = 0; j < key.num_values; j++) {
                        db_sprintf(pbuf, pdata, key.item_size, j, key.type);
                        strcat(pbuf, "\n");
                        STR_INC(pbuf, buffer);
                     }

                     /* shift data pointer to next item */
                     pdata = (char *) pdata + key.item_size * key.num_values;
                  }
            } else {
               /* write variable length bank  */
               if ((bktype & 0xFF00) == TID_LRS1877) {
                  for (i = 0; i < size;) {
                     lrs1877_header = (LRS1877_HEADER *) & lrs1877[i];

                     /* print header */
                     sprintf(pbuf, "GA %d BF %d CN %d",
                             lrs1877_header->geo_addr, lrs1877_header->buffer,
                             lrs1877_header->count);
                     strcat(pbuf, "\n");
                     STR_INC(pbuf, buffer);

                     count = lrs1877_header->count;
                     if (count == 0)
                        break;
                     for (j = 1; j < count; j++) {
                        /* print data */
                        sprintf(pbuf, "GA %d CH %02d ED %d DA %1.1lf",
                                lrs1877[i].geo_addr, lrs1877[i + j].channel,
                                lrs1877[i + j].edge, lrs1877[i + j].data * 0.5);
                        strcat(pbuf, "\n");
                        STR_INC(pbuf, buffer);
                     }

                     i += count;
                  }
               } else
                  for (i = 0; i < size; i++) {
                     if ((bktype & 0xFF00) == 0)
                        db_sprintf(pbuf, pdata, size, i, bktype & 0xFF);

                     else if ((bktype & 0xFF00) == TID_LRS1882)
                        sprintf(pbuf, "GA %d CH %02d DA %d",
                                lrs1882[i].geo_addr, lrs1882[i].channel, lrs1882[i].data);

                     else if ((bktype & 0xFF00) == TID_PCOS3)
                        sprintf(pbuf, "TBD");
                     else
                        db_sprintf(pbuf, pdata, size, i, bktype & 0xFF);

                     strcat(pbuf, "\n");
                     STR_INC(pbuf, buffer);
                  }
            }
         }

      } while (1);
   }

  /*---- FIXED format ----------------------------------------------*/
   if (event_def->format == FORMAT_FIXED) {
      if (event_def->hDefKey == 0)
         cm_msg(MERROR, "write_event_ascii", "cannot find event definition");
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

   /* insert empty line after each event */
   strcat(pbuf, "\n");
   size = strlen(buffer);

   /* write record to device */
   if (out_gzip)
      status = gzwrite(file, buffer, size) == size ? SS_SUCCESS : SS_FILE_ERROR;
   else
      status =
          fwrite(buffer, 1, size, file) == (size_t) size ? SS_SUCCESS : SS_FILE_ERROR;

   return status;
}

/*---- MIDAS output ------------------------------------------------*/

INT write_event_midas(FILE * file, EVENT_HEADER * pevent, ANALYZE_REQUEST * par)
{
   INT status, size = 0, i;
   BOOL exclude;
   BANK_HEADER *pbh;
   BANK_LIST *pbl;
   EVENT_DEF *event_def;
   BANK *pbk;
   BANK32 *pbk32;
   char *pdata, *pdata_copy;
   char *pbuf;
   EVENT_HEADER *pevent_copy;
   DWORD bkname, bksize;
   WORD bktype;
   static char *buffer = NULL;

   if (buffer == NULL)
      buffer = (char *) malloc(EXT_EVENT_SIZE);

   pevent_copy = (EVENT_HEADER *) ALIGN8((PTYPE) buffer);

   if (clp.filter) {
      /* use original event */
      size = pevent->data_size + sizeof(EVENT_HEADER);
      memcpy(pevent_copy, pevent, size);
   } else {
      /* copy only banks which are turned on via /analyzer/bank switches */

    /*---- MIDAS format ----------------------------------------------*/

      event_def = db_get_event_definition(pevent->event_id);
      if (event_def == NULL)
         return SUCCESS;

      if (event_def->format == FORMAT_MIDAS) {
         /* copy event header */
         pbuf = (char *) pevent_copy;
         memcpy(pbuf, pevent, sizeof(EVENT_HEADER));
         pbuf += sizeof(EVENT_HEADER);

         pbh = (BANK_HEADER *) (pevent + 1);

         if (bk_is32(pbh))
            bk_init32(pbuf);
         else
            bk_init(pbuf);

         pbk = NULL;
         pbk32 = NULL;
         pdata_copy = pbuf;
         do {
            /* scan all banks */
            if (bk_is32(pbh)) {
               size = bk_iterate32(pbh, &pbk32, &pdata);
               if (pbk32 == NULL)
                  break;
               bkname = *((DWORD *) pbk32->name);
               bktype = (WORD) pbk32->type;
               bksize = pbk32->data_size;
            } else {
               size = bk_iterate(pbh, &pbk, &pdata);
               if (pbk == NULL)
                  break;
               bkname = *((DWORD *) pbk->name);
               bktype = (WORD) pbk->type;
               bksize = pbk->data_size;
            }

            /* look if bank is in exclude list */
            exclude = FALSE;
            pbl = NULL;
            if (par->bank_list != NULL)
               for (i = 0; par->bank_list[i].name[0]; i++)
                  if (*((DWORD *) par->bank_list[i].name) == bkname) {
                     pbl = &par->bank_list[i];
                     exclude = (pbl->output_flag == 0);
                     break;
                  }

            if (!exclude) {
               /* copy bank */
               bk_create(pbuf, (char *) (&bkname), bktype, &pdata_copy);
               memcpy(pdata_copy, pdata, bksize);
               pdata_copy += bksize;
               bk_close(pbuf, pdata_copy);
            }

         } while (1);

         /* set event size in header */
         size = bk_size(pbuf);
         pevent_copy->data_size = size;
         size += sizeof(EVENT_HEADER);
      }

    /*---- FIXED format ----------------------------------------------*/
      if (event_def->format == FORMAT_FIXED) {
         size = pevent->data_size + sizeof(EVENT_HEADER);
         memcpy(pevent_copy, pevent, size);
      }

      if (pevent_copy->data_size == 0)
         return SUCCESS;
   }

   /* write record to device */
   if (out_gzip)
      status = gzwrite(file, pevent_copy, size) == size ? SUCCESS : SS_FILE_ERROR;
   else
      status =
          fwrite(pevent_copy, 1, size, file) == (size_t) size ? SUCCESS : SS_FILE_ERROR;

   return status;
}

/*---- HBOOK output ------------------------------------------------*/

#ifdef HAVE_HBOOK

INT write_event_hbook(FILE * file, EVENT_HEADER * pevent, ANALYZE_REQUEST * par)
{
   INT i, j, k, n, size, item_size, status;
   BANK *pbk;
   BANK32 *pbk32;
   BANK_LIST *pbl;
   BANK_HEADER *pbh;
   void *pdata;
   BOOL exclude, exclude_all;
   char block_name[5], str[80];
   float rwnt[512];
   EVENT_DEF *event_def;
   HNDLE hkey;
   KEY key;
   DWORD bkname;
   WORD bktype;

   /* return if N-tuples are disabled */
   if (!ntuple_flag)
      return SS_SUCCESS;

   event_def = db_get_event_definition(pevent->event_id);
   if (event_def == NULL)
      return SS_SUCCESS;

   if (event_def->disabled)
      return SS_SUCCESS;

   /* fill number info */
   if (clp.rwnt) {
      memset(rwnt, 0, sizeof(rwnt));
      rwnt[0] = (float) current_run_number;
      rwnt[1] = (float) pevent->serial_number;
      rwnt[2] = (float) pevent->time_stamp;
   } else {
      par->number.run = current_run_number;
      par->number.serial = pevent->serial_number;
      par->number.time = pevent->time_stamp;
   }

  /*---- MIDAS format ----------------------------------------------*/

   if (event_def->format == FORMAT_MIDAS) {
      /* first fill number block */
      strcpy(str, "Number");
      if (!clp.rwnt)
         HFNTB(pevent->event_id, str);

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

         /* fill CW N-tuple */
         if (!exclude && pbl != NULL && !clp.rwnt) {
            /* set array size in bank list */
            if ((bktype & 0xFF) != TID_STRUCT) {
               item_size = rpc_tid_size(bktype & 0xFF);
               if (item_size == 0) {
                  cm_msg(MERROR, "write_event_hbook",
                         "Received bank %s with unknown item size", block_name);
                  continue;
               }

               pbl->n_data = size / item_size;

               /* check bank size */
               if (pbl->n_data > pbl->size) {
                  cm_msg(MERROR, "write_event_hbook",
                         "Bank %s has more (%d) entries than maximum value (%d)",
                         block_name, pbl->n_data, pbl->size);
                  continue;
               }

               /* copy bank to buffer in bank list, DWORD aligned */
               if (item_size >= 4) {
                  size = MIN((INT) pbl->size * item_size, size);
                  memcpy(pbl->addr, pdata, size);
               } else if (item_size == 2)
                  for (i = 0; i < (INT) pbl->n_data; i++)
                     *((DWORD *) pbl->addr + i) = (DWORD) * ((WORD *) pdata + i);
               else if (item_size == 1)
                  for (i = 0; i < (INT) pbl->n_data; i++)
                     *((DWORD *) pbl->addr + i) = (DWORD) * ((BYTE *) pdata + i);
            } else {
               /* hope that structure members are aligned as HBOOK thinks ... */
               memcpy(pbl->addr, pdata, size);
            }

            /* fill N-tuple */
            HFNTB(pevent->event_id, block_name);
         }

         /* fill RW N-tuple */
         if (!exclude && pbl != NULL && clp.rwnt) {
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
                  (char *) pdata += key.item_size * key.num_values;
               }
            }
         }

      } while (TRUE);

      /* fill RW N-tuple */
      if (clp.rwnt && file != NULL && !exclude_all)
         HFN(pevent->event_id, rwnt);

      /* fill shared memory */
      if (file == NULL)
         HFNOV(pevent->event_id, rwnt);

   }


   /* if (event_def->format == FORMAT_MIDAS) */
 /*---- YBOS format ----------------------------------------------*/
   else if (event_def->format == FORMAT_YBOS) {
      YBOS_BANK_HEADER *pybk;

      /* first fill number block */
      strcpy(str, "Number");
      if (!clp.rwnt)
         HFNTB(pevent->event_id, str);

      pbk = NULL;
      exclude_all = TRUE;
      do {
         /* scan all banks */
         size = ybk_iterate((DWORD *) (pevent + 1), &pybk, &pdata);
         if (pybk == NULL)
            break;

         /* in bytes */
         size <<= 2;

         /* look if bank is in exclude list */
         *((DWORD *) block_name) = pybk->name;
         block_name[4] = 0;

         exclude = FALSE;
         pbl = NULL;
         if (par->bank_list != NULL) {
            for (i = 0; par->bank_list[i].name[0]; i++)
               if (*((DWORD *) par->bank_list[i].name) == pybk->name) {
                  pbl = &par->bank_list[i];
                  exclude = (pbl->output_flag == 0);
                  break;
               }
            if (par->bank_list[i].name[0] == 0)
               cm_msg(MERROR, "write_event_hbook", "Received unknown bank %s",
                      block_name);
         }

         /* fill CW N-tuple */
         if (!exclude && pbl != NULL && !clp.rwnt) {
            /* set array size in bank list */
            if ((pybk->type & 0xFF) < MAX_BKTYPE) {
               item_size = ybos_get_tid_size(pybk->type & 0xFF);
               if (item_size == 0) {
                  cm_msg(MERROR, "write_event_hbook",
                         "Received bank %s with unknown item size", block_name);
                  continue;
               }

               pbl->n_data = size / item_size;

               /* check bank size */
               if (pbl->n_data > pbl->size) {
                  cm_msg(MERROR, "write_event_hbook",
                         "Bank %s has more (%d) entries than maximum value (%d)",
                         block_name, pbl->n_data, pbl->size);
                  continue;
               }

               /* copy bank to buffer in bank list, DWORD aligned */
               if (item_size >= 4) {
                  size = MIN((INT) pbl->size * item_size, size);
                  memcpy(pbl->addr, pdata, size);
               } else if (item_size == 2)
                  for (i = 0; i < (INT) pbl->n_data; i++)
                     *((DWORD *) pbl->addr + i) = (DWORD) * ((WORD *) pdata + i);
               else if (item_size == 1)
                  for (i = 0; i < (INT) pbl->n_data; i++)
                     *((DWORD *) pbl->addr + i) = (DWORD) * ((BYTE *) pdata + i);
            } else {
               /* hope that structure members are aligned as HBOOK thinks ... */
               memcpy(pbl->addr, pdata, size);
            }

            /* fill N-tuple */
            HFNTB(pevent->event_id, block_name);
         }

         /* fill RW N-tuple */
         if (!exclude && pbl != NULL && clp.rwnt) {
            exclude_all = FALSE;

            item_size = ybos_get_tid_size(pybk->type & 0xFF);
            /* set array size in bank list */
            if ((pybk->type & 0xFF) < MAX_BKTYPE) {
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
                  switch (pybk->type & 0xFF) {
                  case I1_BKTYPE:
                     rwnt[pbl->n_data + i] = (float) (*((BYTE *) pdata + i));
                     break;
                  case I2_BKTYPE:
                     rwnt[pbl->n_data + i] = (float) (*((WORD *) pdata + i));
                     break;
                  case I4_BKTYPE:
                     rwnt[pbl->n_data + i] = (float) (*((DWORD *) pdata + i));
                     break;
                  case F4_BKTYPE:
                     rwnt[pbl->n_data + i] = (float) (*((float *) pdata + i));
                     break;
                  case D8_BKTYPE:
                     rwnt[pbl->n_data + i] = (float) (*((double *) pdata + i));
                     break;
                  }
               }

               /* zero padding */
               for (; i < (INT) pbl->size; i++)
                  rwnt[pbl->n_data + i] = 0.f;
            } else {
               printf("YBOS is not supporting STRUCTURE banks \n");
            }
         }

      } while (TRUE);

      /* fill RW N-tuple */
      if (clp.rwnt && file != NULL && !exclude_all)
         HFN(pevent->event_id, rwnt);

      /* fill shared memory */
      if (file == NULL)
         HFNOV(pevent->event_id, rwnt);

   }


   /* if (event_def->format == FORMAT_YBOS) */
 /*---- FIXED format ----------------------------------------------*/
   if (event_def->format == FORMAT_FIXED) {
      if (clp.rwnt) {
         /* fill N-tuple from structured bank */
         pdata = pevent + 1;
         k = 3;                 /* index 0..2 for run/serial/time */

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
            (char *) pdata += key.item_size * key.num_values;
         }

         /* fill RW N-tuple */
         if (file != NULL)
            HFN(pevent->event_id, rwnt);

         /* fill shared memory */
         if (file == NULL)
            HFNOV(pevent->event_id, rwnt);
      } else {
         memcpy(par->addr, pevent + 1, pevent->data_size);

         /* fill N-tuple */
         HFNT(pevent->event_id);
      }

   }

   return SUCCESS;
}
#endif                          /* HAVE_HBOOK */

/*---- ROOT output -------------------------------------------------*/

#ifdef HAVE_ROOT

INT write_event_ttree(FILE * file, EVENT_HEADER * pevent, ANALYZE_REQUEST * par)
{
   INT i, bklen;
   BANK *pbk;
   BANK32 *pbk32;
   BANK_LIST *pbl;
   BANK_HEADER *pbh;
   void *pdata;
   BOOL exclude, exclude_all;
   char bank_name[5];
   EVENT_DEF *event_def;
   DWORD bkname;
   WORD bktype;
   EVENT_TREE *et;
   TBranch *branch;

   /* return if N-tuples are disabled */
   if (!ntuple_flag)
      return SS_SUCCESS;

   event_def = db_get_event_definition(pevent->event_id);
   if (event_def == NULL)
      return SS_SUCCESS;

   if (event_def->disabled)
      return SS_SUCCESS;

   /* fill number info */
   par->number.run = current_run_number;
   par->number.serial = pevent->serial_number;
   par->number.time = pevent->time_stamp;

  /*---- MIDAS format ----------------------------------------------*/

   if (event_def->format == FORMAT_MIDAS) {
      /* find event in tree structure */
      for (i = 0; i < tree_struct.n_tree; i++)
         if (tree_struct.event_tree[i].event_id == pevent->event_id)
            break;

      if (i == tree_struct.n_tree) {
         cm_msg(MERROR, "write_event_ttree", "Event #%d not booked by book_ttree()",
                pevent->event_id);
         return SS_INVALID_FORMAT;
      }

      et = tree_struct.event_tree + i;

      /* first mark all banks non-filled */
      for (i = 0; i < et->n_branch; i++)
         et->branch_filled[i] = FALSE;

      /* first fill number block */
      et->branch_filled[0] = TRUE;

      pbk = NULL;
      pbk32 = NULL;
      exclude_all = TRUE;
      do {
         pbh = (BANK_HEADER *) (pevent + 1);
         /* scan all banks */
         if (bk_is32(pbh)) {
            bklen = bk_iterate32(pbh, &pbk32, &pdata);
            if (pbk32 == NULL)
               break;
            bkname = *((DWORD *) pbk32->name);
            bktype = (WORD) pbk32->type;
         } else {
            bklen = bk_iterate(pbh, &pbk, &pdata);
            if (pbk == NULL)
               break;
            bkname = *((DWORD *) pbk->name);
            bktype = (WORD) pbk->type;
         }

         if (rpc_tid_size(bktype & 0xFF))
            bklen /= rpc_tid_size(bktype & 0xFF);

         /* look if bank is in exclude list */
         *((DWORD *) bank_name) = bkname;
         bank_name[4] = 0;

         exclude = FALSE;
         pbl = NULL;
         if (par->bank_list != NULL) {
            for (i = 0; par->bank_list[i].name[0]; i++)
               if (*((DWORD *) par->bank_list[i].name) == bkname) {
                  pbl = &par->bank_list[i];
                  exclude = (pbl->output_flag == 0);
                  break;
               }
            if (par->bank_list[i].name[0] == 0) {
               cm_msg(MERROR, "write_event_ttree", "Received unknown bank %s", bank_name);
               continue;
            }
         }

         /* fill leaf */
         if (!exclude && pbl != NULL) {
            for (i = 0; i < et->n_branch; i++)
               if (*((DWORD *) (&et->branch_name[i * NAME_LENGTH])) == bkname)
                  break;

            if (i == et->n_branch) {
               cm_msg(MERROR, "write_event_ttree", "Received unknown bank %s", bank_name);
               continue;
            }

            exclude_all = FALSE;
            branch = et->branch[i];
            et->branch_filled[i] = TRUE;
            et->branch_len[i] = bklen;

            /* structured bank */
            if ((bktype & 0xFF) != TID_STRUCT) {
               TIter next(branch->GetListOfLeaves());
               TLeaf *leaf = (TLeaf *) next();

               /* varibale length array */
               leaf->SetAddress(&et->branch_len[i]);

               leaf = (TLeaf *) next();
               leaf->SetAddress(pdata);
            } else {
               /* hope that structure members are aligned as TTREE thinks ... */
               branch->SetAddress(pdata);
            }
         }

      } while (TRUE);

      /* check if all branches have been filled */
      for (i = 0; i < et->n_branch; i++)
         if (!et->branch_filled[i])
            cm_msg(MERROR, "root_write",
                   "Bank %s booked but not received, tree cannot be filled",
                   et->branch_name + (i * NAME_LENGTH));

      /* fill tree */
      if (file != NULL && !exclude_all)
         et->tree->Fill();


   }



   /* if (event_def->format == FORMAT_MIDAS) */
 /*---- FIXED format ----------------------------------------------*/
   if (event_def->format == FORMAT_FIXED) {
      /* find event in tree structure */
      for (i = 0; i < tree_struct.n_tree; i++)
         if (tree_struct.event_tree[i].event_id == pevent->event_id)
            break;

      if (i == tree_struct.n_tree) {
         cm_msg(MERROR, "write_event_ttree", "Event #%d not booked by book_ttree()",
                pevent->event_id);
         return SS_INVALID_FORMAT;
      }

      et = tree_struct.event_tree + i;

      et->tree->GetBranch(et->branch_name)->SetAddress(pevent + 1);
      et->tree->Fill();
   }

   return SUCCESS;
}

#endif                          /* HAVE_ROOT */

/*---- ODB output --------------------------------------------------*/

INT write_event_odb(EVENT_HEADER * pevent)
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
         if (rpc_tid_size(bktype & 0xFF))
            n_data /= rpc_tid_size(bktype & 0xFF);

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
               if (key.type != TID_STRING && key.type != TID_LINK)
                  pdata =
                      (void *) VALIGN(pdata, MIN(ss_get_struct_align(), key.item_size));

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
   if (event_def->format == FORMAT_FIXED && !clp.online) {
      if (db_set_record(hDB, event_def->hDefKey, (char *) (pevent + 1),
                        pevent->data_size, 0) != DB_SUCCESS)
         cm_msg(MERROR, "write_event_odb", "event #%d size mismatch", pevent->event_id);
   }

   return SUCCESS;
}

/*------------------------------------------------------------------*/

struct {
   short int event_id;
   DWORD last_time;
} last_time_event[50];

ANALYZE_REQUEST *_current_par;

void correct_num_events(INT i)
{
   if (_current_par)
      _current_par->events_received += i - 1;
}

INT process_event(ANALYZE_REQUEST * par, EVENT_HEADER * pevent)
{
   INT i, status = SUCCESS, ch;
   ANA_MODULE **module;
   DWORD actual_time;
   EVENT_DEF *event_def;
   static DWORD last_time_kb = 0;
   static char *orig_event = NULL;

   /* verbose output */
   if (clp.verbose)
      printf("event %d, number %d, total size %d\n",
             (int) pevent->event_id,
             (int) pevent->serial_number,
             (int) (pevent->data_size + sizeof(EVENT_HEADER)));

   /* save analyze_request for event number correction */
   _current_par = par;

   /* check keyboard once every second */
   actual_time = ss_millitime();
   if (!clp.online && actual_time - last_time_kb > 1000 && !clp.quiet && !pvm_slave) {
      last_time_kb = actual_time;

      while (ss_kbhit()) {
         ch = ss_getchar(0);
         if (ch == -1)
            ch = getchar();

         if ((char) ch == '!')
            return RPC_SHUTDOWN;
      }
   }

   if (par == NULL) {
      /* load ODB with BOR event */
      if (pevent->event_id == EVENTID_BOR) {
         /* get run number from BOR event */
         current_run_number = pevent->serial_number;

         cm_msg(MINFO, "process_event", "Set run number %d in ODB", current_run_number);
         assert(current_run_number > 0);

         /* set run number in ODB */
         status = db_set_value(hDB, 0, "/Runinfo/Run number", &current_run_number,
                               sizeof(current_run_number), 1, TID_INT);
         assert(status == SUCCESS);

         /* load ODB from event */
         odb_load(pevent);

#ifdef HAVE_PVM
         PVM_DEBUG("process_event: ODB load");
#endif
      }
   } else
      /* increment event counter */
      par->events_received++;

#ifdef HAVE_PVM

   /* if master, distribute events to clients */
   if (pvm_master) {
      status = pvm_distribute(par, pevent);
      return status;
   }
#endif

   /* don't analyze special (BOR,MESSAGE,...) events */
   if (par == NULL)
      return SUCCESS;

   /* swap event if necessary */
   event_def = db_get_event_definition(pevent->event_id);
   if (event_def == NULL)
      return 0;

   if (event_def->format == FORMAT_MIDAS)
      bk_swap((BANK_HEADER *) (pevent + 1), FALSE);

   /* keep copy of original event */
   if (clp.filter) {
      if (orig_event == NULL)
         orig_event = (char *) malloc(MAX_EVENT_SIZE + sizeof(EVENT_HEADER));
      memcpy(orig_event, pevent, pevent->data_size + sizeof(EVENT_HEADER));
   } 

  /* skip events after end of run when online */
  if(clp.online && now_stopping) {
    return 0;
  }

  /*---- analyze event ----*/

   /* call non-modular analyzer if defined */
   if (par->analyzer) {
      status = par->analyzer(pevent, (void *) (pevent + 1));

      /* don't continue if event was rejected */
      if (status == ANA_SKIP)
         return 0;
   }

   /* loop over analyzer modules */
   module = par->ana_module;
   for (i = 0; module != NULL && module[i] != NULL; i++) {
      if (module[i]->enabled) {
	//printf("%s\n", module[i]->name);
         status = module[i]->analyzer(pevent, (void *) (pevent + 1));

         /* don't continue if event was rejected */
         if (status == ANA_SKIP)
            return 0;
      }
   }

   if (event_def->format == FORMAT_MIDAS) {
      /* check if event got too large */
      i = bk_size(pevent + 1);
//      if (i > MAX_EVENT_SIZE)
      if (i >= 8*EXT_EVENT_SIZE/10)
         cm_msg(MERROR, "process_event", "Event got too large (%d Bytes) in analyzer", i); 

      /* correct for increased event size */
      pevent->data_size = i;
   }

   if (event_def->format == FORMAT_YBOS) {
      /* check if event got too large */
      i = ybk_size((DWORD *) (pevent + 1));
      if (i > MAX_EVENT_SIZE)
         cm_msg(MERROR, "process_event", "Event got too large (%d Bytes) in analyzer", i);

      /* correct for increased event size */
      pevent->data_size = i;
   }

   /* increment tests */
   if (par->use_tests)
      test_increment();

   /* in filter mode, use original event */
   if (clp.filter)
      pevent = (EVENT_HEADER *) orig_event;

   /* write resulting event */
   if (out_file) {
#ifdef HAVE_HBOOK
      if (out_format == FORMAT_HBOOK)
         status = write_event_hbook(out_file, pevent, par);
#endif                          /* HAVE_HBOOK */
#ifdef HAVE_ROOT
      if (out_format == FORMAT_ROOT)
         status = write_event_ttree(out_file, pevent, par);
#endif                          /* HAVE_ROOT */
      if (out_format == FORMAT_ASCII)
         status = write_event_ascii(out_file, pevent, par);
      if (out_format == FORMAT_MIDAS)
         status = write_event_midas(out_file, pevent, par);

      if (status != SUCCESS) {
         cm_msg(MERROR, "process_event", "Error writing to file (Disk full?)");
         return -1;
      }

      par->events_written++;
   }
#ifdef HAVE_HBOOK
   /* fill shared memory */
   if ((clp.online || equal_ustring(clp.output_file_name, "OFLN"))
       && par->rwnt_buffer_size > 0)
      write_event_hbook(NULL, pevent, par);
#endif                          /* HAVE_HBOOK */

   /* put event in ODB once every second */
   if (out_info.events_to_odb)
      for (i = 0; i < 50; i++) {
         if (last_time_event[i].event_id == pevent->event_id) {
            if (event_def->type == EQ_PERIODIC ||
                event_def->type == EQ_SLOW ||
                actual_time - last_time_event[i].last_time > 1000) {
               last_time_event[i].last_time = actual_time;
               write_event_odb(pevent);
            }
            break;
         }
         if (last_time_event[i].event_id == 0) {
            last_time_event[i].event_id = pevent->event_id;
            last_time_event[i].last_time = actual_time;
            write_event_odb(pevent);
            break;
         }
      }

   return SUCCESS;
}

/*------------------------------------------------------------------*/

void receive_event(HNDLE buffer_handle, HNDLE request_id, EVENT_HEADER * pheader,
                   void *pevent)
/* receive online event */
{
   INT i;
   ANALYZE_REQUEST *par;
   static DWORD buffer_size = 0;
   static char *buffer = NULL;
   char *pb;

   if (buffer == NULL) {
      buffer = (char *) malloc(EXT_EVENT_SIZE);

      if (buffer == NULL) {
         cm_msg(MERROR, "receive_event", "Not enough memory to buffer event of size %d",
                buffer_size);
         return;
      }
   }

   /* align buffer */
   pb = (char *) ALIGN8((PTYPE) buffer);

   /* copy event to local buffer */
   memcpy(pb, pheader, pheader->data_size + sizeof(EVENT_HEADER));

   par = analyze_request;

   for (i = 0; par->event_name[0]; par++)
      if (par->buffer_handle == buffer_handle && par->request_id == request_id) {
         process_event(par, (EVENT_HEADER *) pb);
      }
}

/*------------------------------------------------------------------*/

void update_request(HNDLE hDB, HNDLE hKey, void *info)
{
   AR_INFO *ar_info;
   INT i;

   if (!clp.online)
      return;

   /* check which request's key has changed */
   for (i = 0; analyze_request[i].event_name[0]; i++)
      if (analyze_request[i].hkey_common == hKey) {
         ar_info = &analyze_request[i].ar_info;

         /* remove previous request */
         if (analyze_request[i].request_id != -1)
            bm_delete_request(analyze_request[i].request_id);

         /* if enabled, add new request */
         if (ar_info->enabled)
            bm_request_event(analyze_request[i].buffer_handle, (short) ar_info->event_id,
                             (short) ar_info->trigger_mask, ar_info->sampling_type,
                             &analyze_request[i].request_id, receive_event);
         else
            analyze_request[i].request_id = -1;
      }

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
      db_check_record(hDB, 0, str, ANALYZER_REQUEST_STR, TRUE);
      db_find_key(hDB, 0, str, &hKey);
      analyze_request[index].hkey_common = hKey;

      strcpy(ar_info->client_name, analyzer_name);
      gethostname(ar_info->host, sizeof(ar_info->host));
      db_set_record(hDB, hKey, ar_info, sizeof(AR_INFO), 0);

      /* open hot link to analyzer request info */
      db_open_record(hDB, hKey, ar_info, sizeof(AR_INFO), MODE_READ, update_request,
                     NULL);

      /* create statistics tree */
      sprintf(str, "/%s/%s/Statistics", analyzer_name, analyze_request[index].event_name);
      db_check_record(hDB, 0, str, ANALYZER_STATS_STR, TRUE);
      db_find_key(hDB, 0, str, &hKey);
      assert(hKey);

      ar_stats->events_received = 0;
      ar_stats->events_per_sec = 0;
      ar_stats->events_written = 0;

      /* open hot link to statistics tree */
      status =
          db_open_record(hDB, hKey, ar_stats, sizeof(AR_STATS), MODE_WRITE, NULL, NULL);
      if (status != DB_SUCCESS)
         printf("Cannot open statistics record, probably other analyzer is using it\n");

      if (clp.online) {
      /*---- open event buffer ---------------------------------------*/
         bm_open_buffer(ar_info->buffer, EVENT_BUFFER_SIZE,
                        &analyze_request[index].buffer_handle);

//         /* set the default buffer cache size */
//         bm_set_cache_size(analyze_request[index].buffer_handle, 100000, 0);
         /* turn off buffer cache */
         bm_set_cache_size(analyze_request[index].buffer_handle, 0, 0);

      /*---- request event -------------------------------------------*/
         if (ar_info->enabled)
            bm_request_event(analyze_request[index].buffer_handle,
                             (short) ar_info->event_id, (short) ar_info->trigger_mask,
                             ar_info->sampling_type, &analyze_request[index].request_id,
                             receive_event);
         else
            analyze_request[index].request_id = -1;
      }
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
          (analyze_request[i].events_received / ((actual_time - last_time) / 1000.0));
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
#ifdef HAVE_HBOOK
INT clear_histos_hbook(INT id1, INT id2)
{
   INT i;

   if (id1 != id2) {
      printf("Clear ID %d to ID %d\n", id1, id2);
      for (i = id1; i <= id2; i++)
         if (HEXIST(i))
            HRESET(i, bstr);
   } else {
      printf("Clear ID %d\n", id1);
      HRESET(id1, bstr);
   }

   return SUCCESS;
}
#endif                          /* HAVE_HBOOK */

/*------------------------------------------------------------------*/

void lock_histo(INT id)
{
   INT i;

   for (i = 0; i < 10000; i++)
      if (lock_list[i] == 0)
         break;

   lock_list[i] = id;
}

/*------------------------------------------------------------------*/

#ifdef HAVE_HBOOK
INT ana_callback(INT index, void *prpc_param[])
{
   if (index == RPC_ANA_CLEAR_HISTOS)
      clear_histos_hbook(CINT(0), CINT(1));
   return RPC_SUCCESS;
}
#endif

/*------------------------------------------------------------------*/

void load_last_histos()
{
   char str[256];

   /* load previous online histos */
   if (!clp.no_load) {
      strcpy(str, out_info.last_histo_filename);

      if (strchr(str, DIR_SEPARATOR) == NULL)
         add_data_dir(str, out_info.last_histo_filename);

#ifdef HAVE_HBOOK
      {
         FILE *f;
         char str2[256];
         int i;

         for (i = 0; i < (int) strlen(str); i++)
            if (isupper(str[i]))
               break;

         if (i < (int) strlen(str)) {
            printf
                ("Error: Due to a limitation in HBOOK, directoy names may not contain uppercase\n");
            printf("       characters. Histogram loading from %s will not work.\n", str);
         } else {
            f = fopen(str, "r");
            if (f != NULL) {
               fclose(f);
               printf("Loading previous online histos from %s\n", str);
               strcpy(str2, "A");
               HRGET(0, str, str2);

               /* fix wrongly booked N-tuples at ID 100000 */
               if (HEXIST(100000))
                  HDELET(100000);
            }
         }
      }
#endif                          /* HAVE_HBOOK */

#ifdef HAVE_ROOT
      printf("Loading previous online histos from %s\n", str);
      LoadRootHistograms(gManaHistsDir, str);
#endif
   }
}

/*------------------------------------------------------------------*/

void save_last_histos()
{
   char str[256];

   /* save online histos */
   strcpy(str, out_info.last_histo_filename);
   if (strchr(str, DIR_SEPARATOR) == NULL)
      add_data_dir(str, out_info.last_histo_filename);

   printf("Saving current online histos to %s\n", str);

#ifdef HAVE_HBOOK
   {
      int i;
      char str2[256];

      for (i = 0; i < (int) strlen(str); i++)
         if (isupper(str[i]))
            break;

      if (i < (int) strlen(str)) {
         printf
             ("Error: Due to a limitation in HBOOK, directoy names may not contain uppercase\n");
         printf("       characters. Histogram saving to %s will not work.\n", str);
      } else {
         strcpy(str2, "NT");
         HRPUT(0, str, str2);
      }
   }
#endif

#ifdef HAVE_ROOT
   SaveRootHistograms(gManaHistsDir, str);
#endif

}

/*------------------------------------------------------------------*/

INT loop_online()
{
   INT status = SUCCESS;
   DWORD last_time_loop, last_time_update, actual_time;
   int ch;

   printf("Running analyzer online. Stop with \"!\"\n");

   /* main loop */
   last_time_update = 0;
   last_time_loop = 0;

   do {
      /* calculate events per second */
      actual_time = ss_millitime();

      if (actual_time - last_time_update > 1000) {
         /* update statistics */
         update_stats();
         last_time_update = actual_time;

         /* check keyboard */
         ch = 0;
         while (ss_kbhit()) {
            ch = ss_getchar(0);
            if (ch == -1)
               ch = getchar();

            if ((char) ch == '!')
               break;
         }

         if ((char) ch == '!')
            break;
      }

      if (analyzer_loop_period == 0)
         status = cm_yield(1000);
      else {
         if (actual_time - last_time_loop > (DWORD) analyzer_loop_period) {
            last_time_loop = actual_time;
            analyzer_loop();
         }

         status = cm_yield(analyzer_loop_period);
      }

   } while (status != RPC_SHUTDOWN && status != SS_ABORT);

   /* update statistics */
   update_stats();

   return status;
}

/*------------------------------------------------------------------*/

INT init_module_parameters(BOOL bclose)
{
   INT i, j, status, size;
   ANA_MODULE **module;
   char str[80];
   HNDLE hkey;

   for (i = 0; analyze_request[i].event_name[0]; i++) {
      module = analyze_request[i].ana_module;
      for (j = 0; module != NULL && module[j] != NULL; j++) {
         if (module[j]->parameters != NULL) {
            sprintf(str, "/%s/Parameters/%s", analyzer_name, module[j]->name);

            if (bclose) {
               db_find_key(hDB, 0, str, &hkey);
               db_close_record(hDB, hkey);
            } else {
               status = db_find_key(hDB, 0, str, &hkey);
               if (status == DB_SUCCESS) {
                  db_get_record_size(hDB, hkey, 0, &size);
                  if (size != module[j]->param_size)
                     status = 0;
               }
               if (status != DB_SUCCESS && module[j]->init_str) {
                  if (db_check_record(hDB, 0, str, strcomb(module[j]->init_str), TRUE) !=
                      DB_SUCCESS) {
                     cm_msg(MERROR, "init_module_parameters",
                            "Cannot create/check \"%s\" parameters in ODB", str);
                     return 0;
                  }
               }

               db_find_key(hDB, 0, str, &hkey);
               assert(hkey);

               if (db_open_record(hDB, hkey, module[j]->parameters, module[j]->param_size,
                                  MODE_READ, NULL, NULL) != DB_SUCCESS) {
                  cm_msg(MERROR, "init_module_parameters",
                         "Cannot open \"%s\" parameters in ODB", str);
                  return 0;
               }
            }
         }
      }
   }

   return SUCCESS;
}

/*------------------------------------------------------------------*/

INT bevid_2_mheader(EVENT_HEADER * pevent, DWORD * pybos)
{
   INT status;
   DWORD bklen, bktyp, *pdata;
   YBOS_BANK_HEADER *pybk;

   /* check if EVID is present if so display its content */
   if ((status = ybk_find(pybos, "EVID", &bklen, &bktyp, (void **) &pybk)) == YB_SUCCESS) {
      pdata = (DWORD *) ((YBOS_BANK_HEADER *) pybk + 1);
      if (clp.verbose) {
         printf("--------- EVID --------- Event# %i ------Run#:%i--------\n",
                (int) (YBOS_EVID_EVENT_NB(pdata)), (int) (YBOS_EVID_RUN_NUMBER(pdata)));
         printf("Evid:%4.4x- Mask:%4.4x- Serial:%i- Time:0x%x- Dsize:%i/0x%x",
                (int) YBOS_EVID_EVENT_ID(pdata), (int) YBOS_EVID_TRIGGER_MASK(pdata)
                , (int) YBOS_EVID_SERIAL(pdata), (int) YBOS_EVID_TIME(pdata)
                , (int) pybk->length, (int) pybk->length);
      }
      pevent->event_id = YBOS_EVID_EVENT_ID(pdata);
      pevent->trigger_mask = YBOS_EVID_TRIGGER_MASK(pdata);
      pevent->serial_number = YBOS_EVID_SERIAL(pdata);
      pevent->time_stamp = YBOS_EVID_TIME(pdata);
      pevent->data_size = pybk->length;
   }
   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/

void odb_load(EVENT_HEADER * pevent)
{
   BOOL flag;
   int size, i, status;
   char str[256];
   HNDLE hKey, hKeyRoot, hKeyEq;

   flag = TRUE;
   size = sizeof(flag);
   sprintf(str, "/%s/ODB Load", analyzer_name);
   db_get_value(hDB, 0, str, &flag, &size, TID_BOOL, TRUE);

   if (flag) {
      for (i = 0; i < 10; i++)
         if (clp.protect[i][0] && !clp.quiet)
            printf("Protect ODB tree \"%s\"\n", clp.protect[i]);

      if (!clp.quiet)
         printf("Load ODB from run %d...", (int) current_run_number);

      if (flag == 1) {
         /* lock all ODB values except run parameters */
         db_set_mode(hDB, 0, MODE_READ, TRUE);

         db_find_key(hDB, 0, "/Experiment/Run Parameters", &hKey);
         if (hKey)
            db_set_mode(hDB, hKey, MODE_READ | MODE_WRITE | MODE_DELETE, TRUE);

         /* and analyzer parameters */
         sprintf(str, "/%s/Parameters", analyzer_name);
         db_find_key(hDB, 0, str, &hKey);
         if (hKey)
            db_set_mode(hDB, hKey, MODE_READ | MODE_WRITE | MODE_DELETE, TRUE);

         /* and equipment (except /variables) */
         db_find_key(hDB, 0, "/Equipment", &hKeyRoot);
         if (hKeyRoot) {
            db_set_mode(hDB, hKeyRoot, MODE_READ | MODE_WRITE | MODE_DELETE, FALSE);

            for (i = 0;; i++) {
               status = db_enum_key(hDB, hKeyRoot, i, &hKeyEq);
               if (status == DB_NO_MORE_SUBKEYS)
                  break;

               db_set_mode(hDB, hKeyEq, MODE_READ | MODE_WRITE | MODE_DELETE, TRUE);

               db_find_key(hDB, hKeyEq, "Variables", &hKey);
               if (hKey)
                  db_set_mode(hDB, hKey, MODE_READ, TRUE);
            }
         }

         /* lock protected trees */
         for (i = 0; i < 10; i++)
            if (clp.protect[i][0]) {
               db_find_key(hDB, 0, clp.protect[i], &hKey);
               if (hKey)
                  db_set_mode(hDB, hKey, MODE_READ, TRUE);
            }
      }

      /* close open records to parameters */
      init_module_parameters(TRUE);

      db_paste(hDB, 0, (char *) (pevent + 1));

      if (flag == 1)
         db_set_mode(hDB, 0, MODE_READ | MODE_WRITE | MODE_DELETE, TRUE);

      /* reinit structured opened by user analyzer */
      analyzer_init();

      /* reload parameter files after BOR event */
      if (!clp.quiet)
         printf("OK\n");
      load_parameters(current_run_number);

      /* open module parameters again */
      init_module_parameters(FALSE);
   }
}

/*------------------------------------------------------------------*/

#define MA_DEVICE_DISK        1
#define MA_DEVICE_TAPE        2
#define MA_DEVICE_FTP         3
#define MA_DEVICE_PVM         4

#define MA_FORMAT_MIDAS       (1<<0)
#define MA_FORMAT_YBOS        (1<<2)
#define MA_FORMAT_GZIP        (1<<3)

typedef struct {
   char file_name[256];
   int format;
   int device;
   int fd;
   gzFile gzfile;
   char *buffer;
   int wp, rp;
   /*FTP_CON ftp_con; */
} MA_FILE;

/*------------------------------------------------------------------*/

MA_FILE *ma_open(char *file_name)
{
   char *ext_str;
   int status;
   MA_FILE *file;

   /* allocate MA_FILE structure */
   file = (MA_FILE *) calloc(sizeof(MA_FILE), 1);
   if (file == NULL) {
      cm_msg(MERROR, "open_input_file", "Cannot allocate MA file structure");
      return NULL;
   }

   /* save file name */
   strcpy(file->file_name, file_name);

   /* for now, just read from disk */
   file->device = MA_DEVICE_DISK;

   /* or from PVM */
   if (pvm_slave) {
      file->device = MA_DEVICE_PVM;
      file->buffer = (char *) malloc(PVM_BUFFER_SIZE);
      file->wp = file->rp = 0;
   }

   /* check input file extension */
   if (strchr(file_name, '.')) {
      ext_str = file_name + strlen(file_name) - 1;
      while (*ext_str != '.')
         ext_str--;
   } else
      ext_str = "";

   if (strncmp(ext_str, ".gz", 3) == 0) {
      ext_str--;
      while (*ext_str != '.' && ext_str > file_name)
         ext_str--;
   }

   if (strncmp(file_name, "/dev/", 4) == 0)     /* assume MIDAS tape */
      file->format = MA_FORMAT_MIDAS;
   else if (strncmp(ext_str, ".mid", 4) == 0)
      file->format = MA_FORMAT_MIDAS;
   else if (strncmp(ext_str, ".ybs", 4) == 0)
      file->format = MA_FORMAT_YBOS;
   else {
      printf
          ("Unknown input data format \"%s\". Please use file extension .mid or mid.gz.\n",
           ext_str);
      return NULL;
   }

   if (file->device == MA_DEVICE_DISK) {
      if (file->format == MA_FORMAT_YBOS) {
         status = yb_any_file_ropen(file_name, FORMAT_YBOS);
         if (status != SS_SUCCESS)
            return NULL;
         if (yb_any_physrec_skip(FORMAT_YBOS, -1) != YB_SUCCESS)
            return (NULL);
      } else {
         file->gzfile = gzopen(file_name, "rb");
         if (file->gzfile == NULL)
            return NULL;
      }
   }

   return file;
}

/*------------------------------------------------------------------*/

int ma_close(MA_FILE * file)
{
   if (file->format == MA_FORMAT_YBOS)
      yb_any_file_rclose(FORMAT_YBOS);
   else
      gzclose(file->gzfile);

   free(file);
   return SUCCESS;
}

/*------------------------------------------------------------------*/

int ma_read_event(MA_FILE * file, EVENT_HEADER * pevent, int size)
{
   int status, n;

   if (file->device == MA_DEVICE_DISK) {
      if (file->format == MA_FORMAT_MIDAS) {
         if (size < (int) sizeof(EVENT_HEADER)) {
            cm_msg(MERROR, "ma_read_event", "Buffer size too small");
            return -1;
         }

         /* read event header */
         n = gzread(file->gzfile, pevent, sizeof(EVENT_HEADER));
         if (n < (int) sizeof(EVENT_HEADER)) {
            if (n > 0)
               printf("Unexpected end of file %s, last event skipped\n", file->file_name);
            return -1;
         }

         /* swap event header if in wrong format */
         if (pevent->serial_number > 0x1000000) {
            WORD_SWAP(&pevent->event_id);
            WORD_SWAP(&pevent->trigger_mask);
            DWORD_SWAP(&pevent->serial_number);
            DWORD_SWAP(&pevent->time_stamp);
            DWORD_SWAP(&pevent->data_size);
         }

         /* read event */
         n = 0;
         if (pevent->data_size > 0) {
            if (size < (int) pevent->data_size + (int) sizeof(EVENT_HEADER)) {
               cm_msg(MERROR, "ma_read_event", "Buffer size too small");
               return -1;
            }

            n = gzread(file->gzfile, pevent + 1, pevent->data_size);
            if (n != (INT) pevent->data_size) {
               printf("Unexpected end of file %s, last event skipped\n", file->file_name);
               return -1;
            }
         }

         return n + sizeof(EVENT_HEADER);
      } else if (file->format == MA_FORMAT_YBOS) {
         DWORD *pybos, readn;
         if (ybos_event_get(&pybos, &readn) != SS_SUCCESS)
            return -1;
         status = yb_any_event_swap(FORMAT_YBOS, pybos);
         memcpy((char *) (pevent + 1), (char *) pybos, readn);
         status = bevid_2_mheader(pevent, pybos);
         return readn;
      }
   } else if (file->device == MA_DEVICE_PVM) {
#ifdef HAVE_PVM
      int bufid, len, tag, tid;
      EVENT_HEADER *pe;
      struct timeval timeout;

      /* check if anything in buffer */
      if (file->wp > file->rp) {
         pe = (EVENT_HEADER *) (file->buffer + file->rp);
         size = sizeof(EVENT_HEADER) + pe->data_size;
         memcpy(pevent, pe, size);
         file->rp += size;
         return size;
      }

      /* send data request */
      pvm_initsend(PvmDataInPlace);
      pvm_send(pvm_myparent, TAG_DATA);

      /* receive data */
      timeout.tv_sec = 60;
      timeout.tv_usec = 0;

      bufid = pvm_trecv(-1, -1, &timeout);
      if (bufid < 0) {
         pvm_perror("pvm_recv");
         return -1;
      }
      if (bufid == 0) {
         PVM_DEBUG("ma_read_event: timeout receiving data, aborting analyzer.\n");
         return -1;
      }

      status = pvm_bufinfo(bufid, &len, &tag, &tid);
      if (status < 0) {
         pvm_perror("pvm_bufinfo");
         return -1;
      }

      PVM_DEBUG("ma_read_event: receive tag %d, buflen %d", tag, len);

      if (tag == TAG_EOR || tag == TAG_EXIT)
         return -1;

      file->wp = len;
      file->rp = 0;
      status = pvm_upkbyte((char *) file->buffer, len, 1);
      if (status < 0) {
         pvm_perror("pvm_upkbyte");
         return -1;
      }

      /* no new data available, sleep some time to reduce network traffic */
      if (len == 0)
         ss_sleep(200);

      /* re-call this function */
      return ma_read_event(file, pevent, size);
#endif
   }

   return 0;
}

/*------------------------------------------------------------------*/

INT analyze_run(INT run_number, char *input_file_name, char *output_file_name)
{
   EVENT_HEADER *pevent, *pevent_unaligned;
   ANALYZE_REQUEST *par;
   INT i, n, size;
   DWORD num_events_in, num_events_out;
   char error[256], str[256];
   INT status = SUCCESS;
   MA_FILE *file;
   BOOL skip;
   DWORD start_time;

   /* set output file name and flags in ODB */
   sprintf(str, "/%s/Output/Filename", analyzer_name);
   db_set_value(hDB, 0, str, output_file_name, 256, 1, TID_STRING);
#ifdef HAVE_HBOOK
   sprintf(str, "/%s/Output/RWNT", analyzer_name);
   db_set_value(hDB, 0, str, &clp.rwnt, sizeof(BOOL), 1, TID_BOOL);
#endif

   cm_msg(MINFO, "analyze_run", "Set run number %d in ODB", run_number);
   //assert(run_number > 0);

   /* set run number in ODB */
   status =
       db_set_value(hDB, 0, "/Runinfo/Run number", &run_number, sizeof(run_number), 1,
                    TID_INT);
   assert(status == SUCCESS);

   /* set file name in out_info */
   strcpy(out_info.filename, output_file_name);

   /* let changes propagate to modules */
   cm_yield(0);

   /* open input file, will be changed to ma_open_file later... */
   file = ma_open(input_file_name);
   if (file == NULL) {
      printf("Cannot open input file \"%s\"\n", input_file_name);
      return -1;
   }

   pevent_unaligned = (EVENT_HEADER *) malloc(EXT_EVENT_SIZE);
   if (pevent_unaligned == NULL) {
      printf("Not enough memeory\n");
      return -1;
   }
   pevent = (EVENT_HEADER *) ALIGN8((PTYPE) pevent_unaligned);

   /* call analyzer bor routines */
   bor(run_number, error);

   num_events_in = num_events_out = 0;

   start_time = ss_millitime();

   /* event loop */
   do {
      /* read next event */
      memset(pevent_unaligned, 0, EXT_EVENT_SIZE);
      n = ma_read_event(file, pevent, EXT_EVENT_SIZE);
      if (n <= 0)
         break;

      num_events_in++;

      /* copy system events (BOR, EOR, MESSAGE) to output file */
      if (pevent->event_id < 0) {
         status = process_event(NULL, pevent);
         if (status < 0 || status == RPC_SHUTDOWN)      /* disk full/stop analyzer */
            break;

         if (out_file && out_format == FORMAT_MIDAS) {
            size = pevent->data_size + sizeof(EVENT_HEADER);
            if (out_gzip)
               status = gzwrite(out_file, pevent, size) == size ? SUCCESS : SS_FILE_ERROR;
            else
               status =
                   fwrite(pevent, 1, size,
                          out_file) == (size_t) size ? SUCCESS : SS_FILE_ERROR;

            if (status != SUCCESS) {
               cm_msg(MERROR, "analyze_run", "Error writing to file (Disk full?)");
               return -1;
            }

            num_events_out++;
         }

         /* reinit start time after BOR event */
         if (pevent->event_id == EVENTID_BOR)
            start_time = ss_millitime();
      }

      /* check if event is in event limit */
      skip = FALSE;

      if (!pvm_slave) {
         if (clp.n[0] > 0 || clp.n[1] > 0) {
            if (clp.n[1] == 0) {
               /* treat n[0] as upper limit */
               if (num_events_in > clp.n[0]) {
                  num_events_in--;
                  status = SUCCESS;
                  break;
               }
            } else {
               if (num_events_in > clp.n[1]) {
                  status = SUCCESS;
                  break;
               }
               if (num_events_in < clp.n[0])
                  skip = TRUE;
               else if (clp.n[2] > 0 && num_events_in % clp.n[2] != 0)
                  skip = TRUE;
            }
         }
      }

      if (!skip) {
         /* find request belonging to this event */
         par = analyze_request;
         status = SUCCESS;
         for (i = 0; par->event_name[0]; par++)
            if ((par->ar_info.event_id == EVENTID_ALL ||
                 par->ar_info.event_id == pevent->event_id) &&
                (par->ar_info.trigger_mask == TRIGGER_ALL ||
                 (par->ar_info.trigger_mask & pevent->trigger_mask)) &&
                par->ar_info.enabled) {
               /* analyze this event */
               status = process_event(par, pevent);
               if (status == SUCCESS)
                  num_events_out++;
               if (status < 0 || status == RPC_SHUTDOWN)        /* disk full/stop analyzer */
                  break;

               /* check for Ctrl-C */
               status = cm_yield(0);
            }
         if (status < 0 || status == RPC_SHUTDOWN)
            break;
      }

      /* update ODB statistics once every 100 events */
      if (num_events_in % 100 == 0) {
         update_stats();
         if (!clp.quiet) {
            if (out_file)
               printf("%s:%d  %s:%d  events\r", input_file_name, (int) num_events_in,
                      out_info.filename, (int) num_events_out);
            else
               printf("%s:%d  events\r", input_file_name, (int) num_events_in);

#ifndef OS_WINNT
            fflush(stdout);
#endif
         }
      }
   } while (1);

#ifdef HAVE_PVM
   PVM_DEBUG("analyze_run: event loop finished, status = %d", status);
#endif

   /* signal EOR to slaves */
#ifdef HAVE_PVM
   if (pvm_master) {
      if (status == RPC_SHUTDOWN)
         printf("\nShutting down distributed analyzers, please wait...\n");
      pvm_eor(status == RPC_SHUTDOWN ? TAG_EXIT : TAG_EOR);

      /* merge slave output files */
      if (out_info.filename[0] && !out_append)
         status = pvm_merge();

      start_time = ss_millitime() - start_time;

      update_stats();
      if (!clp.quiet) {
         if (out_file)
            printf("%s:%d  %s:%d  events, %1.2lfs\n", input_file_name, num_events_in,
                   out_info.filename, num_events_out, start_time / 1000.0);
         else
            printf("%s:%d  events, %1.2lfs\n", input_file_name, num_events_in,
                   start_time / 1000.0);
      }
   } else if (pvm_slave) {
      start_time = ss_millitime() - start_time;

      update_stats();
      if (!clp.quiet) {
         if (out_file)
            printf("%s:%d  %s:%d  events, %1.2lfs\n", input_file_name, num_events_in,
                   out_info.filename, num_events_out, start_time / 1000.0);
         else
            printf("%s:%d  events, %1.2lfs\n", input_file_name, num_events_in,
                   start_time / 1000.0);
      }

      eor(current_run_number, error);

      /* send back tests */
      pvm_initsend(PvmDataInPlace);

      for (i = 0; i < n_test; i++)
         pvm_pkbyte((char *) tl[i], sizeof(ANA_TEST), 1);

      PVM_DEBUG("analyze_run: send %d tests back to master", n_test);

      status = pvm_send(pvm_myparent, TAG_EOR);
      if (status < 0) {
         pvm_perror("pvm_send");
         return RPC_SHUTDOWN;
      }
   } else {
      start_time = ss_millitime() - start_time;

      update_stats();
      if (!clp.quiet) {
         if (out_file)
            printf("%s:%d  %s:%d  events, %1.2lfs\n", input_file_name, num_events_in,
                   out_info.filename, num_events_out, start_time / 1000.0);
         else
            printf("%s:%d  events, %1.2lfs\n", input_file_name, num_events_in,
                   start_time / 1000.0);
      }

      /* call analyzer eor routines */
      eor(current_run_number, error);
   }
#else

   start_time = ss_millitime() - start_time;

   update_stats();
   if (!clp.quiet) {
      if (out_file)
         printf("%s:%d  %s:%d  events, %1.2lfs\n", input_file_name, (int) num_events_in,
                out_info.filename, (int) num_events_out, start_time / 1000.0);
      else
         printf("%s:%d  events, %1.2lfs\n", input_file_name, (int) num_events_in,
                start_time / 1000.0);
   }

   /* call analyzer eor routines */
   eor(current_run_number, error);

#endif

   ma_close(file);

   free(pevent_unaligned);

   return status;
}

/*------------------------------------------------------------------*/

INT loop_runs_offline()
{
   INT i, status, run_number;
   char input_file_name[256], output_file_name[256], *prn;
   BANK_LIST *bank_list;

   if (!clp.quiet)
      printf("Running analyzer offline. Stop with \"!\"\n");

   run_number = 0;
   out_append = ((strchr(clp.input_file_name[0], '%') != NULL) &&
                 (strchr(clp.output_file_name, '%') == NULL)) ||
       clp.input_file_name[1][0];

   /* loop over range of files */
   if (clp.run_number[0] > 0) {
      if (strchr(clp.input_file_name[0], '%') == NULL) {
         printf
             ("Input file name must contain a wildcard like \"%%05d\" when using a range.\n");
         return 0;
      }

      if (clp.run_number[0] == 0) {
         printf("End of range not specified.\n");
         return 0;
      }

      for (run_number = clp.run_number[0]; run_number <= clp.run_number[1]; run_number++) {
         sprintf(input_file_name, clp.input_file_name[0], run_number);
         if (strchr(clp.output_file_name, '%') != NULL)
            sprintf(output_file_name, clp.output_file_name, run_number);
         else
            strcpy(output_file_name, clp.output_file_name);

         status = analyze_run(run_number, input_file_name, output_file_name);
         if (status == RPC_SHUTDOWN)
            break;
      }
   } else {
      /* loop over input file names */
      for (i = 0; clp.input_file_name[i][0] && i < 10; i++) {
         strcpy(input_file_name, clp.input_file_name[i]);

         /* get run number from input file */
         prn = input_file_name;
         while (strchr(prn, DIR_SEPARATOR) != NULL)
            prn = strchr(prn, DIR_SEPARATOR) + 1;

         if (strpbrk(prn, "0123456789"))
            run_number = atoi(strpbrk(prn, "0123456789"));

         if (strchr(clp.output_file_name, '%') != NULL) {
            if (run_number == 0) {
               printf("Cannot extract run number from input file name.\n");
               return 0;
            }
            sprintf(output_file_name, clp.output_file_name, run_number);
         } else
            strcpy(output_file_name, clp.output_file_name);

         status = analyze_run(run_number, input_file_name, output_file_name);
         if (status == RPC_SHUTDOWN)
            break;
      }
   }

   /* close output file in append mode */
   if (out_file && out_append) {
      if (out_format == FORMAT_HBOOK) {
#ifdef HAVE_HBOOK
         char str[80];

         HROUT(0, i, bstr);
         strcpy(str, "OFFLINE");
         HREND(str);
#else
         cm_msg(MERROR, "loop_runs_offline", "HBOOK support is not compiled in");
#endif
      } else if (out_format == FORMAT_ROOT) {
#ifdef HAVE_ROOT
         CloseRootOutputFile();
#else
         cm_msg(MERROR, "loop_runs_offline", "ROOT support is not compiled in");
#endif                          /* HAVE_ROOT */
      } else {
         if (out_gzip)
            gzclose(out_file);
         else
            fclose(out_file);
      }

      /* free bank buffer */
      for (i = 0; analyze_request[i].event_name[0]; i++) {
         bank_list = analyze_request[i].bank_list;

         if (bank_list == NULL)
            continue;

         for (; bank_list->name[0]; bank_list++)
            if (bank_list->addr) {
               free(bank_list->addr);
               bank_list->addr = NULL;
            }
      }
   }
#ifdef HAVE_PVM
   /* merge slave output files */
   if (pvm_master && out_info.filename[0] && out_append)
      pvm_merge();
#endif

   return CM_SUCCESS;
}

/*------------------------------------------------------------------*/

#ifdef HAVE_PVM

int pvm_main(char *argv[])
{
   int mytid, status, i, j, dtid, *pvm_tid, bufid;
   char path[256];
   struct timeval timeout;

   getcwd(path, 256);

   mytid = pvm_mytid();
   if (mytid < 0) {
      pvm_perror("pvm_mytid");
      return 0;
   }

   chdir(path);

   status = pvm_setopt(PvmRoute, PvmRouteDirect);
   if (status < 0) {
      pvm_perror("pvm_setopt");
      pvm_exit();
      return 0;
   }

   pvm_myparent = pvm_parent();
   if (pvm_myparent < 0 && pvm_myparent != PvmNoParent) {
      pvm_perror("pvm_parent");
      pvm_exit();
      return 0;
   }

   /* check if master */
   if (pvm_myparent == PvmNoParent) {
      struct pvmhostinfo *hostp;
      int n_host, n_arch;

#ifdef WIN32
      char *p;

      /* use no path, executable must be under $PVM_ROOT$/bin/WIN32 */
      p = argv[0] + strlen(argv[0]) - 1;
      while (p > argv[0] && *p != '\\')
         p--;
      if (*p == '\\')
         p++;
      strcpy(path, p);
#else
      if (strchr(argv[0], '/') == 0) {
         getcwd(path, 256);
         strcat(path, "/");
         strcat(path, argv[0]);
      } else
         strcpy(path, argv[0]);
#endif

      /* return if no parallelization selected */
      if (clp.n_task == -1)
         return SUCCESS;

      /* Set number of slaves to start */
      pvm_config(&n_host, &n_arch, &hostp);

      pvm_n_task = n_host - 1;
      if (clp.n_task != 0)
         pvm_n_task = clp.n_task;

      if (clp.n_task != 1 && pvm_n_task > n_host - 1)
         pvm_n_task = n_host - 1;

      if (pvm_n_task == 0)
         return SUCCESS;

      pvm_master = TRUE;

      pvm_tid = malloc(sizeof(int) * pvm_n_task);
      pvmc = malloc(sizeof(PVM_CLIENT) * pvm_n_task);

      if (pvm_tid == NULL || pvmc == NULL) {
         free(pvm_tid);
         printf("Not enough memory to allocate PVM structures.\n");
         pvm_exit();
         return 0;
      }

      memset(pvmc, 0, sizeof(PVM_CLIENT) * pvm_n_task);

      for (i = 0; i < pvm_n_task; i++) {
         pvmc[i].buffer = malloc(PVM_BUFFER_SIZE);
         if (pvmc[i].buffer == NULL) {
            free(pvm_tid);
            free(pvmc);
            printf("Not enough memory to allocate PVM buffers.\n");
            pvm_exit();
            return 0;
         }
      }

      /* spawn slaves */
      printf("Parallelizing analyzer on %d machines\n", pvm_n_task);
      if (pvm_n_task == 1)
         status = pvm_spawn(path, argv + 1, PvmTaskDefault, NULL, pvm_n_task, pvm_tid);
      else
         status =
             pvm_spawn(path, argv + 1, PvmTaskHost | PvmHostCompl, ".", pvm_n_task,
                       pvm_tid);
      if (status == 0) {
         pvm_perror("pvm_spawn");
         pvm_exit();
         free(pvm_tid);
         free(pvmc);
         return 0;
      }

      for (i = 0; i < pvm_n_task; i++) {
         pvmc[i].tid = pvm_tid[i];
         pvmc[i].wp = 0;
         pvmc[i].n_events = 0;
         pvmc[i].time = 0;
         dtid = pvm_tidtohost(pvm_tid[i]);
         for (j = 0; j < n_host; j++)
            if (dtid == hostp[j].hi_tid)
               strcpy(pvmc[i].host, hostp[j].hi_name);
      }

      PVM_DEBUG("Spawing on hosts:");
      for (i = 0; i < pvm_n_task; i++)
         PVM_DEBUG("%s", pvmc[i].host);

      if (status < pvm_n_task) {
         printf("Trouble spawning slaves. Aborting. Error codes are:\n");
         for (i = 0; i < pvm_n_task; i++)
            printf("TID %d %d\n", i, pvm_tid[i]);

         pvm_exit();
         free(pvm_tid);
         free(pvmc);
         return 0;
      }

      /* send slave index */
      for (i = 0; i < pvm_n_task; i++) {
         pvm_initsend(PvmDataDefault);

         pvm_pkint(&i, 1, 1);

         PVM_DEBUG("pvm_main: send index to client %d", i);

         status = pvm_send(pvmc[i].tid, TAG_INIT);
         if (status < 0) {
            pvm_perror("pvm_send");
            return 0;
         }
      }

      free(pvm_tid);
   } else {
      char path[256];

      pvm_master = FALSE;
      pvm_slave = TRUE;

      /* go to path from argv[0] */
      strcpy(path, argv[0]);
      for (i = strlen(path); i > 0 && path[i] != DIR_SEPARATOR; i--)
         path[i] = 0;
      if (i > 0)
         path[i] = 0;

      chdir(path);
      PVM_DEBUG("PATH=%s", path);

      /* receive slave index */
      timeout.tv_sec = 10;
      timeout.tv_usec = 0;

      bufid = pvm_trecv(-1, -1, &timeout);
      if (bufid < 0) {
         pvm_perror("pvm_recv");
         return 0;
      }
      if (bufid == 0) {
         PVM_DEBUG("pvm_main: timeout receiving index, aborting analyzer.\n");
         return 0;
      }

      status = pvm_upkint(&pvm_client_index, 1, 1);
      if (status < 0) {
         pvm_perror("pvm_upkint");
         return 0;
      }

      PVM_DEBUG("Received client ID %d", pvm_client_index);
   }

   return SUCCESS;
}

/*------------------------------------------------------------------*/

int pvm_send_event(int index, EVENT_HEADER * pevent)
{
   struct timeval timeout;
   int bufid, len, tag, tid, status;

   if (pevent->data_size + sizeof(EVENT_HEADER) >= PVM_BUFFER_SIZE) {
      printf("Event too large (%d) for PVM buffer (%d), analyzer aborted\n",
             pevent->data_size + sizeof(EVENT_HEADER), PVM_BUFFER_SIZE);
      return RPC_SHUTDOWN;
   }

   /* wait on event request */
   timeout.tv_sec = 60;
   timeout.tv_usec = 0;

   bufid = pvm_trecv(pvmc[index].tid, -1, &timeout);
   if (bufid < 0) {
      pvm_perror("pvm_recv");
      return RPC_SHUTDOWN;
   }
   if (bufid == 0) {
      printf("Timeout receiving data requests from %s, aborting analyzer.\n",
             pvmc[index].host);
      return RPC_SHUTDOWN;
   }

   status = pvm_bufinfo(bufid, &len, &tag, &tid);
   if (status < 0) {
      pvm_perror("pvm_bufinfo");
      return RPC_SHUTDOWN;
   }

   PVM_DEBUG("pvm_send_event: received request from client %d", index);

   /* send event */
   pvm_initsend(PvmDataInPlace);

   pvm_pkbyte((char *) pevent, pevent->data_size + sizeof(EVENT_HEADER), 1);

   PVM_DEBUG("pvm_send_event: send events to client %d", index);

   status = pvm_send(tid, TAG_DATA);
   if (status < 0) {
      pvm_perror("pvm_send");
      return RPC_SHUTDOWN;
   }

   pvmc[index].time = ss_millitime();

   return SUCCESS;
}

/*------------------------------------------------------------------*/

int pvm_send_buffer(int index)
{
   struct timeval timeout;
   int i, bufid, len, tag, tid, status;

   PVM_DEBUG("pvm_send_buffer: index %d", index);

   if (index == -2) {
      bufid = pvm_nrecv(-1, -1);
   } else {
      /* wait on event request with timeout */
      timeout.tv_sec = 60;
      timeout.tv_usec = 0;

      bufid = pvm_trecv(-1, -1, &timeout);
   }

   if (bufid < 0) {
      pvm_perror("pvm_recv");
      return RPC_SHUTDOWN;
   }
   if (bufid == 0) {
      if (index == -2)
         return SUCCESS;

      printf("Timeout receiving data requests, aborting analyzer.\n");
      return RPC_SHUTDOWN;
   }

   status = pvm_bufinfo(bufid, &len, &tag, &tid);
   if (status < 0) {
      pvm_perror("pvm_bufinfo");
      return RPC_SHUTDOWN;
   }

   /* find index of that client */
   for (i = 0; i < pvm_n_task; i++)
      if (pvmc[i].tid == tid)
         break;

   if (i == pvm_n_task) {
      cm_msg(MERROR, "pvm_send_buffer", "received message from unknown client %d", tid);
      return RPC_SHUTDOWN;
   }

   PVM_DEBUG("pvm_send_buffer: received request from client %d", i);

   /* send event */
   pvm_initsend(PvmDataInPlace);

   pvm_pkbyte((char *) pvmc[i].buffer, pvmc[i].wp, 1);

   PVM_DEBUG("pvm_send_buffer: send %d events (%1.1lfkB) to client %d",
             pvmc[i].n_events, pvmc[i].wp / 1024.0, i);

   status = pvm_send(tid, TAG_DATA);
   if (status < 0) {
      pvm_perror("pvm_send");
      return RPC_SHUTDOWN;
   }

   pvmc[i].wp = 0;
   pvmc[i].n_events = 0;
   pvmc[i].time = ss_millitime();

   /* if specific client is requested and not emptied, try again */
   if (index >= 0 && index != i)
      return pvm_send_buffer(index);

   return SUCCESS;
}

/*------------------------------------------------------------------*/

int pvm_distribute(ANALYZE_REQUEST * par, EVENT_HEADER * pevent)
{
   char str[256];
   int i, index, size, status, min, max;

   if (par == NULL && pevent->event_id == EVENTID_BOR) {
      /* distribute ODB dump */
      for (i = 0; i < pvm_n_task; i++) {
         status = pvm_send_event(i, pevent);
         if (status != SUCCESS)
            return status;
      }

      return SUCCESS;
   }

   size = sizeof(EVENT_HEADER) + pevent->data_size;

   if (par == NULL || (par->ar_info.sampling_type & GET_FARM) == 0) {
      /* if not farmed, copy to all client buffers */
      for (i = 0; i < pvm_n_task; i++) {
         /* if buffer full, empty it */
         if (pvmc[i].wp + size >= clp.pvm_buf_size) {
            status = pvm_send_buffer(i);
            if (status != SUCCESS)
               return status;
         }

         if (size >= PVM_BUFFER_SIZE) {
            printf("Event too large (%d) for PVM buffer (%d), analyzer aborted\n",
                   size, PVM_BUFFER_SIZE);
            return RPC_SHUTDOWN;
         }

         memcpy(pvmc[i].buffer + pvmc[i].wp, pevent, size);
         pvmc[i].wp += size;
         pvmc[i].n_events++;
      }
   } else {
      /* farmed: look for buffer with lowest level */
      min = PVM_BUFFER_SIZE;
      index = 0;
      for (i = 0; i < pvm_n_task; i++)
         if (pvmc[i].wp < min) {
            min = pvmc[i].wp;
            index = i;
         }

      /* if buffer full, empty it */
      if (pvmc[index].wp + size >= clp.pvm_buf_size) {
         status = pvm_send_buffer(index);
         if (status != SUCCESS)
            return status;
      }

      if (pvmc[index].wp + size >= PVM_BUFFER_SIZE) {
         printf("Event too large (%d) for PVM buffer (%d), analyzer aborted\n",
                size, PVM_BUFFER_SIZE);
         return RPC_SHUTDOWN;
      }

      /* copy to "index" buffer */
      memcpy(pvmc[index].buffer + pvmc[index].wp, pevent, size);
      pvmc[index].wp += size;
      pvmc[index].n_events++;
   }

   sprintf(str, "%1.3lf:  ", (ss_millitime() - pvm_start_time) / 1000.0);
   for (i = 0; i < pvm_n_task; i++)
      if (i == index)
         sprintf(str + strlen(str), "#%d# ", pvmc[i].wp);
      else
         sprintf(str + strlen(str), "%d ", pvmc[i].wp);
   //PVM_DEBUG(str);

   /* find min/max buffer level */
   min = PVM_BUFFER_SIZE;
   max = 0;
   for (i = 0; i < pvm_n_task; i++) {
      if (pvmc[i].wp > max)
         max = pvmc[i].wp;
      if (pvmc[i].wp < min)
         min = pvmc[i].wp;
   }

   /* don't send events if all buffers are less than half full */
   if (max < clp.pvm_buf_size / 2)
      return SUCCESS;

   /* if all buffer are more than half full, wait for next request */
   if (min > clp.pvm_buf_size / 2) {
      status = pvm_send_buffer(-1);
      return status;
   }

   /* probe new requests */
   status = pvm_send_buffer(-2);

   return status;
}

/*------------------------------------------------------------------*/

int pvm_eor(int eor_tag)
{
   struct timeval timeout;
   int bufid, len, tag, tid, i, j, status, size;
   ANA_TEST *tst_buf;
   DWORD count;
   char str[256];

   printf("\n");

   for (i = 0; i < pvm_n_task; i++)
      pvmc[i].eor_sent = FALSE;

   do {
      /* flush remaining buffers */
      timeout.tv_sec = 60;
      timeout.tv_usec = 0;

      bufid = pvm_trecv(-1, -1, &timeout);
      if (bufid < 0) {
         pvm_perror("pvm_recv");
         return RPC_SHUTDOWN;
      }
      if (bufid == 0) {
         printf("Timeout receiving data request, aborting analyzer.\n");
         return RPC_SHUTDOWN;
      }

      status = pvm_bufinfo(bufid, &len, &tag, &tid);
      if (status < 0) {
         pvm_perror("pvm_bufinfo");
         return RPC_SHUTDOWN;
      }

      /* find index of that client */
      for (j = 0; j < pvm_n_task; j++)
         if (pvmc[j].tid == tid)
            break;

      if (j == pvm_n_task) {
         cm_msg(MERROR, "pvm_eor", "received message from unknown client %d", tid);
         return RPC_SHUTDOWN;
      }

      PVM_DEBUG("pvm_eor: received request from client %d", j);

      /* send remaining buffer if data available */
      if (eor_tag == TAG_EOR && pvmc[j].wp > 0) {
         pvm_initsend(PvmDataInPlace);

         pvm_pkbyte((char *) pvmc[j].buffer, pvmc[j].wp, 1);

         PVM_DEBUG("pvm_eor: send %d events (%1.1lfkB) to client %d",
                   pvmc[j].n_events, pvmc[j].wp / 1024.0, j);

         status = pvm_send(tid, TAG_DATA);
         if (status < 0) {
            pvm_perror("pvm_send");
            return RPC_SHUTDOWN;
         }

         pvmc[j].wp = 0;
         pvmc[j].n_events = 0;
         pvmc[j].time = ss_millitime();
      } else {
         /* send EOR */
         pvm_initsend(PvmDataDefault);

         if (eor_tag == TAG_EOR)
            PVM_DEBUG("pvm_eor: send EOR to client %d", j);
         else
            PVM_DEBUG("pvm_eor: send EXIT to client %d", j);

         printf("Shutting down %s               \r", pvmc[j].host);
         fflush(stdout);

         status = pvm_send(tid, eor_tag);
         if (status < 0) {
            pvm_perror("pvm_send");
            return RPC_SHUTDOWN;
         }

         pvmc[j].eor_sent = TRUE;

         /* wait for EOR reply */
         timeout.tv_sec = 60;
         timeout.tv_usec = 0;

         bufid = pvm_trecv(tid, -1, &timeout);
         if (bufid < 0) {
            pvm_perror("pvm_recv");
            return RPC_SHUTDOWN;
         }
         if (bufid == 0) {
            printf("Timeout receiving EOR request, aborting analyzer.\n");
            return RPC_SHUTDOWN;
         }

         status = pvm_bufinfo(bufid, &len, &tag, &tid);
         if (status < 0) {
            pvm_perror("pvm_bufinfo");
            return RPC_SHUTDOWN;
         }

         PVM_DEBUG("\nGot %d bytes", len);

         tst_buf = malloc(len);
         pvm_upkbyte((char *) tst_buf, len, 1);

         /* write tests to ODB */
         for (i = 0; i < (int) (len / sizeof(ANA_TEST)); i++) {
            sprintf(str, "/%s/Tests/%s", analyzer_name, tst_buf[i].name);
            count = 0;
            size = sizeof(DWORD);
            db_get_value(hDB, 0, str, &count, &size, TID_DWORD, TRUE);
            count += tst_buf[i].count;

            db_set_value(hDB, 0, str, &count, sizeof(DWORD), 1, TID_DWORD);
         }

         free(tst_buf);
      }

      /* check if EOR sent to all clients */
      for (j = 0; j < pvm_n_task; j++)
         if (!pvmc[j].eor_sent)
            break;

   } while (j < pvm_n_task);

   printf("\n");

   return SUCCESS;
}

/*------------------------------------------------------------------*/

int pvm_merge()
{
   int i, j;
   char fn[10][8], str[256], file_name[256], error[256];
   char ext[10], *p;

   strcpy(str, out_info.filename);
   if (strchr(str, '%') != NULL)
      sprintf(file_name, str, current_run_number);
   else
      strcpy(file_name, str);

   /* check output file extension */
   out_gzip = FALSE;
   ext[0] = 0;
   if (strchr(file_name, '.')) {
      p = file_name + strlen(file_name) - 1;
      while (*p != '.')
         p--;
      strcpy(ext, p);
   }
   if (strncmp(ext, ".gz", 3) == 0) {
      out_gzip = TRUE;
      *p = 0;
      p--;
      while (*p != '.' && p > file_name)
         p--;
      strcpy(ext, p);
   }

   if (strncmp(ext, ".asc", 4) == 0)
      out_format = FORMAT_ASCII;
   else if (strncmp(ext, ".mid", 4) == 0)
      out_format = FORMAT_MIDAS;
   else if (strncmp(ext, ".rz", 3) == 0)
      out_format = FORMAT_HBOOK;
   else {
      strcpy(error,
             "Unknown output data format. Please use file extension .asc, .mid or .rz.\n");
      cm_msg(MERROR, "pvm_merge", error);
      return 0;
   }

   if (out_format == FORMAT_HBOOK) {
      if (pvm_n_task <= 10) {
         for (i = 0; i < pvm_n_task; i++)
            sprintf(fn[i], "n%d.rz", i);

         HMERGE(pvm_n_task, fn, file_name);
      } else {
         for (i = 0; i <= pvm_n_task / 10; i++) {
            for (j = 0; j < 10 && j + i * 10 < pvm_n_task; j++)
               sprintf(fn[j], "n%d.rz", j + i * 10);

            sprintf(str, "t%d.rz", i);
            printf("Merging %d files to %s:\n", j, str);
            HMERGE(j, fn, str);
         }
         for (i = 0; i <= pvm_n_task / 10; i++)
            sprintf(fn[i], "t%d.rz", i);

         printf("Merging %d files to %s:\n", i, file_name);
         HMERGE(i, fn, file_name);
      }
   }

   return SUCCESS;
}

#endif                          /* PVM */

/*------------------------------------------------------------------*/

#ifdef HAVE_ROOT

void *server_thread(void *arg)
/*
  Server histograms to remove clients
*/
{
   int i;
   char str[256];

   TSocket *s = (TSocket *) arg;

   do {
      if (s->Recv(str, sizeof(str)) <= 0) {
         printf("Closed connection from %s\n", s->GetInetAddress().GetHostName());
         s->Close();
         delete s;
         return NULL;
      } else {
         printf("Received \"%s\"\n", str);

         TMessage *mess = new TMessage(kMESS_OBJECT);

         if (strcmp(str, "LIST") == 0) {
            /* build name array */

            TObjArray *names = new TObjArray(100);

            TIter next(gManaHistsDir->GetList());
            while (TObject * obj = next())
               if (obj->InheritsFrom("TH1"))
                  names->Add(new TObjString(obj->GetName()));

            /* send "names" array */
            mess->Reset();
            mess->WriteObject(names);
            s->Send(*mess);

            for (i = 0; i < names->GetLast() + 1; i++)
               delete(TObjString *) names->At(i);

            delete names;
         }

         else if (strncmp(str, "GET", 3) == 0) {
            /* search histo */
            TObject *obj;
            TIter next(gManaHistsDir->GetList());

            while ((obj = next())) {
               if (strcmp(str + 4, obj->GetName()) == 0)
                  break;
            }

            if (!obj) {
               s->Send("Error");
            } else {
               /* send single histo */
               mess->Reset();
               mess->WriteObject(obj);
               s->Send(*mess);
            }
         }

         else if (strncmp(str, "CLEAR", 5) == 0) {
            /* search histo */
            TObject *obj;
            TIter next(gManaHistsDir->GetList());

            while ((obj = next())) {
               if (strcmp(str + 6, obj->GetName()) == 0)
                  break;
            }

            if (!obj) {
               s->Send("Error");
            } else {
               /* clear histo */
#ifdef OS_LINUX
               TThread::Lock();
               ((TH1 *) obj)->Reset();
               TThread::UnLock();
#endif

               /* send single histo */
               s->Send("OK");
            }
         }

         delete mess;
      }

   } while (1);                 /* do forever */
}

/*------------------------------------------------------------------*/

void *root_server_loop(void *arg)
/*
   Server loop listening for incoming network connections on port
   specified by command line option -s. Starts a searver_thread for 
   each connection.
*/
{
   printf("Root server listening on port %d...\n", clp.root_port);
   TServerSocket *lsock = new TServerSocket(clp.root_port, kTRUE);

   do {
      TSocket *s = lsock->Accept();
      s->Send("RMSERV 1.0");
      printf("New connection from %s\n", s->GetInetAddress().GetHostName());

      /* start a new server thread */
#ifdef OS_LINUX
      TThread *th = new TThread("server_thread", server_thread, s);
      th->Run();
#endif
   } while (1);

   return NULL;
}

/*------------------------------------------------------------------*/

void *root_event_loop(void *arg)
/*
  Thread wrapper around main event loop
*/
{
   if (clp.online)
      loop_online();
   else
      loop_runs_offline();

   gSystem->ExitLoop();

   return NULL;
}

#endif                          /* HAVE_ROOT */

/*------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
   INT status, size;
   char str[256];
   HNDLE hkey;

   /* read in command line parameters into clp structure */
   status = getparam(argc, argv);
   if (status != CM_SUCCESS)
      return 1;
   /* Determine the run number from the input file - Kiburg 1/24/2008 */
   int myrunnumber=0;
   int mystrlen=0;
   char mystr[256];
   strcpy(mystr,clp.input_file_name[0]);
   mystrlen = strlen(mystr);
   if (mystrlen>8){ 
     char tmpstr[]={mystr[mystrlen-9], 
		    mystr[mystrlen-8],
		    mystr[mystrlen-7],
		    mystr[mystrlen-6], 
		    mystr[mystrlen-5]};
     myrunnumber=atoi(tmpstr);
     printf("This file indicates the run number is %d\n",myrunnumber);
   }
   /* myrunnumber now contains the current run number, or 0 if it was not poss */

#ifdef HAVE_PVM
   int i;

   str[0] = 0;
   for (i = 0; i < argc; i++) {
      strcat(str, argv[i]);
      strcat(str, " ");
   }
   PVM_DEBUG("Analyzer started: %s", str);
#endif

#ifdef HAVE_ROOT
   char **rargv;
   int rargc;

   /* copy first argument */
   rargc = 0;
   rargv = (char **) malloc(sizeof(char *) * 2);
   rargv[rargc] = (char *) malloc(strlen(argv[rargc]) + 1);
   strcpy(rargv[rargc], argv[rargc]);
   rargc++;

   /* append argument "-b" for batch mode without graphics, unless graphics */
   /* mode has been requested. */
   if(!clp.graphics) {
     rargv[rargc] = (char *) malloc(3);
     rargv[rargc++] = "-b";
   }
   manaApp = new TApplication("ranalyzer", &rargc, rargv);

   /* free argument memory */
   free(rargv[0]);
   if(rargc == 2) {
     free(rargv[1]);
   }
   free(rargv);

   /* default server port */
   clp.root_port = 9090;
#endif

   /* get default from environment */
   cm_get_environment(clp.host_name, sizeof(clp.host_name), clp.exp_name,
                      sizeof(clp.exp_name));

#ifdef HAVE_HBOOK
   /* set default lrec size */
   clp.lrec = HBOOK_LREC;
#endif                          /* HAVE_HBOOK */


   /* become a daemon */
   if (clp.daemon) {
      printf("Becoming a daemon...\n");
      clp.quiet = TRUE;
      ss_daemon_init(FALSE);
   }

  /* Don't run offline if MIDAS_DIR is not defined */ 
   if ( ! clp.online ) 
     { 
       if ( !getenv("MIDAS_DIR") ) 
         { 
           printf("For offline running the directory MIDAS_DIR must be defined\n"); 
           return 1; 
         } 
     } 
 
 
   /* set default buffer size */
   if (clp.pvm_buf_size == 0)
      clp.pvm_buf_size = 512 * 1024;
   else
      clp.pvm_buf_size *= 1024;
   if (clp.pvm_buf_size > PVM_BUFFER_SIZE) {
      printf("Buffer size cannot be larger than %dkB\n", PVM_BUFFER_SIZE / 1024);
      return 1;
   }

   /* set online mode if no input filename is given */
   clp.online = (clp.input_file_name[0][0] == 0);

#ifdef HAVE_HBOOK
   /* set Ntuple format to RWNT if online */
   if (clp.online || equal_ustring(clp.output_file_name, "OFLN"))
      clp.rwnt = TRUE;
#endif                          /* HAVE_HBOOK */

#ifdef HAVE_PVM
   status = pvm_main(argv);
   if (status != CM_SUCCESS)
      return 1;
#endif

#ifdef HAVE_ROOT
   /* workaround for multi-threading with midas system calls */
   ss_force_single_thread();
#endif

   /* now connect to server */
   if (clp.online) {
      if (clp.host_name[0])
         printf("Connect to experiment %s on host %s...", clp.exp_name, clp.host_name);
      else
         printf("Connect to experiment %s...", clp.exp_name);
   }

   status =
       cm_connect_experiment1(clp.host_name, clp.exp_name, analyzer_name, NULL, odb_size,
                              DEFAULT_WATCHDOG_TIMEOUT);

   if (status == CM_UNDEF_EXP) {
      printf("\nError: Experiment \"%s\" not defined.\n", clp.exp_name);
      if (getenv("MIDAS_DIR")) {
         printf
             ("Note that \"MIDAS_DIR\" is defined, which results in a single experiment\n");
         printf
             ("called \"Default\". If you want to use the \"exptab\" file, undefine \"MIDAS_DIR\".\n");
      }
      return 1;
   } else if (status != CM_SUCCESS) {
      cm_get_error(status, str);
      printf("\nError: %s\n", str);
      return 1;
   }

   if (clp.online)
      printf("OK\n");

   /* set online/offline mode */
   cm_get_experiment_database(&hDB, NULL);
   db_set_value(hDB, 0, "/Runinfo/Online Mode", &clp.online, sizeof(clp.online), 1,
                TID_INT);
   /* set run number in the database -- Brendan Kiburg 1/24/2008 */
   db_set_value(hDB, 0, "/Runinfo/Run number", &myrunnumber, sizeof(int), 1, TID_INT);

   if (clp.online) {
      /* check for duplicate name */
      status = cm_exist(analyzer_name, FALSE);
      if (status == CM_SUCCESS) {
         cm_disconnect_experiment();
         printf("An analyzer named \"%s\" is already running in this experiment.\n",
                analyzer_name);
         printf
             ("Please select another analyzer name in analyzer.c or stop other analyzer.\n");
         return 1;
      }

      /* register transitions if started online */
      if (cm_register_transition(TR_START, tr_prestart, 500) != CM_SUCCESS ||
          cm_register_transition(TR_STOP, tr_stop, 500) != CM_SUCCESS ||
          cm_register_transition(TR_PAUSE, tr_pause, 500) != CM_SUCCESS ||
          cm_register_transition(TR_RESUME, tr_resume, 500) != CM_SUCCESS) {
         printf("Failed to start local RPC server");
         return 1;
      }
   } else {
      if (!pvm_slave) {         /* slave could run on same machine... */
         status = cm_exist(analyzer_name, FALSE);
         if (status == CM_SUCCESS) {
            /* kill hanging previous analyzer */
            cm_cleanup(analyzer_name, FALSE);

            status = cm_exist(analyzer_name, FALSE);
            if (status == CM_SUCCESS) {
               /* analyzer may only run once if offline */
               status = cm_shutdown(analyzer_name, FALSE);
               if (status == CM_SHUTDOWN)
                  printf("Previous analyzer stopped\n");
            }
         }
      }
   }

#ifdef HAVE_HBOOK
   /* register callback for clearing histos */
   cm_register_function(RPC_ANA_CLEAR_HISTOS, ana_callback);
#endif

   /* turn on keepalive messages */
   cm_set_watchdog_params(TRUE, DEFAULT_RPC_TIMEOUT);

   /* decrease watchdog timeout in offline mode */
   if (!clp.online)
      cm_set_watchdog_params(TRUE, 2000);

   /* turn off watchdog if in debug mode */
   if (clp.debug)
      cm_set_watchdog_params(0, 0);

   /* initialize module parameters */
   if (init_module_parameters(FALSE) != CM_SUCCESS) {
      cm_disconnect_experiment();
      return 1;
   }

   /* create ODB structure for output */
   sprintf(str, "/%s/Output", analyzer_name);
   db_check_record(hDB, 0, str, OUT_INFO_STR, TRUE);
   db_find_key(hDB, 0, str, &hkey);
   assert(hkey);
   size = sizeof(out_info);
   db_get_record(hDB, hkey, &out_info, &size, 0);

#ifdef HAVE_ROOT
   /* create the directory for analyzer histograms */
   gManaHistsDir = new TDirectory("MidasHists", "MIDAS Analyzer Histograms", "");
   assert(gManaHistsDir != NULL);

   /* make all ROOT objects created in user module init() functions to into gManaHistsDir */
   gManaHistsDir->cd();

   /* convert .rz names to .root names */
   if (strstr(out_info.last_histo_filename, ".rz"))
      strcpy(out_info.last_histo_filename, "last.root");

   if (strstr(out_info.histo_dump_filename, ".rz"))
      strcpy(out_info.histo_dump_filename, "his%05d.root");

   db_set_record(hDB, hkey, &out_info, sizeof(out_info), 0);

#ifdef OS_LINUX
   /* start socket server */
   if (clp.online && clp.root_port) {
      TThread *th1 = new TThread("root_server_loop", root_server_loop, NULL);
      th1->Run();
   }
#endif

#endif                          /* HAVE_ROOT */

#ifdef HAVE_HBOOK
   /* convert .root names to .rz names */
   if (strstr(out_info.last_histo_filename, ".root"))
      strcpy(out_info.last_histo_filename, "last.rz");

   if (strstr(out_info.histo_dump_filename, ".root"))
      strcpy(out_info.histo_dump_filename, "his%05d.rz");

   db_set_record(hDB, hkey, &out_info, sizeof(out_info), 0);
#endif

#ifdef HAVE_HBOOK
   /* create global memory */
   if (clp.online) {
      HLIMAP(pawc_size / 4, out_info.global_memory_name);
      printf("\nGLOBAL MEMORY NAME = %s\n", out_info.global_memory_name);
   } else {
      if (equal_ustring(clp.output_file_name, "OFLN")) {
         strcpy(str, "OFLN");
         HLIMAP(pawc_size / 4, str);
         printf("\nGLOBAL MEMORY NAME = %s\n", "OFLN");
      } else
         HLIMIT(pawc_size / 4);
   }
#endif                          /* HAVE_HBOOK */

#ifdef HAVE_ROOT
   /* load histos from last.xxx */
   if (clp.online)
      load_last_histos();
#endif

   /* analyzer init function */
   if (mana_init() != CM_SUCCESS) {
      cm_disconnect_experiment();
      return 1;
   }
#ifdef HAVE_HBOOK
   /* load histos from last.xxx */
   if (clp.online)
      load_last_histos();
#endif

   /* reqister event requests */
   register_requests();

   /* initialize ss_getchar */
   if (!clp.quiet && !pvm_slave)
      ss_getchar(0);

  /*---- start main loop ----*/

/* multi-threaded root server only under Linux */
//#if defined(HAVE_ROOT) && defined(OS_LINUX)
#if 0
   /* start event thread */
   TThread *th2 = new TThread("root_event_loop", root_event_loop, NULL);
   th2->Run();

   manaApp->Run();
#else
#ifdef HAVE_ROOT
   if(clp.graphics){
     /* start event thread */
     TThread *th2 = new TThread("root_event_loop", root_event_loop, NULL);
     th2->Run();
     
     manaApp->Run();
   }
#endif // HAVE_ROOT

   else if (clp.online)
      loop_online();
   else
      loop_runs_offline();
#endif

   /* reset terminal */
   if (!clp.quiet && !pvm_slave)
      ss_getchar(TRUE);

   /* call exit function */
   mana_exit();

   /* save histos to last.xxx */
   if (clp.online)
      save_last_histos();

#ifdef HAVE_PVM

   PVM_DEBUG("Analyzer stopped");

   /* exit PVM */
   pvm_exit();

   /* if PVM slave, don't write *SHM file back */
   if (pvm_slave)
      disable_shm_write = TRUE;

#endif

   /* disconnect from experiment */
   cm_disconnect_experiment();

   return 0;
}
