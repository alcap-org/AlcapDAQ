/********************************************************************\

Name:         MDQ_FADCPacketLoss
Created by:   Andrew Edmonds + Joe Grange

Contents:     hDQ_FADCPacketLoss_Total
              - plots the total number of MIDAS events that had a packet loss
                per board

              hDQ_FADCPacketLoss_Fraction
              - plots the fraction of MIDAS events that had a packet loss
                per board

              hDQ_FADCPacketLoss_TotalByEvent
              - plots the total number of packet losses per board
                on an event-by-event basis

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
INT MDQ_FADCPacketLoss_EOR(INT run_number);
INT  MDQ_FADCPacketLoss(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1* hDQ_FADCPacketLoss_Total;
static TH1* hDQ_FADCPacketLoss_Fraction;
static TH2* hDQ_FADCPacketLoss_ByEvent;
static int n_total_midas_events;


ANA_MODULE MDQ_FADCPacketLoss_module =
{
  "MDQ_FADCPacketLoss",        /* module name           */
  "Andrew Edmonds",              /* author                */
  MDQ_FADCPacketLoss,          /* event routine         */
  MDQ_FADCPacketLoss_BOR,      /* BOR routine           */
  MDQ_FADCPacketLoss_EOR,      /* EOR routine           */
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

  // Get run number
  char key_name[80];
  int size,run_number;
  sprintf(key_name, "Runinfo/Run number");
  size=sizeof(int);
  db_get_value(hDB,0,key_name, &run_number, &size, TID_INT,1);

  // Create the histograms
  hDQ_FADCPacketLoss_Total = new TH1F(
    "hDQ_FADCPacketLoss_Total",
    Form("Total number of MIDAS Events with FADC packet loss per board, run %d",run_number),
    4,128, 132);
  hDQ_FADCPacketLoss_Total->SetBit(TH1::kCanRebin);
  hDQ_FADCPacketLoss_Total->GetXaxis()->SetTitle("FADC Board Number");
  hDQ_FADCPacketLoss_Total->GetYaxis()->SetTitle("Number of Events with lost packets");

  hDQ_FADCPacketLoss_Fraction = new TH1F(
    "hDQ_FADCPacketLoss_Fraction",
    Form("Fraction of MIDAS Events with FADC packet loss per board, run %d",run_number),
    4,128, 132);
  hDQ_FADCPacketLoss_Fraction->SetBit(TH1::kCanRebin);
  hDQ_FADCPacketLoss_Fraction->GetXaxis()->SetTitle("FADC Board Number");
  hDQ_FADCPacketLoss_Fraction->GetYaxis()->SetTitle("Fraction of Events with lost packets");

  hDQ_FADCPacketLoss_TotalByEvent = new TH2F(
    "hDQ_FADCPacketLoss_TotalByEvent",
    Form("FADC packet losses by board and event, run %d",run_number),
    4,128, 132,5000,0,5000);
  hDQ_FADCPacketLoss_TotalByEvent->SetBit(TH2::kCanRebin);
  hDQ_FADCPacketLoss_TotalByEvent->GetXaxis()->SetTitle("FADC Board Number");
  hDQ_FADCPacketLoss_TotalByEvent->GetYaxis()->SetTitle("MIDAS event");

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
    hDQ_FADCPacketLoss_Total->Fill(board_number, 1);   
    hDQ_FADCPacketLoss_Fraction->Fill(board_number, 1);
    hDQ_FADCPacketLoss_TotalByEvent->Fill(board_number,midas_event_number, 1);

  }

  ++n_total_midas_events;

  return SUCCESS;
}

INT MDQ_FADCPacketLoss_EOR(INT run_number)
{

  hDQ_FADCPacketLoss_Fraction->Scale(1.0 / n_total_midas_events);

  return SUCCESS;
}

