/********************************************************************\

  Name:         MExample
  Created by:   Andrew Edmonds

  Contents:     Example Module

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
INT  MExample_init(void);
INT  MExample(EVENT_HEADER*, void*);
vector<string> GetAllFADCBankNames();
double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;

static TH1* hExample;

static vector<TOctalFADCBankReader*> fadc_bank_readers;

ANA_MODULE MExample_module =
{
  "MExample",                    /* module name           */
  "Andrew Edmonds",              /* author                */
  MExample,                      /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MExample_init,                 /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MExample_init()
{
  // This histogram has the bank names labeled on the X-axis, and the midas
  // block number on the Y-axis.
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.
  hExample = new TH1I(
    "hExample",
    "Example 1D Histogram",
    100,0,100);
  hExample->SetBit(TH1::kCanRebin);

  vector<string> bank_names = GetAllFADCBankNames();

  for(unsigned int i=0; i<bank_names.size(); i++) {
    fadc_bank_readers.push_back(new TOctalFADCBankReader(bank_names[i]));
  }

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
  * of TOctalFADCIsland objects from the raw Octal FADC data.
  */
INT MExample(EVENT_HEADER *pheader, void *pevent)
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

  // Iterate through the banks and plot the clock ticks
  for (map_iterator iter = pulse_islands_map.begin();
      iter != pulse_islands_map.end(); iter++) {
  
  		double clock_tick = GetClockTickForChannel(iter->first);
  			
  		// Fill histogram
    	hExample->Fill(clock_tick);  
  }

  return SUCCESS;
}
