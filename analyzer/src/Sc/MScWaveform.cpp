/********************************************************************\

Name:         MScWaveform
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
#include <sstream>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TLine.h"

/* AlCap includes */
#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"
#include "TSimpleScPulse.h"
#include "DetectorMap.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MScWaveform_init(void);
INT  MScWaveform(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern map<string, vector<TSimpleScPulse*> > theSimpleScPulseMap;
extern map<string, int> theNSubScPulseMap;
map <string, vector<TH1I*> > theScPulseHistMap;

static vector<TOctalFADCBankReader*> fadc_bank_readers;

ANA_MODULE MScWaveform_module =
{
	"MScWaveform",    /* module name           */
	"Nam Tran",       /* author                */
	MScWaveform,      /* event routine         */
	NULL,             /* BOR routine           */
	NULL,             /* EOR routine           */
	MScWaveform_init, /* init routine          */
	NULL,             /* exit routine          */
	NULL,             /* parameter structure   */
	0,                /* structure size        */
	NULL,             /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MScWaveform_init()
{
	for (detIter aDetIter = DetectorToRawHistMap.begin(); 
			aDetIter != DetectorToRawHistMap.end(); aDetIter++)
	{
		string detname = aDetIter->first;
		
		if (detname.substr(0,2) != "Sc")
			continue;
				
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
INT MScWaveform(EVENT_HEADER *pheader, void *pevent)
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
		
		int island_number = 0;
		int total_pulse_number = 0;
		// Loop over the islands and fill the relevant histogram with the peak height
		for (std::vector<TOctalFADCIsland*>::iterator octalFADCIslandIter = 
				theOctalFADCIslands.begin();
				octalFADCIslandIter != theOctalFADCIslands.end(); 
				octalFADCIslandIter++) 
		{
			island_number++;
			
			std::string bankname = (*bankReaderIter)->GetBankName();
			string detname = ChannelToDetectorMap[bankname];
			
			if (detname.substr(0,2) != "Sc")
				continue;
				
			TSimpleScPulse *scPulse = new TSimpleScPulse(*octalFADCIslandIter);

			double pulseheight;
			if (scPulse->IsPositive())
			{
				pulseheight = pulseheight - scPulse->GetPedestal();
			}
			else
			{
				TSimpleScPulse *invertedPulse = scPulse->Invert();
				pulseheight = invertedPulse->GetMax() - invertedPulse->GetPedestal();
			}

			if (ChannelToDetectorMap.find(bankname) == ChannelToDetectorMap.end())
			{
				// not found, do nothing
			}
			else
			{
				std::stringstream histname;
				histname << "h" << detname << "Raw" << "_Event" << midas_event_number << "_Island" << island_number;
				DetectorToRawHistMap[detname] = (TH1I *)scPulse->GetWaveform(histname.str());
				
				// Loop through the pulses from where we were and store their waveforms in a vector
				std::vector<TH1I*> thePulses;
				std::stringstream bankislandname;
				bankislandname << bankname << island_number;
				
				std::vector<TSimpleScPulse*> theBankPulses = theSimpleScPulseMap[bankname];
				for (int iPulse = 0; iPulse < theNSubScPulseMap[bankislandname.str()]; iPulse++) {
					
					std::stringstream pulsehistname;
					pulsehistname << histname.str() << "_Pulse" << iPulse+1;
					
					TH1I* hPulse = theBankPulses[iPulse + total_pulse_number]->GetWaveform(pulsehistname.str());
					hPulse->SetLineColor(kMagenta);
					
					thePulses.push_back(hPulse);
				}
				std::pair<std::string, std::vector<TH1I*> > thePair(detname, thePulses);
				theScPulseHistMap.insert(thePair);
				
				total_pulse_number += thePulses.size();
			}
			//if (strcmp(bankname.c_str(), "Nfc0") == 0)
			//{
				//hSiRaw->Reset();
				//hSiRaw = scPulse->GetWaveform();
			//}
		}
	}

	return SUCCESS;
}
