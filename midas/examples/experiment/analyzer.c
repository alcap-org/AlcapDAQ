/********************************************************************\

  Name:         analyzer.c
  Created by:   Stefan Ritt

  Contents:     System part of Analyzer code for sample experiment

  $Log: analyzer.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.17  2004/06/18 11:50:38  midas
  Changed ADC0 bank from structured to DWORD

  Revision 1.16  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.15  2003/04/28 15:33:05  midas
  Fixed compiler warnings

  Revision 1.14  2003/04/25 14:49:46  midas
  Removed HBOOK code

  Revision 1.13  2003/04/21 04:00:15  olchansk
  replace MANA_LITE with HAVE_HBOOK

  Revision 1.12  2003/04/14 13:30:52  midas
  Changed bank descriptions because of conflict in fal.c

  Revision 1.11  2003/04/07 23:54:43  olchansk
  add #include <string.h>

  Revision 1.10  2002/05/10 05:22:34  pierre
  add MANA_LITE #ifdef

  Revision 1.9  2002/05/09 02:50:28  midas
  Removed initialization of 'Edit on start' by analyzer

  Revision 1.8  2002/05/08 19:54:40  midas
  Added extra parameter to function db_get_value()

  Revision 1.7  2000/11/20 12:29:37  midas
  Added use_tests flag in analyzer request

  Revision 1.6  2000/09/12 12:36:15  midas
  Removed test messages

  Revision 1.5  2000/08/11 11:43:50  midas
  Added cm_msg1 to produce messages which go to a differnt logging file

  Revision 1.4  2000/03/02 22:00:18  midas
  Changed events sent to double

  Revision 1.3  1998/10/29 14:18:19  midas
  Used hDB consistently

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

/* standard includes */
#include <stdio.h>
#include <string.h>
#include <time.h>

/* midas includes */
#include "midas.h"
#include "experim.h"
#include "analyzer.h"

/* cernlib includes */
#ifdef OS_WINNT
#define VISUAL_CPLUSPLUS
#endif
#ifdef __linux__
#define f2cFortran
#endif

#ifdef HAVE_HBOOK
#include <cfortran.h>
#include <hbook.h>

PAWC_DEFINE(1000000);
#endif                          /* HAVE_HBOOK */

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

/*-- Module declarations -------------------------------------------*/

extern ANA_MODULE scaler_accum_module;
extern ANA_MODULE adc_calib_module;
extern ANA_MODULE adc_summing_module;

ANA_MODULE *scaler_module[] = {
   &scaler_accum_module,
   NULL
};

ANA_MODULE *trigger_module[] = {
   &adc_calib_module,
   &adc_summing_module,
   NULL
};

/*-- Bank definitions ----------------------------------------------*/

ASUM_BANK_STR(asum_bank_str);

BANK_LIST ana_trigger_bank_list[] = {

   /* online banks */
   {"ADC0", TID_WORD, N_ADC, NULL},
   {"TDC0", TID_WORD, N_TDC, NULL},

   /* calculated banks */
   {"CADC", TID_FLOAT, N_ADC, NULL},
   {"ASUM", TID_STRUCT, sizeof(ASUM_BANK), asum_bank_str},

   {""},
};

BANK_LIST ana_scaler_bank_list[] = {
   /* online banks */
   {"SCLR", TID_DWORD, N_ADC, NULL},

   /* calculated banks */
   {"ACUM", TID_DOUBLE, N_ADC, NULL},
   {""},
};

/*-- Event request list --------------------------------------------*/

ANALYZE_REQUEST analyze_request[] = {
   {"Trigger",                  /* equipment name */
    {1,                         /* event ID */
     TRIGGER_ALL,               /* trigger mask */
     GET_SOME,                  /* get some events */
     "SYSTEM",                  /* event buffer */
     TRUE,                      /* enabled */
     "", "",}
    ,
    NULL,                       /* analyzer routine */
    trigger_module,             /* module list */
    ana_trigger_bank_list,      /* bank list */
    1000,                       /* RWNT buffer size */
    TRUE,                       /* Use tests for this event */
    }
   ,

   {"Scaler",                   /* equipment name */
    {2,                         /* event ID */
     TRIGGER_ALL,               /* trigger mask */
     GET_ALL,                   /* get all events */
     "SYSTEM",                  /* event buffer */
     TRUE,                      /* enabled */
     "", "",}
    ,
    NULL,                       /* analyzer routine */
    scaler_module,              /* module list */
    ana_scaler_bank_list,       /* bank list */
    100,                        /* RWNT buffer size */
    }
   ,

   {""}
   ,
};

/*-- Analyzer Init -------------------------------------------------*/

INT analyzer_init()
{
   HNDLE hDB, hKey;
   char str[80];

   RUNINFO_STR(runinfo_str);
   EXP_PARAM_STR(exp_param_str);
   GLOBAL_PARAM_STR(global_param_str);
   TRIGGER_SETTINGS_STR(trigger_settings_str);

   /* open ODB structures */
   cm_get_experiment_database(&hDB, NULL);
   db_create_record(hDB, 0, "/Runinfo", strcomb(runinfo_str));
   db_find_key(hDB, 0, "/Runinfo", &hKey);
   if (db_open_record(hDB, hKey, &runinfo, sizeof(runinfo), MODE_READ, NULL, NULL) !=
       DB_SUCCESS) {
      cm_msg(MERROR, "analyzer_init", "Cannot open \"/Runinfo\" tree in ODB");
      return 0;
   }

   db_create_record(hDB, 0, "/Experiment/Run Parameters", strcomb(exp_param_str));
   db_find_key(hDB, 0, "/Experiment/Run Parameters", &hKey);
   if (db_open_record(hDB, hKey, &exp_param, sizeof(exp_param), MODE_READ, NULL, NULL) !=
       DB_SUCCESS) {
      cm_msg(MERROR, "analyzer_init",
             "Cannot open \"/Experiment/Run Parameters\" tree in ODB");
      return 0;
   }

   sprintf(str, "/%s/Parameters/Global", analyzer_name);
   db_create_record(hDB, 0, str, strcomb(global_param_str));
   db_find_key(hDB, 0, str, &hKey);
   if (db_open_record
       (hDB, hKey, &global_param, sizeof(global_param), MODE_READ, NULL,
        NULL) != DB_SUCCESS) {
      cm_msg(MERROR, "analyzer_init", "Cannot open \"%s\" tree in ODB", str);
      return 0;
   }

   db_create_record(hDB, 0, "/Equipment/Trigger/Settings", strcomb(trigger_settings_str));
   db_find_key(hDB, 0, "/Equipment/Trigger/Settings", &hKey);

   if (db_open_record
       (hDB, hKey, &trigger_settings, sizeof(trigger_settings), MODE_READ, NULL,
        NULL) != DB_SUCCESS) {
      cm_msg(MERROR, "analyzer_init",
             "Cannot open \"/Equipment/Trigger/Settings\" tree in ODB");
      return 0;
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
   double n;
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
      db_get_value(hDB, 0, "/Equipment/Trigger/Statistics/Events sent", &n, &size,
                   TID_DOUBLE, TRUE);

      fprintf(f, "%5.1lfk\t", n / 1000);
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

/*------------------------------------------------------------------*/
