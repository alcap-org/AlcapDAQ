/********************************************************************\

  Name:         odbhist.c
  Created by:   Stefan Ritt, Ilia Chachkhunashvili

  Contents:     MIDAS history display utility

  $Log: odbhist.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:38  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.8  2004/01/08 08:40:11  midas
  Implemented standard indentation

  Revision 1.7  2003/04/23 23:53:41  pierre
  Fixed compiler warning

  Revision 1.6  2000/07/24 14:27:34  midas
  Did some cosmetics

  Revision 1.5  2000/06/15 09:49:07  midas
  Added Ilia's changes and additions (mainly configuration file)

  Revision 1.4  1999/08/27 07:59:53  midas
  Fixed bug

  Revision 1.2  1999/08/24 14:46:44  midas
  Added -a flag and indexed variables

  Revision 1.1  1999/08/23 15:22:39  midas
  Added file


\********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*------------------------------------------------------------------*/

#define SUCCESS 1
#define TRUE    1
#define FALSE   0

typedef unsigned int DWORD;
typedef int INT;
typedef int BOOL;
typedef char str[256];

/*------------------------------------------------------------------*/

str var_params[100];            /* varables to print 100 each of 256 char lenth */

DWORD status, start_run, end_run, run;
INT i, quiet, add, j, k, eoln, boln, weight;
char file_name[256], var_name[256];
double total[100], value[100];
str e_var1, e_var2, e_var3;
double var1, var2, var3, sum, sum_var1;

/*------------------------------------------------------------------*/

BOOL equal_ustring(char *str1, char *str2)
{
   if (str1 == NULL && str2 != NULL)
      return FALSE;
   if (str1 != NULL && str2 == NULL)
      return FALSE;
   if (str1 == NULL && str2 == NULL)
      return TRUE;

   while (*str1)
      if (toupper(*str1++) != toupper(*str2++))
         return FALSE;

   if (*str2)
      return FALSE;

   return TRUE;
}

/*------------------------------------------------------------------*/

int odb_hist(char *file_name, int run_number, char *var_name, int quiet,
             double *value, int eoln, int boln, int print)
/********************************************************************\

  Routine: odb_hist

  Purpose: Print ODB variable for a single .odb file

  Input:
    char *file_name         Name of ODB file
    INT  run_number         Run number for ODB file
    char *var_name          Name of variable to print
    int  quiet              If set, don't print run number
    int eoln                end of line used to detemine when to print "\n"
    int boln                begin of line used to determin when to print 
                            run number if it's needed
    int print               used to print or not the value of variable. 
                            This it's used for "Emils' function", so it 
                            will take and return the value of variable 
                            without printing it.
    
  Note:    
    There are two variables eoln and boln because there can be begin 
    and end of line together when we have just one variable. 

  Output:
    double *value           ODB value in double format

  Function value:
    SUCCESS                 Successful completion

\********************************************************************/
{
   FILE *f;
   char str[256], path[256], key_name[256], line[256];
   int i, index, a, k;

   /* assemble file name */
   sprintf(str, file_name, run_number);

   /* split var_name into path and key with index */
   strcpy(path, "");
   for (i = strlen(var_name) - 1; i >= 0 && var_name[i] != '/'; i--);
   if (var_name[i] == '/') {
      strcpy(path, "[");
      strcat(path, var_name);
      path[i + 1] = 0;
      strcat(path, "]\n");
      strcpy(key_name, var_name + i + 1);
   }
   if (strchr(key_name, '[')) {
      index = atoi(strchr(key_name, '[') + 1);
      *strchr(key_name, '[') = 0;
   } else
      index = -1;

   f = fopen(str, "r");
   if (f != NULL) {
      if ((!quiet) && boln && print)
         printf("%5d: ", run_number);

      /* search path */
      do {
         fgets(line, sizeof(line), f);
         if (line[0] == '[')
            if (equal_ustring(line, path)) {
               /* look for key */
               do {
                  fgets(line, sizeof(line), f);
                  if (strchr(line, '=') != NULL) {
                     strcpy(str, line);
                     *(strchr(str, '=') - 1) = 0;

                     /* check if key name matches */
                     if (equal_ustring(str, key_name)) {
                        if (index == -1) {
                           /* non-arrays */
                           strcpy(str, strchr(line, '=') + 2);
                           if (strchr(str, ':') != NULL) {
                              strcpy(str, strchr(str, ':') + 2);
                              if (strchr(str, '\n') != NULL)
                                 *strchr(str, '\n') = 0;
                              if (str[0] == '[' && strchr(str, ']') != NULL)
                                 strcpy(str, strchr(str, ']') + 2);
                              if (print)
                                 printf(str);
                              *value = strtod(str, NULL);
                              goto finish;
                           }
                        } else {
                           /* arrays */
                           for (i = 0; i <= index; i++)
                              fgets(line, sizeof(line), f);
                           if (line[0] == '[' && atoi(line + 1) == index) {
                              strcpy(str, strchr(line, ']') + 2);
                              if (strchr(str, '\n') != NULL)
                                 *strchr(str, '\n') = 0;
                              if (print)
                                 printf(str);
                              *value = strtod(str, NULL);
                           }
                           goto finish;
                        }

                     }
                  }

               } while (line[0] != '[' || line[1] != '/');

            }
      } while (!feof(f));

    finish:
      if (print) {
         if (eoln)
            printf("\n");
         else {
            a = 0;
            while (str[a] != '\0')
               a++;
            k = a;
            while ((str[k] != '.') && (k >= 0))
               k--;
            for (i = 0; i < (10 - (a - k)); i++)
               printf(" ");
            printf("\t");
         }
         fclose(f);
      }
   }
   /* f != NULL */
   return SUCCESS;
}

