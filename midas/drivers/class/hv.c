/********************************************************************\

  Name:         hv.c
  Created by:   Stefan Ritt

  Contents:     High Voltage Class Driver

  $Log: hv.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:08  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.18  2004/05/07 19:40:10  midas
  Replaced min/max by MIN/MAX macros

  Revision 1.17  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.16  2003/10/30 12:37:33  midas
  Don't overwrite number of channels from ODB

  Revision 1.15  2003/10/06 09:17:10  midas
  Output commect if # channels overwritten by ODB

  Revision 1.14  2003/09/29 11:57:30  midas
  Fixed bug with driver entry point offset

  Revision 1.13  2003/09/23 09:16:37  midas
  Added <math.h>

  Revision 1.12  2003/08/26 23:27:40  pierre
  Cannot use // to denote comments because this does not compile under VxWorks; replaced all new // introduced on 2003/06/03 by normal comments

  Revision 1.11  2003/06/03 09:09:39  suter_a
  allow negative high voltage values and adopeted hv_ramp accordingly

  Revision 1.10  2003/05/12 12:05:26  midas
  Removed tabs

  Revision 1.9  2003/03/06 11:43:11  midas
  Added CMD_SET_LABEL routines

  Revision 1.8  2002/06/06 08:06:32  midas
  Implemented DF_xx scheme

  Revision 1.7  2002/06/06 07:50:12  midas
  Implemented scheme with DF_xxx flags

  Revision 1.6  2002/05/08 19:54:40  midas
  Added extra parameter to function db_get_value()

  Revision 1.5  2002/03/14 13:02:48  midas
  Added ramping speed for both up/down

  Revision 1.4  2001/04/04 04:14:04  midas
  Use CMD_SET_CURRENT_LIMIT_ALL, use CMD_SET_ALL when more than 10% of channel have changed

  Revision 1.3  2001/01/03 16:20:07  midas
  Added Bus Driver scheme

  Revision 1.2  2000/03/02 21:54:02  midas
  Added offset in readout routines, added cmd_set_label

  Revision 1.1  1999/12/20 10:18:16  midas
  Reorganized driver directory structure

  Revision 1.4  1999/10/27 14:05:03  midas
  Fixed bug that demand_mirror was not initialized correctly

  Revision 1.3  1999/06/25 13:49:46  midas
  Print message "setting channels"

  Revision 1.2  1998/10/12 12:18:55  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <math.h>
#include "midas.h"
#include "ybos.h"

typedef struct {

   /* ODB keys */
   HNDLE hKeyRoot, hKeyDemand, hKeyMeasured, hKeyCurrent;

   /* globals */
   INT num_channels;
   INT format;
   DWORD last_update;
   INT last_channel;

   /* items in /Variables record */
   char *names;
   float *demand;
   float *measured;
   float *current;

   /* items in /Settings */
   float *update_threshold;
   float *update_threshold_current;
   float *voltage_limit;
   float *current_limit;
   float *rampup_speed;
   float *rampdown_speed;

   /* mirror arrays */
   float *demand_mirror;
   float *measured_mirror;
   float *current_mirror;
   DWORD *last_change;

   void **driver;
   INT *channel_offset;
   void **dd_info;
   DWORD *flags;

} HV_INFO;

#define DRIVER(_i) ((INT (*)(INT cmd, ...))(hv_info->driver[_i]))

#ifndef abs
#define abs(a) (((a) < 0)   ? -(a) : (a))
#endif

/*------------------------------------------------------------------*/

static void free_mem(HV_INFO * hv_info)
{
   free(hv_info->names);
   free(hv_info->demand);
   free(hv_info->measured);
   free(hv_info->current);

   free(hv_info->update_threshold);
   free(hv_info->update_threshold_current);
   free(hv_info->voltage_limit);
   free(hv_info->current_limit);
   free(hv_info->rampup_speed);
   free(hv_info->rampdown_speed);

   free(hv_info->demand_mirror);
   free(hv_info->measured_mirror);
   free(hv_info->current_mirror);
   free(hv_info->last_change);

   free(hv_info->dd_info);
   free(hv_info->channel_offset);
   free(hv_info->driver);
   free(hv_info->flags);

   free(hv_info);
}

