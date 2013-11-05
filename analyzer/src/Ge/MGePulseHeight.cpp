/********************************************************************\

  Name:         MGePulseHeight
  Created by:   Nam Tran

  Contents:     A module to plot the pulse height spectrum of the Ge detector

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
#include "TOctalFADCIsland.h"
#include "TOctalFADCBankReader.h"
#include "TGlobalData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MGePulseHeight_init(void);
INT  MGePulseHeight(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;

static TH1* hGeSlow_Heights; // Bank: Ncc0

static vector<TOctalFADCBankReader*> fadc_bank_readers;

ANA_MODULE MGePulseHeight_module =
{
  "MGePulseHeight",              /* module name           */
  "Nam Tran",         			     /* author                */
  MGePulseHeight,                /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MGePulseHeight_init,           /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MGePulseHeight_init()
{
	hGeSlow_Heights = new TH1I("hGeSlow_Heights", 
			"Plot of the pulse heights from the Ge detector", 100,0,100);

  vector<string> bank_names = GetAllFADCBankNames();

  for(unsigned int i=0; i<bank_names.size(); i++) {
    fadc_bank_readers.push_back(new TOctalFADCBankReader(bank_names[i]));
  }

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
  * of TOctalFADCIsland objects from the raw Octal FADC data.
  */
INT MGePulseHeight(EVENT_HEADER *pheader, void *pevent)
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
  for (
			std::vector<TOctalFADCBankReader*>::iterator 
				bankReaderIter = fadc_bank_readers.begin();
			bankReaderIter != fadc_bank_readers.end(); 
			bankReaderIter++) 
	{
  	   (*bankReaderIter)->ProcessEvent(pheader, pevent);
  	   std::vector<TOctalFADCIsland*> 
				 theOctalFADCIslands = (*bankReaderIter)->GetIslandVectorCopy();
  	   
  	   // Loop over the islands and fill the relevant histogram with the peak height
  	   for (std::vector<TOctalFADCIsland*>::iterator 
						 octalFADCIslandIter = theOctalFADCIslands.begin(); 
					 octalFADCIslandIter != theOctalFADCIslands.end(); 
					 octalFADCIslandIter++) 
			 {
  	   		if (strcmp((*bankReaderIter)->GetBankName().c_str(), "Ncc0") == 0)
						hGeSlow_Heights->Fill((*octalFADCIslandIter)->GetMax()); 
					// NB the pulses might be negative so this could be wrong!!!
  	   }
  	   
  }
  return SUCCESS;
}
