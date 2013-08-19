/********************************************************************\

  Name:         odb_test.c
  Created by:   Stefan Ritt

  Contents:     Small demo program to show how to connect to the ODB

  $Log: odb_test.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.4  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.3  2002/05/08 20:51:41  midas
  Added extra parameter to function db_get_value()

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "midas.h"

int run_number;

void run_number_changed(HNDLE hDB, HNDLE hKey, void *info)
{
   printf("New run number: %d\n", run_number);
}

main()
{
   int status, size;
   HNDLE hDB, hKey;

   /* connect to experiment */
   status = cm_connect_experiment("", "", "ODBTest", NULL);
   if (status != CM_SUCCESS)
      return 1;

   /* get handle to online database */
   cm_get_experiment_database(&hDB, &hKey);

   /* read key "runinfo/run number" */
   size = sizeof(run_number);
   status =
       db_get_value(hDB, 0, "/runinfo/run number", &run_number, &size, TID_INT, TRUE);
   if (status != DB_SUCCESS) {
      printf("Cannot read run number");
      return 0;
   }

   printf("Current run number is %d\n", run_number);

   /* set new run number */
   run_number++;
   db_set_value(hDB, 0, "/runinfo/run number", &run_number, size, 1, TID_INT);


   /* now open run_number with automatic updates */
   db_find_key(hDB, 0, "/runinfo/run number", &hKey);
   db_open_record(hDB, hKey, &run_number, sizeof(run_number),
                  MODE_READ, run_number_changed, NULL);

   printf("Waiting for run number to change. Hit RETURN to abort\n");

   do {
      cm_yield(1000);
   } while (!ss_kbhit());

   db_close_record(hDB, hKey);

   /* disconnect from experiment */
   cm_disconnect_experiment();

   return 1;
}
