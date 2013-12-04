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
  /****** Construct histograms ******/
  char hist_name[256];
  char hist_title[256];

  // muPC1 Xwire histograms
  sprintf(hist_name,  "muPC1_X_raw_successive_tdiffs");
  sprintf(hist_title, "Tdiffs between successive raw #muPC1 Xwire hits");
  muPC1_Xwire_successive_tdiffs = new TH2D(hist_name, hist_title, 1451, -252.5, 7002.5, kMuPC1NumXWires, 0.5, (kMuPC1NumXWires+0.5));
  /*
  sprintf(hist_name,  "muPC1_XwireVStime_2D");
  sprintf(hist_title, "#muPC1 raw Xwire hits vs.time 2D");
  muPC1_XwireVStime_2D = new TH2D(hist_name, hist_title, 1000, 0, 50000000, kMuPC1NumXWires, 0.5, (kMuPC1NumXWires+0.5));
  */

  // muPC1 Ywire histograms
  sprintf(hist_name,  "muPC1_Y_raw_successive_tdiffs");
  sprintf(hist_title, "Tdiffs between successive raw #muPC1 Ywire hits");
  muPC1_Ywire_successive_tdiffs = new TH2D(hist_name, hist_title, 1451, -252.5, 7002.5, kMuPC1NumYWires, 0.5, (kMuPC1NumYWires+0.5));
  /*
  sprintf(hist_name,  "muPC1_YwireVStime_2D");
  sprintf(hist_title, "#muPC1 raw Ywire hits vs.time 2D");
  muPC1_YwireVStime_2D = new TH2D(hist_name, hist_title, 1000, 0, 50000000, kMuPC1NumYWires, 0.5, (kMuPC1NumYWires+0.5));
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

  // Loop over hits within hit_bank and fill muPC1 raw data histograms
  for (int i=0; i<hit_bank_size; i++) {
    int par = hit_bank[i].parameter;

    // Xwires
    if ((par >= 4001) && (par <= (4000+kMuPC1NumXWires))) {
      int wire = (par-4000);
      // tdiffs between successive Xwire hits, per wire
      if (prev_hit_index_muPC1_Xwire[wire] >= 0) {
	muPC1_Xwire_successive_tdiffs->Fill((hit_bank[i].time -
					     hit_bank[prev_hit_index_muPC1_Xwire[wire]].time),
					    wire);
      }
      prev_hit_index_muPC1_Xwire[wire] = i;
      // 2D Xwire hits vs. time
      //      muPC1_XwireVStime_2D->Fill(hit_bank[i].time, wire);
    }

    // Ywires
    if ((par >= 4051) && (par <= (4050+kMuPC1NumYWires))) {
      int wire = (par-4050);
      // tdiffs between successive Ywire hits, per wire
      if (prev_hit_index_muPC1_Ywire[wire] >= 0) {
	muPC1_Ywire_successive_tdiffs->Fill((hit_bank[i].time -
					     hit_bank[prev_hit_index_muPC1_Ywire[wire]].time),
					    wire);
      }
      prev_hit_index_muPC1_Ywire[wire] = i;
      // 2D Ywire hits vs. time
      //      muPC1_YwireVStime_2D->Fill(hit_bank[i].time, wire);
    }
  }

  /* ############################################### */

  return SUCCESS;
}
