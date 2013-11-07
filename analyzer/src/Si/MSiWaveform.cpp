/********************************************************************\

Name:         MSiWaveform
Created by:   Nam Tran

Contents:     A module to plot waveforms of pulses from Si detectors

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TDirectory.h"

/* AlCap includes */
#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"
#include "TSimpleSiPulse.h"
#include "DetectorMap2.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MSiWaveform_init(void);
INT  MSiWaveform(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;

static TH1* hSiRaw;

static vector<TOctalFADCBankReader*> fadc_bank_readers;

ANA_MODULE MSiWaveform_module =
{
	"MSiWaveform", 		/* module name           */
	"Nam Tran", 					/* author                */
	MSiWaveform, 			/* event routine         */
	NULL, 								/* BOR routine           */
	NULL, 								/* EOR routine           */
	MSiWaveform_init, 	/* init routine          */
	NULL, 								/* exit routine          */
	NULL, 								/* parameter structure   */
	0, 										/* structure size        */
	NULL, 								/* initial parameters    */
};

/** This method initializes histograms.
*/
INT MSiWaveform_init()
{
	hSiRaw = new TH1I("hSiRaw","hSiRaw",100,0,100);

	for (detIter aDetIter = DetectorToRawHistMap.begin(); 
			aDetIter != DetectorToRawHistMap.end(); aDetIter++)
	{
		string detname = aDetIter->first;
		string histname = detname + "Raw";
		aDetIter->second =
			new TH1I(histname.c_str(), histname.c_str(), 100, 0, 100);
	}

	vector<string> bank_names = GetAllFADCBankNames();

	for(unsigned int i=0; i<bank_names.size(); i++) {
		fadc_bank_readers.push_back(new TOctalFADCBankReader(bank_names[i]));
	}

	return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MSiWaveform(EVENT_HEADER *pheader, void *pevent)
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
	for (std::vector<TOctalFADCBankReader*>::iterator 
			bankReaderIter = fadc_bank_readers.begin();
			bankReaderIter != fadc_bank_readers.end(); 
			bankReaderIter++) 
	{
		(*bankReaderIter)->ProcessEvent(pheader, pevent);
		std::vector<TOctalFADCIsland*> theOctalFADCIslands = 
			(*bankReaderIter)->GetIslandVectorCopy();

		// Loop over the islands and fill the relevant histogram with the peak height
		for (std::vector<TOctalFADCIsland*>::iterator octalFADCIslandIter = 
				theOctalFADCIslands.begin();
				octalFADCIslandIter != theOctalFADCIslands.end(); 
				octalFADCIslandIter++) 
		{
			std::string bankname = (*bankReaderIter)->GetBankName();
			TSimpleSiPulse *siPulse = new TSimpleSiPulse(*octalFADCIslandIter);

			double pulseheight;
			if (siPulse->IsPositive())
			{
				pulseheight = pulseheight - siPulse->GetPedestal();
			}
			else
			{
				TSimpleSiPulse *invertedPulse = siPulse->Invert();
				pulseheight = invertedPulse->GetMax() - invertedPulse->GetPedestal();
			}

			if (ChannelToDetectorMap.find(bankname) == ChannelToDetectorMap.end())
			{
				// not found, do nothing
			}
			else
			{
				string detname = ChannelToDetectorMap[bankname];
				string histname = detname + "Raw";
				delete gDirectory->Get(histname.c_str());
				DetectorToRawHistMap[detname] = (TH1I *)siPulse->GetWaveform(detname+"Raw");
			}
			//if (strcmp(bankname.c_str(), "Nfc0") == 0)
			//{
				//hSiRaw->Reset();
				//hSiRaw = siPulse->GetWaveform();
			//}
		}
	}

	return SUCCESS;
}
