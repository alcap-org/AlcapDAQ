/********************************************************************\

  Name:         epics_ca.c
  Created by:   Stefan Ritt (Modified RA36)

  Contents:     Epics channel access device driver Version 3.14

  class driver is generic with Demand and Measured Variables

  device driver list
  DEVICE_DRIVER epics_driver[] = {
                                       // initial demand value taken from instrument
       { "EPICSBB", epics_ca, <nchans>, null, DF_PRIO_DEVICE | DF_NOUPDALL},
       { "" }
  };

  EPICS base package must be installed   (caRepeater, libca.so libCom.so)

  $Id: epics_ca.c,v 1.2 2011-06-25 20:56:00 l_data Exp $

\********************************************************************/

#define GFA_SPECIFIC              /* include GFA_SPECIFIC code in private       */
                                  /* header and source                          */

// #define OMIT_ODBUPDATE         /* define OMIT_ODBUPDATE to omit update of    */
                                  /* /Equipment/<equi>/Variables/Demand         */

#include <errno.h>

#include "epics_ca_private.h"     /* include private header                     */

//#define MIDEBUG               /* write debug information */
//#define MIDEBUG1              /* debug information about functions (except _get()) */
//#define MIDEBUG2              /* exhaustive debug information */
//#define MIDEBUGP              /* status return of ca_pend_io() and ca_pend_event() */
//#define MIDEBUGE              /* event handler debug using cm_msg(MLOG) */
//#define MIDEBUGE1             /* event handler func */
//#define MIDEBUGF              /* print ss_time()'ed info to file */

/*---- globals -----------------------------------------------------*/

static char  smsg[128];
#ifdef MIDEBUGF
static FILE *fp = NULL;
#endif

#ifdef GFA_SPECIFIC
typedef struct _GFA_msg {  // message corresponding to value
  long           value;       
  char          *msg;
} GFA_msg;

typedef struct _GFA_mask {// mask and pointer to list of value/message pairs
  long          mask;
  char         *desc;
  GFA_msg      *sm;
  int           nm;
} GFA_mask;

typedef struct _GFA_channel {// channel <spec>ification and pointer to list of masks
  char         *spec;
  char         *desc;
  GFA_mask     *cm;
  int           ns;
  BOOL          iscmd;
} GFA_channel;

typedef struct _GFA_type { // device type and pointer to list of channel <spec>
  char         *type;
  char         *desc;
  GFA_channel  *tc;
  int           nc;
} GFA_type;

/* ------------------------ KV-STA :STA:1 messages --------------------- */
GFA_msg KVstamsg0003[] = { 
    {0x0000, "Moving"  },
    {0x0001, "Open"    }, 
    {0x0002, "Closed"  },
    {0x0003, "Error"   }
};

GFA_msg KVstamsg0004[] = { 
    {0x0004, "Pass Bew.OK"  }, 
    {0x0000, "Pass Bew.NOK" }
};

GFA_msg KVstamsg0008[] = { 
    {0x0008, "RPS Bew.OK"   }, 
    {0x0000, "RPS Bew.NOK"  }
};

GFA_mask KVsta[] = {
    { 0x0003, "Status", KVstamsg0003, sizeof(KVstamsg0003)/sizeof(GFA_msg)},
    { 0x0004, "Status", KVstamsg0004, sizeof(KVstamsg0004)/sizeof(GFA_msg)},
    { 0x0008, "Status", KVstamsg0008, sizeof(KVstamsg0008)/sizeof(GFA_msg)},
};

/* ------------------------ KV-COM :COM:2 commands --------------------- */
GFA_msg KVcommsg0001[] = { 
    {0x0000, "OPEN"  }, 
    {0x0001, "CLOSE" }
};

GFA_mask KVcom[] = {
    { 0x0001, "Commands", KVcommsg0001, sizeof(KVcommsg0001)/sizeof(GFA_msg)}
};

GFA_channel KVchan[] = {
    { ":STA:1", "Status",   KVsta, sizeof(KVsta)/sizeof(GFA_mask), FALSE},
    { ":COM:2", "Commmand", KVcom, sizeof(KVcom)/sizeof(GFA_mask), TRUE },
};

/* ------------------------ KombiPS1 :STA:1 messages --------------------- */
/* NOTE: MultiIO :STA:1 messages are identical   */
                                                                  //             8 4 2 1
GFA_msg KPS1stamsg000f[] = {                                      // status bits 3+2+1+0
    // bit 0 : set : Soll power on
    // bit 1 : set : Soll power off
    // bit 2 : set : Ist  power on
    // bit 3 : set : Ist  power off 
    {0x0000, "Soll and Ist NOT ON and NOT OFF"                     },      //    0 0 0 0
    {0x0001, "Soll ON - Ist NOT ON and NOT OFF"                    },      //    0 0 0 1
    {0x0002, "Soll OFF - Ist NOT ON and NOT OFF"                   },      //    0 0 1 0
    {0x0003, "Soll ON and OFF - Ist NOT ON and NOT OFF"            },      //    0 0 1 1
    {0x0004, "Soll NOT ON and NOT OFF - Ist ON"                    },      //    0 1 0 0
    {0x0005, "On"                                                  },      //    0 1 0 1
    {0x0006, "Soll OFF - Ist ON"                                   },      //    0 1 1 0
    {0x0007, "Soll ON and OFF, Ist ON"                             },      //    0 1 1 1
    {0x0008, "Soll NOT ON and NOT OFF - Ist OFF"                   },      //    1 0 0 0
    {0x0009, "Soll On - Ist OFF"                                   },      //    1 0 0 1
    {0x000a, "Off"                                                 },      //    1 0 1 0
    {0x000b, "Soll ON and OFF - Ist OFF"                           },      //    1 0 1 1
    {0x000c, "Soll NOT ON and NOT OFF - Ist ON and OFF"            },      //    1 1 0 0
    {0x000d, "Soll ON - Ist ON and OFF"                            },      //    1 1 0 1
    {0x000e, "Soll OFF - Ist ON and OFF"                           },      //    1 1 1 0
    {0x000f, "Soll ON and OFF - Ist ON and OFF"                    }       //    1 1 1 1
};

GFA_msg KPS1stamsg0010[] = {       // status bit  4
#ifdef MIDEBUG
    {0x0000, "In Remote Mode" },
#endif
    {0x0010, "In Local Mode"  }
};

GFA_msg KPS1stamsg0020[] = {       // status bit  5
#ifdef MIDEBUG
    {0x0020, "SG Status OK"     },
#endif
    {0x0000, "SG Status NOK"    }
};

GFA_msg KPS1stamsg0040[] = {       // status bit  6
#ifdef MIDEBUG
    {0x0040, "PSC Status OK"     },
#endif
    {0x0000, "PSC Status NOK"    }
};

GFA_msg KPS1stamsg0080[] = {       // status bit  7
#ifdef MIDEBUG
    {0x0080, "Config OK"      },
#endif
    {0x0000, "Config Error"   }
};
                                   // status bit  8 NOT USED
                                   // status bit  9 NOT USED
GFA_msg KPS1stamsg0400[] = {       // status bit 10 
//#ifdef MIDEBUG
    {0x0400, "IST == SOLA"    },
//#endif
    {0x0000, "IST <> SOLA"    }
};

GFA_msg KPS1stamsg0800[] = {       // status bit 11
#ifdef MIDEBUG
    {0x0800, "IST >= Low Limit" },
#endif
    {0x0000, "IST < Low Limit"  }
};

GFA_msg KPS1stamsg1000[] = {       // status bit 12
#ifdef MIDEBUG
    {0x1000, "IST <= High Limit"},
#endif
    {0x0000, "IST > High Limit" }
};

GFA_msg KPS1stamsg2000[] = {       // status bit 13
#ifdef MIDEBUG
    {0x2000, "SOL==SOLA"      },
#endif
    {0x0000, "SOL<>SOLA"      }   
};
                                   // status bit 14 NOT USED
GFA_msg KPS1stamsg8000[] = {       // status bit 15           
#ifdef MIDEBUG
    {0x8000, "PSC Link OK"     },   
#endif
    {0x0000, "PSC Link Error"  }
};

GFA_mask KPS1sta[] = {
    { 0x000F, "Status", KPS1stamsg000f, sizeof(KPS1stamsg000f)/sizeof(GFA_msg)},
    { 0x0010, "Status", KPS1stamsg0010, sizeof(KPS1stamsg0010)/sizeof(GFA_msg)},
    { 0x0020, "Status", KPS1stamsg0020, sizeof(KPS1stamsg0020)/sizeof(GFA_msg)},
    { 0x0040, "Status", KPS1stamsg0040, sizeof(KPS1stamsg0040)/sizeof(GFA_msg)},
    { 0x0080, "Status", KPS1stamsg0080, sizeof(KPS1stamsg0080)/sizeof(GFA_msg)},
    // bit 8 & 9 not used
    { 0x0400, "Status", KPS1stamsg0400, sizeof(KPS1stamsg0400)/sizeof(GFA_msg)},
    { 0x0800, "Status", KPS1stamsg0800, sizeof(KPS1stamsg0800)/sizeof(GFA_msg)},
    { 0x1000, "Status", KPS1stamsg1000, sizeof(KPS1stamsg1000)/sizeof(GFA_msg)},
    { 0x2000, "Status", KPS1stamsg2000, sizeof(KPS1stamsg2000)/sizeof(GFA_msg)},
    // bit 14 not used
    { 0x8000, "Status", KPS1stamsg8000, sizeof(KPS1stamsg8000)/sizeof(GFA_msg)}
};

/* ------------------------ KombiPS1 :COM:2 commands --------------------- */
/* NOTE: MultiIO :COM:2 commands are identical   */
GFA_msg KPS1cmdmsg[] = { 
    {0x0000, "ON"   },
    {0x0001, "OFF"  }, 
    {0x0002, "RESET"}
};

GFA_mask KPS1cmd[] = {
    { 0x0003, "Command", KPS1cmdmsg, sizeof(KPS1cmdmsg)/sizeof(GFA_msg)}
};

/* ------------------------ KombiPS1 list of channels -------------------- */
GFA_channel KPS1chan[] = {
    { ":STA:1", "Status",   KPS1sta, sizeof(KPS1sta)/sizeof(GFA_mask), FALSE},
    { ":COM:2", "Command",  KPS1cmd, sizeof(KPS1cmd)/sizeof(GFA_mask), TRUE },
    { ":IST:2", "Istwert",  NULL,    0,                                FALSE},
    { ":SOL:2", "Sollwert", NULL,    0,                                TRUE }
};

/* ------------------------ Slit system :STA:1 commands --------------------- */

GFA_msg Slitstamsg307[] = { 
    {0x0001, "in CW limit switch"              }, 
    {0x0002, "in CCW limit switch"             }, 
    {0x0003, "not connected/command error"     }, 
    {0x0004, "stopped"                         }, 
    {0x0005, "in CW limit switch and stopped"  }, 
    {0x0006, "in CCW limit switch and stopped" } 
};

GFA_msg Slitstamsg300[] = { 
    {0x0100, "moving CW"    } ,
    {0x0200, "moving CCW"   } 
};

GFA_mask Slitsta[] = {
    { 0x0307, "Status", Slitstamsg307, sizeof(Slitstamsg307)/sizeof(GFA_msg)},
    { 0x0300, "Status", Slitstamsg300, sizeof(Slitstamsg300)/sizeof(GFA_msg)}
};

/* ------------------------ Slit system :COM:2 commands --------------------- */

GFA_msg Slitcmdmsg[] = { 
    {0x0000, "MOVE_TO_CW"    }, 
    {0x0001, "MOVE_TO_CCW"   },
    {0x0002, "STOP"          },
    {0x0003, "MOVE_TO_SOL"   },
    {0x0004, "SOL=POSA"      },
    {0x0005, "RESET_CW"      },
    {0x0006, "RESET_CCW"     }
};

GFA_mask Slitcmd[] = {
    { 0x0007, "Command", Slitcmdmsg, sizeof(Slitcmdmsg)/sizeof(GFA_msg)}
};

/* ------------------------ Slit system list of channels -------------------- */
GFA_channel Slitchan[] = {
    {":STA:1", "Status",                 Slitsta,sizeof(Slitsta)/sizeof(GFA_mask),FALSE},
    {":COM:2", "Command",                Slitcmd,sizeof(Slitcmd)/sizeof(GFA_mask),TRUE },
    {":IST1:2","Position ADC rohwert",   NULL,   0,                               FALSE},
    {":SOL:2", "Sollwert",               NULL,   0,                               TRUE },
    {":POSA:2","Position ADC gefiltert", NULL,   0,                               FALSE}
};

/* ------------------------ Spin rotator/Separator channels ----------------- */
GFA_msg HVPSstamsg[] = { 
    {0x0000, "OFF"  },
    {0x0001, "ON"   }
};

GFA_mask HVPSsta[] = {
    { 0x0001, "Status", HVPSstamsg, sizeof(HVPSstamsg)/sizeof(GFA_msg)}
};

GFA_channel HVPSchan[] = {
    {"HVN:STA:2", "Neg. HV Status", HVPSsta, sizeof(HVPSsta)/sizeof(GFA_mask),   FALSE},
    {"HVN:ISTI:2",      "Neg. HV Current Measured [uA]", NULL,   0,       FALSE},
    {"HVN:ISTV:2",      "Neg. HV Voltage Measured [kV]", NULL,   0,       FALSE},
    {"HVN:SOLI:2",      "Neg. HV Current Demand   [uA]", NULL,   0,       TRUE},
    {"HVN:SOLV:2",      "Neg. HV Voltage Demand   [kV]", NULL,   0,       TRUE},
    {"HVP:STA:2", "Pos. HV Status", HVPSsta, sizeof(HVPSsta)/sizeof(GFA_mask),   FALSE},
    {"HVP:ISTI:2",      "Pos. HV Current Measured [uA]", NULL,   0,       FALSE},
    {"HVP:ISTV:2",      "Pos. HV Voltage Measured [kV]", NULL,   0,       FALSE},
    {"HVP:SOLI:2",      "Pos. HV Current Demand   [uA]", NULL,   0,       TRUE},
    {"HVP:SOLV:2",      "Pos. HV Voltage Demand   [kV]", NULL,   0,       TRUE},
    {"HV:COM:2",        "HV IDLE=0/ON=1/OFF=2/SET=3",    NULL,   0,       TRUE},
    {"HV:XRAY:2",       "Xray ",                         NULL,   0,       FALSE},
    {"HV:VAC:2",        "Vacuum",                        NULL,   0,       FALSE},
    {"HV:TIME:2",       "TimeStamp of SoftIOC",          NULL,   0,       FALSE}
};

/* ------------------------ List of all GFA types ------------------------ */
GFA_type gfatype[] = {
    /* for these GFA types KombiPS1 or MultiIO devices are used */
    { "BEND",   "Bending magnet",       KPS1chan, sizeof(KPS1chan)/sizeof(GFA_channel)},
    { "QUAD",   "Quadrupol magnet",     KPS1chan, sizeof(KPS1chan)/sizeof(GFA_channel)},
    { "SEPT",   "Septum magnet",        KPS1chan, sizeof(KPS1chan)/sizeof(GFA_channel)},
    { "STER",   "Steering magnet",      KPS1chan, sizeof(KPS1chan)/sizeof(GFA_channel)},
    { "PS",     "Spinrot magnet",       KPS1chan, sizeof(KPS1chan)/sizeof(GFA_channel)},
    { "TS",     "Shim magnet",          KPS1chan, sizeof(KPS1chan)/sizeof(GFA_channel)},
    { "KICKER", "Kicker HV",            KPS1chan, sizeof(KPS1chan)/sizeof(GFA_channel)},
    { "BX",     "Beam Blocker",         KVchan,   sizeof(KVchan  )/sizeof(GFA_channel)},
    { "FENTE",  "Slit system",          Slitchan, sizeof(Slitchan)/sizeof(GFA_channel)},
//NIY "SEXT",   "Sextupol magnet",
//NIY "VALVE",  "Valve",
//NIY "PUMP",   "Pump",
//NIY "GAUGE",  "Gauge",
    { "HVPS",   "Spinrot/Sep HV",       HVPSchan, sizeof(HVPSchan)/sizeof(GFA_channel)},
//NIY 
    { "",       "Terminator",        NULL,     0},
};

#define N_GFA_TYPES  (sizeof(gfatype)/sizeof(GFA_type)) -1

#endif /* ifdef GFA_SPECIFIC */

/*---- functions ---------------------------------------------------*/

INT epics_ca           (INT cmd, ...);
INT epics_ca_init      (HNDLE , void **, INT);
INT epics_ca_exit      (CA_INFO *);
INT epics_ca_set       (CA_INFO *, INT, float, BOOL, BOOL);
INT epics_ca_set_all   (CA_INFO *, INT, float);
INT epics_ca_set_label (CA_INFO *, INT, char  *);
INT epics_ca_get       (CA_INFO *, INT, float *);
INT epics_ca_get_demand(CA_INFO *, INT, float *);
INT epics_ca_get_all   (CA_INFO *, INT, float *);

INT epics_ca_set_pending (CA_INFO *, BOOL);

INT epics_ca_get_default_name     (CA_INFO *, INT, char  *);
INT epics_ca_get_default_threshold(CA_INFO *, INT, float *);