/*------------------------------------------------------------------*/

INT hv_read(EQUIPMENT * pequipment, int channel)
{
   int i, j, status;
   float max_diff;
   DWORD act_time, min_time;
   BOOL changed;
   HV_INFO *hv_info;
   HNDLE hDB;

   hv_info = (HV_INFO *) pequipment->cd_info;
   cm_get_experiment_database(&hDB, NULL);

   if (channel == -1) {
      for (i = 0, j = 0; pequipment->driver[i].name[0]; i++) {
         status = pequipment->driver[i].dd(CMD_GET_ALL,
                                           pequipment->driver[i].dd_info,
                                           pequipment->driver[i].channels,
                                           hv_info->measured + j);
         status = pequipment->driver[i].dd(CMD_GET_CURRENT_ALL,
                                           pequipment->driver[i].dd_info,
                                           pequipment->driver[i].channels,
                                           hv_info->current + j);

         j += pequipment->driver[i].channels;
      }
   } else {
      status = DRIVER(channel) (CMD_GET, hv_info->dd_info[channel],
                                channel - hv_info->channel_offset[channel],
                                &hv_info->measured[channel]);
      status = DRIVER(channel) (CMD_GET_CURRENT, hv_info->dd_info[channel],
                                channel - hv_info->channel_offset[channel],
                                &hv_info->current[channel]);
   }

   /* check how much channels have changed since last ODB update */
   act_time = ss_millitime();

   /* check for update measured */
   max_diff = 0.f;
   min_time = 10000;
   changed = FALSE;
   for (i = 0; i < hv_info->num_channels; i++) {
      if (abs(hv_info->measured[i] - hv_info->measured_mirror[i]) > max_diff)
         max_diff = abs(hv_info->measured[i] - hv_info->measured_mirror[i]);

      if (abs(hv_info->measured[i] - hv_info->measured_mirror[i]) >
          hv_info->update_threshold[i])
         changed = TRUE;

      if (act_time - hv_info->last_change[i] < min_time)
         min_time = act_time - hv_info->last_change[i];
   }

   /* update if change is more than update_sensitivity or less than 2sec ago 
      or last update is older than a minute */
   if (changed || (min_time < 2000 && max_diff > 0) ||
       act_time - hv_info->last_update > 60000) {
      hv_info->last_update = act_time;

      for (i = 0; i < hv_info->num_channels; i++)
         hv_info->measured_mirror[i] = hv_info->measured[i];

      db_set_data(hDB, hv_info->hKeyMeasured, hv_info->measured,
                  sizeof(float) * hv_info->num_channels, hv_info->num_channels,
                  TID_FLOAT);

      pequipment->odb_out++;
   }

   /* check for update current */
   max_diff = 0.f;
   min_time = 10000;
   changed = FALSE;
   for (i = 0; i < hv_info->num_channels; i++) {
      if (abs(hv_info->current[i] - hv_info->current_mirror[i]) > max_diff)
         max_diff = abs(hv_info->current[i] - hv_info->current_mirror[i]);

      if (abs(hv_info->current[i] - hv_info->current_mirror[i]) >
          hv_info->update_threshold_current[i])
         changed = TRUE;

      if (act_time - hv_info->last_change[i] < min_time)
         min_time = act_time - hv_info->last_change[i];
   }

   /* update if change is more than update_sensitivity or less than 2sec ago */
   if (changed || (min_time < 2000 && max_diff > 0)) {
      for (i = 0; i < hv_info->num_channels; i++)
         hv_info->current_mirror[i] = hv_info->current[i];

      db_set_data(hDB, hv_info->hKeyCurrent, hv_info->current,
                  sizeof(float) * hv_info->num_channels, hv_info->num_channels,
                  TID_FLOAT);

      pequipment->odb_out++;
   }

   return status;
}

/*------------------------------------------------------------------*/

