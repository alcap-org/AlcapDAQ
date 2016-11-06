////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDQ_muScTDiff
/// \ingroup lldq
/// \author Andrew Edmonds
///
/// \brief
/// Correlations of TPI timestamps with timestamps in muSc.
///
/// \details
/// Creates a number of histograms. The muSc histogram exists but remains
/// unfilled.
/// @{

/// \var DQ_muScTDiff_histograms_map;
/// \brief
/// Map of bank name to time stamp correlation with muSc TPI time stamps.
/// The muSc histogram exists but is empty.

/// \var DQ_muScTDiff_histograms_normalised_map
/// \brief
/// Same as ::DQ_muScTDiff_histograms_map, but normalized to number of
/// muon hits according to TDC.

/// \var DQ_muScTDiff_histograms_with_time_shift_map
/// \brief
/// Same as ::DQ_muScTDiff_histograms_map, except each TPI's time stamp has had
/// the expected timing correlation peak subtracted off. If correct, the peak
/// should be at zero in these plots.
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
INT  MDQ_muScTDiff_init(void);
INT  MDQ_muScTDiff(EVENT_HEADER*, void*);
INT  MDQ_muScTDiff_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

map <std::string, TH1F*> DQ_muScTDiff_histograms_map;
map <std::string, TH1F*> DQ_muScTDiff_histograms_normalised_map;
map <std::string, TH1F*> DQ_muScTDiff_histograms_with_time_shift_map;
float axis_limit = 50000;

//extern TH1F* hDQ_TDCCheck_muSc;

ANA_MODULE MDQ_muScTDiff_module =
{
	"MDQ_muScTDiff",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MDQ_muScTDiff,                      /* event routine         */
	NULL,                          /* BOR routine           */
	MDQ_muScTDiff_eor,                          /* EOR routine           */
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

    // hDQ_muScTDiff_[DetName]_[BankName]
    std::string histname = "hDQ_muScTDiff_" + detname + "_" + bankname;
    std::string histtitle = "Time differences between muSc and " + detname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(), 20000, -axis_limit, axis_limit);
    std::string axislabel = "Time Difference (muSc - " + detname + ") [ns]";
    hDQ_Histogram->GetXaxis()->SetTitle(axislabel.c_str());
    hDQ_Histogram->GetYaxis()->SetTitle("Number of TPulseIslands");
    DQ_muScTDiff_histograms_map[bankname] = hDQ_Histogram;

    // The normalised histogram
    std::string normhistname = histname + "_normalised";
    std::string normhisttitle = histtitle + " (normalised)";
    TH1F* hDQ_Histogram_Normalised = new TH1F(normhistname.c_str(), normhisttitle.c_str(), 20000, -axis_limit, axis_limit);
    hDQ_Histogram_Normalised->GetXaxis()->SetTitle(axislabel.c_str());
    std::string yaxislabel = hDQ_Histogram->GetYaxis()->GetTitle();
    yaxislabel += " per TDC muSc Hit";
    hDQ_Histogram_Normalised->GetYaxis()->SetTitle(yaxislabel.c_str());
    DQ_muScTDiff_histograms_normalised_map[bankname] = hDQ_Histogram_Normalised;

    // The histogram with the time shifts
    std::string timeshift_histname = histname + "_with-time-shift";
    std::string timeshift_histtitle = histtitle + " (with time shift)";
    TH1F* hDQ_Histogram_TimeShift = new TH1F(timeshift_histname.c_str(), timeshift_histtitle.c_str(), 20000, -axis_limit, axis_limit);
    hDQ_Histogram_TimeShift->GetXaxis()->SetTitle(axislabel.c_str());
    yaxislabel = hDQ_Histogram->GetYaxis()->GetTitle();
    hDQ_Histogram_Normalised->GetYaxis()->SetTitle(yaxislabel.c_str());
    DQ_muScTDiff_histograms_with_time_shift_map[bankname] = hDQ_Histogram_TimeShift;
  }

  gDirectory->Cd("/MidasHists/");
  return SUCCESS;
}

/** This method does any last minute things to the histograms at the end of the run
 */
INT MDQ_muScTDiff_eor(INT run_number) {

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
      
//    if (DQ_muScTDiff_histograms_normalised_map.find(bankname) != DQ_muScTDiff_histograms_normalised_map.end()) {
//
//      // Normalise to the muSc hits
//      DQ_muScTDiff_histograms_normalised_map[bankname]->Scale(1./hDQ_TDCCheck_muSc->GetEntries());
//    }
  }

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
	std::string muSc_bankname = gSetup->GetBankName("SiT-1-F");
	std::vector<TPulseIsland*> theMuScPulses = pulse_islands_map[muSc_bankname];

	// Loop over the map and get each bankname, vector pair
	for (map_iterator mapIter = pulse_islands_map.begin(); mapIter != pulse_islands_map.end(); ++mapIter) 
	  {
	    std::string bankname = mapIter->first;
	    std::string detname = gSetup->GetDetectorName(bankname);
	    
	    // Don't bother comparing muSc to muSc
	    if (detname == "SiT-1-F")
	      continue;

	    std::vector<TPulseIsland*> thePulses = mapIter->second;
			
	    // Make sure the histograms exist (put here so that it find() only called once per detector)
	    if (DQ_muScTDiff_histograms_map.find(bankname) != DQ_muScTDiff_histograms_map.end()) {
	      
	      // Get the histograms before looping through all the pulses
	      TH1F* hDQ_muScTDiff = DQ_muScTDiff_histograms_map[bankname];
	      TH1F* hDQ_muScTDiff_Norm = DQ_muScTDiff_histograms_normalised_map[bankname];
	      TH1F* hDQ_muScTDiff_TimeShift = DQ_muScTDiff_histograms_with_time_shift_map[bankname];
	      double time_shift = TSetupData::Instance()->GetTimeShift(bankname);

	      // Loop over the muSc pulses
	      for (std::vector<TPulseIsland*>::iterator muScPulseIter = theMuScPulses.begin(); muScPulseIter != theMuScPulses.end(); ++muScPulseIter) {

		// Loop over the TPulseIslands and plot the histogram
		for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); ++pulseIter) {

		  // Get the pulse times for the muSc pulse and the detector pulse
		  double muSc_time = (*muScPulseIter)->GetPulseTime();
		  double det_time = (*pulseIter)->GetPulseTime();
		  double tdiff = muSc_time - det_time;

		  // The pulses should be time-ordered so if the tdiff goes outside of the axis range, then we can just skip to the next muSc pulse
		  if (std::fabs(tdiff) > 2*axis_limit)
		    break;

		  // Fill the histogram
		  hDQ_muScTDiff->Fill(tdiff);
		  hDQ_muScTDiff_Norm->Fill(tdiff);
		  hDQ_muScTDiff_TimeShift->Fill(tdiff+time_shift);
		}
	      }
	    }
	  }
	return SUCCESS;
}

/// @}