char *castatestr(enum channel_state);
void epics_ca_exception_handler ( struct exception_handler_args );
void epics_ca_event_handler     ( struct event_handler_args );
void epics_ca_connection_handler( struct connection_handler_args );

/*----------------------------------------------------------------------------*/

char *castatestr(enum channel_state castate) {
  
  switch(castate) {
    case cs_never_conn:
         strcpy(&smsg[0],"Server not found or unavailable");
      break;

    case cs_prev_conn:
         strcpy(&smsg[0],"Previously connected to server");
      break;

    case cs_conn:
         strcpy(&smsg[0],"Connected to server");
      break;

    case cs_closed:
         strcpy(&smsg[0],"Channel deleted by user");
      break;

    default:
      strcpy(&smsg[0],"**UNKNOWN STATE**");
  }
  return &smsg[0];
}

/*----------------------------------------------------------------------------*/
//
// epics_ca_set_pending : 
//   BOOL pendig
//         TRUE        increment info->pending blocking in range (tolerance) checks 
//                     until all pending operations accomplished
//         FALSE       decrement info->pending.  in range (tolerance) checks will
//                     be evaluated when all dependecies are released
//
INT epics_ca_set_pending (CA_INFO *info, BOOL pending) {
   INT status;

   status = FE_SUCCESS;
   if (info) {
     if (pending) {
       if (info->pending == 0) cm_msg(MLOG,"","*** Blocking Tolerance InRange checking");
       info->pending++;
     } else {
       if (info->pending > 0) {
         info->pending--;
         if (info->pending == 0) 
            cm_msg(MLOG,"","*** Tolerance InRange checking not blocked anymore");
       } else {
         cm_msg(MERROR,"epics_ca_set_pending","PROGRAMMING ERROR releasing pending "
                                              "AND info->pending == 0");
       }
     }
   }
   return status;
}
/*----------------------------------------------------------------------------*/

// exception handler callback

void epics_ca_exception_handler( struct exception_handler_args eargs) {
  char buf[512];
  char *pname;

#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : ++epics_ca_exception_handler()\n", ss_time());
#endif
#ifdef MIDEBUG1
   cm_msg(MLOG,"","++epics_ca_exception_handler()");
#endif
  if (eargs.chid) {
    sprintf(buf,"%s - chan=%s op=%d data_type = %s count=%d",
                 eargs.ctx, ca_name(eargs.chid), eargs.op, dbr_type_to_text(eargs.type),
                 eargs.count);
  } else {
    sprintf(buf,"%s - op=%d", eargs.ctx, eargs.op);
  }
  cm_msg(MLOG,"","Status: %d EXCEPTION: %s",eargs.stat,buf);
  ca_signal(eargs.stat, buf);

#ifdef MIDEBUG1
   cm_msg(MLOG,"","--epics_ca_exception_handler()");
#endif
#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : --epics_ca_exception_handler()\n", ss_time());
#endif
}

/*----------------------------------------------------------------------------*/

// connection handler callback

void epics_ca_connection_handler( struct connection_handler_args cargs) {

#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : ++epics_ca_connection_handler()\n", ss_time());
#endif
#ifdef MIDEBUG1
   cm_msg(MLOG,"","++epics_ca_connection_handler()");
#endif
  
  if (cargs.chid) {
    CA_INFO *pinfo;
    int i,status;

    pinfo = (CA_INFO *) ca_puser(cargs.chid);
    if (pinfo) {
      BOOL found;

      /* search channel index */
      for (found = FALSE, i = 0; i < pinfo->num_channels; i++) {
        if (*(pinfo->pchid+i) == cargs.chid) {  // matching channel ID?

          if (cargs.op == CA_OP_CONN_UP) {
            if (*(pinfo->subscribed+i)) {
              cm_msg(MLOG,"","Channel %d(%s) RECONNECTED",i,ca_name(cargs.chid));
#ifdef MIDEBUGF
              if (fp) fprintf(fp,"%u : Channel %d(%s) RECONNECTED\n", ss_time(),
                              i,ca_name(cargs.chid));
#endif
              if (!*(pinfo->found+i)) {
                *(pinfo->found+i) = TRUE;
                cm_msg(MLOG,"","epics_ca_connection_handler:  found flag of channel %d"
                               "(%s)was not set yet",i,ca_name(cargs.chid));
              }
            } else {
#ifdef MIDEBUG1
              cm_msg(MLOG,"","Channel %d(%s) CONNECTED",i,ca_name(cargs.chid));
#endif
#ifdef MIDEBUGF
              if (fp) fprintf(fp,"%u : Channel %d(%s) CONNECTED\n", ss_time(),
                              i,ca_name(cargs.chid));
#endif
              // chan specific: omit channels with S-flag e.g.: returning strings
              //                and :COM:2 channels
              if ((strchr(pinfo->settings.chnflags+CHNFLAGS_LENGTH*i,'S') == NULL)&&
               (strstr(pinfo->settings.channel_names+CHN_NAME_LENGTH*i,":COM:2")==NULL)){
                if (!pinfo->subscribe_disabled) {
                  cm_msg(MLOG,"","Channel %d(%s) CONNECTED",i,ca_name(cargs.chid));
                  // subscribe event handler callback for this channel when not created
                  // subscribed value is converted to float
                  status = ca_create_subscription(DBR_FLOAT,1,*(pinfo->pchid+i),DBE_LOG|
                                       DBE_VALUE, epics_ca_event_handler, pinfo, NULL);
                  if (!(status & ECA_NORMAL)) {
                    cm_msg(MERROR, "epics_ca_connection_handler", "Channel %d(%s): "
                      "ca_create_subscription() returned status 0x%X = \"%s\"",i,
                      pinfo->settings.channel_names+CHN_NAME_LENGTH*i,status,
                      ca_message(status));
                  } else {
#ifdef MIDEBUG1
                    cm_msg(MLOG,"","Channel %d(%s): CA event handler (float) will be "
                                   "installed",i,
                                pinfo->settings.channel_names+CHN_NAME_LENGTH*i,status);
#endif
                    *(pinfo->subscribed+i) = TRUE;
                    /* NOTE: ca_pend_io(0.01); may not be called in this event handler */
                  }
                }
              } else {
                if (strchr(pinfo->settings.chnflags+CHNFLAGS_LENGTH*i,'S'))
                  cm_msg(MLOG,"","Channel %d(%s): S flag: CA event handler will NOT be "
                       "installed", i, pinfo->settings.channel_names+CHN_NAME_LENGTH*i); 
#ifdef MIDEBUG
               else if (strstr(pinfo->settings.channel_names+CHN_NAME_LENGTH*i,":COM:2"))
                  cm_msg(MLOG,"","Channel %d(%s): COM Channel : CA event handler will NO"
                    "T be installed",i,pinfo->settings.channel_names+CHN_NAME_LENGTH*i); 
#endif
                *(pinfo->subscribed+i) = TRUE; // mark as handled
              }
              *(pinfo->found+i) = TRUE; // mark channel as found
            }
          //if (strchr(pinfo->settings.chnflags+CHNFLAGS_LENGTH*i,'C') != NULL)
              *(pinfo->down+i) = FALSE;     // mark channel to be NOT down
          } else {
            cm_msg(MLOG,"","Channel %d(%s) is DOWN",i,ca_name(cargs.chid));
          //if (strchr(pinfo->settings.chnflags+CHNFLAGS_LENGTH*i,'C') != NULL)
              *(pinfo->down+i) = TRUE;    // mark channel to be down
            *(pinfo->firstread+i) = TRUE; // mark channel for first read after DOWN
          }
          found = TRUE;
          break;
        } 
      } // for all channels

      if (!found)
        cm_msg(MERROR,"epics_ca_connection_handler","Channel %s NOT found in list!",
                                                    ca_name(cargs.chid));
    } else {
      cm_msg(MERROR,"epics_ca_connection_handler","NULL pointer returned by ca_puser()");
    }
  } else {
    cm_msg(MERROR,"epics_ca_connection_handler","channel id nil returned as argument");
  }

#ifdef MIDEBUG1
  cm_msg(MLOG,"","--epics_ca_connection_handler()");
#endif
#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : --epics_ca_connection_handler()\n", ss_time());
#endif
}

/*----------------------------------------------------------------------------*/

// event handler callback

void epics_ca_event_handler(struct event_handler_args args) {
   CA_INFO *pinfo;
   int      i;
   BOOL     found;

#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : ++epics_ca_event_handler()\n", ss_time());
#endif
#ifdef MIDEBUGE1
   cm_msg(MLOG,"","++epics_ca_event_handler()");
#endif

   found = FALSE;
 
   pinfo = (CA_INFO *) args.usr;

   if (pinfo) {
     /* find channel index */
     for (i = 0; i < pinfo->num_channels; i++) {
       if (*(pinfo->pchid+i) == args.chid) {  // matching channel ID?
         if (args.type == DBR_FLOAT) {        // data type is pure float?
           float tval;

           if (*(pinfo->subscribed+i) != TRUE) {
             *(pinfo->subscribed+i) = TRUE;
             cm_msg(MLOG,"","epics_ca_event_handler: Subscribed flag not yet set for"
               "channel %d(%s)",i,pinfo->settings.channel_names+CHN_NAME_LENGTH*i);
           }

           if (*(pinfo->down+i)) {
#ifdef MIDEBUG1
              cm_msg(MLOG,"","epics_ca_event_handler: channel %d(%s) Marked DOWN. Now "
                "set to NOT DOWN",i,pinfo->settings.channel_names+CHN_NAME_LENGTH*i);
#endif
          //if (strchr(pinfo->settings.chnflags+CHNFLAGS_LENGTH*i,'C') != NULL)
              *(pinfo->down+i) = FALSE;     // mark channel to be NOT down
           }
           if (args.dbr) {
             tval = *((float *)args.dbr);
#ifdef MIDEBUGE
             cm_msg(MLOG,"","callback for channel %d(%s): value %f", i, 
                            pinfo->settings.channel_names+CHN_NAME_LENGTH*i,tval);
#endif
#ifdef MIDEBUGF
             if (fp) fprintf(fp,"%u : callback for channel %d(%s): value %f\n", 
                  ss_time(),i,pinfo->settings.channel_names+CHN_NAME_LENGTH*i,tval);
#endif
             if ((args.status != ECA_NORMAL) && (*(pinfo->status+i)!=args.status)) {
                 cm_msg(MLOG,"","epics_ca_event_handler: WARNING Status %d = \"%s\" for "
                                "channel %d(%s)", args.status, ca_message(args.status),
                                i,pinfo->settings.channel_names+CHN_NAME_LENGTH*i);
             }
             *(pinfo->status+i) = args.status;
             if (pinfo->array) {
               *(pinfo->lastget+i) = ss_time();
               if (*(pinfo->array+i) != tval) {
#ifdef GFA_SPECIFIC
                 char *tpos;

                 // channel has <device>:<spec>
                 if ((tpos=strchr(pinfo->settings.channel_names+CHN_NAME_LENGTH*i,':'))
                   != NULL) {
                   int j;

                   for (j = 0; j < N_GFA_TYPES; j++) {
                     // find type
                    if (strcmp(gfatype[j].type,pinfo->settings.dtype+DTYPE_LENGTH*i)==0){
                       GFA_channel *tc;
                       int k;
#ifdef MIDEBUGE
                       cm_msg(MLOG,"","Found type %s", gfatype[j].type);
#endif
                       tc = gfatype[j].tc;
                       for (k=0; k < gfatype[j].nc; k++) {  // channels of this type

                         if (!(tc+k)) continue;

                         if (strcmp(tpos,(tc+k)->spec ) == 0) { // matching :<spec>
                           int l;
                           GFA_mask *cm;
#ifdef MIDEBUGE
                           cm_msg(MLOG,"","Found spec %s", (tc+k)->spec);
#endif
                           cm = (tc+k)->cm;
                           for (l = 0; l < (tc+k)->ns; l++) { // for all masks
                             int m;
                             int ttval, tarr;
                             GFA_msg      *sm;

                             if (!(cm+l)) continue;
#ifdef MIDEBUGE
                             cm_msg(MLOG,"","Checking Mask 0x%4.4X",(cm+l)->mask);
#endif
                             ttval = (int)tval & (cm+l)->mask; 
                             tarr  = (int) *(pinfo->array+i) & (cm+l)->mask;
                             if (*(pinfo->array+i) == -2.f) tarr = 0; // hide setup flag
                             if (( ttval != tarr)||(*(pinfo->array+i) == -2.f)) {
                               char *tpost,*tposa;
                               tpost = tposa = NULL;

                               sm = (cm+l)->sm;
                               for (m = 0; m < (cm+l)->nm; m++) {
                                 if (!(sm+m)) continue;
                           //    cm_msg(MLOG,"","check Value 0x%4.4X : \"%s\"",
                           //                   (sm+m)->value, (sm+m)->msg);
                                 if ((sm+m)->value == ttval) {
                                   if (tpost) cm_msg(MLOG,"","New value: %s overwritten"
                                                " by %s", tpost, (sm+m)->msg);
                                   tpost = (sm+m)->msg;
                                 }
                                 if ((sm+m)->value == tarr)  {
                                   if (tposa) cm_msg(MLOG,"","Prev value: %s overwritten"
                                                " by %s", tposa, (sm+m)->msg);
                                   tposa = (sm+m)->msg;
                                 }
                               } // for all values

                               if (tpost) {
                                 if (*(pinfo->array+i) == -2.f) tposa = NULL;//hide setup
                                 if (tposa) {
                                   cm_msg(MLOG,"","%s: \"%s\" changed to \"%s\"",
                                     pinfo->settings.channel_names+CHN_NAME_LENGTH*i,
                                     tposa, tpost);
                                 } else {
                                   if (strstr(pinfo->settings.channel_names+
                                              CHN_NAME_LENGTH*i,":STA:")) {
                                     cm_msg(MLOG,"","%s: Current status is \"%s\"",
                                       pinfo->settings.channel_names+CHN_NAME_LENGTH*i,
                                       tpost);
                                   } else if (strstr(pinfo->settings.channel_names+
                                                     CHN_NAME_LENGTH*i,":COM:")) {
#ifdef MIDEBUG
                                     cm_msg(MLOG,"","%s: Current RAW command is \"%s\"",
                                       pinfo->settings.channel_names+CHN_NAME_LENGTH*i,
                                       tpost);
#else
                                     // DO NOTHING
#endif
                                   } else {
                                     cm_msg(MLOG,"","%s: \"%s\" is currently set",
                                       pinfo->settings.channel_names+CHN_NAME_LENGTH*i,
                                       tpost);
                                   }
                                 }
                               } else {
                                 // NIY nothing assigned to mask
                               }
                             }
                           } // for all masks
                           break;
                         }
                       } // for all channels of this type
                       break;
                     } 
                   } // for all GFA types
                 }
#endif
                 // First reading after connection was down
                 if (*(pinfo->firstread+i)) {
                   // R-flag is set
                   if (strchr(pinfo->settings.chnflags+CHNFLAGS_LENGTH*i,'R')) {
                     // demand value and readout of this channel are different
                     // send demand again 
                     if (*(pinfo->demand+i) != tval) {
                       cm_msg(MLOG,"","Channel %d(%s) UP again. Demand may not have been"
                            " set and R(eset) flag set -> Setting demand %f again",i,
                            pinfo->settings.channel_names+CHN_NAME_LENGTH*i,
                            *(pinfo->demand+i));
                       // for this channel and dependent channels when PNZV flags set 
                       // force setting demand value
                       epics_ca_set(pinfo,i,*(pinfo->demand+i),FALSE,TRUE); 
                     }
                   } 
                   *(pinfo->firstread+i) = FALSE;
                 } 
                 *(pinfo->array+i) = tval;
               }
             }
           } else {
             cm_msg(MERROR,"epics_ca_event_handler","Channel %d(%s): NULL pointer to "
                  "data returned as dbr argument!", i,
                   pinfo->settings.channel_names+CHN_NAME_LENGTH*i); 
           }
         } else {
#ifdef MIDEBUG
           cm_msg(MLOG,"","epics_ca_event_handler: channel %d(%s): Unsupported data "
                          "type %s", i, pinfo->settings.channel_names+CHN_NAME_LENGTH*i,
                          dbr_type_to_text(args.type));
#endif
         }
         found = TRUE;
         break;
       }
     }
   }

   if (!found && (args.status != ECA_NORMAL)) {
     cm_msg(MERROR,"epics_ca_event_handler","WARNING Status %d = \"%s\"", args.status, 
                                            ca_message(args.status));
   }
#ifdef MIDEBUGE1
   cm_msg(MLOG,"","--epics_ca_event_handler()");
#endif
#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : --epics_ca_event_handler()\n", ss_time());
#endif
}

/*---- device driver routines --------------------------------------*/

