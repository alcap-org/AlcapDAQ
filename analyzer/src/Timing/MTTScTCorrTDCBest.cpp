////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTTScTCorrTDCBest
///
/// \brief
/// Timing correlations from within TDC
/// where only the best-matched TTSc pulse is included in the plot.
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


/*-- Module declaration --------------------------------------------*/
static INT MTTScTCorrTDCBest_init(void);
static INT MTTScTCorrTDCBest(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;

/// \brief
/// List of BU CAEN bank names for the event loop.
//static const int NCHANTDC = 32;
//const double TIME_LOW = -10e3, TIME_HIGH = 10e3; // ns
const double TIME_LOW = -2500., TIME_HIGH = 7500.; // ns
static TH1* vhTTScTCorrTDCBest[NCHANTDC];


ANA_MODULE MTTScTCorrTDCBest_module =
{
  "MTTScTCorrTDCBest",    /* module name           */
  "Cole Kampa",    /* author                */
  MTTScTCorrTDCBest,      /* event routine         */
  NULL,              /* BOR routine           */
  NULL,              /* EOR routine           */
  MTTScTCorrTDCBest_init, /* init routine          */
  NULL,              /* exit routine          */
  NULL,              /* parameter structure   */
  0,                 /* structure size        */
  NULL,              /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTTScTCorrTDCBest_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TTScTCorrTDCBest")->cd();
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    char histname[64]; sprintf(histname, "hTTScTCorrTDCBest_%s", bank);
    char histtitle[64]; sprintf(histtitle, "TTSc TCorr (best) with %s", gSetup->GetDetectorName(bank).c_str());
    vhTTScTCorrTDCBest[ich] = new TH1D(histname, histtitle, 40000, TIME_LOW, TIME_HIGH);
    //vhTTScTCorrTDCBest[ich]->Sumw2();
    vhTTScTCorrTDCBest[ich]->GetXaxis()->SetTitle("Timing Difference (ns)");
  }
  cwd->cd();
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MTTScTCorrTDCBest(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;

  const std::string ref_bank = gSetup->GetBankName("TTSc");
  if (!tdc_map.count(ref_bank)) {
    printf("MTTScTCorrTDCBest: No reference hits TTSc!\n");
    return SUCCESS;
  }
  const std::vector<int64_t>& ref_hits = tdc_map.at(ref_bank);

  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[8]; sprintf(bank, "T4%02d", ich);
    if (!tdc_map.count(bank)) continue;

    const std::vector<int64_t>& hits = tdc_map.at(bank);
    for (int i = 0, j0 = 0; i < hits.size(); ++i) {
      //bool to_fill = true;
      bool to_fill = false;
      double dt_min = TIME_HIGH;
      for (int j = j0; j < ref_hits.size(); ++j) {
        //static const double clock_tick = 0.025; // ns
        //const double dt = clock_tick*(hits[i] - ref_hits[j]);
        const double dt = TICKTDC*(hits[i] - ref_hits[j]);
        //double dt = clock_tick*(hits[i] - ref_hits[j]);
        //dt_min = std::min(dt_min, fabs(dt));
        if (dt < TIME_LOW)
        {
          //to_fill = false;
          break;
        }
        else if (dt < TIME_HIGH)
        {
          //vhTScTCorrTDC[ich]->Fill(dt);
        }
      	else
        {
      	  ++j0;
        }
        // check whether to update the dt_min
        // if we do at least once, then we should fill the histogram
        if (fabs(dt) < fabs(dt_min)) {
          dt_min = dt;
          to_fill = true;
        }
      }
      if (to_fill) {
        // only fill once per hit, using the smallest dt
        vhTTScTCorrTDCBest[ich]->Fill(dt_min);
      }
    }
  }

  return SUCCESS;
}

/// @}
