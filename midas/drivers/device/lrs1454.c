/********************************************************************\

  Name:         lrs1454.c
  Created by:   Stefan Ritt

  Contents:     LeCroy LRS1454/1458 Voltage Device Driver

  $Log: lrs1454.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.17  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.16  2003/10/29 13:08:56  midas
  Added blanks

  Revision 1.15  2003/10/06 09:18:20  midas
  Fix segfault if module does not report voltage

  Revision 1.14  2003/09/30 16:12:11  midas
  Show error about panic switch

  Revision 1.13  2003/09/29 11:56:44  midas
  Initial revision

  Revision 1.12  2003/09/29 08:01:32  midas
  Added 'zero channels'

  Revision 1.11  2002/06/06 08:06:32  midas
  Implemented DF_xx scheme

  Revision 1.10  2001/04/11 02:11:24  midas
  First zero all channels (for the case of trip problems)

  Revision 1.9  2001/04/06 03:58:35  midas
  Moved debugging flag into ODB for BD

  Revision 1.8  2001/04/06 01:57:12  midas
  Added #include <stdlib.h> for atof under Linux

  Revision 1.7  2001/04/06 01:33:57  midas
  Addef fflush

  Revision 1.6  2001/04/05 12:48:53  midas
  Re-open broken TCP connection

  Revision 1.5  2001/04/04 04:11:24  midas
  Various changes at KEK, added CMD_SET_CURRENT_LIMIT_ALL

  Revision 1.4  2001/02/26 13:57:34  midas
  Made ajdustments to work with tcpip bus driver

  Revision 1.3  2001/01/04 11:17:23  midas
  Implemented Bus Driver scheme

  Revision 1.2  2000/12/18 09:45:39  midas
  changed CR-LF

  Revision 1.1  2000/12/05 00:49:55  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "midas.h"

/*---- globals -----------------------------------------------------*/

#define DEFAULT_TIMEOUT 10000   /* 10 sec. */

typedef struct {
   char password[NAME_LENGTH];
   int polarity[16];
} lrs1454_SETTINGS;

#define LRS1454_SETTINGS_STR "\
Password = STRING : [32] lrs1450\n\
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
[15] -1\n\
"

typedef struct {
   lrs1454_SETTINGS settings;
   int num_channels;
    INT(*bd) (INT cmd, ...);    /* bus driver entry function */
   void *bd_info;               /* private info of bus driver */
   HNDLE hkey;                  /* ODB key for bus driver info */
} LRS1454_INFO;

/*---- device driver routines --------------------------------------*/

INT lrs1454_setup(LRS1454_INFO * info, BOOL zero_channels)
{
   int status, i;
   char str[1000];

   /* initialize bus driver */
   status = info->bd(CMD_INIT, info->hkey, &info->bd_info);

   if (status != SUCCESS)
      return status;

   /* wait for "NETPASSWORD:"  */
   BD_PUTS("\r");
   BD_GETS(str, sizeof(str), ":", DEFAULT_TIMEOUT);

   strcpy(str, info->settings.password);
   strcat(str, "\r");
   BD_PUTS(str);

   status = BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);
   if (strchr(str, '>') == NULL) {
      cm_msg(MERROR, "lrs1454_init",
             "LRS1454 doesn't respond. Check power and connection.");
      return FE_ERR_HW;
   }

   /* check for override */
   if (strstr(str, "OVERRIDE") != NULL) {
      BD_PUTS("OVERRIDE\r");
      status = BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);
      if (strchr(str, '>') == NULL) {
         cm_msg(MERROR, "lrs1454_init", "Cannot override other telnet session.");
         return FE_ERR_HW;
      }
      BD_PUTS("\r");
      BD_GETS(str, sizeof(str), ":", DEFAULT_TIMEOUT);
   }

   /* go into EDIT mode */
   BD_PUTS("EDIT\r");
   BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);

   if (strstr(str, "LOCAL") != NULL) {
      cm_msg(MERROR, "lrs1454_init",
             "HV Key Switch is set to LOCAL, please set to remote");
      return FE_ERR_HW;
   }

   if (strstr(str, "Panic") != NULL) {
      cm_msg(MERROR, "lrs1454_init", "HV Panic Off Switch is activated, please reset");
      return FE_ERR_HW;
   }

   /* zero channels initially */
   if (zero_channels) {
      printf("\n");
      for (i = 0; i < (info->num_channels - 1) / 12 + 1; i++) {
         printf("Zero module %d\r", i);
         fflush(stdout);
         sprintf(str, "LD L%d DV 0 0 0 0 0 0 0 0 0 0 0 0\r", i);
         BD_PUTS(str);
         status = BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);
         if (strchr(str, '>') == NULL || strstr(str, "ERROR")) {
            cm_msg(MERROR, "lrs1454_init",
                   "Cannot zero module %d. Please exchange module.", i);
//        return FE_ERR_HW;
         }
      }
   }

   /* turn on HV main switch */
   BD_PUTS("HVON\r");
   status = BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);
   if (strchr(str, '>') == NULL) {
      cm_msg(MERROR, "lrs1454_init",
             "LRS1454 cannot turn on HV. Check HVON command manually.");
      return FE_ERR_HW;
   }

   /* enable channels */
   printf("\n");
   for (i = 0; i < (info->num_channels - 1) / 12 + 1; i++) {
      printf("Enable module %d\r", i);
      fflush(stdout);
      sprintf(str, "LD L%d CE 1 1 1 1 1 1 1 1 1 1 1 1\r", i);
      BD_PUTS(str);
      status = BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);
      if (strchr(str, '>') == NULL || strstr(str, "ERROR")) {
         cm_msg(MERROR, "lrs1454_init",
                "Cannot enable module %d. Please exchange module.", i);
//      return FE_ERR_HW;
      }
   }

   return FE_SUCCESS;
}

