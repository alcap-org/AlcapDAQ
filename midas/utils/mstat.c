/********************************************************************\
  
  Name:         mstat.c
  Created by:   Pierre-Andre Amaudruz
  
  Contents:     Display/log some pertinent information of the ODB
  
  $Log: mstat.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:38  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.17  2004/01/08 08:40:11  midas
  Implemented standard indentation

  Revision 1.16  2003/05/09 07:40:05  midas
  Added extra parameter to cm_get_environment

  Revision 1.15  2003/04/25 04:14:52  pierre
  Fixed compiler warning

  Revision 1.14  2003/04/23 23:53:54  pierre
  Fixed compiler warning

  Revision 1.13  2002/05/08 19:54:41  midas
  Added extra parameter to function db_get_value()

  Revision 1.12  2001/02/16 19:08:52  pierre
  - fix FAL test

  Revision 1.11  2001/02/16 18:51:30  pierre
  - Added logger status section for FAL

  Revision 1.10  2000/02/28 17:57:06  pierre
  - Change statistics to TID_DOUBLE for Super event implementation

  Revision 1.9  2000/02/25 18:51:51  pierre
  - include deferred transition messages

  Revision 1.8  1999/10/07 13:17:36  midas
  Put a few EXPRT im msystem.h to make NT happy, updated NT makefile

  Revision 1.7  1999/09/24 00:04:12  pierre
  - Modified Lazy status for multiple lazy channel

  Revision 1.6  1999/06/23 09:50:06  midas
  Changed lazy label from "last" to "<empty>"

  Revision 1.5  1999/02/05 23:47:55  pierre
  - Fix lazylogger client display

  Revision 1.4  1999/01/18 17:20:47  pierre
  - Added lazylogger status line
  - correct main for new keyboard handling

  Revision 1.3  1998/10/23 14:21:52  midas
  - Modified version scheme from 1.06 to 1.6.0
  - cm_get_version() now returns versino as string

  Revision 1.2  1998/10/12 12:19:04  midas
  Added Log tag in header


  Previous revision history
  ------------------------------------------------------------------
  date        by    modification
  --------    ---   ------------------------------------------------
  30-JAN-97   PAA   created
  20-Oct-97   PAA   included malarm functionality

\********************************************************************/

#include <fcntl.h>
#include "midas.h"
#include "msystem.h"

#define MAX_LINE         64
#define LINE_LENGTH      85
#define ALINE_LENGTH    256

#if defined( OS_WINNT )
#define ESC_FLAG 0
#else
#define ESC_FLAG 1
#endif

INT rn;

/* Global variables 
 *  (required since no way to pass these inside callbacks) */
INT numprocs;
DWORD checktime, lastchecktime, delta_time;
INT nlocal;
HNDLE hKeynlocal, hKeychktime;
BOOL active_flag, esc_flag;
INT loop, cur_max_line;
char ststr[MAX_LINE][LINE_LENGTH];

