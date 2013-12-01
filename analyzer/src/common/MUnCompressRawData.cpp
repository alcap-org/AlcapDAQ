/********************************************************************\

  Name:         MUnCompressRawData.cpp
  Created by:   Tom Banks

  Contents:     Name says it all: this module reverses any data
                compression performed on the raw data before storage.
                The code is lifted from Fred's compression software
                in CVS daq/tdc_compress: the main body comes from 
                "mucap_expand.cpp", which calls the function 
                "expand_event" in "overall.cpp."
                
\********************************************************************/
                                                       
/*-- Include files -------------------------------------------------*/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <byteswap.h>
#include <time.h>

/* MIDAS includes */
#include "midas.h"

/* MuCap includes */
#include "mucap_compress.h"
#include "common.h"

/*-- Parameters ----------------------------------------------------*/

extern HNDLE hDB;

/*-- Static parameters ---------------------------------------------*/

/*-- Module declaration --------------------------------------------*/

INT MUnCompressRawData_init(void);
INT MUnCompressRawData(EVENT_HEADER*, void*);

ANA_MODULE MUnCompressRawData_module = {
  "MUnCompressRawData",        /* module name           */  
  "Tom Banks",                 /* author                */
  MUnCompressRawData,          /* event routine         */
  NULL,                        /* BOR routine           */  
  NULL,                        /* EOR routine           */
  MUnCompressRawData_init,     /* init routine          */
  NULL,                        /* exit routine          */
  NULL,                        /* parameter structure   */
  0,                           /* structure size        */
  NULL,                        /* initial parameters    */
};

/*-- init routine --------------------------------------------------*/

INT MUnCompressRawData_init(void)
{
  compress_load_all();

  return SUCCESS;
}

/*-- event routine -------------------------------------------------*/

INT MUnCompressRawData(EVENT_HEADER *pheader, void *pevent)
{
  int event_id = pheader->event_id;

  int event_serial = pheader->serial_number;
  
  // *** modified by VT on 9-Oct-2008 ***
  //int skip_threshold = 40;
  int skip_threshold = 1;
  if (event_serial % skip_threshold !=0){
    printf("Event is not divisible by %d, no analysis will be done\n",skip_threshold);
    return ANA_SKIP;
  }
  time_t block_time = pheader->time_stamp;
  char fdate[] = "%Y-%m-%d", ftime[] = "%H:%M:%S";
  struct tm block_tm;
  localtime_r(&block_time, &block_tm);
  char sdate[100], stime[100];
  strftime(sdate, 100, fdate, &block_tm);
  strftime(stime, 100, ftime, &block_tm);
  printf("Current block (serial number %d) stored on %s at %s\n", 
	 event_serial, sdate, stime);

  if (event_id == EVENTID_BOR || 
      event_id == EVENTID_EOR ||
      event_id == EVENTID_MESSAGE) {
      
    // No decompression is necessary for non-ordinary events

  } else {

    expand_event(pevent, pevent);

  }

  return SUCCESS;
}

/**************************************************************/