INT epics_ca_init(HNDLE hKey, void **pinfo, INT channels) {
   int status, i;
   HNDLE hDB, subkey;
   CA_INFO *info;
   enum channel_state castate;
   char tpath[MAX_ODB_PATH];
   char *pnames;
   BOOL loaded;

#ifdef MIDEBUGF
   sprintf(tpath,"EPICS_ca_%d.log",ss_time());
   fp = fopen(tpath,"w");
   if (fp) fprintf(fp,"%u : ++epics_ca_init(%d)\n",ss_time(),channels);
#endif

#ifdef MIDEBUG1
   cm_msg(MLOG,"","++epics_ca_init(init %d channels)",channels);
#ifdef MIDEBUGF
   cm_msg(MLOG,"","ss_time()'ed Info written to %s",tpath);
#endif
#endif

   pnames = NULL;
   loaded = FALSE;

   cm_get_experiment_database(&hDB, NULL);

   /* allocate info structure */
   info = calloc(1, sizeof(CA_INFO)); 
   *pinfo = info;       // must be done first else core dump on error return

   /* check if necessary event variables are defined */
   if (!getenv("EPICS_CA_AUTO_ADDR_LIST") || !getenv("EPICS_CA_ADDR_LIST") ||
       !getenv("EPICS_CA_SERVER_PORT")) {
      if (!getenv("EPICS_CA_AUTO_ADDR_LIST"))
        cm_msg(MERROR, "epics_ca_init", 
                       "Please define environment variable 'EPICS_CA_AUTO_ADDR_LIST'");
      if (!getenv("EPICS_CA_ADDR_LIST"))
        cm_msg(MERROR, "epics_ca_init", 
                       "Please define environment variable 'EPICS_CA_ADDR_LIST'");
      if (!getenv("EPICS_CA_SERVER_PORT"))
        cm_msg(MERROR, "epics_ca_init", 
                       "Please define environment variable 'EPICS_CA_SERVER_PORT'");
      return FE_ERR_HW;
   } else {
     cm_msg(MLOG,"","EPICS_CA_AUTO_ADDR_LIST is %s",getenv("EPICS_CA_AUTO_ADDR_LIST"));
     cm_msg(MLOG,"","EPICS_CA_ADDR_LIST is %s",     getenv("EPICS_CA_ADDR_LIST"));
     cm_msg(MLOG,"","EPICS_CA_SERVER_PORT is %s",   getenv("EPICS_CA_SERVER_PORT"));
   }

   // NIY check if caRepeater program is available 

   info->num_channels = channels;

   /* initially disable all commands */
   info->cmd_disabled = 1;

   /* initialize EPICS CA driver */
   status = ca_context_create(ca_disable_preemptive_callback);
   if (!(status & ECA_NORMAL)) {
      cm_msg(MERROR, "epics_ca_init", "Unable to initialize! ca_context_create() "
                     "returned status 0X%x = \"%s\"",status,ca_message(status));
      return FE_ERR_HW;
   }

   status = ca_add_exception_event(epics_ca_exception_handler,0);
   if (!(status & ECA_NORMAL)) 
      cm_msg(MERROR, "epics_ca_init", "Unable to install global exception handler! "
                     "ca_add_exception_handler() returned status 0X%x = \"%s\"",
                     status,ca_message(status));

   /* allocate arrays */
   info->settings.channel_names = calloc(channels, CHN_NAME_LENGTH);
#ifdef GFA_SPECIFIC
   info->settings.dtype         = calloc(channels, DTYPE_LENGTH);
   info->settings.dsec          = calloc(channels, DSEC_LENGTH);
#endif
   info->settings.chnflags      = calloc(channels, CHNFLAGS_LENGTH);
   info->settings.minval        = calloc(channels, sizeof(float));
   info->settings.maxval        = calloc(channels, sizeof(float));
   info->settings.tolerance     = calloc(channels, sizeof(float));
   info->settings.used          = calloc(channels, sizeof(BOOL));
   info->settings.readonly      = calloc(channels, sizeof(BOOL));

   info->array         = calloc(channels, sizeof(float));
   info->array_m       = calloc(channels, sizeof(float));
   info->demand        = calloc(channels, sizeof(float));
   info->threshold     = NULL;
   info->lastset       = calloc(channels, sizeof(DWORD));
   info->lastget       = calloc(channels, sizeof(DWORD));
   info->status        = calloc(channels, sizeof(int));
   info->pchid         = calloc(channels, sizeof(chid));
   info->found         = calloc(channels, sizeof(BOOL));
   info->iscmd         = calloc(channels, sizeof(BOOL));
   info->subscribed    = calloc(channels, sizeof(BOOL));
   info->down          = calloc(channels, sizeof(BOOL));
   info->firstread     = calloc(channels, sizeof(BOOL));
   info->nerr          = calloc(channels, sizeof(INT));
   info->nmsg          = calloc(channels, sizeof(INT));
   info->tolchan       = calloc(channels, sizeof(INT));
   info->gchan         = calloc(channels, sizeof(INT));
   info->intol         = calloc(channels, sizeof(BOOL));

   /* init arrays */
   for (i = 0; i < channels; i++) {
       sprintf(info->settings.channel_names+CHN_NAME_LENGTH*i, "NONE_%d", i);
#ifdef GFA_SPECIFIC
       sprintf(info->settings.dtype+DTYPE_LENGTH*i,            "**TYPE_NOT_INIT**");
       sprintf(info->settings.dsec+DSEC_LENGTH*i,              "**SEC_NOT_INIT**");
#endif
       sprintf(info->settings.chnflags+CHNFLAGS_LENGTH*i,      "US");
       *(info->settings.minval+i)   = 0.f;
       *(info->settings.maxval+i)   = 0.f;
       *(info->settings.tolerance+i)= 0.f;
   /*  *(info->settings.used+i)     = FALSE;    already init'd */
       *(info->settings.readonly+i) = TRUE;

       *(info->array+i)      = -2.f;
       *(info->array_m+i)    = -2.f;
       *(info->demand+i)     = -2.f;
    /* *(info->found+i)      = FALSE; already init'd */
    /* *(info->iscmd+i)      = FALSE; already init'd */
    /* *(info->subscribed+i) = FALSE; already init'd */
       *(info->down+i)       = TRUE;
    /* *(info->firstread+i)  = FALSE; already init'd */
    /* *(info->nerr+i)       = 0;     already init'd */
    /* *(info->nmsg+i)       = 0;     already init'd */
       *(info->tolchan+i)    = -1;
       *(info->gchan+i)      = -1;
       *(info->intol+i)      = TRUE;
    /* *(info->pchid+i)      = 0;     already init'd */
       *(info->lastset+i)    = ss_time();
    /* *(info->lastget+i)    = 0;     already init'd */
       *(info->status+i)     = ECA_NORMAL;
   }

   // remember if DD/Loadfile is already present
   status = db_find_key(hDB, hKey, "DD/Loadfile", &subkey);

   strcpy(info->settings.loadfile,"NONE");
   db_merge_data(hDB, hKey, "DD/Loadfile", info->settings.loadfile, 
                            NAME_LENGTH, 1, TID_STRING);
   // DD/Loadfile was not in ODB -> stop here user may now set loadfile name
   if (status == DB_NO_KEY) {
     cm_msg(MLOG,"","Created \"DD/Loadfile\" in ODB -> set name of loadfile and restart"
                    " to load initial setup from local loadfile");
     cm_msg(MLOG,"","Or just restart and do manual setup in ODB");
     return FE_ERR_HW;
   }

   info->settings.Sum_NOT_Connected = 0;
   db_merge_data(hDB, hKey, "DD/Sum_NOT_Connected", 
                 &info->settings.Sum_NOT_Connected, sizeof(int), 1, TID_INT);
   status = db_find_key(hDB, hKey, "DD/Sum_NOT_Connected", &info->hkeySNC);
   if (status == DB_SUCCESS) {
     if (info->settings.Sum_NOT_Connected != 0) {
       info->settings.Sum_NOT_Connected = 0;
       db_set_data(hDB, info->hkeySNC, &info->settings.Sum_NOT_Connected, 
                     sizeof(int), 1, TID_INT);
     }
   } else {
     info->hkeySNC = 0;
   }

   info->settings.Sum_NOT_in_Tolerance = 0;
   db_merge_data(hDB, hKey, "DD/Sum_NOT_in_Tolerance", 
                 &info->settings.Sum_NOT_in_Tolerance, sizeof(int), 1, TID_INT);
   status = db_find_key(hDB, hKey, "DD/Sum_NOT_in_Tolerance", &info->hkeySNT);
   if (status == DB_SUCCESS) {
     if (info->settings.Sum_NOT_in_Tolerance != 0) {
       info->settings.Sum_NOT_in_Tolerance = 0;
       db_set_data(hDB,info->hkeySNT,&info->settings.Sum_NOT_in_Tolerance,
                   sizeof(int),1,TID_INT);
     }
   } else {
     info->hkeySNT = 0;
   }

   strcpy(info->settings.AlarmWhenNOTConnected,"NONE");
   db_merge_data(hDB, hKey, "DD/AlarmWhenNOTConnected", 
                 info->settings.AlarmWhenNOTConnected, NAME_LENGTH, 1, TID_STRING);

   strcpy(info->settings.AlarmWhenNOTinTolerance,"NONE");
   db_merge_data(hDB, hKey, "DD/AlarmWhenNOTinTolerance", 
                 info->settings.AlarmWhenNOTinTolerance, NAME_LENGTH, 1, TID_STRING);

   // read info about EPICS Channel Name, Used, Readonly, Chn flags, Name, Min, Max, 
   // Threshold from a local file and write to DD if not created yet
   if (strcmp(info->settings.loadfile,"NONE") != 0){

     if (db_find_key(hDB, hKey, "DD/EPICS Channel name", &subkey) == DB_NO_KEY) {
       FILE *stream;

       i = 0;
       if ((stream = fopen(info->settings.loadfile,"r")) != NULL) {
         char tline[255];
         char *p,*pn,*po;
         int  n,k;

         pnames =      calloc(channels, NAME_LENGTH);
         info->threshold = calloc(channels, sizeof(float));
         n = 0;
         while (!feof(stream)) {
           //             1   2   3     4         5   6   7         8
           // EPICSCAname[,Y/N,Y/N,flags,MidasName,Min,Max,Threshold,Toler] # comments
           if (fgets(tline, sizeof(tline), stream) != NULL) {
             n++;
             if ((p = strchr(tline,'#'))) *p = '\0';  // remove comments

             if (strlen(tline) > 0) { // make sure last field on line ends with ,
               int j;
               for (j=strlen(tline)-1; j >= 0; j--) {
                 if (!isspace(tline[j])) { // remove trailing blanks
                   if (tline[j] != ',') {tline[j+1] = ','; tline[j+2] = '\0';}
                   break;
                 }
               }
             }
             if ((strlen(tline) > 0)&&(p = strchr(tline,','))) { // EPICS channel name
               if (i < channels) {
                 cm_msg(MLOG,"","Channel %d: Processing Line %d: %s", i, n, tline);
                 pn = p+1; *p = '\0';
                 po = &tline[0];

                 for (k=strlen(po)-1; k >= 0; k--) // remove trailing blanks
                   if (isspace(*(po+k))) *(po+k)='\0'; else break;
                 while (isspace(*po)) po++;        // remove leading blanks

                 if (strlen(po) >= CHN_NAME_LENGTH) *(po+CHN_NAME_LENGTH-1) = '\0'; 
#ifdef GFA_SPECIFIC
                 // NIY check GFA convention ?
#endif
                 strcpy(info->settings.channel_names+ CHN_NAME_LENGTH*i,po);

                 // take EPICS Channel as Midas Name default (maybe overwritten below)
                 if (strlen(po) >= NAME_LENGTH) *(po+NAME_LENGTH-1) = '\0'; 
                 strcpy(pnames+NAME_LENGTH*i,po);

                 if (info->threshold) *(info->threshold+i) = 0.1f; 
               
                 if (p = strchr(pn,',')) {  // used 
                   po = pn; pn = p+1; *p = '\0';
                   if (strchr(po,'y')||strchr(po,'Y')) *(info->settings.used+i) = TRUE;

                   if (p = strchr(pn,',')) { // readonly
                     po = pn; pn = p+1; *p = '\0';
                     if (strchr(po,'n')||strchr(po,'N')) 
                       *(info->settings.readonly+i)=FALSE;

                     if (p = strchr(pn,',')) { // channel flags
                       po = pn; pn = p+1; *p = '\0'; 
                       for (k=strlen(po)-1; k >= 0; k--) // remove trailing blanks
                         if (isspace(*(po+k))) *(po+k)='\0'; else break;
                       if (strlen(po) >= CHNFLAGS_LENGTH) *(po+CHNFLAGS_LENGTH-1) = '\0';
                       if (strlen(po) > 0) {
                         int k1;
                         
                         // remove blanks
                         for (k=0, k1=0; k < strlen(po); k++) {
                           *(info->settings.chnflags+ CHNFLAGS_LENGTH*i+k) = '\0';
                           if (!isspace(*(po+k))) {
#ifdef GFA_SPECIFIC
                             // NIY check flags also with/without GFA_SPECIFIC
                             // NIY T should only be set for SOL, IST or POSA
#else
#endif
                             // NIY R should only be set for :SOL: or maybe :COM:
                             *(info->settings.chnflags+ CHNFLAGS_LENGTH*i+k1) = *(po+k);
                             k1++;
                             *(info->settings.chnflags+ CHNFLAGS_LENGTH*i+k1) = '\0';
                           }
                         }
                       }

                       if ((p = strchr(pn,','))) { // Midas Name
                         po = pn; pn = p+1; *p = '\0';
                         for (k=strlen(po)-1; k >= 0; k--) // remove trailing blanks
                           if (isspace(*(po+k))) *(po+k)='\0'; else break;
                         while (isspace(*po)) po++;        // remove leading blanks
                         if (strlen(po) >= NAME_LENGTH) *(po+NAME_LENGTH-1) = '\0';
                         if (strlen(po) > 0) {
                           strcpy(pnames+NAME_LENGTH*i,po);
                         }

                         if ((p = strchr(pn,','))) { // Minval
                           po = pn; pn = p+1; *p = '\0';
                           for (k=strlen(po)-1; k >= 0; k--) // remove trailing blanks
                             if (isspace(*(po+k))) *(po+k)='\0'; else break;
                           if (strlen(po) > 0) {
                             float tval;
                             if (sscanf(po,"%f",&tval) == 1) 
                               *(info->settings.minval+i) = tval;
                           }

                           if ((p = strchr(pn,','))) { // Maxval
                             po = pn; pn = p+1; *p = '\0';
                             for (k=strlen(po)-1; k >= 0; k--) // remove trailing blanks
                               if (isspace(*(po+k))) *(po+k)='\0'; else break;
                             if (strlen(po) > 0) {
                               float tval;
                               if (sscanf(po,"%f",&tval) == 1) 
                                 *(info->settings.maxval+i) = tval;
                             }

                             if ((p = strchr(pn,','))) { // threshold
                               po = pn; pn = p+1; *p = '\0';
                               for (k=strlen(po)-1; k >= 0; k--)// remove trailing blanks
                                 if (isspace(*(po+k))) *(po+k)='\0'; else break;
                               if (strlen(po) > 0) {
                                 float tval;
                                 if ((sscanf(po,"%f",&tval) == 1) && info->threshold) 
                                   *(info->threshold+i) = tval;
                               }

                               for (k=strlen(pn)-1; k >= 0; k--)// remove trailing blanks
                                 if (isspace(*(pn+k))) *(pn+k)='\0'; else break;
                               if (strlen(pn) > 0) {   // rest is tolerance
                                 float tval;
                                 if (sscanf(pn,"%f",&tval) == 1) 
                                   *(info->settings.tolerance+i) = tval;
                               }
                             }
                           }
                         }
                       }
                     }
                   }
                 }
                 // NIY check min < max
                 // NIY check threshold > 0
#ifdef GFA_SPECIFIC
                 // NIY check tolerance >= 0 when T flag
#endif
               } else {
                 cm_msg(MLOG,"","Line %d not handled! %s", n, tline);
               }
               i++;
             }
           } 
         } // while not EOF 

         if (i < channels) {
           cm_msg(MLOG,"","Less channels (%d) loaded from %s than expected (%d) " 
                          "-> recompile with %d or add more channel lines", i, 
                          info->settings.loadfile, channels, i);
           for (; i < channels; i++) { // init rest to defaults
             sprintf(pnames+NAME_LENGTH*i,"NONE_%d",i);
             if (info->threshold) *(info->threshold+i) = 0.1f; 
           } 
         } else if (i > channels) {
           cm_msg(MLOG,"","More channels (%d) read from %s than expected (%d) " 
                          "-> recompile with %d or remove channel lines", i, 
                          info->settings.loadfile, channels, i);
         } else {
           cm_msg(MLOG,"","Expected number of channels (%d) read ", channels);
         }
         if (i > 0) loaded = TRUE; 
         fclose(stream);
       } else {
         cm_msg(MLOG,"","ERROR %d returned when trying to open %s for read access!",
                        errno, info->settings.loadfile);
       }
     }
   } 
  
   /* init or get channel names, Used and Readonly DD settings */
   db_merge_data(hDB, hKey, "DD/EPICS Channel name", info->settings.channel_names, 
                            CHN_NAME_LENGTH * channels, channels, TID_STRING);
   db_merge_data(hDB, hKey, "DD/Used", info->settings.used, 
                            sizeof(BOOL)* channels, channels, TID_BOOL);
   db_merge_data(hDB, hKey, "DD/Readonly", info->settings.readonly, 
                            sizeof(BOOL)* channels, channels, TID_BOOL);
   db_merge_data(hDB, hKey, "DD/Chn_Flags", info->settings.chnflags, 
                            CHNFLAGS_LENGTH * channels, channels, TID_STRING);
#ifdef GFA_SPECIFIC
   db_merge_data(hDB, hKey, "DD/DESC:TYP", info->settings.dtype, 
                            DTYPE_LENGTH * channels, channels, TID_STRING);
   db_merge_data(hDB, hKey, "DD/DESC:SEC", info->settings.dsec, 
                            DSEC_LENGTH * channels, channels, TID_STRING);
#endif
   if (loaded) { // min and max were loaded write them now
     db_merge_data(hDB, hKey, "DD/Minval", info->settings.minval, 
                              sizeof(float)* channels, channels, TID_FLOAT);
     db_merge_data(hDB, hKey, "DD/Maxval", info->settings.maxval, 
                              sizeof(float)* channels, channels, TID_FLOAT);
   }

   db_merge_data(hDB, hKey, "DD/Tolerance", info->settings.tolerance, 
                            sizeof(float)* channels, channels, TID_FLOAT);

   /* -- search for channels */

#ifdef GFA_SPECIFIC
   /* search for <gfa_device>:DESC:TYP and <gfa_device>:DESC:SEC  to 
    * get device type and section of channel */
   {
     chid *tchidt;
     chid *tchids;
     char tname[2*CHN_NAME_LENGTH]; 
     HNDLE tkey;

     tchidt = calloc(channels, sizeof(chid));
     tchids = calloc(channels, sizeof(chid));

     if (tchidt && tchids) {
       /* post channel search to IO buffer */
       for (i = 0; i < channels; i++) {
         if (*(info->settings.used+i)) {
           char *tpos;

           strcpy(tname,info->settings.channel_names+CHN_NAME_LENGTH*i);
           tpos = strchr(tname,':');
           /*
            * GFA specific: devices of the same type have the same channel names
            * channel name is <gfa_device_name>:<gfa_device_specific_channel_info>
            * e.g.: BMB1:COM:2   to set/read demand  or BMB1:STA:1 to read status
            *       <gfa_device>:DESC:TYP contains info about GFA device type
            *       <gfa_device>:DESC:SEC contains info about GFA device section
            */
           if (tpos) *tpos = '\0';
           strcat(tname,":DESC:TYP");           
           status = ca_create_channel(tname,NULL,NULL,0,tchidt+i);
           if (!(status & ECA_NORMAL))
             cm_msg(MERROR, "epics_ca_init", "ERROR status 0X%x = \"%s\" returned by "
               "ca_create_channel(%s)", status, ca_message(status), tname);
         }
       }

       for (i = 0; i < channels; i++) {
         if (*(info->settings.used+i)) {
           char *tpos;

           strcpy(tname,info->settings.channel_names+CHN_NAME_LENGTH*i);
           tpos = strchr(tname,':');
           if (tpos) *tpos = '\0';
           strcat(tname,":DESC:SEC");           
           status = ca_create_channel(tname,NULL,NULL,0,tchids+i);
           if (!(status & ECA_NORMAL))
             cm_msg(MERROR, "epics_ca_init", "ERROR status 0X%x = \"%s\" returned by "
               "ca_create_channel(%s)", status, ca_message(status), tname);
         }
       }

       /* flush send buffer and block for outstanding queries */
       status = ca_pend_io(5.0);
       if (status == ECA_TIMEOUT) status = ca_pend_io(5.0);
       if (status == ECA_TIMEOUT) status = ca_pend_io(5.0);
#ifdef MIDEBUGP
       cm_msg(MLOG,"","ca_pend_io(5.0) status 0X%x = \"%s\"",status,ca_message(status));
#endif
       /* if channel is connected post get channel info to IO buffer */
       for (i = 0; i < channels; i++) {
         if (*(info->settings.used+i)) { 
           strcpy(info->settings.dtype+DTYPE_LENGTH*i, "**TYPE_NOT_RETURNED**");
           if (ca_state(*(tchidt+i)) == cs_conn) {
             status = ca_get(DBR_STRING,*(tchidt+i),info->settings.dtype+DTYPE_LENGTH*i);
             if (status != ECA_NORMAL)
               cm_msg(MLOG,"","ca_get(dtype of channel %d): status 0X%x = \"%s\"",i,
                              status,ca_message(status));
           } else {
             strcpy(info->settings.dtype+DTYPE_LENGTH*i, "**TYPE_NOT_FOUND**");
           }
         } else {
           strcpy(info->settings.dtype+DTYPE_LENGTH*i, "**CHAN_NOT_USED**");
         }
       }

       for (i = 0; i < channels; i++) {
         if (*(info->settings.used+i)) { 
           strcpy(info->settings.dsec+DSEC_LENGTH*i, "**SEC_NOT_RETURNED**");
           if (ca_state(*(tchids+i)) == cs_conn) {
             status = ca_get(DBR_STRING,*(tchids+i),info->settings.dsec+DSEC_LENGTH*i);
             if (status != ECA_NORMAL)
               cm_msg(MLOG,"","ca_get(dsec of channel %d): status 0X%x = \"%s\"",i,
                              status,ca_message(status));
           } else {
             strcpy(info->settings.dsec+DSEC_LENGTH*i, "**SEC_NOT_FOUND**");
           }
         } else {
           strcpy(info->settings.dsec+DSEC_LENGTH*i, "**CHAN_NOT_USED**");
         }
       }
       status = ca_pend_io(1.0);  /* flush buffer and block for outstanding q's */
       if (status == ECA_TIMEOUT) status = ca_pend_io(1.0);
#ifdef MIDEBUGP
       cm_msg(MLOG,"","ca_pend_io(1.0) status 0X%x = \"%s\"",status,ca_message(status));
#endif
       /* NOTE: status is not evaluated */

#ifdef MIDEBUG2
       cm_msg(MLOG,"","ss_sleep(2000)");
#endif
       ss_sleep(2000);            /* wait some time */

       status = ca_pend_event(1.0);  /* probably not necessary to wait again */   
#ifdef MIDEBUGP
       cm_msg(MLOG,"","ca_pend_event(1.0)");
#endif

       // (re)set info when not connected then release channels
       for (i = 0; i < channels; i++) {
         if (*(tchidt+i)) {
           if (*(info->settings.used+i)) {
             if (ca_state(*(tchidt+i)) != cs_conn) {
               // some beamline channel locks (KanalverschlusS) are not fully
               // implemented in EPICS therefore supply TYP information from chan name
               if (strncmp(info->settings.channel_names+CHN_NAME_LENGTH*i,"KS",2)==0)
                 strcpy(info->settings.dtype+DTYPE_LENGTH*i, "BX");
               // separator HV and spinrotator HV are from another server
               else if ((strncmp(info->settings.channel_names+CHN_NAME_LENGTH*i,"SEP",3) 
                   ==0) && 
                  (strstr(info->settings.channel_names+CHN_NAME_LENGTH*i,"HVN:") ||
                   strstr(info->settings.channel_names+CHN_NAME_LENGTH*i,"HVP:") ||
                   strstr(info->settings.channel_names+CHN_NAME_LENGTH*i,"HV:")))
                 strcpy(info->settings.dtype+DTYPE_LENGTH*i, "HVPS");
               else
                 strcpy(info->settings.dtype+DTYPE_LENGTH*i, "**TYPE_NOT_FOUND**");
             }
           } 
           status = ca_clear_channel(*(tchidt+i));
           if (!(status & ECA_NORMAL))
             cm_msg(MERROR, "epics_ca_init", ":DESC:TYP of Channel %d(%s): "
                            "ca_clear_channel() returned status 0X%x = \"%s\"",i,
                            info->settings.channel_names+CHN_NAME_LENGTH*i,
                            status, ca_message(status));
         }
       }
       for (i = 0; i < channels; i++) {
         if (*(tchids+i)) {
           if (*(info->settings.used+i)) {
             if (ca_state(*(tchids+i)) != cs_conn) {
               // spinrotator/separator HV is not fully implemented
               // in EPICS therefore supply SEC information from channel name
               if (strncmp(info->settings.channel_names+CHN_NAME_LENGTH*i,"sep31:",6)==0)
                 strcpy(info->settings.dsec+DSEC_LENGTH*i, "PIM31");
               else
                 strcpy(info->settings.dsec+DSEC_LENGTH*i, "**SEC_NOT_FOUND**");
             }
           }
           status = ca_clear_channel(*(tchids+i));
           if (!(status & ECA_NORMAL))
             cm_msg(MERROR, "epics_ca_init", ":DESC:SEC of Channel %d(%s): "
                            "ca_clear_channel() returned status 0X%x = \"%s\"",i,
                            info->settings.channel_names+CHN_NAME_LENGTH*i,
                            status, ca_message(status));
         }
       }
       status = ca_pend_io(1.0); /* flush buffer */
       if (status == ECA_TIMEOUT) status = ca_pend_io(1.0);
#ifdef MIDEBUGP
       cm_msg(MLOG,"","ca_pend_io(1.0) status 0X%x = \"%s\"",status,ca_message(status));
#endif

     }

     if (tchidt)  free(tchidt);
     if (tchids)  free(tchids);

     /* update ODB */
     status = db_find_key(hDB, hKey, "DD/DESC:TYP", &tkey);
     if (status == DB_SUCCESS) 
       db_set_data(hDB, tkey, info->settings.dtype, DTYPE_LENGTH * channels, 
                   channels, TID_STRING);

     status = db_find_key(hDB, hKey, "DD/DESC:SEC", &tkey);
     if (status == DB_SUCCESS) 
       db_set_data(hDB, tkey, info->settings.dsec, DSEC_LENGTH * channels, 
                   channels, TID_STRING);

   }
#endif /* #ifdef GFA_SPECIFIC */

   /* disable event handler callback subscription in the connection handler callback
    * before everything is set-up */
   info->subscribe_disabled = TRUE;
 
   /* post create channel connection to IO buffer */
   for (i = 0; i < channels; i++) {
     if (*(info->settings.used+i)) {
        /* NOTE: connection handler used -> ca_pend_io() will not block - waiting for 
         *       channel to enter connected state */               
        status = ca_create_channel(info->settings.channel_names+CHN_NAME_LENGTH*i,
                                   epics_ca_connection_handler, info, 0, info->pchid+i);
        if (!(status & ECA_NORMAL))
           cm_msg(MERROR, "epics_ca_init", "ERROR status 0X%x = \"%s\" returned by "
                          "ca_create_channel(%s)", status, ca_message(status),
                          info->settings.channel_names+CHN_NAME_LENGTH*i);
     }
   }

   /* flush send buffer */
   status = ca_pend_io(5.0);
   if (status == ECA_TIMEOUT) status = ca_pend_io(5.0);
   if (status == ECA_TIMEOUT) status = ca_pend_io(5.0);
   if (status == ECA_TIMEOUT) status = ca_pend_io(5.0);
   if (status == ECA_TIMEOUT) status = ca_pend_io(5.0);
#ifdef MIDEBUGP
   cm_msg(MLOG,"","ca_pend_io(5.0) status 0X%x = \"%s\"", status, ca_message(status));
#endif
#ifdef MIDEBUG2
   cm_msg(MLOG,"","ss_sleep(5000)");
#endif
   ss_sleep(5000);
   ca_pend_event(4.0);
#ifdef MIDEBUGP
   cm_msg(MLOG,"","ca_pend_event(4.0)");
#endif

   /* timeout occured or normal (ca_pend_io,only) -> process channels */
   if ((status == ECA_TIMEOUT) || (status == ECA_NORMAL)){

     for (i = 0; i < channels; i++) {
       if (*(info->settings.used+i)) {
         if ((castate=ca_state(*(info->pchid+i))) != cs_conn) {
           cm_msg(MERROR, "epics_ca_init", "Not connected to EPICS Channel %d(%s). "
                                           "Channel state is %d = \"%s\"",i, 
                                      info->settings.channel_names+CHN_NAME_LENGTH*i, 
                                           castate, castatestr(castate));
         } else {
           if (!*(info->found+i)) {
             cm_msg(MLOG, "epics_ca_init", "Connected to EPICS Channel %d(%s)", i,
                                         info->settings.channel_names+CHN_NAME_LENGTH*i);
             *(info->found+i) = TRUE;
           }
         }
       }
     }

   } else {
     cm_msg(MLOG,"epics_ca_init","Returned status %d = \"%s\" of ca_pend_io(A) is not "
                                 "handled", status, ca_message(status));
   }

   // get MIN/MAX if not already present 
   // NOTE: some channels may still be missing 
                             status = db_find_key(hDB, hKey, "DD/Minval", &subkey);
   if (status !=  DB_NO_KEY) status = db_find_key(hDB, hKey, "DD/Maxval", &subkey);

   if (status == DB_NO_KEY) {
     struct dbr_ctrl_float *pctrl;

     cm_msg(MLOG,"","Query LOW and HIGH of channels");
     pctrl = calloc(channels,sizeof(struct dbr_ctrl_float));
     if (pctrl) {
       int nchans;
       for (nchans = 0, i = 0; i < channels; i++) {
         if (*(info->settings.used+i)) {
           if ((castate=ca_state(*(info->pchid+i))) == cs_conn) {
             if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'U') == NULL) {
               if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'S') == NULL) {
                 if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'M') != NULL) {
                   status = ca_get(DBR_CTRL_FLOAT,*(info->pchid+i),pctrl+i);
                   if (status != ECA_NORMAL)
                     cm_msg(MLOG,"","ca_get(DBR_CTRL_FLOAT of channel %d): "
                                    "status 0X%x = \"%s\"",i,status,ca_message(status));
                   nchans++;
                 } else {
                   cm_msg(MLOG,"","Channel %d M flag not set", i);
                 }
               } else {
                 cm_msg(MLOG,"","Channel %d S flag set", i);
               }
             } else {
               cm_msg(MLOG,"","Channel %d U flag set", i);
             }
           } else {
             cm_msg(MLOG,"","Channel %d not connected", i);
           }
         } else {
           cm_msg(MLOG,"","Channel %d not used", i);
         }
       }

       if (nchans>0) {
         cm_msg(MLOG,"","Queried LOW and HIGH of %d channels",nchans);
         /* flush send buffer */
         status = ca_pend_io(5.0);
         if (status == ECA_TIMEOUT) status = ca_pend_io(5.0);
         if (status == ECA_TIMEOUT) status = ca_pend_io(5.0);
#ifdef MIDEBUGP
         cm_msg(MLOG,"","ca_pend_io(5.0) status 0X%x =\"%s\"",status,ca_message(status));
#endif
#ifdef MIDEBUG2
         cm_msg(MLOG,"","ss_sleep(1000)");
#endif
         ss_sleep(1000);
         ca_pend_event(4.0);
#ifdef MIDEBUGP
         cm_msg(MLOG,"","ca_pend_event(1.0)");
#endif
         for (i = 0; i < channels; i++) {
           if (*(info->settings.used+i)) {
             if ((castate=ca_state(*(info->pchid+i))) == cs_conn) {
               if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'U') == NULL) {
                 if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'S') == NULL) {
                   if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'M') != NULL) {
                     *(info->settings.minval+i) = (pctrl+i)->lower_ctrl_limit;
                      cm_msg(MLOG,"","Channel %d(%s): LOW is %f",
                          i,info->settings.channel_names+CHN_NAME_LENGTH*i,
                          *(info->settings.minval+i)); 
                     *(info->settings.maxval+i) = (pctrl+i)->upper_ctrl_limit; 
                      cm_msg(MLOG,"","Channel %d(%s): HIGH is %f",
                          i,info->settings.channel_names+CHN_NAME_LENGTH*i,
                          *(info->settings.maxval+i)); 
                   }
                 }
               }
             }
           }
         }
       }
       free(pctrl);
     } else {
       cm_msg(MERROR,"epics_ca_init","Memory not allocated for struct dbr_ctrl_float");
     }
   }

   /* create and init or read DD/Minval DD/Maxval */
   db_merge_data(hDB, hKey, "DD/Minval", info->settings.minval, 
                            sizeof(float)* channels, channels, TID_FLOAT);
   db_merge_data(hDB, hKey, "DD/Maxval", info->settings.maxval, 
                            sizeof(float)* channels, channels, TID_FLOAT);

   /* enable event handler callback subscription in connection handler */
   info->subscribe_disabled = FALSE;

   /* buffer add change notifications */
   for (i = 0; i < channels; i++) {
     /* only for connected channels */
     if (*(info->settings.used+i) && ((castate=ca_state(info->pchid[i])) == cs_conn)) {
       // chan specific: omit channels with S-flag e.g.: returning strings
       //                and :COM:2 channels
       if ((strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'S') == NULL) &&
           (strstr(info->settings.channel_names+CHN_NAME_LENGTH*i,":COM:2") == NULL)){
         // subscribe event handler callback for this channel converting value to float
         status = ca_create_subscription(DBR_FLOAT, 1, *(info->pchid+i), DBE_VALUE, 
                                         epics_ca_event_handler, info, NULL);
         if (!(status & ECA_NORMAL)) {
           cm_msg(MERROR,"epics_ca_init","Channel %d(%s): ca_create_subscription() retur"
             "ned status 0x%X = \"%s\"",i,info->settings.channel_names+CHN_NAME_LENGTH*i,
             status, ca_message(status));
         } else {
#ifdef MIDEBUG1
           cm_msg(MLOG,"","Channel %d(%s): CA event handler (float) will be installed",
                          i,info->settings.channel_names+CHN_NAME_LENGTH*i); 
#endif
           *(info->subscribed+i) = TRUE;
         }
       } else {
         if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'S'))
           cm_msg(MLOG,"","Channel %d(%s): S flag: CA event handler will NOT be "
             "installed", i,info->settings.channel_names+CHN_NAME_LENGTH*i); 
