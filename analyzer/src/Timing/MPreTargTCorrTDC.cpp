////////////////////////////////////////////////////////////////////////////////
/// \defgroup MPreTargTCorrTDC
///
/// \brief
/// Timing correlations from within TDC
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
static INT MPreTargTCorrTDC_init(void);
static INT MPreTargTCorrTDC(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
static const int NCHANTDC = 32;
const double TIME_LOW = -10e3, TIME_HIGH = 10e3;
static TH1* vhPreTargTCorrTDC[NCHANTDC];


ANA_MODULE MPreTargTCorrTDC_module =
{
  "MPreTargTCorrTDC",    /* module name           */
  "John R Quirk",        /* author                */
  MPreTargTCorrTDC,      /* event routine         */
  NULL,                  /* BOR routine           */
  NULL,                  /* EOR routine           */
  MPreTargTCorrTDC_init, /* init routine          */
  NULL,                  /* exit routine          */
  NULL,                  /* parameter structure   */
  0,                     /* structure size        */
  NULL,                  /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MPreTargTCorrTDC_init() {
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    char hist[64]; sprintf(hist, "hPreTargTCorrTDC_%s", bank);
    vhPreTargTCorrTDC[ich] = new TH1D(hist, hist, 20000, TIME_LOW, TIME_HIGH);
    vhPreTargTCorrTDC[ich]->GetXaxis()->SetTitle("Timing Difference (ns)");
  }
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MPreTargTCorrTDC(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
  const std::string ref_bank = gSetup->GetBankName("TPreTarg");
  if (!tdc_map.count(ref_bank)) {
    printf("MPreTargTCorrTDC: No reference hits TPreTarg!\n");
    return SUCCESS;
  }
  const std::vector<int64_t>& ref_hits = tdc_map.at(ref_bank);

  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    if (!tdc_map.count(bank)) continue;

    const std::vector<int64_t>& hits = tdc_map.at(bank);
    for (int i = 0; i < hits.size(); ++i) {
      for (int j = 0; j < ref_hits.size(); ++j) {
        static const double clock_tick = 0.025; // ns
        const int64_t dt = hits[i] - ref_hits[j];
        if (dt < TIME_LOW)
          break;
        else if (dt < TIME_HIGH)
          vhPreTargTCorrTDC[ich]->Fill(clock_tick*dt);
      }
    }
  }

  return SUCCESS;
}

/// @}
