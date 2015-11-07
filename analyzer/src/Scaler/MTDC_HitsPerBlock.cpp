/********************************************************************\

Name:         MTDC_HitsPerBlock
Created by:   Andrew Edmonds

Contents:     Plots the number of TDC hits per channel per MIDAS block

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
INT MTDC_HitsPerBlock_init(void);
INT MTDC_HitsPerBlock_bor(INT);
INT MTDC_HitsPerBlock(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1F* hTDCHitCountsPerBlock;
static TH1F* hTDCHitCountsAvg10Blocks;
static const int n_blocks_for_average = 10;
std::map<std::string, std::vector<double> > tdc_previous_counts;

ANA_MODULE MTDC_HitsPerBlock_module =
{
	"MTDC_HitsPerBlock",    /* module name           */
	"Andrew Edmonds",               /* author                */
	MTDC_HitsPerBlock,      /* event routine         */
	MTDC_HitsPerBlock_bor,  /* BOR routine           */
	NULL,                           /* EOR routine           */
	MTDC_HitsPerBlock_init, /* init routine          */
	NULL,                           /* exit routine          */
	NULL,                           /* parameter structure   */
	0,                              /* structure size        */
	NULL,                           /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MTDC_HitsPerBlock_init() {
  // The following histograms are created for each channel:


  hTDCHitCountsPerBlock = new TH1F("hTDCHitCountsPerBlock", "Number of TDC Hits in this Block", 1,0,1);
  hTDCHitCountsPerBlock->SetXTitle("Channel");
  hTDCHitCountsPerBlock->SetYTitle("Number of Hits (This Block)");
  hTDCHitCountsPerBlock->SetBit(TH1::kCanRebin);

  hTDCHitCountsAvg10Blocks = new TH1F("hTDCHitCountsAvg10Blocks", "Number of TDC Hits in this Block", 1,0,1);
  hTDCHitCountsAvg10Blocks->SetXTitle("Channel");
  hTDCHitCountsAvg10Blocks->SetYTitle("Number of Hits [Hz]"); // since we are averageing over 10 * 100 ms blocks
  hTDCHitCountsAvg10Blocks->SetBit(TH1::kCanRebin);

  return SUCCESS;
}

// Resets the histograms at the beginning of each run
// so that the online display updates
INT MTDC_HitsPerBlock_bor(INT run_number) {

  hTDCHitCountsPerBlock->Reset();
  hTDCHitCountsAvg10Blocks->Reset();

  const std::map< std::string, std::vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;

  // Loop through the TDC hits and plot the times, we know that the sync pulse is in Ch1
  for (std::map<std::string, vector<int64_t> >::const_iterator theMapIter = tdc_map.begin();
       theMapIter != tdc_map.end(); theMapIter++) {

    std::string tdc_bankname = theMapIter->first;
    std::string tdc_detname = gSetup->GetDetectorName(tdc_bankname);
    std::vector<double>& counts = tdc_previous_counts[tdc_detname];
    counts.clear();
  }
  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MTDC_HitsPerBlock(EVENT_HEADER *pheader, void *pevent) {

  int midas_event_number = pheader->serial_number;

  // Reset the every block histogram
  hTDCHitCountsPerBlock->Reset();

  const std::map< std::string, std::vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;

  // Loop through the TDC hits and plot the times, we know that the sync pulse is in Ch1
  for (std::map<std::string, vector<int64_t> >::const_iterator theMapIter = tdc_map.begin();
       theMapIter != tdc_map.end(); theMapIter++) {
    std::string tdc_bankname = theMapIter->first;
    std::string tdc_detname = gSetup->GetDetectorName(tdc_bankname);
    
    if (tdc_detname == "TRollover") {
      continue;
    }
    std::vector<int64_t> theTDCHits = theMapIter->second;
    int n_tdc_hits = theTDCHits.size();
    hTDCHitCountsPerBlock->Fill(tdc_detname.c_str(), n_tdc_hits);

    std::vector<double>& counts = tdc_previous_counts[tdc_detname];
    if (midas_event_number > 10) {
      counts.at( (midas_event_number%10)) = n_tdc_hits;
    }
    else {
      counts.push_back(n_tdc_hits);
    }

    double average = 0;
    for (std::vector<double>::const_iterator i_count = counts.begin(); i_count != counts.end(); ++i_count) {
      average += (*i_count);
    }
    hTDCHitCountsAvg10Blocks->SetBinContent(hTDCHitCountsAvg10Blocks->GetXaxis()->FindBin(tdc_detname.c_str()), average);

    // Set the range so that we don't see all the empty bins since kCanRebin doubles the number of bins each time
    hTDCHitCountsPerBlock->GetXaxis()->SetRange(1, tdc_previous_counts.size());
    hTDCHitCountsAvg10Blocks->GetXaxis()->SetRange(1, tdc_previous_counts.size());
  }

  return SUCCESS;
}