#ifdef MIDEBUG
         else if (strstr(info->settings.channel_names+CHN_NAME_LENGTH*i,":COM:2"))
           cm_msg(MLOG,"","Channel %d(%s): COM Channel : CA event handler will NOT "
             "be installed",i,info->settings.channel_names+CHN_NAME_LENGTH*i); 
#endif
         *(info->subscribed+i) = TRUE; // mark as handled
       }
     //if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'C') != NULL)
         *(info->down+i) = FALSE;  // channel is currently up
     } else {
     //if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'C') != NULL)
         *(info->down+i) = TRUE;  // channel is currently down
     }
   }

   /* flush send buffer and block for outstanding queries */
   status = ca_pend_io(5.0);
   if (status == ECA_TIMEOUT) status = ca_pend_io(5.0);
#ifdef MIDEBUGP
   cm_msg(MLOG,"","ca_pend_io(5.0) status 0X%x = \"%s\"", status, ca_message(status));
#endif
#ifdef MIDEBUG2
   cm_msg(MLOG,"","ss_sleep(2000)");
#endif
   ss_sleep(2000);  // wait fixed time interval for (initial) events

   status = ca_pend_event(5.0);  // process receive buffer to handle events
#ifdef MIDEBUGP
   cm_msg(MLOG,"","ca_pend_event(5.0)");
