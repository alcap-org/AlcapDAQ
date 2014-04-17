/********************************************************************\

Name:         MDQ_FADCBufferOverflow
Created by:   Andrew Edmonds + Joe Grange

Contents:     hDQ_FADCBufferOverflow



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
#include <TDirectory.h>

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
INT  MDQ_FADCBufferOverflow_init(void);
INT MDQ_FADCBufferOverflow_BOR(INT run_number);
INT MDQ_FADCBufferOverflow_EOR(INT run_number);
INT  MDQ_FADCBufferOverflow(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1* hDQ_FADCBufferOverflowTot;
static TH1* hDQ_FADCBufferOverflowFrac;
static TH2* hDQ_FADCBufferOverflowByEvent;
static int n_total_midas_events;


ANA_MODULE MDQ_FADCBufferOverflow_module =
{
  "MDQ_FADCBufferOverflow",        /* module name           */
  "Andrew Edmonds",              /* author                */
  MDQ_FADCBufferOverflow,          /* event routine         */
  MDQ_FADCBufferOverflow_BOR,      /* BOR routine           */
  MDQ_FADCBufferOverflow_EOR,      /* EOR routine           */
  MDQ_FADCBufferOverflow_init,     /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MDQ_FADCBufferOverflow_init()
{
  // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Get run number
  char key_name[80];
  int size,run_number;
  sprintf(key_name, "Runinfo/Run number");
  size=sizeof(int);
  db_get_value(hDB,0,key_name, &run_number, &size, TID_INT,1);

  // Create the histograms
  hDQ_FADCBufferOverflowTot = new TH1F(
    "hDQ_FADCBufferOverflowTot",
    Form("Total number of MIDAS Events with buffer overflows per board, run %d",run_number),
    4,128, 132);
  hDQ_FADCBufferOverflowTot->SetBit(TH1::kCanRebin);
  hDQ_FADCBufferOverflowTot->GetXaxis()->SetTitle("FADC Board Number");
  hDQ_FADCBufferOverflowTot->GetYaxis()->SetTitle("Number of events with buffer overflows");

  hDQ_FADCBufferOverflowFrac = new TH1F(
    "hDQ_FADCBufferOverflowFrac",
    Form("Fraction of MIDAS Events with FADC buffer overflows by board, run %d",run_number),
    4,128, 132);
  hDQ_FADCBufferOverflowFrac->SetBit(TH1::kCanRebin);
  hDQ_FADCBufferOverflowFrac->GetXaxis()->SetTitle("FADC Board Number");
  hDQ_FADCBufferOverflowFrac->GetYaxis()->SetTitle("Fraction of events with buffer overflows");

  hDQ_FADCBufferOverflowByEvent = new TH2F(
    "hDQ_FADCBufferOverflowByEvent",
    Form("FADC buffer overflows by board and event, run %d",run_number),
    4,128, 132,1000,0,1000);
  hDQ_FADCBufferOverflowByEvent->SetBit(TH1::kCanRebin);
  hDQ_FADCBufferOverflowByEvent->GetXaxis()->SetTitle("FADC Board Number");
  hDQ_FADCBufferOverflowByEvent->GetYaxis()->SetTitle("MIDAS event");

  gDirectory->Cd("/MidasHists/");

  return SUCCESS;
}

/**
 *  This method executes at the start of each run
 */
INT MDQ_FADCBufferOverflow_BOR(INT run_number)
{
  n_total_midas_events = 1;
  
  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
  * of TOctalFADCIsland objects from the raw Octal FADC data.
  */
INT MDQ_FADCBufferOverflow(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;

  // Get the NBUF bank and get a pointer to the raw data
  unsigned int* raw; // points at the raw data
  int bankSize = bk_locate(pevent,"NBUF",&raw);

  int board_number = 0;
  int n_buffers_overflow = 0;

  bool Boards[256]; // We can have maximally 256 FADC boards due to the address space
  for(int i=0; i<256; ++i) Boards[i] = false;

  for (int i = 0; i < bankSize; i++) {
    board_number = *(raw+i);

    if(*(raw+i) >= 0 && *(raw+i) < 255) Boards[i] = true;

    n_buffers_overflow++;
        //printf("Event #%d: Board %d n buffers overflowed %d\n", midas_event_number, board_number,n_buffers_overflow);

    // Fill Diagnostic histogram
    hDQ_FADCBufferOverflowTot->Fill(board_number, 1);   
    hDQ_FADCBufferOverflowFrac->Fill(board_number, 1);
    hDQ_FADCBufferOverflowByEvent->Fill(board_number,midas_event_number, 1);

  }

  /*
  for(int i=0; i<256; ++i){
    if(Boards[i] == true){
      hDQ_FADCBufferOverflowFrac->Scale(1.0 / n_total_midas_events);
    }
  }
  */  

  ++n_total_midas_events;

  return SUCCESS;
}

INT MDQ_FADCBufferOverflow_EOR(INT run_number)
{

  //for(int i=0; i<256; ++i){
    //if(Boards[i] == true){
      hDQ_FADCBufferOverflowFrac->Scale(1.0 / n_total_midas_events);
    //}
  //}

  return SUCCESS;
}
