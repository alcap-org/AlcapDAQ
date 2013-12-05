/********************************************************************\

  Name:         MMuSCAnalysisC.cpp
  Created by:   Tom Banks

  Contents:     Generates muSC histograms, and extracts important
                offset information from the ODB.

\********************************************************************/
                                                        
/*-- Include files -------------------------------------------------*/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1.h"
#include "TDirectory.h"

/* MuCap includes */
#include "common.h"
#include "ucb_common.h"
#include "uiuc_utils.h"

extern HNDLE hDB;
extern TDirectory *gManaHistsDir;

/*-- Parameters ----------------------------------------------------*/

double muSCCopy2TimeOffset;
double muSCATimeOffset;
double muSCRoutedTimeOffset[4];

/*-- Static parameters ---------------------------------------------*/

// Declare histograms
static TH1 *muSC_time;
static TH1 *muSC_copy1_successive_hit_tdiffs;
static TH1 *muSC_copy1_AD_successive_hit_tdiffs;
static TH1 *muSC_router_seq_errors;
static TH1 *muSC_error_blocks_skipped;

/*-- Module declaration --------------------------------------------*/

INT MMuSCAnalysisC_init(void);
INT MMuSCAnalysisC(EVENT_HEADER*, void*);

ANA_MODULE MMuSCAnalysisC_module = {
  "MMuSCAnalysisC",              /* module name           */  
  "Tom Banks",                   /* author                */
  MMuSCAnalysisC,                /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MMuSCAnalysisC_init,           /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/*-- init routine --------------------------------------------------*/

INT MMuSCAnalysisC_init(void)
{
  /* Read time offsets for the various copies of the muSC from the ODB */
  char key_name[80];
  int size,runnumber;
  sprintf(key_name, "Runinfo/Run number");
  size=sizeof(int);
  db_get_value(hDB,0,key_name, &runnumber, &size, TID_INT,1);
  // printf("==================\n\n\n in MMuSCAnalysisC_init\n runnumber is %d\n\n=============\n",runnumber);
  int PSI_runnumber=1;
  char stem[200];
  sprintf(stem,"/Experiment/Run%d",PSI_runnumber);
  printf("==================\n\n\n in MMuSCAnalysisC_init\n PSI runnumber is %d\n\n=============\n",PSI_runnumber); 
  sprintf(key_name, "%s/muSC/muSCCopy2TimeOffset",stem);
  size = sizeof(muSCCopy2TimeOffset);
  db_get_value(hDB, 0, key_name, &muSCCopy2TimeOffset, &size, TID_DOUBLE, 1);

  sprintf(key_name, "%s/muSC/muSCATimeOffset",stem);
  size = sizeof(muSCATimeOffset);
  db_get_value(hDB, 0, key_name, &muSCATimeOffset, &size, TID_DOUBLE, 1);

  sprintf(key_name, "%s/muSC/muSCRoutedTimeOffset",stem);
  size = sizeof(muSCRoutedTimeOffset);
  db_get_value(hDB, 0, key_name, &muSCRoutedTimeOffset, &size, TID_DOUBLE, 1);

  /****** Construct histograms ******/
  char hist_name[256];
  char hist_title[256];

  // muSC successive hits tdiff histogram
  sprintf(hist_name,  "muSC_copy1_successive_hit_tdiffs");
  sprintf(hist_title, "#muSC tdiffs between successive hits");
  muSC_copy1_successive_hit_tdiffs = new TH1D(hist_name, hist_title, 44201, -5000.625, 50250.625);

  // muSC time histogram                                                                                          
    sprintf(hist_name,  "muSC_time");
  sprintf(hist_title, "muSC time");
  muSC_time = new TH1D(hist_name, hist_title, 44201, -5000.625, 50250.625);
  
  // muSC successive hits tdiff histogram, after artificial deadtime (AD)
  sprintf(hist_name,  "muSC_copy1_AD_successive_hit_tdiffs");
  sprintf(hist_title, "#muSC tdiffs between successive AD hits");
  muSC_copy1_AD_successive_hit_tdiffs = new TH1D(hist_name, hist_title, 44201, -5000.625, 50250.625);

  // muSC router sequence error tracking
  sprintf(hist_name,  "muSC_router_seq_errors");
  sprintf(hist_title, "#muSC router sequence errors");
  muSC_router_seq_errors = new TH1D(hist_name, hist_title, 4, -0.5, 3.5);

  // Blocks skipped due to muSC copy1/copy2 count discrepancies
  sprintf(hist_name,  "blocks_skipped_muSC_matching_errors");
  sprintf(hist_title, "blocks skipped due to #muSC matching errors");
  muSC_error_blocks_skipped = new TH1D(hist_name, hist_title, 3, -0.5, 2.5);

 return SUCCESS;
}

/*-- event routine -------------------------------------------------*/

INT MMuSCAnalysisC(EVENT_HEADER *pheader, void *pevent)
{
  /* ############################################### */
  // Open up the "HITS" bank
  channel_hit *hit_bank;
  int hit_bank_size = bk_locate(pevent, "HITS", (DWORD *) &hit_bank);
  hit_bank_size = hit_bank_size * sizeof(DWORD) / sizeof(channel_hit);
  if (hit_bank == NULL) {
    fprintf(stderr, "MMuSCAnalysisC: Could not find bank HITS.\n");
  }

  /* ############################################### */
  // MuSC arrival histograms, with and w/o AD.
  int prevMuSChitindex = -1;
  int prevMuSCADhitindex = -1;
  for (int i=0; i<hit_bank_size; i++) {
    if (hit_bank[i].parameter == 6001) {
      muSC_time->Fill(hit_bank[i].time);
      // Successive tdiffs between muSC copy1 (parameter 6001) hits
      if (prevMuSChitindex >= 0) {
	double tdiff = hit_bank[i].time - 
	               hit_bank[prevMuSChitindex].time;
	muSC_copy1_successive_hit_tdiffs->Fill(tdiff);
      }
      prevMuSChitindex = i;

      // Successive tdiffs between AD muSC hits
      if (prevMuSCADhitindex >= 0) {
	double tdiff = hit_bank[i].time - 
	               hit_bank[prevMuSCADhitindex].time;
        if (tdiff > kMuSCADInterval) {
	  muSC_copy1_AD_successive_hit_tdiffs->Fill(tdiff);
	  prevMuSCADhitindex = i;
	}
      }
      if (prevMuSCADhitindex == -1) {
	prevMuSCADhitindex = i;
      }

    }
  }

  /* ############################################### */

  // check that the routed muSC always alternates 1-2-3-4
  int last_route_port = -1;
  double last_route_time = -1;
  for (int i=0; i<hit_bank_size; i++) {
    if (hit_bank[i].parameter >= 6006 && hit_bank[i].parameter <= 6009) {
      int port = hit_bank[i].parameter - 6006;
      if(last_route_port != -1 && port != (last_route_port+1)%4) {
	muSC_router_seq_errors->Fill(port);
      }
      last_route_port = port;
      last_route_time = hit_bank[i].time;
    }
  }

  /* ############################################### */

  // look for too many mismatched muSC copy 1 vs. copy 2 hits
  bool skip = false;
  for(int i = 1; i <= 2; i++) { 
    char name[80];
    sprintf(name, "muSC_copy%d_only_block", i);
    TH1 *only = (TH1 *) gManaHistsDir->Get(name);
    if(only == NULL) {
      fprintf(stderr, "Warning: Unable to find histogram %s\n", name);
      continue;
    }
    int bin = only->GetXaxis()->FindBin(pheader->serial_number);
    if(only->GetBinContent(bin) > kMuSCMismatchThreshold+0.5) {
      fprintf(stderr, "Skipping block %d: too many hits in muSC copy %d only (%f)\n", 
	      pheader->serial_number, i, only->GetBinContent(bin));
      skip = true;
    }
  }

  if(skip) {
    muSC_error_blocks_skipped->Fill(1);     
    return ANA_SKIP;
  } else {
    return SUCCESS;
  }
}
