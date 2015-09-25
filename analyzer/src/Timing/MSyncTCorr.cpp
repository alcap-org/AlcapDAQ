////////////////////////////////////////////////////////////////////////////////
/// \defgroup MSyncTCorr
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
#include "TH2D.h"
#include "TDirectory.h"

//JG: added alcap includes
/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"


extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
using std::string;
using std::vector;
using std::map;

static INT MSyncTCorr_init(void);
static INT MSyncTCorr(EVENT_HEADER*, void*);

namespace {
  const double TIME_LOW = 0., TIME_HIGH = 3.e3; // ns; ~550Hz pulser
  string vsSyncBankWFD[NCRATE];
  string sSyncBankTDC;
  //TH1* vhSyncTCorr[NCRATE];
  TH2* vhSyncTCorr[NCRATE];
}

ANA_MODULE MSyncTCorr_module =
{
  "MSyncTCorr",    /* module name           */
  "John R Quirk",  /* author                */
  MSyncTCorr,      /* event routine         */
  NULL,            /* BOR routine           */
  NULL,            /* EOR routine           */
  MSyncTCorr_init, /* init routine          */
  NULL,            /* exit routine          */
  NULL,            /* parameter structure   */
  0,               /* structure size        */
  NULL,            /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT MSyncTCorr_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("SyncTCorr/")->cd();
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    char hist[64]; sprintf(hist, "hPulserTCorr_Crate%d", icrate);
    char det[32]; sprintf(det, "SyncCrate%d", icrate);
    vsSyncBankWFD[icrate] = gSetup->GetBankName(det);
    //vhSyncTCorr[icrate] = new TH1D(hist, hist, 3000, TIME_LOW, TIME_HIGH);
    vhSyncTCorr[icrate] = new TH2D(hist, hist,
				   3000,  TIME_LOW, TIME_HIGH,
				   1100, 0.,       110.e6);
    vhSyncTCorr[icrate]->Sumw2();
    vhSyncTCorr[icrate]->GetXaxis()->SetTitle("Timing Difference TDC-WFD (ns)");
    vhSyncTCorr[icrate]->GetYaxis()->SetTitle("TDC Time (ns)");
  }
  sSyncBankTDC = gSetup->GetBankName("TSync");
  cwd->cd();
  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT MSyncTCorr(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<int64_t> >& tdc_map =
      gData->fTDCHitsToChannelMap;
  const map< string, vector<TPulseIsland*> >& wfd_map =
      gData->fPulseIslandToChannelMap;

  if (!tdc_map.count(sSyncBankTDC)) {
    printf("MSyncTCorr: No reference hits TSync!\n");
    return SUCCESS;
  }
  const vector<int64_t>& hits = tdc_map.at(sSyncBankTDC);

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    if (!wfd_map.count(vsSyncBankWFD[icrate]) ||
	gData->fTDCSynchronizationPulseIndex[icrate] == -1) continue;

    const double toff = gData->fTDCSynchronizationPulseOffset[icrate];
    const vector<TPulseIsland*>& tpis = wfd_map.at(vsSyncBankWFD[icrate]);
    for (int t = 0, p0 = 0; t < hits.size(); ++t) {
      for (int p = p0; p < tpis.size(); ++p) {
        const double dt = TICKTDC*hits[t] -
	  TICKWFD[icrate]*tpis[p]->GetTimeStamp() -
	  toff;
        if (dt < TIME_LOW)
	  break;
        else if (dt < TIME_HIGH)
	  //	  vhSyncTCorr[icrate]->Fill(dt);
	  vhSyncTCorr[icrate]->Fill(dt, TICKTDC*hits[t]);
	else
	  ++p0;
      }
    }
  }

  return SUCCESS;
}

/// @}
