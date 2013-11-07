/********************************************************************\

  Name:         MSiPulsePlot
  Created by:   Andrew Edmonds

  Contents:     A module to plot the pulses of the silicon detector

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <cmath>
#include <iostream>
#include <sstream>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>

/* AlCap includes */
#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"
#include "TSimpleSiPulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MSiPulsePlot_init(void);
INT  MSiPulsePlot(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

vector<TSimpleSiPulse*> GetPulsesFromIsland(TSimpleSiPulse* island);
TH1* PlotPulse(TSimpleSiPulse* pulse_to_plot, std::string bankname);

extern HNDLE hDB;
extern TGlobalData* gData;

vector<TH1*> islandPlots;
vector< vector<TH1*> > pulsePlots; 

static vector<TOctalFADCBankReader*> fadc_bank_readers;

ANA_MODULE MSiPulsePlot_module =
{
  "MSiPulsePlot",                    /* module name           */
  "Andrew Edmonds",              /* author                */
  MSiPulsePlot,                      /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MSiPulsePlot_init,                 /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MSiPulsePlot_init()
{
  vector<string> bank_names = GetAllFADCBankNames();

  for(unsigned int i=0; i<bank_names.size(); i++) {
    fadc_bank_readers.push_back(new TOctalFADCBankReader(bank_names[i]));
  }

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
  * of TOctalFADCIsland objects from the raw Octal FADC data.
  */
INT MSiPulsePlot(EVENT_HEADER *pheader, void *pevent)
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

  // Iterate through the banks readers
  for (std::vector<TOctalFADCBankReader*>::iterator bankReaderIter = fadc_bank_readers.begin();
  	   bankReaderIter != fadc_bank_readers.end(); bankReaderIter++) {
  	   
  	   (*bankReaderIter)->ProcessEvent(pheader, pevent);
  	   std::vector<TOctalFADCIsland*> theOctalFADCIslands = (*bankReaderIter)->GetIslandVectorCopy();
  	   
  	   // Loop over the islands
  	   for (std::vector<TOctalFADCIsland*>::iterator octalFADCIslandIter = theOctalFADCIslands.begin();
  	   		octalFADCIslandIter != theOctalFADCIslands.end(); octalFADCIslandIter++) {
  	   		// Create a TSimpleSiPulse for the island
  	   		unsigned int nped = 10;
  	   		TSimpleSiPulse* simple_si_island = new TSimpleSiPulse((*octalFADCIslandIter), nped);
    		
    		// Find all the pulses in this island
  	   		vector<TSimpleSiPulse*> pulses_on_island = GetPulsesFromIsland(simple_si_island);
  	   		
  	   		// Plot the island
  	   		std::string bankname = (*bankReaderIter)->GetBankName();
  	   		TH1I* hIslandPlot = (TH1I*) PlotPulse(simple_si_island, bankname);
  	   		
  	   		// Change the title of the histogram to add the island number
  	   		std::stringstream histname;
  	   		int island_number = octalFADCIslandIter - theOctalFADCIslands.begin() + 1;
  	   		histname << hIslandPlot->GetName() << "_Island" << island_number;
  	   		hIslandPlot->SetName(histname.str().c_str());
  	   		
  	   		// Add the histogram to the vector of island plots
  	   		islandPlots.push_back(hIslandPlot);
  	   		
  	   		// Loop through and add the pulses to the main pulses vector
  	   		std::vector<TH1*> pulsesOnIslandPlots;
  	   		for (std::vector<TSimpleSiPulse*>::iterator iter = pulses_on_island.begin(); 
  	   				iter != pulses_on_island.end(); iter++) {
  	   		
  	   			// create the histogram
  	   			TH1I* hPulsePlot = (TH1I*) PlotPulse(*iter, bankname);
  	   			
  	   			// Change the title of the histogram to add the island number and pulse number
  	   			std::stringstream histname;
  	   			histname << hPulsePlot->GetName() << "_Island" << island_number << "_Pulse" << iter - pulses_on_island.begin() + 1;
  	   			hPulsePlot->SetName(histname.str().c_str());
  	   			
  	   			pulsesOnIslandPlots.push_back(hPulsePlot);
  	   		}
  	   		
  	   		// Add the vector of plots for this island to the main vector
  	   		pulsePlots.push_back(pulsesOnIslandPlots);
  	   }  	   
  }

  return SUCCESS;
}

// Plot the pulse given on a histogram and then return it
TH1* PlotPulse(TSimpleSiPulse* pulse_to_plot, std::string bankname) {
	
	std::string detector;
	if (bankname == "Nac0")
		detector = "hSiL2Slow_Heights";
	else if (bankname == "Nbc0")
		detector = "hSiR2Slow";
	
	else if (bankname == "Nec0")
		detector = "hSiL2Fast";
	else if (bankname == "Nfc0")
		detector = "hSiR2Fast";
	
	else if (bankname == "Nae0")
		detector = "hSiL1_1Fast";
	else if (bankname == "Nbe0")
		detector = "hSiL1_2Fast";
	else if (bankname == "Nce0")
		detector = "hSiL1_3Fast";
	else if (bankname == "Nde0")
		detector = "hSiL1_4Fast";
	else if (bankname == "Nee0")
		detector = "hSiR1_1Fast";
	else if (bankname == "Nae0")
		detector = "hSiR1_2Fast";
	else if (bankname == "Nge0")
		detector = "hSiR1_3Fast";
	else if (bankname == "Nhe0")
		detector = "hSiR1_4Fast";
	
	
	else if (bankname == "Naf0")
		detector = "hSiL1_1Slow";
	else if (bankname == "Nbf0")
		detector = "hSiL1_2Slow";
	else if (bankname == "Ncf0")
		detector = "hSiL1_3Slow";
	else if (bankname == "Ndf0")
		detector = "hSiL1_4Slow";
	else if (bankname == "Nef0")
		detector = "hSiR1_1Slow";
	else if (bankname == "Nff0")
		detector = "hSiR1_2Slow";
	else if (bankname == "Ngf0")
		detector = "hSiR1_3Slow";
	else if (bankname == "Nhf0")
		detector = "hSiR1_4Slow";
	
	TH1I* hPlot = new TH1I(detector.c_str(), detector.c_str(), 100, 0, 100);
	
	// Get the samples and fill the histogram
	std::vector<int> theSamples = pulse_to_plot->GetSampleVector();
	
	for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
	
		hPlot->Fill(sampleIter - theSamples.begin(), (*sampleIter));
	
	}
	
	return hPlot;
}
