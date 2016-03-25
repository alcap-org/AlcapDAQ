/********************************************************************\

  Name:         MTrendTree
  Created by:   John R Quirk

  Contents:     Module to generate a ROOT tree with trend data

\********************************************************************/

/* Standard includes */
#include <iostream>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TTree.h>
#include <TBranch.h>

/* AlCap includes */
#include "TTrendTree.h"

/*-- Module declaration --------------------------------------------*/
INT MTrendTree_bor(INT);
INT MTrendTree_eor(INT);
INT MTrendTree_init(void);
INT MTrendTree_exit(void);
INT MTrendTree(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TTrendTree *gTrendTree;

ANA_MODULE MTrendTree_module =
{
  "MTrendTree",    /* module name           */
  "John R Quirk",  /* author                */
  MTrendTree,      /* event routine         */
  MTrendTree_bor,  /* BOR routine           */
  MTrendTree_eor,  /* EOR routine           */
  MTrendTree_init, /* init routine          */
  MTrendTree_exit, /* exit routine          */
  NULL,            /* parameter structure   */
  0,               /* structure size        */
  NULL,            /* initial parameters    */
};

INT MTrendTree_init()
{
  return SUCCESS;
}

static int gRun = -1;
static int gBlocks = -1;
INT MTrendTree_bor(INT run)
{
  gRun = run;
  gBlocks = 0;
  gTrendTree->Init(run);
  return SUCCESS;
}

INT MTrendTree_eor(INT run)
{

  // Get start, stop, and run times
  ULong64_t start = 0, stop = 0;
  int size = sizeof(DWORD);
  char keyname[128];
  sprintf(keyname, "/Runinfo/Start time binary");
  db_get_value(hDB, 0, keyname, &start, &size, TID_DWORD, false);
  size = sizeof(DWORD);
  sprintf(keyname, "/Runinfo/Stop time binary");
  db_get_value(hDB, 0, keyname, &stop, &size, TID_DWORD, false);

  gTrendTree->FillRunTree("start",  start);
  gTrendTree->FillRunTree("stop",   stop);
  gTrendTree->FillRunTree("blocks", gBlocks);
  gTrendTree->Finish();
  return SUCCESS;
}

INT MTrendTree_exit()
{
  return SUCCESS;
}

// Even though we don't use this, we need it to be called.
INT MTrendTree(EVENT_HEADER* ev, void*)
{
  ++gBlocks;
  gTrendTree->FillBlockTree("run",   gRun);
  gTrendTree->FillBlockTree("block", ev->serial_number);
  return SUCCESS;
}
