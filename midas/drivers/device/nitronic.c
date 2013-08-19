/********************************************************************\

  Name:         nitronic.c
  Created by:   Stefan Ritt

  Contents:     Nitronic HVS 132 High Voltage Device Driver

  $Log: nitronic.c,v $
  Revision 1.2  2005/06/20 23:55:29  mucap
  (Fred)
  Updated driver for Nitronic power supply to follow conventions of recent
  Midas distributions, including support for the CMD_SET_LABEL,
  CMD_GET_DEFAULT_NAME, and CMD_SET_CURRENT_LIMIT_ALL commands.

  Revision 1.7  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.6  2001/01/04 10:50:46  midas
  Switched off debugging

  Revision 1.5  2001/01/04 10:20:57  midas
  Increased timeouts from 1s to 3s

  Revision 1.4  2001/01/03 16:11:36  midas
  Fixed compiler warnings

  Revision 1.3  2000/12/18 13:43:49  midas
  Implemented new bus driver scheme

  Revision 1.2  2000/10/19 13:00:42  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include "midas.h"

/*---- globals -----------------------------------------------------*/

typedef struct {
   int address;
} NITRONIC_SETTINGS;

#define NITRONIC_SETTINGS_STR "\
Address = INT : 0\n\
"

typedef struct {
   NITRONIC_SETTINGS settings;
   int num_channels;
    INT(*bd) (INT cmd, ...);    /* bus driver entry function */
   void *bd_info;               /* private info of bus driver */
} NITRONIC_INFO;

/*---- device driver routines --------------------------------------*/

void nitronic_switch(NITRONIC_INFO * info)
{
   static INT last_address = -1;
   char str[80];
   INT status;

   if (info->settings.address != last_address) {
      sprintf(str, "M%02d", info->settings.address);
      BD_PUTS(str);
      status = BD_GETS(str, sizeof(str), "\r+", 5000);
      if (!status) {
         cm_msg(MERROR, "nitronic_switch",
                "NITRONIC adr %d doesn't respond. Check power and RS232 connection.",
                info->settings.address);
         return;
      }

      last_address = info->settings.address;
   }
}

/*------------------------------------------------------------------*/

