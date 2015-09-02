////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTTScTCorrWFD
///
/// \brief
/// Timing correlations from within WFD to TDC target scintillator signal.
/// @{
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdint.h>
#include <cmath>
#include <string>
#include <map>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH2F.h"
#include "TDirectory.h"

//JG: added alcap includes
/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MTTScTCorrWFD_init(void);
static INT MTTScTCorrWFD(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
const double TIME_LOW = -3000., TIME_HIGH = 3000.;
  TH2* vvhTTScTCorrWFDT[NCRATE][MAXNCHANWFD];
  TH2* vvhTTScTCorrWFDE[NCRATE][MAXNCHANWFD];
  std::string WFDBANKS[NCRATE][MAXNCHANWFD];
  std::string TTSCBANK;
}

ANA_MODULE MTTScTCorrWFD_module =
{
  "MTTScTCorrWFD",    /* module name           */
  "John R Quirk",     /* author                */
  MTTScTCorrWFD,      /* event routine         */
  NULL,               /* BOR routine           */
  NULL,               /* EOR routine           */
  MTTScTCorrWFD_init, /* init routine          */
  NULL,               /* exit routine          */
  NULL,               /* parameter structure   */
  0,                  /* structure size        */
  NULL,               /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTTScTCorrWFD_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TTScTCorrWFD")->cd();

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      char bank[8], name[64], title[128];
      sprintf(bank, "D%d%02d", icrate, ich);
      sprintf(name, "hTTScTCorrWFDT_%s", bank);
      const int emax = std::pow(2, gSetup->GetNBits(bank));
      WFDBANKS[icrate][ich] = bank;
      sprintf(title,
              "TTSc timing correlation %s;WFD-TTSc Time (ns);Time WFD (ns)",
              gSetup->GetDetectorName(bank).c_str());
      vvhTTScTCorrWFDT[icrate][ich] = new TH2F(name, title,
					       1000, TIME_LOW, TIME_HIGH,
					       2000, 0., 110e6);
      sprintf(name, "hTTScTCorrWFDE_%s", bank);
      sprintf(title,
              "TTSc timing correlation %s;WFD-TTSc Time (ns);WFD Energy (ADC)",
              gSetup->GetDetectorName(bank).c_str());
      vvhTTScTCorrWFDE[icrate][ich] = new TH2F(name, title,
					       1000, TIME_LOW, TIME_HIGH,
					       emax, 0., emax);
    }
  }
  TTSCBANK = gSetup->GetBankName("TTSc");
  
  cwd->cd();
  return SUCCESS;
}

INT MTTScTCorrWFD(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;
  
  if (!tdc_map.count(TTSCBANK)) {
    printf("MTTScTCorrWFD: No reference hits TTSc!\n");
    return SUCCESS;
  }
  const std::vector<int64_t>& hits = tdc_map.at(TTSCBANK);
  
  
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    const double toff = gData->fTDCSynchronizationPulseOffset[icrate];
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if (!wfd_map.count(WFDBANKS[icrate][ich])) continue;
      const std::vector<TPulseIsland*>& tpis =
	wfd_map.at(WFDBANKS[icrate][ich]);
      
      for (int i = 0, j0 = 0; i < tpis.size(); ++i) {
        const double t = TICKWFD[icrate] * tpis[i]->GetTimeStamp() + toff;
	const double e = tpis[i]->GetPulseHeight();
        for (int j = j0; j < hits.size(); ++j) {
	  const double dt = t - TICKTDC * hits[j];
          if (dt < TIME_LOW) {
            break;
	  } else if (dt < TIME_HIGH) {
            vvhTTScTCorrWFDT[icrate][ich]->Fill(dt, t);
	    vvhTTScTCorrWFDE[icrate][ich]->Fill(dt, e);
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
