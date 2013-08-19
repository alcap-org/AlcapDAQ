/********************************************************************\

  Name:         lrs1440.c
  Created by:   Stefan Ritt

  Contents:     LeCroy LRS 1440 High Voltage Device Driver

  $Log: lrs1440.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.6  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.5  2003/09/29 11:56:44  midas
  Initial revision

  Revision 1.4  2001/01/04 12:21:46  midas
  Fixed bug

  Revision 1.3  2001/01/04 12:05:41  midas
  Implemented Bus Driver scheme

  Revision 1.2  2000/03/17 13:13:58  midas
  Changed rs232.h to bus\rs232.h

  Revision 1.1  1999/12/20 10:18:19  midas
  Reorganized driver directory structure

  Revision 1.5  1999/06/25 13:50:10  midas
  Reduced timeout from 3s to 1s

  Revision 1.4  1999/01/14 17:01:44  midas
  Added rs232 port sharing

  Revision 1.3  1999/01/14 15:57:54  midas
  Adapted to new "info" structure

  Revision 1.2  1998/10/12 12:18:57  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include "midas.h"

/*---- globals -----------------------------------------------------*/

typedef struct {
   int address;
   int polarity[16];
} LRS1440_SETTINGS;

#define LRS1440_SETTINGS_STR "\
Address = INT : 1\n\
Polarity = INT[16] :\n\
[0] -1\n\
[1] -1\n\
[2] -1\n\
[3] -1\n\
[4] -1\n\
[5] -1\n\
[6] -1\n\
[7] -1\n\
[8] -1\n\
[9] -1\n\
[10] -1\n\
[11] -1\n\
[12] -1\n\
[13] -1\n\
[14] -1\n\
"

typedef struct {
   LRS1440_SETTINGS settings;
   int num_channels;
    INT(*bd) (INT cmd, ...);    /* bus driver entry function */
   void *bd_info;               /* private info of bus driver */
} LRS1440_INFO;

/*---- device driver routines --------------------------------------*/

void lrs1440_switch(LRS1440_INFO * info)
{
   static INT last_address = -1;
   char str[80];
   INT status;

   if (info->settings.address != last_address) {
      BD_PUTS("M16\r\n");
      BD_GETS(str, sizeof(str), "M16\r\n", 1000);
      sprintf(str, "M%02d\r\n", info->settings.address);
      BD_PUTS(str);
      status = BD_GETS(str, sizeof(str), "responding\r\n", 2000);
      if (!status) {
         cm_msg(MERROR, "lrs1440_init",
                "LRS1440 adr %d doesn't respond. Check power and RS232 connection.",
                info->settings.address);
         return;
      }

      last_address = info->settings.address;
   }
}

/*------------------------------------------------------------------*/

