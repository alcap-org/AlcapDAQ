/********************************************************************\

  Name:         analyzer.c
  Created by:   Stefan Ritt

  Contents:     Single file analyzer code for sample experiment

  $Log: analyzer.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.2  2002/05/08 20:51:41  midas
  Added extra parameter to function db_get_value()

  Revision 1.1  1999/02/22 12:24:56  midas
  Initial revision


\********************************************************************/

/* standard includes */
#include <stdio.h>
#include <time.h>

/* midas includes */
#include "midas.h"
#include "experim.h"

/* cernlib includes */
#ifdef OS_WINNT
#define VISUAL_CPLUSPLUS
#endif
#ifdef __linux__
#define f2cFortran
#endif
#include <cfortran.h>
#include <hbook.h>

PAWC_DEFINE(2000000);

/*-- Globals -------------------------------------------------------*/

/* The analyzer name (client name) as seen by other MIDAS clients   */
char *analyzer_name = "Analyzer";

/* analyzer_loop is called with this interval in ms (0 to disable)  */
INT analyzer_loop_period = 0;

/* default ODB size */
INT odb_size = DEFAULT_ODB_SIZE;

/* ODB structures */
RUNINFO runinfo;
GLOBAL_PARAM global_param;
EXP_PARAM exp_param;
TRIGGER_SETTINGS trigger_settings;

/* number of channels */
#define N_ADC          8
#define N_TDC          8
#define N_SCLR         8

#define ADC_N_BINS   600
#define ADC_X_LOW      0
#define ADC_X_HIGH  1200

#define TDC_N_BINS   500
#define TDC_X_LOW      0
#define TDC_X_HIGH  2500

#define ADC_ID      1000
#define TDC_ID      1100

int analyze_trigger_event(EVENT_HEADER * pheader, void *pevent);
int analyze_scaler_event(EVENT_HEADER * pheader, void *pevent);

/*-- Bank list -----------------------------------------------------*/

BANK_LIST trigger_bank_list[] = {
   {"ADC0", TID_WORD, N_ADC, NULL},
   {"TDC0", TID_WORD, N_TDC, NULL},
   {""},
};

BANK_LIST scaler_bank_list[] = {
   {"SCLR", TID_DWORD, N_SCLR, NULL},
   {""},
};

/*-- Event request list --------------------------------------------*/

ANALYZE_REQUEST analyze_request[] = {
   {"Trigger",                  /* equipment name */
    1,                          /* event ID */
    TRIGGER_ALL,                /* trigger mask */
    GET_SOME,                   /* get some events */
    "SYSTEM",                   /* event buffer */
    TRUE,                       /* enabled */
    "", "",
    analyze_trigger_event,      /* analyzer routine */
    NULL,                       /* module list */
    trigger_bank_list,          /* list */
    10000,                      /* RWNT buffer size */
    },

   {"Scaler",                   /* equipment name */
    2,                          /* event ID */
    TRIGGER_ALL,                /* trigger mask */
    GET_ALL,                    /* get all events */
    "SYSTEM",                   /* event buffer */
    TRUE,                       /* enabled */
    "", "",
    analyze_scaler_event,       /* analyzer routine */
    NULL,                       /* module list */
    scaler_bank_list,           /* bank list */
    0,                          /* RWNT buffer size */
    },

   {""}
};

/*-- Analyzer Init -------------------------------------------------*/

