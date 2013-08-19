/********************************************************************\

  Name:         slowdev.c
  Created by:   Pierre-Andre Amaudruz

  Contents:     Slow Device Generic Class Driver
  Based on the generic class driver, implements a check channel
  in the idle task based on circular/alternate on minimum of
  n raised channels + one. See doc for further explanation.

  $Log: slowdev.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.1  2002/05/10 05:47:37  pierre
  based on generic + idle on N raised + 1


\********************************************************************/

#include <stdio.h>
#include "midas.h"
#include "ybos.h"

#define ODB_UPDATE_TIME 5000

typedef struct {

   /* ODB keys */
   HNDLE hKeyRoot, hKeyDemand, hKeyMeasured;

   /* globals */
   INT num_channels;
   INT format;
   DWORD last_update;
   INT last_channel;

   /* items in /Variables record */
   float *demand;
   float *measured;
   BOOL *raised;

   /* items in /Settings */
   char *names;
   float *update_threshold;

   /* mirror arrays */
   float *demand_mirror;
   float *measured_mirror;
   DWORD *last_change;

   void **driver;
   INT *channel_offset;
   void **dd_info;

} GEN_INFO;

#define DRIVER(_i) ((INT (*)(INT cmd, ...))(gen_info->driver[_i]))

#ifndef abs
#define abs(a) (((a) < 0)   ? -(a) : (a))
#endif

/*------------------------------------------------------------------*/

static void free_mem(GEN_INFO * gen_info)
{
   free(gen_info->names);
   free(gen_info->demand);
   free(gen_info->measured);
   free(gen_info->raised);

   free(gen_info->update_threshold);

   free(gen_info->demand_mirror);
   free(gen_info->measured_mirror);
   free(gen_info->last_change);

   free(gen_info->dd_info);
   free(gen_info->channel_offset);
   free(gen_info->driver);

   free(gen_info);
}

/*------------------------------------------------------------------*/

INT gen_read(EQUIPMENT * pequipment, int channel)
{
   int i, status;
   float max_diff;
   DWORD act_time, min_time;
   BOOL changed;
   GEN_INFO *gen_info;
   HNDLE hDB;

  /*---- read measured value ----*/
   gen_info = (GEN_INFO *) pequipment->cd_info;
   cm_get_experiment_database(&hDB, NULL);

   /* Get channel value */
   status = DRIVER(channel) (CMD_GET, gen_info->dd_info[channel],
                             channel - gen_info->channel_offset[channel],
                             &gen_info->measured[channel]);

  /*---- read demand value ----*/
   status = DRIVER(channel) (CMD_GET_DEMAND, gen_info->dd_info[channel],
                             channel - gen_info->channel_offset[channel],
                             &gen_info->demand[channel]);

   if (gen_info->demand[channel] != gen_info->demand_mirror[channel]) {
      gen_info->demand_mirror[channel] = gen_info->demand[channel];
      db_set_data(hDB, gen_info->hKeyDemand, gen_info->demand,
                  sizeof(float) * gen_info->num_channels, gen_info->num_channels,
                  TID_FLOAT);
   }

   return status;
}

/*------------------------------------------------------------------*/

void gen_demand(INT hDB, INT hKey, void *info)
{
   INT i, status;
   GEN_INFO *gen_info;
   EQUIPMENT *pequipment;

   pequipment = (EQUIPMENT *) info;
   gen_info = (GEN_INFO *) pequipment->cd_info;

   /* set individual channels only if demand value differs */
   for (i = 0; i < gen_info->num_channels; i++)
      if (gen_info->demand[i] != gen_info->demand_mirror[i]) {
         status = DRIVER(i) (CMD_SET, gen_info->dd_info[i],
                             i - gen_info->channel_offset[i], gen_info->demand[i]);
         gen_info->demand_mirror[i] = gen_info->demand[i];
         gen_info->last_change[i] = ss_millitime();
      }

   pequipment->odb_in++;
}

/*------------------------------------------------------------------*/

