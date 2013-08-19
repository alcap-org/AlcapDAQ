 /********************************************************************\

  Name:         mhist.c
  Created by:   Stefan Ritt

  Contents:     MIDAS history display utility

  $Log: mhist.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:38  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.20  2004/07/12 12:23:24  midas
  Fixed small bug with hs_enum_vars()

  Revision 1.19  2004/01/08 08:40:11  midas
  Implemented standard indentation

  Revision 1.18  2003/04/23 23:54:04  pierre
  Fixed compiler warning

  Revision 1.17  2003/01/13 15:47:56  midas
  Fixed a few bugs, now -l is working with -f

  Revision 1.16  2002/09/13 16:36:51  pierre
  add -i idx1:idx2 option for -v request

  Revision 1.15  2002/05/22 05:43:33  midas
  Added extra variables to hs_enum_vars for mhist to display array size

  Revision 1.14  2002/05/08 19:54:41  midas
  Added extra parameter to function db_get_value()

  Revision 1.13  2001/12/13 08:50:53  midas
  Removed tabs in source code

  Revision 1.12  2001/12/13 04:31:52  pierre
  add hs_set_path, add -z for path

  Revision 1.11  2000/06/20 06:52:27  midas
  Changed date format from DDMMYY to YYMMDD

  Revision 1.10  2000/04/25 11:56:09  midas
  Added -s and -p flags to specify start/stop time explicitly

  Revision 1.9  2000/04/25 11:04:42  midas
  Added display_vars(filename) function which can be used to display variables
  in an arbitrary history file via the '-l' and '-f filename' flags

  Revision 1.8  2000/04/17 17:15:06  pierre
  - added arg -l for list of EventID and variables (single arg)
  - added arg -b for binary time stamp.
  - change prg exit with return 0 for Unix success.

  Revision 1.7  1999/08/17 12:39:51  midas
  Fixed bug

  Revision 1.6  1999/07/06 09:02:53  midas
  Fixed little bug

  Revision 1.5  1999/06/28 12:01:10  midas
  Added -f flag

  Revision 1.4  1999/06/02 07:52:50  midas
  Fixed compiler warning

  Revision 1.3  1999/05/05 12:03:21  midas
  Adapted usage of new history functions

  Revision 1.2  1998/10/12 12:19:03  midas
  Added Log tag in header

\********************************************************************/

#include "midas.h"
#include "msystem.h"

void tmp()
{
   time_t tm;

   time(&tm);
   tm = ss_time();
   hs_dump(1, tm - 3600, tm, 0, FALSE);
}

/*------------------------------------------------------------------*/

TAG temp_tag[] = {
   {"Temperatures", TID_FLOAT, 100},
   {"Humidity", TID_FLOAT, 99},
   {"Pressure1", TID_FLOAT, 1},
};

TAG hv_tag[] = {
   {"HV", TID_FLOAT, 100},
};

float hist[200];
float hv[100];
BOOL binary_time;
TAG tag[10];
void write_hist_speed()
/* write some history */
{
   DWORD start_time, act_time;
   int i, j, bytes;

   hs_define_event(1, "Temperature", temp_tag, sizeof(temp_tag));
   hs_define_event(2, "HV", hv_tag, sizeof(hv_tag));

   start_time = ss_millitime();
   j = bytes = 0;
   do {
      for (i = 0; i < 100; i++) {
         hist[i] = (float) j;
         hs_write_event(1, hist, sizeof(hist));
         hs_write_event(2, hv, sizeof(hv));
      }

      j += 2 * i;
      bytes += (sizeof(hist) + sizeof(hv)) * i;
      act_time = ss_millitime();

      printf("%ld\n", ss_time());

   } while (act_time - start_time < 10000);

   printf("%ld events (%ld kB) per sec.\n", j / (act_time - start_time) * 1000,
          bytes / 1024 / (act_time - start_time) * 1000);
}

