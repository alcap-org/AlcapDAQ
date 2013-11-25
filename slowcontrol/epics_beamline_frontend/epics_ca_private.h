/********************************************************************\

  Name:         epics_ca_private.h
  Created by:   RA36 (modified part of epics_ca.c)

  Contents:     Channel Access device driver private function declarations

  $Id: epics_ca_private.h,v 1.2 2011-06-25 20:56:00 l_data Exp $

\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cadef.h"

#ifdef _MSC_VER                 /* collision between varargs and stdarg */
#undef va_arg
#undef va_start
#undef va_end
#endif

#include "midas.h"

#ifndef _EPICS_CA_PRIVATE_H_
#define _EPICS_CA_PRIVATE_H_

/*----------------------------------------------------------------------------*/

#define CHN_NAME_LENGTH     32      /* length of channel names */
#define CHNFLAGS_LENGTH     16      /* chan flags string size */

/*----------------------------------------------------------------------------*/

#ifdef GFA_SPECIFIC

#define DTYPE_LENGTH        40     /* epics DBR_STRING size - fixed */
#define DSEC_LENGTH         40     /* epics DBR_STRING size - fixed */

#endif
      //    DD settings
      //    DD/Loadfile <loadfile>
      //      <loadfile> will be read and interpreted and DD entries for
      //      EPICS Channel name, Used, Readonly, Chn_flags, Minval, Maxval, Threshold,
      //      Tolerance will be created if it they did not previously exist
      //
      //    DD/AlarmWhenNOTConnected <alarm>
      //      if not NONE, Alarm <alarm> will initially be reset in epics_ca_init() and
      //      when Sum_NOT_Connected changes to 0.
      //
      //    DD/Sum_NOT_Connected <number>
      //      <number> > 0 may be used as Alarm level
      //
      //    DD/AlarmWhenNOTinTolerance <alarm>
      //      if not NONE, Alarm <alarm> will initially be reset in epics_ca_init() and
      //      when Sum_NOT_in_Tolerance changes to 0.
      //     e.g.: Alarm condition for Alarm "BL tolerance" 
      //       /Equipment/eblpim3/Settings/Devices/EPICSGFA/DD/Sum_NOT_in_Tolerance > 0
      //
      //    DD/Sum_NOT_in_Tolerance <number>
      //      <number> > 0 may be used as Alarm level
      //      NOTE: Pending operations set Sum_NOT_in_tolerance > 0 until reliable
      //            checks are possible. To make sure WAIT <device> INRANGE in autorun
      //            waits until all operations are completed.
      //
      //    DD/EPICS Channel name []   max. 32 characters
      //
      //    DD/Used []          y/n
      //
      //    DD/Readonly []      y/n
      //
      //    DD/Chn_Flags []
      //  
      //      Chn_flags are evaluated when (re)connecting, reading or setting the 
      //      respective channel
      //
      //      - Initial value not changed yet after initial setup
      //        U = NOT USED (Channel will not be handled)
      //
      //      - Initialized
      //        I = Channel may be used and has no special flags
      //
      //      - channel subscription may be disabled          
      //        S = NO subscription of channel changes
      //            e.g. for strings
      //
      //      - update demand value of channel when readout of value changes
      //        F = Feedback of demand channel readout to demand value of 
      //            channel when demand value was not updated by epics_ca_set()
      //            in the last 60 seconds
      //        NOTE: ODB update only - demand value will not be put to EPICS channel
      //
      //        See GFA specific flag G for update of Command or Setpoint channel demand
      //        by the interpreted readout of a measured or status channel.
      //               
      //      - Min/Max Info for GUI or Min/Max value when sending new demand value 
      //        to EPICS channel
      //        M = Write Min & Max in DD during first init of DD/MinVal and DD/MaxVal
      //            Change new demand value to Min or Max value when out of range
      //
      //        NOTE: Maybe modify Min/Max of channels manually when channel is not 
      //              available during init or Min or Max value should be different 
      //        NOTE: any value may be set if MinVal and MaxVal are both equal to 0
      //        NOTE: any value may be set if M is not specified in DD/Chn_Flags
      //
      //        Delete DD/MinVAl DD/MaxVal after the first setup to regenerate
      //        Min/Max from LOW/HIGH when channels are connected.
      //        NOTE: S and U overrule M
      //                                                       
      //      - information when channel is not connected ....               
      //        D = Set measured value to -1 to flag channel not found, disconnected
      //            or no read access
      //
      //      - information if channel is not connected
      //        C = check if channel is not connected. 
      //        NOTE: When NOT connected DD/Sum_NOT_connected is incremented. 
      //
      //      - reset demand value
      //        R = (Re)set Demand value on EPICS channel when channel is reconnected
      //            and readout of demand value and demand value differ.
      //        NOTE: Setpoint must always be valid. 
      //              e.g. GFA device type FENTE must therefore always be set to
      //                   setpoint and command MOVE_TO_SOL must be set
      //
      //*** GFA Specific *** 
      //
      //      - update demand value of channel corresponding to measured or status chan
      //        G = Feedback from "Measured" or "Status" channel readout to correspondng
      //            demand channel and demand value was not updated by epics_ca_set()
      //            in the last 60 seconds. 
      //        NOTE: ODB update only - demand value will not be put to EPICS channel
      //        NOTE: Type FENTE :SOL:2 will only be updated with IST1:2 or POSA:2
      //              when not moving anymore (NOT 0x0100 and NOT 0x0200) and in status 
      //              "stopped" (0x0004) and previous cmd was SOL=POSA(0x0004) 
      //              or cmd was MOVE_TO_CW/CCW (0x0000/0x0001) and status is 
      //              "in CW/CCW limit switch" (0x0001/0x0002)
      //        See non GFA specific flag F for update of demand by demand readout
      //
      //      - additional epics channel e.g. for Power ON/OFF may be set
      //        according to GFA device type
      //                
      //        P = Turn power ON  (other channel e.g. :COM:2) when setting this channel
      //                                                       e.g. :SOL:2
      //        N = Turn power ON  (other channel e.g. :COM:2) when setting > 0
      //            Turn power OFF (other channel e.g. :COM:2) when setting <= 0
      //        Z = Turn power OFF (other channel e.g. :COM:2) when setting to zero
      //            Turn power ON  (other channel e.g. :COM:2) when setting <> zero
      //        NOTE: N overrules Z and P. Z overrules P.
      //
      //      - set command set_to_demand on other channel    
      //        V = Set value for "Move to setpoint" MOVE_TO_SOL=0x0003 
      //            (in other channel e.g. :COM:2 for GFA type FENTE)  
      //            when setting this channel e.g. :SOL:2
      //
      //      - check tolerance of this channel
      //        T = check if (this) demand channel e.g. <dev>:SOL:2 and corresponding
      //            measured channel e.g.: <dev>:IST:2 are in tolerance 
      //            when <tolerance> of demand is > 0.
      //            Do not check tolerance when demand value is negative and N flag is 
      //            set. 
      //        NOTE: When NOT in tolerance DD/Sum_NOT_in_Tolerance is incremented.
      //
      //      Examples of channel flags
      //
      //      EPICS Channel   GFA_type    Channel flags  Iscmd
      //      -------------   ----------  -------------  ------
      //      FS11-L:COM:2    FENTE        I             y
      //      FS11-L:STA:1    FENTE        IG            y
      //      FS11-L:SOL:2    FENTE        IVMCTRF       y
      //      FS11-L:IST1:2   FENTE        IG            y
      //      ASM11:COM:2     BEND         I             y
      //      ASM11:STA:1     BEND         IG            n
      //      ASM11:SOL:2     BEND         IFZMCTR       y
      //      ASM11:IST:2     BEND         I             n
      //      KSD11:COM:2     BX           I             y
      //      KSD11:STA:1     BX           IG            n
      //      SEP31HVN:SOLI:2 HVPS         IF            y
      //      SEP31HVN:SOLV:2 HVPS         IFT           y
      //
      //*** END GFA Specific *** 
      //
      //    DD/Minval []  [0.0]
      //       Minimum value from Loadfile or updated from LOW attribute of Channel
      //       when M flag is set and DD/Minval does not exist
      //
      //    DD/Maxval []  [0.0]
      //       Maximum value from Loadfile or updated from HIGH attribute of Channel
      //       when M flag is set and DD/Maxval does not exist
      //    NOTE: if Minval[i] and Maxval[i] of EPICS channel [i] is 0.0 there will be
      //          no range checking when a demand value is set on channel [i]
      // 
      //    DD/Tolerance [] [0.0]
      //       Tolerance from Loadfile 
      //    NOTE: Tolerance will not be checked when 0.0 is set as tolerance
      //

