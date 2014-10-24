////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_MuPC
/// \ingroup lldq
/// \author Joe Grange
///
/// \brief
/// Plots diagnostic distributions for MuPC 
///
////////////////////////////////////////////////////////////////////////////////

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
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MDQ_MuPC_init(void);
INT MDQ_MuPC_BOR(INT run_number);
INT MDQ_MuPC_EOR(INT run_number);
INT  MDQ_MuPC(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH2* hDQ_MuPC_XvsT;
static TH2* hDQ_MuPC_YvsT;
static TH2* hDQ_MuPC_XvsY_Total;
static TH2* hDQ_MuPC_XvsY_ExclHotWires;
static TH2* hDQ_MuPC_XvsY_Fraction;
static int n_total_clusters;

ANA_MODULE MDQ_MuPC_module =
{
  "MDQ_MuPC",        /* module name           */
  "Joe Grange",              /* author                */
  MDQ_MuPC,          /* event routine         */
  MDQ_MuPC_BOR,      /* BOR routine           */
  MDQ_MuPC_EOR,      /* EOR routine           */
  MDQ_MuPC_init,     /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MDQ_MuPC_init()
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
  hDQ_MuPC_XvsT = new TH2F(
    "hDQ_MuPC_XvsT",
    Form("X wire vs MIDAS block time, run %d",run_number),
    1000,-10000000, 120000000, 24, 0.5, (24+0.5));

  //hDQ_MuPC_XvsT->SetBit(TH1::kCanRebin);
  hDQ_MuPC_XvsT->GetXaxis()->SetTitle("time (ns)");
  hDQ_MuPC_XvsT->GetXaxis()->CenterTitle(1);
  hDQ_MuPC_XvsT->GetYaxis()->SetTitle("X wire hit");
  hDQ_MuPC_XvsT->GetYaxis()->CenterTitle(1);

  hDQ_MuPC_YvsT = new TH2F(
    "hDQ_MuPC_YvsT",
    Form("X wire vs MIDAS block time, run %d",run_number),
    1000,-10000000, 120000000, 24, 0.5, (24+0.5));

  //hDQ_MuPC_YvsT->SetBit(TH1::kCanRebin);
  hDQ_MuPC_YvsT->GetXaxis()->SetTitle("time (ns)");
  hDQ_MuPC_YvsT->GetXaxis()->CenterTitle(1);
  hDQ_MuPC_YvsT->GetYaxis()->SetTitle("Y wire hit");
  hDQ_MuPC_YvsT->GetYaxis()->CenterTitle(1);
 
  hDQ_MuPC_XvsY_Total = new TH2F(
    "hDQ_MuPC_XvsY_Total",
    Form("X wire vs Y wire, each cluster and run %d",run_number),
    24, 0.5, (24+0.5), 24, 0.5, (24+0.5));

  hDQ_MuPC_XvsY_Total->GetXaxis()->SetTitle("X wire hit");
  hDQ_MuPC_XvsY_Total->GetXaxis()->CenterTitle(1);
  hDQ_MuPC_XvsY_Total->GetYaxis()->SetTitle("Y wire hit");
  hDQ_MuPC_XvsY_Total->GetYaxis()->CenterTitle(1);

  hDQ_MuPC_XvsY_ExclHotWires = new TH2F(
    "hDQ_MuPC_XvsY_ExclHotWires",
    Form("X wire vs Y wire, each cluster and run %d",run_number),
    24, 0.5, (24+0.5), 24, 0.5, (24+0.5));

  hDQ_MuPC_XvsY_ExclHotWires->GetXaxis()->SetTitle("X wire hit");
  hDQ_MuPC_XvsY_ExclHotWires->GetXaxis()->CenterTitle(1);
  hDQ_MuPC_XvsY_ExclHotWires->GetYaxis()->SetTitle("Y wire hit");
  hDQ_MuPC_XvsY_ExclHotWires->GetYaxis()->CenterTitle(1);

  hDQ_MuPC_XvsY_Fraction = new TH2F(
    "hDQ_MuPC_XvsY_Fraction",
    Form("Fraction of cluster position in X wire vs Y wire, run %d",run_number),
    24, 0.5, (24+0.5), 24, 0.5, (24+0.5));

  //hDQ_MuPC_XvsY_Fraction->SetBit(TH1::kCanRebin);
  hDQ_MuPC_XvsY_Fraction->GetXaxis()->SetTitle("X wire hit");
  hDQ_MuPC_XvsY_Fraction->GetXaxis()->CenterTitle(1);
  hDQ_MuPC_XvsY_Fraction->GetYaxis()->SetTitle("Y wire hit");
  hDQ_MuPC_XvsY_Fraction->GetYaxis()->CenterTitle(1);

  gDirectory->Cd("/MidasHists/");

  return SUCCESS;
}

/**
 *  This method executes at the start of each run
 */
INT MDQ_MuPC_BOR(INT run_number)
{
  //n_total_midas_events = 1;
  
  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
  * of TOctalFADCIsland objects from the raw Octal FADC data.
  */
INT MDQ_MuPC(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;

  // Get the MPC1 bank and get a pointer to the raw data
  double* raw; // points at the raw data

  int bankSize = bk_locate(pevent,"MPC1",&raw);

  bankSize = bankSize*sizeof(DWORD)/sizeof(double);

  double timestamp = 0.;
  double wire_number_x = 0.;
  double wire_number_y = 0.;

  for (int i=0; i<bankSize; i+=3){

    wire_number_x = 0.;
    wire_number_x = (double)*(raw+i);

    wire_number_y = 0.;
    wire_number_y = (double)*(raw+i+1);

    timestamp = 0.;
    timestamp = (double)*(raw+i+2);

    if (wire_number_y>1 && (wire_number_y<22)) hDQ_MuPC_XvsY_ExclHotWires->Fill(wire_number_x,wire_number_y);

    hDQ_MuPC_XvsY_Total->Fill(wire_number_x,wire_number_y);
    hDQ_MuPC_XvsY_Fraction->Fill(wire_number_x,wire_number_y);
    hDQ_MuPC_XvsT->Fill(timestamp,wire_number_x);
    hDQ_MuPC_YvsT->Fill(timestamp,wire_number_y);

    //printf("index %d out of %d, x wire %f y wire %f tstamp %f\n",i,bankSize,wire_number_x,wire_number_y,timestamp);

    n_total_clusters++;

  }

  return SUCCESS;
}

INT MDQ_MuPC_EOR(INT run_number)
{

  hDQ_MuPC_XvsY_Fraction->Scale(1.0 / (double)n_total_clusters);

  return SUCCESS;
}

/// @}
