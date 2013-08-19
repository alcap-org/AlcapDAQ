/********************************************************************\

  Name:         produce.c
  Created by:   Stefan Ritt

  Contents:     Buffer manager test program. Simple producer connec-
                ting to a SYSTEM buffer and sending some data.

  $Log: produce.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.8  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.7  1999/11/23 11:48:40  midas
  Replaced random() by rand(), since random doesn't exist under NT

  Revision 1.6  1999/11/09 13:22:12  midas
  Changed event size slightly to be above 10

  Revision 1.5  1999/10/26 12:26:05  midas
  Added variable event size for test purposes

  Revision 1.4  1999/08/06 15:10:32  midas
  Release 1.6.4

  Revision 1.3  1999/04/30 13:19:53  midas
  Changed inter-process communication (ss_resume, bm_notify_clients, etc)
  to strings so that server process can receive it's own watchdog produced
  messages (pass buffer name insteas buffer handle)

  Revision 1.2  1998/10/12 12:18:59  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "midas.h"

/*------------------------------------------------------------------*/

#ifdef OS_VXWORKS
produce()
#else
main()
#endif
{
   INT hBuf, status, i;
   char *event, str[256];
   INT *pdata, count;
   INT start, stop;
   double rate;
   int id, size, act_size, variable_size, flush = 0;
   char host_name[256];

   /* get parameters */

   printf("ID of event to produce: ");
   ss_gets(str, 256);
   id = atoi(str);

   printf("Host to connect: ");
   ss_gets(host_name, 256);

   printf("Event size: ");
   ss_gets(str, 256);
   size = atoi(str);
   if (size < 0) {
      variable_size = 1;
      size = -size;
   } else
      variable_size = 0;

   /* connect to experiment */
   status = cm_connect_experiment(host_name, "", "Producer", NULL);
   if (status != CM_SUCCESS)
      return 1;

   /* open the event buffer with default size */
   bm_open_buffer(EVENT_BUFFER_NAME, EVENT_BUFFER_SIZE, &hBuf);

   /* set the buffer write cache size */
   bm_set_cache_size(hBuf, 0, 100000);

   /* allocate event buffer */
   event = (char *) malloc(size + sizeof(EVENT_HEADER));
   memset(event, 0, size + sizeof(EVENT_HEADER));
   if (event == NULL) {
      printf("Not enough memory for event buffer\n");
      goto error;
   }
   pdata = (INT *) (event + sizeof(EVENT_HEADER));

   do {
      start = ss_millitime();
      count = 0;

      do {
         for (i = 0; i < 10; i++) {
            if (variable_size)
               act_size = (rand() % (size - 10)) + 10;
            else
               act_size = size;

            /* place the event size in the first and last word of
               the event to check later if data has been overwritten
               accidentally */
            pdata[0] = act_size;
            pdata[act_size / 4 - 1] = act_size;

            /* compose an event header with serial number */
            bm_compose_event((EVENT_HEADER *) event, (short) id, 1,
                             act_size, ((EVENT_HEADER *) (event))->serial_number + 1);

            if (act_size < 0)
               printf("Error: act_size = %d, size = %d\n", act_size, size);

            /* now send event */
            status = rpc_send_event(hBuf, event, act_size + sizeof(EVENT_HEADER), SYNC);
            /* status = bm_send_event(hBuf, event, 
               act_size+sizeof(EVENT_HEADER), SYNC); */

            if (status != BM_SUCCESS) {
               printf("rpc_send_event returned error %d, event_size %d\n",
                      status, act_size);
               goto error;
            }

/*
        printf(".");
        getchar();
*/

            count += act_size;
         }

         /* repeat this loop for 1s */
      } while (ss_millitime() - start < 1000);

      /* Now calculate the rate */
      stop = ss_millitime();
      if (stop != start)
         rate = count / 1024.0 / 1024.0 / (stop / 1000.0 - start / 1000.0);
      else
         rate = 0;

      printf("Rate: %1.2lf MB/sec\n", rate);

      /* flush buffers every 10 seconds */
      if ((flush++) % 10 == 0) {
         rpc_flush_event();
         bm_flush_cache(hBuf, SYNC);
         printf("flush\n");
      }

      status = cm_yield(0);

   } while (status != RPC_SHUTDOWN && status != SS_ABORT);

 error:

   cm_disconnect_experiment();
   return 1;
}
