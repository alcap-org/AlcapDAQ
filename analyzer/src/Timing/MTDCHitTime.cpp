////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTDCHitTime
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
#include <TH1F.h>
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
static INT MTDCHitTime_init(void);
static INT MTDCHitTime(EVENT_HEADER*, void*);
static INT MTDCHitTime_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  TDirectory* DIR;
  TH1* vhTDCHitTime[NCHANTDC];
  std::string TDCBANKS[NCHANTDC];
}

ANA_MODULE MTDCHitTime_module =
{
  "MTDCHitTime",    /* module name           */
  "John R Quirk",   /* author                */
  MTDCHitTime,      /* event routine         */
  NULL,             /* BOR routine           */
  MTDCHitTime_eor,  /* EOR routine           */
  MTDCHitTime_init, /* init routine          */
  NULL,             /* exit routine          */
  NULL,             /* parameter structure   */
  0,                /* structure size        */
  NULL,             /* initial parameters    */
};

INT MTDCHitTime_init() {
  TDirectory* cwd = gDirectory;
  DIR = gDirectory->mkdir("TDCHitTime");
  DIR->cd();

  // Create a histogram for each detector
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    const string det = gSetup->GetDetectorName(bank);
    TDCBANKS[ich] = bank;
    char name[64], title[128];
    sprintf(name, "hTDCHitTime_%s_%s", bank, det.c_str());
    sprintf(title, "TDC block hit times %s;Time (ns)", det.c_str());
    vhTDCHitTime[ich] = new TH1F(name, title, 120.e6/25.e2, 0., 120.e6);
    vhTDCHitTime[ich]->Sumw2();
  }
  cwd->cd();
  return SUCCESS;
}

INT MTDCHitTime_eor(INT run_number) {
  TDirectory* cwd = gDirectory;
  DIR->cd();
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    TH1* h0 = vhTDCHitTime[ich];
    string name(h0->GetName());
    string title(h0->GetTitle());
    name += "_normalized";
    title += " (normalized)";
    TH1* h = (TH1*)h0->Clone(name.c_str());
    h->SetTitle(title.c_str());
    h->Scale(1./gData->NMuRun());
  }
  cwd->cd();
  return SUCCESS;
}

INT MTDCHitTime(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;

  for (int ich = 0; ich < NCHANTDC; ++ich) {
    if (!tdc_map.count(TDCBANKS[ich])) continue;
    const vector<int64_t>& hits = tdc_map.at(TDCBANKS[ich]);
    for (int i = 0; i < hits.size(); ++i)
      vhTDCHitTime[ich]->Fill(TICKTDC*hits[i]);
  }

  return SUCCESS;
}

/// @}
