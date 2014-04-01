/********************************************************************\

  Name:         MOctalFADCPacketLoss
  Created by:   Andrew Edmonds

  Contents:     Module to plot the average number of lost packets per FADC board

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
#include <TH2.h>

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
INT  MOctalFADCPacketLoss_init(void);
INT MOctalFADCPacketLoss_BOR(INT run_number);
INT  MOctalFADCPacketLoss(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1* hNOctalFADCAvgPacketLoss;
static TH1* hNOctalFADCEventsWithPacketLoss;
static TH1* hNOctalFADCEventsWithPacketLossPercent;
static int midas_events;


ANA_MODULE MOctalFADCPacketLoss_module =
{
  "MOctalFADCPacketLoss",        /* module name           */
  "Andrew Edmonds",              /* author                */
  MOctalFADCPacketLoss,          /* event routine         */
  MOctalFADCPacketLoss_BOR,      /* BOR routine           */
  NULL,                          /* EOR routine           */
  MOctalFADCPacketLoss_init,     /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MOctalFADCPacketLoss_init()
{
  // This histogram has the bank names labeled on the X-axis, and the midas
  // block number on the Y-axis.
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.
  hNOctalFADCAvgPacketLoss = new TH1F(
    "hNOctalFADCAvgPacketLoss",
    "Number of FADC packets lost per board",
    4,128, 132);
  hNOctalFADCAvgPacketLoss->SetBit(TH1::kCanRebin);
  hNOctalFADCAvgPacketLoss->GetXaxis()->SetTitle("FADC Board Number");
  hNOctalFADCAvgPacketLoss->GetYaxis()->SetTitle("Number of Packets Lost");

  hNOctalFADCEventsWithPacketLoss = new TH1F(
    "hNOctalFADCEventsWithPacketLoss",
    "Number of MIDAS Events with FADC packet loss per board",
    4,128, 132);
  hNOctalFADCEventsWithPacketLoss->SetBit(TH1::kCanRebin);
  hNOctalFADCEventsWithPacketLoss->GetXaxis()->SetTitle("FADC Board Number");
  hNOctalFADCEventsWithPacketLoss->GetYaxis()->SetTitle("Number of Events with lost packets");

  hNOctalFADCEventsWithPacketLossPercent = new TH1F(
    "hNOctalFADCEventsWithPacketLossPercent",
    "Percentage of MIDAS Events with FADC packet loss per board",
    4,128, 132);
  hNOctalFADCEventsWithPacketLossPercent->SetBit(TH1::kCanRebin);
  hNOctalFADCEventsWithPacketLossPercent->GetXaxis()->SetTitle("FADC Board Number");
  hNOctalFADCEventsWithPacketLossPercent->GetYaxis()->SetTitle("Average Number of Events with lost packets");

  return SUCCESS;
}

/**
 *  This method executes at the start of each run
 */
INT MOctalFADCPacketLoss_BOR(INT run_number)
{
  midas_events = 1;
  
  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
  * of TOctalFADCIsland objects from the raw Octal FADC data.
  */
INT MOctalFADCPacketLoss(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;

  // Want the average and since I'll scale by 1/midas_event_number at the end, I should undo it now before I add the new data
  //if (midas_event_number > 1)
  //  hNOctalFADCAvgPacketLoss->Scale(midas_event_number-1);   

  unsigned int* raw; // Points at the raw data
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
    hNOctalFADCAvgPacketLoss->Fill(board_number, 1);   
  }

  for(int i=0; i<256; ++i){
    if(Boards[i] == true){
      int bin = hNOctalFADCEventsWithPacketLoss->FindBin(i+1);
      hNOctalFADCEventsWithPacketLoss->Fill(bin);
      hNOctalFADCEventsWithPacketLossPercent->SetBinContent(bin, hNOctalFADCEventsWithPacketLoss->GetBinContent(bin) / midas_events);
    }
  }
  
  ++midas_events;

  // Scale down for the average, the final midas event should leave this correct
  //hNOctalFADCAvgPacketLoss->Scale(1.0 / midas_event_number);

  return SUCCESS;
}