void generate_hist()
/* write some history */
{
   int i, j;

   hs_define_event(1, "Temperature", temp_tag, sizeof(temp_tag));
   hs_write_event(1, hist, sizeof(hist));

   hs_define_event(2, "HV", hv_tag, sizeof(hv_tag));
   hs_write_event(2, hv, sizeof(hv));

   for (i = 0; i < 10; i++) {
      hist[0] = (float) i;
      hist[1] = i / 10.f;
      hs_write_event(1, hist, sizeof(hist));

      for (j = 0; j < 100; j++)
         hv[j] = j + i / 10.f;
      hs_write_event(2, hv, sizeof(hv));
      printf("%ld\n", ss_time());
      ss_sleep(1000);
   }
}

/*------------------------------------------------------------------*/
INT query_params(DWORD * ev_id, DWORD * start_time, DWORD * end_time,
                 DWORD * interval, char *var_name, DWORD * var_type,
                 INT * var_n_data, DWORD * index)
{
   DWORD status, hour, i, bytes, n, *var_n, n_bytes;
   INT var_index, *event_id, name_size, id_size;
   char *event_name;
   char *var_names;

   status = hs_count_events(0, &n);
   if (status != HS_SUCCESS)
      return status;
   name_size = n * NAME_LENGTH;
   id_size = n * sizeof(INT);
   event_name = malloc(name_size);
   event_id = malloc(id_size);
   hs_enum_events(0, event_name, (DWORD *) & name_size, event_id, (DWORD *) & id_size);

   printf("Available events:\n");
   for (i = 0; i < n; i++)
      printf("ID %d: %s\n", event_id[i], event_name + i * NAME_LENGTH);

   if (n > 1) {
      printf("\nSelect event ID: ");
      scanf("%ld", ev_id);
   } else
      *ev_id = event_id[0];

   hs_count_vars(0, *ev_id, &n);
   n_bytes = n * sizeof(DWORD);
   bytes = n * NAME_LENGTH;
   var_names = malloc(bytes);
   var_n = malloc(n_bytes);
   hs_enum_vars(0, *ev_id, var_names, &bytes, var_n, &n_bytes);

   printf("\nAvailable variables:\n");
   for (i = 0; i < n; i++)
      if (var_n[i] > 1)
         printf("%ld: %s[%ld]\n", i, var_names + i * NAME_LENGTH, var_n[i]);
      else
         printf("%ld: %s\n", i, var_names + i * NAME_LENGTH);

   *index = var_index = 0;
   if (n > 1) {
      printf("\nSelect variable (0..%ld,-1 for all): ", n - 1);
      scanf("%d", &var_index);
      if (var_index >= (INT) n)
         var_index = n - 1;
      if (var_index >= 0)
         hs_get_var(0, *ev_id, var_names + var_index * NAME_LENGTH, var_type, var_n_data);
      if (var_index >= 0 && *var_n_data > 1 && *var_type != TID_STRING) {
         printf("\nSelect index (0..%d): ", *var_n_data - 1);
         scanf("%ld", index);
      }
   }

   if (var_index < 0)
      var_name[0] = 0;
   else
      strcpy(var_name, var_names + var_index * NAME_LENGTH);

   printf("\nHow many hours: ");
   scanf("%ld", &hour);
   *start_time = ss_time() - hour * 3600;
   *end_time = ss_time();

   printf("\nInterval [sec]: ");
   scanf("%ld", interval);
   printf("\n");

   free(var_names);
   free(var_n);
   free(event_name);
   free(event_id);

   return HS_SUCCESS;
}

/*------------------------------------------------------------------*/

