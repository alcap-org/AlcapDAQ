/********************************************************************\

  Name:         rpc_clnt.c
  Created by:   Stefan Ritt

  Contents:     Mini RPC client program for test purposes.

  $Log: rpc_clnt.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.4  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.3  2003/05/09 10:03:14  midas
  Added missing argument in rpc_connect_client()

  Revision 1.2  1999/10/26 12:00:57  midas
  Replaced gets by ss_gets

  Revision 1.1  1999/04/27 11:10:56  midas
  Added files to CVS


\********************************************************************/

#include "midas.h"
#include <stdio.h>

/* user RPC ID's must start after RPC_MIN_ID */
#define RPC_MYTEST RPC_MIN_ID

/* define MYTEST function with some parameters, 
   must match definition on server */
RPC_LIST rpc_list[] = {

   {RPC_MYTEST, "rpc_mytest",
    {{TID_BYTE, RPC_IN}
     ,
     {TID_WORD, RPC_IN}
     ,
     {TID_INT, RPC_IN}
     ,
     {TID_FLOAT, RPC_IN}
     ,
     {TID_DOUBLE, RPC_IN}
     ,
     {TID_BYTE, RPC_OUT}
     ,
     {TID_WORD, RPC_OUT}
     ,
     {TID_INT, RPC_OUT}
     ,
     {TID_FLOAT, RPC_OUT}
     ,
     {TID_DOUBLE, RPC_OUT}
     ,
     {0}
     }
    }
   ,

   {0}

};

/*------------------------------------------------------------------*/

main()
{
   char host_name[80];
   HNDLE hConn;
   INT status;
   BYTE b;
   WORD w;
   INT i;
   float f;
   double d;

   printf("Remote host name: ");
   ss_gets(host_name, sizeof(host_name));

   /* register this as an RPC client with rpc_list */
   rpc_register_client("MYCLIENT", rpc_list);

   /* connect to RPC server */
   status = rpc_client_connect(host_name, 1750, "", &hConn);
   if (status != RPC_SUCCESS) {
      printf("Cannot connect to RPC server running on %s at port 1750.\n", host_name);
      return 0;
   }

   f = 3.5f;
   d = 4.5;

   /* rpc_mytest just doubles numbers */
   rpc_client_call(hConn, RPC_MYTEST, 1, 2, 3l, f, d, &b, &w, &i, &f, &d);

   printf("\nResult should be:  2 4 6 7.0 9.0\n");
   printf("Result is:         %d %d %ld %1.1f %1.1lf\n", b, w, i, f, d);

   printf("\nhit return...");
   b = getchar();

   /* disconnect this client from server */
   rpc_client_disconnect(hConn, FALSE);
   return 1;
}
