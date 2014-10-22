/********************************************************************\

  Name:         MMuPC1AnalysisC.cpp
  Created by:   Tom Banks

  Contents:     Generates muPC1 histograms.  The "successive tdiff" 
                histograms are particularly useful for determining
                the afterpulse-clustering intervals in the subsequent
                MQL module.
                
\********************************************************************/
                                                        
/*-- Include files -------------------------------------------------*/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <TDirectory.h>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

/* MuCap includes */
#include "common.h"
#include "ucb_common.h"

/*-- Parameters ----------------------------------------------------*/

/*-- Static parameters ---------------------------------------------*/

// Declare histograms
static TH2 *muPC1_Xwire_successive_tdiffs;
static TH2 *muPC1_XwireVStime_2D;
static TH2 *muPC1_Ywire_successive_tdiffs;
static TH2 *muPC1_YwireVStime_2D;
//static TH1 *mu_4101_VStime;

//static TH2 *muPC_4101tCorrX;
//static TH2 *muPC_4101tCorrY;

/*-- Module declaration --------------------------------------------*/

INT MMuPC1AnalysisC_init(void);
INT MMuPC1AnalysisC(EVENT_HEADER*, void*);

ANA_MODULE MMuPC1AnalysisC_module = {
  "MMuPC1AnalysisC",             /* module name           */  
  "Tom Banks",                   /* author                */
  MMuPC1AnalysisC,               /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MMuPC1AnalysisC_init,          /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/*-- init routine --------------------------------------------------*/

INT MMuPC1AnalysisC_init(void)
{

  // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("muPC_checks")) {
  
    std::string dir_name("muPC_checks/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  
  }

  /****** Construct histograms ******/
  char hist_name[256];
  char hist_title[256];

  // muPC1 Xwire histograms
  sprintf(hist_name,  "muPC1_X_raw_successive_tdiffs");
  sprintf(hist_title, "Tdiffs between successive raw #muPC Xwire hits");
  muPC1_Xwire_successive_tdiffs = new TH2D(hist_name, hist_title, 1451, -252.5, 50002.5, kMuPC1NumXWires, 0.5, (kMuPC1NumXWires+0.5));
  muPC1_Xwire_successive_tdiffs->GetXaxis()->SetTitle("time difference (ns)");
  muPC1_Xwire_successive_tdiffs->GetYaxis()->SetTitle("X wire number");

  sprintf(hist_name,  "muPC1_XwireVStime_2D");
  sprintf(hist_title, "#muPC raw Xwire hits vs time");
  muPC1_XwireVStime_2D = new TH2D(hist_name, hist_title, 1000, -10000000, 120000000, kMuPC1NumXWires, 0.5, (kMuPC1NumXWires+0.5));
  muPC1_XwireVStime_2D->GetXaxis()->SetTitle("hit time (ns)");
  muPC1_XwireVStime_2D->GetXaxis()->CenterTitle(1);
  muPC1_XwireVStime_2D->GetYaxis()->SetTitle("X wire number");
  muPC1_XwireVStime_2D->GetYaxis()->CenterTitle(1);

  // muPC1 Ywire histograms
  sprintf(hist_name,  "muPC1_Y_raw_successive_tdiffs");
  sprintf(hist_title, "Tdiffs between successive raw #muPC Ywire hits");
  muPC1_Ywire_successive_tdiffs = new TH2D(hist_name, hist_title, 1451, -252.5, 50002.5, kMuPC1NumYWires, 0.5, (kMuPC1NumYWires+0.5));//x axis orig to 7002.5
  muPC1_Ywire_successive_tdiffs->GetXaxis()->SetTitle("time difference (ns)");
  muPC1_Ywire_successive_tdiffs->GetYaxis()->SetTitle("Y wire number");

  /*
  sprintf(hist_name,  "muPC_4101tCorrX");
  sprintf(hist_title, "#muPC 4101 time corr");
  muPC_4101tCorrX = new TH2D(hist_name, hist_title, 1000, -10000000, 120000000, kMuPC1NumXWires, 0.5, (kMuPC1NumXWires+0.5));
  muPC_4101tCorrX->GetXaxis()->SetTitle("hit time (ns)");
  muPC_4101tCorrX->GetXaxis()->CenterTitle(1);
  muPC_4101tCorrX->GetYaxis()->SetTitle("X wire number");
  muPC_4101tCorrX->GetYaxis()->CenterTitle(1);

  sprintf(hist_name,  "muPC_4101tCorrY");
  sprintf(hist_title, "#muPC 4101 time corr");
  muPC_4101tCorrY = new TH2D(hist_name, hist_title, 1000, -10000000, 120000000, kMuPC1NumYWires, 0.5, (kMuPC1NumYWires+0.5));
  muPC_4101tCorrY->GetXaxis()->SetTitle("hit time (ns)");
  muPC_4101tCorrY->GetXaxis()->CenterTitle(1);
  muPC_4101tCorrY->GetYaxis()->SetTitle("Y wire number");
  muPC_4101tCorrY->GetYaxis()->CenterTitle(1);
  */

  sprintf(hist_name,  "muPC1_YwireVStime_2D");
  sprintf(hist_title, "#muPC raw Ywire hits vs time");
  muPC1_YwireVStime_2D = new TH2D(hist_name, hist_title, 1000, -10000000, 120000000, kMuPC1NumYWires, 0.5, (kMuPC1NumYWires+0.5));
  muPC1_YwireVStime_2D->GetXaxis()->SetTitle("hit time (ns)");
  muPC1_YwireVStime_2D->GetXaxis()->CenterTitle(1);
  muPC1_YwireVStime_2D->GetYaxis()->SetTitle("Y wire number");
  muPC1_YwireVStime_2D->GetYaxis()->CenterTitle(1);

  /*
  sprintf(hist_name,  "mu_4101_VStime");
  sprintf(hist_title, "4101 hits vs time");
  mu_4101_VStime = new TH1D(hist_name, hist_title, 1000, -10000000, 120000000);
  mu_4101_VStime->GetXaxis()->SetTitle("hit time (ns)");
  mu_4101_VStime->GetXaxis()->CenterTitle(1);
  mu_4101_VStime->GetYaxis()->SetTitle("hits");
  mu_4101_VStime->GetYaxis()->CenterTitle(1);
  */

  return SUCCESS;
}

/*-- event routine -------------------------------------------------*/

INT MMuPC1AnalysisC(EVENT_HEADER *pheader, void *pevent)
{
  /* ############################################### */
  // Open up the bank "HITS" (contains CAEN & COMP hits by parameter #)
  channel_hit *hit_bank;
  int hit_bank_size = bk_locate(pevent, "HITS", (DWORD *) &hit_bank);
  hit_bank_size = hit_bank_size * sizeof(DWORD) / sizeof(channel_hit);
  if(hit_bank == NULL) {
    fprintf(stderr, "MMuPC1AnalysisC: Could not find bank HITS.\n");
    return !SUCCESS;
  }

  /* ############################################### */

  // Initialize the "previous hit" indices
  int prev_hit_index_muPC1_Xwire[kMuPC1NumXWires+1];
  for (int j=1; j<=kMuPC1NumXWires; j++) {
    prev_hit_index_muPC1_Xwire[j] = -1;
  }
  int prev_hit_index_muPC1_Ywire[kMuPC1NumYWires+1];
  for (int j=1; j<=kMuPC1NumYWires; j++) {
    prev_hit_index_muPC1_Ywire[j] = -1;
  }

  double muPCxWireT = 0.;
  int wireX = 0;
  double muPCyWireT = 0.;
  int wireY = 0;
  double testT = 0.;

  // Loop over hits within hit_bank and fill muPC1 raw data histograms
  for (int i=0; i<hit_bank_size; i++) {
    int par = hit_bank[i].parameter;

    // Xwires
    if ((par >= 4001) && (par <= (4000+kMuPC1NumXWires))) {
      int wire = (par-4000);
      //printf("x hit! num %d out of %d; wire %d time %f \n",i,hit_bank_size,wire,hit_bank[i].time);
      // tdiffs between successive Xwire hits, per wire
      if (prev_hit_index_muPC1_Xwire[wire] >= 0) {
	muPC1_Xwire_successive_tdiffs->Fill((hit_bank[i].time -
					     hit_bank[prev_hit_index_muPC1_Xwire[wire]].time),
					    wire);
      }
      prev_hit_index_muPC1_Xwire[wire] = i;
      // 2D Xwire hits vs. time
            muPC1_XwireVStime_2D->Fill(hit_bank[i].time, wire);
      muPCxWireT = hit_bank[i].time;
      wireX = wire;
    }

    // Ywires
    if ((par >= 4051) && (par <= (4050+kMuPC1NumYWires))) {
      int wire = (par-4050);
      //printf("y hit! num %d out of %d; wire %d time %f \n",i,hit_bank_size,wire,hit_bank[i].time);

      // tdiffs between successive Ywire hits, per wire
      if (prev_hit_index_muPC1_Ywire[wire] >= 0) {
	muPC1_Ywire_successive_tdiffs->Fill((hit_bank[i].time -
					     hit_bank[prev_hit_index_muPC1_Ywire[wire]].time),
					    wire);
      }
      prev_hit_index_muPC1_Ywire[wire] = i;
      // 2D Ywire hits vs. time
            muPC1_YwireVStime_2D->Fill(hit_bank[i].time, wire);
      muPCyWireT = hit_bank[i].time;
      wireY = wire;

    }

    /*
    if (par==4101) {
      mu_4101_VStime->Fill(hit_bank[i].time);
      testT = hit_bank[i].time;
    }

    if (fabs(muPCxWireT-testT)<100){
      //printf(" mupc x time - 4101 %f, xwire %d\n",muPCxWireT-testT,wireX); 
      muPC_4101tCorrX->Fill(muPCxWireT, wireX);
    }
    if (fabs(muPCyWireT-testT)<100){
      //printf(" mupc y time - 4101 %f, ywire %d\n",muPCyWireT-testT,wireY);
      muPC_4101tCorrY->Fill(muPCyWireT, wireY);
    }
    */

  }

  /* ############################################### */

  return SUCCESS;
}
