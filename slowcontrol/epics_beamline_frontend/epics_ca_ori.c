/********************************************************************\

  Name:         epics_ca.c
  Created by:   Stefan Ritt

  Contents:     Epics channel access device driver

  $Id: epics_ca_ori.c,v 1.2 2011-06-25 20:56:00 l_data Exp $

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

#define MIDEBUG

/*---- globals -----------------------------------------------------*/

#define CHN_NAME_LENGTH 32      /* length of channel names */

typedef struct {
   char *channel_names;
   chid *pv_handles;
   float *array;
   INT num_channels;
   DWORD flags;
   INT cmd_disabled;
} CA_INFO;


void epics_ca_callback(struct event_handler_args args)
{
   CA_INFO *info;
   int i;
#ifdef MIDEBUG
   cm_msg(MLOG,"","++epics_ca_callback()");
#endif
   info = (CA_INFO *) args.usr;

   /* search channel index */
   for (i = 0; i < info->num_channels; i++)
      if (info->pv_handles[i] == args.chid) {
#ifdef MIDEBUG
         cm_msg(MLOG,"","callback for channel %d", i);
#endif
         break;
      }

   if (i < info->num_channels)
      info->array[i] = *((float *) args.dbr);

#ifdef MIDEBUG
   cm_msg(MLOG,"","--epics_ca_callback()");
#endif
}

/*---- device driver routines --------------------------------------*/

