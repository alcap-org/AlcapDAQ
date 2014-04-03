/********************************************************************\

Name:         MDQ_muScTDiff
Created by:   Andrew Edmonds

Contents:     hDQ_muScTDiff_[DetName] 
               - Plots: the time differences (in ns) between the pulses in the muSc and each detector
               - To Check: the time shift for the ODB correction

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
INT  MDQ_muScTDiff_init(void);
INT  MDQ_muScTDiff(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH1F*> DQ_muScTDiff_histograms_map;

ANA_MODULE MDQ_muScTDiff_module =
{
	"MDQ_muScTDiff",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_muScTDiff,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MDQ_muScTDiff_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_muScTDiff_init()
{
    // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DataQuality_LowLevel")) {
    
    std::string dir_name("DataQuality_LowLevel/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Create a histogram for each detector
  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    // hDQ_muScTDiff_[DetName]
    std::string histname = "hDQ_muScTDiff_" + detname;
    std::string histtitle = "Time differences between muSc and " + detname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(), 10000, -50000, 50000);
    std::string axislabel = "Time Difference (muSc - " + detname + ") [ns]";
    hDQ_Histogram->GetXaxis()->SetTitle(axislabel.c_str());
    hDQ_Histogram->GetYaxis()->SetTitle("Number of TPulseIslands");
    DQ_muScTDiff_histograms_map[bankname] = hDQ_Histogram;
  }

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_muScTDiff(EVENT_HEADER *pheader, void *pevent)
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

	// Get the muSc pulses
	std::string muSc_bankname = gSetup->GetBankName("muSc");
	std::vector<TPulseIsland*> theMuScPulses = pulse_islands_map[muSc_bankname];

	// Loop over the muSc pulses
	for (std::vector<TPulseIsland*>::iterator muScPulseIter = theMuScPulses.begin(); muScPulseIter != theMuScPulses.end(); ++muScPulseIter) {

	  // Loop over the map and get each bankname, vector pair
	  for (map_iterator mapIter = pulse_islands_map.begin(); mapIter != pulse_islands_map.end(); ++mapIter) 
	    {
	      std::string bankname = mapIter->first;
	      std::string detname = gSetup->GetDetectorName(bankname);
	      
	      // Don't bother comparing muSc to muSc
	      if (detname == "muSc")
		continue;

	      std::vector<TPulseIsland*> thePulses = mapIter->second;
			
	      // Loop over the TPulseIslands and plot the histogram
	      for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); ++pulseIter) {


		// Make sure the histograms exist and then fill them
		if (DQ_muScTDiff_histograms_map.find(bankname) != DQ_muScTDiff_histograms_map.end()) {

		  // Get the pulse times for the muSc pulse and the detector pulse
		  double muSc_time = (*muScPulseIter)->GetPulseTime();
		  double det_time = (*pulseIter)->GetPulseTime();
		  double tdiff = muSc_time - det_time;

		  DQ_muScTDiff_histograms_map[bankname]->Fill(tdiff);
		}
	      }
	    }
	}
	return SUCCESS;
}