INT hv_ramp(HV_INFO * hv_info)
{
   INT i, status, switch_tag;
   float delta, ramp_speed;

   for (i = 0; i < hv_info->num_channels; i++) {
      if (hv_info->demand[i] != hv_info->demand_mirror[i]) {
         /* check if to ramp up or down */
         if ((hv_info->demand[i] >= 0.f) && (hv_info->demand_mirror[i] > 0.f)) {
            switch_tag = FALSE;
            if (hv_info->demand[i] > hv_info->demand_mirror[i])
               ramp_speed = hv_info->rampup_speed[i];
            else
               ramp_speed = hv_info->rampdown_speed[i];
         }
         if ((hv_info->demand[i] >= 0.f) && (hv_info->demand_mirror[i] < 0.f)) {
            switch_tag = TRUE;
            ramp_speed = hv_info->rampdown_speed[i];
         }
         if ((hv_info->demand[i] < 0.f) && (hv_info->demand_mirror[i] > 0.f)) {
            switch_tag = TRUE;
            ramp_speed = hv_info->rampdown_speed[i];
         }
         if ((hv_info->demand[i] < 0.f) && (hv_info->demand_mirror[i] < 0.f)) {
            switch_tag = FALSE;
            if (hv_info->demand[i] > hv_info->demand_mirror[i])
               ramp_speed = hv_info->rampdown_speed[i];
            else
               ramp_speed = hv_info->rampup_speed[i];
         }
         if (hv_info->demand_mirror[i] == 0.f) {
            switch_tag = FALSE;
            ramp_speed = hv_info->rampup_speed[i];
         }

         if (ramp_speed == 0.f)
            if (switch_tag)
               hv_info->demand_mirror[i] = 0.f; /* go to zero */
            else
               hv_info->demand_mirror[i] = hv_info->demand[i];  /* step directly to the new high voltage */
         else {
            delta = (float) ((ss_millitime() -
                              hv_info->last_change[i]) / 1000.0 * ramp_speed);
            if (hv_info->demand[i] > hv_info->demand_mirror[i])
               hv_info->demand_mirror[i] =
                   MIN(hv_info->demand[i], hv_info->demand_mirror[i] + delta);
            else
               hv_info->demand_mirror[i] =
                   MAX(hv_info->demand[i], hv_info->demand_mirror[i] - delta);
         }
         status = DRIVER(i) (CMD_SET, hv_info->dd_info[i],
                             i - hv_info->channel_offset[i], hv_info->demand_mirror[i]);
         hv_info->last_change[i] = ss_millitime();
      }
   }

   return status;
}


/*------------------------------------------------------------------*/

void hv_demand(INT hDB, INT hKey, void *info)
{
   INT i, status, n, offset;
   HV_INFO *hv_info;
   EQUIPMENT *pequipment;

   pequipment = (EQUIPMENT *) info;
   hv_info = (HV_INFO *) pequipment->cd_info;

   /* check for voltage limit */
   for (i = 0; i < hv_info->num_channels; i++)
      if (hv_info->demand[i] > hv_info->voltage_limit[i])
         hv_info->demand[i] = hv_info->voltage_limit[i];

   /* check how many channels differ */
   for (i = 1, n = 0; i < hv_info->num_channels; i++)
      if ((fabs(hv_info->demand[i]) > fabs(hv_info->demand_mirror[i])
           && hv_info->rampup_speed[i] == 0)
          || (fabs(hv_info->demand[i]) < fabs(hv_info->demand_mirror[i])
              && hv_info->rampdown_speed[i] == 0))
         n++;

   /* if more than 10% differ, use SET_ALL command which is faster in that case */
   if (n > hv_info->num_channels / 10) {
      for (i = 0, offset = 0; pequipment->driver[i].name[0]; i++) {
         if ((pequipment->driver[i].flags & DF_READ_ONLY) == 0)
            status = pequipment->driver[i].dd(CMD_SET_ALL, pequipment->driver[i].dd_info,
                                              pequipment->driver[i].channels,
                                              hv_info->demand + offset);

         offset += pequipment->driver[i].channels;
      }

      for (i = 0; i < hv_info->num_channels; i++) {
         if (hv_info->demand[i] != hv_info->demand_mirror[i])
            hv_info->last_change[i] = ss_millitime();
         hv_info->demand_mirror[i] = hv_info->demand[i];
      }

      /* read back all channels */
      hv_read(pequipment, -1);
   } else
      /* set individual channels only if demand value differs */
      for (i = 0; i < hv_info->num_channels; i++)
         if (hv_info->demand[i] != hv_info->demand_mirror[i] ||
             fabs(hv_info->measured[i] - hv_info->demand[i]) > 100.f) {
            hv_info->last_change[i] = ss_millitime();
         }

   pequipment->odb_in++;

   /* execute one ramping */
   hv_ramp(hv_info);
}

