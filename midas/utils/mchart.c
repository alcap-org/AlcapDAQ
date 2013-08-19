/********************************************************************\

  Name:         mchart.c
  Created by:   Pierre-Andre Amaudruz

  Contents:     Periodic Midas chart graph data generator.
                Generate the proper configuration and data files for
    the gstripchart application or the GH (Gertjan Hofman)
    stripchart.tcl
    standard running description is:
    mchart -f <file> -q <ODB tree> -c
     ex:  mchart -f trigger -q /equipment/trigger/statistics -c
          produce trigger.conf and trigger (data)

    mchart -f <file>[.conf] -g
     ex:  mchart -f trigger[.conf] -gg
          produce periodically trigger and spawn gstripchart for
    display of the data.
    See mchart -h for further info.

  $Log: mchart.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:38  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.11  2004/01/08 08:40:11  midas
  Implemented standard indentation

  Revision 1.10  2003/05/09 07:40:05  midas
  Added extra parameter to cm_get_environment

  Revision 1.9  2003/04/23 23:08:33  pierre
  Fixed compiler warning

  Revision 1.8  2002/06/03 06:07:15  midas
  Added extra parameter to ss_daemon_init to keep stdout

  Revision 1.7  2002/05/10 23:57:37  pierre
  pass arg to stripchart + cosmetics

  Revision 1.6  2001/12/08 01:04:13  pierre
  fix float min, max values

  Revision 1.5  2000/10/20 21:59:55  pierre
  - Fix duplicate config bug
  - Add messages on spawning, creation.

  Revision 1.4  2000/10/20 19:10:57  pierre
  - cleanup code for arguments
    mchart -f <file>[.conf] for run
    mchart -f <file>[.conf] -q <[/]eqp_path> for creation
    disable -D if -c or -q
    prevent config overwrite if -q not valid

  Revision 1.3  2000/09/29 20:10:30  pierre
  fix warning messages

  Revision 1.2  2000/04/20 18:22:38  pierre
  - added "%" to "_" substitution for variable names (midas group, tcl parsing)

  Revision 1.1  2000/04/18 20:29:15  pierre
  - Periodic Midas chart graph data generator. (new application)

\********************************************************************/

#include <fcntl.h>
#include "midas.h"
#include "msystem.h"

#define  CONF_CREATE  1
#define  CONF_APPEND  2

char color[][16] =
    { "blue", "red", "brown", "black", "orange", "cyan", "yellow", "beige", "DarkCyan",
   "DarkOliveGreen", "goldenrod", "DeepPink"
};
INT max_element = 8;
DWORD delta_time;
BOOL keep = FALSE, debug = FALSE, create_only = FALSE;
BOOL create = FALSE, config_done = FALSE;
INT graph = 0, element = 0;

INT mchart_compose(HNDLE hDB, char *confpath, char *datapath, char *eqpstr);
INT conf_compose(INT action, char *confpath, char *datapath, char *field, float val);
float toplimit = 0., botlimit = 0.;

/*------------------------------------------------------------------*/
INT conf_compose(INT action, char *confpath, char *datapath, char *field, float val)
{
   FILE *f;
   float minval, maxval;

   if (config_done)
      return 0;

   switch (action) {
   case CONF_CREATE:
      if (keep)
         return 1;
      /* open device */
      f = fopen(confpath, "w+");
      if (f == NULL) {
         printf("Cannot open configuration strip file.\n");
         return 0;
      }
      fprintf(f, "#Equipment:            >%s", field);
      fprintf(f, "\n");
      fprintf(f, "menu:                   on\n");
      fprintf(f, "slider:                 on\n");
      fprintf(f, "type:                   gtk\n");

      fprintf(f, "minor_ticks:            12\n");
      fprintf(f, "major_ticks:            6\n");

      fprintf(f, "chart-interval:         1.000\n");
      fprintf(f, "chart-filter:           0.500\n");
      fprintf(f, "slider-interval:        0.200\n");
      fprintf(f, "slider-filter:          0.200\n");
      fclose(f);
      break;

   case CONF_APPEND:
      if (keep)
         return 1;

      f = fopen(confpath, "a+");
      if (f == NULL) {
         printf("Cannot open configuration strip file.\n");
         return 0;
      }
      if ((element % max_element) == 0)
         element = 0;

      minval = maxval = val;
      /* check upper limits */
      if (toplimit == 0.) {     /* use current value */
         if (val == 0.)
            maxval = 5.;
         else if (val > 0.)
            maxval *= 2.;
         else
            maxval /= 2.;
      } else                    /* use arg value */
         maxval = toplimit;

      /* check lower limits */
      if (botlimit == 0.) {     /* use current value */
         if (val == 0.)
            minval = -5.;
         else if (val > 0.)
            minval /= 2.;
         else
            minval *= 2.;
      } else                    /* use arg value */
         minval = botlimit;

      fprintf(f, "begin:        %s\n", field);
      fprintf(f, "  filename:     %s\n", datapath);
      fprintf(f, "  fields:       2\n");
      fprintf(f, "  pattern:      %s\n", field);
      fprintf(f, "  equation:     $2\n");
      fprintf(f, "  color:        %s\n", color[element++]);
      fprintf(f, "  maximum:      %6.2f\n", maxval);
      fprintf(f, "  minimum:      %6.2f\n", minval);
      fprintf(f, "  id_char:      1\n");
      fprintf(f, "end:            %s\n", field);
      fclose(f);
      break;
   default:
      printf(" unknown command \n");
   }
   return 1;
}

