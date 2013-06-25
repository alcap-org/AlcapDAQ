/********************************************************************\
Name:         mevb.h
  Created by:   Pierre-Andre Amaudruz

  Contents:     Event builder header file
  $Log: mevb.h,v $
  Revision 1.2  2003/05/13 19:27:04  mucap
  Fixes to allow a subset of the crates to participate in an event.  Also,
  this pulls in Pierre's CVS changes to the generic eventbuilder code.

  Revision 1.7  2003/01/28 20:44:50  pierre
  Remove user field in fragment struct

  Revision 1.6  2002/10/07 17:04:10  pierre
  fix tr_stop request

  Revision 1.5  2002/09/28 00:48:48  pierre
  Add EB_USER_ERROR handling

  Revision 1.4  2002/09/25 18:37:46  pierre
  correct: header passing, user field, abort run

  Revision 1.3  2002/06/14 04:57:09  pierre
  revised for 1.9.x

\********************************************************************/

#define EBUILDER(_name) char *_name[] = {\
"[Settings]",\
"Event ID = WORD : 1",\
"Trigger mask = WORD : 1",\
"Buffer = STRING : [32] SYSTEM",\
"Format = STRING : [32] MIDAS",\
"User build = BOOL : n",\
"User Field = STRING : [64] ",\
"Event mask = DWORD : 3",\
"Hostname = STRING : [64] ",\
"",\
"[Statistics]",\
"Events sent = DOUBLE : 0",\
"Events per sec. = DOUBLE : 0",\
"kBytes per sec. = DOUBLE : 0",\
"",\
NULL }

#define EBUILDER_CHANNEL(_name) char *_name[] = {\
"[Frag1/Settings]",\
"Event ID = WORD : 1",\
"Trigger mask = WORD : -1",\
"Buffer = STRING : [32] BUF1",\
"Format = STRING : [32] MIDAS",\
"Event mask = DWORD : 1",\
"",\
"[Frag1/Statistics]",\
"Events sent = DOUBLE : 0",\
"Events per sec. = DOUBLE : 0",\
"kBytes per sec. = DOUBLE : 0",\
"",\
"[Frag2/Settings]",\
"Event ID = WORD : 2",\
"Trigger mask = WORD : -1",\
"Buffer = STRING : [32] BUF2",\
"Format = STRING : [32] MIDAS",\
"Event mask = DWORD : 2",\
"",\
"[Frag2/Statistics]",\
"Events sent = DOUBLE : 0",\
"Events per sec. = DOUBLE : 0",\
"kBytes per sec. = DOUBLE : 0",\
"",\
NULL }

typedef struct {
    WORD      event_id;
    WORD      trigger_mask;
    char      buffer[32];
    char      format[32];
    BOOL      user_build;
    char      user_field[64];
    DWORD     emask;
    char      hostname[64];
  } EBUILDER_SETTINGS;

typedef struct {
    WORD      event_id;
    WORD      trigger_mask;
    char      buffer[32];
    char      format[32];
    DWORD     emask;
  } EBUILDER_SETTINGS_CH;

typedef struct {
    double    events_sent;
    double    events_per_sec_;
    double    kbytes_per_sec_;
  } EBUILDER_STATISTICS;

typedef struct {
    char  name[32];
    INT   hBuf;
    INT   req_id;
    INT   hStat;
    INT   timeout;
    DWORD serial;
    char *pfragment;
    EBUILDER_SETTINGS_CH   set;
    EBUILDER_STATISTICS   stat;
} EBUILDER_CHANNEL;

#define   EB_SUCCESS               1
#define   EB_COMPOSE_TIMEOUT      -1
#define   EB_ERROR              1001
#define   EB_USER_ERROR         1002
#define   EB_ABORTED            1003
#define   EB_SKIP               1004
#define   TIMEOUT                 10
#define   MAX_CHANNELS             8
