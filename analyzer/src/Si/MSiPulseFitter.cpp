/********************************************************************\

Name:         MSiPulseFitter
Created by:   Andrew Edmonds

Contents:     A module to plot the pulse heights of the silicon detector

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>

/* AlCap includes */
#include "DetectorMap.h"

#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"
#include "TSimpleSiPulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MSiPulseFitter_init(void);
INT  MSiPulseFitter(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern map<string, vector<TSimpleSiPulse*> > theSimpleSiPulseMap;

static vector<TOctalFADCBankReader*> fadc_bank_readers;

ANA_MODULE MSiPulseFitter_module =
{
	"MSiPulseFitter",                    /* module name           */
	"Andrew Edmonds",              /* author                */
	MSiPulseFitter,                      /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MSiPulseFitter_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MSiPulseFitter_init()
{
  // This histogram has the bank names labeled on the X-axis, and the midas
  // block number on the Y-axis.
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.
  vector<string> bank_names = GetAllFADCBankNames();

  for(unsigned int i=0; i<bank_names.size(); i++) {
    fadc_bank_readers.push_back(new TOctalFADCBankReader(bank_names[i]));
  }

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MSiPulseFitter(EVENT_HEADER *pheader, void *pevent)
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
		string detname = ChannelToDetectorMap[bankname];
			
		if (detname.substr(0,2) != "Si")
			continue;
			
		std::vector<TSimpleSiPulse*> theSiPulses = simpleSiMapIter->second;
			
		// Loop over the TSimpleSiPulses
		for (std::vector<TSimpleSiPulse*>::iterator siPulse = theSiPulses.begin(); siPulse != theSiPulses.end(); siPulse++) {
			
			// If it's a slow pulse...
			if ( (*siPulse)->IsSlowPulse() ) {
				// ...produce a fitting function
				TH1* hPulse = (*siPulse)->GetWaveform("hPulse"); // get the waveform
				TF1* gaussian = new TF1("gaus", "[0]*TMath::Gaus(x, [1], [2], 0) + [3]", hPulse->GetXaxis()->GetXmin()+1,hPulse->GetXaxis()->GetXmax()-1); // reduce the range a bit because ROOT did this weird thing where it roughly doubles the contents of all the bins !?!?
        		gaussian->SetParameter(0, -300);
        		gaussian->SetParameter(1, hPulse->GetMean());
        		gaussian->SetParameter(2, hPulse->GetRMS());
        		gaussian->SetParameter(3, (*siPulse)->GetPedestal());
                
        		gaussian->SetLineColor(kRed);
        		gaussian->SetLineWidth(2);
        		hPulse->Fit("gaus", "QR");
			}
		}
	}
	return SUCCESS;
}