/*------------------------------------------------------------------*/
INT mchart_get_names(HNDLE hDB, char *eqpstr, char *element, char **pname, INT * esize)
{
   char *pslash, *p;
   char strtmp[128];
   HNDLE hKeyS, hSubKey;
   KEY key;
   INT i, size, status;
   BOOL bslash = FALSE;

   /* convert to upper */
   p = eqpstr;
   for (i = 0; i < 128; i++)
      strtmp[i] = 0;
   while (*p)
      *p++ = (char) tolower(*p);

   /* I don't remember what I'm doing here but it's useful! */
   if (strncmp(eqpstr, "equipment/", 10) == 0)
      bslash = TRUE;
   if (strncmp(eqpstr, "/equipment/", 11) == 0) {
      /* if the -q starts with /equipment and 2 more slash are following
         OR
         if the -q starts with equipment and 1 more slash are following
         then I search the Settings for the names of the variables */
      pslash = strchr(eqpstr, '/');
      pslash = strchr(pslash + 1, '/');
      if (!bslash)
         pslash = strchr(pslash + 1, '/');
      if (pslash) {
         strncpy(strtmp, eqpstr, pslash - eqpstr);
         strcat(strtmp, "/settings");

         if (db_find_key(hDB, 0, strtmp, &hKeyS) == DB_SUCCESS) {
            db_get_key(hDB, hKeyS, &key);
            for (i = 0;; i++) { /* search first for "Names element" */
               db_enum_key(hDB, hKeyS, i, &hSubKey);
               if (!hSubKey)
                  break;
               db_get_key(hDB, hSubKey, &key);
               if (key.name[5] && key.type == TID_STRING) {     /* fmt Names xxxx */
                  p = &(key.name[6]);
                  if (strstr(p, element) == NULL)
                     continue;
                  *esize = key.item_size;
                  size = *esize * key.num_values;
                  *pname = malloc(size);
                  status = db_get_data(hDB, hSubKey, *pname, &size, key.type);
                  return key.num_values;
               }
            }
            /* rescan dir for "Names" only now */
            db_get_key(hDB, hKeyS, &key);
            for (i = 0;; i++) {
               db_enum_key(hDB, hKeyS, i, &hSubKey);
               if (!hSubKey)
                  break;
               db_get_key(hDB, hSubKey, &key);
               if (strncmp(key.name, "Names", 5) == 0) {        /* fmt Names (only) */
                  *esize = key.item_size;
                  size = *esize * key.num_values;
                  *pname = malloc(size);
                  status = db_get_data(hDB, hSubKey, *pname, &size, key.type);
                  return key.num_values;
               }
            }
         }
      }
   }
   return 0;
}

