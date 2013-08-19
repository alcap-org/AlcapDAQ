/********************************************************************\

  Name:         lrs4032.c
  Created by:   Stefan Ritt

  Contents:     LeCroy LRS 4032 High Voltage Device Driver

  $Log: lrs4032.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  2002/05/08 20:51:41  midas
  Added extra parameter to function db_get_value()

  Revision 1.1  1999/12/20 10:18:20  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:57  midas
  Added Log tag in header


  Previous revision history
  -------------------------------------------------------------------
  date         by    modification
  ---------    ---   ------------------------------------------------
  26-MAR-97    SR    created
  26-Feb-98    PAA   included CAMAC

\********************************************************************/
#include <stdio.h>
#include <math.h>
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

#define LRS4032_STR "\
Branch = INT : 0\n\
Crate  = INT : 7\n\
Station  = INT : 1\n\
Mainframe = INT[16] :\n\
[0] 63\n\
[1]  0\n\
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

/*---- globals -----------------------------------------------------*/

/* Specific LRS4032 HV control structure */
struct {
   INT branch;
   INT crate;
   INT station;
   INT mainframe[16];
   INT ext0;
   INT ext1;
} lrs;

/*---- device driver routines ------------------------------------------------*/
INT lrs4032_init(HNDLE hKeyRoot, INT index, INT channels)
{
   INT size, q;
   HNDLE hKeyFrame;
   HNDLE hDB;
   DWORD status;

   cm_get_experiment_database(&hDB, NULL);

   status = db_create_record(hDB, hKeyRoot, "", LRS4032_STR);
   if (status != DB_SUCCESS) {
      cm_msg(MERROR, "lrs4032_init", "Cannot create BCN_Mainframe\n");
      return FE_ERR_ODB;
   }
   size = sizeof(lrs.branch);
   status = db_get_value(hDB, hKeyRoot, "Branch", &lrs.branch, &size, TID_INT, TRUE);
   if (status != DB_SUCCESS) {
      cm_msg(MINFO, "lrs4032_init",
             "Cannot find 'Branch' entry in ../Devices/LRS4032 database");
      return FE_ERR_ODB;
   }
   status = db_get_value(hDB, hKeyRoot, "Crate", &lrs.crate, &size, TID_INT, TRUE);
   status = db_get_value(hDB, hKeyRoot, "Station", &lrs.station, &size, TID_INT, TRUE);
   size = sizeof(lrs.mainframe);
   status = db_find_key(hDB, hKeyRoot, "Mainframe", &hKeyFrame);
   status = db_get_data(hDB, hKeyFrame, &lrs.mainframe[0], &size, TID_INT);

   /* save ext for HV slot */
   cdreg(&lrs.ext0, lrs.branch, lrs.crate, lrs.station, 0);
   cdreg(&lrs.ext1, lrs.branch, lrs.crate, lrs.station, 1);

   /* initialize device */
   cfsa(24, lrs.ext0, &status, &q);
   cfsa(24, lrs.ext1, &status, &q);
   cfsa(9, lrs.ext0, &status, &q);
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/
INT lrs4032_exit(INT index)
{
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/
INT lrs4032_assign_channel(INT direction, INT * channel, INT * frame, INT * chanframe)
{
   INT ch;

   ch = *channel;

   switch (direction) {
   case FRAME_TO_CHANNEL:
      printf("lrs4032_assign_channel: FRAME_TO_CHANNEL not implemented\n");
      break;
   case CHANNEL_TO_FRAME:
      *frame = (ch / 32);
      *chanframe = ch - (*frame) * 32;
      if (*frame == 0)
         *frame = 0x10;
#ifdef DEBUG
      printf("%i ->", ch);
      printf(" %i %i ", *frame, *chanframe);
#endif
      break;
   }
   return 1;
}

/*----------------------------------------------------------------------------*/
INT lrs4032_set(INT index, INT channel, float fvalue)
{
   INT frame, fch;
   INT nlam, status, q;
   INT value;
   DWORD dummy, cmd, data;

   value = (INT) fvalue;

   status = 1;

   /* convert channel to frame and channel within frame */
   if (lrs4032_assign_channel(CHANNEL_TO_FRAME, &channel, &frame, &fch) < 0)
      return FE_ERR_ODB;

   /* compose data word for request */
   frame <<= 4;
   fch <<= 10;

   /* set command */
   cmd = frame | fch | 0x0;
#ifdef DEBUG
   printf("WLRS4032:cmd:0x%x n:%i", cmd, lrs.station);
#endif
   nlam = q = 0;
   {
      cfsa(16, lrs.ext0, &cmd, &q);
      ss_sleep(100);
      if (q == 0)
         status++;

      /* Write data */
      data = (value << 4) | 0x1;
      cfsa(16, lrs.ext0, &data, &q);

#ifdef DEBUG
      printf(" status:%i - value:0x%x - qx:%i ", status, data, q);
#endif
      ss_sleep(100);            /* (3000); */
      status++;
      nlam++;
   }

   /* LAM check */
   q = nlam = 0;
   while (nlam < 100 && q != 1) {
      cfsa(27, lrs.ext1, &dummy, &q);
      cm_yield(100);            /* check for watchdog requests */
      status++;
      nlam++;
   }
#ifdef DEBUG
   printf("st:%i - nlam:%i - q:%i\n", status, nlam, q);
#endif
   /* clear LAM */
   cfsa(9, lrs.ext0, &dummy, &q);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs4032_set_all(INT index, INT channels, float value)
{
   INT i;

#ifdef DEBUG
   printf("in lrs4032_set_all\n");
#endif
   for (i = 0; i < channels; i++) {
      lrs4032_set(index, i, value);
      cm_yield(0);              /* check for watchdog requests */
   }
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs4032_get(INT index, INT channel, float *pvalue)
{
   INT value;
   INT frame, fch;
   INT nlam, status, q;
   DWORD cmd, dummy, dummi;

   status = 1;
   /* convert channel to frame and channel within frame */
   if (lrs4032_assign_channel(CHANNEL_TO_FRAME, &channel, &frame, &fch) < 0)
      return FE_ERR_ODB;

   /* compose data word for request */
   frame <<= 4;
   fch <<= 10;

   /* set command */
   cmd = frame | fch | 0x3;
#ifdef DEBUG
   printf(" RLRS4032:cmd:%x ext:%x %i", cmd, lrs.ext0, lrs.station);
#endif

   cfsa(16, lrs.ext0, &cmd, &q);
   ss_sleep(500);
   if (q == 0)
      status++;

   /* Set LAM check */
   nlam = q = 0;
   while (nlam < 5 && q != 1) {
      cfsa(27, lrs.ext1, &dummy, &q);
      ss_sleep(100);
      status++;
      nlam++;
   }

   /* Read value */
   value = q = status = 0;
   cfsa(2, lrs.ext0, &dummi, &q);
   value = dummi;
   ss_sleep(200);

   /* LAM check */
   q = nlam = 0;
   while (nlam < 10 && q != 1) {
      cfsa(27, lrs.ext1, &dummy, &q);
      ss_sleep(50);
      status++;
      nlam++;
   }

   /* decode value */
   value >>= 4;
#ifdef DEBUG
   printf(" st:%i - value:%d - q:%i\n", status, value, q);
#endif

   /* clear LAM */
   cfsa(9, lrs.ext0, &dummy, &q);
   *pvalue = (float) value;
   return FE_SUCCESS;
}

/*------------------------------------------------------------------*/

INT lrs4032_get_all(INT index, INT channels, float *array)
{
   float value;
   INT i;

#ifdef DEBUG
   printf("in lrs4032_get_all\n");
#endif

   /* read ALL this equip channels */
   for (i = 0; i <= channels; i++) {
      lrs4032_get(index, i, &value);
      array[i] = value;
      cm_yield(0);              /* check for watchdog requests */
   }

   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT lrs4032(INT cmd, ...)
{
   va_list argptr;
   HNDLE hKey;
   INT index, channel, status;
   float value, *pvalue;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

   switch (cmd) {
   case CMD_INIT:
      hKey = va_arg(argptr, HNDLE);
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      status = lrs4032_init(hKey, index, channel);
      break;

   case CMD_EXIT:
      index = va_arg(argptr, INT);
      status = lrs4032_exit(index);
      break;

   case CMD_SET:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
#ifdef DEBUG
      printf("in lrs4032 cmd Set -> :%i -chan:%i -Val:%f ", cmd, channel, value);
#endif
      status = lrs4032_set(index, channel, value);
      break;

   case CMD_SET_ALL:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
#ifdef DEBUG
      printf("in lrs4032 cmd Set all -> :%i -chan:%i -Val:%f ", cmd, channel, value);
#endif
      status = lrs4032_set_all(index, channel, value);
      break;

   case CMD_GET:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs4032_get(index, channel, pvalue);
#ifdef DEBUG
      printf("in lrs4032 cmd Get <- :%i -Chan:%i -Val:%f ", cmd, channel, *pvalue);
#endif
      break;

   case CMD_GET_ALL:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs4032_get_all(index, channel, pvalue);
#ifdef DEBUG
      printf("in lrs4032 cmd get all <-:%i -Chan:%i -Val:%f ", cmd, channel, *pvalue);
#endif
      break;

      /* current measurements not supported in LRS4032 */

   case CMD_GET_CURRENT:
      break;

   case CMD_GET_CURRENT_ALL:
      break;

   case CMD_SET_CURRENT_LIMIT:
      break;

   default:
      cm_msg(MERROR, "lrs4032 device driver", "Received unknown command %d", cmd);
      status = FE_ERR_DRIVER;
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
/*------------------------------------------------------------------*/