/*------------------------------------------------------------------*/

void hv_current_limit(INT hDB, INT hKey, void *info)
{
   INT i;
   HV_INFO *hv_info;
   EQUIPMENT *pequipment;

   pequipment = (EQUIPMENT *) info;
   hv_info = (HV_INFO *) pequipment->cd_info;

   hv_info = ((EQUIPMENT *) info)->cd_info;

   /* check for voltage limit */
   for (i = 0; i < hv_info->num_channels; i++)
      DRIVER(i) (CMD_SET_CURRENT_LIMIT, hv_info->dd_info[i],
                 i - hv_info->channel_offset[i], hv_info->current_limit[i]);

   pequipment->odb_in++;
}

/*------------------------------------------------------------------*/

void hv_update_label(INT hDB, INT hKey, void *info)
{
   INT i, status;
   HV_INFO *hv_info;
   EQUIPMENT *pequipment;

   pequipment = (EQUIPMENT *) info;
   hv_info = (HV_INFO *) pequipment->cd_info;

   /* update channel labels based on the midas channel names */
   for (i = 0; i < hv_info->num_channels; i++)
      status = DRIVER(i) (CMD_SET_LABEL, hv_info->dd_info[i],
                          i - hv_info->channel_offset[i],
                          hv_info->names + NAME_LENGTH * i);
}

/*------------------------------------------------------------------*/

