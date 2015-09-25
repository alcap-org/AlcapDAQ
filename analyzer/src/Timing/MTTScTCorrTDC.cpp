////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTTScTCorrTDC
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
#include "TDirectory.h"

//JG: added alcap includes
/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MTTScTCorrTDC_init(void);
static INT MTTScTCorrTDC_eor(INT);
static INT MTTScTCorrTDC(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  const double TIME_LOW = -1500., TIME_HIGH = 1500.; // ns
  TH1* vhTTScTCorrTDC[NCHANTDC];
  TH1* vhTTScTCorrTDCNorm[NCHANTDC];
  std::string TDCBANKS[NCHANTDC];
}

ANA_MODULE MTTScTCorrTDC_module =
{
  "MTTScTCorrTDC",    /* module name           */
  "John R Quirk",     /* author                */
  MTTScTCorrTDC,      /* event routine         */
  NULL,               /* BOR routine           */
  MTTScTCorrTDC_eor,  /* EOR routine           */
  MTTScTCorrTDC_init, /* init routine          */
  NULL,               /* exit routine          */
  NULL,               /* parameter structure   */
  0,                  /* structure size        */
  NULL,               /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTTScTCorrTDC_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TTScTCorrTDC")->cd();

  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    char histname[64]; sprintf(histname, "hTTScTCorrTDC_%s", bank);
    char histtitle[64]; sprintf(histtitle, "TTSc TCorr with %s", gSetup->GetDetectorName(bank).c_str());
    vhTTScTCorrTDC[ich] = new TH1D(histname, histtitle, 20000, TIME_LOW, TIME_HIGH);
    vhTTScTCorrTDC[ich]->Sumw2();
    vhTTScTCorrTDC[ich]->GetXaxis()->SetTitle("Timing Difference (ns)");
    sprintf(histname, "hTTScTCorrTDC_%s_Norm", bank);
    sprintf(histtitle, "TTSc TCorr with %s (Normalized)", gSetup->GetDetectorName(bank).c_str());
    vhTTScTCorrTDCNorm[ich] = new TH1D(histname, histtitle, 20000, TIME_LOW, TIME_HIGH);
    vhTTScTCorrTDCNorm[ich]->Sumw2();
    vhTTScTCorrTDCNorm[ich]->GetXaxis()->SetTitle("Timing Difference (ns)");
    TDCBANKS[ich] = bank;
  }
  cwd->cd();
  return SUCCESS;
}

INT MTTScTCorrTDC_eor(INT run_number) {
  for (int ich = 0; ich < NCHANTDC; ++ich)
    vhTTScTCorrTDCNorm[ich]->Scale(1./gData->NBlocks());
  return SUCCESS;
}

/*-- module event routine -----------------------------------------*/
INT MTTScTCorrTDC(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;

  const std::string ref_bank = gSetup->GetBankName("TTSc");
  if (!tdc_map.count(ref_bank)) {
    printf("MTTScTCorrTDC: No reference hits TTSc!\n");
    return SUCCESS;
  }
  const std::vector<int64_t>& ref_hits = tdc_map.at(ref_bank);

  for (int ich = 0; ich < NCHANTDC; ++ich) {
    if (!tdc_map.count(TDCBANKS[ich])) continue;

    const std::vector<int64_t>& hits = tdc_map.at(TDCBANKS[ich]);
    const double norm = 1./hits.size();
    for (int i = 0, j0 = 0; i < hits.size(); ++i) {
      for (int j = j0; j < ref_hits.size(); ++j) {
        const double dt = TICKTDC*(hits[i] - ref_hits[j]);
        if (dt < TIME_LOW) {
          break;
        } else if (dt < TIME_HIGH) {
          vhTTScTCorrTDC[ich]->Fill(dt);
	  vhTTScTCorrTDCNorm[ich]->Fill(dt, norm);
	} else {
	  ++j0;
	}
      }
    }
  }

  return SUCCESS;
}

/// @}