#endif
   if ((status == ECA_TIMEOUT) || (status == ECA_NORMAL)) {
      for (i = 0; i < channels; i++) {
        if (*(info->settings.used+i) && *(info->found+i)) {
          if (ca_state(info->pchid[i]) != cs_conn)
             cm_msg(MERROR, "epics_ca_init", "Not connected to %s",
                                     info->settings.channel_names+CHN_NAME_LENGTH*i);
        } else if (*(info->found+i)){
          if (*(info->settings.used+i)) {
            cm_msg(MLOG,"","Channel %d(%s): Not connected (state %d = \"%s\") -> no CA "
                           "callback installed and channel disabled", i,
                           info->settings.channel_names+CHN_NAME_LENGTH*i, castate, 
                           castatestr(castate));
          }
          *(info->found+i) = FALSE;
        }
      }
   } else if (status != ECA_NORMAL) {
#ifdef MIDEBUG
     cm_msg(MLOG,"epics_ca_init","Returned status %d \"%s\" of ca_pend_io(B) is not "
                                 "handled", status, ca_message(status));
#endif
   }

   /* log information */
   for (i = 0; i < channels; i++) {
     if (*(info->settings.used+i)) {
         // R only for :COM: or :SOL: 
#ifdef GFA_SPECIFIC
       if (strstr(info->settings.channel_names+CHN_NAME_LENGTH*i,":COM:") ||
           strstr(info->settings.channel_names+CHN_NAME_LENGTH*i,":SOL:"))
         sprintf(smsg,"Channel %d(%s): %s Type: %s Section: %s Chn_Flags: %s", i,
                      info->settings.channel_names+CHN_NAME_LENGTH*i, 
                      *(info->settings.readonly+i)?"R":"W",
                      info->settings.dtype+DTYPE_LENGTH *i,
                      info->settings.dsec+DSEC_LENGTH*i,
                      info->settings.chnflags+CHNFLAGS_LENGTH*i);
       else
         sprintf(smsg,"Channel %d(%s): R Type: %s Section: %s Chn_Flags: %s", i,
                      info->settings.channel_names+CHN_NAME_LENGTH*i, 
                      info->settings.dtype+DTYPE_LENGTH *i,
                      info->settings.dsec+DSEC_LENGTH*i,
                      info->settings.chnflags+CHNFLAGS_LENGTH*i);
#else
       sprintf(smsg,"Channel %d(%s): %s Chn_Flags: %s", i,
                    info->settings.channel_names+CHN_NAME_LENGTH*i,
                    *(info->readonly+i)?"R":"W",
                    info->settings.chnflags+CHNFLAGS_LENGTH*i);
#endif /* #ifdef GFA_SPECIFIC */

       if ((strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'U'))||
           (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'S'))) {
         if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'I')||
             strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'F')||
             strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'M')||
             strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'C')||
#ifdef GFA_SPECIFIC
             strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'P')||
             strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'N')||
             strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'Z')||
             strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'V')||
             strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'T')||
             strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'G')||
#endif
             strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'R')  ) 
           strcat(smsg," ** Other flag(s) overruled by U and/or S **");
       } else {
         if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'M')) {
           sprintf(&smsg[strlen(smsg)]," Minval = %.2f, Maxval = %.2f",
                   *(info->settings.minval+i), *(info->settings.maxval+i));
         }
#ifdef GFA_SPECIFIC
         // NIY check and list additional info (tolerance when T, 
         if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'T')) {
           if ( *(info->settings.tolerance+i) >= 0.f) 
             sprintf(&smsg[strlen(smsg)]," Tolerance = %f",
                                         *(info->settings.tolerance+i));
         }
#endif
       }
       cm_msg(MLOG,"",smsg);
     } else {
       sprintf(smsg,"Channel %d(%s): NOT Used", i,
                      info->settings.channel_names+CHN_NAME_LENGTH*i);
       cm_msg(MLOG,"",smsg);
     }
   }

#ifndef OMIT_ODBUPDATE
   // get equipment name
   if ((status = db_get_path(hDB, hKey, tpath, sizeof(tpath))) == DB_SUCCESS) {
     int i;
     for (i = strlen("/Equipment/"); i < strlen(tpath); i++) {
       if (tpath[i] == '/') {
         tpath[i] = '\0';
         strncpy(info->name, &tpath[strlen("/Equipment/")], NAME_LENGTH);
         info->name[NAME_LENGTH - 1] = '\0';
         sprintf(tpath, "/Equipment/%s/Variables/Demand",info->name);
         status = db_find_key(hDB, 0, tpath, &info->hkeyDemand);
         if (status != DB_SUCCESS) info->hkeyDemand = 0;
         break;
       }
     }
     cm_msg(MINFO, "", "epics_ca_init: Equipment name is %s", info->name);
   } else {
     cm_msg(MERROR, "epics_ca_init","ERROR %d getting key path", status);
   }
#else 
   info->hkeyDemand = 0;
#endif

#ifndef OMIT_ODBUPDATE
   // directly init ODB settings of Names
   if (pnames != NULL) {
     sprintf(tpath, "/Equipment/%s/Settings/Names",info->name);
     db_merge_data(hDB, 0, tpath, pnames, NAME_LENGTH*channels, channels, TID_STRING);
     free(pnames);
     pnames = NULL;
   }
#endif

#ifndef OMIT_ODBUPDATE
   // directly init ODB settings of thresholds
   if (info->threshold != NULL) {
     // NIY TODO does not work
     //sprintf(tpath, "/Equipment/%s/Settings/Update Threshold Measured",info->name);
     //db_merge_data(hDB, 0, tpath, info->threshold, sizeof(float)* channels, channels, 
     //              TID_FLOAT);
     //free(info->threshold);
     //info->threshold = NULL;
   }
#endif

#ifdef GFA_SPECIFIC

  /* list GFA types, their channels, channel masks, values and corresp. messages */
#ifdef MIDEBUG
  cm_msg(MLOG, "", "List of used GFA types");

  for (i=0; i < N_GFA_TYPES; i++) {
    int  n,j;
    BOOL found;    
    GFA_channel *tc;

    for (found=FALSE,n=0; n < info->num_channels; n++) {
      if (strcmp(info->settings.dtype+DTYPE_LENGTH*n,gfatype[i].type) == 0) {
        found = TRUE;
        break;
      }
    }

    if (!found) continue;

    cm_msg(MLOG,"","TYPE: \"%s\"  \"%s\"  %d channels", gfatype[i].type, gfatype[i].desc,
                                                        gfatype[i].nc);
    tc = gfatype[i].tc;

    for (j = 0; j < gfatype[i].nc; j++) {
      int k;
      GFA_mask *cm;
      
      if (!(tc+j)) continue;
#ifdef MIDEBUG1
      cm_msg(MLOG,"","**Channel \"%s\" \"%s\" %d masks",(tc+j)->spec,(tc+j)->desc,
                                                      (tc+j)->ns); 
#else
      cm_msg(MLOG,"","**Channel \"%s\" \"%s\"",(tc+j)->spec,(tc+j)->desc); 
#endif
      cm = (tc+j)->cm;
      for (k = 0; k < (tc+j)->ns; k++) {
        int l;
        GFA_msg      *sm;

        if (!(cm+k)) continue;
        cm_msg(MLOG,"","****Mask 0x%4.4X",(cm+k)->mask);
        sm = (cm+k)->sm;
        for (l = 0; l < (cm+k)->nm; l++) {
          if (!(sm+l)) continue;
          cm_msg(MLOG,"","******Value 0x%4.4X : \"%s\"",(sm+l)->value, (sm+l)->msg);
        }
        
      }
    }
  }
#endif

  // check if GFA type is in list and channel is known
  //     when not write message NO GFA specific handling possible for TYP
  for (i=0; i < info->num_channels; i++) {
    int  n;
    BOOL found;    
    for (found=FALSE,n=0; n < N_GFA_TYPES; n++) {
      if (strcmp(info->settings.dtype+DTYPE_LENGTH*i,gfatype[n].type) == 0) {
        found = TRUE;
        break;
      }
    }

    if (found) {
        int m;
        char *tpos;

        tpos = strchr(info->settings.channel_names+CHN_NAME_LENGTH*i,':');
        if (tpos) {
          for (found = FALSE, m=0; m < gfatype[n].nc; m++) { //loop over channels of type
            if ((gfatype[n].tc+m) && (strcmp((gfatype[n].tc+m)->spec,tpos)==0)) {
              found = TRUE;                                 // channel spec found
              *(info->iscmd+i) = (gfatype[n].tc+m)->iscmd;  // copy iscmd flag of spec
              break;
            } 
          }
          if (!found && (strcmp(info->settings.dtype+DTYPE_LENGTH*i,"HVPS")==0)) {
            for (found = FALSE, m=0; m < gfatype[n].nc; m++) { //loop over chans of type
              if ((gfatype[n].tc+m) && 
                   strstr(info->settings.channel_names+CHN_NAME_LENGTH*i,
                          (gfatype[n].tc+m)->spec)) {
                found = TRUE;                                 // channel spec found
                *(info->iscmd+i) = (gfatype[n].tc+m)->iscmd;  // copy iscmd flag of spec
                break;
              } 
            }
          }
          if (!found) {
            cm_msg(MLOG,"","Channel %d(%s): Not found in list of handled channels for "
                           "GFA type \"%s\"", i, 
                           info->settings.channel_names+CHN_NAME_LENGTH*i,
                           info->settings.dtype+DTYPE_LENGTH*i); 
            if (!*(info->settings.readonly+i)) // not found but write flag set
              *(info->iscmd+i) = TRUE;         // assume command
          }
        }
    } else {
      // omit channels with unknown types
      if (strncmp(info->settings.dtype+DTYPE_LENGTH*i,"**",2) == 0) continue;

      cm_msg(MLOG,"","Channel %d(%s): GFA Type \"%s\" not found in list"
                     " of handled GFA types!", i, 
                     info->settings.channel_names+CHN_NAME_LENGTH*i,
                     info->settings.dtype+DTYPE_LENGTH*i); 
    }
  }
#else
  for (i=0; i < info->num_channels; i++) {
    if (!*(info->settings.readonly+i)) // write flag set -> assume command
      *(info->iscmd+i) = TRUE;
  }
#endif /* #ifdef GFA_SPECIFIC */

  // reset alarms
  if (strncmp(info->settings.AlarmWhenNOTConnected,"NONE",4) != 0)
    al_reset_alarm(info->settings.AlarmWhenNOTConnected);

  if (strncmp(info->settings.AlarmWhenNOTinTolerance,"NONE",4) != 0)
    al_reset_alarm(info->settings.AlarmWhenNOTinTolerance);


  // initialize demand of channel (and array_m) from readout array 
  // if iscmd flag is set and not down
  // else init mirror array_m from readout array
  for (i=0; i < info->num_channels; i++)
    if (*(info->iscmd+i) && !*(info->down+i))
      epics_ca_get_demand(info, i, info->demand+i);
    else
      *(info->array_m+i) = *(info->array+i);

   /* got here without error -> enable all commands */
   info->cmd_disabled = FALSE;

#ifdef MIDEBUG1
   cm_msg(MLOG,"","--epics_ca_init()");
