/********************************************************************\

Name:         MSiPulsePlot
Created by:   Andrew Edmonds

Contents:     A module to plot the pulse Plots of the silicon detector

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

extern HNDLE hDB;
extern TGlobalData* gData;
extern map<string, vector<TSimpleSiPulse*> > theSimpleSiPulseMap;

static vector<TH1*> hSiL2Slow_PulsePlots; // Bank: Nac0
static vector<TH1*> hSiR2Slow_PulsePlots; // Bank: Nbc0

static vector<TH1*> hSiL2Fast_PulsePlots; // Bank: Nec0
static vector<TH1*> hSiR2Fast_PulsePlots; // Bank: Nfc0

static vector<TH1*> hSiL1_1Fast_PulsePlots; // Bank: Nae0
static vector<TH1*> hSiL1_2Fast_PulsePlots; // Bank: Nbe0
static vector<TH1*> hSiL1_3Fast_PulsePlots; // Bank: Nce0
static vector<TH1*> hSiL1_4Fast_PulsePlots; // Bank: Nde0
static vector<TH1*> hSiR1_1Fast_PulsePlots; // Bank: Nee0
static vector<TH1*> hSiR1_2Fast_PulsePlots; // Bank: Nfe0
static vector<TH1*> hSiR1_3Fast_PulsePlots; // Bank: Nge0
static vector<TH1*> hSiR1_4Fast_PulsePlots; // Bank: Nhe0

static vector<TH1*> hSiL1_1Slow_PulsePlots; // Bank: Naf0
static vector<TH1*> hSiL1_2Slow_PulsePlots; // Bank: Nbf0
static vector<TH1*> hSiL1_3Slow_PulsePlots; // Bank: Ncf0
static vector<TH1*> hSiL1_4Slow_PulsePlots; // Bank: Ndf0
static vector<TH1*> hSiR1_1Slow_PulsePlots; // Bank: Nef0
static vector<TH1*> hSiR1_2Slow_PulsePlots; // Bank: Nff0
static vector<TH1*> hSiR1_3Slow_PulsePlots; // Bank: Ngf0
static vector<TH1*> hSiR1_4Slow_PulsePlots; // Bank: Nhf0

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

	// Loop over the map and get each bankname, vector pair
	for (std::map<string, vector<TSimpleSiPulse*> >::iterator simpleSiMapIter = theSimpleSiPulseMap.begin();
			simpleSiMapIter != theSimpleSiPulseMap.end(); 
			simpleSiMapIter++) 
	{
		std::string bankname = simpleSiMapIter->first;
		std::vector<TSimpleSiPulse*> theSiPulses = simpleSiMapIter->second;
			
		// Loop over the TSimpleSiPulses and plot the histogram
		for (std::vector<TSimpleSiPulse*>::iterator siPulse = theSiPulses.begin(); siPulse != theSiPulses.end(); siPulse++) {
			
			// Create a histogram for the pulse plot
			int n_bins = 100; int x_min = 0; int x_max = 100;
			TH1I* hPulsePlot = new TH1I("Placeholder", "Placeholder", n_bins, x_min, x_max);
			hPulsePlot->SetBit(TH1::kCanRebin);

			// Fill the histogram
			std::vector<int> theSamples = (*siPulse)->GetSampleVector();
			for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
			
				hPulsePlot->Fill(sampleIter - theSamples.begin(), (*sampleIter));
			
			}
			
			// Push it back into the appropriate vector
			if (strcmp(bankname.c_str(), "Nac0P") == 0) {
				std::stringstream histname;
				histname << "hSiL2Slow_Pulse" << hSiL2Slow_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiL2 slow detector");
				hSiL2Slow_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nbc0P") == 0) {
				std::stringstream histname;
				histname << "hSiR2Slow_Pulse" << hSiR2Slow_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiR2 slow detector");
				hSiR2Slow_PulsePlots.push_back(hPulsePlot);
			}

			else if (strcmp(bankname.c_str(), "Nec0P") == 0){
				std::stringstream histname;
				histname << "hSiL2Fast_Pulse" << hSiL2Fast_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiL2 fast detector");
				hSiL2Fast_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nfc0P") == 0){
				std::stringstream histname;
				histname << "hSiR2Fast_Pulse" << hSiR2Fast_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiR2 fast detector");
				hSiR2Fast_PulsePlots.push_back(hPulsePlot);
			}

			else if (strcmp(bankname.c_str(), "Nae0P") == 0){
				std::stringstream histname;
				histname << "hSiL1_1Fast_Pulse" << hSiL1_1Fast_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiL1_1 fast detector");
				hSiL1_1Fast_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nbe0P") == 0){
				std::stringstream histname;
				histname << "hSiL1_2Fast_Pulse" << hSiL1_2Fast_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiL1_2 fast detector");
				hSiL1_2Fast_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nce0P") == 0){
				std::stringstream histname;
				histname << "hSiL1_3Fast_Pulse" << hSiL1_3Fast_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiL1_3 fast detector");
				hSiL1_3Fast_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nde0P") == 0){
				std::stringstream histname;
				histname << "hSiL1_4Fast_Pulse" << hSiL1_4Fast_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiL1_4 fast detector");
				hSiL1_4Fast_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nee0P") == 0){
				std::stringstream histname;
				histname << "hSiR1_1Fast_Pulse" << hSiR1_1Fast_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiR1_1 fast detector");
				hSiR1_1Fast_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nfe0P") == 0){
				std::stringstream histname;
				histname << "hSiR1_2Fast_Pulse" << hSiR1_2Fast_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiR1_2 fast detector");
				hSiR1_2Fast_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nge0P") == 0){
				std::stringstream histname;
				histname << "hSiR1_3Fast_Pulse" << hSiR1_3Fast_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiR1_3 fast detector");
				hSiR1_3Fast_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nhe0P") == 0){
				std::stringstream histname;
				histname << "hSiR1_4Fast_Pulse" << hSiR1_4Fast_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiR1_4 fast detector");
				hSiR1_4Fast_PulsePlots.push_back(hPulsePlot);
			}

			else if (strcmp(bankname.c_str(), "Naf0P") == 0){
				std::stringstream histname;
				histname << "hSiL1_1Slow_Pulse" << hSiL1_1Slow_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiL1_1 slow detector");
				hSiL1_1Slow_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nbf0P") == 0){
				std::stringstream histname;
				histname << "hSiL1_2Slow_Pulse" << hSiL1_2Slow_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiL1_2 slow detector");
				hSiL1_2Slow_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Ncf0P") == 0){
				std::stringstream histname;
				histname << "hSiL1_3Slow_Pulse" << hSiL1_3Slow_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiL1_3 slow detector");
				hSiL1_3Slow_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Ndf0P") == 0){
				std::stringstream histname;
				histname << "hSiL1_4Slow_Pulse" << hSiL1_4Slow_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiL1_4 slow detector");
				hSiL1_4Slow_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nef0P") == 0){
				std::stringstream histname;
				histname << "hSiR1_1Slow_Pulse" << hSiR1_1Slow_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiR1_1 slow detector");
				hSiR1_1Slow_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nff0P") == 0){
				std::stringstream histname;
				histname << "hSiR1_2Slow_Pulse" << hSiR1_2Slow_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiR1_2 slow detector");
				hSiR1_2Slow_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Ngf0P") == 0){
				std::stringstream histname;
				histname << "hSiR1_3Slow_Pulse" << hSiR1_3Slow_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiR1_3 slow detector");
				hSiR1_3Slow_PulsePlots.push_back(hPulsePlot);
			}
			else if (strcmp(bankname.c_str(), "Nhf0P") == 0){
				std::stringstream histname;
				histname << "hSiR1_4Slow_Pulse" << hSiR1_4Slow_PulsePlots.size();
				hPulsePlot->SetName(histname.str().c_str());
				hPulsePlot->SetTitle("Plot of a pulse from the SiL2 slow detector");
				hSiR1_4Slow_PulsePlots.push_back(hPulsePlot);
			}
		}
	}
	return SUCCESS;
}