INT display_vars(char *file_name)
{
   DWORD status, i, j, bytes, n, nv, ltime, *var_n, n_bytes;
   INT *event_id, name_size, id_size;
   char *event_name;
   char *var_names;

   ltime = 0;
   if (file_name[0]) {
      struct tm tms;

      memset(&tms, 0, sizeof(tms));
      tms.tm_hour = 12;
      tms.tm_year = 10 * (file_name[0] - '0') + (file_name[1] - '0');
      if (tms.tm_year < 90)
         tms.tm_year += 100;
      tms.tm_mon = 10 * (file_name[2] - '0') + (file_name[3] - '0') - 1;
      tms.tm_mday = 10 * (file_name[4] - '0') + (file_name[5] - '0');
      ltime = mktime(&tms);
   }

   status = hs_count_events(ltime, &n);
   if (status != HS_SUCCESS)
      return status;

   name_size = n * NAME_LENGTH;
   id_size = n * sizeof(INT);
   event_name = malloc(name_size);
   event_id = malloc(id_size);
   hs_enum_events(ltime, event_name, (DWORD *) & name_size, event_id,
                  (DWORD *) & id_size);

   for (i = 0; i < n; i++) {
      printf("\nEvent ID %d: %s\n", event_id[i], event_name + i * NAME_LENGTH);
      hs_count_vars(ltime, event_id[i], &nv);
      bytes = nv * NAME_LENGTH;
      n_bytes = nv * sizeof(DWORD);
      var_names = malloc(bytes);
      var_n = malloc(nv * sizeof(DWORD));

      hs_enum_vars(ltime, event_id[i], var_names, &bytes, var_n, &n_bytes);
      for (j = 0; j < nv; j++)
         if (var_n[j] > 1)
            printf("%ld: %s[%ld]\n", j, var_names + j * NAME_LENGTH, var_n[j]);
         else
            printf("%ld: %s\n", j, var_names + j * NAME_LENGTH);

      free(var_n);
      free(var_names);
   }

   free(event_name);
   free(event_id);

   return HS_SUCCESS;
}

/*------------------------------------------------------------------*/
void display_single_hist(DWORD event_id, DWORD start_time, DWORD end_time,
                         DWORD interval, char *var_name, DWORD index)
/* read back history */
{
   char buffer[10000];
   DWORD tbuffer[1000];
   DWORD i, size, tbsize, n, type;
   char str[256];
   INT status = 0;

   do {
      size = sizeof(buffer);
      tbsize = sizeof(tbuffer);
      status = hs_read(event_id, start_time, end_time, interval, var_name, index,
                       tbuffer, &tbsize, buffer, &size, &type, &n);

      if (n == 0)
         printf("No variables \"%s\" found in specified time range\n", var_name);

      for (i = 0; i < n; i++) {
         if (binary_time)
            sprintf(str, "%li ", tbuffer[i]);
         else {
            sprintf(str, "%s", ctime(&tbuffer[i]) + 4);
            str[20] = '\t';
         }
         if (type == TID_STRING) {
            strcat(str, "\n");
            strcpy(&str[strlen(str)], buffer + (size / n) * i);
         } else
            db_sprintf(&str[strlen(str)], buffer, rpc_tid_size(type), i, type);

         strcat(str, "\n");

         printf(str);
      }

      if (status == HS_TRUNCATED)
         start_time = tbuffer[n - 1] + (tbuffer[n - 1] - tbuffer[n - 2]);

   } while (status == HS_TRUNCATED);
}

/*------------------------------------------------------------------*/

void display_range_hist(DWORD event_id, DWORD start_time, DWORD end_time,
                        DWORD interval, char *var_name, DWORD index1, DWORD index2)
/* read back history */
{
   char buffer[50][10000];
   DWORD tbuffer[1000];
   DWORD i, size[50], tbsize, n[50], type[50], idx;
   char str[256];
   INT status = 0, j;

   if (index2 > index1 + 49) {
      printf("Specified range too large (max 50)\n");
      return;
   } else if (index2 < index1) {
      printf("Wrong specified range (low>high)\n");
      return;
   }

   do {
      for (j = 0, idx = index1; idx < index2 + 1; idx++, j++) {
         size[j] = sizeof(buffer);
         tbsize = sizeof(tbuffer);
         status = hs_read(event_id, start_time, end_time, interval, var_name, idx,
                          tbuffer, &tbsize, buffer[j], &size[j], &type[j], &n[j]);

         if (n[j] == 0)
            printf("No variables \"%s\" found in specified time range\n", var_name);

         if (status == HS_TRUNCATED)
            start_time = tbuffer[n[j] - 1] + (tbuffer[n[j] - 1] - tbuffer[n[j] - 2]);
      }
   } while (status == HS_TRUNCATED);

   printf("mhist for Var:%s[%ld:%ld]\n", var_name, index1, index2);
   for (i = 0; i < n[0]; i++) {
      if (binary_time)
         sprintf(str, "%li ", tbuffer[i]);
      else {
         sprintf(str, "%s", ctime(&tbuffer[i]) + 4);
         str[20] = '\t';
      }

      for (j = 0, idx = index1; idx < index2 + 1; idx++, j++) {
         if (type[j] == TID_STRING) {
            strcat(str, "\n");
            strcpy(&str[strlen(str)], &(buffer[j])[0] + (size[j] / n[j]) * i);
         } else {
            strcat(str, "\t");
            db_sprintf(&str[strlen(str)], buffer[j], rpc_tid_size(type[j]), i, type[j]);
         }
      }

      strcat(str, "\n");

      printf(str);
   }
}

