/********************************************************************\

Name:         MWFD_HitsPerBlock
Created by:   Andrew Edmonds

Contents:     Plots the number of WFD hits per channel per MIDAS block

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT MWFD_HitsPerBlock_init(void);
INT MWFD_HitsPerBlock_bor(INT);
INT MWFD_HitsPerBlock(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1F* hWFDHitCountsPerBlock;
static TH1F* hWFDHitCountsAvg10Blocks;
static const int n_blocks_for_average = 10;
std::map<std::string, std::vector<double> > wfd_previous_counts;

ANA_MODULE MWFD_HitsPerBlock_module =
{
	"MWFD_HitsPerBlock",    /* module name           */
	"Andrew Edmonds",               /* author                */
	MWFD_HitsPerBlock,      /* event routine         */
	MWFD_HitsPerBlock_bor,  /* BOR routine           */
	NULL,                           /* EOR routine           */
	MWFD_HitsPerBlock_init, /* init routine          */
	NULL,                           /* exit routine          */
	NULL,                           /* parameter structure   */
	0,                              /* structure size        */
	NULL,                           /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MWFD_HitsPerBlock_init() {
  // The following histograms are created for each channel:


  hWFDHitCountsPerBlock = new TH1F("hWFDHitCountsPerBlock", "Number of WFD Hits in this Block", 1,0,1);
  hWFDHitCountsPerBlock->SetXTitle("Channel");
  hWFDHitCountsPerBlock->SetYTitle("Number of Hits (This Block)");
  hWFDHitCountsPerBlock->SetBit(TH1::kCanRebin);

  hWFDHitCountsAvg10Blocks = new TH1F("hWFDHitCountsAvg10Blocks", "Number of WFD Hits in this Block", 1,0,1);
  hWFDHitCountsAvg10Blocks->SetXTitle("Channel");
  hWFDHitCountsAvg10Blocks->SetYTitle("Number of Hits [Hz]"); // since we are averageing over 10 * 100 ms blocks
  hWFDHitCountsAvg10Blocks->SetBit(TH1::kCanRebin);

  return SUCCESS;
}

// Resets the histograms at the beginning of each run
// so that the online display updates
INT MWFD_HitsPerBlock_bor(INT run_number) {

  hWFDHitCountsPerBlock->Reset();
  hWFDHitCountsAvg10Blocks->Reset();
  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MWFD_HitsPerBlock(EVENT_HEADER *pheader, void *pevent) {

  int midas_event_number = pheader->serial_number;

  // Reset the every block histogram
  hWFDHitCountsPerBlock->Reset();

  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;

  // Loop through the WFD hits and plot the times, we know that the sync pulse is in Ch1
  for (std::map<std::string, vector<TPulseIsland*> >::const_iterator theMapIter = wfd_map.begin();
       theMapIter != wfd_map.end(); theMapIter++) {
    std::string wfd_bankname = theMapIter->first;
    std::string wfd_detname = gSetup->GetDetectorName(wfd_bankname);
    
    if (!gSetup->IsWFD(wfd_bankname)) {
      continue;
    }

    std::vector<TPulseIsland*> thePulses = theMapIter->second;
    int n_wfd_hits = thePulses.size();
    hWFDHitCountsPerBlock->Fill(wfd_detname.c_str(), n_wfd_hits);

    std::vector<double>& counts = wfd_previous_counts[wfd_detname];
    if (midas_event_number > 10) {
      counts.at( (midas_event_number%10)) = n_wfd_hits;
    }
    else {
      counts.push_back(n_wfd_hits);
    }

    double average = 0;
    for (std::vector<double>::const_iterator i_count = counts.begin(); i_count != counts.end(); ++i_count) {
      average += (*i_count);
    }
    hWFDHitCountsAvg10Blocks->SetBinContent(hWFDHitCountsAvg10Blocks->GetXaxis()->FindBin(wfd_detname.c_str()), average);

    // Set the range so that we don't see all the empty bins since kCanRebin doubles the number of bins each time
    hWFDHitCountsPerBlock->GetXaxis()->SetRange(1, wfd_previous_counts.size());
    hWFDHitCountsAvg10Blocks->GetXaxis()->SetRange(1, wfd_previous_counts.size());
  }

  return SUCCESS;
}
