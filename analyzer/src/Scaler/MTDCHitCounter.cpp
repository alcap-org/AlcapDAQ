////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTDCHitCounter
/// \ingroup lldq
/// \author John Quirk
///
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <utility>
#include <cmath>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1D.h>
#include <TDirectory.h>

/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
static INT MTDCHitCounter_init(void);
static INT MTDCHitCounter(EVENT_HEADER*, void*);
static INT MTDCHitCounter_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  TDirectory* DIR;
  TH1* vhTDCHitCounter[NCHANTDC];
  TH1* vhTDCHitRates[NCHANTDC];
  std::string TDCBANKS[NCHANTDC];
}

ANA_MODULE MTDCHitCounter_module =
{
  "MTDCHitCounter",    /* module name           */
  "John R Quirk",      /* author                */
  MTDCHitCounter,      /* event routine         */
  NULL,                /* BOR routine           */
  MTDCHitCounter_eor,  /* EOR routine           */
  MTDCHitCounter_init, /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};

INT MTDCHitCounter_init() {
  TDirectory* cwd = gDirectory;
  DIR = gDirectory->mkdir("TDCHitCounter");
  DIR->cd();

  // Create a histogram for each detector
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    const string det = gSetup->GetDetectorName(bank);
    TDCBANKS[ich] = bank;

    char name[64], title[128];
    sprintf(name, "hTDCHitCounter_%s_%s", bank, det.c_str());
    sprintf(title, "TDC hits per block %s;Hits", det.c_str());
    vhTDCHitCounter[ich] = new TH1D(name, title, 10000, 0., 10000.);

    sprintf(name, "hTDCHitRate_%s_%s", bank, det.c_str());
    sprintf(title, "TDC rates per block %s;Hits", det.c_str());
    vhTDCHitRates[ich] = new TH1D(name, title, 10000, 0., 10.);
    
  }
  cwd->cd();
  return SUCCESS;
}

INT MTDCHitCounter_eor(INT run_number) {
  TDirectory* cwd = gDirectory;
  DIR->cd();
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    TH1* h0 = vhTDCHitCounter[ich];
    std::string name(h0->GetName());
    std::string title(h0->GetTitle());
    name += "_normalized"; title += " (normalized)";
    TH1* h = (TH1*)vhTDCHitCounter[ich]->Clone(name.c_str());
    h->SetTitle(title.c_str());
    h->Scale(1./h->GetEntries());

    h0 = vhTDCHitRates[ich];
    name = h0->GetName();
    title = h0->GetTitle();
    name += "_normalized"; title += " (normalized)";
    h = (TH1*)vhTDCHitRates[ich]->Clone(name.c_str());
    h->SetTitle(title.c_str());
    h->Scale(1./h->GetEntries());
  }
  cwd->cd();
  return SUCCESS;
}

INT MTDCHitCounter(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;

  for (int ich = 0; ich < NCHANTDC; ++ich) {
    if (tdc_map.count(TDCBANKS[ich])) {
      const int n = tdc_map.at(TDCBANKS[ich]).size();
      vhTDCHitCounter[ich]->Fill(n);
      vhTDCHitRates[ich]->Fill(n/(double)gData->NMuBlock());
    }
  }

  return SUCCESS;
}

/// @}
