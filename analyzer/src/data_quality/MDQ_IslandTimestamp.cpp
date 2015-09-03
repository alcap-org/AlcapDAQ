////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_IslandTimestamp
/// \ingroup lldq
/// \author Andrew Edmonds
///
/// \brief
/// Histograms the timestamps from each MIDAS event for each detector.
///
/// \details
/// Creates a number of histograms.
/// @{

/// \var DQ_IslandTimestamp_histograms_map
/// \brief
/// Map of bank name to histogram of time stamps within a MIDAS event. 

/// \var DQ_IslandTimestamp_histograms_normalised_map
/// \brief
/// Same as ::DQ_IslandTimestamp_histograms_map, but vertical axis is normalised
/// to number of muon hits according to TDC.
////////////////////////////////////////////////////////////////////////////////

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
INT  MDQ_IslandTimestamp_init(void);
INT  MDQ_IslandTimestamp(EVENT_HEADER*, void*);
INT  MDQ_IslandTimestamp_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH1F*> DQ_IslandTimestamp_histograms_map;
map <std::string, TH1F*> DQ_IslandTimestamp_histograms_normalised_map;

extern TH1F* hDQ_TDCCheck_nMuons;

ANA_MODULE MDQ_IslandTimestamp_module =
{
	"MDQ_IslandTimestamp",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_IslandTimestamp,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_IslandTimestamp_eor,                          /* EOR routine           */
	MDQ_IslandTimestamp_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MDQ_IslandTimestamp_init()
{
  // See if the DataQuality_LowLevel/ directory already exists
  if (!gDirectory->Cd("DQ_IslandTimestamp")) {
    
    std::string dir_name("DQ_IslandTimestamp/");
    gDirectory->mkdir(dir_name.c_str());
    gDirectory->Cd(dir_name.c_str());
  }

  // Create a histogram for each detector
  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    if(TSetupData::IsTDC(bankname)) continue;

    double bin_width = gSetup->GetClockTick(bankname) * 1000;
    double bin_max = 120e6;
    int n_bins = bin_max / bin_width;

    // hDQ_IslandTimestamp_[DetName]_[BankName]
    std::string histname = "hDQ_IslandTimestamp_" + detname + "_" + bankname;
    std::string histtitle = "Distribution of time stamps in " + detname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(), n_bins, 0, bin_max);
    hDQ_Histogram->GetXaxis()->SetTitle("Time Stamp [ns]");
    hDQ_Histogram->GetYaxis()->SetTitle("Number of TPulseIslands");
    DQ_IslandTimestamp_histograms_map[bankname] = hDQ_Histogram;

    // The normalised histogram
    histname += "_normalised";
    histtitle += " (normalised)";
    TH1F* hDQ_Histogram_Normalised = new TH1F(histname.c_str(), histtitle.c_str(), n_bins,0,bin_max);
    hDQ_Histogram_Normalised->GetXaxis()->SetTitle("Time Stamp [ns]");
    std::string yaxislabel = hDQ_Histogram->GetYaxis()->GetTitle();
    yaxislabel += " per TDC TSc Hit";
    hDQ_Histogram_Normalised->GetYaxis()->SetTitle(yaxislabel.c_str());
    DQ_IslandTimestamp_histograms_normalised_map[bankname] = hDQ_Histogram_Normalised;
  }

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}


/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_IslandTimestamp_eor(INT run_number) {

  // Some typedefs
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;
  
  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map =
    gData->fPulseIslandToChannelMap;

  // Loop over the map and get each bankname, vector pair
  for (map_iterator mapIter = pulse_islands_map.begin(); mapIter != pulse_islands_map.end(); ++mapIter) {

    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    if(TSetupData::IsTDC(bankname)) continue;      

    // Make sure the histograms exist and then fill them
    if (DQ_IslandTimestamp_histograms_normalised_map.find(bankname) != DQ_IslandTimestamp_histograms_normalised_map.end()) {
      DQ_IslandTimestamp_histograms_normalised_map[bankname]->Scale(1./hDQ_TDCCheck_nMuons->GetEntries());
    }
  }

  return SUCCESS;
}

/** This method fills the histograms
 */
INT MDQ_IslandTimestamp(EVENT_HEADER *pheader, void *pevent)
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

	// Loop over the map and get each bankname, vector pair
	for (map_iterator mapIter = pulse_islands_map.begin(); mapIter != pulse_islands_map.end(); ++mapIter) 
	{
	  std::string bankname = mapIter->first;
	  std::string detname = gSetup->GetDetectorName(bankname);
	  if(TSetupData::IsTDC(bankname)) continue;
	  std::vector<TPulseIsland*> thePulses = mapIter->second;

	  // Get the histograms before looping through the pulses
	  TH1F* hDQ_IslandTimestamp = DQ_IslandTimestamp_histograms_map[bankname];
	  TH1F* hDQ_IslandTimestamp_Norm = DQ_IslandTimestamp_histograms_normalised_map[bankname];
			
	  // Loop over the TPulseIslands and plot the histogram
	  for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); ++pulseIter) {

	    // Make sure the histograms exist and then fill them
	    if (DQ_IslandTimestamp_histograms_map.find(bankname) != DQ_IslandTimestamp_histograms_map.end()) {
	      int time_stamp = (*pulseIter)->GetTimeStamp();
	      double clock_tick_in_ns = (*pulseIter)->GetClockTickInNs();
	      double block_time = time_stamp * clock_tick_in_ns;

	      hDQ_IslandTimestamp->Fill(block_time);
	      hDQ_IslandTimestamp_Norm->Fill(block_time);
	    }
	  }
	}
	return SUCCESS;
}

/// @}
