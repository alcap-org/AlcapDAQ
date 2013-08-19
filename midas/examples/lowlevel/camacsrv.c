/********************************************************************\

  Name:         camacsrvr.c
  Created by:   Stefan Ritt

  Contents:     CAMAC RPC server program

  $Log: camacsrv.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include "midas.h"
#include "msystem.h"
#include "mcstd.h"

int debug;

/*------------------------------------------------------------------*/

INT cnaf16(DWORD cmd, DWORD b, DWORD c, DWORD n, DWORD a, DWORD f,
           WORD * pdata, DWORD * size, DWORD * x, DWORD * q)
{
   INT i, count;
   WORD *ptmp;

   /* determine repeat count */
   count = *size / sizeof(WORD);        /* 16 bit */

   switch (cmd) {
    /*---- special commands ----*/

   case CNAF_INHIBIT_SET:
      cam_inhibit_set(c);
      break;
   case CNAF_INHIBIT_CLEAR:
      cam_inhibit_clear(c);
      break;
   case CNAF_CRATE_CLEAR:
      cam_crate_clear(c);
      break;
   case CNAF_CRATE_ZINIT:
      cam_crate_zinit(c);
      break;

   case CNAF_TEST:
      break;

    /*---- 16 bit ----*/

   case CNAF:
      for (i = 0; i < count; i++)
         if (f < 16)
            cam16i_q(c, n, a, f, pdata, (int *) x, (int *) q);
         else
            cam16o_q(c, n, a, f, pdata[i], (int *) x, (int *) q);
      break;

   case CNAF_nQ:
      ptmp = pdata;
      if (f < 16)
         cam16i_rq(c, n, a, f, &ptmp, count);

      /* return reduced return size */
      *size = (int) ptmp - (int) pdata;
      break;

   default:
      printf("cnaf: Unknown command 0x%X\n", cmd);
   }

   if (debug)
      printf("cmd=%d c=%d n=%d a=%d f=%d d=%X x=%d q=%d\n", cmd, c, n, a, f, *pdata, *x,
             *q);

   return RPC_SUCCESS;
}

/*------------------------------------------------------------------*/

INT cnaf24(DWORD cmd, DWORD b, DWORD c, DWORD n, DWORD a, DWORD f,
           DWORD * pdata, DWORD * size, DWORD * x, DWORD * q)
{
   INT i, count;
   DWORD *ptmp;

   /* determine repeat count */
   count = *size / sizeof(DWORD);       /* 24 bit */

   switch (cmd) {
   case CNAF:
      for (i = 0; i < count; i++)
         if (f < 16)
            cam24i_q(c, n, a, f, pdata, (int *) x, (int *) q);
         else
            cam24o_q(c, n, a, f, pdata[i], (int *) x, (int *) q);
      break;

   case CNAF_nQ:
      ptmp = pdata;
      if (f < 16)
         cam24i_rq(c, n, a, f, &ptmp, count);

      /* return reduced return size */
      *size = (int) ptmp - (int) pdata;
      break;

   default:
      printf("cnaf: Unknown command 0x%X\n", cmd);
   }

   if (debug)
      printf("cmd=%d c=%d n=%d a=%d f=%d d=%X x=%d q=%d\n", cmd, c, n, a, f, *pdata, *x,
             *q);

   return RPC_SUCCESS;
}

/*------------------------------------------------------------------*/

INT rpc_dispatch(INT index, void *prpc_param[])
{
   INT status;

   switch (index) {
   case RPC_CNAF16:
      status = cnaf16(CDWORD(0), CDWORD(1), CDWORD(2), CDWORD(3), CDWORD(4),
                      CDWORD(5), CPWORD(6), CPDWORD(7), CPDWORD(8), CPDWORD(9));
      break;

   case RPC_CNAF24:
      status = cnaf24(CDWORD(0), CDWORD(1), CDWORD(2), CDWORD(3), CDWORD(4),
                      CDWORD(5), CPDWORD(6), CPDWORD(7), CPDWORD(8), CPDWORD(9));
      break;

   default:
      cm_msg(MERROR, "rpc_dispatch", "received unrecognized command");
   }

   return status;
}

/*------------------------------------------------------------------*/

main(int argc, char *argv[])
{
   INT status, port;

   port = 1750;
   status = rpc_register_server(ST_REMOTE, "", &port, NULL);
   if (status != RPC_SUCCESS) {
      printf("Cannot start server\n");
      return 1;
   }
   if (argc > 1) {
      debug = 1;
      printf("CAMAC server started (debugging mode)\n");
   } else {
      debug = 0;
      printf("CAMAC server started\n");
   }

   rpc_register_functions(rpc_get_internal_list(0), NULL);
   rpc_register_functions(rpc_get_internal_list(1), NULL);
   rpc_register_function(RPC_CNAF16, rpc_dispatch);
   rpc_register_function(RPC_CNAF24, rpc_dispatch);

   cam_init();

   while (cm_yield(1000) != RPC_SHUTDOWN);

   rpc_server_shutdown();

   return 0;
}