/*------------------------------------------------------------------*/

int load_pars_from_file(char filename[256])
/********************************************************************\

  Routine: load_pars_from_file

  Purpose: Load parameters for odbhist from file

  Input:
    char *filename          Name of configuration file

  Output:
    <implicit through global variables>

  Function value:
    1                       Successful completion
    0                       Error

\********************************************************************/
{
   FILE *f1;
   char line[256];
   int result;
   int getstr;

   getstr = 1;

   f1 = fopen(filename, "r");
   if (f1 != NULL) {
      result = 1;
      while (result != 0) {
         if (getstr) {
            if (fgets(line, sizeof(line), f1) == NULL)
               break;
         } else
            getstr = 1;

         if (line[0] == '[') {
            switch (line[1]) {
            case 'a':
               if (fgets(line, sizeof(line), f1) != NULL) {
                  switch (line[0]) {
                  case '1':
                     add = TRUE;
                     break;
                  case '0':
                     add = FALSE;
                     break;
                  default:
                     result = 0;
                  }
               } else
                  result = 0;
               break;

            case 'q':
               if (fgets(line, sizeof(line), f1) != NULL) {
                  switch (line[0]) {
                  case '1':
                     quiet = TRUE;
                     break;
                  case '0':
                     quiet = FALSE;
                     break;
                  default:
                     result = 0;
                  }
               } else
                  result = 0;
               break;

            case 'f':
               if ((fgets(line, sizeof(line), f1) != NULL) && (line[0] != '['))
                  strcpy(file_name, line);
               else
                  result = 0;
               break;

            case 'v':
               j = -1;
               while (fgets(line, sizeof(line), f1) != NULL && line[0] != '[') {
                  if (line[0] != '\n')
                     strcpy(var_params[++j], line);
               }
               if (j == -1)
                  result = 0;
               else
                  getstr = 0;

               /* to get correct number of variables in "j" global variable */
               if (line[0] != '[')
                  j--;
               break;

            case 'e':
               weight = 1;

               if ((fgets(line, sizeof(line), f1) != NULL) && (line[0] != '['))
                  strcpy(e_var1, line);
               else {
                  result = 0;
                  break;
               }

               if ((fgets(line, sizeof(line), f1) != NULL) && (line[0] != '['))
                  strcpy(e_var2, line);
               else {
                  result = 0;
                  break;
               }

               if ((fgets(line, sizeof(line), f1) != NULL) && (line[0] != '['))
                  strcpy(e_var3, line);
               else {
                  result = 0;
                  break;
               }
               break;

            case 'r':
               if ((fgets(line, sizeof(line), f1) != NULL) && (line[0] != '['))
                  start_run = atoi(line);
               else {
                  result = 0;
                  break;
               }
               if ((fgets(line, sizeof(line), f1) != NULL) && (line[0] != '['))
                  end_run = atoi(line);
               else {
                  result = 0;
                  break;
               }

               break;

            default:
               result = 0;
            }
         }
      }                         /* while */
   } /* if */
   else {
      result = 0;
      printf("\n ERROR:\nCan't open file %s\n", filename);
   }
   if (result != 0)
      if (fclose(f1))
         result = 0;

   return result;
}

