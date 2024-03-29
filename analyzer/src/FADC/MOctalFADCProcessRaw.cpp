/********************************************************************\

  Name:         MOctalFADCProcessRaw
  Created by:   Michael Murray

  Contents:     Module to extract TOctalFADCIsland objects from
                the data banks corresponding to Fred Gray's Octal
                FADC digitizer. This processes the raw data only,
                making no assumptions about the attached detector,
                digitization frequency, etc.

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
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MOctalFADCProcessRaw_init(void);
INT  MOctalFADCProcessRaw(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH2* hNOctalFADCIslandsReadPerBlock;

static vector<TOctalFADCBankReader*> fadc_bank_readers;

ANA_MODULE MOctalFADCProcessRaw_module =
{
  "MOctalFADCProcessRaw",        /* module name           */
  "Michael Murray",              /* author                */
  MOctalFADCProcessRaw,          /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MOctalFADCProcessRaw_init,     /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MOctalFADCProcessRaw_init()
{
  // This histogram has the bank names labeled on the X-axis, and the midas
  // block number on the Y-axis.
  // This uses the TH1::kCanRebin mechanism to expand automatically to the
  // number of FADC banks.
  hNOctalFADCIslandsReadPerBlock = new TH2I(
    "hNOctalFADCIslandsReadPerBlock",
    "Number of FADC Islands read by block",
    1,0,1, 10000,0,10000);
  hNOctalFADCIslandsReadPerBlock->SetBit(TH1::kCanRebin);

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 
    
    std::string bankname = mapIter->first;

    // We only want the FADC banks here
    if (TSetupData::IsFADC(bankname))
      fadc_bank_readers.push_back(new TOctalFADCBankReader(bankname));
  }

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
  * of TOctalFADCIsland objects from the raw Octal FADC data.
  */
INT MOctalFADCProcessRaw(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;

  /////////////////////////////////////////////////////////////////////
  // Make TPulseIsland objects for each of the TOctalFADCIslands.

  // Some typedefs
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map =
    gData->fPulseIslandToChannelMap;

  // Delete the islands found in the previous block. Don't clear
  // the map of (bank_name, vector) pairs. Once we use a bank name
  // once, it will have a (possibly empty) entry in this map for the
  // whole run.
  for(map_iterator iter = pulse_islands_map.begin();
      iter != pulse_islands_map.end(); iter++){
    vector<TPulseIsland*>& islands = iter->second;
    // Delete the pointers to TPulseIslands, then clear the vector
    for(unsigned int j=0; j<islands.size(); j++) {
      if(islands[j]) { delete islands[j]; islands[j] = NULL; }
    }
    islands.clear();
  }

  // Get islands from all banks and add them to the global structure
  for(unsigned int i=0; i<fadc_bank_readers.size(); i++) {
    string bank_name = fadc_bank_readers[i]->GetBankName();

    // Read the islands with the bank reader
    /// \todo Check for errors when reading/processing the FADC banks
    fadc_bank_readers[i]->ProcessEvent(pheader, pevent);
    vector<TOctalFADCIsland*> fadc_islands =
      fadc_bank_readers[i]->GetIslandVectorCopy();

    // Make vector of TPulseIsland from TOctalFADCIsland. Now this module
    // owns the memory associated with these.
    vector<TPulseIsland*> pulse_islands;
    for(unsigned int j=0; j<fadc_islands.size(); j++) {
      pulse_islands.push_back(new TPulseIsland(
        fadc_islands[j]->GetTime(), fadc_islands[j]->GetSampleVector(),bank_name));
    }

    // Add a pair (bank_name, vector_of_islands) to the std::map in gData
    pulse_islands_map[bank_name] = pulse_islands;

    // Fill Diagnostic histogram
    hNOctalFADCIslandsReadPerBlock->Fill(bank_name.c_str(), midas_event_number,
      fadc_islands.size());

    // print for testing
    if(midas_event_number == 1) {      
		  printf("TEST MESSAGE: Read %d events from bank %s in event %d\n",
        fadc_bank_readers[i]->GetNIslands(),
        fadc_bank_readers[i]->GetBankName().c_str(),
        midas_event_number);
    }
  }

  return SUCCESS;
}
