////////////////////////////////////////////////////////////////////////////////
/// \defgroup MSyncTCorrGeCHEH
///
/// \brief
/// Timing correlations from Sync pulses (Crate 4 WFD and TDC) and Ge WFD (raw and CF)
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
static INT MSyncTCorrGeCHEH_init(void);
static INT MSyncTCorrGeCHEH_eor(INT);
static INT MSyncTCorrGeCHEH(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  //const double TIME_LOW = -10000, TIME_HIGH = 10000; //ns lower end for Ge after CF
  //const double TIME_LOW = -1e6, TIME_HIGH = 1e6; //ns should cover all pulses
  const double TIME_LOW = -2e6, TIME_HIGH = 2e6; //ns should cover all pulses
  TH1* vvhSyncTCorrGeRaw[2];
  TH1* vvhSyncTCorrGeCF[2];
  //std::string WFDBANKS[NCRATE][MAXNCHANWFD];
  std::string WFDBANKS[MAXNCHANWFD];
  std::string TDCBANKS[NCHANTDC];
}

ANA_MODULE MSyncTCorrGeCHEH_module =
{
  "MSyncTCorrGeCHEH",    /* module name           */
  "Cole Kampa",    /* author                */
  MSyncTCorrGeCHEH,      /* event routine         */
  NULL,              /* BOR routine           */
  MSyncTCorrGeCHEH_eor,  /* EOR routine           */
  MSyncTCorrGeCHEH_init, /* init routine          */
  NULL,              /* exit routine          */
  NULL,              /* parameter structure   */
  0,                 /* structure size        */
  NULL,              /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MSyncTCorrGeCHEH_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("SyncTCorrGeCHEH")->cd();

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
    char histname[64]; sprintf(histname, "hSyncTCorrGeRaw_%s", bank);
    char histtitle[64]; sprintf(histtitle, "%s TCorr with GeCHEH (raw)", detname.c_str());
    char histname_CF[64]; sprintf(histname_CF, "hSyncTCorrGeCF_%s", bank);
    char histtitle_CF[64]; sprintf(histtitle_CF, "%s TCorr with GeCHEH (CF)", detname.c_str());
    vvhSyncTCorrGeRaw[0] = new TH1D(histname, histtitle, 20000, TIME_LOW, TIME_HIGH);
    vvhSyncTCorrGeCF[0] = new TH1D(histname_CF, histtitle_CF, 20000, TIME_LOW, TIME_HIGH);
    vvhSyncTCorrGeRaw[0]->GetXaxis()->SetTitle("Timing Difference Ge-Sync (ns)");
    vvhSyncTCorrGeCF[0]->GetXaxis()->SetTitle("Timing Difference Ge-Sync (ns)");
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
    char histname[64]; sprintf(histname, "hSyncTCorrGeRaw_%s", bank);
    char histtitle[64]; sprintf(histtitle, "%s TCorr with GeCHEH (raw)", detname.c_str());
    char histname_CF[64]; sprintf(histname_CF, "hSyncTCorrSyncGeCF_%s", bank);
    char histtitle_CF[64]; sprintf(histtitle_CF, "%s TCorr with GeCHEH (CF)", detname.c_str());
    vvhSyncTCorrGeRaw[1] = new TH1D(histname, histtitle, 20000, TIME_LOW, TIME_HIGH);
    vvhSyncTCorrGeCF[1] = new TH1D(histname_CF, histtitle_CF, 20000, TIME_LOW, TIME_HIGH);
    vvhSyncTCorrGeRaw[1]->GetXaxis()->SetTitle("Timing Difference Ge-TSync (ns)");
    vvhSyncTCorrGeCF[1]->GetXaxis()->SetTitle("Timing Difference Ge-TSync (ns)");
    TDCBANKS[ich] = bank;
  }

  cwd->cd();
  return SUCCESS;
}

INT MSyncTCorrGeCHEH_eor(INT run_number) {
  return SUCCESS;
}

/*-- module event routine -----------------------------------------*/
INT MSyncTCorrGeCHEH(EVENT_HEADER *pheader, void *pevent) {
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
      // for (int i = 0, j0 = 0; i < tpis.size(); ++i) {
      for (int i = 0, j0 = 0; i < ref_tpis.size(); ++i) {
        // raw and CF Ge timestamps
        double TS;
        if (useCF) {
          TS = ref_tpis[i]->GetTimeStampCF();
        } else {
          TS = ref_tpis[i]->GetTimeStamp();
        }
        // for (int j = j0; j < ref_tpis.size(); ++j) {
        for (int j = j0; j < tpis.size(); ++j) {
          double wfd_timestamp = 0;
          if (tpis[j]->HasCFTime()) {
            wfd_timestamp = tpis[j]->GetTimeStampCF();
          } else {
            wfd_timestamp = (double)tpis[j]->GetTimeStamp();
          }
          const double t = TICKWFD[icrate] * wfd_timestamp;
          const double dt = tickref * TS - t;
          if (dt < TIME_LOW) {
            break;
          } else if (dt < TIME_HIGH) {
            if (useCF) {
              vvhSyncTCorrGeCF[0]->Fill(dt);
            } else {
              vvhSyncTCorrGeRaw[0]->Fill(dt);
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
      //for (int i = 0, j0 = 0; i < hits.size(); ++i) {
      for (int i = 0, j0 = 0; i < ref_tpis.size(); ++i) {
        // raw and CF Ge timestamps
        double TS;
        if (useCF) {
          TS = ref_tpis[i]->GetTimeStampCF();
        } else {
          TS = ref_tpis[i]->GetTimeStamp();
        }
        // for (int j = j0; j < ref_tpis.size(); ++j) {
        for (int j = j0; j < hits.size(); ++j) {
          const double t = TICKTDC*hits[j];
          const double dt = ((tickref * TS) + toff) - t;
          if (dt < TIME_LOW) {
            break;
          } else if (dt < TIME_HIGH) {
            if (useCF) {
              vvhSyncTCorrGeCF[1]->Fill(dt);
            } else {
              vvhSyncTCorrGeRaw[1]->Fill(dt);
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