/*------------------------------------------------------------------*/
INT mchart_get_array(FILE * f, char *eqpstr, HNDLE hDB, HNDLE hSubkey, KEY key,
                     char *confpath, char *datapath)
{
   char field[128];
   INT nfound, status, i, size, esize;
   char *pspace, *pdata = NULL, *pname = NULL;
   float value;

   size = key.num_values * key.item_size;
   pdata = malloc(size);
   status = db_get_data(hDB, hSubkey, pdata, &size, key.type);
   if (status != DB_SUCCESS) {
      free(pdata);
      return 0;
   }

   if (debug)
      printf("%s : size:%d\n", key.name, size);

   nfound = mchart_get_names(hDB, eqpstr, key.name, &pname, &esize);

   if (debug)
      printf("#name:%d #Values:%d\n", nfound, key.num_values);
   /* if names found but no array then use variable names */
   if (key.num_values == 1)
      nfound = FALSE;
   for (i = 0; i < key.num_values; i++) {
      if (nfound)
         strcpy(field, (pname + (i * esize)));
      else {
         if (key.num_values == 1)
            sprintf(field, "%s", key.name);
         else                   /* if no settings/Names ... found then compose names here */
            sprintf(field, "%s[%i]", key.name, i);
      }

      /* substitution for tcl */
      while ((pspace = strstr(field, " ")))
         *pspace = '_';
      while ((pspace = strstr(field, "%")))
         *pspace = '_';

      /* fill data file */
      if (key.type == TID_INT) {
         value = (float) *((INT *) (pdata + (i * key.item_size)));
         fprintf(f, "%s %d\n", field, *((INT *) (pdata + (i * key.item_size))));
      } else if (key.type == TID_WORD) {
         value = (float) *((WORD *) (pdata + (i * key.item_size)));
         fprintf(f, "%s %d\n", field, *((WORD *) (pdata + (i * key.item_size))));
      } else if (key.type == TID_DWORD) {
         value = (float) *((DWORD *) (pdata + (i * key.item_size)));
         fprintf(f, "%s %ld\n", field, *((DWORD *) (pdata + (i * key.item_size))));
      } else if (key.type == TID_FLOAT) {
         value = *((float *) (pdata + (i * key.item_size)));
         fprintf(f, "%s %f\n", field, *((float *) (pdata + (i * key.item_size))));
      } else if (key.type == TID_DOUBLE) {
         value = (float) *((double *) (pdata + (i * key.item_size)));
         fprintf(f, "%s %e\n", field, *((double *) (pdata + (i * key.item_size))));
      } else
         continue;

      if (!config_done) {
         if (debug)
            printf("Field:%s Values:%f\n", field, value);
         conf_compose(CONF_APPEND, confpath, datapath, field, value);
      }
   }
   if (pdata)
      free(pdata);
   if (pname)
      free(pname);
   return 1;
}

/*------------------------------------------------------------------*/
INT mchart_compose(HNDLE hDB, char *confpath, char *datapath, char *eqpstr)
{
   FILE *fHandle;
   INT i, status;
   KEY key;
   HNDLE hKey, hSubkey;

   /* check if dir exists */
   if ((status = db_find_key(hDB, 0, eqpstr, &hKey)) == DB_SUCCESS) {
      fHandle = fopen(datapath, "w+");
      if (fHandle == NULL) {
         printf("Cannot open config %s file.\n", confpath);
         return -1;
      }

      db_get_key(hDB, hKey, &key);
      if (key.type != TID_KEY) {
         status = mchart_get_array(fHandle, eqpstr, hDB, hKey, key, confpath, datapath);
         if (status < 1) {
            printf("Error: cannot get array for %s\n", eqpstr);
            return -2;
         }
      } else
         for (i = 0;; i++) {
            db_enum_key(hDB, hKey, i, &hSubkey);
            if (!hSubkey)
               break;
            db_get_key(hDB, hSubkey, &key);
            status =
                mchart_get_array(fHandle, eqpstr, hDB, hSubkey, key, confpath, datapath);
            if (status < 1) {
               printf("Error: cannot get array for %s\n", eqpstr);
               return -3;
            }
         }                      /* get value */
   } /* equ found */
   else {
      printf("Error: Equipment:%s not found\n", eqpstr);
      return 0;
   }
   fclose(fHandle);
   config_done = TRUE;
   return 1;
}

