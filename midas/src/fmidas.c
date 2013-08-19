/********************************************************************\

  Name:         FMIDAS.C
  Created by:   Stefan Ritt

  Contents:     Wrapper functions for MIDAS library functions to be
                called from Fortran programs.

  $Log: fmidas.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:30  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.2  1998/10/12 12:19:01  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <midas.h>

/*---- Windows NT section ------------------------------------------*/

#ifdef OS_WINNT

int __stdcall CM_CONNECT_EXPERIMENT(char *fhost, int lhost,
                                    char *fexp, int lexp, char *fname, int lname)
{
   char host[256], exp[256], name[256];

   strncpy(host, fhost, lhost);
   host[lhost] = 0;

   strncpy(exp, fexp, lexp);
   exp[lexp] = 0;

   strncpy(name, fname, lname);
   name[lname] = 0;

   return cm_connect_experiment(host, exp, name, NULL);
}

int __stdcall CM_DISCONNECT_EXPERIMENT()
{
   return cm_disconnect_experiment();
}

int __stdcall BM_OPEN_BUFFER(char *fname, int lname, int *buffer_size, int *buffer_handle)
{
   char name[256];

   strncpy(name, fname, lname);
   name[lname] = 0;

   return bm_open_buffer(name, *buffer_size, buffer_handle);
}

extern void __stdcall PROCESS_EVENT();

void _process_event(HNDLE hBuf, HNDLE hRequest, EVENT_HEADER * pevent, void *pdata)
{
   PROCESS_EVENT(&hBuf, &hRequest, pevent, pdata);
}

int __stdcall BM_REQUEST_EVENT(int *buffer_handle, int *event_id,
                               int *trigger_mask, int *sampling_type, int *request_id)
{
   return bm_request_event(*buffer_handle,
                           (short int) *event_id,
                           (short int) *trigger_mask,
                           *sampling_type, request_id, _process_event);
}

int __stdcall CM_YIELD(int *millisec)
{
   return cm_yield(*millisec);
}

#endif

/*-----------------------------------------------------------------*/
