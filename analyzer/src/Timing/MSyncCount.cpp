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
INT MSyncCount_init(void);
INT MSyncCount_eor(INT);
INT MSyncCount(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
using std::string;
using std::vector;
using std::map;
namespace {
  TDirectory* DIR;
  const int HEIGHTCUT = 100;
  TH1F* vhSyncCountWFD[NCRATE];
  TH1F* hSyncCountTDC;
  string SyncBankWFD[NCRATE];
  string SyncBankTDC;
}
  
ANA_MODULE MSyncCount_module =
{
  "MSyncCount",    /* module name           */
  "John R Quirk",  /* author                */
  MSyncCount,      /* event routine         */
  NULL,            /* BOR routine           */
  MSyncCount_eor,  /* EOR routine           */
  MSyncCount_init, /* init routine          */
  NULL,            /* exit routine          */
  NULL,            /* parameter structure   */
  0,               /* structure size        */
  NULL,            /* initial parameters    */
};

INT MSyncCount_init() {
  TDirectory* cwd = gDirectory;
  DIR = gDirectory->mkdir("SyncCount/");
  DIR->cd();

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

INT MSyncCount_eor(INT run_number) {
  TDirectory* cwd = gDirectory;
  DIR->cd();
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    TH1* h0 = vhSyncCountWFD[icrate];
    if (h0) {
      string name(h0->GetName());
      string title(h0->GetTitle());
      name += "_Norm"; title += " (normalized)";
      TH1* h = (TH1*)h0->Clone(name.c_str());
      h->SetTitle(title.c_str());
      h->Scale(1./h->GetEntries());
    }
  }
  TH1* h0 = hSyncCountTDC;
  if (h0) {
    string name(h0->GetName());
    string title(h0->GetTitle());
    name += "_Norm"; title += " (normalized)";
    TH1* h = (TH1*)h0->Clone(name.c_str());
    h->SetTitle(title.c_str());
    h->Scale(1./h->GetEntries());
  }
  cwd->cd();
  return SUCCESS;
}

INT MSyncCount(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;
  const map< string, vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    if (SyncBankWFD[icrate].empty()) continue;
    if (!wfd_map.count(SyncBankWFD[icrate])) continue;

    int npulse = 0;
    const vector<TPulseIsland*>& pulses = wfd_map.at(SyncBankWFD[icrate]);
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
