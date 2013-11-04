/********************************************************************\

  Name:         SiDetectorHisto.cpp
  Created by:   Nam Tran

  Contents:     Create histograms for Si detectors

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
INT  SiDetectorHisto_init(void);
INT  SiDetectorHisto(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;

static TH2* hNOctalFADCIslandsReadPerBlock;
static TH1* hTestHisto;

ANA_MODULE SiDetectorHisto_module =
{
  "SiDetectorHisto",        /* module name           */
  "Nam Tran",              /* author                */
  SiDetectorHisto,          /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  SiDetectorHisto_init,     /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT SiDetectorHisto_init()
{
	hTestHisto = new TH1D("htest","A test histogram",1000,0,100);
	hTestHisto->Fill(2);
	hTestHisto->Fill(4);

  return SUCCESS;
}

INT SiDetectorHisto(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  //int midas_event_number = pheader->serial_number;

    // Fill Diagnostic histogram
    //hNOctalFADCIslandsReadPerBlock->Fill(bank_name.c_str(), midas_event_number,
      //fadc_islands.size());

  return SUCCESS;
}
