/********************************************************************\

  Name:         lrs2415.c
  Created by:   Stefan Ritt

  Contents:     LeCroy 2415 CAMAC HV Device Driver

  $Log: lrs2415.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  2002/05/08 20:51:41  midas
  Added extra parameter to function db_get_value()

  Revision 1.1  1999/12/20 10:18:20  midas
  Reorganized driver directory structure

  Revision 1.3  1999/06/25 10:59:30  midas
  Fixed bug in CAMAC access

  Revision 1.2  1998/10/12 12:18:57  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "midas.h"
#include "mcstd.h"

/*---- globals -----------------------------------------------------*/

#define MAX_DEVICES 10

static INT camac_crate[MAX_DEVICES];
static INT camac_slot[MAX_DEVICES];
static float config[MAX_DEVICES];

/*---- device driver routines --------------------------------------*/

INT lrs2415_init(HNDLE hKey, INT index, INT channels)
{
   int size;
   HNDLE hDB;

   cm_get_experiment_database(&hDB, NULL);

   /* read LRS2415 settings */
   camac_crate[index] = 0;
   size = sizeof(INT);
   db_get_value(hDB, hKey, "CAMAC crate", &camac_crate[index], &size, TID_INT, TRUE);

   camac_slot[index] = 1;
   size = sizeof(INT);
   db_get_value(hDB, hKey, "CAMAC slot", &camac_slot[index], &size, TID_INT, TRUE);

   config[index] = (float) 3.5;
   size = sizeof(float);
   db_get_value(hDB, hKey, "Configuration", &config[index], &size, TID_FLOAT, TRUE);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs2415_exit(INT index)
{
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs2415_set(INT index, INT channel, float value)
{
   if (config[index] == 3.5)
      value *= 4;
   else
      value *= 2;

   camo(camac_crate[index], camac_slot[index], 0, 16, (WORD) value);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs2415_get(INT index, INT channel, float *pvalue)
{
   INT q, x, d;

   /* trigger voltage conversion */
   camc(camac_crate[index], camac_slot[index], 0, 26);

   do {
      cam16i_q(camac_crate[index], camac_slot[index], 0, 0, (WORD *) & d, &x, &q);
   } while (x && !q);

   if (config[index] == 3.5)
      *pvalue = (float) d;
   else
      *pvalue = (float) d *2;

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs2415_get_current(INT index, INT channel, float *pvalue)
{
   INT q, x, d;

   /* trigger current conversion */
   camc(camac_crate[index], camac_slot[index], 1, 26);

   do {
      cam16i_q(camac_crate[index], camac_slot[index], 0, 0, (WORD *) & d, &x, &q);
   } while (x && !q);

   if (config[index] == 3.5)
      *pvalue = (float) (d * 0.625);
   else
      *pvalue = (float) (d * 0.250);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs2415_set_current_limit(INT index, INT channel, float value)
{
   if (config[index] == 3.5)
      value /= 10;
   else
      value /= 4;

   camo(camac_crate[index], camac_slot[index], 1, 16, (WORD) value);

   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT lrs2415(INT cmd, ...)
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
      status = lrs2415_init(hKey, index, channel);
      break;

   case CMD_EXIT:
      index = va_arg(argptr, INT);
      status = lrs2415_exit(index);
      break;

   case CMD_SET:
   case CMD_SET_ALL:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      status = lrs2415_set(index, channel, value);
      break;

   case CMD_GET:
   case CMD_GET_ALL:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs2415_get(index, channel, pvalue);
      break;

   case CMD_GET_CURRENT:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs2415_get_current(index, channel, pvalue);
      break;

   case CMD_SET_CURRENT_LIMIT:
      index = va_arg(argptr, INT);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      status = lrs2415_set_current_limit(index, channel, value);
      break;

   default:
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
