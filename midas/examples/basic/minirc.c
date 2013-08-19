/********************************************************************\

  Name:         minirc.c
  Created by:   Stefan Ritt

  Contents:     A "Mini-Run Control" program showing the basic concept
                of starting/stopping runs in the MIDAS system

  $Log: minirc.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "midas.h"

/*------------------------------------------------------------------*/

main()
{
   INT run_number, status;
   char host_name[256];
   char str[256];

   printf("Host to connect: ");
   ss_gets(host_name, 256);

   /* connect to experiment */
   status = cm_connect_experiment(host_name, "", "MiniRC", NULL);
   if (status != CM_SUCCESS)
      return 1;

   printf("Enter run number: ");
   ss_gets(str, 256);
   run_number = atoi(str);

   printf("Start run\n");

   /* start run */
   if (cm_transition(TR_START, run_number, str, sizeof(str), SYNC) != CM_SUCCESS)
      printf(str);

   printf("Hit RETURN to stop run");
   getchar();

   /* stop run */
   if (cm_transition(TR_STOP, run_number, str, sizeof(str), SYNC) != CM_SUCCESS)
      printf(str);

   cm_disconnect_experiment();

   return 1;
}