INT analyzer_init()
{
   HNDLE hDB, hKey;
   char str[80];
   int i;

   RUNINFO_STR(runinfo_str);
   EXP_PARAM_STR(exp_param_str);
   EXP_EDIT_STR(exp_edit_str);
   GLOBAL_PARAM_STR(global_param_str);
   TRIGGER_SETTINGS_STR(trigger_settings_str);

   /* following code opens ODB structures to make them accessible
      from the analyzer code as C structures */

   cm_get_experiment_database(&hDB, NULL);
   db_create_record(hDB, 0, "/Runinfo", strcomb(runinfo_str));
   db_find_key(hDB, 0, "/Runinfo", &hKey);
   if (db_open_record(hDB, hKey, &runinfo, sizeof(runinfo), MODE_READ,
                      NULL, NULL) != DB_SUCCESS) {
      cm_msg(MERROR, "analyzer_init", "Cannot open \"/Runinfo\" tree in ODB");
      return 0;
   }

   db_create_record(hDB, 0, "/Experiment/Run Parameters", strcomb(exp_param_str));

   db_find_key(hDB, 0, "/Experiment/Run Parameters", &hKey);
   if (db_open_record(hDB, hKey, &exp_param, sizeof(exp_param), MODE_READ,
                      NULL, NULL) != DB_SUCCESS) {
      cm_msg(MERROR, "analyzer_init",
             "Cannot open \"/Experiment/Run Parameters\" tree in ODB");
      return 0;
   }

   db_create_record(hDB, 0, "/Experiment/Edit on start", strcomb(exp_edit_str));

   sprintf(str, "/%s/Parameters/Global", analyzer_name);
   db_create_record(hDB, 0, str, strcomb(global_param_str));
   db_find_key(hDB, 0, str, &hKey);
   if (db_open_record(hDB, hKey, &global_param, sizeof(global_param),
                      MODE_READ, NULL, NULL) != DB_SUCCESS) {
      cm_msg(MERROR, "analyzer_init", "Cannot open \"%s\" tree in ODB", str);
      return 0;
   }

   db_create_record(hDB, 0, "/Equipment/Trigger/Settings", strcomb(trigger_settings_str));
   db_find_key(hDB, 0, "/Equipment/Trigger/Settings", &hKey);

   if (db_open_record(hDB, hKey, &trigger_settings, sizeof(trigger_settings),
                      MODE_READ, NULL, NULL) != DB_SUCCESS) {
      cm_msg(MERROR, "analyzer_init",
             "Cannot open \"/Equipment/Trigger/Settings\" tree in ODB");
      return 0;
   }

  /*---- user code to book histos ----------------------------------*/

   /* book ADC histos */
   for (i = 0; i < N_ADC; i++) {
      sprintf(str, "ADC%02d", i);
      HBOOK1(ADC_ID + i, str, ADC_N_BINS, (float) ADC_X_LOW, (float) ADC_X_HIGH, 0.f);
   }

   /* book TDC histos */
   for (i = 0; i < N_TDC; i++) {
      sprintf(str, "TDC%02d", i);
      HBOOK1(TDC_ID + i, str, TDC_N_BINS, (float) TDC_X_LOW, (float) TDC_X_HIGH, 0.f);
   }

   return SUCCESS;
}

/*-- Analyzer Exit -------------------------------------------------*/

INT analyzer_exit()
{
   return CM_SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT ana_begin_of_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT ana_end_of_run(INT run_number, char *error)
{
   FILE *f;
   time_t now;
   char str[256];
   int size;
   DWORD n;
   HNDLE hDB;
   BOOL flag;

   cm_get_experiment_database(&hDB, NULL);

   /* update run log if run was written and running online */

   size = sizeof(flag);
   db_get_value(hDB, 0, "/Logger/Write data", &flag, &size, TID_BOOL, TRUE);
   if (flag && runinfo.online_mode == 1) {
      /* update run log */
      size = sizeof(str);
      str[0] = 0;
      db_get_value(hDB, 0, "/Logger/Data Dir", str, &size, TID_STRING, TRUE);
      if (str[0] != 0)
         if (str[strlen(str) - 1] != DIR_SEPARATOR)
            strcat(str, DIR_SEPARATOR_STR);
      strcat(str, "runlog.txt");

      f = fopen(str, "a");

      time(&now);
      strcpy(str, ctime(&now));
      str[10] = 0;

      fprintf(f, "%s\t%3d\t", str, runinfo.run_number);

      strcpy(str, runinfo.start_time);
      str[19] = 0;
      fprintf(f, "%s\t", str + 11);

      strcpy(str, ctime(&now));
      str[19] = 0;
      fprintf(f, "%s\t", str + 11);

      size = sizeof(n);
      db_get_value(hDB, 0, "/Equipment/Trigger/Statistics/Events sent",
                   &n, &size, TID_DWORD, TRUE);

      fprintf(f, "%dk\t", (int) (n / 1000.0 + 0.5));
      fprintf(f, "%s\n", exp_param.comment);

      fclose(f);
   }

   return CM_SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT ana_pause_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- Resume Run ----------------------------------------------------*/

INT ana_resume_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- Analyzer Loop -------------------------------------------------*/

INT analyzer_loop()
{
   return CM_SUCCESS;
}

/*-- Analyze Events ------------------------------------------------*/

INT analyze_trigger_event(EVENT_HEADER * pheader, void *pevent)
{
   int i;
   WORD *padc, *ptdc;

   /* look for ADC0 and TDC0 bank, return if not present */
   if (bk_locate(pevent, "ADC0", &padc) == 0 || bk_locate(pevent, "TDC0", &ptdc) == 0)
      return 0;

   /* fill ADC and TDC histos */
   for (i = 0; i < N_ADC; i++)
      HF1(ADC_ID + i, padc[i], 1.f);
   for (i = 0; i < N_TDC; i++)
      HF1(ADC_ID + i, ptdc[i], 1.f);

   return CM_SUCCESS;
}

INT analyze_scaler_event(EVENT_HEADER * pheader, void *pevent)
{
   return CM_SUCCESS;
}
