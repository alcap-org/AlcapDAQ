////////////////////////////////////////////////////////////////////////////////
/// \defgroup MGeCHEHTCorrSync
///
/// \brief
/// Timing correlations from Ge WFD (raw and CF) from Sync pulses (Crate 4 WFD and TDC)
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
static INT MGeCHEHTCorrSync_init(void);
static INT MGeCHEHTCorrSync_eor(INT);
static INT MGeCHEHTCorrSync(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  //const double TIME_LOW = -10000, TIME_HIGH = 10000; //ns lower end for Ge after CF
  //const double TIME_LOW = -1e6, TIME_HIGH = 1e6; //ns should cover all pulses
  const double TIME_LOW = -2e6, TIME_HIGH = 2e6; //ns should cover all pulses
  TH1* vvhGeRawTCorrSync[2];
  TH1* vvhGeCFTCorrSync[2];
  //std::string WFDBANKS[NCRATE][MAXNCHANWFD];
  std::string WFDBANKS[MAXNCHANWFD];
  std::string TDCBANKS[NCHANTDC];
}

ANA_MODULE MGeCHEHTCorrSync_module =
{
  "MGeCHEHTCorrSync",    /* module name           */
  "Cole Kampa",    /* author                */
  MGeCHEHTCorrSync,      /* event routine         */
  NULL,              /* BOR routine           */
  MGeCHEHTCorrSync_eor,  /* EOR routine           */
  MGeCHEHTCorrSync_init, /* init routine          */
  NULL,              /* exit routine          */
  NULL,              /* parameter structure   */
  0,                 /* structure size        */
  NULL,              /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MGeCHEHTCorrSync_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("GeCHEHTCorrSync")->cd();

  std::string Gebank = gSetup->GetBankName("GeCHEH");
  int icrate = Gebank[1] - '0';

  // Add Sync4 (WFD)
  for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
    char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
    std::string detname = gSetup->GetDetectorName(bank);
    if (detname.find("Sync") == std::string::npos) {
      WFDBANKS[ich] = "BLANK";
      continue;
    }
    char histname[64]; sprintf(histname, "hGeRawTCorrSync_%s", bank);
    char histtitle[64]; sprintf(histtitle, "GeCHEH (raw) TCorr with %s", detname.c_str());
    char histname_CF[64]; sprintf(histname_CF, "hGeCFTCorrSync_%s", bank);
    char histtitle_CF[64]; sprintf(histtitle_CF, "GeCHEH (CF) TCorr with %s", detname.c_str());
    vvhGeRawTCorrSync[0] = new TH1D(histname, histtitle, 20000, TIME_LOW, TIME_HIGH);
    vvhGeCFTCorrSync[0] = new TH1D(histname_CF, histtitle_CF, 20000, TIME_LOW, TIME_HIGH);
    vvhGeRawTCorrSync[0]->GetXaxis()->SetTitle("Timing Difference Ge-Sync (ns)");
    vvhGeCFTCorrSync[0]->GetXaxis()->SetTitle("Timing Difference Ge-Sync (ns)");
    WFDBANKS[ich] = bank;
  }

  // Add TSync
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    std::string detname = gSetup->GetDetectorName(bank);
    if (detname.find("TSync") == std::string::npos) {
      TDCBANKS[ich] = "BLANK";
      continue;
    }
    char histname[64]; sprintf(histname, "hGeRawTCorrSync_%s", bank);
    char histtitle[64]; sprintf(histtitle, "GeCHEH (raw) TCorr with %s", detname.c_str());
    char histname_CF[64]; sprintf(histname_CF, "hGeCFTCorrSync_%s", bank);
    char histtitle_CF[64]; sprintf(histtitle_CF, "GeCHEH (CF) TCorr with %s", detname.c_str());
    vvhGeRawTCorrSync[1] = new TH1D(histname, histtitle, 20000, TIME_LOW, TIME_HIGH);
    vvhGeCFTCorrSync[1] = new TH1D(histname_CF, histtitle_CF, 20000, TIME_LOW, TIME_HIGH);
    vvhGeRawTCorrSync[1]->GetXaxis()->SetTitle("Timing Difference Ge-TSync (ns)");
    vvhGeCFTCorrSync[1]->GetXaxis()->SetTitle("Timing Difference Ge-TSync (ns)");
    TDCBANKS[ich] = bank;
  }

  cwd->cd();
  return SUCCESS;
}

INT MGeCHEHTCorrSync_eor(INT run_number) {
  return SUCCESS;
}

/*-- module event routine -----------------------------------------*/
INT MGeCHEHTCorrSync(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;

  const static std::string ref_bank = gSetup->GetBankName("GeCHEH");
  if (!wfd_map.count(ref_bank)) {
    printf("MGeCHEHTCorrSync: No reference pulses GeCHEH!\n");
    return SUCCESS;
  }
  int icrate = ref_bank[1] - '0';
  const double toff = gData->fTDCSynchronizationPulseOffset[icrate];
  const std::vector<TPulseIsland*>& ref_tpis = wfd_map.at(ref_bank);
  const static double tickref = TICKWFD[ref_bank[1]-'0'];

  // add Sync4
  for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
    if (!wfd_map.count(WFDBANKS[ich])) continue;

    const std::vector<TPulseIsland*>& tpis = wfd_map.at(WFDBANKS[ich]);
    for (int k = 0; k < 2; ++k) {
      bool useCF = (k == 0);
      for (int i = 0, j0 = 0; i < tpis.size(); ++i) {
        double wfd_timestamp = 0;
        if (tpis[i]->HasCFTime()) {
          wfd_timestamp = tpis[i]->GetTimeStampCF();
        } else {
          wfd_timestamp = (double)tpis[i]->GetTimeStamp();
        }
        //const double t = TICKWFD[icrate] * (wfd_timestamp + toff);
        const double t = TICKWFD[icrate] * wfd_timestamp;
        for (int j = j0; j < ref_tpis.size(); ++j) {
          // raw and CF Ge timestamps
          double TS;
          if (useCF) {
            TS = ref_tpis[j]->GetTimeStampCF();
          } else {
            TS = ref_tpis[j]->GetTimeStamp();
          }
          const double dt = tickref * TS - t;
          if (dt < TIME_LOW) {
            break;
          } else if (dt < TIME_HIGH) {
            if (useCF) {
              vvhGeCFTCorrSync[0]->Fill(dt);
            } else {
              vvhGeRawTCorrSync[0]->Fill(dt);
            }
          } else {
            ++j0;
          }
        }
      }
    }
  }

  // Add TSync
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    if (!tdc_map.count(TDCBANKS[ich])) continue;

    const std::vector<int64_t>& hits = tdc_map.at(TDCBANKS[ich]);
    for (int k = 0; k < 2; ++k) {
      bool useCF = (k == 0);
      for (int i = 0, j0 = 0; i < hits.size(); ++i) {
        const double t = TICKTDC*hits[i];
        for (int j = j0; j < ref_tpis.size(); ++j) {
          // raw and CF Ge timestamps
          double TS;
          if (useCF) {
            TS = ref_tpis[j]->GetTimeStampCF();
          } else {
            TS = ref_tpis[j]->GetTimeStamp();
          }
          const double dt = ((tickref * TS) + toff) - t;
          if (dt < TIME_LOW) {
            break;
          } else if (dt < TIME_HIGH) {
            if (useCF) {
              vvhGeCFTCorrSync[1]->Fill(dt);
            } else {
              vvhGeRawTCorrSync[1]->Fill(dt);
            }
          } else {
            ++j0;
          }
        }
      }
    }
  }

  return SUCCESS;
}

/// @}
