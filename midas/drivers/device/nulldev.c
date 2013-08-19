/********************************************************************\

  Name:         nulldev.c
  Created by:   Stefan Ritt

  Contents:     NULL Device Driver. This file can be used as a 
                template to write a read device driver

  $Log: nulldev.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.6  2004/05/07 19:40:11  midas
  Replaced min/max by MIN/MAX macros

  Revision 1.5  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.4  2003/09/29 11:56:44  midas
  Initial revision

  Revision 1.3  2003/05/09 10:33:52  midas
  Fixed bug with CMD_SET_ALL

  Revision 1.2  2002/06/06 07:50:12  midas
  Implemented scheme with DF_xxx flags

  Revision 1.1  2002/03/14 13:12:17  midas
  Split null driver in bus and device drivers


\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "midas.h"

/*---- globals -----------------------------------------------------*/

#define DEFAULT_TIMEOUT 10000   /* 10 sec. */

/* Store any parameters the device driver needs in following 
   structure. Edit the NULLDEV_SETTINGS_STR accordingly. This 
   contains usually the address of the device. For a CAMAC device
   this could be crate and station for example. */

typedef struct {
   int address;
} NULLDEV_SETTINGS;

#define NULLDEV_SETTINGS_STR "\
Address = INT : 1\n\
"

/* following structure contains private variables to the device
   driver. It is necessary to store it here in case the device
   driver is used for more than one device in one frontend. If it
   would be stored in a global variable, one device could over-
   write the other device's variables. */

typedef struct {
   NULLDEV_SETTINGS nulldev_settings;
   float *array;
   INT num_channels;
    INT(*bd) (INT cmd, ...);    /* bus driver entry function */
   void *bd_info;               /* private info of bus driver */
   HNDLE hkey;                  /* ODB key for bus driver info */
} NULLDEV_INFO;

/*---- device driver routines --------------------------------------*/

/* the init function creates a ODB record which contains the
   settings and initialized it variables as well as the bus driver */

INT nulldev_init(HNDLE hkey, void **pinfo, INT channels, INT(*bd) (INT cmd, ...))
{
   int status, size;
   HNDLE hDB, hkeydd;
   NULLDEV_INFO *info;

   /* allocate info structure */
   info = calloc(1, sizeof(NULLDEV_INFO));
   *pinfo = info;

   cm_get_experiment_database(&hDB, NULL);

   /* create NULLDEV settings record */
   status = db_create_record(hDB, hkey, "DD", NULLDEV_SETTINGS_STR);
   if (status != DB_SUCCESS)
      return FE_ERR_ODB;

   db_find_key(hDB, hkey, "DD", &hkeydd);
   size = sizeof(info->nulldev_settings);
   db_get_record(hDB, hkeydd, &info->nulldev_settings, &size, 0);

   /* initialize driver */
   info->num_channels = channels;
   info->array = calloc(channels, sizeof(float));
   info->bd = bd;
   info->hkey = hkey;

   if (!bd)
      return FE_ERR_ODB;

   /* initialize bus driver */
   status = info->bd(CMD_INIT, info->hkey, &info->bd_info);

   if (status != SUCCESS)
      return status;

   /* initialization of device, something like ... */
   BD_PUTS("init");

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nulldev_exit(NULLDEV_INFO * info)
{
   /* call EXIT function of bus driver, usually closes device */
   info->bd(CMD_EXIT, info->bd_info);

   /* free local variables */
   if (info->array)
      free(info->array);

   free(info);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nulldev_set(NULLDEV_INFO * info, INT channel, float value)
{
   char str[80];

   /* set channel to a specific value, something like ... */
   sprintf(str, "SET %d %lf", channel, value);
   BD_PUTS(str);
   BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);

   /* simulate writing by storing value in local array, has to be removed
      in a real driver */
   if (channel < info->num_channels)
      info->array[channel] = value;

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nulldev_set_all(NULLDEV_INFO * info, INT channels, float *value)
{
   int i;
   char str[100000];

   /* put here some optimized form of setting all channels simultaneously like ... */
   strcpy(str, "SETALL ");
   for (i = 0; i < MIN(info->num_channels, channels); i++)
      sprintf(str + strlen(str), "%lf ", value[i]);
   BD_PUTS(str);
   BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);

   /* simulate writing by storing values in local array */
   for (i = 0; i < MIN(info->num_channels, channels); i++)
      info->array[i] = value[i];

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nulldev_get(NULLDEV_INFO * info, INT channel, float *pvalue)
{
   int status;
   char str[80];

   /* read value from channel, something like ... */
   sprintf(str, "GET %d", channel);
   BD_PUTS(str);
   status = BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);

   *pvalue = (float) atof(str);

   /* simulate reading by copying set data from local array */
   if (channel < info->num_channels)
      *pvalue = info->array[channel];
   else
      *pvalue = 0.f;

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT nulldev_get_all(NULLDEV_INFO * info, INT channels, float *pvalue)
{
   int i;

   /* put here some optimized form of reading all channels. If the deviced
      does not support such a function, one can call nulldev_get() in a loop 

      strcpy(str, "GETALL");
      BD_PUTS(str);
      BD_GETS(str, sizeof(str), ">", DEFAULT_TIMEOUT);

      for (i=0 ; i<MIN(info->num_channels, channels) ; i++)
      pvalue[i] = atof(str+i*5); // extract individual values from reply
    */

   /* simulate reading by copying set data from local array */
   for (i = 0; i < MIN(info->num_channels, channels); i++)
      pvalue[i] = info->array[i];

   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT nulldev(INT cmd, ...)
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
      status = nulldev_init(hKey, info, channel, bd);
      break;

   case CMD_EXIT:
      info = va_arg(argptr, void *);
      status = nulldev_exit(info);
      break;

   case CMD_SET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);   // floats are passed as double
      status = nulldev_set(info, channel, value);
      break;

   case CMD_SET_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = (float *) va_arg(argptr, float *);
      status = nulldev_set_all(info, channel, pvalue);
      break;

   case CMD_GET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = nulldev_get(info, channel, pvalue);
      break;

   case CMD_GET_ALL:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = nulldev_get_all(info, channel, pvalue);
      break;

   default:
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