/*------------------------------------------------------------------*/
DWORD convert_time(char *t)
/* convert date in format YYMMDD[.HHMM[SS]] into decimal time */
{
   struct tm tms;
   DWORD ltime;
   INT isdst;

#if !defined(OS_VXWORKS)
#if !defined(OS_VMS)
   tzset();
#endif
#endif

   memset(&tms, 0, sizeof(tms));

   tms.tm_year = 10 * (t[0] - '0') + (t[1] - '0');
   tms.tm_mon = 10 * (t[2] - '0') + (t[3] - '0') - 1;
   tms.tm_mday = 10 * (t[4] - '0') + (t[5] - '0');
   if (tms.tm_year < 90)
      tms.tm_year += 100;
   if (t[6] == '.') {
      tms.tm_hour = 10 * (t[7] - '0') + (t[8] - '0');
      tms.tm_min = 10 * (t[9] - '0') + (t[10] - '0');
      if (t[11])
         tms.tm_sec = 10 * (t[11] - '0') + (t[12] - '0');
   }

   ltime = mktime(&tms);

   /* correct for dst */
   memcpy(&tms, localtime(&ltime), sizeof(tms));

   isdst = tms.tm_isdst;
   memset(&tms, 0, sizeof(tms));
   tms.tm_isdst = isdst;

   tms.tm_year = 10 * (t[0] - '0') + (t[1] - '0');
   tms.tm_mon = 10 * (t[2] - '0') + (t[3] - '0') - 1;
   tms.tm_mday = 10 * (t[4] - '0') + (t[5] - '0');
   if (tms.tm_year < 90)
      tms.tm_year += 100;
   if (t[6] == '.') {
      tms.tm_hour = 10 * (t[7] - '0') + (t[8] - '0');
      tms.tm_min = 10 * (t[9] - '0') + (t[10] - '0');
      if (t[11])
         tms.tm_sec = 10 * (t[11] - '0') + (t[12] - '0');
   }

   ltime = mktime(&tms);

   return ltime;
}