void gen_update_label(INT hDB, INT hKey, void *info)
{
   INT i, status;
   GEN_INFO *gen_info;
   EQUIPMENT *pequipment;

   pequipment = (EQUIPMENT *) info;
   gen_info = (GEN_INFO *) pequipment->cd_info;

   /* update channel labels based on the midas channel names */
   for (i = 0; i < gen_info->num_channels; i++)
      status = DRIVER(i) (CMD_SET_LABEL, gen_info->dd_info[i],
                          i - gen_info->channel_offset[i],
                          gen_info->names + NAME_LENGTH * i);
}

/*------------------------------------------------------------------*/

INT gen_init(EQUIPMENT * pequipment)
{
   int status, size, i, j, index, offset;
   char str[256];
   HNDLE hDB, hKey, hNames;
   GEN_INFO *gen_info;

   /* allocate private data */
   pequipment->cd_info = calloc(1, sizeof(GEN_INFO));
   gen_info = (GEN_INFO *) pequipment->cd_info;

   /* get class driver root key */
   cm_get_experiment_database(&hDB, NULL);
   sprintf(str, "/Equipment/%s", pequipment->name);
   db_create_key(hDB, 0, str, TID_KEY);
   db_find_key(hDB, 0, str, &gen_info->hKeyRoot);

   /* save event format */
   size = sizeof(str);
   db_get_value(hDB, gen_info->hKeyRoot, "Common/Format", str, &size, TID_STRING, TRUE);

   if (equal_ustring(str, "Fixed"))
      gen_info->format = FORMAT_FIXED;
   else if (equal_ustring(str, "MIDAS"))
      gen_info->format = FORMAT_MIDAS;
   else if (equal_ustring(str, "YBOS"))
      gen_info->format = FORMAT_YBOS;

   /* count total number of channels */
   db_create_key(hDB, gen_info->hKeyRoot, "Settings/Channels", TID_KEY);
   db_find_key(hDB, gen_info->hKeyRoot, "Settings/Channels", &hKey);

   for (i = 0, gen_info->num_channels = 0; pequipment->driver[i].name[0]; i++) {
      /* ODB value has priority over driver list in channel number */
      size = sizeof(INT);
      db_get_value(hDB, hKey, pequipment->driver[i].name,
                   &pequipment->driver[i].channels, &size, TID_INT, TRUE);

      if (pequipment->driver[i].channels == 0)
         pequipment->driver[i].channels = 1;

      gen_info->num_channels += pequipment->driver[i].channels;
   }

   if (gen_info->num_channels == 0) {
      cm_msg(MERROR, "hv_init", "No channels found in device driver list");
      return FE_ERR_ODB;
   }

   /* Allocate memory for buffers */
   gen_info->names = (char *) calloc(gen_info->num_channels, NAME_LENGTH);

   gen_info->demand = (float *) calloc(gen_info->num_channels, sizeof(float));
   gen_info->measured = (float *) calloc(gen_info->num_channels, sizeof(float));
   gen_info->raised = (BOOL *) calloc(gen_info->num_channels, sizeof(BOOL));

   gen_info->update_threshold = (float *) calloc(gen_info->num_channels, sizeof(float));

   gen_info->demand_mirror = (float *) calloc(gen_info->num_channels, sizeof(float));
   gen_info->measured_mirror = (float *) calloc(gen_info->num_channels, sizeof(float));
   gen_info->last_change = (DWORD *) calloc(gen_info->num_channels, sizeof(DWORD));

   gen_info->dd_info = (void *) calloc(gen_info->num_channels, sizeof(void *));
   gen_info->channel_offset = (INT *) calloc(gen_info->num_channels, sizeof(INT));
   gen_info->driver = (void *) calloc(gen_info->num_channels, sizeof(void *));

   if (!gen_info->driver) {
      cm_msg(MERROR, "hv_init", "Not enough memory");
      return FE_ERR_ODB;
   }

  /*---- Initialize device drivers ----*/

   /* call init method */
   for (i = 0; pequipment->driver[i].name[0]; i++) {
      sprintf(str, "Settings/Devices/%s", pequipment->driver[i].name);
      status = db_find_key(hDB, gen_info->hKeyRoot, str, &hKey);
      if (status != DB_SUCCESS) {
         db_create_key(hDB, gen_info->hKeyRoot, str, TID_KEY);
         status = db_find_key(hDB, gen_info->hKeyRoot, str, &hKey);
         if (status != DB_SUCCESS) {
            cm_msg(MERROR, "hv_init", "Cannot create %s entry in online database", str);
            free_mem(gen_info);
            return FE_ERR_ODB;
         }
      }

      status = pequipment->driver[i].dd(CMD_INIT, hKey, &pequipment->driver[i].dd_info,
                                        pequipment->driver[i].channels,
                                        pequipment->driver[i].cmd_disabled);
      if (status != FE_SUCCESS) {
         free_mem(gen_info);
         return status;
      }
   }

   /* compose device driver channel assignment */
   for (i = 0, j = 0, index = 0, offset = 0; i < gen_info->num_channels; i++, j++) {
      while (j >= pequipment->driver[index].channels && pequipment->driver[index].name[0]) {
         offset += j;
         index++;
         j = 0;
      }

      gen_info->driver[i] = pequipment->driver[index].dd;
      gen_info->dd_info[i] = pequipment->driver[index].dd_info;
      gen_info->channel_offset[i] = offset;
   }

  /*---- create demand variables ----*/

   /* get demand from ODB */
   status =
       db_find_key(hDB, gen_info->hKeyRoot, "Variables/Demand", &gen_info->hKeyDemand);
   if (status == DB_SUCCESS) {
      size = sizeof(float) * gen_info->num_channels;
      db_get_data(hDB, gen_info->hKeyDemand, gen_info->demand, &size, TID_FLOAT);
   }
   /* let device driver overwrite demand values, if it supports it */
   for (i = 0; i < gen_info->num_channels; i++) {
      DRIVER(i) (CMD_GET_DEMAND, gen_info->dd_info[i],
                 i - gen_info->channel_offset[i], &gen_info->demand[i]);
      gen_info->demand_mirror[i] = -12345.f;    /* invalid value */
   }
   /* write back demand values */
   status =
       db_find_key(hDB, gen_info->hKeyRoot, "Variables/Demand", &gen_info->hKeyDemand);
   if (status != DB_SUCCESS) {
      db_create_key(hDB, gen_info->hKeyRoot, "Variables/Demand", TID_FLOAT);
      db_find_key(hDB, gen_info->hKeyRoot, "Variables/Demand", &gen_info->hKeyDemand);
   }
   size = sizeof(float) * gen_info->num_channels;
   db_set_data(hDB, gen_info->hKeyDemand, gen_info->demand, size,
               gen_info->num_channels, TID_FLOAT);
   db_open_record(hDB, gen_info->hKeyDemand, gen_info->demand,
                  gen_info->num_channels * sizeof(float), MODE_READ, gen_demand,
                  pequipment);

  /*---- create measured variables ----*/
   db_merge_data(hDB, gen_info->hKeyRoot, "Variables/Measured",
                 gen_info->measured, sizeof(float) * gen_info->num_channels,
                 gen_info->num_channels, TID_FLOAT);
   db_find_key(hDB, gen_info->hKeyRoot, "Variables/Measured", &gen_info->hKeyMeasured);
   memcpy(gen_info->measured_mirror, gen_info->measured,
          gen_info->num_channels * sizeof(float));

  /*---- get default names from device driver ----*/
   for (i = 0; i < gen_info->num_channels; i++) {
      sprintf(gen_info->names + NAME_LENGTH * i, "Default%%CH %d", i);
      DRIVER(i) (CMD_GET_DEFAULT_NAME, gen_info->dd_info[i],
                 i - gen_info->channel_offset[i], gen_info->names + NAME_LENGTH * i);
   }
   db_merge_data(hDB, gen_info->hKeyRoot, "Settings/Names",
                 gen_info->names, NAME_LENGTH * gen_info->num_channels,
                 gen_info->num_channels, TID_STRING);

  /*---- set labels form midas SC names ----*/
   for (i = 0; i < gen_info->num_channels; i++) {
      gen_info = (GEN_INFO *) pequipment->cd_info;
      DRIVER(i) (CMD_SET_LABEL, gen_info->dd_info[i],
                 i - gen_info->channel_offset[i], gen_info->names + NAME_LENGTH * i);
   }

   /* open hotlink on channel names */
   if (db_find_key(hDB, gen_info->hKeyRoot, "Settings/Names", &hNames) == DB_SUCCESS)
      db_open_record(hDB, hNames, gen_info->names, NAME_LENGTH * gen_info->num_channels,
                     MODE_READ, gen_update_label, pequipment);

  /*---- get default update threshold from device driver ----*/
   for (i = 0; i < gen_info->num_channels; i++) {
      gen_info->update_threshold[i] = 2.f;      /* default 2 units */
      DRIVER(i) (CMD_GET_DEFAULT_THRESHOLD, gen_info->dd_info[i],
                 i - gen_info->channel_offset[i], &gen_info->update_threshold[i]);
   }
   db_merge_data(hDB, gen_info->hKeyRoot, "Settings/Update Threshold Measured",
                 gen_info->update_threshold, sizeof(float) * gen_info->num_channels,
                 gen_info->num_channels, TID_FLOAT);

  /*---- set initial demand values ----*/
   gen_demand(hDB, gen_info->hKeyDemand, pequipment);

   /* initially read all channels */
   for (i = 0; i < gen_info->num_channels; i++)
      gen_read(pequipment, i);

   return FE_SUCCESS;
}

