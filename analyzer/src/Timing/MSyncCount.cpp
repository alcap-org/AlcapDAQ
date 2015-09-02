/********************************************************************\

Name:         MSyncCount
Created by:   John R Quirk

Contents:     A module to fill a histogram of the pulse lengths from each channel

\********************************************************************/

/* Standard includes */
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <utility>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1F.h"
#include "TH2.h"
#include "TDirectory.h"

/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"


/*-- Module declaration --------------------------------------------*/
INT  MSyncCount_init(void);
INT  MSyncCount(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  const int HEIGHTCUT = 100;
  TH1F* vhSyncCountWFD[NCRATE];
  TH1F* hSyncCountTDC;
  std::string SyncBankWFD[NCRATE];
  std::string SyncBankTDC;
}
  
ANA_MODULE MSyncCount_module =
{
  "MSyncCount",    /* module name           */
  "John R Quirk",  /* author                */
  MSyncCount,      /* event routine         */
  NULL,            /* BOR routine           */
  NULL,            /* EOR routine           */
  MSyncCount_init, /* init routine          */
  NULL,            /* exit routine          */
  NULL,            /* parameter structure   */
  0,               /* structure size        */
  NULL,            /* initial parameters    */
};

/** This method initializes histograms.
*/
INT MSyncCount_init() {
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("SyncCount/")->cd();

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    char tmp[8]; sprintf(tmp, "%d", icrate);
    SyncBankWFD[icrate] = gSetup->GetBankName(std::string("SyncCrate") + tmp);
    if (SyncBankWFD[icrate].empty()) {
      vhSyncCountWFD[icrate] = NULL;
      continue;
    }
    char name[32]; sprintf(name, "hSyncCount_Crate%d", icrate);
    char title[64]; sprintf(title,
                            "Sync Pulse Count Crate %d;Pulses per block",
                            icrate);
    vhSyncCountWFD[icrate] = new TH1F(name, title, 70, 0., 70.);
  }
  SyncBankTDC = gSetup->GetBankName(std::string("TSync"));
  if (SyncBankTDC.empty()) {
    hSyncCountTDC = NULL;
  } else {
    char name[32]; sprintf(name, "hSyncCount_TDC");
    char title[64]; sprintf(title, "Sync Pulse Count TDC;Pulses per block");
    hSyncCountTDC = new TH1F(name, title, 70, 0., 70.);
  }

  cwd->cd();
  return SUCCESS;
}

INT MSyncCount(EVENT_HEADER *pheader, void *pevent) {
  const std::map< std::string, std::vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;
  const std::map< std::string, std::vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    if (SyncBankWFD[icrate].empty()) continue;
    if (!wfd_map.count(SyncBankWFD[icrate])) continue;

    int npulse = 0;
    const std::vector<TPulseIsland*>& pulses = wfd_map.at(SyncBankWFD[icrate]);
    for (int ipulse = 0; ipulse < pulses.size(); ++ipulse)
      if (pulses[ipulse]->GetPulseHeight() > HEIGHTCUT)
	++npulse;
    vhSyncCountWFD[icrate]->Fill(npulse);
  }

  if (SyncBankTDC.empty()) return SUCCESS;
  
  if(tdc_map.count(SyncBankTDC))
    hSyncCountTDC->Fill(tdc_map.at(SyncBankTDC).size());

  return SUCCESS;
}
