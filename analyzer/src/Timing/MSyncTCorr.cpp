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

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MSyncTCorr_init(void);
static INT MSyncTCorr(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
static const int NCRATE = 3;
static const int CRATES[NCRATE]  = {4, 5, 7};
const double TIME_LOW = -100e3, TIME_HIGH = 100e3; // ns; 10 kHz pulser
static TH1* vhSyncTCorr[NCRATE];


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
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    char hist[64]; sprintf(hist, "hPulserTCorr_Crate%d", CRATES[icrate]);
    vhSyncTCorr[icrate] = new TH1D(hist, hist, 1000, TIME_LOW, TIME_HIGH);
    vhSyncTCorr[icrate]->GetXaxis()->SetTitle("Timing Difference WFD-TDC (ns)");
  }
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MSyncTCorr(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
      gData->fTDCHitsToChannelMap;
  const std::map< std::string, std::vector<TPulseIsland*> > wfd_map =
      gData->fPulseIslandToChannelMap;

  const std::string ref_bank = gSetup->GetBankName("TSync");
  if (!tdc_map.count(ref_bank)) {
    printf("MSyncTCorr: No reference hits TPulser!\n");
    return SUCCESS;
  }
  const std::vector<int64_t>& ref_hits = tdc_map.at(ref_bank);

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    char det[32]; sprintf(det, "SyncCrate%d", CRATES[icrate]);
    const std::string bank = gSetup->GetBankName(det);
    if (!wfd_map.count(bank)) {
      printf("MSyncTCorr: No pulser hits %s!\n", bank.c_str());
      continue;
    }

    const std::vector<TPulseIsland*>& pulses = wfd_map.at(bank);
    for (int p = 0; p < pulses.size(); ++p) {
      for (int t = 0; t < ref_hits.size(); ++t) {
        static const double tdc_tick = 0.025; // ns
        const double dt = pulses[p]->GetPulseTime() - tdc_tick*ref_hits[t];
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