/*------------------------------------------------------------------*/

INT gen_exit(EQUIPMENT * pequipment)
{
   INT i;

   free_mem((GEN_INFO *) pequipment->cd_info);

   /* call exit method of device drivers */
   for (i = 0; pequipment->driver[i].dd != NULL; i++)
      pequipment->driver[i].dd(CMD_EXIT, pequipment->driver[i].dd_info);

   return FE_SUCCESS;
}

/*------------------------------------------------------------------*/
INT gen_idle(EQUIPMENT * pequipment)
{
   static BOOL odb_update_flag = FALSE;
   static INT last_raised_channel = -1;
   static INT number_raised = 0;
   INT act, i, next_raised, status;
   DWORD act_time, loop;
   GEN_INFO *gen_info;
   HNDLE hDB;

   /* Class info shortcut */
   gen_info = (GEN_INFO *) pequipment->cd_info;

   /* get current time */
   act_time = ss_millitime();

   /* Do loop N channel per idle */
   for (loop = 0; loop < number_raised + 1; loop++) {

      /* Select channel to read */
      /* k = last_raised_channel; */

      if (last_raised_channel < 0) {
         /* Normal channel */
         act = (gen_info->last_channel + 1) % gen_info->num_channels;
//      printf("N     :%d ", act);
      } else {
         /* Raised channel */
         act = last_raised_channel;
//      printf("    R :%d ", act);
      }

      /* Read channel */
      status = gen_read(pequipment, act);

      /* Check read value against previous value (mirror) */
      if (abs(gen_info->measured[act] - gen_info->measured_mirror[act]) >
          gen_info->update_threshold[act]) {
         /* Greater than Delta => raise channel for close monitoring */
         gen_info->raised[act] = TRUE;
         /* Force an ODB update to reflect change */
         odb_update_flag = TRUE;
      } else {
         /* Within the limits => lower channel if it was raised */
         gen_info->raised[act] = FALSE;
      }

//    printf("F:%d mes:%f mir:%f th:%f\n", gen_info->raised[act]
//         , gen_info->measured[act]
//         ,gen_info->measured_mirror[act], gen_info->update_threshold[act]);

      /* hold last value */
      gen_info->measured_mirror[act] = gen_info->measured[act];

      /* Get next raised channel if any (-1:none) */
      if (act == gen_info->num_channels) {
         next_raised = -1;
      } else {
         for (next_raised = act + 1; next_raised < gen_info->num_channels; next_raised++) {
            if (gen_info->raised[next_raised])
               break;
         }
         if (next_raised == gen_info->num_channels) {
            next_raised = -1;
         }
      }

      /* update last raised channel for next go */
      if (next_raised < 0) {
         /* no more raised channel set normal channel (increment channel) */
         gen_info->last_channel = (gen_info->last_channel + 1) % gen_info->num_channels;
         last_raised_channel = -1;
      } else {
         /* More raised channel then set for next round */
         last_raised_channel = next_raised;
      }

      /* count the raised channels
         to be used for possible multiple read per idle
         see for loop at begining of function.
       */
      for (i = 0, number_raised = 0; i < gen_info->num_channels; i++) {
         if (gen_info->raised[i])
            number_raised++;
      }
   }

   /* sends new values to ODB if:
      ODB_UPDATE_TIME timeout exceeded
      or
      At least one channel has been raised */
   if (odb_update_flag || ((act_time - gen_info->last_update) > ODB_UPDATE_TIME)) {
      cm_get_experiment_database(&hDB, NULL);
      db_set_data(hDB, gen_info->hKeyMeasured, gen_info->measured,
                  sizeof(float) * gen_info->num_channels, gen_info->num_channels,
                  TID_FLOAT);
      gen_info->last_update = act_time;
      odb_update_flag = FALSE;

      /* Keep odb count */
      pequipment->odb_out++;
   }
   return status;
}