INT nitronic_init(HNDLE hkey, void **pinfo, INT channels, INT(*bd) (INT cmd, ...))
{
   int status, size;
   char str[256];
   HNDLE hDB, hkeydd;
   NITRONIC_INFO *info;

   /* allocate info structure */
   info = (NITRONIC_INFO *) calloc(1, sizeof(NITRONIC_INFO));
   *pinfo = info;

   cm_get_experiment_database(&hDB, NULL);

   /* create NITRONIC settings record */
   status = db_create_record(hDB, hkey, "DD", NITRONIC_SETTINGS_STR);
   if (status != DB_SUCCESS)
      return FE_ERR_ODB;

   db_find_key(hDB, hkey, "DD", &hkeydd);
   size = sizeof(info->settings);
   db_get_record(hDB, hkeydd, &info->settings, &size, 0);

   info->num_channels = channels;
   info->bd = bd;

   /* initialize bus driver */
   if (!bd)
      return FE_ERR_ODB;

   status = bd(CMD_INIT, hkey, &info->bd_info);

   if (status != SUCCESS)
      return status;

   bd(CMD_DEBUG, FALSE);

   /* check if module is living  */
   sprintf(str, "M%02d", info->settings.address);
   BD_PUTS(str);
   status = BD_GETS(str, sizeof(str), "\r+", 2000);
   if (!status) {
      cm_msg(MERROR, "nitronic_init",
             "NITRONIC adr %d doesn't respond. Check power and RS232 connection.",
             info->settings.address);
      return FE_ERR_HW;
   }

   /* turn on HV main switch */
   BD_PUTS("ONA");
   BD_GETS(str, sizeof(str), "\r+", 5000);

   BD_PUTS("M16\r\n");

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nitronic_exit(NITRONIC_INFO * info)
{
   info->bd(CMD_EXIT, info->bd_info);

   free(info);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nitronic_set(NITRONIC_INFO * info, INT channel, float value)
{
   char str[80];

   nitronic_switch(info);

   sprintf(str, "UCH%02d%04.0fL", channel + 1, value);
   BD_PUTS(str);
   BD_GETS(str, sizeof(str), "+", 3000);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nitronic_set_all(NITRONIC_INFO * info, INT channels, float value)
{
   char str[80];
   INT i;

   nitronic_switch(info);

   for (i = 0; i < channels; i++) {
      sprintf(str, "UCH%02d%04.0fL", i + 1, value);
      BD_PUTS(str);
      BD_GETS(str, sizeof(str), "+", 3000);
   }

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nitronic_get(NITRONIC_INFO * info, INT channel, float *pvalue)
{
   int value;
   char str[256];

   nitronic_switch(info);

   sprintf(str, "CH%02dL", channel + 1);
   BD_PUTS(str);
   BD_GETS(str, sizeof(str), "+", 3000);
   sscanf(str + 14, "%d", &value);

   *pvalue = (float) value;

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nitronic_get_current(NITRONIC_INFO * info, INT channel, float *pvalue)
{
   int value;
   char str[256];

   nitronic_switch(info);

   sprintf(str, "CH%02dL", channel + 1);
   BD_PUTS(str);
   BD_GETS(str, sizeof(str), "+", 3000);
   sscanf(str + 19, "%d", &value);

   *pvalue = (float) value;

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nitronic_get_all(NITRONIC_INFO * info, INT channels, float *array)
{
   int value, i;
   char str[256];

   nitronic_switch(info);

   BD_PUTS("CH01A");
   BD_GETS(str, sizeof(str), "\r", 5000);

   for (i = 0; i < 32; i++) {
      BD_GETS(str, sizeof(str), "\r", 5000);
      if (i < channels) {
         sscanf(str + 12, "%d", &value);
         array[i] = (float) value;
      }
   }
   BD_GETS(str, sizeof(str), "+", 2000);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nitronic_get_current_all(NITRONIC_INFO * info, INT channels, float *array)
{
   int value, i;
   char str[256];

   nitronic_switch(info);

   BD_PUTS("CH01A");
   BD_GETS(str, sizeof(str), "\r", 5000);

   for (i = 0; i < 32; i++) {
      BD_GETS(str, sizeof(str), "\r", 5000);
      if (i < channels) {
         sscanf(str + 19, "%d", &value);
         array[i] = (float) value;
      }
   }
   BD_GETS(str, sizeof(str), "+", 2000);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nitronic_set_current_limit(NITRONIC_INFO * info, float limit)
{
   char str[256];

   nitronic_switch(info);

   sprintf(str, "ICH01%04dA", (int) limit);
   BD_PUTS(str);
   BD_GETS(str, sizeof(str), "\r+", 5000);

   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT nitronic(INT cmd, ...)
{
   va_list argptr;
   HNDLE hKey;
   INT channel, flags, status;
   float value, *pvalue;
   void *info, *bd;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

   switch (cmd) {
   case CMD_INIT:
      hKey = va_arg(argptr, HNDLE);
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      flags = va_arg(argptr, INT);
      bd = va_arg(argptr, void *);
      status = nitronic_init(hKey, info, channel, bd);
      break;

   case CMD_EXIT:
      info = va_arg(argptr, void *);
      status = nitronic_exit(info);
      break;

   case CMD_SET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      status = nitronic_set(info, channel, value);
      break;

   case CMD_SET_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue   =  va_arg(argptr, float *);
      status = nitronic_set_all(info, channel, *pvalue);
      break;

   case CMD_GET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue  = va_arg(argptr, float*);
      status = nitronic_get(info, channel, pvalue);
      break;

   case CMD_GET_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = nitronic_get_all(info, channel, pvalue);
      break;

   case CMD_GET_CURRENT:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = nitronic_get_current(info, channel, pvalue);
      break;

   case CMD_GET_CURRENT_ALL:
      break;

   case CMD_SET_CURRENT_LIMIT:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      if (channel == 0)         /* one one limit for all channels */
         status = nitronic_set_current_limit(info, value);
      else
         status = FE_SUCCESS;
      break;

    case CMD_SET_LABEL:
      status = FE_SUCCESS;
      break;

    case CMD_GET_DEFAULT_NAME:
      status = FE_SUCCESS;
      break;

    case CMD_SET_CURRENT_LIMIT_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float*);
      status = nitronic_set_current_limit(info, pvalue[0]);
      break;

   default:
      cm_msg(MERROR, "nitronic device driver", "Received unknown command %d", cmd);
      status = FE_ERR_DRIVER;
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