/*------------------------------------------------------------------*/
int main(int argc, char **argv)
{
   char command[128];
   BOOL daemon, keep_strip = FALSE;
   INT status, last_time = 0;
   DWORD i;
   HNDLE hDB, hKey;
   char host_name[HOST_NAME_LENGTH], expt_name[NAME_LENGTH];
   char eqpstr[128] = { '\0' };
   char ch, cmdline[256];
   char mchart_dir[128] = { '\0' }, mchart_data[128], mchart_conf[128];
   INT msg, childpid;
   int sys_err;
   /* set default */
   cm_get_environment(host_name, sizeof(host_name), expt_name, sizeof(expt_name));
   delta_time = 5000;
   daemon = FALSE;

   /* retrieve environment */
   if (getenv("MCHART_DIR")) {
      strcpy(mchart_dir, getenv("MCHART_DIR"));
      if (mchart_dir[strlen(mchart_dir) - 1] != DIR_SEPARATOR)
         strcat(mchart_dir, DIR_SEPARATOR_STR);
   }

   /* get parameters parse command line parameters */
   for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-' && argv[i][1] == 'd')
         debug = TRUE;
      else if (argv[i][0] == '-' && argv[i][1] == 'c')
         create_only = TRUE;
      else if (argv[i][0] == '-' && argv[i][1] == 's')
         keep = TRUE;
      else if (argv[i][0] == '-' && argv[i][1] == 'g' && argv[i][2] == 'g')
         graph = 2;
      else if (argv[i][0] == '-' && argv[i][1] == 'g' && argv[i][2] == 'h')
         graph = 3;
      else if (argv[i][0] == '-' && argv[i][1] == 'g')
         graph = 1;
      else if (argv[i][0] == '-' && argv[i][1] == 'k')
         keep_strip = TRUE;
      else if (argv[i][0] == '-' && argv[i][1] == 'D')
         daemon = TRUE;
      else if (argv[i][0] == '-') {
         if (i + 1 >= argc || (argv[i + 1][0] == '-' && !isdigit(argv[i + 1][1])))
            goto usage;
         if (strncmp(argv[i], "-u", 2) == 0)
            delta_time = 1000 * (atoi(argv[++i]));
         else if (strncmp(argv[i], "-f", 2) == 0) {
            char *pt;
            pt = strstr(argv[++i], ".conf");
            if (pt)
               *pt = '\0';
            strcpy(mchart_data, mchart_dir);
            strcat(mchart_data, argv[i]);
            sprintf(mchart_conf, "%s.conf", mchart_data);
         } else if (strncmp(argv[i], "-e", 2) == 0)
            strcpy(expt_name, argv[++i]);
         else if (strncmp(argv[i], "-h", 2) == 0)
            strcpy(host_name, argv[++i]);
         else if (strncmp(argv[i], "-q", 2) == 0) {
            if (argv[++i][0] != '/')
               strcpy(eqpstr, "/");
            strcat(eqpstr, argv[i]);
            create = TRUE;
         } else if (strncmp(argv[i], "-b", 2) == 0)
            botlimit = (float) atof(argv[++i]);
         else if (strncmp(argv[i], "-t", 2) == 0)
            toplimit = (float) atof(argv[++i]);
      } else {
       usage:
         printf("usage:   mchart -u updates (5sec)\n");
         printf("                -f configuration or data file name\n");
         printf("                -q equipment\n");
         printf(" (override all) -b bot_limit -t top_limit (only with -q) \n");
         printf("                -c create config only \n");
         printf("                -k keep spawned stripchart or gstripchart\n");
         printf
             ("                -g spawn Hofman stripchart or gstripchart if possible\n");
         printf("                -gg forces gstripchart spawning\n");
         printf("                -gh forces  stripchart spawning\n");
         printf("                -D start as daemon (prevented if -c or -q)\n");
         printf("                   requires $MCHART_DIR\n");
         printf("               [-h Hostname] [-e Experiment]\n\n");
         printf("ex: Creation  : mchart -e myexpt -h myhost -c \n");
         printf("                -f mydata -q /equipement/myeqp/variables\n");
         printf("    Running   : mchart -e myexpt -h myhost -f mydata \n");
         printf("    Run/Graph : mchart -e myexpt -h myhost -f mydata -g\n");
         printf("    MCHART_DIR: environment variable for mchar directory\n\n");
         return 0;
      }
   }

/* Daemon start */
   if (daemon && !(create || create_only)) {
      printf("Becoming a daemon...\n");
      ss_daemon_init(FALSE);
   }

   if (daemon)
      printf("arg -D ignored due to creation request! \n");

