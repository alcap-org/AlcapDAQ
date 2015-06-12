////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTDCWFDTcorr
/// \author John R Quirk
///
/// \brief
/// Timing correlations from within TDC
/// @{
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
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
static INT MTDCWFDTCorr_init(void);
static INT MTDCWFDTCorr(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static const int NTDCCHAN = 32;
const double TIME_LOW = -10e3, TIME_HIGH = 10e3;
static TH1* vhTDCWFDTCorr[NTDCCHAN];

ANA_MODULE MTDCWFDTCorr_module =
{
  "MTDCWFDTCorr",    /* module name           */
  "John R Quirk",    /* author                */
  MTDCWFDTCorr,      /* event routine         */
  NULL,              /* BOR routine           */
  NULL,              /* EOR routine           */
  MTDCWFDTCorr_init, /* init routine          */
  NULL,              /* exit routine          */
  NULL,              /* parameter structure   */
  0,                 /* structure size        */
  NULL,              /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTDCWFDTCorr_init() {
  for (int ich = 0; ich < NTDCCHAN; ++ich) {
    char tdc_bank[5], wfd_bank[5];

    sprintf(tdc_bank, "T4%02d", ich);
    const std::string det = gSetup->GetDetectorName(tdc_bank).substr(1);
    sprintf(wfd_bank, "%s", gSetup->GetBankName(det).c_str());
    if (!strlen(wfd_bank)) {
      printf("INFO: MTDCWFDTCorr: No corresponding WFD for bank %s\n",
             tdc_bank);
      continue;
    }
    printf("TDC bank %s, WFD bank %s, Detector %s\n", tdc_bank, wfd_bank, det.c_str());

    char hist[64]; sprintf(hist, "hTDCWFDTcorr_%s", det.c_str());
    vhTDCWFDTCorr[ich] = new TH1D(hist, hist, 20000, TIME_LOW, TIME_HIGH);
    vhTDCWFDTCorr[ich]->GetXaxis()->SetTitle("Timing Difference (ns)");
  }
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MTDCWFDTCorr(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
      gData->fTDCHitsToChannelMap;
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map =
        gData->fPulseIslandToChannelMap;

  for (int ich = 0; ich < NTDCCHAN; ++ich) {
    if (!vhTDCWFDTCorr[ich]) continue;
    char tdc_bank[5], wfd_bank[5];

    sprintf(tdc_bank, "T4%02d", ich);
    sprintf(wfd_bank, "%s", gSetup->GetBankName(gSetup->GetDetectorName(tdc_bank).substr(1)).c_str());

    const std::vector<int64_t>&       times  = tdc_map.at(tdc_bank);
    const std::vector<TPulseIsland*>& pulses = wfd_map.at(wfd_bank);
    for (int t = 0; t < times.size(); ++t) {
      for (int p = 0; p < pulses.size(); ++p) {
        static const double tdc_tick = 0.025; // ns
        const double dt = tdc_tick*times[t] - pulses[p]->GetPulseTime();
        if (dt < TIME_LOW)
          break;
        else if (dt < TIME_HIGH)
          vhTDCWFDTCorr[ich]->Fill(dt);
      }
    }
  }

  return SUCCESS;
}

/// @}
