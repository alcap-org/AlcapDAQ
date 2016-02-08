/********************************************************************\

Name:         MmuXTest
Created by:   Frederik

Contents:     Test case to add a module to the AlCap software for muX. Might do some HPGe stuff.

\********************************************************************/

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
#include <TH2.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"
#include "TGeHit.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MmuXTest_init(void);
INT  MmuXTest_bor(INT);
INT  MmuXTest(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static std::vector<std::string> left_thin_banknames;
static std::vector<std::string> right_thin_banknames;

using std::cout;
using std::cerr;
using std::cin;
using std::endl;

ANA_MODULE MmuXTest_module =
{
	"MmuXTest",                    /* module name           */
	"Frederik",              /* author                */
	MmuXTest,                      /* event routine         */
	MmuXTest_bor,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MmuXTest_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};


/** This method initializes histograms.
*/

INT MmuXTest_init(){


  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); mapIter != bank_to_detector_map.end(); mapIter++) { 

      std::string bankname = mapIter->first;
      std::string detname = gSetup->GetDetectorName(bankname);
    }


  return SUCCESS;
}

// Resets the histograms at the beginning of each run so that the online display updates
INT MmuXTest_bor(INT run_number)
{
  return SUCCESS;

}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MmuXTest(EVENT_HEADER *pheader, void *pevent)
{
	// Get the event number
	int midas_event_number = pheader->serial_number;

	// Some typedefs
	typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
	typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
	typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

	// Fetch a reference to the gData structure that stores a map
	// of (bank_name, vector<TPulseIsland*>) pairs
	TStringPulseIslandMap& pulse_islands_map = gData->fPulseIslandToChannelMap;
	std::map<std::string, std::vector<TPulseIsland*> >& pulses_map = gData->fPulseIslandToChannelMap;
	
	for (map_iterator theMapIter = pulses_map.begin(); theMapIter != pulses_map.end(); theMapIter++)
	{
          std::string bankname = theMapIter->first;
          std::vector<TPulseIsland*> thePulses = theMapIter->second;
          
          cout << "bankname " << bankname << endl;

          // Loop over the TPulseIslands and plot the histogram
          for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); pulseIter++) 
          {
          }
            
        }

	
	///////////////////////////////////////
	return SUCCESS;
}