/*------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
   int cfg, print;

   strcpy(var_name, "/Runinfo/Run number");
   strcpy(file_name, "run%05d.odb");
   start_run = end_run = 0;
   quiet = FALSE;
   add = FALSE;
   print = 1;                   /* print = 1 means that variable will be printed */

   k = 0;
   cfg = 0;
   j = -1;

   /* parse command line parameters */

   for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-')
         if (argv[i][1] == 'c') {
            printf(argv[i + 1]);
            printf("\n");
            if (!(load_pars_from_file(argv[i + 1])))
               goto usage;
            else
               cfg = 1;
         }
   }

   for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
         if (argv[i][1] == 'q')
            quiet = TRUE;
         else if (argv[i][1] == 'a')
            add = TRUE;
         else {
            if (i + 1 >= argc || argv[i + 1][0] == '-')
               goto usage;
            if (argv[i][1] == 'r') {
               start_run = atoi(argv[++i]);
               end_run = atoi(argv[++i]);
            } else if (argv[i][1] == 'v') {
               j = -1;          /*if this not used the parameters from here will be add to "v" parameters from cfg file */
               while (i + 1 < argc && argv[i + 1][0] != '-')
                  if (argv[i + 1][0] != '-') {
                     i++;
                     j++;
                     if (argv[i][0] != '-')
                        strcpy(var_params[j], argv[i]);
                  }
            } else if (argv[i][1] == 'f')
               strcpy(file_name, argv[++i]);
            else if (argv[i][1] == 'e') {
               /* user must be carefull here, he must enter all 3 params */
               strcpy(e_var1, argv[++i]);
               strcpy(e_var2, argv[++i]);
               strcpy(e_var3, argv[++i]);
               weight = 1;
            } else if (cfg);
            else
               goto usage;
         }
      } else if (!cfg) {

       usage:
         printf("\nusage: odbhist -r <run1> <run2> -v <varname>[index]\n");
         printf("       [-f <filename>] [-q] [-a] [-c <file>]\n");
         printf("       [-e <var1> <var2> <var3>]\n\n");
         printf("       <run1> <run2>    Range of run numbers (inclusive)\n");
         printf
             ("       <varname>        ODB variable name like \"/Runinfo/Run number\"\n");
         printf("       [index]          Index if <varname> is an array\n");
         printf("       -q               Don't display run number\n");
         printf("       -a               Add numbers for all runs\n");
         printf("       -c               load configuration from file\n");
         printf("                        (parameters loaded from cfg file will be\n");
         printf("                        overwriten by parameters from command line)\n");
         printf("       -e  <var1> <var2> <var3>\n");
         printf("                        this parameter will return\n");
         printf("                        sum(var1*var2/var3)/sum(var1)\n");
         printf("                        summing will be done over all runs\n");
         return 0;

      }
   }

   if (end_run < start_run) {
      printf("Second run is %d and must be larger or equal than first run %d\n",
             end_run, start_run);
      return 0;
   }

   /* printing of header is needed here */
   for (run = start_run; run <= end_run; run++) {
      for (k = 0; k <= j; k++) {
         if (k == j)
            eoln = 1;
         else
            eoln = 0;

         if (k == 0)
            boln = 1;
         else
            boln = 0;

         strcpy(var_name, var_params[k]);
         value[k] = 0;

         status = odb_hist(file_name, run, var_name, quiet, &value[k], eoln, boln, print);
         if (status != SUCCESS)
            break;

         total[k] += value[k];
      }                         /*for k */

      if (weight) {
         strcpy(var_name, e_var1);
         var1 = 0;
         status = odb_hist(file_name, run, var_name, quiet, &var1, 0, 0, 0);
         if (status != SUCCESS) {
            printf("error in -e parameter var1 \n");
            goto usage;
         }

         strcpy(var_name, e_var2);
         var2 = 0;
         status = odb_hist(file_name, run, var_name, quiet, &var2, 0, 0, 0);
         if (status != SUCCESS) {
            printf("error in -e parameter var2\n");
            goto usage;
         }

         strcpy(var_name, e_var3);
         var3 = 0;
         status = odb_hist(file_name, run, var_name, quiet, &var3, 0, 0, 0);
         if (status != SUCCESS) {
            printf("error in -e parameter var3\n");
            goto usage;
         }

         sum += var1 * var2 / var3;
         sum_var1 += var1;
      }                         /* if weight */
   }                            /* for run */

   if (add) {
      printf("\nTotal: ");
      if (quiet)
         printf("\n");
      for (k = 0; k <= j; k++)
         printf("%lf\t", total[k]);
      printf("\n");
   }

   if (weight) {
      printf("weighted sum is %lf\n", sum / sum_var1);
      printf("sum of var1 is %lf\n", sum_var1);
   }

   return 1;
}
