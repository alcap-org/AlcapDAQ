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
#include "TH2D.h"
#include "TDirectory.h"

//JG: added alcap includes
/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"


/*-- Module declaration --------------------------------------------*/
static INT MTTScTCorrWFD_init(void);
static INT MTTScTCorrWFD_eor(INT);
static INT MTTScTCorrWFD(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
using std::string;
using std::map;
using std::vector;
namespace {
  TDirectory* DIR;
  const double TIME_LOW = -3000., TIME_HIGH = 3000.;
  TH2* vvhTTScTCorrWFDT[NCRATE][MAXNCHANWFD];
  TH2* vvhTTScTCorrWFDE[NCRATE][MAXNCHANWFD];
  TH2* vvhTTScTCorrWFDT_Norm[NCRATE][MAXNCHANWFD];
  TH2* vvhTTScTCorrWFDE_Norm[NCRATE][MAXNCHANWFD];
  std::string WFDBANKS[NCRATE][MAXNCHANWFD];
  std::string TTSCBANK;
}

ANA_MODULE MTTScTCorrWFD_module =
{
  "MTTScTCorrWFD",    /* module name           */
  "John R Quirk",     /* author                */
  MTTScTCorrWFD,      /* event routine         */
  NULL,               /* BOR routine           */
  MTTScTCorrWFD_eor,  /* EOR routine           */
  MTTScTCorrWFD_init, /* init routine          */
  NULL,               /* exit routine          */
  NULL,               /* parameter structure   */
  0,                  /* structure size        */
  NULL,               /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MTTScTCorrWFD_init() {
  TDirectory* cwd = gDirectory;
  DIR = gDirectory->mkdir("TTScTCorrWFD");
  DIR->cd();

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      char bank[8], name[64], title[128];
      sprintf(bank, "D%d%02d", icrate, ich);
      WFDBANKS[icrate][ich] = bank;
      const int emax = std::pow(2, gSetup->GetNBits(bank));
      sprintf(name, "hTTScTCorrWFD_%s_T", bank);
      sprintf(title,
              "TTSc timing correlation %s;WFD-TTSc Time (ns);Time WFD (ns)",
              gSetup->GetDetectorName(bank).c_str());
      vvhTTScTCorrWFDT[icrate][ich] = new TH2D(name, title,
					       1000, TIME_LOW, TIME_HIGH,
					       200,  0.,       120.e6);
      vvhTTScTCorrWFDT[icrate][ich]->Sumw2();
      
      sprintf(name, "hTTScTCorrWFD_%s_E", bank);
      sprintf(title,
              "TTSc timing correlation %s;WFD-TTSc Time (ns);WFD Energy (ADC)",
              gSetup->GetDetectorName(bank).c_str());
      vvhTTScTCorrWFDE[icrate][ich] = new TH2D(name, title,
					       1000, TIME_LOW, TIME_HIGH,
					       emax/16.,  0., emax);
      vvhTTScTCorrWFDE[icrate][ich]->Sumw2();
      
      sprintf(name, "hTTScTCorrWFD_%s_Norm_T", bank);
      sprintf(title,
              "TTSc timing correlation %s (Normalized);WFD-TTSc Time (ns);Time WFD (ns)",
              gSetup->GetDetectorName(bank).c_str());
      vvhTTScTCorrWFDT_Norm[icrate][ich] = new TH2D(name, title,
						    1000, TIME_LOW, TIME_HIGH,
						    200,  0.,       120.e6);
      vvhTTScTCorrWFDT_Norm[icrate][ich]->Sumw2();
      
      sprintf(name, "hTTScTCorrWFD_%s_Norm_E", bank);
      sprintf(title,
              "TTSc timing correlation %s (Normalized);WFD-TTSc Time (ns);WFD Energy (ADC)",
              gSetup->GetDetectorName(bank).c_str());
      vvhTTScTCorrWFDE_Norm[icrate][ich] = new TH2D(name, title,
						    1000, TIME_LOW, TIME_HIGH,
						    emax/16, 0., emax);
      vvhTTScTCorrWFDE_Norm[icrate][ich]->Sumw2();
    }
  }
  TTSCBANK = gSetup->GetBankName("TTSc");
  
  cwd->cd();
  return SUCCESS;
}

INT MTTScTCorrWFD_eor(INT run_number) {
  TDirectory* cwd = gDirectory;
  DIR->cd();
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      TH2* h0 = vvhTTScTCorrWFDE[icrate][ich];
      string name(h0->GetName());
      name.erase(name.size()-2);
      h0->ProjectionX(name.c_str(), 0, -1, "e");

      h0 = vvhTTScTCorrWFDE_Norm[icrate][ich];
      h0->Scale(1./gData->NBlocks());
      name = h0->GetName();
      name.erase(name.size()-2);
      h0->ProjectionX(name.c_str(), 0, -1, "e");
    }
  }
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

      const double norm = 1./tpis.size();
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
            vvhTTScTCorrWFDT_Norm[icrate][ich]->Fill(dt, t, norm);
	    vvhTTScTCorrWFDE_Norm[icrate][ich]->Fill(dt, e, norm);
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
