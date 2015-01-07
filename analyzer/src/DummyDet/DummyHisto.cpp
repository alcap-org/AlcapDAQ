/********************************************************************\

  Name:         DummyHisto.cpp
  Created by:   Nam Tran

  Contents:     Create a dummy histogram

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>

/* AlCap includes */
//#include "TOctalFADCIsland.h"
//#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  DummyHisto_init(void);
INT  DummyHisto(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;

static TH1* hDummyHisto;

ANA_MODULE DummyHisto_module =
{
  "DummyHisto",        /* module name           */
  "Nam Tran",              /* author                */
  DummyHisto,          /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  DummyHisto_init,     /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT DummyHisto_init()
{
	hDummyHisto = new TH1D("hDummy1","A test histogram",1000,0,100);
	hDummyHisto->Fill(2,24);
	hDummyHisto->Fill(4,405);

  return SUCCESS;
}

INT DummyHisto(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  //int midas_event_number = pheader->serial_number;

    // Fill Diagnostic histogram
    //hNOctalFADCIslandsReadPerBlock->Fill(bank_name.c_str(), midas_event_number,
      //fadc_islands.size());

  return SUCCESS;
}