INT epics_ca_init(HNDLE hKey, void **pinfo, INT channels)
{
   int status, i;
   HNDLE hDB;
   CA_INFO *info;

#ifdef MIDEBUG
   cm_msg(MLOG,"","++epics_ca_init(%d channels)",channels);
#endif
   /* allocate info structure */
   info = calloc(1, sizeof(CA_INFO));
   *pinfo = info;

   /* disable all commands by default */
   info->cmd_disabled = 1;

   cm_get_experiment_database(&hDB, NULL);

   /* get channel names */
   info->channel_names = calloc(channels, CHN_NAME_LENGTH);
   for (i = 0; i < channels; i++)
      sprintf(info->channel_names + CHN_NAME_LENGTH * i, "Channel%d", i);
   db_merge_data(hDB, hKey, "Channel name",
                 info->channel_names, CHN_NAME_LENGTH * channels, channels, TID_STRING);

   /* initialize driver */
   status = ca_task_initialize();
   if (!(status & ECA_NORMAL)) {
      cm_msg(MERROR, "epics_ca_init", "Unable to initialize! ca_task_initialize() "
                                      "returned 0X%x",status);
      return FE_ERR_HW;
   }

   /* allocate arrays */
   info->array = calloc(channels, sizeof(float));
   info->pv_handles = calloc(channels, sizeof(void *));
   for (i = 0; i < channels; i++)
      info->pv_handles[i] = calloc(1, sizeof(chid));

   /* search channels */
   info->num_channels = channels;

   for (i = 0; i < channels; i++) {
      status = ca_search(info->channel_names+CHN_NAME_LENGTH * i, &info->pv_handles[i]);
      if (!(status & ECA_NORMAL))
         cm_msg(MERROR, "epics_ca_init", "ERROR 0X%x returned by ca_search(%s)",
                status, info->channel_names + CHN_NAME_LENGTH * i);
   }

   if ((status=ca_pend_io(5.0)) == ECA_TIMEOUT) {
      for (i = 0; i < channels; i++)
         if (ca_state(info->pv_handles[i]) != cs_conn)
            cm_msg(MERROR, "epics_ca_init", "Not connect to %s",
                   info->channel_names + CHN_NAME_LENGTH * i);
   } else if (status != ECA_NORMAL) {
     // NIY unknown status
   }

   /* add change notifications */
   for (i = 0; i < channels; i++) {
      /* ignore unconnected channels */
      if (ca_state(info->pv_handles[i]) != cs_conn) {
         // NIY ignoring unconnected channel %s
         continue;
      }
      status = ca_add_event(DBR_FLOAT, info->pv_handles[i], epics_ca_callback,
                            info, NULL);
      if (!(status & ECA_NORMAL))
         cm_msg(MERROR, "epics_ca_init", "channel %s ca_add_event() returned 0X%x",
                info->channel_names + CHN_NAME_LENGTH * i, status);
   }

   if ((status=ca_pend_io(5.0)) == ECA_TIMEOUT) {
      for (i = 0; i < channels; i++)
         if (ca_state(info->pv_handles[i]) != cs_conn)
            cm_msg(MERROR, "epics_ca_init", "Not connect to %s",
                   info->channel_names + CHN_NAME_LENGTH * i);
   } else if (status != ECA_NORMAL) {
     // NIY unknown status
   }

   /* enable all commands by default */
   info->cmd_disabled = 0;

#ifdef MIDEBUG
   cm_msg(MLOG,"","--epics_ca_init()");
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_exit(CA_INFO * info)
{
   int i;

#ifdef MIDEBUG
   cm_msg(MLOG,"","++epics_ca_exit()");
#endif

   ca_task_exit();

   if (info->pv_handles) {
      for (i = 0; i < info->num_channels; i++)
         if (info->pv_handles[i]) {
            free(info->pv_handles[i]);
            info->pv_handles[i] = NULL;
         }
      free(info->pv_handles);
   }

   if (info->array)
      free(info->array);
   if (info->channel_names)
      free(info->channel_names);

   free(info);

#ifdef MIDEBUG
   cm_msg(MLOG,"","--epics_ca_exit()");
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_set(CA_INFO * info, INT channel, float value)
{
#ifdef MIDEBUG
   cm_msg(MLOG,"","++epics_ca_set(channel %d, value %f)",channel,value);
#endif
   //ca_put(DBR_FLOAT, info->pv_handles[channel], &value);

#ifdef MIDEBUG
   cm_msg(MLOG,"","--epics_ca_set()");
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_set_all(CA_INFO * info, INT channels, float value)
{
   INT i;

#ifdef MIDEBUG
   cm_msg(MLOG,"","++epics_ca_set_all(%d channels)",channels);
#endif
   for (i = 0; i < MIN(info->num_channels, channels); i++)
      ca_put(DBR_FLOAT, info->pv_handles[i], &value);

#ifdef MIDEBUG
   cm_msg(MLOG,"","--epics_ca_set_all()");
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_set_label(CA_INFO * info, INT channels, char *label)
{
   int status;
   char str[256];
   chid chan_id;

#ifdef MIDEBUG
   cm_msg(MLOG,"","++epics_ca_set_label(channel %d, label %s)", channels, label);
#endif
   sprintf(str, "%s.DESC", info->channel_names + CHN_NAME_LENGTH * channels);
   status = ca_search(str, &chan_id);

   status = ca_pend_io(2.0);
   if (status != ECA_NORMAL)
      printf("%s not found\n", str);

   status = ca_put(ca_field_type(chan_id), chan_id, label);
   ca_pend_event(0.01);

#ifdef MIDEBUG
   cm_msg(MLOG,"","--epics_ca_set_label()");
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_get(CA_INFO * info, INT channel, float *pvalue)
{
#ifdef MIDEBUG
   cm_msg(MLOG,"","++epics_ca_get(channel %d)",channel);
#endif
   ca_pend_event(0.0001);
   if (pvalue) *pvalue = info->array[channel];

#ifdef MIDEBUG
   if (pvalue)
     cm_msg(MLOG,"","--epics_ca_get(value = %f)",*pvalue);
   else
     cm_msg(MLOG,"","--epics_ca_get(NULL pointer to return)");
#endif
   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

INT epics_ca_get_all(CA_INFO * info, INT channels, float *pvalue)
{
   int i;

#ifdef MIDEBUG
   cm_msg(MLOG,"","++epics_ca_get_all(%d channels)", channels);
#endif
   for (i = 0; i < MIN(info->num_channels, channels); i++)
      epics_ca_get(info, i, pvalue + i);

#ifdef MIDEBUG
   cm_msg(MLOG,"","--epics_ca_get_all()");
#endif
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

#ifdef MIDEBUG
   cm_msg(MLOG,"","++epics_ca(CMD=%d)",cmd);
#endif
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

#ifdef MIDEBUG
   cm_msg(MLOG,"","--epics_ca()");
#endif
   return status;
}

/*------------------------------------------------------------------*/
