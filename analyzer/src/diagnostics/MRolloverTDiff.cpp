////////////////////////////////////////////////////////////////////////////////
/// \defgroup MRolloverTDiff
///
/// \brief
/// Rollover timing diff
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
static INT MRolloverTDiff_init(void);
static INT MRolloverTDiff(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
static const int NCHANTDC = 32;
static TH1* hRolloverTDiff;


ANA_MODULE MRolloverTDiff_module =
{
  "MRolloverTDiff",    /* module name           */
  "John R Quirk",      /* author                */
  MRolloverTDiff,      /* event routine         */
  NULL,                /* BOR routine           */
  NULL,                /* EOR routine           */
  MRolloverTDiff_init, /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MRolloverTDiff_init() {
  hRolloverTDiff = new TH1D("hRolloverTDiff",
                            "Rollover Time Distribution;Ti-T(i-1) (ns)",
                            600, 0., 600e3);
  hRolloverTDiff->Sumw2();
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MRolloverTDiff(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;
  const std::string bank = gSetup->GetBankName("TRollover");

  if (!tdc_map.count(bank)) {
    printf("MRolloverTDiff: No hits TRollover!\n");
    return SUCCESS;
  }
  const std::vector<int64_t>& hits = tdc_map.at(bank);

  for (int ihit = 1; ihit < hits.size(); ++ihit) {
    static const double tdc_tick = 0.025; //ns
    hRolloverTDiff->Fill(tdc_tick*(hits[ihit] - hits[ihit-1]));
  }

  return SUCCESS;
}

/// @}
