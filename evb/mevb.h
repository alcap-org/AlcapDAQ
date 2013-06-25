/********************************************************************\
Name:         mevb.h
  Created by:   Pierre-Andre Amaudruz

  Contents:     Event builder header file
  $Log: mevb.h,v $
  Revision 1.3  2005/10/31 14:11:37  mucap
  (Fred)
  Update to new major version of MIDAS event builder.

  Revision 1.10  2004/10/07 20:08:34  pierre
  1.9.5

  Revision 1.9  2004/09/29 16:20:43  pierre
  change Ebuilder structure

  Revision 1.8  2004/01/08 08:40:08  midas
  Implemented standard indentation

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
"[.]",\
"Number of Fragment = INT : 0",\
"User build = BOOL : n",\
"User Field = STRING : [64] 100",\
"Fragment Required = BOOL[2] :",\
"[0] 0",\
"[0] 0",\
"",\
NULL }


typedef struct {
   WORD event_id;
   WORD trigger_mask;
   INT  nfragment;
   char buffer[32];
   char format[32];
   BOOL user_build;
   char user_field[64];
   char hostname[64];
   BOOL *preqfrag;
   BOOL *received;
} EBUILDER_SETTINGS;

typedef struct {
   double events_sent;
   double events_per_sec_;
   double kbytes_per_sec_;
} EBUILDER_STATISTICS;

typedef struct {
   char buffer[32];
   char format[32];
   WORD event_id;
   WORD trigger_mask;
   INT type;
   INT hBuf;
   INT req_id;
   INT hStat;
   INT timeout;
   DWORD serial;
   char *pfragment;
} EBUILDER_CHANNEL;

#define   EB_SUCCESS               1
#define   EB_COMPOSE_TIMEOUT      -1
#define   EB_ERROR              1001
#define   EB_USER_ERROR         1002
#define   EB_ABORTED            1003
#define   EB_SKIP               1004
#define   TIMEOUT              10000
#define   MAX_CHANNELS           128
