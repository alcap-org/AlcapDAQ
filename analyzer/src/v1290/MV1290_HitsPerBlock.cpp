/********************************************************************\

Name:         MV1290_HitsPerBlock
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
INT MV1290_HitsPerBlock_init(void);
INT MV1290_HitsPerBlock_bor(INT);
INT MV1290_HitsPerBlock(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1F* hTDCHitCountsPerBlock;
static TH1F* hTDCHitCountsAvg10Blocks;
static const int n_blocks_for_average = 10;
std::map<std::string, std::vector<double> > previous_counts;

ANA_MODULE MV1290_HitsPerBlock_module =
{
	"MV1290_HitsPerBlock",    /* module name           */
	"Andrew Edmonds",               /* author                */
	MV1290_HitsPerBlock,      /* event routine         */
	MV1290_HitsPerBlock_bor,  /* BOR routine           */
	NULL,                           /* EOR routine           */
	MV1290_HitsPerBlock_init, /* init routine          */
	NULL,                           /* exit routine          */
	NULL,                           /* parameter structure   */
	0,                              /* structure size        */
	NULL,                           /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MV1290_HitsPerBlock_init() {
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
INT MV1290_HitsPerBlock_bor(INT run_number) {

  hTDCHitCountsPerBlock->Reset();
  hTDCHitCountsAvg10Blocks->Reset();
  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MV1290_HitsPerBlock(EVENT_HEADER *pheader, void *pevent) {

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

    std::vector<double>& counts = previous_counts[tdc_detname];
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
  }

  return SUCCESS;
}
