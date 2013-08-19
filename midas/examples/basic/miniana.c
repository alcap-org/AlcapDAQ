/********************************************************************\

  Name:         miniana.c
  Created by:   Stefan Ritt

  Contents:     Minimum analyzer program to receive an event

  $Log: miniana.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include "midas.h"


void process_event(HNDLE hBuf, HNDLE request_id, EVENT_HEADER * pheader, void *pevent)
{
   printf("Received event #%d\r", pheader->serial_number);
}

main()
{
   INT status, request_id;
   HNDLE hBufEvent;

   status = cm_connect_experiment("", "sample", "Simple Analyzer", NULL);
   if (status != CM_SUCCESS)
      return 1;

   bm_open_buffer("SYSTEM", EVENT_BUFFER_SIZE, &hBufEvent);
   bm_request_event(hBufEvent, 1, TRIGGER_ALL, GET_ALL, &request_id, process_event);

   do {
      status = cm_yield(1000);
   } while (status != RPC_SHUTDOWN && status != SS_ABORT);

   cm_disconnect_experiment();

   return 0;
}