INT lrs1440_init(HNDLE hkey, void **pinfo, INT channels, INT(*bd) (INT cmd, ...))
{
   int status, size;
   char str[256];
   HNDLE hDB, hkeydd;
   LRS1440_INFO *info;

   /* allocate info structure */
   info = calloc(1, sizeof(LRS1440_INFO));
   *pinfo = info;

   cm_get_experiment_database(&hDB, NULL);

   /* create LRS1440 settings record */
   status = db_create_record(hDB, hkey, "DD", LRS1440_SETTINGS_STR);
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
   BD_PUTS("M16\r\n");
   status = BD_GETS(str, sizeof(str), "M16\r\n", 2000);
   if (!status) {
      cm_msg(MERROR, "lrs1440_init",
             "LRS1440 adr %d doesn't respond. Check power and RS232 connection.",
             info->settings.address);
      return FE_ERR_HW;
   }

   sprintf(str, "M%02d\r\n", info->settings.address);
   BD_PUTS(str);
   status = BD_GETS(str, sizeof(str), "responding\r\n", 3000);
   if (!status) {
      cm_msg(MERROR, "lrs1440_init",
             "LRS1440 adr %d doesn't respond. Check power and RS232 connection.",
             info->settings.address);
      return FE_ERR_HW;
   }

   /* check if HV enabled */
   BD_PUTS("ST\r");
   BD_GETS(str, sizeof(str), "abled", 3000);

   if (str[strlen(str) - 6] == 's') {
      /* read rest of status */
      BD_GETS(str, sizeof(str), "", 0);

      cm_msg(MERROR, "lrs1440_init",
             "LeCroy1440 adr %d: HV disabled by front panel button",
             info->settings.address);
//    return FE_ERR_HW;
   }

   /* read rest of status */
   BD_GETS(str, sizeof(str), "", 0);

   /* turn on HV main switch */
   BD_PUTS("ON\r");
   BD_GETS(str, sizeof(str), "Turn on\r\n", 5000);

   BD_PUTS("M16\r\n");

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1440_exit(LRS1440_INFO * info)
{
   info->bd(CMD_EXIT, info->bd_info);

   free(info);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1440_set(LRS1440_INFO * info, INT channel, float value)
{
   char str[80];

   lrs1440_switch(info);

   sprintf(str, "W%04.0f C%02d\r", info->settings.polarity[channel / 16] * value,
           channel);

   BD_PUTS(str);
   BD_GETS(str, sizeof(str), "\r\n\r\n", 1000);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1440_set_all(LRS1440_INFO * info, INT channels, float value)
{
   char str[80];
   INT i;

   lrs1440_switch(info);

   for (i = 0; i < (channels - 1) / 16 + 1; i++) {
      sprintf(str, "W%04.0f C%d DO16\r", info->settings.polarity[i / 16] * value, i * 16);
      BD_PUTS(str);
      BD_GETS(str, sizeof(str), "\r\n\r\n", 3000);
   }

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1440_get(LRS1440_INFO * info, INT channel, float *pvalue)
{
   int value;
   char str[256];

   lrs1440_switch(info);

   sprintf(str, "R V C%02d\r", channel);
   BD_PUTS(str);
   BD_GETS(str, sizeof(str), "Act ", 1000);
   BD_GETS(str, sizeof(str), "\r\n\r\n", 1000);
   sscanf(str + 1, "%d", &value);

   *pvalue = (float) value;

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1440_get_all(LRS1440_INFO * info, INT channels, float *array)
{
   int value, i, j;
   char str[256];

   lrs1440_switch(info);

   sprintf(str, "R V F C0 DO%d\r", channels);
   BD_PUTS(str);

   for (i = 0; i <= (channels - 1) / 8; i++) {
      BD_GETS(str, sizeof(str), "Act ", 5000);
      BD_GETS(str, sizeof(str), "\r\n", 2000);
      for (j = 0; j < 8 && i * 8 + j < channels; j++) {
         sscanf(str + 1 + j * 6, "%d", &value);
         array[i * 8 + j] = (float) value;
      }
   }
   BD_GETS(str, sizeof(str), "\r\n", 2000);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1440_set_current_limit(LRS1440_INFO * info, float limit)
{
   char str[256];

   lrs1440_switch(info);

   if (limit > 255)
      limit = 255.f;

   sprintf(str, "LI %1.0f\r", limit);
   BD_PUTS(str);
   BD_GETS(str, sizeof(str), "\r\n\r\n", 1000);

   sprintf(str, "LI %1.0f\r", -limit);
   BD_PUTS(str);
   BD_GETS(str, sizeof(str), "\r\n\r\n", 1000);

   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT lrs1440(INT cmd, ...)
{
   va_list argptr;
   HNDLE hKey;
   INT channel, status;
   float value, *pvalue;
   void *info, *bd;
   DWORD flags;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

   switch (cmd) {
   case CMD_INIT:
      hKey = va_arg(argptr, HNDLE);
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      flags = va_arg(argptr, DWORD);
      bd = va_arg(argptr, void *);
      status = lrs1440_init(hKey, info, channel, bd);
      break;

   case CMD_EXIT:
      info = va_arg(argptr, void *);
      status = lrs1440_exit(info);
      break;

   case CMD_SET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      status = lrs1440_set(info, channel, value);
      break;

   case CMD_SET_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      status = lrs1440_set_all(info, channel, value);
      break;

   case CMD_GET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs1440_get(info, channel, pvalue);
      break;

   case CMD_GET_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs1440_get_all(info, channel, pvalue);
      break;

      /* current measurements not supported in LRS1440 */

   case CMD_GET_CURRENT:
      break;

   case CMD_GET_CURRENT_ALL:
      break;

   case CMD_SET_CURRENT_LIMIT:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      if (channel == 0)         /* one one limit for all channels */
         status = lrs1440_set_current_limit(info, value);
      else
         status = FE_SUCCESS;
      break;

   default:
      cm_msg(MERROR, "lrs1440 device driver", "Received unknown command %d", cmd);
      status = FE_ERR_DRIVER;
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