/*------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
   DWORD status, event_id, start_time = 0, end_time, interval, index, index1 = 0, index2 =
       0;
   INT i, var_n_data;
   BOOL list_query;
   DWORD var_type;
   char var_name[NAME_LENGTH], file_name[256];
   char *p, path_name[256], path1_name[256];
   char start_name[64];
   char *column;

   /* turn off system message */
   cm_set_msg_print(0, MT_ALL, puts);

   var_name[0] = 0;
   file_name[0] = 0;
   list_query = FALSE;
   path_name[0] = 0;
   path1_name[0] = 0;
   start_name[0] = 0;

   if (argc == 1) {
      status = query_params(&event_id, &start_time, &end_time,
                            &interval, var_name, &var_type, &var_n_data, &index);
      if (status != HS_SUCCESS)
         return 1;
   } else {
      /* at least one argument */
      end_time = ss_time();
      start_time = end_time - 3600;
      interval = 1;
      index = 0, index1 = 0, index2 = 0;
      var_type = 0;
      event_id = 0;
      binary_time = FALSE;

      for (i = 1; i < argc; i++) {
         if (argv[i][0] == '-' && argv[i][1] == 'b')
            binary_time = TRUE;
         else if (argv[i][0] == '-' && argv[i][1] == 'l')
            list_query = TRUE;
         else if (argv[i][0] == '-') {
            if (i + 1 >= argc || argv[i + 1][0] == '-')
               goto usage;
            if (strncmp(argv[i], "-e", 2) == 0)
               event_id = atoi(argv[++i]);
            else if (strncmp(argv[i], "-v", 2) == 0)
               strcpy(var_name, argv[++i]);
            else if (strncmp(argv[i], "-i", 2) == 0) {
               if ((column = strchr(argv[++i], ':')) == NULL) {
                  index1 = atoi(argv[i]);
                  index2 = 0;
               } else {
                  *column = 0;
                  index1 = atoi(argv[i]);
                  index2 = atoi(column + 1);
               }
            } else if (strncmp(argv[i], "-h", 2) == 0)
               start_time = ss_time() - atoi(argv[++i]) * 3600;
            else if (strncmp(argv[i], "-d", 2) == 0)
               start_time = ss_time() - atoi(argv[++i]) * 3600 * 24;
            else if (strncmp(argv[i], "-s", 2) == 0) {
               strcpy(start_name, argv[++i]);
               start_time = convert_time(argv[i]);
            } else if (strncmp(argv[i], "-p", 2) == 0)
               end_time = convert_time(argv[++i]);
            else if (strncmp(argv[i], "-t", 2) == 0)
               interval = atoi(argv[++i]);
            else if (strncmp(argv[i], "-f", 2) == 0)
               strcpy(path_name, argv[++i]);
            else if (strncmp(argv[i], "-z", 2) == 0)
               strcpy(path1_name, argv[++i]);
         } else {
          usage:
            printf("\nusage: mhist -e Event ID -v Variable Name\n");
            printf("         [-i Index] index of variables which are arrays\n");
            printf
                ("         [-i Index1:Index2] index range of variables which are arrays (max 50)\n");
            printf
                ("         [-t Interval] minimum interval in sec. between two displayed records\n");
            printf("         [-h Hours] display between some hours ago and now\n");
            printf("         [-d Days] display between some days ago and now\n");
            printf("         [-f File] specify history file explicitly\n");
            printf("         [-s Start date] specify start date YYMMDD[.HHMM[SS]]\n");
            printf("         [-p End date] specify end date YYMMDD[.HHMM[SS]]\n");
            printf("         [-l] list available events and variables\n");
            printf("         [-b] display time stamp in decimal format\n");
            printf
                ("         [-z path] path to the location of the history files (def:cwd)\n");
            return 1;
         }
      }
   }

   /*
      -z is needed in case the mhist called by script

      -f /path/file.hst
      -f file.hst -z /path         =>   file_name = file.hst
      path1_name= /path/
      -f /path/file.hst -z path1   =>   file_name = file.hst
      path1_name = path1
    */
   p = strrchr(path_name, DIR_SEPARATOR);
   if (p != NULL) {
      strcpy(file_name, p + 1);
      *(p + 1) = '\0';
      if (path1_name[0] == '\0')
         strcpy(path1_name, path_name);
   } else {
      strcpy(file_name, path_name);
      path_name[0] = '\0';
   }

   /* Set path */
   if (path1_name[0])
      hs_set_path(path1_name);

   /* -l listing only */
   if (list_query) {
      /* Overwrite the file_name if -s is given with -l only
         in order to produce the -l on the specified date */
      if (start_name[0] != 0)
         strcpy(file_name, start_name);

      display_vars(file_name);
   }

   /* -f given takes -e -b */
   else if (file_name[0])
      hs_fdump(file_name, event_id, binary_time);

   /* -v given takes -e -s -p -t -b */
   else if (var_name[0] == 0)
      hs_dump(event_id, start_time, end_time, interval, binary_time);

   /* Interactive variable display */
   else if (index2 == 0)
      display_single_hist(event_id, start_time, end_time, interval, var_name, index1);
   else
      display_range_hist(event_id, start_time, end_time,
                         interval, var_name, index1, index2);

   return 0;
}
