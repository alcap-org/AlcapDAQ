/********************************************************************\

  Name:         MOctalFADCBufferOverflow
  Created by:   Andrew Edmonds

  Contents:     Module to plot the boards that overflowed in each event

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

/* AlCap includes */
#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MOctalFADCBufferOverflow_init(void);
INT  MOctalFADCBufferOverflow(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1* hNOctalFADCBufferOverflow;

ANA_MODULE MOctalFADCBufferOverflow_module =
{
  "MOctalFADCBufferOverflow",        /* module name           */
  "Andrew Edmonds",              /* author                */
  MOctalFADCBufferOverflow,          /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MOctalFADCBufferOverflow_init,     /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MOctalFADCBufferOverflow_init()
{
  // This histogram has the bank names labeled on the X-axis, and the midas
  // block number on the Y-axis.
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.
  hNOctalFADCBufferOverflow = new TH1F(
    "hNOctalFADCBufferOverflow",
    "Total Number of Events in which the FADC Overflowed",
    4,128, 132);
  hNOctalFADCBufferOverflow->SetBit(TH1::kCanRebin);
  hNOctalFADCBufferOverflow->GetXaxis()->SetTitle("FADC Board Number");
  hNOctalFADCBufferOverflow->GetYaxis()->SetTitle("Total Number of Buffer Overflows");

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
  * of TOctalFADCIsland objects from the raw Octal FADC data.
  */
INT MOctalFADCBufferOverflow(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;

  unsigned int* raw; // Points at the raw data
  int bankSize = bk_locate(pevent,"NBUF",&raw);

  if (bankSize != 0) {
    for (int i = 0; i < bankSize; i++) {
      hNOctalFADCBufferOverflow->Fill(*(raw+i));
    }
  }

  return SUCCESS;
}
