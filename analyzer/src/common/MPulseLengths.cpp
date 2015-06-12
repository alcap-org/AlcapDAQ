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
INT  MPulseLengths_init(void);
INT  MPulseLengths(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static const int NCRATE = 9;
static const int NCHAN[NCRATE] = {0, 0, 0, 0, 8, 4, 0, 8, 8};

static std::vector<TH1I*> hvPulseLengths;

ANA_MODULE MPulseLengths_module =
{
	"MPulseLengths",    /* module name           */
	"John R Quirk",     /* author                */
	MPulseLengths,      /* event routine         */
	NULL,               /* BOR routine           */
	NULL,               /* EOR routine           */
	MPulseLengths_init, /* init routine          */
	NULL,               /* exit routine          */
	NULL,               /* parameter structure   */
	0,                  /* structure size        */
	NULL,               /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MPulseLengths_init() {

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ichannel = 0; ichannel < NCHAN[icrate]; ++ichannel) {
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ichannel);
      char name[32]; sprintf(name, "hPulseLengths_%s", bank);
      char title[64]; sprintf(title, "Pulse Lengths %s",
                              gSetup->GetDetectorName(bank).c_str());
      hvPulseLengths.push_back(new TH1I(name, title, 600, 0., 600.));
    }
  }
  return SUCCESS;
}

/** This method processes one MIDAS block
 */
INT MPulseLengths(EVENT_HEADER *pheader, void *pevent) {
	std::map< std::string, std::vector<TPulseIsland*> >& pulses_map =
      gData->fPulseIslandToChannelMap;
  for (int icrate = 0, ihist = 0; icrate < NCRATE; ++icrate) {
    for (int ichannel = 0; ichannel < NCHAN[icrate]; ++ichannel, ++ihist) {
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ichannel);
      if (pulses_map.count(bank)) {
        std::vector<TPulseIsland*>& pulses = pulses_map[bank];
        for (int ipulse = 0; ipulse < pulses.size(); ++ipulse)
          hvPulseLengths[ihist]->Fill(pulses[ipulse]->GetSamples().size());
      }
    }
  }

	return SUCCESS;
}