INT lrs1454_init(HNDLE hkey, void **pinfo, INT channels, INT(*bd) (INT cmd, ...))
{
   INT status, size;
   HNDLE hDB, hkeydd;
   LRS1454_INFO *info;

   /* allocate info structure */
   info = (LRS1454_INFO *) calloc(1, sizeof(LRS1454_INFO));
   *pinfo = info;

   cm_get_experiment_database(&hDB, NULL);

   /* create lrs1454 settings record */
   status = db_create_record(hDB, hkey, "DD", LRS1454_SETTINGS_STR);
   if (status != DB_SUCCESS)
      return FE_ERR_ODB;

   db_find_key(hDB, hkey, "DD", &hkeydd);
   size = sizeof(info->settings);
   db_get_record(hDB, hkeydd, &info->settings, &size, 0);

   info->num_channels = channels;
   info->bd = bd;
   info->hkey = hkey;

   if (!bd)
      return FE_ERR_ODB;

   return lrs1454_setup(info, FALSE);   /* try without zero channels */
}

/*----------------------------------------------------------------------------*/

INT lrs1454_exit(LRS1454_INFO * info)
{
   BD_PUTS("quit\r");
   info->bd(CMD_EXIT, info->bd_info);

   free(info);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1454_set(LRS1454_INFO * info, INT channel, float value)
{
   char str[80];

   sprintf(str, "LD L%d.%d DV %1.1f\r", channel / 12, channel % 12,
           info->settings.polarity[channel / 12] * value);

   BD_PUTS(str);
   BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1454_set_all(LRS1454_INFO * info, INT channels, float *pvalue)
{
   char str[256];
   INT i, j, status;

   for (i = 0; i < (channels - 1) / 12 + 1; i++) {
      printf("Setting voltage module %d\r", i);
      fflush(stdout);

      sprintf(str, "LD L%d DV", i);

      for (j = 0; j < channels && j < 12; j++) {
         if (i * 12 + j == channels)
            break;

         sprintf(str + strlen(str), " %1.1f", *pvalue++ * info->settings.polarity[i]);
      }
      strcat(str, "\r");

      BD_PUTS(str);
      status = BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);
      if (strchr(str, '>') == NULL) {
         cm_msg(MERROR, "lrs1454_set_all",
                "LRS1454 cannot set voltages on module %d. Please check device manually.",
                i);
         return FE_ERR_HW;      //
      }
   }

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1454_get(LRS1454_INFO * info, INT channel, float *pvalue)
{
   int status;
   char str[256], *p;

   sprintf(str, "RC L%d.%d MV\r", channel / 12, channel % 12);
   BD_PUTS(str);
   status = BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);

   /* if connection lost, reconnect */
   if (status == 0) {
      cm_msg(MERROR, "lrs1454_get", "Connection broken to LRS1454");

      /* get name of bus driver */
      info->bd(CMD_NAME, info->bd_info, str);

      /* reconnect in rs232 mode */
      if (strcmp(str, "rs232")) {
         BD_PUTS("\r");
         BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);
      }

      /* re-open tcpip socket connection */
      if (strcmp(str, "tcpip")) {
         printf("\n\n\n\n\n\n\n\n");
         info->bd(CMD_EXIT, info->bd_info);
         do {
            printf("Trying to reconnect to LRS1454...\n");
            status = lrs1454_setup(info, FALSE);
            cm_yield(0);
         } while (status != SUCCESS);

         printf("...success              \n");

         cm_msg(MINFO, "lrs1454_get", "Successfully reconnected to LRS1454");

         /* re-read value */
         return lrs1454_get(info, channel, pvalue);
      }
   }

   if (strstr(str, "to begin")) {
      BD_PUTS("1450\r");
      BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);
      return lrs1454_get(info, channel, pvalue);
   }
   p = str + strlen(str) - 1;;
   while (*p && *p != ' ')
      p--;

   *pvalue = (float) fabs(atof(p + 1));

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1454_get_current(LRS1454_INFO * info, INT channel, float *pvalue)
{
   char str[256], *p;

   sprintf(str, "RC L%d.%d MC\r", channel / 12, channel % 12);
   BD_PUTS(str);
   BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);

   p = str + strlen(str) - 1;;
   while (*p && *p != ' ')
      p--;

   *pvalue = (float) fabs(atof(p + 1));

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1454_get_all(LRS1454_INFO * info, INT channels, float *array)
{
   int i, j;
   char str[256], *p;

   for (i = 0; i < (channels - 1) / 12 + 1; i++) {
      printf("Reading voltage module %d    \r", i);
      fflush(stdout);

      sprintf(str, "RC L%d MV\r", i);
      BD_PUTS(str);
      BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);

      p = strstr(str, "MV") + 3;
      if (p == NULL)
         return lrs1454_get_all(info, channels, array);

      if (strstr(p, "MV"))
         p = strstr(p, "MV") + 3;

      for (j = 0; j < 12 && i * 12 + j < channels; j++) {
         array[i * 12 + j] = (float) fabs(atof(p));
         p = strchr(p, ' ');
         if (p == NULL)
            break;
         p++;
      }
   }

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1454_get_current_all(LRS1454_INFO * info, INT channels, float *array)
{
   int i, j;
   char str[256], *p;

   for (i = 0; i < (channels - 1) / 12 + 1; i++) {
      printf("Reading current module %d    \r", i);
      fflush(stdout);

      sprintf(str, "RC L%d MC\r", i);
      BD_PUTS(str);
      BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);

      p = strstr(str, "MC") + 3;
      if (strstr(p, "MC"))
         p = strstr(p, "MC") + 3;

      for (j = 0; j < 12 && i * 12 + j < channels; j++) {
         array[i * 12 + j] = (float) fabs(atof(p));
         p = strchr(p, ' ');
         if (p == NULL)
            break;
         p++;
      }
   }

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1454_set_current_limit(LRS1454_INFO * info, int channel, float limit)
{
   char str[256];

   sprintf(str, "LD L%d.%d TC %1.1f\r", channel / 12, channel % 12, limit);

   BD_PUTS(str);
   BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT lrs1454_set_current_limit_all(LRS1454_INFO * info, int channels, float *limit)
{
   char str[1000];
   INT i, j;

   for (i = 0; i < (channels - 1) / 12 + 1; i++) {
      printf("Setting current limit module %d\r", i);
      fflush(stdout);

      sprintf(str, "LD L%d TC", i);

      for (j = 0; j < channels && j < 12; j++) {
         if (i * 12 + j == channels)
            break;

         sprintf(str + strlen(str), " %1.1f", *limit++);
      }
      strcat(str, "\r");

      BD_PUTS(str);
      BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);
   }

   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT lrs1454(INT cmd, ...)
{
   va_list argptr;
   HNDLE hKey;
   INT channel, status;
   DWORD flags;
   float value, *pvalue;
   void *info, *bd;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

   switch (cmd) {
   case CMD_INIT:
      hKey = va_arg(argptr, HNDLE);
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      flags = va_arg(argptr, DWORD);
      bd = va_arg(argptr, void *);
      status = lrs1454_init(hKey, info, channel, bd);
      break;

   case CMD_EXIT:
      info = va_arg(argptr, void *);
      status = lrs1454_exit(info);
      break;

   case CMD_SET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      status = lrs1454_set(info, channel, value);
      break;

   case CMD_SET_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs1454_set_all(info, channel, pvalue);
      break;

   case CMD_GET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs1454_get(info, channel, pvalue);
      break;

   case CMD_GET_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs1454_get_all(info, channel, pvalue);
      break;

   case CMD_GET_CURRENT:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs1454_get_current(info, channel, pvalue);
      break;

   case CMD_GET_CURRENT_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs1454_get_current_all(info, channel, pvalue);
      break;

   case CMD_SET_CURRENT_LIMIT:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      status = lrs1454_set_current_limit(info, channel, value);
      break;

   case CMD_SET_CURRENT_LIMIT_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = lrs1454_set_current_limit_all(info, channel, pvalue);
      break;

   case CMD_GET_DEFAULT_NAME:
   case CMD_SET_LABEL:
      break;

   default:
      cm_msg(MERROR, "lrs1454 device driver", "Received unknown command %d", cmd);
      status = FE_ERR_DRIVER;
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
