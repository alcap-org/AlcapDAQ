/********************************************************************\

Name:         MDQ_TDCCheck
Created by:   Andrew Edmonds

Contents:     hDQ_TDCCheck_[DetName] 
               - Plots: the time stamp (in ns) for each TPulseIsland
               - To Check: time stamps go up to ~100 ms
               - Soln: if they don't, check the sampling frequency in the ODB

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
INT  MDQ_TDCCheck_init(void);
INT  MDQ_TDCCheck(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH1F*> DQ_TDCCheck_histograms_map;

ANA_MODULE MDQ_TDCCheck_module =
{
	"MDQ_TDCCheck",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_TDCCheck,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MDQ_TDCCheck_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_TDCCheck_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Create a histogram for each detector
  /*  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    // hDQ_TDCCheck_[DetName]
    std::string histname = "hDQ_TDCCheck_" + detname;
    std::string histtitle = "Distribution of time stamps in " + detname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(), 1200, 0, 120e6);
    hDQ_Histogram->GetXaxis()->SetTitle("Time Stamp [ns]");
    hDQ_Histogram->GetYaxis()->SetTitle("Number of TPulseIslands");
    DQ_TDCCheck_histograms_map[bankname] = hDQ_Histogram;
  }
  */
  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_TDCCheck(EVENT_HEADER *pheader, void *pevent)
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
        printf("Number of hits: %d\n", hit_bank_size);

	// At the moment just loop through the hits and print the information
	// Parameter values have been obtained from MMuSCAnalysisMQL.cpp and MMuPC1AnalysisMQL.cpp
	for (int i = 0; i < hit_bank_size; ++i) {
	  if (hit_bank[i].parameter == 6011)
	    printf("muSC hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	  else if (hit_bank[i].parameter == 6002)
	    printf("muSCA hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	  else if (hit_bank[i].parameter >= 4001 && hit_bank[i].parameter <= 4074)
	    printf("muPC hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	  else
	    printf("Unknown hit! Hit #%d: time = %f, parameter = %d\n", i, hit_bank[i].time, hit_bank[i].parameter);
	}

	// Loop over the map and get each bankname, vector pair
	/*	for (map_iterator mapIter = pulse_islands_map.begin(); mapIter != pulse_islands_map.end(); ++mapIter) 
	{
	  std::string bankname = mapIter->first;
	  std::string detname = gSetup->GetDetectorName(bankname);
	  std::vector<TPulseIsland*> thePulses = mapIter->second;
			
	  // Loop over the TPulseIslands and plot the histogram
	  for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); ++pulseIter) {

	    // Make sure the histograms exist and then fill them
	    if (DQ_TDCCheck_histograms_map.find(bankname) != DQ_TDCCheck_histograms_map.end()) {
	      int time_stamp = (*pulseIter)->GetTimeStamp();
	      double clock_tick_in_ns = (*pulseIter)->GetClockTickInNs();
	      double block_time = time_stamp * clock_tick_in_ns;

	      DQ_TDCCheck_histograms_map[bankname]->Fill(block_time);
	    }
	  }
	  }*/
	return SUCCESS;
}
