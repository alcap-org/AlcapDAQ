/********************************************************************\

  Name:         rpc_srvr.c
  Created by:   Stefan Ritt

  Contents:     A simple RPC server program. Implements a function
                rpc_test, which simply doubles some parameters.
                This function can be calles remotely by rpc_clnt.


  $Log: rpc_srvr.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.2  1999/10/26 11:20:43  midas
  Fixed wrong parameters in rpc_set_debug

  Revision 1.1  1999/04/27 11:10:56  midas
  Added files to CVS


\********************************************************************/

#include "midas.h"
#include <stdio.h>

#define RPC_MYTEST RPC_MIN_ID

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

/* just a small test routine which doubles numbers */
INT rpc_mytest(BYTE b, WORD w, INT i, float f, double d,
               BYTE * b1, WORD * w1, INT * i1, float *f1, double *d1)
{
   *b1 = b * 2;
   *w1 = w * 2;
   *i1 = i * 2;
   *f1 = f * 2;
   *d1 = d * 2;

   return 1;
}

/* this dispatchers forwards RPC calls to the destination functions */
INT rpc_dispatch(INT index, void *prpc_param[])
{
   INT status;

   switch (index) {
   case RPC_MYTEST:
      status = rpc_mytest(CBYTE(0), CWORD(1), CINT(2), CFLOAT(3), CDOUBLE(4),
                          CPBYTE(5), CPWORD(6), CPINT(7), CPFLOAT(8), CPDOUBLE(9));
      break;

   default:
      cm_msg(MERROR, "rpc_dispatch", "received unrecognized command");
   }

   return status;
}

/*------------------------------------------------------------------*/

main()
{
   INT status, port;

   /* register RPC server under port 1750 */
   port = 1750;
   status = rpc_register_server(ST_REMOTE, "", &port, NULL);
   if (status != RPC_SUCCESS) {
      printf("Cannot start server");
      return 0;
   }

   /* Register function list. Calls get forwarded to rpc_dispatch */
   rpc_register_functions(rpc_list, rpc_dispatch);

   /* Print debugging messages */
   rpc_set_debug((void (*)(char *)) puts, 0);

   /* Server loop */
   while (cm_yield(1000) != RPC_SHUTDOWN);

   /* Shutdown server */
   rpc_server_shutdown();

   return 1;
}