/*------------------------------------------------------------------*/
INT open_log_midstat(INT file_mode, INT runn, char *svpath)
{
   char srun[32];
   INT fHandle;

   if (file_mode == 1) {        /* append run number */
      strcat(svpath, ".");
      sprintf(srun, "Run%4.4i", runn);
      strncat(svpath, srun, strlen(srun));
      printf("output with run file:%s-\n", svpath);
   }
   /* open device */
#ifdef OS_UNIX
   if ((fHandle = open(svpath, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
#endif
#ifdef OS_OSF1
      if ((fHandle = open(svpath, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
#endif
#ifdef OS_VXWORKS
         if ((fHandle = open(svpath, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
#endif
#ifdef OS_WINNT
            if ((fHandle =
                 _open(svpath, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IWRITE)) == -1)
#endif
            {
               printf("File %s cannot be created\n", svpath);
            }
   return (fHandle);
}

/*------------------------------------------------------------------*/
void compose_status(HNDLE hDB, HNDLE hKey)
{
   BOOL atleastone_active;
   INT savej, j, ii, i, size;
   char str[256], path[256];
   KEY key;
   HNDLE hSubkey;
   char strtmp[256];
   time_t full_time;
   DWORD difftime;

   /* Clear string page */
   memset(ststr, ' ', sizeof(ststr));

   for (j = 0; j < MAX_LINE; j++)
      ststr[j][LINE_LENGTH - 1] = '\0';

/* --------------------- Run info -------------------------------- */
   {

      INT rs, rt;
      char cs[256], stt[256], spt[256], ex[256];
      DWORD tb, tsb;

      size = sizeof(rs);
      db_get_value(hDB, 0, "/Runinfo/State", &rs, &size, TID_INT, TRUE);
      if (rs == STATE_RUNNING)
         strcpy(cs, "Running");
      if (rs == STATE_PAUSED)
         strcpy(cs, "Paused ");
      if (rs == STATE_STOPPED)
         strcpy(cs, "Stopped");
      size = sizeof(rn);
      db_get_value(hDB, 0, "/Runinfo/run number", &rn, &size, TID_INT, TRUE);
      size = sizeof(stt);
      db_get_value(hDB, 0, "/Runinfo/start time", stt, &size, TID_STRING, TRUE);
      size = sizeof(spt);
      db_get_value(hDB, 0, "/Runinfo/stop time", spt, &size, TID_STRING, TRUE);
      size = sizeof(tb);
      db_get_value(hDB, 0, "/runinfo/Start Time binary", &tb, &size, TID_DWORD, TRUE);
      size = sizeof(tsb);
      db_get_value(hDB, 0, "/runinfo/Stop Time binary", &tsb, &size, TID_DWORD, TRUE);
      size = sizeof(rt);
      db_get_value(hDB, 0, "/Runinfo/Requested transition", &rt, &size, TID_INT, TRUE);

      /* Experiment info */
      size = sizeof(ex);
      db_get_value(hDB, 0, "/experiment/name", ex, &size, TID_STRING, TRUE);

      j = 0;
      time(&full_time);
      strcpy(str, ctime(&full_time));
      str[24] = 0;
      if (active_flag)
         sprintf(&(ststr[j++][0]),
                 "*-v%s- MIDAS status -- Alarm Checker active-------%s-*",
                 cm_get_version(), str);
      else
         sprintf(&(ststr[j++][0]),
                 "*-v%s- MIDAS status page -------------------------%s-*",
                 cm_get_version(), str);

      sprintf(&(ststr[j][0]), "Experiment:%s", ex);
      sprintf(&(ststr[j][23]), "Run#:%d", rn);

      /* PAA revisit the /Runinfo for run time display */
      /* state */
      if (rs == STATE_RUNNING) {
         if (rt == TR_STOP)
            sprintf(&(ststr[j][35]), "Deferred_Stop");
         else {
            if (esc_flag)
               sprintf(&(ststr[j][35]), "State:\033[1m%s\033[m", cs);
            else
               sprintf(&(ststr[j][36]), "State:%s", cs);
         }
      } else {
         if (rt == TR_START)
            sprintf(&(ststr[j][36]), "Deferred_Start");
         else
            sprintf(&(ststr[j][36]), "State:%s", cs);
      }

      /* time */
      if (rs != STATE_STOPPED) {
         cm_time((DWORD *) (&full_time));
         difftime = full_time - tb;
         if (esc_flag)
            sprintf(&(ststr[j++][66]), "Run time :%02ld:%02ld:%02ld",
                    difftime / 3600, difftime % 3600 / 60, difftime % 60);
         else
            sprintf(&(ststr[j++][54]), "     Run time :%02ld:%02ld:%02ld",
                    difftime / 3600, difftime % 3600 / 60, difftime % 60);
      } else if (rs == STATE_STOPPED) {
         if (tsb < tb)
            difftime = 0;
         else
            difftime = tsb - tb;
         if (esc_flag)
            sprintf(&(ststr[j++][54]), "Full Run time :%02ld:%02ld:%02ld",
                    difftime / 3600, difftime % 3600 / 60, difftime % 60);
         else
            sprintf(&(ststr[j++][54]), "Full Run time :%02ld:%02ld:%02ld",
                    difftime / 3600, difftime % 3600 / 60, difftime % 60);

         sprintf(&(ststr[j][42]), " Stop time:%s", spt);
      }

      /* Start Stop time */
      sprintf(&(ststr[j][0]), "Start time:%s", stt);
      ststr[++j][0] = '\0';
      ststr[++j][0] = '\0';
   }                            /* --- run info --- */

/* --------------------- Equipment tree -------------------------- */
   {
      double equevtsend;
      double equevtpsec;
      char equclient[256];
      char equnode[256];
      double equkbpsec;
      BOOL equenabled;

      size = sizeof(str);
      savej = j;
      atleastone_active = FALSE;
      /* check if dir exists */
      if (db_find_key(hDB, 0, "/equipment", &hKey) == DB_SUCCESS) {
         sprintf(&(ststr[j][0]), "FE Equip.");
         sprintf(&(ststr[j][12]), "Node");
         sprintf(&(ststr[j][30]), "Event Taken");
         sprintf(&(ststr[j][45]), "Event Rate[/s]");
         sprintf(&(ststr[j++][60]), "Data Rate[Kb/s]");
         for (i = 0;; i++) {
            db_enum_key(hDB, hKey, i, &hSubkey);
            if (!hSubkey)
               break;
            db_get_key(hDB, hSubkey, &key);
            if ((key.type == TID_KEY) &&
                ((strstr(key.name, "ODB")) == NULL) &&
                ((strstr(key.name, "BOR")) == NULL) &&
                ((strstr(key.name, "EOR")) == NULL)) {
               /* check if client running this equipment is present */
               /* extract client name from equipment */
               size = sizeof(equclient);
               sprintf(strtmp, "/equipment/%s/common/Frontend name", key.name);
               db_get_value(hDB, 0, strtmp, equclient, &size, TID_STRING, TRUE);
               /* search client name under /system/clients/xxx/name */
               if (cm_exist(equclient, TRUE) == CM_SUCCESS) {
                  atleastone_active = TRUE;
                  size = sizeof(equenabled);
                  sprintf(strtmp, "/equipment/%s/common/enabled", key.name);
                  db_get_value(hDB, 0, strtmp, &equenabled, &size, TID_BOOL, TRUE);

                  size = sizeof(equevtsend);
                  sprintf(strtmp, "/equipment/%s/statistics/events sent", key.name);
                  db_get_value(hDB, 0, strtmp, &equevtsend, &size, TID_DOUBLE, TRUE);

                  size = sizeof(equevtpsec);
                  sprintf(strtmp, "/equipment/%s/statistics/events per sec.", key.name);
                  db_get_value(hDB, 0, strtmp, &equevtpsec, &size, TID_DOUBLE, TRUE);

                  size = sizeof(equkbpsec);
                  sprintf(strtmp, "/equipment/%s/statistics/kBytes per sec.", key.name);
                  db_get_value(hDB, 0, strtmp, &equkbpsec, &size, TID_DOUBLE, TRUE);

                  size = sizeof(equnode);
                  sprintf(strtmp, "/equipment/%s/common/Frontend host", key.name);
                  db_get_value(hDB, 0, strtmp, equnode, &size, TID_STRING, TRUE);
                  {
                     char *pp, sdummy[64];
                     memset(sdummy, 0, 64);
                     sprintf(&(ststr[j][0]), "%s ", key.name);
                     pp = strchr(equnode, '.');
                     if (pp != NULL)
                        sprintf(&(ststr[j][12]), "%s",
                                strncpy(sdummy, equnode, pp - equnode));
                     else
                        sprintf(&(ststr[j][12]), "%s",
                                strncpy(sdummy, equnode, strlen(equnode)));

                     if (equevtsend > 1E9)
                        sprintf(&(ststr[j][30]), "%1.3lfG", equevtsend / 1E9);
                     else if (equevtsend > 1E6)
                        sprintf(&(ststr[j][30]), "%1.3lfM", equevtsend / 1E6);
                     else
                        sprintf(&(ststr[j][30]), "%1.0lf", equevtsend);

                     if (equenabled) {
                        if (esc_flag) {
                           sprintf(&(ststr[j][45]), "\033[7m%1.1lf\033[m", equevtpsec);
                           sprintf(&(ststr[j++][67]), "%1.1lf", equkbpsec);
                        } else {
                           sprintf(&(ststr[j][45]), "%1.1lf", equevtpsec);
                           sprintf(&(ststr[j++][60]), "%1.1lf", equkbpsec);
                        }
                     } else {
                        sprintf(&(ststr[j][45]), "%1.1lf", equevtpsec);
                        sprintf(&(ststr[j++][60]), "%1.1lf", equkbpsec);
                     }
                  }             /* get value */
               }                /* active */
            }                   /* eor==NULL */
         }                      /* for equipment */
      }
      /* Front-End message */
      if (!atleastone_active) {
         memset(&(ststr[savej][0]), 0, LINE_LENGTH);
         sprintf(&(ststr[savej][0]), "... No Front-End currently running...");
      }
   }                            /* --- Equipment tree --- */

/* --------------------- Logger tree ----------------------------- */
/* search client name "Logger" under /system/clients/xxx/name */
   if ((cm_exist("logger", FALSE) == CM_SUCCESS)
       || (cm_exist("fal", FALSE) == CM_SUCCESS)) {
      char datadir[256];
      char mesfil[256];
      BOOL wd, lactive;
      char lpath[256];
      char ltype[64];
      char lstate[64];
      double levt;
      double lbyt;

      /* logger */
      ststr[j++][0] = '\0';
      size = sizeof(datadir);
      db_get_value(hDB, 0, "/logger/data dir", datadir, &size, TID_STRING, TRUE);
      size = sizeof(mesfil);
      db_get_value(hDB, 0, "/logger/message file", mesfil, &size, TID_STRING, TRUE);
      size = sizeof(wd);
      db_get_value(hDB, 0, "/logger/write data", &wd, &size, TID_BOOL, TRUE);
      sprintf(&ststr[j][0], "Logger Data dir: %s", datadir);
      sprintf(&ststr[j++][45], "Message File: %s", mesfil);

      /* check if dir exists */
      if (db_find_key(hDB, 0, "/logger/channels", &hKey) == DB_SUCCESS) {
         sprintf(&(ststr[j][0]), "Chan.");
         sprintf(&(ststr[j][8]), "Active");
         sprintf(&(ststr[j][15]), "Type");
         sprintf(&(ststr[j][25]), "Filename");
         sprintf(&(ststr[j][45]), "Events Taken");
         sprintf(&(ststr[j++][60]), "KBytes Taken");
         for (i = 0;; i++) {
            db_enum_key(hDB, hKey, i, &hSubkey);
            if (!hSubkey)
               break;
            db_get_key(hDB, hSubkey, &key);
            if (key.type == TID_KEY) {
               size = sizeof(lactive);
               sprintf(strtmp, "/logger/channels/%s/settings/active", key.name);
               db_get_value(hDB, 0, strtmp, &lactive, &size, TID_BOOL, TRUE);
               sprintf(lstate, "No");
               if (lactive)
                  sprintf(lstate, "Yes");
               size = sizeof(lpath);
               sprintf(strtmp, "/logger/channels/%s/settings/Filename", key.name);
               db_get_value(hDB, 0, strtmp, lpath, &size, TID_STRING, TRUE);

               /* substitue "%d" by current run number */
               str[0] = 0;
               strcat(str, lpath);
               if (strchr(str, '%'))
                  sprintf(path, str, rn);
               else
                  strcpy(path, str);
               strcpy(lpath, path);

               size = sizeof(ltype);
               sprintf(strtmp, "/logger/channels/%s/settings/type", key.name);
               db_get_value(hDB, 0, strtmp, ltype, &size, TID_STRING, TRUE);

               size = sizeof(levt);
               sprintf(strtmp, "/logger/channels/%s/statistics/Events written", key.name);
               db_get_value(hDB, 0, strtmp, &levt, &size, TID_DOUBLE, TRUE);

               size = sizeof(lbyt);
               sprintf(strtmp, "/logger/channels/%s/statistics/Bytes written", key.name);
               db_get_value(hDB, 0, strtmp, &lbyt, &size, TID_DOUBLE, TRUE);
               lbyt /= 1024;
               if (lactive) {
                  if (esc_flag) {
                     sprintf(&(ststr[j][0]), "  \033[7m%s\033[m", key.name);
                     if (wd == 1)
                        sprintf(&(ststr[j][15]), "%s", lstate);
                     else
                        sprintf(&(ststr[j][15]), "(%s)", lstate);
                     sprintf(&(ststr[j][22]), "%s", ltype);
                     sprintf(&(ststr[j][32]), "%s", lpath);
                     sprintf(&(ststr[j][52]), "%.0f", levt);
                     sprintf(&(ststr[j++][67]), "%9.2e", lbyt);
                  } else {      /* no esc */

                     sprintf(&(ststr[j][0]), "  %s", key.name);
                     if (wd == 1)
                        sprintf(&(ststr[j][8]), "%s", lstate);
                     else
                        sprintf(&(ststr[j][8]), "(%s)", lstate);
                     sprintf(&(ststr[j][15]), "%s", ltype);
                     sprintf(&(ststr[j][25]), "%s", lpath);
                     sprintf(&(ststr[j][45]), "%.0f", levt);
                     sprintf(&(ststr[j++][60]), "%9.2e", lbyt);
                  }
               } else {         /* not active */

                  sprintf(&(ststr[j][0]), "  %s", key.name);
                  if (wd == 1)
                     sprintf(&(ststr[j][8]), "%s", lstate);
                  else
                     sprintf(&(ststr[j][8]), "(%s)", lstate);
                  sprintf(&(ststr[j][15]), "%s", ltype);
                  sprintf(&(ststr[j][25]), "%s", lpath);
                  sprintf(&(ststr[j][45]), "%.0f", levt);
                  sprintf(&(ststr[j++][60]), "%9.2e", lbyt);
               }
            }                   /* key */
         }                      /* for */
      }                         /* exists */
   } else {
      ststr[j++][0] = '\0';
//-PAA  sprintf(&(ststr[j++][0]),"");
      sprintf(&(ststr[j++][0]), "... Logger currently not running...");
   }

/* --------------------- Lazy logger tree ------------------------ */
/* include lazy if running */
/* search client name under /system/clients/xxx/name */
   {
      float cr, bs;
      INT status, nf, size, i, k;
      char bn[128], tl[128];
      HNDLE hlKey, hKey, hSubkey;
      char client_name[NAME_LENGTH];

      status = db_find_key(hDB, 0, "System/Clients", &hKey);
      if (status != DB_SUCCESS)
         return;

      k = 0;
      /* loop over all clients */
      for (i = 0;; i++) {
         status = db_enum_key(hDB, hKey, i, &hSubkey);
         if (status == DB_NO_MORE_SUBKEYS)
            break;

         if (status == DB_SUCCESS) {
            /* get client name */
            size = sizeof(client_name);
            db_get_value(hDB, hSubkey, "Name", client_name, &size, TID_STRING, TRUE);
            client_name[4] = 0; /* search only for the 4 first char */
            if (equal_ustring(client_name, "Lazy")) {
               sprintf(str, "/Lazy/%s", &client_name[5]);
               status = db_find_key(hDB, 0, str, &hlKey);
               if (status == DB_SUCCESS) {
                  size = sizeof(tl);
                  db_get_value(hDB, hlKey, "/Settings/List label", tl, &size, TID_STRING,
                               TRUE);
                  if (*tl == '\0')
                     sprintf(tl, "<empty>");
                  size = sizeof(cr);
                  db_get_value(hDB, hlKey, "statistics/Copy progress [%]", &cr, &size,
                               TID_FLOAT, TRUE);
                  size = sizeof(nf);
                  db_get_value(hDB, hlKey, "statistics/Number of Files", &nf, &size,
                               TID_INT, TRUE);
                  size = sizeof(bs);
                  db_get_value(hDB, hlKey, "statistics/Backup status [%]", &bs, &size,
                               TID_FLOAT, TRUE);
                  size = sizeof(bn);
                  db_get_value(hDB, hlKey, "statistics/Backup file", bn, &size,
                               TID_STRING, TRUE);

                  if (k == 0) {
                     ststr[j++][0] = '\0';
//              sprintf(ststr[j++],"");
                     sprintf(&(ststr[j][0]), "Lazy Label");
                     sprintf(&(ststr[j][15]), "Progress");
                     sprintf(&(ststr[j][25]), "File name");
                     sprintf(&(ststr[j][45]), "#files");
                     sprintf(&(ststr[j++][60]), "Total");
                  }
                  sprintf(&(ststr[j][0]), "%s", tl);
                  sprintf(&(ststr[j][15]), "%.0f[%%]", cr);
                  sprintf(&(ststr[j][25]), "%s", bn);
                  sprintf(&(ststr[j][45]), "%i", nf);
                  sprintf(&(ststr[j++][60]), "%.1f[%%]", bs);
                  k++;
               }
            }
         }
      }
   }

   ststr[j++][0] = '\0';

/* --------------------- System client list ---------------------- */
/* Get current Client listing */
   if (db_find_key(hDB, 0, "/system/clients", &hKey) == DB_SUCCESS) {
      char clientn[256], clienth[256];
      char *pp, sdummy[64];


      ii = 0;

      sprintf(&(ststr[j][0]), "Clients:");
      for (i = 0;; i++) {
         db_enum_key(hDB, hKey, i, &hSubkey);
         if (!hSubkey)
            break;
         db_get_key(hDB, hSubkey, &key);

         memset(strtmp, 0, sizeof(strtmp));
         size = sizeof(clientn);
         sprintf(strtmp, "name");
         db_get_value(hDB, hSubkey, strtmp, clientn, &size, TID_STRING, TRUE);
         memset(strtmp, 0, sizeof(strtmp));
         size = sizeof(clienth);
         sprintf(strtmp, "host");
         db_get_value(hDB, hSubkey, strtmp, clienth, &size, TID_STRING, TRUE);
         if (ii > 2) {
            ii = 0;
            ststr[++j][0] = '\0';
         }
         memset(sdummy, 0, 64);
         pp = strchr(clienth, '.');
         if (pp != NULL)
            sprintf(&(ststr[j][10 + 23 * ii]), "%s/%s", clientn,
                    strncpy(sdummy, clienth, pp - clienth));
         else
            sprintf(&(ststr[j][10 + 23 * ii]), "%s/%s", clientn, clienth);
         ii++;
      }
      j++;
   }

   if (loop == 1)
      sprintf(ststr[j++],
              "*- [!<cr>] to Exit --- [R<cr>] to Refresh -------------------- Delay:%2.li [sec]-*",
              delta_time / 1000);
   else
      sprintf(ststr[j++],
              "*------------------------------------------------------------------------------*");

   cur_max_line = j;

   /* remove '/0' */
   for (j = 0; j < MAX_LINE; j++)
      while (strlen(ststr[j]) < (LINE_LENGTH - 1))
         ststr[j][strlen(ststr[j])] = ' ';
   return;
}

/*------------------------------------------------------------------*/
int main(int argc, char **argv)
{
   INT status, last_time = 0, file_mode;
   HNDLE hDB, hKey;
   char host_name[HOST_NAME_LENGTH], expt_name[NAME_LENGTH], str[32];
   char ch, svpath[256];
   INT fHandle, i, j = 0, last_max_line = 0;
   INT msg;
   BOOL debug;

   esc_flag = ESC_FLAG;

   /* set default */
   cm_get_environment(host_name, sizeof(host_name), expt_name, sizeof(expt_name));
   svpath[0] = 0;
   file_mode = 1;
   loop = 0;
   delta_time = 5000;

   /* get parameters */
   /* parse command line parameters */
   for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-' && argv[i][1] == 'd')
         debug = TRUE;
      else if (strncmp(argv[i], "-l", 2) == 0)
         loop = 1;
      else if (argv[i][0] == '-') {
         if (i + 1 >= argc || argv[i + 1][0] == '-')
            goto usage;
         if (strncmp(argv[i], "-w", 2) == 0)
            delta_time = 1000 * (atoi(argv[++i]));
         else if (strncmp(argv[i], "-f", 2) == 0)
            strcpy(svpath, argv[++i]);
         else if (strncmp(argv[i], "-e", 2) == 0)
            strcpy(expt_name, argv[++i]);
         else if (strncmp(argv[i], "-h", 2) == 0)
            strcpy(host_name, argv[++i]);
         else if (strncmp(argv[i], "-c", 2) == 0) {
            strcpy(str, argv[++i]);
            if (strncmp(str, "n", 1) == 0 || strncmp(str, "N", 1) == 0)
               file_mode = 0;
         } else {
          usage:
            printf("usage: mstat  -l (loop) -w delay (5sec) -f filename (null)\n");
            printf("              -c compose (Addrun#/norun#)\n");
            printf("             [-h Hostname] [-e Experiment]\n\n");
            return 0;
         }
      }
   }

   /* connect to experiment */
   status = cm_connect_experiment(host_name, expt_name, "MStatus", 0);
   if (status != CM_SUCCESS)
      return 1;

#ifdef _DEBUG
   cm_set_watchdog_params(TRUE, 0);
#endif

   /* reset page */
   memset(ststr, '\0', sizeof(ststr));

   /* turn off message display, turn on message logging */
   cm_set_msg_print(MT_ALL, 0, NULL);

   /* connect to the database */
   cm_get_experiment_database(&hDB, &hKey);

   /* generate status page */
   if (loop == 0) {
      j = 0;
      if (svpath[0] != 0) {
         compose_status(hDB, hKey);
         fHandle = open_log_midstat(file_mode, rn, svpath);
         esc_flag = 0;
         compose_status(hDB, hKey);
         while ((j < cur_max_line) && (ststr[j][0] != '\0')) {
            strncpy(svpath, ststr[j], 80);
            svpath[80] = '\0';
            printf("%s\n", svpath);
            write(fHandle, "\n", 1);
            write(fHandle, ststr[j], strlen(ststr[j]));
            j++;
         }
         close(fHandle);
      } else {
         esc_flag = 0;
         compose_status(hDB, hKey);
         while ((j < cur_max_line) && (ststr[j][0] != '\0')) {
            strncpy(svpath, ststr[j++], 80);
            svpath[80] = '\0';
            printf("%s\n", svpath);
         }
      }
   } else {

      /* initialize ss_getchar() */
      ss_getchar(0);

      do {
         if ((ss_millitime() - last_time) > delta_time) {
            last_time = ss_millitime();
            compose_status(hDB, hKey);
            if (cur_max_line < last_max_line)
               ss_clear_screen();
            last_max_line = cur_max_line;

            j = 0;
            while ((j < cur_max_line) && (ststr[j][0] != '\0')) {
               ss_printf(0, j, "%s", ststr[j]);
               j++;
            }
         }
         ch = 0;
         while (ss_kbhit()) {
            ch = ss_getchar(0);
            if (ch == -1)
               ch = getchar();
            if (ch == 'R')
               ss_clear_screen();
            if ((char) ch == '!')
               break;
         }
         msg = cm_yield(200);
      } while (msg != RPC_SHUTDOWN && msg != SS_ABORT && ch != '!');
   }
   printf("\n");

   /* reset terminal */
   ss_getchar(TRUE);

   cm_disconnect_experiment();
   return 1;
}
