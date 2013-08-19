/********************************************************************\

  Name:         caen170a.c
  Created by:   Pierre-André Amaudruz

  Contents:     CAEN 170a High Voltage Device Driver

  $Log: caen170a.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  2002/05/08 20:51:41  midas
  Added extra parameter to function db_get_value()

  Revision 1.1  1999/12/20 10:18:19  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:55  midas
  Added Log tag in header


\********************************************************************/

#include "midas.h"
#include "msystem.h"

/* Select the hardware interface */
#include "camacnul.c"           /* for code debugging no hardware involved */
                                                                                                                                                                    /* #include "camacrpc.c" *//* Use Midas frontend driver (FCNA) */
                                                                                                                                                                    /* #include "camlinux.c" *//* for the Direct CAMAC from Linux PC box */
                                                                                                                                                                    /* #include "ces8210.c"  *//* for the CES8210 VME CAMAC interface */

#include "esone.c"              /* Declare esone standard (cdreg,cfsa...) */

#define FRAME_TO_CHANNEL 0
#define CHANNEL_TO_FRAME 1

#define CAEN170A_STR "\
Branch = INT : 0\n\
Crate  = INT : 7\n\
Station  = INT : 1\n\
Mainframe = INT[16] :\n\
[0] 11\n\
[1] 21\n\
[2]  0\n\
[3]  0\n\
[4]  0\n\
[5]  0\n\
[6]  0\n\
[7]  0\n\
[8]  0\n\
[9]  0\n\
[10]  0\n\
[11]  0\n\
[12]  0\n\
[13]  0\n\
[14]  0\n\
[15]  0\n\
"

/* Specific to the CAEN170A HV control structure */
static struct {
   INT branch;
   INT crate;
   INT station;
   INT mainframe[16];
   INT ext0;
   INT ext1;
   unsigned long st;
} caen;

/*---- device driver routines ------------------------------------------------*/