INT hv_init(EQUIPMENT * pequipment)
{
   int status, size, i, j, index, offset;
   char str[256];
   HNDLE hDB, hKey, hNames;
   HV_INFO *hv_info;

   /* allocate private data */
   pequipment->cd_info = calloc(1, sizeof(HV_INFO));
   hv_info = (HV_INFO *) pequipment->cd_info;

   /* get class driver root key */
   cm_get_experiment_database(&hDB, NULL);
   sprintf(str, "/Equipment/%s", pequipment->name);
   db_create_key(hDB, 0, str, TID_KEY);
   db_find_key(hDB, 0, str, &hv_info->hKeyRoot);

   /* save event format */
   size = sizeof(str);
   db_get_value(hDB, hv_info->hKeyRoot, "Common/Format", str, &size, TID_STRING, TRUE);

   if (equal_ustring(str, "Fixed"))
      hv_info->format = FORMAT_FIXED;
   else if (equal_ustring(str, "MIDAS"))
      hv_info->format = FORMAT_MIDAS;
   else if (equal_ustring(str, "YBOS"))
      hv_info->format = FORMAT_YBOS;

   /* count total number of channels */
   for (i = 0, hv_info->num_channels = 0; pequipment->driver[i].name[0]; i++) {
      if (pequipment->driver[i].channels == 0) {
         cm_msg(MERROR, "hv_init", "Driver with zero channels not allowed");
         return FE_ERR_ODB;
      }

      hv_info->num_channels += pequipment->driver[i].channels;
   }

   if (hv_info->num_channels == 0) {
      cm_msg(MERROR, "hv_init", "No channels found in device driver list");
      return FE_ERR_ODB;
   }

   /* Allocate memory for buffers */
   hv_info->names = (char *) calloc(hv_info->num_channels, NAME_LENGTH);

   hv_info->demand = (float *) calloc(hv_info->num_channels, sizeof(float));
   hv_info->measured = (float *) calloc(hv_info->num_channels, sizeof(float));
   hv_info->current = (float *) calloc(hv_info->num_channels, sizeof(float));

   hv_info->update_threshold = (float *) calloc(hv_info->num_channels, sizeof(float));
   hv_info->update_threshold_current =
       (float *) calloc(hv_info->num_channels, sizeof(float));
   hv_info->voltage_limit = (float *) calloc(hv_info->num_channels, sizeof(float));
   hv_info->current_limit = (float *) calloc(hv_info->num_channels, sizeof(float));
   hv_info->rampup_speed = (float *) calloc(hv_info->num_channels, sizeof(float));
   hv_info->rampdown_speed = (float *) calloc(hv_info->num_channels, sizeof(float));

   hv_info->demand_mirror = (float *) calloc(hv_info->num_channels, sizeof(float));
   hv_info->measured_mirror = (float *) calloc(hv_info->num_channels, sizeof(float));
   hv_info->current_mirror = (float *) calloc(hv_info->num_channels, sizeof(float));
   hv_info->last_change = (DWORD *) calloc(hv_info->num_channels, sizeof(DWORD));

   hv_info->dd_info = (void *) calloc(hv_info->num_channels, sizeof(void *));
   hv_info->channel_offset = (INT *) calloc(hv_info->num_channels, sizeof(INT));
   hv_info->driver = (void *) calloc(hv_info->num_channels, sizeof(void *));
   hv_info->flags = (DWORD *) calloc(hv_info->num_channels, sizeof(void *));

   if (!hv_info->flags) {
      cm_msg(MERROR, "hv_init", "Not enough memory");
      return FE_ERR_ODB;
   }

  /*---- Create/Read settings ----*/

   /* Names */
   for (i = 0; i < hv_info->num_channels; i++)
      sprintf(hv_info->names + NAME_LENGTH * i, "Default%%CH %d", i);
   db_merge_data(hDB, hv_info->hKeyRoot, "Settings/Names",
                 hv_info->names, NAME_LENGTH * hv_info->num_channels,
                 hv_info->num_channels, TID_STRING);

   /* Update threshold */
   for (i = 0; i < hv_info->num_channels; i++)
      hv_info->update_threshold[i] = 2.f;       /* default 2V */
   db_merge_data(hDB, hv_info->hKeyRoot, "Settings/Update Threshold Measured",
                 hv_info->update_threshold, sizeof(float) * hv_info->num_channels,
                 hv_info->num_channels, TID_FLOAT);

   /* Update threshold current */
   for (i = 0; i < hv_info->num_channels; i++)
      hv_info->update_threshold_current[i] = 2.f;       /* default 2uA */
   db_merge_data(hDB, hv_info->hKeyRoot, "Settings/Update Threshold Current",
                 hv_info->update_threshold_current, sizeof(float) * hv_info->num_channels,
                 hv_info->num_channels, TID_FLOAT);

   /* Voltage limit */
   for (i = 0; i < hv_info->num_channels; i++)
      hv_info->voltage_limit[i] = 3000.f;       /* default 3000V */
   db_merge_data(hDB, hv_info->hKeyRoot, "Settings/Voltage Limit",
                 hv_info->voltage_limit, sizeof(float) * hv_info->num_channels,
                 hv_info->num_channels, TID_FLOAT);
   db_find_key(hDB, hv_info->hKeyRoot, "Settings/Voltage Limit", &hKey);
   db_open_record(hDB, hKey, hv_info->voltage_limit,
                  sizeof(float) * hv_info->num_channels, MODE_READ, NULL, NULL);

   /* Current limit */
   for (i = 0; i < hv_info->num_channels; i++)
      hv_info->current_limit[i] = 3000.f;       /* default 3000V */
   db_merge_data(hDB, hv_info->hKeyRoot, "Settings/Current Limit",
                 hv_info->current_limit, sizeof(float) * hv_info->num_channels,
                 hv_info->num_channels, TID_FLOAT);
   db_find_key(hDB, hv_info->hKeyRoot, "Settings/Current Limit", &hKey);
   db_open_record(hDB, hKey, hv_info->current_limit,
                  sizeof(float) * hv_info->num_channels, MODE_READ, hv_current_limit,
                  pequipment);

   /* Ramp speed */
   for (i = 0; i < hv_info->num_channels; i++) {
      /* disabled by default */
      hv_info->rampup_speed[i] = 0.f;
      hv_info->rampdown_speed[i] = 0.f;
   }

   db_merge_data(hDB, hv_info->hKeyRoot, "Settings/Ramp Up Speed",
                 hv_info->rampup_speed, sizeof(float) * hv_info->num_channels,
                 hv_info->num_channels, TID_FLOAT);
   db_find_key(hDB, hv_info->hKeyRoot, "Settings/Ramp Up Speed", &hKey);
   db_open_record(hDB, hKey, hv_info->rampup_speed, sizeof(float) * hv_info->num_channels,
                  MODE_READ, NULL, NULL);

   db_merge_data(hDB, hv_info->hKeyRoot, "Settings/Ramp Down Speed",
                 hv_info->rampdown_speed, sizeof(float) * hv_info->num_channels,
                 hv_info->num_channels, TID_FLOAT);
   db_find_key(hDB, hv_info->hKeyRoot, "Settings/Ramp Down Speed", &hKey);
   db_open_record(hDB, hKey, hv_info->rampdown_speed,
                  sizeof(float) * hv_info->num_channels, MODE_READ, NULL, NULL);

  /*---- Create/Read variables ----*/

   /* Demand */
   db_merge_data(hDB, hv_info->hKeyRoot, "Variables/Demand",
                 hv_info->demand, sizeof(float) * hv_info->num_channels,
                 hv_info->num_channels, TID_FLOAT);
   db_find_key(hDB, hv_info->hKeyRoot, "Variables/Demand", &hv_info->hKeyDemand);
   db_open_record(hDB, hv_info->hKeyDemand, hv_info->demand,
                  hv_info->num_channels * sizeof(float), MODE_READ, hv_demand,
                  pequipment);

   /* Measured */
   db_merge_data(hDB, hv_info->hKeyRoot, "Variables/Measured",
                 hv_info->measured, sizeof(float) * hv_info->num_channels,
                 hv_info->num_channels, TID_FLOAT);
   db_find_key(hDB, hv_info->hKeyRoot, "Variables/Measured", &hv_info->hKeyMeasured);
   memcpy(hv_info->measured_mirror, hv_info->measured,
          hv_info->num_channels * sizeof(float));

   /* Current */
   db_merge_data(hDB, hv_info->hKeyRoot, "Variables/Current",
                 hv_info->current, sizeof(float) * hv_info->num_channels,
                 hv_info->num_channels, TID_FLOAT);
   db_find_key(hDB, hv_info->hKeyRoot, "Variables/Current", &hv_info->hKeyCurrent);
   memcpy(hv_info->current_mirror, hv_info->current,
          hv_info->num_channels * sizeof(float));

  /*---- Initialize device drivers ----*/

   /* call init method */
   for (i = 0; pequipment->driver[i].name[0]; i++) {
      sprintf(str, "Settings/Devices/%s", pequipment->driver[i].name);
      status = db_find_key(hDB, hv_info->hKeyRoot, str, &hKey);
      if (status != DB_SUCCESS) {
         db_create_key(hDB, hv_info->hKeyRoot, str, TID_KEY);
         status = db_find_key(hDB, hv_info->hKeyRoot, str, &hKey);
         if (status != DB_SUCCESS) {
            cm_msg(MERROR, "hv_init", "Cannot create %s entry in online database", str);
            free_mem(hv_info);
            return FE_ERR_ODB;
         }
      }

      status = pequipment->driver[i].dd(CMD_INIT, hKey, &pequipment->driver[i].dd_info,
                                        pequipment->driver[i].channels,
                                        pequipment->driver[i].flags,
                                        pequipment->driver[i].bd);
      if (status != FE_SUCCESS) {
         free_mem(hv_info);
         return status;
      }
   }

   /* compose device driver channel assignment */
   for (i = 0, j = 0, index = 0, offset = 0; i < hv_info->num_channels; i++, j++) {
      while (j >= pequipment->driver[index].channels && pequipment->driver[index].name[0]) {
         offset += j;
         index++;
         j = 0;
      }

      hv_info->driver[i] = pequipment->driver[index].dd;
      hv_info->dd_info[i] = pequipment->driver[index].dd_info;
      hv_info->channel_offset[i] = offset;
      hv_info->flags[i] = pequipment->driver[index].flags;
   }

  /*---- get default names from device driver ----*/
   size = NAME_LENGTH * sizeof(char);

   db_find_key(hDB, hv_info->hKeyRoot, "Settings/Names", &hKey);
   for (i = 0; i < hv_info->num_channels; i++) {
      DRIVER(i) (CMD_GET_DEFAULT_NAME, hv_info->dd_info[i],
                 i - hv_info->channel_offset[i], hv_info->names + NAME_LENGTH * i);
      db_set_data_index(hDB, hKey, hv_info->names + NAME_LENGTH * i, size, i, TID_STRING);
   }

  /*---- set labels form midas SC names ----*/
   for (i = 0; i < hv_info->num_channels; i++) {
      DRIVER(i) (CMD_SET_LABEL, hv_info->dd_info[i],
                 i - hv_info->channel_offset[i], hv_info->names + NAME_LENGTH * i);
   }

   /* open hotlink on channel names */
   if (db_find_key(hDB, hv_info->hKeyRoot, "Settings/Names", &hNames) == DB_SUCCESS)
      db_open_record(hDB, hNames, hv_info->names, NAME_LENGTH * hv_info->num_channels,
                     MODE_READ, hv_update_label, pequipment);

   /* set current limits and initial demand values */
   printf("\n");
   for (i = 0, offset = 0; pequipment->driver[i].name[0]; i++) {
      for (j = 0; j < pequipment->driver[i].channels; j++)
         hv_info->demand_mirror[offset + j] =
             MIN(hv_info->demand[offset + j], hv_info->voltage_limit[offset + j]);

      DRIVER(i + offset) (CMD_SET_CURRENT_LIMIT_ALL, pequipment->driver[i].dd_info,
                          pequipment->driver[i].channels,
                          hv_info->current_limit + offset);

      printf("\n");

      if ((hv_info->flags[i] & DF_PRIO_DEVICE) == 0)
         status = DRIVER(i + offset) (CMD_SET_ALL, pequipment->driver[i].dd_info,
                                      pequipment->driver[i].channels,
                                      hv_info->demand_mirror + offset);
      if (status != FE_SUCCESS)
         return status;

      offset += pequipment->driver[i].channels;
   }
   printf("\n");

   /* get demand valus from device if asked for */
   for (i = 0; i < hv_info->num_channels; i++) {
      if (hv_info->flags[i] & DF_PRIO_DEVICE) {
         DRIVER(i) (CMD_GET_DEMAND, hv_info->dd_info[i],
                    i - hv_info->channel_offset[i], &hv_info->demand[i]);
         hv_info->demand_mirror[i] = hv_info->demand[i];
      }
   }
   db_set_record(hDB, hv_info->hKeyDemand, hv_info->demand,
                 hv_info->num_channels * sizeof(float), 0);

   /* initially read all channels if not too much */
   if (hv_info->num_channels < 256)
      hv_read(pequipment, -1);

   return FE_SUCCESS;
}

