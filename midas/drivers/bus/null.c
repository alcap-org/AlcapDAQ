/********************************************************************\

  Name:         null.c
  Created by:   Stefan Ritt

  Contents:     NULL bus driver

  $Log: null.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:07  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  2002/05/29 13:24:56  midas
  Added stdlib.h and stdarg.h

  Revision 1.1  2002/03/14 13:12:17  midas
  Split null driver in bus and device drivers


\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "midas.h"

static int debug_flag = 0;

typedef struct {
   char device[NAME_LENGTH];
} NULL_SETTINGS;

#define NULL_SETTINGS_STR "\
Device = STRING : [32] com1\n\
"

typedef struct {
   NULL_SETTINGS settings;
   int fd;                      /* device handle for NULL device */
} NULL_INFO;

/*------------------------------------------------------------------*/

int null_open(char *device)
{
   int fd;

   /* open device using device name, like

      fd = open(device);
      if (fd == -1)
      return -1;

    */

   /* simulate open */
   fd = 1;

   return fd;
}

/*----------------------------------------------------------------------------*/

INT null_exit(NULL_INFO * info)
{
   /* something like

      close(info->fd);

    */

   return SUCCESS;
}

/*----------------------------------------------------------------------------*/

int null_write(NULL_INFO * info, char *data, int size)
{
   DWORD written, i;

   if (debug_flag) {
      FILE *f;

      f = fopen("null.log", "a");
      fprintf(f, "write: ");
      for (i = 0; (int) i < size; i++)
         fprintf(f, "%X ", data[i]);
      fprintf(f, "\n");
      fclose(f);
   }

   /* something like
      write(info->fd, data, size, &written);

    */

   /* simulate writing */
   written = size;

   return written;
}

/*----------------------------------------------------------------------------*/

int null_read(NULL_INFO * info, char *data, int size, int timeout)
{
   int i, l;

   /* somethng like

      memset(data, 0, size);
      for (l=0 ; l<size ; l++)
      {
      status = read(info->fd, data+l, 1, &i);
      if (!status || i == 0)
      break;
      }
    */

   /* simulate reading */
   ss_sleep(10);
   l = 0;

   if (debug_flag && l > 0) {
      FILE *f;

      f = fopen("null.log", "a");
      fprintf(f, "read: ");
      for (i = 0; i < l; i++)
         fprintf(f, "%X ", data[i]);
      fprintf(f, "\n");
      fclose(f);
   }

   return l;
}

/*----------------------------------------------------------------------------*/

int null_puts(NULL_INFO * info, char *str)
{
   DWORD written;

   if (debug_flag) {
      FILE *f;

      f = fopen("null.log", "a");
      fprintf(f, "puts: %s\n", str);
      fclose(f);
   }

   /* something like
      write(info->fd, str, strlen(str), &written);

    */

   /* simulate writing */
   written = strlen(str);

   return written;
}

/*----------------------------------------------------------------------------*/

int null_gets(NULL_INFO * info, char *str, int size, char *pattern, int timeout)
{
   int l;

   /* something like 

      memset(str, 0, size);
      for (l=0 ; l<size-1 ; l++)
      {
      status = read(info->fd, str+l, 1, &i);
      if (!status || i == 0)
      {
      if (pattern && pattern[0])
      return 0;
      break;
      }

      if (pattern && pattern[0])
      if (strstr(str, pattern) != NULL)
      break;
      }
    */

   /* simulate reading */
   ss_sleep(10);
   l = 0;
   str[0] = 0;

   if (debug_flag && l > 0) {
      FILE *f;

      f = fopen("null.log", "a");
      fprintf(f, "gets %s: %s\n", pattern, str);
      fclose(f);
   }

   return l;
}

/*----------------------------------------------------------------------------*/

int null_init(HNDLE hkey, void **pinfo)
{
   HNDLE hDB, hkeybd;
   INT size, status;
   NULL_INFO *info;

   /* allocate info structure */
   info = calloc(1, sizeof(NULL_INFO));
   *pinfo = info;

   cm_get_experiment_database(&hDB, NULL);

   /* create NULL settings record */
   status = db_create_record(hDB, hkey, "BD", NULL_SETTINGS_STR);
   if (status != DB_SUCCESS)
      return FE_ERR_ODB;

   db_find_key(hDB, hkey, "BD", &hkeybd);
   size = sizeof(info->settings);
   db_get_record(hDB, hkeybd, &info->settings, &size, 0);

   /* open port */
   info->fd = null_open(info->settings.device);

   if (info->fd < 0)
      return FE_ERR_HW;

   return SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT null(INT cmd, ...)
{
   va_list argptr;
   HNDLE hkey;
   INT status, size, timeout;
   void *info;
   char *str, *pattern;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

   switch (cmd) {
   case CMD_INIT:
      hkey = va_arg(argptr, HNDLE);
      info = va_arg(argptr, void *);
      status = null_init(hkey, info);
      break;

   case CMD_EXIT:
      info = va_arg(argptr, void *);
      status = null_exit(info);
      break;

   case CMD_NAME:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      strcpy(str, "null");
      break;

   case CMD_WRITE:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      size = va_arg(argptr, int);
      status = null_write(info, str, size);
      break;

   case CMD_READ:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      size = va_arg(argptr, INT);
      timeout = va_arg(argptr, INT);
      status = null_read(info, str, size, timeout);
      break;

   case CMD_PUTS:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      status = null_puts(info, str);
      break;

   case CMD_GETS:
      info = va_arg(argptr, void *);
      str = va_arg(argptr, char *);
      size = va_arg(argptr, INT);
      pattern = va_arg(argptr, char *);
      timeout = va_arg(argptr, INT);
      status = null_gets(info, str, size, pattern, timeout);
      break;

   case CMD_DEBUG:
      status = va_arg(argptr, INT);
      debug_flag = status;
      break;
   }

   va_end(argptr);

   return status;
}