#endif
#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : --epics_ca_init()\n", ss_time());
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_exit(CA_INFO *info) {
  int i, status;

#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : ++epics_ca_exit()\n", ss_time());
#endif
#ifdef MIDEBUG1
   cm_msg(MLOG,"","++epics_ca_exit()");
#endif

  if (info) {
    // NOTE: not necessary to call ca_clear_subscription() 

    // release channels and event subscription
    for (i = 0; i < info->num_channels; i++) {
      if (*(info->pchid+i)) { 
        status = ca_clear_channel(*(info->pchid+i));
        if (!(status & ECA_NORMAL))
          cm_msg(MERROR, "epics_ca_init", "Channel %d(%s): ca_clear_channel() returned "
                         "status 0X%x = \"%s\"",i,
                         info->settings.channel_names+CHN_NAME_LENGTH*i,
                         status, ca_message(status));
      }
    }
    status = ca_pend_io(1.0);
#ifdef MIDEBUGP
    cm_msg(MLOG,"","ca_pend_io(1.0) status 0X%x = \"%s\"", status, ca_message(status));
#endif
    status = ca_pend_event(1.0);
#ifdef MIDEBUGP
    cm_msg(MLOG,"","ca_pend_event(1.0)");
#endif
  }

  // close channel access 
  ca_context_destroy();

  // free arrays
  if (info) {
    if (info->settings.channel_names) { free(info->settings.channel_names); 
                                         info->settings.channel_names = NULL; }
#ifdef GFA_SPECIFIC
    if (info->settings.dtype)         { free(info->settings.dtype);         
                                        info->settings.dtype         = NULL; }
    if (info->settings.dsec)          { free(info->settings.dsec);         
                                        info->settings.dsec          = NULL; }
#endif
    if (info->settings.chnflags)      { free(info->settings.chnflags);     
                                        info->settings.chnflags      = NULL; }
    if (info->settings.minval)        { free(info->settings.minval);          
                                        info->settings.minval        = NULL; }
    if (info->settings.maxval)        { free(info->settings.maxval);          
                                        info->settings.maxval        = NULL; }
    if (info->settings.tolerance)     { free(info->settings.tolerance);       
                                        info->settings.tolerance     = NULL; }
    if (info->settings.used)          { free(info->settings.used);          
                                        info->settings.used          = NULL; }
    if (info->settings.readonly)      { free(info->settings.readonly);      
                                        info->settings.readonly      = NULL; }

    if (info->pchid)         { free(info->pchid);         info->pchid         = NULL; }
    if (info->array)         { free(info->array);         info->array         = NULL; }
    if (info->array_m)       { free(info->array_m);       info->array_m       = NULL; }
    if (info->demand)        { free(info->demand);        info->demand        = NULL; }
    if (info->lastset)       { free(info->lastset);       info->lastset       = NULL; }
    if (info->lastget)       { free(info->lastget);       info->lastget       = NULL; }
    if (info->status)        { free(info->status);        info->status        = NULL; }
    if (info->found)         { free(info->found);         info->found         = NULL; }
    if (info->iscmd)         { free(info->iscmd);         info->iscmd         = NULL; }
    if (info->subscribed)    { free(info->subscribed);    info->subscribed    = NULL; }
    if (info->down)          { free(info->down);          info->down          = NULL; }
    if (info->firstread)     { free(info->firstread);     info->firstread     = NULL; }
    if (info->nerr)          { free(info->nerr);          info->nerr          = NULL; }
    if (info->nmsg)          { free(info->nmsg);          info->nmsg          = NULL; }
    if (info->tolchan)       { free(info->tolchan);       info->tolchan       = NULL; }
    if (info->gchan)         { free(info->gchan);         info->gchan         = NULL; }
    if (info->intol)         { free(info->intol);         info->intol         = NULL; }
    if (info->threshold)     { free(info->threshold);     info->threshold     = NULL; }

    free(info);
  }

#ifdef MIDEBUG1
  cm_msg(MLOG,"","--epics_ca_exit()");
#endif

#ifdef MIDEBUGF
  if (fp) {
    fprintf(fp,"%u : --epics_ca_exit()\n",ss_time());
    fclose(fp);
    fp = NULL;
  }
#endif

  return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_set(CA_INFO *info, INT channel, float value, BOOL flush,BOOL forced_update){
#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : ++epics_ca_set(channel=%d(%s), value=%f, flush=%d, force_upd"
                  "=%d)\n", ss_time(), channel, 
                  info->settings.channel_names+CHN_NAME_LENGTH*channel, 
                  value, flush, forced_update);
#endif
#ifdef MIDEBUG1
  cm_msg(MLOG,"","  ++epics_ca_set(channel=%d, value=%f, flush=%d,force_upd=%d)",
                 channel,value,flush,forced_update);
#endif
   if (info) {
     if (channel < info->num_channels) {
       if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*channel,'U') == NULL) {
         if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*channel,'S') == NULL) {
           if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*channel,'M') != NULL) {
             if ((*(info->settings.minval+channel) != 0.f) ||
                 (*(info->settings.maxval+channel) != 0.f)) {
               if (*(info->found+channel) && *(info->settings.used+channel) &&
                   !*(info->settings.readonly+channel) && *(info->iscmd+channel)) {
                 if (value < *(info->settings.minval+channel)) {
                   cm_msg(MLOG,"","Channel %d (%s): New demand value (%f) lower than "
                                  "min value (%f)! - Setting min value", channel,
                                  info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                  value, *(info->settings.minval+channel));
                   value = *(info->settings.minval+channel);
                   // NOTE: demand in ODB is not (re)set!
                 } else if (value > *(info->settings.maxval+channel)) {
                   cm_msg(MLOG,"","Channel %d (%s): New demand value (%f) larger than "
                                  "max value (%f)! - Setting max value", channel,
                                  info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                  value, *(info->settings.maxval+channel));
                   value = *(info->settings.maxval+channel);
                   // NOTE: demand in ODB is not (re)set!
                 }
               }
             }
           }
         }
       }

       if ((value != *(info->demand+channel))||forced_update) {
         if (*(info->found+channel) && *(info->settings.used+channel) &&
            !*(info->settings.readonly+channel) && *(info->iscmd+channel)) {
           enum channel_state castate;
           if ((castate=ca_state(*(info->pchid+channel)))== cs_conn) {
             if (ca_write_access(*(info->pchid+channel))) {
               int status;

#ifdef GFA_SPECIFIC
               {
                 char *tpos;

                 // channel has <device>:<spec>
                 if ((tpos=strchr(info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                  ':')) != NULL) {
                   int j;

                   for (j = 0; j < N_GFA_TYPES; j++) {
                     // find type
                    if (strcmp(gfatype[j].type,info->settings.dtype+DTYPE_LENGTH*channel)
                       ==0){
                       GFA_channel *tc;
                       int k;
#ifdef MIDEBUGE
                       cm_msg(MLOG,"","Found type %s", gfatype[j].type);
#endif
                       tc = gfatype[j].tc;
                       for (k=0; k < gfatype[j].nc; k++) {  // channels of this type

                         if (!(tc+k)) continue;

                         if (strcmp(tpos,(tc+k)->spec ) == 0) { // matching :<spec>
                           int l;
                           GFA_mask *cm;
#ifdef MIDEBUGE
                           cm_msg(MLOG,"","Found spec %s NS=%d",(tc+k)->spec,(tc+k)->ns);
#endif
                           cm = (tc+k)->cm;
                           for (l = 0; l < (tc+k)->ns; l++) { // for all masks
                             int m;
                             int ttval, tdem;
                             GFA_msg  *sm;

                             if (!(cm+l)) continue;
#ifdef MIDEBUGE
                             cm_msg(MLOG,"","Checking Mask 0x%4.4X",(cm+l)->mask);
#endif
                             ttval = (int)value & (cm+l)->mask; 
                             tdem  = (int) *(info->demand+channel) & (cm+l)->mask;
                             if (*(info->demand+channel) == -2.f) tdem = 0; // hide setup
                             if (( ttval != tdem)||(*(info->demand+channel) == -2.f)) {
                               char *tpost,*tposa;
                               tpost = tposa = NULL;

                               sm = (cm+l)->sm;
                               for (m = 0; m < (cm+l)->nm; m++) {
                                 if (!(sm+m)) continue;
                           //    cm_msg(MLOG,"","check Value 0x%4.4X : \"%s\"",
                           //                   (sm+m)->value, (sm+m)->msg);
                                 if ((sm+m)->value == ttval) {
                                   if (tpost) cm_msg(MLOG,"","New value: %s overwritten"
                                                " by %s", tpost, (sm+m)->msg);
                                   tpost = (sm+m)->msg;
                                 }
                                 if ((sm+m)->value == tdem)  {
                                   if (tposa) cm_msg(MLOG,"","Prev value: %s overwritten"
                                                " by %s", tposa, (sm+m)->msg);
                                   tposa = (sm+m)->msg;
                                 }
                               } // for all values

                               if (tpost) {
                                 if (*(info->demand+channel) == -2.f) tposa = NULL;//hide
                                 if (tposa) {
                                   cm_msg(MLOG,"","%s: \"%s\" changed to \"%s\"",
                                    info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                     tposa, tpost);
                                 } else {
                                   if (strstr(info->settings.channel_names+
                                                     CHN_NAME_LENGTH*channel,":COM:")) {

                                     cm_msg(MLOG,"","%s: Command is \"%s\"",
                                   info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                       tpost);
                                   } else {
                                     cm_msg(MLOG,"","%s: \"%s\" is currently set",
                                    info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                       tpost);
                                   }
                                 }
                               // nothing assigned to mask
                               } else {
                                 if (*(info->demand+channel) != -2.f)
                                   cm_msg(MLOG,"","Channel %d(%s) %f set to %f",channel, 
                                    info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                          *(info->demand+channel),value);
                                 else
                                   cm_msg(MLOG,"","Channel %d(%s) set to %f", channel, 
                                    info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                          value);
                               }
                             }
                           } // for all masks
                           if ((tc+k)->ns <= 0) {
                             if (*(info->demand+channel) != -2.f)
                               cm_msg(MLOG,"","Channel %d(%s) %f set to %f",channel, 
                                  info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                      *(info->demand+channel),value);
                             else
                               cm_msg(MLOG,"","Channel %d(%s) set to %f", channel, 
                                  info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                      value);
                           }
                           break;
                         }
                       } // for all channels of this type
                       if (gfatype[j].nc <= 0) {
                         if (*(info->demand+channel) != -2.f)
                           cm_msg(MLOG,"","Channel %d(%s) %f set to %f",channel, 
                                  info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                  *(info->demand+channel),value);
                         else
                           cm_msg(MLOG,"","Channel %d(%s) set to %f", channel, 
                                  info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                  value);
                       }
                       break;
                     } 
                   } // for all GFA types
                 }
               }
#else
               if (*(info->demand+channel) != -2.f)
                 cm_msg(MLOG,"","Channel %d(%s) %f set to %f",channel, 
                        info->settings.channel_names+CHN_NAME_LENGTH*channel,
                        *(info->demand+channel),value);
               else
                 cm_msg(MLOG,"","Channel %d(%s) set to %f", channel, 
                        info->settings.channel_names+CHN_NAME_LENGTH*channel,
                        value);
#endif

               // no :COM:2 readout therefore simulate readout of :COM:2 channel
               if (strstr(info->settings.channel_names+CHN_NAME_LENGTH*channel,":COM:2"))
                 *(info->array_m+channel) = value;

               ca_put(DBR_FLOAT, info->pchid[channel], &value);
#ifdef GFA_SPECIFIC
               // GFA specific handling : maybe post pending command
               if (!info->handling_disabled) {
                 char *ds;

                 //   disable GFA specific handling
                 info->handling_disabled = TRUE;

                 // type and channel specific changes of other channel(s)
                 // when specific flags of this channel set and apply rule to new value
                 //  e.g. Z flag : Turn power off when new value is zero  
                 ds = info->settings.dtype+DTYPE_LENGTH*channel;
#ifdef MIDEBUG2
                 cm_msg(MLOG,"","Looking for device specific handling of Device type %s",ds);
#endif
                 if ((strcmp(ds,"BEND")==0) || (strcmp(ds,"QUAD")==0) || 
                     (strcmp(ds,"SEPT")==0) || (strcmp(ds,"STER")==0) ||
                     (strcmp(ds,"PS"  )==0) || 
                     (strcmp(ds,"TS"  )==0) || (strcmp(ds,"KICKER")==0)) {
                   char chname[CHN_NAME_LENGTH];
                   char *tpos;

#ifdef MIDEBUG2
                   cm_msg(MLOG,"","Device type %s found",ds);
#endif
                   strcpy(chname,info->settings.channel_names+CHN_NAME_LENGTH*channel);

                   if ((tpos = strstr(chname,":SOL:2")) != NULL) {
                     int i;

                     strcpy(tpos,":COM:2");
#ifdef MIDEBUG2
                     cm_msg(MLOG,"","Looking for channel %s",chname);
#endif
                     for (i = 0; i < info->num_channels; i++) {
                       if (i==channel) continue;
                       if (strcmp(info->settings.channel_names+CHN_NAME_LENGTH*i,chname)
                           == 0) {
                         float tval;
                         char *chflgs;

                         tval = (*info->demand+i); // current value of other
                         // evaluate channel flags of channel
                         chflgs = info->settings.chnflags+CHNFLAGS_LENGTH*channel;
                         if (strchr(chflgs,'P')) {
                           tval = 0.f;   // ON
                         }
                         if (strchr(chflgs,'Z')) {
                           if (value == 0.f)
                             tval = 1.f; // OFF
                           else
                             tval = 0.f; // ON
                         }    
                         if (strchr(chflgs,'N')) {
                           if (value > 0.f)
                             tval = 0.f; // ON
                           else
                             tval = 1.f; // OFF
                         }    
                         if (tval != *(info->demand+i)) {
                           if (*(info->found+i) && *(info->settings.used+i) &&
                               !*(info->settings.readonly+i)) {
                             HNDLE hDB;
                             epics_ca_set(info, i, tval, FALSE, forced_update);
#ifndef OMIT_ODBUPDATE
                             // update ODB
                             cm_get_experiment_database(&hDB, NULL);
                             if (info->hkeyDemand != 0) {
                               INT mstatus;
                               mstatus = db_set_data_index(hDB, info->hkeyDemand, &tval, 
                                                           sizeof(float), i, TID_FLOAT);
                               if (mstatus != DB_SUCCESS) ; // NIY message
                                 
                             }
#endif
                           }
                         }
                         break;  // channel found
                       }
                     }
                   }

                 } else if (strcmp(ds,"FENTE")==0) {
                   char chname[CHN_NAME_LENGTH];
                   char *tpos;

#ifdef MIDEBUG2
                   cm_msg(MLOG,"","Device type %s found",ds);
#endif
                   strcpy(chname,info->settings.channel_names+CHN_NAME_LENGTH*channel);

                   if ((tpos = strstr(chname,":SOL:2")) != NULL) {
                     int i;

                     strcpy(tpos,":COM:2");
#ifdef MIDEBUG2
                     cm_msg(MLOG,"","Looking for channel %s",chname);
#endif
                     for (i = 0; i < info->num_channels; i++) {
                       if (i==channel) continue;
                       if (strcmp(info->settings.channel_names+CHN_NAME_LENGTH*i,chname)
                           == 0) {
                         float tval;
                         char *chflgs;

                         tval = (*info->demand+i); // current value of other
                         // evaluate channel flags of channel
                         chflgs = info->settings.chnflags+CHNFLAGS_LENGTH*channel;
                         if (strchr(chflgs,'V')) {
                           tval = 3.f;   // 3 MOVE_TO_SOL
                         }    
                         if (tval != *(info->demand+i)) {
                           if (*(info->found+i) && *(info->settings.used+i) &&
                               !*(info->settings.readonly+i)) {
                             HNDLE hDB;

                             epics_ca_set(info, i, tval, FALSE, forced_update);
                             // update ODB
                             cm_get_experiment_database(&hDB, NULL);
                             if (info->hkeyDemand != 0) {
                               INT mstatus;
                               mstatus = db_set_data_index(hDB, info->hkeyDemand, &tval, 
                                                           sizeof(float), i, TID_FLOAT);
                               if (mstatus != DB_SUCCESS) ; // NIY message
                             }
                           }
                         }
                         break;
                       }
                     }
                   }
                 } else if (strcmp(ds,"BX")==0) {
                   // DO NOTHING
                 } else if (strcmp(ds,"HVPS")==0) {
                   // DO NOTHING (too complicated to be handled correctly) 
                 } else {
                   cm_msg(MLOG,"","epics_ca_set: GFA type %s is currently not handled",
                                  ds);
                 }
                 //   enable GFA specific handling
                 info->handling_disabled = FALSE; 
               }
#endif  /* #ifdef GFA_SPECIFIC */
               if (flush) {
                 status = ca_pend_io(0.01);
#ifdef MIDEBUGP
                 cm_msg(MLOG,"","ca_pend_io(0.01) status 0X%x = \"%s\"", status, 
                                ca_message(status));
#endif
               }
             } else {
               cm_msg(MLOG,"","SET: channel %d(%s) NO WRITE ACCESS!",
                         channel,info->settings.channel_names+CHN_NAME_LENGTH*channel); 
             }
           } else {
             cm_msg(MLOG,"","SET: channel %d(%s) is (currently?) not connected! "
                    "Channel state is %d = \"%s\"", channel, 
                    info->settings.channel_names+CHN_NAME_LENGTH*channel, castate, 
                    castatestr(castate)); 
           }
         } else if (!*(info->found+channel)){
           cm_msg(MLOG,"","SET: channel %d(%s) is (currently?) not enabled!",
                          channel,info->settings.channel_names+CHN_NAME_LENGTH*channel); 
         } else if (!*(info->settings.used+channel)) {
           cm_msg(MLOG,"","SET: channel %d(%s) is not used!",
                          channel,info->settings.channel_names+CHN_NAME_LENGTH*channel); 
         } else if (*(info->settings.readonly+channel)) {
           cm_msg(MLOG,"","SET: channel %d(%s) is set read ONLY!",
                          channel,info->settings.channel_names+CHN_NAME_LENGTH*channel); 
         } else if (!*(info->iscmd+channel)){
           cm_msg(MLOG,"","SET: channel %d(%s) is not a channel to be set!",
                          channel,info->settings.channel_names+CHN_NAME_LENGTH*channel); 
         }
       } else {
#ifdef MIDEBUG
         cm_msg(MLOG,"","Demand and readout (%f) did not change -> not updating channel"
                         " %d(%s)", value, channel, 
                         info->settings.channel_names+CHN_NAME_LENGTH*channel);
#endif
       }
       if (info->lastset) *(info->lastset+channel) = ss_time();
       if (info->demand)  *(info->demand+channel)  = value;
     }
   }
#ifdef MIDEBUG1
   cm_msg(MLOG,"","  --epics_ca_set()");
