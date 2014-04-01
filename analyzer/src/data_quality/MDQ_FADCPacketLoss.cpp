/********************************************************************\

  Name:         MDQ_FADCPacketLoss
  Created by:   Andrew Edmonds

  Contents:     A low-level data quality module

  Histograms:

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
INT  MDQ_FADCPacketLoss_init(void);
INT MDQ_FADCPacketLoss_BOR(INT run_number);
INT  MDQ_FADCPacketLoss(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1* hDQ_FADCPacketLoss;
static int midas_events;


ANA_MODULE MDQ_FADCPacketLoss_module =
{
  "MDQ_FADCPacketLoss",        /* module name           */
  "Andrew Edmonds",              /* author                */
  MDQ_FADCPacketLoss,          /* event routine         */
  MDQ_FADCPacketLoss_BOR,      /* BOR routine           */
  NULL,                          /* EOR routine           */
  MDQ_FADCPacketLoss_init,     /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MDQ_FADCPacketLoss_init()
{
  // Create the low-level data quality directory
  std::string dir_name("DataQuality_LowLevel/MDQ_FADCPacketLoss");
  TDirectory* dir = gDirectory->mkdir(dir_name.c_str());
  dir->cd(dir_name.c_str());

  // This histogram has the bank names labeled on the X-axis, and the midas
  // block number on the Y-axis.
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.
  hDQ_FADCPacketLoss = new TH1F(
    "hDQ_FADCPacketLoss",
    "Fraction of MIDAS Events with FADC packet loss per board",
    4,128, 132);
  hDQ_FADCPacketLoss->SetBit(TH1::kCanRebin);
  hDQ_FADCPacketLoss->GetXaxis()->SetTitle("FADC Board Number");
  hDQ_FADCPacketLoss->GetYaxis()->SetTitle("Fraction of Events with lost packets");

  return SUCCESS;
}

/**
 *  This method executes at the start of each run
 */
INT MDQ_FADCPacketLoss_BOR(INT run_number)
{
  midas_events = 1;
  
  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
  * of TOctalFADCIsland objects from the raw Octal FADC data.
  */
INT MDQ_FADCPacketLoss(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;

  // Get the NLSS bank and get a pointer to the raw data
  unsigned int* raw; // points at the raw data
  int bankSize = bk_locate(pevent,"NLSS",&raw);

  int board_number = 0;
  int packet_lost = 0;
  int n_packets_lost = 0;

  bool Boards[256]; // We can have maximally 256 FADC boards due to the address space
  for(int i=0; i<256; ++i) Boards[i] = false;

  for (int i = 0; i < bankSize; i+=2) {
    board_number = *(raw+i);
    packet_lost = *(raw+i+1);
    
    if(*(raw+i) >= 0 && *(raw+i) < 255) Boards[i] = true;

    n_packets_lost++;
    //    printf("Event #%d: Board %d lost packet #%d\n", midas_event_number, board_number, packet_lost);

    // Fill Diagnostic histogram
    hDQ_FADCPacketLoss->Fill(board_number, 1);   
  }

  for(int i=0; i<256; ++i){
    if(Boards[i] == true){
      hDQ_FADCPacketLoss->Scale(1.0 / midas_events);
    }
  }
  
  ++midas_events;

  return SUCCESS;
}