/*------------------------------------------------------------------*/

INT hv_exit(EQUIPMENT * pequipment)
{
   INT i;

   free_mem((HV_INFO *) pequipment->cd_info);

   /* call exit method of device drivers */
   for (i = 0; pequipment->driver[i].dd != NULL; i++)
      pequipment->driver[i].dd(CMD_EXIT, pequipment->driver[i].dd_info);

   return FE_SUCCESS;
}

/*------------------------------------------------------------------*/

INT hv_idle(EQUIPMENT * pequipment)
{
   INT act, status;
   DWORD act_time;
   HV_INFO *hv_info;

   hv_info = (HV_INFO *) pequipment->cd_info;

   /* do ramping */
   hv_ramp(hv_info);

   /* select next measurement channel */
   act_time = ss_millitime();
   act = (hv_info->last_channel + 1) % hv_info->num_channels;

   /* look for the next channel recently updated */
   while (!(act_time - hv_info->last_change[act] < 5000 ||
            (act_time - hv_info->last_change[act] < 20000
             && abs(hv_info->measured[act] - hv_info->demand[act]) >
             2 * hv_info->update_threshold[act]))) {
      act = (act + 1) % hv_info->num_channels;
      if (act == hv_info->last_channel) {
         act = (hv_info->last_channel + 1) % hv_info->num_channels;
         break;
      }
   }

   /* measure channel */
   status = hv_read(pequipment, act);
   hv_info->last_channel = act;

   return status;
}

