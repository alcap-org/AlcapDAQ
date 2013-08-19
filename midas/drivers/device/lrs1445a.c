/********************************************************************\

  Name:         lrs1445a.c
  Created by:   Stefan Ritt

  Contents:     LeCroy LRS 1440 High Voltage Device Driver

  $Log: lrs1445a.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  2003/10/06 09:17:46  midas
  Fixed bugs with set_all

  Revision 1.1  2003/09/29 11:56:44  midas
  Initial revision

  
\********************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include "midas.h"

/*---- globals -----------------------------------------------------*/

typedef struct {
   int address;
   int polarity[16];
} LRS1445A_SETTINGS;

#define LRS1445A_SETTINGS_STR "\
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
   LRS1445A_SETTINGS settings;
   int num_channels;
    INT(*bd) (INT cmd, ...);    /* bus driver entry function */
   void *bd_info;               /* private info of bus driver */
} LRS1445A_INFO;

/*---- device driver routines --------------------------------------*/

void lrs1445a_switch(LRS1445A_INFO * info)
{
   static INT last_address = -1;
   char str[80];
   INT status;

   if (info->settings.address != last_address) {
      BD_PUTS("MAINFRAME 15\r\n");
      BD_GETS(str, sizeof(str), "MAINFRAME 15\r\n", 1000);
      sprintf(str, "MAINFRAME %02d\r\n", info->settings.address);
      BD_PUTS(str);
      status = BD_GETS(str, sizeof(str), "> ", 2000);
      if (!status) {
         cm_msg(MERROR, "lrs1445a_init",
                "LRS1445A adr %d doesn't respond. Check power and RS232 connection.",
                info->settings.address);
         return;
      }

      last_address = info->settings.address;
   }
}

/*------------------------------------------------------------------*/

