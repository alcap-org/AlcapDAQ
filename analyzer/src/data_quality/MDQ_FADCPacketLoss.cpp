/********************************************************************\

Name:         MDQ_FADCPacketLoss
Created by:   Andrew Edmonds + Joe Grange

Contents:     hDQ_FADCPacketLoss
               - Plots: the fraction of MIDAS events that had a packet loss, the total number
			of packet losses, and packet losses by board and midas block number
               - To Check: value is not too high(?)
               - Soln: throw out whole run (?)



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
INT  MDQ_FADCPacketLoss_init(void);
INT MDQ_FADCPacketLoss_BOR(INT run_number);
INT  MDQ_FADCPacketLoss(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1* hDQ_FADCPacketLossTot;
static TH1* hDQ_FADCPacketLossFrac;
static TH2* hDQ_FADCPacketLossByEvent;
static int n_total_midas_events;


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
  // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Create the histograms
  hDQ_FADCPacketLossTot = new TH1F(
    "hDQ_FADCPacketLossTot",
    "Total number of MIDAS Events with FADC packet loss per board",
    4,128, 132);
  hDQ_FADCPacketLossTot->SetBit(TH1::kCanRebin);
  hDQ_FADCPacketLossTot->GetXaxis()->SetTitle("FADC Board Number");
  hDQ_FADCPacketLossTot->GetYaxis()->SetTitle("Number of Events with lost packets");

  hDQ_FADCPacketLossFrac = new TH1F(
    "hDQ_FADCPacketLossFrac",
    "Fraction of MIDAS Events with FADC packet loss per board",
    4,128, 132);
  hDQ_FADCPacketLossFrac->SetBit(TH1::kCanRebin);
  hDQ_FADCPacketLossFrac->GetXaxis()->SetTitle("FADC Board Number");
  hDQ_FADCPacketLossFrac->GetYaxis()->SetTitle("Fraction of Events with lost packets");

  hDQ_FADCPacketLossByEvent = new TH2F(
    "hDQ_FADCPacketLossByEvent",
    "FADC packet losses by board and event",
    4,128, 132,100,0,100);
  hDQ_FADCPacketLossByEvent->SetBit(TH2::kCanRebin);
  hDQ_FADCPacketLossByEvent->GetXaxis()->SetTitle("FADC Board Number");
  hDQ_FADCPacketLossByEvent->GetYaxis()->SetTitle("MIDAS event");

  gDirectory->Cd("/MidasHists/");

  return SUCCESS;
}

/**
 *  This method executes at the start of each run
 */
INT MDQ_FADCPacketLoss_BOR(INT run_number)
{
  n_total_midas_events = 1;
  
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

    //printf(" event number %d i %d bank size %d board number %d\n",midas_event_number,i,bankSize,board_number);

    packet_lost = *(raw+i+1);
    
    if(*(raw+i) >= 0 && *(raw+i) < 255) Boards[i] = true;

    n_packets_lost++;
        //printf("Event #%d: Board %d lost packet #%d n packets lost %d\n", midas_event_number, board_number, packet_lost,n_packets_lost);

    // Fill Diagnostic histogram
    hDQ_FADCPacketLossTot->Fill(board_number, 1);   
    hDQ_FADCPacketLossFrac->Fill(board_number, 1);
    hDQ_FADCPacketLossByEvent->Fill(board_number,midas_event_number, 1);

  }

  for(int i=0; i<256; ++i){
    if(Boards[i] == true){
      hDQ_FADCPacketLossFrac->Scale(1.0 / n_total_midas_events);
    }
  }
  
  ++n_total_midas_events;

  return SUCCESS;
}
