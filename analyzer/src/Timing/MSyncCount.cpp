/********************************************************************\

Name:         MPulseLengths
Created by:   John R Quirk

Contents:     A module to fill a histogram of the pulse lengths from each channel

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <utility>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH2.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"


/*-- Module declaration --------------------------------------------*/
INT  MSyncCount_init(void);
INT  MSyncCount(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static const int NCRATE = 9;
static const int HEIGHTCUT = 100;

static std::vector<TH1I*> hvSyncCount;

ANA_MODULE MSyncCount_module =
{
	"MSyncCount",    /* module name           */
	"John R Quirk",  /* author                */
	MSyncCount,      /* event routine         */
	NULL,            /* BOR routine           */
	NULL,            /* EOR routine           */
	MSyncCount_init, /* init routine          */
	NULL,            /* exit routine          */
	NULL,            /* parameter structure   */
	0,               /* structure size        */
	NULL,            /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MSyncCount_init() {
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    char name[32]; sprintf(name, "hSyncCount_Crate%d", icrate);
    char title[64]; sprintf(title,
                            "Sync Pulse Count Crate %d;Pulses per block",
                            icrate);
    hvSyncCount.push_back(new TH1I(name, title, 70, 0., 70.));
  }
  char name[32]; sprintf(name, "hSyncCount_TDC");
  char title[64]; sprintf(title, "Sync Pulse Count TDC;Pulses per block");
  hvSyncCount.push_back(new TH1I(name, title, 70, 0., 70.));
  return SUCCESS;
}

/** This method processes one MIDAS block
 */
INT MSyncCount(EVENT_HEADER *pheader, void *pevent) {
	const std::map< std::string, std::vector<TPulseIsland*> >& pulses_map =
    gData->fPulseIslandToChannelMap;

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    char det[16]; sprintf(det, "SyncCrate%d", icrate);
    std::string bank = gSetup->GetBankName(det);
    if (pulses_map.count(bank)) {
      const std::vector<TPulseIsland*>& pulses = pulses_map.at(bank);
      int npulse = 0;
      for (int ipulse = 0; ipulse < pulses.size(); ++ipulse)
        if (pulses[ipulse]->GetPulseHeight() > HEIGHTCUT)
          ++npulse;
      hvSyncCount[icrate]->Fill(npulse);
    } else {
      hvSyncCount[icrate]->Fill(0.);
    }
  }

  const std::map< std::string, std::vector<int64_t> >& tdcs_map =
    gData->fTDCHitsToChannelMap;
  char det[16]; sprintf(det, "TSync");
  std::string bank = gSetup->GetBankName(det);
  if(tdcs_map.count(bank))
    hvSyncCount[NCRATE]->Fill(tdcs_map.at(bank).size());

	return SUCCESS;
}