INT caen170a_init(HNDLE hKeyRoot, INT index, INT channels)
{
   INT status, size, q;
   HNDLE hKeyFrame;
   HNDLE hDB;

   cm_get_experiment_database(&hDB, NULL);

   status = db_create_record(hDB, hKeyRoot, "", CAEN170A_STR);
   if (status != DB_SUCCESS) {
      cm_msg(MERROR, "caen170a_init", "Cannot create BCN_Mainframe\n");
      return FE_ERR_ODB;
   }
   size = sizeof(caen.branch);
   status = db_get_value(hDB, hKeyRoot, "Branch", &caen.branch, &size, TID_INT, TRUE);
   if (status != DB_SUCCESS) {
      cm_msg(MINFO, "caen170a_init",
             "Cannot find 'Branch' entry in ../Devices/HV170A database");
      return FE_ERR_ODB;
   }
   status = db_get_value(hDB, hKeyRoot, "Crate", &caen.crate, &size, TID_INT, TRUE);
   status = db_get_value(hDB, hKeyRoot, "Station", &caen.station, &size, TID_INT, TRUE);
   size = sizeof(caen.mainframe);
   status = db_find_key(hDB, hKeyRoot, "Mainframe", &hKeyFrame);
   status = db_get_data(hDB, hKeyFrame, &caen.mainframe[0], &size, TID_INT);

   /* save ext for HV slot */
   cdreg(&caen.ext0, caen.branch, caen.crate, caen.station, 0);
   cdreg(&caen.ext1, caen.branch, caen.crate, caen.station, 1);
   cfsa(24, caen.ext1, &caen.st, &q);
   cfsa(9, caen.ext1, &caen.st, &q);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT caen170a_exit(INT index)
{
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/
INT caen170a_assign_channel(INT direction, INT * channel, INT * frame, INT * chanframe)
{
   INT ch;

   ch = *channel;

   switch (direction) {
   case FRAME_TO_CHANNEL:
      printf("caen170a_assign_channel: FRAME_TO_CHANNEL not implemented\n");
      break;
   case CHANNEL_TO_FRAME:
      *frame = caen.mainframe[(ch / 64)];
      *chanframe = (ch % 64);
#ifdef DEBUG
      printf("%i ->", ch);
      printf(" %i %i ", *frame, *chanframe);
#endif
      break;
   }
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/
INT caen170a_set(INT index, INT channel, float fvalue)
{
   INT cmd, frame, fch, nlam, q, status, value, svalue;

   value = (INT) abs(fvalue);

   /* convert channel to frame and channel within frame */
   if (caen170a_assign_channel(CHANNEL_TO_FRAME, &channel, &frame, &fch) < 0)
      return FE_ERR_ODB;

   /* Clear buffer */
   cfsa(9, caen.ext0, &caen.st, &q);

   /* HAS to be there! */
   ss_sleep(400);

   /* host */
   status = 1;
   cfsa(16, caen.ext0, &caen.st, &q);

   /* crate */
   cfsa(16, caen.ext0, (unsigned long *) &frame, &q);

   /* operation 0x05.. Set relay OFF , .. channel# */
   /* operation 0x04.. Set relay ON , .. channel# */
   /* operation 0x03.. write Vset, .. channel# */
   cmd = 0x0300 | (fch & 0xff);
   cfsa(16, caen.ext0, (unsigned long *) &cmd, &q);

   /* set HV */
   cfsa(16, caen.ext0, (unsigned long *) &value, &q);

   /* ship op request */
   status = 1;
   cfsa(17, caen.ext0, &caen.st, &q);

   /* HAS to be there! */
   ss_sleep(300);
   nlam = 0;
   do {
      ss_sleep(100);
      cfsa(0, caen.ext0, (unsigned long *) &svalue, &q);
      nlam++;
   } while ((nlam < 10) && (q != 1));

   /* decode value */
   /* error returned :
      0xff00 : busy.
      0xff01 : unknown code or erro in the request.
      0xff02 : no channel present.
      0xff03 : Vset > HVin.
      0xff04 : HVin > 1500 ON command not enabled.
      0xff05 : Config number > 9.
      0xfffd : No data transmitted.
      0xfffe : The master ID is incorrect.
      0xffff : The address slave does not exist.
    */
   cfsa(0, caen.ext0, (unsigned long *) &svalue, &q);

#ifdef DEBUG
   printf(" - value :0x%x - value:%i - qx:%i : nlam:%i\n", svalue, svalue, q, nlam);
#endif

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT caen170a_set_all(INT index, INT channels, float value)
{
   INT i;

   for (i = 0; i < channels; i++) {
      caen170a_set(index, i, value);
      cm_yield(0);              /* check for watchdog requests */
   }

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT caen170a_get(INT index, INT channel, float *pvalue)
{
   INT cmd, frame, fch;
   INT nlam, q;
   INT value;

   /* convert channel to frame and channel within frame */
   if (caen170a_assign_channel(CHANNEL_TO_FRAME, &channel, &frame, &fch) < 0)
      return FE_ERR_ODB;

   /* clear LAM */
   cfsa(9, caen.ext0, (unsigned long *) &cmd, &q);

   /* HAS to be there! */
   ss_sleep(400);

   /* host = 1 */
   cmd = 1;
   cfsa(16, caen.ext0, (unsigned long *) &cmd, &q);

   /* mainframe CAEN address */
   cfsa(16, caen.ext0, (unsigned long *) &frame, &q);

   /* operation 0x0000 Read Module name */
   /* operation 0x02.. Read Vset , .. channel# */
   /* operation 0x01.. Read Vmon , .. channel# */
   cmd = 0x0100 | (fch & 0xff);
   cfsa(16, caen.ext0, (unsigned long *) &cmd, &q);

   /* ship op request */
   cmd = 1;
   cfsa(17, caen.ext0, (unsigned long *) &cmd, &q);

   /* HAS to be there! */
   ss_sleep(400);

   nlam = 0;
   do {
      ss_sleep(100);
      cfsa(0, caen.ext0, (unsigned long *) &value, &q);
      nlam++;
   } while ((nlam < 10) && (q != 1));

   /* read data & decode value remove relay status (bit 15) if below error status */
   cfsa(0, caen.ext0, (unsigned long *) &value, &q);
   /* error returned :
      0xff00 : busy.
      0xff01 : unknown code or erro in the request.
      0xff02 : no channel present.
      0xff03 : Vset > HVin.
      0xff04 : HVin > 1500 ON command not enabled.
      0xff05 : Config number > 9.
      0xfffd : No data transmitted.
      0xfffe : The master ID is incorrect.
      0xffff : The address slave does not exist.
    */
   if (value < 0xff00)
      *pvalue = (float) (value & 0x7fff);

#ifdef DEBUG
   printf(" - value :0x%x - value:%f - qx:%i : nlam:%i\n", value, *pvalue, q, nlam);
#endif

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/
INT caen170a_get_all(INT index, INT channels, float *array)
{
   float value;
   INT i;

   /* read ALL this equip channels */
   for (i = 0; i <= channels; i++) {
      caen170a_get(index, i, (float *) &value);
      array[i] = (float) value;
      cm_yield(0);              /* check for watchdog requests */
   }
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*---- device driver entry point -----------------------------------*/
INT caen170a(INT cmd, ...)
{
   va_list argptr;
   HNDLE hKey;
   INT channel, status, index;
   float value, *pvalue;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

   switch (cmd) {
   case CMD_INIT:
      hKey = va_arg(argptr, HNDLE);
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      status = caen170a_init(hKey, index, channel);
      break;

   case CMD_EXIT:
      index = va_arg(argptr, INT);
      status = caen170a_exit(index);
      break;

   case CMD_SET:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
#ifdef DEBUG
      printf("in caen170a cmd set ->:%i -chan:%i -Val:%f ", cmd, channel, value);
#endif
      status = caen170a_set(index, channel, value);
      break;

   case CMD_SET_ALL:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
#ifdef DEBUG
      printf("in caen170a cmd set all:%i -chan:%i -Val:%f ", cmd, channel, value);
#endif
      status = caen170a_set_all(index, channel, value);
      break;

   case CMD_GET:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = caen170a_get(index, channel, pvalue);
#ifdef DEBUG
      printf("in caen170a cmd get <-:%i -Chan:%i -Val:%f ", cmd, channel, *pvalue);
#endif
      break;

   case CMD_GET_ALL:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = caen170a_get_all(index, channel, pvalue);
#ifdef DEBUG
      printf("in caen170a cmd get all:%i -Chan:%i -Val:%f ", cmd, channel, *pvalue);
#endif
      break;

      /* current measurements not supported in CAEN170A */

   case CMD_GET_CURRENT:
      break;

   case CMD_GET_CURRENT_ALL:
      break;

   case CMD_SET_CURRENT_LIMIT:
      break;

   default:
      cm_msg(MERROR, "caen170a device driver", "Received unknown command %d", cmd);
      status = FE_ERR_DRIVER;
      break;
   }
   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
