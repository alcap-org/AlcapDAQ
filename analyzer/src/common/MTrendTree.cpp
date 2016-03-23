/********************************************************************\

  Name:         MTrendTree
  Created by:   John R Quirk

  Contents:     Module to generate a ROOT tree with trend data

\********************************************************************/

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TTree.h>
#include <TBranch.h>

/* Standard includes */
#include <iostream>

/*-- Module declaration --------------------------------------------*/
INT MTrendTree_bor(INT);
INT MTrendTree_eor(INT);
INT MTrendTree_init(void);
INT MTrendTree_exit(void);
INT MTrendTree(EVENT_HEADER*, void*);

extern TTree *gTrendTree;

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

INT MTrendTree_bor(INT run)
{
  gTrendTree = new TTree("Trends","Trends of data");
  TBranch* br = gTrendTree->Branch("run", &run);
  br->Fill();
  br->ResetAddress();
  return SUCCESS;
}

INT MTrendTree_eor(INT run)
{
  gTrendTree->SetEntries();
  gTrendTree->Write();
  delete gTrendTree;
  gTrendTree = NULL;
  return SUCCESS;
}

INT MTrendTree_exit()
{
  std::cout << "TREND TREE EXIT...";
  std::cout << "DONE" << std::endl;
  return SUCCESS;
}

// Even though we don't use this, we need it to be called.
INT MTrendTree(EVENT_HEADER*, void*)
{
  return SUCCESS;
}