#endif
#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : --epics_ca_set(channel=%d, value=%f)\n", ss_time(),
                  channel,value);
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_set_all(CA_INFO * info, INT channels, float value) {
   INT status,i;

#ifdef MIDEBUG1
   cm_msg(MLOG,"","  ++epics_ca_set_all(%d channels)",channels);
#endif
   for (i = 0; i < MIN(info->num_channels, channels); i++)
     epics_ca_set(info, i, value, FALSE, FALSE);

   status = ca_pend_io(0.01);
#ifdef MIDEBUGP
   cm_msg(MLOG,"","ca_pend_io(0.01) status 0X%x = \"%s\"", status, ca_message(status));
#endif

#ifdef MIDEBUG1
   cm_msg(MLOG,"","  --epics_ca_set_all()");
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_set_label(CA_INFO * info, INT channel, char *label) {
   int status;
   char str[256];
   chid chan_id;

#ifdef MIDEBUG1
   cm_msg(MLOG,"","  ++epics_ca_set_label(channel %d, label %s)", channel, label);
#endif
   sprintf(str, "%s.DESC", info->settings.channel_names+CHN_NAME_LENGTH*channel);
   status = ca_create_channel(str, NULL, NULL, 0, &chan_id);
   if (status == ECA_NORMAL) {
     status = ca_pend_io(2.0);
#ifdef MIDEBUGP
     cm_msg(MLOG,"","ca_pend_io(2.0) status 0X%x = \"%s\"", status, ca_message(status));
#endif
     if (status == ECA_NORMAL) {
       if (ca_state(chan_id) == cs_conn) {
         status = ca_put(ca_field_type(chan_id), chan_id, label);
         status = ca_pend_io(0.01);
#ifdef MIDEBUGP
         status = cm_msg(MLOG,"", "ca_pend_io(0.01) status 0X%x = \"%s\"", status, 
                                  ca_message(status));
#endif
       } else {
         cm_msg(MLOG,"","epics_ca_set_label: Channel %s is not connected", str);
       }
     } else {
       cm_msg(MERROR,"epics_ca_set_label","%s not found", str);
     }
     ca_clear_channel(chan_id);
   } else {
     cm_msg(MERROR,"epics_ca_set_label","ca_create_channel(%s) Returned status "
                                        "0X%x = \"%s\"",str,status,ca_message(status));
   }
#ifdef MIDEBUG1
   cm_msg(MLOG,"","  --epics_ca_set_label()");
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_get(CA_INFO * info, INT channel, float *pvalue) {
   int status;
#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : ++epics_ca_get(channel=%d(%s))\n", ss_time(),channel,
                  info->settings.channel_names+CHN_NAME_LENGTH*channel);
#endif
#ifdef MIDEBUG2
   cm_msg(MLOG,"","  ++epics_ca_get(channel %d)",channel);
#endif

   // NOTE: pending events will be handled in event handler callback

   if (channel == 0) {
     if (info->num_channels > 50) {
#ifdef MIDEBUGP
       cm_msg(MLOG,"","ca_pend_event(0.01)");
#endif
       ca_pend_event(0.01);
     } else {
#ifdef MIDEBUG2
       cm_msg(MLOG,"","ss_sleep(50)");
#endif
       ss_sleep(50);
#ifdef MIDEBUGP
       cm_msg(MLOG,"","ca_pend_event(0.1)");
#endif
       ca_pend_event(0.1);
     }
     status = ca_pend_io(0.001);
#ifdef MIDEBUGP
     cm_msg(MLOG,"","ca_pend_io(0.001) status 0X%x =\"%s\"",status,ca_message(status));
#endif
   } else {
     ca_pend_event(0.001);
   }

   // return last value
   if (*(info->found+channel)) {
     if (ca_state(*(info->pchid+channel)) == cs_conn) {
       if (ca_read_access(*(info->pchid+channel))) { 
         float newdemand;

         newdemand = *(info->demand+channel);
         // process and then get demand value and also update array_m of dependent chans
         epics_ca_get_demand(info, channel, &newdemand);
         if (pvalue) *pvalue = *(info->array_m+channel);

         if (newdemand != *(info->demand+channel)) {
           // difference of DWORD not negative due to change of system time?
           if (ss_time() > *(info->lastset+channel)) {
 //          if ((ss_time() - *(info->lastset+channel)) > 60) {
             if ((ss_time() - *(info->lastset+channel)) > 2) {

               if (*(info->iscmd+channel)) {
                 if (*(info->demand+channel) != -2.f)
                   cm_msg(MLOG, "", "Channel %d(%s): demand (%f) changed to %f", channel,
                                    info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                    *(info->demand+channel),newdemand);
                 else 
                   cm_msg(MLOG, "", "Channel %d(%s): current demand is %f", channel,
                                    info->settings.channel_names+CHN_NAME_LENGTH*channel,
                                    newdemand);
               }

               if ((*(info->iscmd+channel)) && ((*(info->gchan+channel) > 0) ||
                   (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*channel,'F')!=NULL))){
                 int size;
                 HNDLE hDB;
#ifdef GFA_SPECIFIC
                 BOOL oldflag;
#endif 

#ifdef GFA_SPECIFIC
                 oldflag = info->handling_disabled;
                 // maybe disable handling of GFA specific
                 // info->handling_disabled = TRUE;
#endif 
                 // F flag: set new demand value (limits may be checked and set)
                 if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*channel,'F')!=NULL){
                   DWORD lastset;
                   // NOTE: ...ca_set() sets *(info->demand+channel) to newdemand 
                   //       and puts new value to "command" channel
                   lastset = *(info->lastset+channel); // keep lastset time
                   epics_ca_set(info,channel,newdemand,TRUE,FALSE); 
                   *(info->lastset+channel) = lastset; // restore lastset
                 } else {
                   // G flag: "status" feedback therefore not necessary to put newdemand
                   //         to "command" channel again
                   *(info->demand+channel) = newdemand;
                 }
#ifdef GFA_SPECIFIC
                 info->handling_disabled  = oldflag;
#endif

#ifndef OMIT_ODBUPDATE                 
                 // update ODB Demand
                 size = sizeof(float);
                 cm_get_experiment_database(&hDB, NULL);
                 if (info->hkeyDemand != 0) {
                   INT mstatus;
                   mstatus = db_set_data_index(hDB,info->hkeyDemand, &newdemand,
                                               size, channel, TID_FLOAT);
                   if (mstatus != DB_SUCCESS) ; // NIY message
                 }
#endif
               } else
                 *(info->demand+channel) = newdemand;
               *(info->lastset+channel) = ss_time();
             }
           } else {
             *(info->lastset+channel) = ss_time();
           }
         }
         *(info->nmsg+channel) = 0;
       } else {
         if (*(info->nmsg+channel) == 0) {
           cm_msg(MLOG, "", "Channel %d(%s): NO read access!", channel,
                            info->settings.channel_names+CHN_NAME_LENGTH*channel);
           *(info->nmsg+channel) += 1;
         }
         if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*channel,'D') && pvalue)
           *pvalue = -1.f;
       }
     } else {
       if (*(info->nmsg+channel) == 0) {
         cm_msg(MLOG, "", "Channel %d(%s): NOT connected!", channel,
                          info->settings.channel_names+CHN_NAME_LENGTH*channel);
         *(info->nmsg+channel) += 1;
       }
       if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*channel,'D') && pvalue)
         *pvalue = -1.f;
     }
   } else {
     if (*(info->nmsg+channel) == 0) {
       if (*(info->settings.used+channel))
         cm_msg(MLOG, "", "Channel %d(%s): NOT found!", channel,
                          info->settings.channel_names+CHN_NAME_LENGTH*channel);
       *(info->nmsg+channel) += 1;
     }
     if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*channel,'D') && pvalue)
       *pvalue = -1.f;
   }

   // after all channels were processed once
   if (channel == info->num_channels-1) {
     // check every 10 seconds
     if ((ss_time() > info->lastSNC) && ((ss_time() - info->lastSNC) > 10)) { 
       int i,tSNC;
#ifdef MIDEBUG1
       cm_msg(MLOG,"","C-flag: Checking if channels with C-flag are connected");
#endif
       for (i=0,tSNC=0; i < info->num_channels; i++) {
    //   if (*(info->found+i)&&*(info->settings.used+i)&&!*(info->settings.readonly+i)&&
         if (                  *(info->settings.used+i)                               &&
             (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'C')) && *(info->down+i)){
#ifdef MIDEBUG
           cm_msg(MLOG,"","C-flag: Channel %d (%s) is currently not connected",i,
                          info->settings.channel_names+CHN_NAME_LENGTH*i);
#endif
           tSNC++;
         }
       }

       if (tSNC != info->settings.Sum_NOT_Connected) {
         cm_msg(MLOG,"","Number of Channels (with C-flag) NOT connected changed from "
                        "%d to %d", info->settings.Sum_NOT_Connected, tSNC);
         info->settings.Sum_NOT_Connected = tSNC;
         // update ODB when different
         if (info->hkeySNC) {
           HNDLE hDB;
           cm_get_experiment_database(&hDB, NULL);
           db_set_data(hDB, info->hkeySNC, &info->settings.Sum_NOT_Connected, 
                       sizeof(int), 1, TID_INT);
         }
         // reset alarm when sum is 0
         if ((tSNC==0) && (strncmp(info->settings.AlarmWhenNOTConnected,"NONE",4) != 0))
           al_reset_alarm(info->settings.AlarmWhenNOTConnected);
       }
       
       info->lastSNC = ss_time();

     } else if (ss_time() < info->lastSNC) {
       cm_msg(MLOG,"","Sum of channels NOT connected: System time was reset");
       info->lastSNC = ss_time();
     }

#ifdef GFA_SPECIFIC
     // check every 10 seconds
     if ((ss_time() > info->lastSNT) && ((ss_time() - info->lastSNT) > 10)) { 
       int i,tSNT;
       BOOL *handled;
#ifdef MIDEBUG
       cm_msg(MLOG,"","T-flag: Checking if channels with T-flag are in tolerance");
#endif
       handled = calloc(info->num_channels,sizeof(BOOL));
       for (i=0,tSNT=0; i < info->num_channels; i++) {
         // T flag set, not handled yet and tolerance > 0
    //   if (*(info->found+i)&&*(info->settings.used+i)&&!*(info->settings.readonly+i)&&
         if (                  *(info->settings.used+i)                               &&
             strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'T') &&
             (!handled || !*(handled+i)) && (*(info->settings.tolerance+i)>0.f)){
           if (*(info->tolchan+i) == -1.f) {
             char tename[CHN_NAME_LENGTH];
             int  j;
             BOOL findrefchan;
             char *ds;
             char *tpos;

             findrefchan = TRUE;
             strcpy(tename,info->settings.channel_names+CHN_NAME_LENGTH*i);

             ds = info->settings.dtype+DTYPE_LENGTH*i;
#ifdef MIDEBUG2
             cm_msg(MLOG,"","Looking for device specific handling of Device type %s",ds);
#endif
             if ((strcmp(ds,"BEND")==0) || (strcmp(ds,"QUAD")==0) || 
                 (strcmp(ds,"SEPT")==0) || (strcmp(ds,"STER")==0) ||
                 (strcmp(ds,"PS"  )==0) || 
                 (strcmp(ds,"TS"  )==0) || (strcmp(ds,"KICKER")==0)) {

#ifdef MIDEBUG2
               cm_msg(MLOG,"","T-flag: Device type %s found",ds);
#endif
               // assign corresponding channel
               if ((tpos = strstr(tename,":SOL:2")) != NULL) {
                 strcpy(tpos,":IST:2");
               } else if ((tpos = strstr(tename,":IST:2")) != NULL) {
                 strcpy(tpos,":SOL:2");
               } else {
                 cm_msg(MLOG,"","T-flag: Channel %d (%s) not handled for type %s", i,
                                info->settings.channel_names+CHN_NAME_LENGTH*i,ds);
                 findrefchan = FALSE;
               }
             } else if (strcmp(ds,"FENTE")==0) {
               // assign corresponding channel
               if ((tpos = strstr(tename,":SOL:2")) != NULL) {
                 strcpy(tpos,":IST1:2");
               } else if ((tpos = strstr(tename,":IST1:2")) != NULL) {
                 strcpy(tpos,":SOL:2");
               } else if ((tpos = strstr(tename,":POSA:2")) != NULL) {
                 strcpy(tpos,":SOL:2");
               } else {
                 cm_msg(MLOG,"","T-flag: Channel %d (%s) not handled for type %s", i,
                               info->settings.channel_names+CHN_NAME_LENGTH*i,ds);
                 findrefchan = FALSE;
               }
             } else if (strcmp(ds,"BX")==0) {
               // DO NOTHING
               findrefchan = FALSE;
             } else if (strcmp(ds,"HVPS")==0) {
               // assign corresponding channel NOTE: I(max) has no tolerance!
               if ((tpos = strstr(tename,":SOLV:2")) != NULL) {
                 strcpy(tpos,":ISTV:2");
               } else if ((tpos = strstr(tename,":ISTV:2")) != NULL) {
                 strcpy(tpos,":SOLV:2");
               } else {
                 cm_msg(MLOG,"","T-flag: Channel %d (%s) not handled for type %s", i,
                               info->settings.channel_names+CHN_NAME_LENGTH*i,ds);
                 findrefchan = FALSE;
               }
             } else {
               cm_msg(MLOG,"","T-flag: Channel %d (%s) type %s is not handled", i,
                            info->settings.channel_names+CHN_NAME_LENGTH*i,ds);
               findrefchan = FALSE;
             }

             if (findrefchan) {
#ifdef MIDEBUG1
               cm_msg(MLOG,"","T-flag: Searching for channel %s",tename);
#endif
               for (j=0; j < info->num_channels; j++) {
                 if (i != j) {
                   if (strcmp(tename,info->settings.channel_names+CHN_NAME_LENGTH*j)==0){
                     *(info->tolchan+i) = j;
                     // NIY if (*(info->tolchan+j) > 0) warning channel was already
                     //                                 assigned
                     *(info->tolchan+j) = i;
                     break;
                   }
                 }
               } // for all (other) channels
             } else {
               *(info->tolchan+i) = -2.f; // mark as already processed
             }
           } // search for corresponding channels

           if (*(info->tolchan+i) > 0) { // tolerance needs to be checked
             float tvali,tvalr,diff;
             int   j;
             BOOL omit;

             omit = FALSE;
             j = *(info->tolchan+i);
             if (*(info->iscmd+i)) {
               tvali = *(info->demand+i);
               tvalr = *(info->array+j);
               // exceptions?
               // N flag: Power Off when Negative -> omit
               if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'N')&&(tvali < 0.f))
                 omit = TRUE;
             } else {
               tvali = *(info->array+i);
               tvalr = *(info->demand+j);
               // exceptions?
               // N flag: Power Off when Negative -> omit
               if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*j,'N')&&(tvalr < 0.f))
                 omit = TRUE;
             }

             diff = tvali - tvalr;
             if (diff < 0.f) diff = -diff;
             if (diff > *(info->settings.tolerance+i)) {
               if (!omit) {
                 if (*(info->intol+i) || *(info->intol+j)) {
                   cm_msg(MLOG,"","Channel %d (%s)=%f and Channel %d (%s)=%f : "
                                  "difference %f is out of tolerance %f",
                                  i,info->settings.channel_names+CHN_NAME_LENGTH*i,tvali,
                                  j,info->settings.channel_names+CHN_NAME_LENGTH*j,tvalr,
                                  diff, *(info->settings.tolerance+i));
                   *(info->intol+i) = FALSE;
                   *(info->intol+j) = FALSE;
                 }
                 tSNT++;
               }
             } else {
#ifdef MIDEBUG1
               cm_msg(MLOG,"","Channel %d (%s)=%f and Channel %d (%s)=%f : "
                              "difference %f is in tolerance %f",
                              i,info->settings.channel_names+CHN_NAME_LENGTH*i,tvali,
                              j,info->settings.channel_names+CHN_NAME_LENGTH*j,tvalr,
                              diff, *(info->settings.tolerance+i));
#endif
               *(info->intol+i) = TRUE;
               *(info->intol+j) = TRUE;
             }

             if (handled) {
               *(handled+i) = TRUE;
               *(handled+j) = TRUE;
             }

           }
         } else {
#ifdef MIDEBUG
         if (!*(info->settings.used+i))
           cm_msg(MLOG,"","Channel %d (%s): not used",i,
                          info->settings.channel_names+CHN_NAME_LENGTH*i);
         if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*i,'T') == NULL)
           cm_msg(MLOG,"","Channel %d (%s): T flag not set",i,
                          info->settings.channel_names+CHN_NAME_LENGTH*i);
         if (handled && *(handled+i)) 
           cm_msg(MLOG,"","Channel %d (%s): already handled",i,
                          info->settings.channel_names+CHN_NAME_LENGTH*i);
         if (*(info->settings.tolerance+i)<=0.f)
           cm_msg(MLOG,"","Channel %d (%s): invalid tolerance %f", i,
                          info->settings.channel_names+CHN_NAME_LENGTH*i,
                          *(info->settings.tolerance+i));
#endif
         } // channel used?
       } // for all channels

       if (info->pending > 0) {
           tSNT += info->pending;
       }

       if (info->settings.Sum_NOT_in_Tolerance != tSNT) {
         // pending operations, tolerance check is not reliable 
         if ((info->pending > 0) && (info->pending == tSNT)) {
           if (info->pending == 1)
             cm_msg(MLOG,"","Pending operation -> In Range checking blocked");
           else 
             cm_msg(MLOG,"","%d Pending operations -> In Range checking blocked", 
                            info->pending);
             
         } else {
           if (tSNT != 0)
             cm_msg(MLOG,"","Number of Channels (T) NOT in Tolerance Range changed "
                            "from %d to %d", info->settings.Sum_NOT_in_Tolerance, tSNT);
           else
             cm_msg(MLOG,"","All Channels (T) in Tolerance Range");
         }
         info->settings.Sum_NOT_in_Tolerance = tSNT;
         // update ODB when different
         if (info->hkeySNT) {
           HNDLE hDB;
           cm_get_experiment_database(&hDB, NULL);
           db_set_data(hDB,info->hkeySNT,&info->settings.Sum_NOT_in_Tolerance,
                       sizeof(int),1,TID_INT);
         }
         // reset alarm when sum is 0
         if ((tSNT==0)&&(strncmp(info->settings.AlarmWhenNOTinTolerance,"NONE",4) != 0))
           al_reset_alarm(info->settings.AlarmWhenNOTinTolerance);
       }

       info->lastSNT = ss_time();
       if (handled) { free(handled); handled = NULL;}

     } else if (ss_time() < info->lastSNT) {
       cm_msg(MLOG,"","Sum of channels NOT in Tolerance: System time was reset");
       info->lastSNT = ss_time();
     }
#endif /* #ifdef GFA_SPECIFIC */
   }

#ifdef MIDEBUG2
   if (pvalue)
     cm_msg(MLOG,"","  --epics_ca_get(value = %f)",*pvalue);
   else
     cm_msg(MLOG,"","  --epics_ca_get(NULL pointer to return data)");