/*------------------------------------------------------------------*/

INT cd_hv_read(char *pevent, int offset)
{
   float *pdata;
   DWORD *pdw;
   HV_INFO *hv_info;
   EQUIPMENT *pequipment;

   pequipment = *((EQUIPMENT **) pevent);
   hv_info = (HV_INFO *) pequipment->cd_info;

   if (hv_info->format == FORMAT_FIXED) {
      memcpy(pevent, hv_info->demand, sizeof(float) * hv_info->num_channels);
      pevent += sizeof(float) * hv_info->num_channels;

      memcpy(pevent, hv_info->measured, sizeof(float) * hv_info->num_channels);
      pevent += sizeof(float) * hv_info->num_channels;

      memcpy(pevent, hv_info->current, sizeof(float) * hv_info->num_channels);
      pevent += sizeof(float) * hv_info->num_channels;

      return 3 * sizeof(float) * hv_info->num_channels;
   } else if (hv_info->format == FORMAT_MIDAS) {
      bk_init(pevent);

      /* create DMND bank */
      bk_create(pevent, "DMND", TID_FLOAT, &pdata);
      memcpy(pdata, hv_info->demand, sizeof(float) * hv_info->num_channels);
      pdata += hv_info->num_channels;
      bk_close(pevent, pdata);

      /* create MSRD bank */
      bk_create(pevent, "MSRD", TID_FLOAT, &pdata);
      memcpy(pdata, hv_info->measured, sizeof(float) * hv_info->num_channels);
      pdata += hv_info->num_channels;
      bk_close(pevent, pdata);

      /* create CRNT bank */
      bk_create(pevent, "CRNT", TID_FLOAT, &pdata);
      memcpy(pdata, hv_info->current, sizeof(float) * hv_info->num_channels);
      pdata += hv_info->num_channels;
      bk_close(pevent, pdata);

      return bk_size(pevent);
   } else if (hv_info->format == FORMAT_YBOS) {
      ybk_init((DWORD *) pevent);

      /* create EVID bank */
      ybk_create((DWORD *) pevent, "EVID", I4_BKTYPE, (DWORD *) (&pdw));
      *(pdw)++ = EVENT_ID(pevent);      /* Event_ID + Mask */
      *(pdw)++ = SERIAL_NUMBER(pevent); /* Serial number */
      *(pdw)++ = TIME_STAMP(pevent);    /* Time Stamp */
      ybk_close((DWORD *) pevent, pdw);

      /* create DMND bank */
      ybk_create((DWORD *) pevent, "DMND", F4_BKTYPE, (DWORD *) & pdata);
      memcpy(pdata, hv_info->demand, sizeof(float) * hv_info->num_channels);
      pdata += hv_info->num_channels;
      ybk_close((DWORD *) pevent, pdata);

      /* create MSRD bank */
      ybk_create((DWORD *) pevent, "MSRD", F4_BKTYPE, (DWORD *) & pdata);
      memcpy(pdata, hv_info->measured, sizeof(float) * hv_info->num_channels);
      pdata += hv_info->num_channels;
      ybk_close((DWORD *) pevent, pdata);

      /* create CRNT bank */
      ybk_create((DWORD *) pevent, "CRNT", F4_BKTYPE, (DWORD *) & pdata);
      memcpy(pdata, hv_info->current, sizeof(float) * hv_info->num_channels);
      pdata += hv_info->num_channels;
      ybk_close((DWORD *) pevent, pdata);

      return ybk_size((DWORD *) pevent);
   }

   return 0;
}

/*------------------------------------------------------------------*/

INT cd_hv(INT cmd, PEQUIPMENT pequipment)
{
   INT status;

   switch (cmd) {
   case CMD_INIT:
      status = hv_init(pequipment);
      break;

   case CMD_EXIT:
      status = hv_exit(pequipment);
      break;

   case CMD_IDLE:
      status = hv_idle(pequipment);
      break;

   default:
      cm_msg(MERROR, "HV class driver", "Received unknown command %d", cmd);
      status = FE_ERR_DRIVER;
      break;

   }

   return status;
}
