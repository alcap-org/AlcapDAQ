////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTSyncTDiff
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
static INT MTSyncTDiff_init(void);
static INT MTSyncTDiff(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
// 10 kHz pulser, 100us correlation
static const int NBanks = 8;
static TH1* vhSyncTDiff;
static TH1* vhTSyncTDiff; 


ANA_MODULE MTSyncTDiff_module =
{
  "MTSyncTDiff",       /* module name           */
  "Damien Alexander",  /* author                */
  MTSyncTDiff,         /* event routine         */
  NULL,                /* BOR routine           */
  NULL,                /* EOR routine           */
  MTSyncTDiff_init,    /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTSyncTDiff_init() {
  char hist[64] = "TSyncTDiffTDC";
  vhTSyncTDiff = new TH1D(hist, hist, 4000, 70000000, 95000000);
  vhTSyncTDiff->GetXaxis()->SetTitle("Timing Difference on Sync Pulse TDC (clock ticks)");
  for(int i=0; i< 8; i++){
    char bank[5]; sprintf(bank, "D7%02d", i);
    std::string detname = gSetup->GetDetectorName(bank);
    if(detname != "SyncCrate7")
      continue;
    char hist[64]; sprintf(hist, "hTScTDiffWFD_%s", bank);
    vhSyncTDiff = new TH1D(hist, hist, 20000, 0, 1850000);
    vhSyncTDiff->GetXaxis()->SetTitle("Timing Difference in Sync Pulse WFD (ns)");
  }
  
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MTSyncTDiff(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
      gData->fTDCHitsToChannelMap;

  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  const std::string ref2_bank = gSetup->GetBankName("SyncCrate7");
  if (!wfd_map.count(ref2_bank)) {
    printf("MPreTargTCorrWFD: No reference hits Sync!\n");
    return SUCCESS;
  }
  const std::vector<TPulseIsland*>& ref2_pulses = wfd_map.at(ref2_bank);


  const std::string ref_bank = gSetup->GetBankName("TSync");
  if (!tdc_map.count(ref_bank)) {
    printf("MSyncTCorr: No reference hits TPulser!\n");
    return SUCCESS;
  }
  const std::vector<int64_t>& ref_hits = tdc_map.at(ref_bank);
  for (int t = 1; t < ref_hits.size(); ++t) {
    const double dt = ref_hits[t] - ref_hits[t-1];
    //std::cout << dt << std::endl;
    vhTSyncTDiff->Fill(dt);
  }

  for (int ich= 0; ich < 8; ++ich) {
    char bank[5]; sprintf(bank, "D7%02d", ich);
    if (!wfd_map.count(bank)) continue;
    if(bank != ref2_bank)
      continue;
    for (int i = 1; i < ref2_pulses.size(); ++i) {
        const int64_t dt = ref2_pulses[i]->GetPulseTime() - ref2_pulses[i-1]->GetPulseTime();
	vhSyncTDiff->Fill(dt);
      
    }
  }

  return SUCCESS;
}

/// @}
