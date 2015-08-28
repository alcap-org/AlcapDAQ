////////////////////////////////////////////////////////////////////////////////
/// \defgroup MSyncTCorr
///
/// \brief
/// Synchronization pulser correlation
/// @{
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <map>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1D.h"
#include "TDirectory.h"

//JG: added alcap includes
/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"


extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
using std::string;
using std::vector;
using std::map;

static INT MSyncTCorr_init(void);
static INT MSyncTCorr(EVENT_HEADER*, void*);

namespace {
  const double TIME_LOW = -100e3, TIME_HIGH = 100e3; // ns; 10 kHz pulser
  string syncbank[NCRATE];
  TH1* vhSyncTCorr[NCRATE];
}

ANA_MODULE MSyncTCorr_module =
{
  "MSyncTCorr",    /* module name           */
  "John R Quirk",  /* author                */
  MSyncTCorr,      /* event routine         */
  NULL,            /* BOR routine           */
  NULL,            /* EOR routine           */
  MSyncTCorr_init, /* init routine          */
  NULL,            /* exit routine          */
  NULL,            /* parameter structure   */
  0,               /* structure size        */
  NULL,            /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MSyncTCorr_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("SyncTCorr/")->cd();
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    char hist[64]; sprintf(hist, "hPulserTCorr_Crate%d", icrate);
    char det[32]; sprintf(det, "SyncCrate%d", icrate);
    syncbank[icrate] = gSetup->GetBankName(det);
    vhSyncTCorr[icrate] = new TH1D(hist, hist, 10000, TIME_LOW, TIME_HIGH);
    vhSyncTCorr[icrate]->GetXaxis()->SetTitle("Timing Difference TDC-WFD (ns)");
  }
  cwd->cd();
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MSyncTCorr(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<int64_t> >& tdc_map =
      gData->fTDCHitsToChannelMap;
  const map< string, vector<TPulseIsland*> > wfd_map =
      gData->fPulseIslandToChannelMap;

  const string ref_bank = gSetup->GetBankName("TSync");
  if (!tdc_map.count(ref_bank)) {
    printf("MSyncTCorr: No reference hits TPulser!\n");
    return SUCCESS;
  }
  const vector<int64_t>& hits = tdc_map.at(ref_bank);

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    if (!wfd_map.count(syncbank[icrate]) ||
	gData->fTDCSynchronizationPulseIndex[icrate] == -1) continue;
    
    const double toff = gData->fTDCSynchronizationPulseOffset[icrate];
    const vector<TPulseIsland*>& pulses = wfd_map.at(syncbank[icrate]);
    for (int t = 0; t < hits.size(); ++t) {
      for (int p = 0; p < pulses.size(); ++p) {
        const double dt = TICKTDC*hits[t] -
	  TICKWFD[icrate]*pulses[p]->GetTimeStamp() -
	  toff;
        if (dt < TIME_LOW)
          break;
        else if (dt < TIME_HIGH)
          vhSyncTCorr[icrate]->Fill(dt);
      }
    }
  }

  return SUCCESS;
}

/// @}
