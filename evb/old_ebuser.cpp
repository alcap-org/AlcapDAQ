/********************************************************************\

  Name:         ebuser.c
  Created by:   Pierre-Andre Amaudruz

  Contents:     User section for the Event builder

  $Log: ebuser.cpp,v $
  Revision 1.2  2004/05/31 14:26:34  mucap
  Commented out compression.  (Leftover uncommitted change from run7)

  Revision 1.1  2003/07/17 18:52:54  fegray
  Converted eventbuilder to C++.

  Revision 1.3  2003/05/13 19:27:04  mucap
  Fixes to allow a subset of the crates to participate in an event.  Also,
  this pulls in Pierre's CVS changes to the generic eventbuilder code.

  Revision 1.2  2003/05/13 17:02:47  fegray
  Changes to allow a subset of the crates to run.

  Revision 1.1  2003/05/13 16:21:16  mucap
  First commit of MIDAS (Ethernet-based) event-builder.

  Revision 1.6  2002/09/28 00:49:08  pierre
  Add EB_USER_ERROR example

  Revision 1.5  2002/09/25 18:38:03  pierre
  correct: header passing, user field, abort run

  Revision 1.4  2002/07/13 05:46:10  pierre
  added ybos comments

  Revision 1.3  2002/06/14 04:59:46  pierre
  revised for ybos

  Revision 1.2  2002/01/17 23:34:29  pierre
  doc++ format

  Revision 1.1.1.1  2002/01/17 19:49:54  pierre
  Initial Version

\********************************************************************/

#include <stdio.h>
#include "midas.h"
#include "mevb.h"
#include "ybos.h"

#include "mucap_compress.h"

#define NUMCRATES 5

INT eb_begin_of_run(INT, char *, char *);
INT eb_end_of_run(INT, char *);
INT ebuser(INT, EBUILDER_CHANNEL *, EVENT_HEADER *, void *, INT *);

/* Globals */
INT  lModulo = 100;

/*--------------------------------------------------------------------*/
/** eb_begin_of_run()
Hook to the event builder task at PreStart transition.
\begin{verbatim}
\end{verbatim}

@param rn run number
@param UserField argument from /Ebuilder/Settings
@param error error string to be passed back to the system.
@return EB_SUCCESS
*/
INT eb_begin_of_run(INT rn, char * UserField, char * error)
{
  extern EBUILDER_SETTINGS ebset;
  extern EBUILDER_CHANNEL  ebch[MAX_CHANNELS];
  extern HNDLE hDB;

  int n;
  int mask = 0;

  char keyName[256];
  BOOL enabled;
  int size;

  /* 
   * Determine which crates are enabled and set up the mask 
   * accordingly.
   */
  for(n = 1; n <= NUMCRATES; n++) {
    sprintf(keyName, "/Equipment/Crate %d/Settings/Enabled", n);
    size = sizeof(enabled);
    db_get_value(hDB, 0, keyName, &enabled, &size, TID_BOOL, FALSE);
    if(size == sizeof(enabled) && enabled) {
       ebch[n-1].trigger_mask = (1 << (n-1));
       mask |= ebch[n-1].trigger_mask;
    } else {
       ebch[n-1].trigger_mask = 0;
    }
  }

  cm_msg(MINFO, "eb_begin_of_run", "Setting event mask to 0x%x", mask);
  ebset.trigger_mask = mask;
 
  // Initialize online compression
  compress_load_all();
  
  return EB_SUCCESS;
}

/*--------------------------------------------------------------------*/
/** eb_end_of_run()
Hook to the event builder task at completion of event collection after
receiving the Stop transition.
\begin{verbatim}
\end{verbatim}

@param rn run number
@param error error string to be passed back to the system.
@return EB_SUCCESS
*/
INT eb_end_of_run(INT rn, char * error)
{
  return EB_SUCCESS;
}

/*--------------------------------------------------------------------*/
/** @name eb_user()
Hook to the event builder task after the reception of
all fragments of the same serial number. The destination
event has already the final EVENT_HEADER setup with
the data size set to 0. It is than possible to
add private data at this point using the proper
bank calls.

The ebch[] array structure points to nfragment channel structure
with the following content:
\begin{verbatim}
typedef struct {
    char  name[32];         // Fragment name (Buffer name).
    DWORD serial;           // Serial fragment number.
    char *pfragment;        // Pointer to fragment (EVENT_HEADER *)
    ...
} EBUILDER_CHANNEL;
\end{verbatim}

The correct code for including your own MIDAS bank is shown below where
{\bf TID_xxx} is one of the valid Bank type starting with {\bf TID_} for
midas format or {\bf xxx_BKTYPE} for Ybos data format.
{\bf bank_name} is a 4 character descriptor.
{\bf pdata} has to be declared accordingly with the bank type.
Refers to the ebuser.c source code for further description.

{\Large It is not possible to mix within the same destination event different
   event format!}

\begin{verbatim}
  // Event is empty, fill it with BANK_HEADER
  // If you need to add your own bank at this stage 
  
  bk_init(pevent);
  bk_create(pevent, bank_name, TID_xxxx, &pdata);
  *pdata++ = ...;
  *dest_size = bk_close(pevent, pdata);
  pheader->data_size = *dest_size + sizeof(EVENT_HEADER);
\end{verbatim}

For YBOS format, use the following example.

\begin{verbatim}
  ybk_init(pevent);
  ybk_create(pevent, "EBBK", I4_BKTYPE, &pdata);
  *pdata++ = 0x12345678;
  *pdata++ = 0x87654321;
  *dest_size = ybk_close(pevent, pdata);
  *dest_size *= 4;
  pheader->data_size = *dest_size + sizeof(YBOS_BANK_HEADER);
\end{verbatim}

@param nfrag Number of fragment.
@param ebch  Structure to all the fragments.
@param pheader Destination pointer to the header.
@param pevent Destination pointer to the bank header.
@param dest_size Destination event size in bytes.
@memo  event builder user code for private data filtering.
@return EB_SUCCESS
*/
INT eb_user(INT nfrag
    , EBUILDER_CHANNEL * ebch, EVENT_HEADER *pheader
    , void *pevent, INT * dest_size)
{
  // Initialize output event
  bk_init32(pevent);

  // Loop over the event fragments, performing compression into the output event
  for(int i = 0; i < nfrag; i++) {
    void *fragment =  ebch[i].pfragment;

    if(fragment != NULL) {
      compress_event(((EVENT_HEADER *) fragment) + 1, pevent);
      pheader->serial_number = 
        ((EVENT_HEADER *) ebch[i].pfragment)->serial_number;
    }
  }

  // Set the size of the output event properly
  pheader->data_size = *dest_size = bk_size(pevent);

  printf("Returning size %d\n", pheader->data_size);

  return EB_SUCCESS;
}

