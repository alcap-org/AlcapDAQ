/********************************************************************\

  Name:         minife.c
  Created by:   Stefan Ritt

  Contents:     A "Mini-Frontend" program showing the basic concept
                of frontend in the MIDAS system

  $Log: minife.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

#include "midas.h"
#include "msystem.h"
#include <stdio.h>

/*------------------------------------------------------------------*/

INT running;                    /* stopped:0, started:1 */
INT serial = 0;                 /* event serial number  */
INT run_number = 0;             /* run number */

HNDLE hBuf;                     /* buffer handle for events */

INT tr_start(INT rn, char *error)
{
   printf("Start run %d\n", rn);

   serial = 0;
   run_number = rn;
   running = TRUE;

   return CM_SUCCESS;
}

INT tr_stop(INT rn, char *error)
{
   printf("Stop run %d -", rn);

   running = FALSE;

   rpc_flush_event();           /* send events in TCP cache */
   bm_flush_cache(hBuf, SYNC);  /* flush buffer cache */

   printf(" done\n");

   return CM_SUCCESS;
}

/*------------------------------------------------------------------*/

#ifdef OS_VXWORKS
minife(char *ahost_name)
#else
main()
#endif
{
   INT status, size, *pdata, id;
   INT convert_flags;
   DWORD last_time = 0;
   char event[2000];
   char host_name[256];
   char dash[] = "-/|\\";
   BOOL blocked = FALSE;

#ifdef OS_VXWORKS
   strcpy(host_name, ahost_name);
#else
   printf("Host to connect: ");
   ss_gets(host_name, 256);
#endif

   /* now connect to server */
   status = cm_connect_experiment(host_name, "", "MiniFE", NULL);
   if (status != CM_SUCCESS)
      return 1;

   if (cm_register_transition(TR_START, tr_start) != CM_SUCCESS ||
       cm_register_transition(TR_STOP, tr_stop) != CM_SUCCESS) {
      printf("Cannot start server");
      return 0;
   }
#ifdef _DEBUG
   cm_set_watchdog_params(TRUE, 0);
#endif

   /* open the "system" buffer, 1M size */
   bm_open_buffer(EVENT_BUFFER_NAME, EVENT_BUFFER_SIZE, &hBuf);

   /* set the buffer cache size on the server */
   bm_set_cache_size(hBuf, 0, 100000);

   pdata = (INT *) (event + sizeof(EVENT_HEADER));
   size = 1000;

   /* compose an event */
   bm_compose_event((EVENT_HEADER *) event, 1, 1, size, serial);

   pdata[0] = size;             /* simulate data for consume.exe */
   pdata[size / 4 - 1] = size;

   /* convert test data if necessary to format on server */
   rpc_get_convert_flags(&convert_flags);
   if (convert_flags) {
      rpc_convert_single(&pdata[0], TID_DWORD, RPC_OUTGOING, convert_flags);
      rpc_convert_single(&pdata[size / 4 - 1], TID_DWORD, RPC_OUTGOING, convert_flags);
   }

   do {
      if (!running) {
         /* if run is not started, call yield with a timeout of 1s */
         status = cm_yield(1000);
         if (status == RPC_SHUTDOWN || status == SS_ABORT)
            break;
      } else {
         /* if run is started, call yield with a timeout of 0 once each second.
            This minimizes the calls to select() inside rpc_server_yield and
            increases the network speed */
         if (ss_millitime() - last_time > 300) {
            printf("%c\r", dash[id++ % 4]);
            fflush(stdout);

            status = cm_yield(0);
            if (status == RPC_SHUTDOWN || status == SS_ABORT)
               break;
            last_time = ss_millitime();
         }
      }

      /* if run is started, send event */
      if (running) {
         /* only compose new event if not blocked by previous send */
         if (!blocked) {
            /* increment serial number */
            ((EVENT_HEADER *) event)->serial_number = serial++;
         }

         status = rpc_send_event(hBuf, event, size + sizeof(EVENT_HEADER), ASYNC);
         if (status == BM_ASYNC_RETURN)
            blocked = TRUE;
         else if (status == BM_SUCCESS)
            blocked = FALSE;
         else if (status != BM_SUCCESS) {
            printf("Error sending event");
            goto error;
         }

      }

   } while (run_number < 1000);

   cm_disconnect_experiment();

 error:

   return 1;
}