#endif
#ifdef MIDEBUGF
  if (fp) {
    if (pvalue)
      fprintf(fp,"%u : --epics_ca_get(channel=%d(%s), value=%f)\n", ss_time(), channel,
              info->settings.channel_names+CHN_NAME_LENGTH*channel, *pvalue);
    else
      fprintf(fp,"%u : --epics_ca_get(channel=%d, NULL pointer to return data)\n", 
              ss_time(), channel);
  }
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/
// 
// return demand value of channel
// handle command channels with associated G-flag
//
INT epics_ca_get_demand(CA_INFO * info, INT channel, float *pvalue){
#ifdef MIDEBUGF
  if (fp) fprintf(fp,"%u : ++epics_ca_get_demand(channel=%d(%s))\n", ss_time(), channel,
                  info->settings.channel_names+CHN_NAME_LENGTH*channel);
#endif
#ifdef MIDEBUG1
   cm_msg(MLOG,"","  ++epics_ca_get_demand(channel %d)",channel);
#endif

   if (info) {
     if (channel < info->num_channels) {

       // channel number of dependent channel is not initialized yet
       // search for channel
       if (*(info->gchan+channel) == -1.f) {
         char tename[CHN_NAME_LENGTH];
         int  j;
         BOOL findrefchan;
         char *ds;
         char *tpos;

         findrefchan = TRUE;
         strcpy(tename,info->settings.channel_names+CHN_NAME_LENGTH*channel);

         ds = info->settings.dtype+DTYPE_LENGTH*channel;
#ifdef MIDEBUG2
         cm_msg(MLOG,"","G-flag: search Looking for device specific handling of Device "
                        "type %s", ds);
#endif
         if ((strcmp(ds,"BEND")==0) || (strcmp(ds,"QUAD")==0) || 
             (strcmp(ds,"SEPT")==0) || (strcmp(ds,"STER")==0) || 
             (strcmp(ds,"PS"  )==0) || (strcmp(ds,"TS"  )==0) ||
             (strcmp(ds,"KICKER")==0)                             ) {

#ifdef MIDEBUG2
           cm_msg(MLOG,"","G-flag: Device type %s found",ds);
#endif
           // assign corresponding channel
           if ((tpos = strstr(tename,":COM:2")) != NULL) {
             strcpy(tpos,":STA:1");
           } else if ((tpos = strstr(tename,":STA:1")) != NULL) {
             strcpy(tpos,":COM:2");
           } else if ((tpos = strstr(tename,":SOL:2")) != NULL) {
             findrefchan = FALSE;
           } else if ((tpos = strstr(tename,":IST:2")) != NULL) {
             findrefchan = FALSE;
           } else {
             cm_msg(MLOG,"","G-flag: search Channel %d (%s) not handled for type %s", 
               channel, info->settings.channel_names+CHN_NAME_LENGTH*channel,ds);
             findrefchan = FALSE;
           }

         } else if (strcmp(ds,"FENTE")==0) {
           // assign corresponding channel
           if        ((tpos = strstr(tename,":COM:2")) != NULL) {
             strcpy(tpos,":STA:1");
           } else if ((tpos = strstr(tename,":STA:1")) != NULL) {
             strcpy(tpos,":COM:2");
           } else if ((tpos = strstr(tename,":SOL:2")) != NULL) {
             strcpy(tpos,":IST1:2");
           } else if ((tpos = strstr(tename,":IST1:2")) != NULL) {
             strcpy(tpos,":SOL:2");
           } else if ((tpos = strstr(tename,":POSA:2")) != NULL) {
             strcpy(tpos,":SOL:2");
           } else {
             cm_msg(MLOG,"","G-flag: search Channel %d (%s) not handled for type %s", 
               channel, info->settings.channel_names+CHN_NAME_LENGTH*channel,ds);
             findrefchan = FALSE;
           }

         } else if (strcmp(ds,"BX")==0) {
           if        ((tpos = strstr(tename,":COM:2")) != NULL) {
             strcpy(tpos,":STA:1");
           } else if ((tpos = strstr(tename,":STA:1")) != NULL) {
             strcpy(tpos,":COM:2");
           } else {
             cm_msg(MLOG,"","G-flag: search Channel %d (%s) not handled for type %s", 
               channel, info->settings.channel_names+CHN_NAME_LENGTH*channel,ds);
             findrefchan = FALSE;
           }

         } else if (strcmp(ds,"HVPS")==0) {
           // DO NOTHING (no G-flags)
           findrefchan = FALSE;
         } else {
           if (strncmp(ds,"**",2) != 0)
             cm_msg(MLOG,"","G-flag: search Channel %d (%s) type %s is not handled", 
               channel, info->settings.channel_names+CHN_NAME_LENGTH*channel,ds);
           findrefchan = FALSE;
         }

         if (findrefchan) {
#ifdef MIDEBUG1
           cm_msg(MLOG,"","G-flag: search Searching for channel %s",tename);
#endif
           for (j=0; j < info->num_channels; j++) {
             if (channel != j) {
               if (strcmp(tename,info->settings.channel_names+CHN_NAME_LENGTH*j)==0){
                 if (strchr(info->settings.chnflags+CHNFLAGS_LENGTH*channel,'G') ||
                     strchr(info->settings.chnflags+CHNFLAGS_LENGTH*j,'G')) {
                   *(info->gchan+channel) = j;
                   // NIY if (*(info->gchan+j) > 0) warning channel was already
                   //                               assigned to channel ...
                   *(info->gchan+j) = channel;
                   break;
                 }
               }
             }
           } // for all (other) channels

           if (*(info->gchan+channel) < 0) {
             cm_msg(MLOG,"","G-flag: search Channel corresponding to %d (%s) NOT found!",
                        channel, info->settings.channel_names+CHN_NAME_LENGTH*channel);
             *(info->gchan+channel) = -2.f; // mark as already processed
           } 
         } else {
           *(info->gchan+channel) = -2.f; // mark as already processed
         }
       } // search for corresponding channels

       // command/setpoint channel?
       if (*(info->iscmd+channel)) {
         if (*(info->gchan+channel) > 0) {
           char *ds;
           char *tpos;
           int  gchan;

           gchan = *(info->gchan+channel);
           ds = info->settings.dtype+DTYPE_LENGTH*channel;
#ifdef MIDEBUG2
           cm_msg(MLOG,"","G-flag: command/setpoint channel: Looking for device "
                          "specific handling of Device type %s",ds);
#endif
           if ((strcmp(ds,"BEND")==0) || (strcmp(ds,"QUAD")==0) || 
               (strcmp(ds,"SEPT")==0) || (strcmp(ds,"STER")==0) || 
               (strcmp(ds,"PS"  )==0) ||
               (strcmp(ds,"TS"  )==0) || (strcmp(ds,"KICKER")==0)) {

             // update COM:2 reading STA:1 for KombiPS1 or MultiIO
             if (strstr(info->settings.channel_names+CHN_NAME_LENGTH*channel,":COM:2")) {
               INT tval;
               tval = (int) *(info->array+gchan);
               // Status is ON : 0x0005 ("Soll ON" : 0x0001 and (|) "Ist ON" : 0x0004)
               if ((tval & 0x0005) == 0x0005) {
                 if (*(info->array_m+channel) != 0.f) {
                   if (*(info->array_m+channel) != -2.f)
                     cm_msg(MLOG,"","%d(%s): Command changed to \"ON\"", channel,
                             info->settings.channel_names+CHN_NAME_LENGTH*channel); 
                   else
                     cm_msg(MLOG,"","%d(%s): Current Command is \"ON\"", channel,
                             info->settings.channel_names+CHN_NAME_LENGTH*channel); 
                   *(info->array_m+channel) = 0.f; // command ON
                 }
               } else { // everything NOT ON is OFF
                 if (*(info->array_m+channel) != 1.f) {
                   if (*(info->array_m+channel) != -2.f)
                     cm_msg(MLOG,"","%d(%s): Command changed to \"OFF\"", channel,
                             info->settings.channel_names+CHN_NAME_LENGTH*channel); 
                   else
                     cm_msg(MLOG,"","%d(%s): Current Command is \"OFF\"", channel,
                             info->settings.channel_names+CHN_NAME_LENGTH*channel); 
                   *(info->array_m+channel) = 1.f; // command OFF
                 }
               }
               *(info->array_m+gchan) = *(info->array+gchan);  // update :STA:1
             }

           } else if (strcmp(ds,"FENTE")==0) {
             if (strstr(info->settings.channel_names+CHN_NAME_LENGTH*channel,":COM:2")) {
               int cur, prev;
               cur  = (int) *(info->array  +gchan);
               prev = (int) *(info->array_m+gchan);
               // when status changed maybe update COM:2 with info from STA:1
               if ((cur != prev)||(*(info->array_m+channel) == -2.f)) {
                 // not moving anymore
                 if (((cur & 0x0100) != 0x0100) && ((cur & 0x0200) != 0x0200)) {
                   // stopped and NOT error
                   if ((cur & 0x0004) && ((cur & 0x0003) != 0x0003)) {
                     // not command STOP and not MOVE_TO_SOL?
                     if ((*(info->array_m+channel) != (float) 0x0002) &&
                         (*(info->array_m+channel) != (float) 0x0003)) {
                       if (*(info->array_m+channel) != -2.f)
                         cm_msg(MLOG,"","%d(%s): Command changed to \"STOP\"", channel,
                                info->settings.channel_names+CHN_NAME_LENGTH*channel); 
                       else
                         cm_msg(MLOG,"","%d(%s): Current Command is \"STOP\"", channel,
                                info->settings.channel_names+CHN_NAME_LENGTH*channel); 
                       *(info->array_m+channel) = (float) 0x0002; // command STOP
                     }
                   }
                 }
               } 
               *(info->array_m+gchan) = *(info->array+gchan);     // update :STA:1
             } else if (strstr(info->settings.channel_names+CHN_NAME_LENGTH*channel,
                               ":SOL:2")) {
               // NIY G-flag described in epics_ca_private.h should be handled here
               //
               // NIY SOL:2 and IST1 or POLA and :STA:1 - > maybe set SOL:2
               *(info->array_m+channel) = *(info->array+channel); // update :SOL:2
               *(info->array_m+gchan) = *(info->array+gchan);     // update IST1 or POLA
             }

           } else if (strcmp(ds,"BX") == 0) {
             // update COM:2 reading STA:1 for BX
             if (strstr(info->settings.channel_names+CHN_NAME_LENGTH*channel,":COM:2")) {
               INT tval;
               tval = (int) *(info->array+gchan);
               // Status is Open : 0x0001 or Closed : 0x0002
               if ((tval & 0x0001) == 0x0001) {
                 if (*(info->array_m+channel) != 0.f) {
                   if (*(info->array_m+channel) != -2.f)
                     cm_msg(MLOG,"","%d(%s): Command changed to \"OPEN\"", channel,
                             info->settings.channel_names+CHN_NAME_LENGTH*channel); 
                   else
                     cm_msg(MLOG,"","%d(%s): Current Command is \"OPEN\"", channel,
                             info->settings.channel_names+CHN_NAME_LENGTH*channel); 
                   *(info->array_m+channel) = 0.f; // command OPEN
                 } 
               } else { // NOT OPEN -> CLOSE
                 if (*(info->array_m+channel) != 1.f) {
                   if (*(info->array_m+channel) != -2.f)
                     cm_msg(MLOG,"","%d(%s): Command changed to \"CLOSE\"", channel,
                             info->settings.channel_names+CHN_NAME_LENGTH*channel); 
                   else
                     cm_msg(MLOG,"","%d(%s): Current Command is \"CLOSE\"", channel,
                             info->settings.channel_names+CHN_NAME_LENGTH*channel); 
                   *(info->array_m+channel) = 1.f; // command CLOSE
                 } 
               }
               *(info->array_m+gchan) = *(info->array+gchan);  // update :STA:1
             }
           } else {
             cm_msg(MLOG,"","G-flag: command/setpoint Channel %d (%s) type %s is not han"
               "dled", channel, info->settings.channel_names+CHN_NAME_LENGTH*channel,ds);
           }
         } else {
           // no corresponding "status" channel -> mirror demand readout of channel
           *(info->array_m+channel) = *(info->array+channel);
         }

         if (pvalue) *pvalue = *(info->array_m+channel);

       // not a command channel
       } else {
         // and no corresponding command channel -> mirror readout of channel
         if (*(info->gchan+channel) < -1.f)
           *(info->array_m+channel) = *(info->array+channel);
         if (pvalue) *pvalue = 0.f; // NO demand value to return
       }
     }
   }
#ifdef MIDEBUG1
   if (pvalue)
     cm_msg(MLOG,"","  --epics_ca_get_demand(value = %f)",*pvalue);
   else
     cm_msg(MLOG,"","  --epics_ca_get_demand(NULL pointer to return)");
#endif
#ifdef MIDEBUGF
  if (fp) {
    if (pvalue)
      fprintf(fp,"%u : --epics_ca_get_demand(channel=%d(%s), value=%f)\n", ss_time(),
                 channel, info->settings.channel_names+CHN_NAME_LENGTH*channel, *pvalue);
    else
      fprintf(fp,"%u : --epics_ca_get_demand(channel=%d,NULL pointer to return demand)\n"
              ,ss_time(), channel);
  }
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/
INT epics_ca_get_all(CA_INFO * info, INT channels, float *pvalue) {
   int i;

#ifdef MIDEBUG1
   cm_msg(MLOG,"","  ++epics_ca_get_all(%d channels)", channels);
#endif
   for (i = 0; i < MIN(info->num_channels, channels); i++)
     epics_ca_get(info, i, pvalue + i);

#ifdef MIDEBUG1
   cm_msg(MLOG,"","  --epics_ca_get_all()");
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/
INT epics_ca_get_default_name(CA_INFO *info, INT channel, char *name) {
#ifdef MIDEBUG1
   cm_msg(MLOG,"","  ++epics_ca_get_default_name(channel %d, name %s)", channel, name);
#endif
   if ((name != NULL) && (channel >= 0) && (channel< info->num_channels)) {
     strcpy(name, info->settings.channel_names + CHN_NAME_LENGTH * channel);
   }
#ifdef MIDEBUG1
   cm_msg(MLOG,"","  --epics_ca_get_default_name(channel %d, name %s)", channel, name);
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_get_default_threshold(CA_INFO *info, INT channel, float *pvalue) {

#ifdef MIDEBUG1
   cm_msg(MLOG,"","  ++epics_ca_get_default_threshold(channel %d)", channel);
#endif
   if (pvalue != NULL) {
     *pvalue = 0.1f;
     if (info->threshold && (channel >= 0) && (channel < info->num_channels))
       *pvalue = *(info->threshold+channel);
   }
#ifdef MIDEBUG1
   if (pvalue)
     cm_msg(MLOG,"","  --epics_ca_get_default_threshold(channel %d, threshold %f)", 
                    channel, *pvalue);
#endif
   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT epics_ca(INT cmd, ...) {
   va_list argptr;
   HNDLE hKey;
   INT channel, status;
   DWORD flags;
   float value, *pvalue;
   CA_INFO *info;
   char *label;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

#ifdef MIDEBUG2
   cm_msg(MLOG,"","++epics_ca(CMD=%d)",cmd);
#endif
   if (cmd == CMD_INIT) {
     void *pinfo;

     hKey    = va_arg(argptr, HNDLE);
     pinfo   = va_arg(argptr, void *);
     channel = va_arg(argptr, INT);
     flags   = va_arg(argptr, DWORD);
     status  = epics_ca_init(hKey, pinfo, channel);
     if (pinfo) {
       info = *(CA_INFO **) pinfo;
       if (info) info->flags = flags;
     }
   } else {
     info = va_arg(argptr, void *);

     /* only execute command if enabled */
     if (!info || info->cmd_disabled) {
       ss_sleep(100);
#ifdef MIDEBUG2
       cm_msg(MLOG,"","ss_sleep(100)");
#endif
       status = FE_ERR_DISABLED;
     } else {
       switch (cmd) {
         case CMD_INIT:
            break;

         case CMD_EXIT:
            status = epics_ca_exit(info);
            break;

         case CMD_SET:
            channel = va_arg(argptr, INT);
            value = (float) va_arg(argptr, double);
            status = epics_ca_set(info, channel, value, TRUE, FALSE);
            break;

         case CMD_SET_ALL:
            channel = va_arg(argptr, INT);
            value = (float) va_arg(argptr, double);
            status = epics_ca_set_all(info, channel, value);
            break;

         case CMD_SET_LABEL:
            channel = va_arg(argptr, INT);
            label = va_arg(argptr, char *);
            //status = epics_ca_set_label(info, channel, label);
            status = FE_SUCCESS;
            break;

         case CMD_GET:
            channel = va_arg(argptr, INT);
            pvalue = va_arg(argptr, float *);
            status = epics_ca_get(info, channel, pvalue);
            break;

         case CMD_GET_DEMAND:
            channel = va_arg(argptr, INT);
            pvalue = va_arg(argptr, float *);
            status = epics_ca_get_demand(info, channel, pvalue);
            break;

         case CMD_GET_ALL:
            channel = va_arg(argptr, INT);
            pvalue = va_arg(argptr, float *);
            status = epics_ca_get_all(info, channel, pvalue);
            break;

         case CMD_GET_DEFAULT_NAME:
            channel = va_arg(argptr, INT);
            label = va_arg(argptr, char *);
            status = epics_ca_get_default_name(info, channel, label);
            break;

         case CMD_GET_DEFAULT_THRESHOLD:
            channel = va_arg(argptr, INT);
            pvalue = va_arg(argptr, float *);
            status = epics_ca_get_default_threshold(info, channel, pvalue);
            break;

         default:
#ifdef MIDEBUG
   cm_msg(MLOG,"","**epics_ca(CMD=%d) is not handled",cmd);
#endif
            break;
       }
     }
   }

   va_end(argptr);

#ifdef MIDEBUG2
   cm_msg(MLOG,"","--epics_ca()");
#endif
   return status;
}

/*--- EOF epics_ca.c -------------------------------------------------*/