/*----------------------------------------------------------------------------*/

typedef struct {
   char   loadfile[NAME_LENGTH]; // file name of load file
   char   AlarmWhenNOTConnected[NAME_LENGTH];
   int    Sum_NOT_Connected;     // sum of DOWN channels with C flag set
   char   AlarmWhenNOTinTolerance[NAME_LENGTH];
   int    Sum_NOT_in_Tolerance;  // out of tolerance channels with T flag & tol > 0.0
   char  *channel_names;       // channel_names
#ifdef GFA_SPECIFIC
   char  *dtype;               // description type    (also for interpretation by GUI)
   char  *dsec;                // description section (also for interpretation by GUI)
#endif
   char  *chnflags;            // special flags of channel
   float *minval;              // channel minval
   float *maxval;              // channel maxval
   float *tolerance;           // channel tolerance
   BOOL  *used;                // used flag
   BOOL  *readonly;            // readonly flag
} CA_SETTINGS;

typedef struct {
   CA_SETTINGS settings;       // DD specific settings in ODB
   chid  *pchid;               // channel ID of channels
   float *array;               // channel readout
   float *array_m;             // channel readout mirror
   float *demand;              // channel demand
   float *threshold;           // channel threshold (only used when reading loadfile)
   DWORD *lastset;             // channel timestamp when last set
   DWORD *lastget;             // channel timestamp when last get
   int   *status;              // status returned in event handler
   BOOL  *found;               // found flag
   BOOL  *iscmd;               // iscmd flag
   BOOL  *subscribed;          // subscribed flag
   BOOL  *down;                // down flag
   BOOL  *firstread;           // flag expecting first reading when UP again
   INT   *nerr;                // number of errors
   INT   *nmsg;                // number of messages
   INT   *tolchan;             // index of corresponding tolerance channel
   INT   *gchan;               // index of corresponding channel with G-flag
   BOOL  *intol;               // channel is/is not in tolerance range
   INT    num_channels;        // number of channels
   DWORD  flags;               // flags supplied when epics_ca(CMD_INIT, ...) is called
   INT    cmd_disabled;        // command execution in epics_ca() is initially disabled
   INT    subscribe_disabled;  // subscribe (in connection handler) is initially disabled
#ifdef GFA_SPECIFIC
   BOOL   handling_disabled;   // GFA specific handling is (currently) disabled
#endif                         // to avoid infinit loop of handling dependencies...
   char   name[NAME_LENGTH];   // equipment <name>
   HNDLE  hkeyDemand;          // "/Equipment/<equi>/Variables/Demand" key handle
   HNDLE  hkeyDD;              // "DD" key handle
   HNDLE  hkeySNC;             // "DD/Sum_NOT_Connected" key handle
   HNDLE  hkeySNT;             // "DD/Sum_NOT_in_Tolerance" key handle
   DWORD  lastSNC;             // last time Sum_NOT_Connected of C flag channels
   DWORD  lastSNT;             // last time Sum_NOT_in_Tolerance of T flag channels
   INT    pending;             // pending operation -> increment Sum_NOT_in_Tolerance
} CA_INFO;

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

#endif /* _EPICS_CA_PRIVATE_H_ */

/*--- EOF epics_ca_private.h -------------------------------------------------*/
