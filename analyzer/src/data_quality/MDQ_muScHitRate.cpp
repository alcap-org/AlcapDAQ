/********************************************************************\

Name:         MDQ_muScHitRate
Created by:   Andrew Edmonds

Contents:     hDQ_muScHitRate_muSc
               - Plots: the number of muSc hits as seen in the TDC (parameter = 6011)

              hDQ_muScHitRate_muScA
               - Plots: the number of muScA hits as seen in the TDC (parameter = 6002)

              hDQ_muScHitRate_muPC
               - Plots: the number of muPC hits as seen in the TDC (4001 <= parameter <= 4074)

              hDQ_muScHitRate_Unknown
               - Plots: the number of hits in the TDC with any other parameter

              hDQ_muScHitRate_muSc_time
               - Plots: the time of the muSc hits as seen in the TDC

              hDQ_muScHitRate_TDiff
               - Plots: the time difference between muSc hits in the TDC and the BU CAEN

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>

/* MIDAS includes */
#include "midas.h"

/* MuCap includes */
#include "../muSC_muPC/common.h" // for channel_hit

/* ROOT includes */
#include <TH1.h>
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MDQ_muScHitRate_init(void);
INT  MDQ_muScHitRate(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

TH1F* hDQ_muScHitRate_muSc;
TH1F* hDQ_muScHitRate_muScA;
TH1F* hDQ_muScHitRate_muPC;
TH1F* hDQ_muScHitRate_Unknown;

TH1F* hDQ_muScHitRate_muSc_time;
TH1F* hDQ_muScHitRate_TDiff;


ANA_MODULE MDQ_muScHitRate_module =
{
	"MDQ_muScHitRate",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_muScHitRate,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MDQ_muScHitRate_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_muScHitRate_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Create some histograms
  hDQ_muScHitRate_muSc = new TH1F("hDQ_muScHitRate_muSc", "Number of hits in muSc", 7000,0,7000);
  hDQ_muScHitRate_muSc->GetXaxis()->SetTitle("TDC Parameter");
  hDQ_muScHitRate_muSc->GetYaxis()->SetTitle("Number of Hits");

  hDQ_muScHitRate_muScA = new TH1F("hDQ_muScHitRate_muScA", "Number of hits in muScA", 7000,0,7000);
  hDQ_muScHitRate_muScA->GetXaxis()->SetTitle("TDC Parameter");
  hDQ_muScHitRate_muScA->GetYaxis()->SetTitle("Number of Hits");

  hDQ_muScHitRate_muPC = new TH1F("hDQ_muScHitRate_muPC", "Number of hits in muPC", 7000,0,7000);
  hDQ_muScHitRate_muPC->GetXaxis()->SetTitle("TDC Parameter");
  hDQ_muScHitRate_muPC->GetYaxis()->SetTitle("Number of Hits");

  hDQ_muScHitRate_Unknown = new TH1F("hDQ_muScHitRate_Unknown", "Number of hits in Unknown", 7000,0,7000);
  hDQ_muScHitRate_Unknown->GetXaxis()->SetTitle("TDC Parameter");
  hDQ_muScHitRate_Unknown->GetYaxis()->SetTitle("Number of Hits");

  hDQ_muScHitRate_muSc_time = new TH1F("hDQ_muScHitRate_muSc_time", "Time of TDC hits in muSc", 1200,0,120e6);
  hDQ_muScHitRate_muSc_time->GetXaxis()->SetTitle("Time of muSc Hit [ns]");
  hDQ_muScHitRate_muSc_time->GetYaxis()->SetTitle("Number of Hits");

  hDQ_muScHitRate_TDiff = new TH1F("hDQ_muScHitRate_TDiff", "Time difference between muSc hit in TDC and BU", 10000,-5000,5000);
  hDQ_muScHitRate_TDiff->GetXaxis()->SetTitle("Time Difference of muSc Hits (BU CAEN - TDC)");
  hDQ_muScHitRate_TDiff->GetYaxis()->SetTitle("Number of Hits");

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_muScHitRate(EVENT_HEADER *pheader, void *pevent)
{
	// Get the event number
	int midas_event_number = pheader->serial_number;

	// Some typedefs
	typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
	typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
	typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

	// Fetch a reference to the gData structure that stores a map
	// of (bank_name, vector<TPulseIsland*>) pairs
	TStringPulseIslandMap& pulse_islands_map =
		gData->fPulseIslandToChannelMap;

	// Get a pointer to the hit data in the TDC
	// NB copied and modified from MMuSCAnalysisC.cpp
	channel_hit *hit_bank;
	int hit_bank_size = bk_locate(pevent, "HITS", (DWORD *) &hit_bank);
	hit_bank_size = hit_bank_size * sizeof(DWORD) / sizeof(channel_hit);

	// At the moment just loop through the hits and print the information
	// Parameter values have been obtained from MMuSCAnalysisMQL.cpp and MMuPC1AnalysisMQL.cpp
	for (int i = 0; i < hit_bank_size; ++i) {
	  if (hit_bank[i].parameter == 6011) {
	    //	    printf("muSC hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	    hDQ_muScHitRate_muSc->Fill(hit_bank[i].parameter);
	    hDQ_muScHitRate_muSc_time->Fill(hit_bank[i].time);

	    // Plot the time difference between the time as given by the TDC and as given by the BU CAEN
	    std::string detname = "muSc";
	    std::string bankname = gSetup->GetBankName(detname);
	    std::vector<TPulseIsland*> theMuScPulses = pulse_islands_map[bankname];

	    // Loop over the TPulseIslands and plot the histogram
	    for (std::vector<TPulseIsland*>::iterator pulseIter = theMuScPulses.begin(); pulseIter != theMuScPulses.end(); ++pulseIter) {

	      // Get the timestamp of the TPI in ns
	      int time_stamp = (*pulseIter)->GetTimeStamp();
	      double clock_tick_in_ns = (*pulseIter)->GetClockTickInNs();
	      double block_time = time_stamp * clock_tick_in_ns;

	      hDQ_muScHitRate_TDiff->Fill(block_time - hit_bank[i].time);
	    }
	  }

	  else if (hit_bank[i].parameter == 6002)
	    //	    printf("muSCA hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	    hDQ_muScHitRate_muScA->Fill(hit_bank[i].parameter);

	  else if (hit_bank[i].parameter >= 4001 && hit_bank[i].parameter <= 4074)
	    //	    printf("muPC hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	    hDQ_muScHitRate_muPC->Fill(hit_bank[i].parameter);

	  else
	    //	    printf("Unknown hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	    hDQ_muScHitRate_Unknown->Fill(hit_bank[i].parameter);
	}

	return SUCCESS;
}