/* connect to experiment */
   status = cm_connect_experiment(host_name, expt_name, "MChart", 0);
   if (status != CM_SUCCESS)
      return 1;

   if (debug)
      cm_set_watchdog_params(TRUE, 0);

   /* turn off message display, turn on message logging */
   cm_set_msg_print(MT_ALL, 0, NULL);

   /* connect to the database */
   cm_get_experiment_database(&hDB, &hKey);

   /* initialize ss_getchar() */
   ss_getchar(0);

   /* recompose command line */
   sprintf(cmdline, "%s ", argv[1]);
   for (i = 2; i < argc; i++) {
      strcat(cmdline, " ");
      strcat(cmdline, argv[i]);
   }

   /* check if creation required */
   if (!create) {
      /* extract equipment key */
      FILE *f;
      char strtmp[128];
      char *peqp;

      config_done = TRUE;
      create = FALSE;
      /* Overwrite the -q using the file.conf content */
      f = fopen(mchart_conf, "r");
      if (f == NULL) {
         printf("Error: Cannot open %s\n", mchart_conf);
         goto error;
      }
      fgets(strtmp, 128, f);
      if (strstr(strtmp, "#Equipment")) {
         peqp = strstr(strtmp, ">");
         sprintf(eqpstr, "%s", peqp + 1);
      }
      fclose(f);
      peqp = strstr(eqpstr, "\n");
      *peqp = 0;
   } else {
      INT size;
      /* creation of config file requested */
      /* check if equipment string is a valifd key in order to
         prevent overwriting the configuration file */
      size = sizeof(eqpstr);
      if (db_find_key(hDB, 0, eqpstr, &hKey) != DB_SUCCESS) {
         printf("unknown odb path under -q arg. (%s)\n", eqpstr);
         goto error;
      }
      status = conf_compose(CONF_CREATE, mchart_conf, mchart_data, eqpstr, (float) argc);
      if (status != 1)
         goto error;
   }

   do {
      if ((ss_millitime() - last_time) > delta_time) {
         last_time = ss_millitime();
         status = mchart_compose(hDB, mchart_conf, mchart_data, eqpstr);
         if (status != 1)
            goto error;
         if (create_only) {
            printf("mchart file %s created\n", mchart_conf);
            goto out;
         }
      }

      /* spawn graph once if possible */
      if (graph == 1) {
         char command[128];
         char *list = NULL;
         INT i, nfile;

         char strip[][32] = { "/usr/local/bin", "stripchart",
            "/usr/bin", "gstripchart",
            "/home/midas/bin", "stripchart",
            "/usr/sbin", "gstripchart",
            "/home/chaos/bin", "stripchart",
            "/sbin", "gstripchart",
            ""
         };

         i = 0;
         while (strip[i][0]) {
            nfile = ss_file_find(strip[i], strip[i + 1], &list);
            free(list);
            if (nfile == 1)
               break;
            i += 2;
         }
         if (strip[i][0] == 0) {
            printf("No graphic package found in following dir:\n");
            i = 0;
            while (strip[i][0]) {
               printf("Package : %s/%s \n", strip[i], strip[i + 1]);
               i += 2;
            }
            break;
         }
         if (i % 2)
            sprintf(command, "gstripchart -g 500x200-200-800 -f %s", mchart_conf);
         else
            sprintf(command, "stripchart %s", mchart_conf);

         if (!daemon)
            printf("Spawning graph with %s ...\nUse '!' to exit\n", command);
         ss_exec(command, &childpid);
      } else if (graph == 2) {  /* Gnu graph */
         sprintf(command, "gstripchart -g 500x200-200-800 -f %s", mchart_conf);
         if (!daemon)
            printf("Spawning graph with %s ...\nUse '!' to exit\n", command);
         ss_exec(command, &childpid);
      } else if (graph == 3) {  /* Hofman graph */
         if (debug) {
            sprintf(command, "stripchart -debug %s", mchart_conf);
            if (!daemon)
               printf("Spawning graph with %s ...in DEBUG mode \nUse '!' to exit\n",
                      command);
            sys_err = system(command);
            printf("system error return %d \n", sys_err);
            cm_disconnect_experiment();
            return 1;

         } else {
            sprintf(command, "stripchart %s", mchart_conf);
            if (!daemon)
               printf("Spawning graph with %s .... \nUse '!' to exit\n", command);
            ss_exec(command, &childpid);
         }

      }

      graph = 0;
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

      /* exit this process if no stripchart associated to it */
      if (!keep_strip && !ss_existpid(childpid))
         break;

   } while (msg != RPC_SHUTDOWN && msg != SS_ABORT && ch != '!');

 out:
 error:
   printf("\n");

   /* reset terminal */
   ss_getchar(TRUE);

   cm_disconnect_experiment();
   return 1;
}
