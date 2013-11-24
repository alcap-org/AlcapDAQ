/********************************************************************\

Name:         MPulseWaveforms
Created by:   Andrew Edmonds

Contents:     A module to plot the waveforms of the pulses from each channel
              NB Plots ALL the pulses so only use to analyze a few events

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>

/* AlCap includes */
#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MPulseWaveforms_init(void);
INT  MPulseWaveforms(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static vector<TOctalFADCBankReader*> fadc_bank_readers;
map <std::string, std::vector<TH1I*> > waveform_histograms_map;

ANA_MODULE MPulseWaveforms_module =
{
	"MPulseWaveforms",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MPulseWaveforms,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MPulseWaveforms_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MPulseWaveforms_init()
{
  // This histogram has the pulse waveforms on the X-axis and the number of pulses on the Y-axis
  // One histogram is created for each detector
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.
  vector<string> bank_names = GetAllFADCBankNames();

  for(unsigned int i=0; i<bank_names.size(); i++) {
    fadc_bank_readers.push_back(new TOctalFADCBankReader(bank_names[i]));

    std::vector<TH1I*> waveform_vector;
    std::pair<std::string, std::vector<TH1I*> > thePair(bank_names[i], waveform_vector);
    waveform_histograms_map.insert(thePair);
  }

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MPulseWaveforms(EVENT_HEADER *pheader, void *pevent)
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
	for (map_iterator theMapIter = pulse_islands_map.begin(); theMapIter != pulse_islands_map.end(); theMapIter++) 
	{
	  std::string bankname = theMapIter->first;
	  std::vector<TPulseIsland*> thePulses = theMapIter->second;
			
	  // Loop over the TPulseIslands and plot the histogram
	  for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); pulseIter++) {
	    
			
	    // Find the relevant bank name from the detector name
	    if (waveform_histograms_map.find(bankname) != waveform_histograms_map.end()) {
	      std::string detname = gSetup->GetDetectorName(bankname);
	      std::stringstream histname;
	      histname << "h" << detname << "_Waveform_Block" << midas_event_number << "_Pulse" << pulseIter - thePulses.begin();
	      std::string histtitle = "Plot of the pulse waveforms for the " + detname + " detector";
	      TH1I* hDetWaveform = (*pulseIter)->GetPulseWaveform(histname.str(), histtitle);
	      hDetWaveform->GetXaxis()->SetTitle("Time");
	      hDetWaveform->GetYaxis()->SetTitle("ADC Value");

	      waveform_histograms_map[bankname].push_back(hDetWaveform);
	    }
	  }
	}
	return SUCCESS;
}