INT lrs1445a_init(HNDLE hkey, void **pinfo, INT channels, INT(*bd) (INT cmd, ...))
{
   int status, size;
   char str[1000];
   HNDLE hDB, hkeydd;
   LRS1445A_INFO *info;

   /* allocate info structure */
   info = calloc(1, sizeof(LRS1445A_INFO));
   *pinfo = info;

   cm_get_experiment_database(&hDB, NULL);

   /* create LRS1445A settings record */
   status = db_create_record(hDB, hkey, "DD", LRS1445A_SETTINGS_STR);
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

   if (status != SUCCESS) {
      cm_msg(MERROR, "lrs1445a_init",
             "Cannot access RS232 port. Mayb other application is using it.");
      return status;
   }

   bd(CMD_DEBUG, FALSE);

   /* check if module is living  */
   sprintf(str, "MAINFRAME %02d\r\n", info->settings.address);
   BD_PUTS(str);
   status = BD_GETS(str, sizeof(str), "> ", 3000);
   if (!status) {
      cm_msg(MERROR, "lrs1445a_init",
             "LRS1445A adr %d doesn't respond. Check power and RS232 connection.",
             info->settings.address);
      return FE_ERR_HW;
   }

   /* check if HV enabled */
   BD_PUTS("SHOW STATUS\r");
   BD_GETS(str, sizeof(str), "> ", 3000);

   if (strstr(str, "disabled")) {
      cm_msg(MERROR, "lrs1445a_init",
             "LeCroy1440 adr %d: HV disabled by front panel button",
             info->settings.address);
//    return FE_ERR_HW;
   }

   /* turn on HV main switch */
   BD_PUTS("ON\r");
   BD_GETS(str, sizeof(str), "> ", 5000);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1445a_exit(LRS1445A_INFO * info)
{
   info->bd(CMD_EXIT, info->bd_info);

   free(info);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1445a_set(LRS1445A_INFO * info, INT channel, float value)
{
   char str[80];

   lrs1445a_switch(info);

   sprintf(str, "WRITE (%d,%d) %04.0f\r", channel / 16, channel % 16,
           info->settings.polarity[channel / 16] * value);

   BD_PUTS(str);
   BD_GETS(str, sizeof(str), "> ", 1000);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1445a_set_all(LRS1445A_INFO * info, INT channels, float *pvalue)
{
   char str[1000];
   INT i, j, n;

   lrs1445a_switch(info);

   for (i = 0; i < (channels - 1) / 16 + 1; i++) {
      n = channels - i * 16 - 1;
      if (n > 7)
         n = 7;
      sprintf(str, "WRITE (%d,0-%d) ", i, n);

      for (j = 0; j < channels && j < 8; j++) {
         if (i * 16 + j == channels)
            break;

         sprintf(str + strlen(str), "%1.1f,", *pvalue++ * info->settings.polarity[i]);
      }
      sprintf(str + strlen(str), "\r");

      BD_PUTS(str);
      BD_GETS(str, sizeof(str), "> ", 3000);

      n = channels - i * 16 - 1;
      if (n > 15)
         n = 15;
      sprintf(str, "WRITE (%d,8-%d) ", i, n);

      for (j = 8; j < channels && j < 16; j++) {
         if (i * 16 + j == channels)
            break;

         sprintf(str + strlen(str), "%1.1f,", *pvalue++ * info->settings.polarity[i]);
      }
      sprintf(str + strlen(str), "\r");

      BD_PUTS(str);
      BD_GETS(str, sizeof(str), "> ", 3000);
   }

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1445a_get(LRS1445A_INFO * info, INT channel, float *pvalue)
{
   double value;
   char *p, str[256];

   lrs1445a_switch(info);

   sprintf(str, "READ (%d,%d)\r", channel / 16, channel % 16);
   BD_PUTS(str);
   BD_GETS(str, sizeof(str), "> ", 1000);
   p = str + 77;

   if (channel % 16 > 9)
      p++;
   if (channel / 16 > 9)
      p++;

   sscanf(p, "%lf", &value);

   *pvalue = (float) value;

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1445a_get_all(LRS1445A_INFO * info, INT channels, float *array)
{
   int i, j;
   double value;
   char str[256];

   lrs1445a_switch(info);

   for (i = 0; i <= (channels - 1) / 16; i++) {
      sprintf(str, "READ (%d,0-15)\r", i);
      BD_PUTS(str);
      BD_GETS(str, sizeof(str), "Current\r\n", 5000);

      for (j = 0; j < 16 && i * 8 + j < channels; j++) {
         BD_GETS(str, sizeof(str), "\r\n", 5000);
         sscanf(str + 23, "%lf", &value);
         array[i * 8 + j] = (float) value;
      }
   }
   BD_GETS(str, sizeof(str), "> ", 2000);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1445a_set_current_limit(LRS1445A_INFO * info, float limit)
{
   lrs1445a_switch(info);

   if (limit > 2550)
      limit = 2550.f;

   /* command SET CURRENT LIMIT does not work ??? */

   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT lrs1445a(INT cmd, ...)
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
      status = lrs1445a_init(hKey, info, channel, bd);
      break;

   case CMD_EXIT:
      info = va_arg(argptr, void *);
      status = lrs1445a_exit(info);
      break;

   case CMD_SET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      status = lrs1445a_set(info, channel, value);
      break;

   case CMD_SET_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs1445a_set_all(info, channel, pvalue);
      break;

   case CMD_GET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs1445a_get(info, channel, pvalue);
      break;

   case CMD_GET_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs1445a_get_all(info, channel, pvalue);
      break;

      /* current measurements not supported in LRS1445A */

   case CMD_GET_CURRENT:
      break;

   case CMD_GET_CURRENT_ALL:
      break;

   case CMD_SET_CURRENT_LIMIT:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      if (channel == 0)         /* one one limit for all channels */
         status = lrs1445a_set_current_limit(info, value);
      else
         status = FE_SUCCESS;
      break;

   case CMD_SET_CURRENT_LIMIT_ALL:
   case CMD_GET_DEFAULT_NAME:
   case CMD_SET_LABEL:
      break;

   default:
      cm_msg(MERROR, "lrs1445a device driver", "Received unknown command %d", cmd);
      status = FE_ERR_DRIVER;
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