/*------------------------------------------------------------------*/
INT cd_gen_read(char *pevent, int offset)
{
   float *pdata;
   DWORD *pdw;
   GEN_INFO *gen_info;
   EQUIPMENT *pequipment;

   pequipment = *((EQUIPMENT **) pevent);
   gen_info = (GEN_INFO *) pequipment->cd_info;

   if (gen_info->format == FORMAT_FIXED) {
      memcpy(pevent, gen_info->demand, sizeof(float) * gen_info->num_channels);
      pevent += sizeof(float) * gen_info->num_channels;

      memcpy(pevent, gen_info->measured, sizeof(float) * gen_info->num_channels);
      pevent += sizeof(float) * gen_info->num_channels;

      return 2 * sizeof(float) * gen_info->num_channels;
   } else if (gen_info->format == FORMAT_MIDAS) {
      bk_init(pevent);

      /* create DMND bank */
      bk_create(pevent, "DMND", TID_FLOAT, &pdata);
      memcpy(pdata, gen_info->demand, sizeof(float) * gen_info->num_channels);
      pdata += gen_info->num_channels;
      bk_close(pevent, pdata);

      /* create MSRD bank */
      bk_create(pevent, "MSRD", TID_FLOAT, &pdata);
      memcpy(pdata, gen_info->measured, sizeof(float) * gen_info->num_channels);
      pdata += gen_info->num_channels;
      bk_close(pevent, pdata);

      return bk_size(pevent);
   } else if (gen_info->format == FORMAT_YBOS) {
      ybk_init((DWORD *) pevent);

      /* create EVID bank */
      ybk_create((DWORD *) pevent, "EVID", I4_BKTYPE, (DWORD *) (&pdw));
      *(pdw)++ = EVENT_ID(pevent);      /* Event_ID + Mask */
      *(pdw)++ = SERIAL_NUMBER(pevent); /* Serial number */
      *(pdw)++ = TIME_STAMP(pevent);    /* Time Stamp */
      ybk_close((DWORD *) pevent, pdw);

      /* create DMND bank */
      ybk_create((DWORD *) pevent, "DMND", F4_BKTYPE, (DWORD *) & pdata);
      memcpy(pdata, gen_info->demand, sizeof(float) * gen_info->num_channels);
      pdata += gen_info->num_channels;
      ybk_close((DWORD *) pevent, pdata);

      /* create MSRD bank */
      ybk_create((DWORD *) pevent, "MSRD", F4_BKTYPE, (DWORD *) & pdata);
      memcpy(pdata, gen_info->measured, sizeof(float) * gen_info->num_channels);
      pdata += gen_info->num_channels;
      ybk_close((DWORD *) pevent, pdata);

      return ybk_size((DWORD *) pevent);
   } else
      return 0;
}

/*------------------------------------------------------------------*/

INT cd_gen(INT cmd, EQUIPMENT * pequipment)
{
   INT status;

   switch (cmd) {
   case CMD_INIT:
      status = gen_init(pequipment);
      break;

   case CMD_EXIT:
      status = gen_exit(pequipment);
      break;

   case CMD_IDLE:
      status = gen_idle(pequipment);
      break;

   default:
      cm_msg(MERROR, "Generic class driver", "Received unknown command %d", cmd);
      status = FE_ERR_DRIVER;
      break;
   }

   return status;
}
