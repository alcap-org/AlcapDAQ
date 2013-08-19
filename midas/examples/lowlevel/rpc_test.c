/********************************************************************\

  Name:         rpc_test.c
  Created by:   Stefan Ritt

  Contents:     Mini RPC client for testing purposes

                This program connects to a server running the MIDAS
                main server. It calles the routine rpc_test on this
                server, which simply doubles some arguments. By
                receiving the proper results, one can check that the
                parameter passing via MIDAS RPCs works correctly.

  $Log: rpc_test.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.2  1998/10/12 12:18:59  midas
  Added Log tag in header


\********************************************************************/

#include "midas.h"
#include "mrpc.h"
#include <stdio.h>

/*------------------------------------------------------------------*/

#ifdef OS_VXWORKS
rpc_test(char *ahost_name)
#else
main()
#endif
{
   char host_name[256];
   INT status;
   BYTE b;
   WORD w;
   INT i;
   float f;
   double d;

#ifdef OS_VXWORKS
   strcpy(host_name, ahost_name);
#else
   printf("Remote host IP address: ");
   ss_gets(host_name, 256);
#endif

   status = cm_connect_experiment(host_name, "", "RPC_TEST", NULL);
   if (status != CM_SUCCESS)
      return 1;

   f = 3.5f;
   d = 4.5;

   /* rpc_test just doubles numbers */
   rpc_call(RPC_TEST, 1, 2, 3l, f, d, &b, &w, &i, &f, &d);

   printf("\nResult should be:  2 4 6 7.0 9.0\n");
   printf("Result is:         %d %d %ld %1.1f %1.1lf\n", b, w, i, f, d);

   printf("\nhit return...");
   b = getchar();

   cm_disconnect_experiment();
   return 1;
}
