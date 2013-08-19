/********************************************************************\

  Name:         epics_ca.c
  Created by:   Stefan Ritt

  Contents:     Epics channel access device driver

  $Log: epics_ca.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.5  2004/05/07 19:40:11  midas
  Replaced min/max by MIN/MAX macros

  Revision 1.4  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.3  2002/06/06 07:50:12  midas
  Implemented scheme with DF_xxx flags

  Revision 1.2  2000/03/02 21:50:53  midas
  Added set_label command and possibility to disable individual functions

  Revision 1.1  1999/12/20 10:18:19  midas
  Reorganized driver directory structure

  Revision 1.7  1999/09/22 15:27:35  midas
  Removed TABs

  Revision 1.6  1999/09/22 13:01:12  midas
  Figured out that pv_handle is an array of pointers

  Revision 1.5  1999/09/22 12:53:04  midas
  Removed sizeof(chid) by sizeof(struct channel_in_use)

  Revision 1.4  1999/09/22 12:13:42  midas
  Fixed compiler warning

  Revision 1.3  1999/09/22 11:30:48  midas
  Added event driven data readout

  Revision 1.2  1999/09/21 13:48:40  midas
  Fixed compiler warning

  Revision 1.1  1999/08/31 15:16:26  midas
  Added file

  Revision 1.2  1998/10/12 12:18:57  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "cadef.h"

#ifdef _MSC_VER                 /* collision between varargs and stdarg */
#undef va_arg
#undef va_start
#undef va_end
#endif

#include "midas.h"

/*---- globals -----------------------------------------------------*/

#define CHN_NAME_LENGTH 32      /* length of channel names */

typedef struct {
   char *channel_names;
   chid *pv_handles;
   float *array;
   INT num_channels;
   DWORD flags;
} CA_INFO;


void epics_ca_callback(struct event_handler_args args)
{
   CA_INFO *info;
   int i;

   info = (CA_INFO *) args.usr;

   /* search channel index */
   for (i = 0; i < info->num_channels; i++)
      if (info->pv_handles[i] == args.chid)
         break;

   if (i < info->num_channels)
      info->array[i] = *((float *) args.dbr);
}

/*---- device driver routines --------------------------------------*/

INT epics_ca_init(HNDLE hKey, void **pinfo, INT channels)
{
   int status, i;
   HNDLE hDB;
   CA_INFO *info;

   /* allocate info structure */
   info = calloc(1, sizeof(CA_INFO));
   *pinfo = info;

   cm_get_experiment_database(&hDB, NULL);

   /* get channel names */
   info->channel_names = calloc(channels, CHN_NAME_LENGTH);
   for (i = 0; i < channels; i++)
      sprintf(info->channel_names + CHN_NAME_LENGTH * i, "Channel%d", i);
   db_merge_data(hDB, hKey, "Channel name",
                 info->channel_names, CHN_NAME_LENGTH * channels, channels, TID_STRING);

   /* initialize driver */
   status = ca_task_initialize();
   if (!(status & CA_M_SUCCESS)) {
      cm_msg(MERROR, "epics_ca_init", "unable to initialize");
      return FE_ERR_HW;
   }

   /* allocate arrays */
   info->array = calloc(channels, sizeof(float));
   info->pv_handles = calloc(channels, sizeof(void *));
   for (i = 0; i < channels; i++)
      info->pv_handles[i] = calloc(1, sizeof(struct channel_in_use));

   /* search channels */
   info->num_channels = channels;

   for (i = 0; i < channels; i++) {
      status = ca_search(info->channel_names + CHN_NAME_LENGTH * i, &info->pv_handles[i]);
      if (!(status & CA_M_SUCCESS))
         cm_msg(MERROR, "epics_ca_init", "cannot find tag %s",
                info->channel_names + CHN_NAME_LENGTH * i);
   }

   if (ca_pend_io(5.0) == ECA_TIMEOUT) {
      for (i = 0; i < channels; i++)
         if (info->pv_handles[i]->state != 2)
            cm_msg(MERROR, "epics_ca_init", "cannot connect to %s",
                   info->channel_names + CHN_NAME_LENGTH * i);
   }

   /* add change notifications */
   for (i = 0; i < channels; i++) {
      /* ignore unconnected channels */
      if (info->pv_handles[i]->state != 2)
         continue;

      status = ca_add_event(DBR_FLOAT, info->pv_handles[i], epics_ca_callback,
                            info, NULL);

      if (!(status & CA_M_SUCCESS))
         cm_msg(MERROR, "epics_ca_init", "cannot add event to channel %s",
                info->channel_names + CHN_NAME_LENGTH * i);
   }

   if (ca_pend_io(5.0) == ECA_TIMEOUT) {
      for (i = 0; i < channels; i++)
         if (info->pv_handles[i]->state != 2)
            cm_msg(MERROR, "epics_ca_init", "cannot add event to channel %s",
                   info->channel_names + CHN_NAME_LENGTH * i);
   }

   /* enable all commands by default */
   info->cmd_disabled = 0;

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_exit(CA_INFO * info)
{
   int i;

   ca_task_exit();

   if (info->pv_handles) {
      for (i = 0; i < info->num_channels; i++)
         info->pv_handles[i] = calloc(1, sizeof(struct channel_in_use));
      free(info->pv_handles);
   }

   if (info->array)
      free(info->array);
   if (info->channel_names)
      free(info->channel_names);

   free(info);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_set(CA_INFO * info, INT channel, float value)
{
   ca_put(DBR_FLOAT, info->pv_handles[channel], &value);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_set_all(CA_INFO * info, INT channels, float value)
{
   INT i;

   for (i = 0; i < MIN(info->num_channels, channels); i++)
      ca_put(DBR_FLOAT, info->pv_handles[i], &value);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_set_label(CA_INFO * info, INT channels, char *label)
{
   int status;
   char str[256];
   chid chan_id;

   sprintf(str, "%s.DESC", info->channel_names + CHN_NAME_LENGTH * channels);
   status = ca_search(str, &chan_id);

   status = ca_pend_io(2.0);
   if (status != ECA_NORMAL)
      printf("%s not found\n", str);

   status = ca_put(ca_field_type(chan_id), chan_id, label);
   ca_pend_event(0.01);

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_get(CA_INFO * info, INT channel, float *pvalue)
{
   ca_pend_event(0.0001);
   *pvalue = info->array[channel];

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_get_all(CA_INFO * info, INT channels, float *pvalue)
{
   int i;

   for (i = 0; i < MIN(info->num_channels, channels); i++)
      epics_ca_get(info, i, pvalue + i);

   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT epics_ca(INT cmd, ...)
{
   va_list argptr;
   HNDLE hKey;
   INT channel, status;
   DWORD flags;
   float value, *pvalue;
   CA_INFO *info;
   char *label;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

   if (cmd == CMD_INIT) {
      void *pinfo;

      hKey = va_arg(argptr, HNDLE);
      pinfo = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      flags = va_arg(argptr, DWORD);
      status = epics_ca_init(hKey, pinfo, channel);
      info = *(CA_INFO **) pinfo;
      info->flags = flags;
   } else {
      info = va_arg(argptr, void *);

      /* only execute command if enabled */
      if (cmd & info->cmd_disabled)
         status = FE_ERR_DISABLED;
      else
         switch (cmd) {
         case CMD_INIT:
            break;

         case CMD_EXIT:
            status = epics_ca_exit(info);
            break;

         case CMD_SET:
            channel = va_arg(argptr, INT);
            value = (float) va_arg(argptr, double);
            status = epics_ca_set(info, channel, value);
            break;

         case CMD_SET_ALL:
            channel = va_arg(argptr, INT);
            value = (float) va_arg(argptr, double);
            status = epics_ca_set_all(info, channel, value);
            break;

         case CMD_SET_LABEL:
            channel = va_arg(argptr, INT);
            label = va_arg(argptr, char *);
            status = epics_ca_set_label(info, channel, label);
            break;

         case CMD_GET:
            channel = va_arg(argptr, INT);
            pvalue = va_arg(argptr, float *);
            status = epics_ca_get(info, channel, pvalue);
            break;

         case CMD_GET_ALL:
            channel = va_arg(argptr, INT);
            pvalue = va_arg(argptr, float *);
            status = epics_ca_get_all(info, channel, pvalue);
            break;

         default:
            break;
         }
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
