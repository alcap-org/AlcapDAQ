////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTDCClockBitFreq
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
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;

/*-- Module declaration --------------------------------------------*/
static INT MTDCClockBitFreq_init(void);
static INT MTDCClockBitFreq(EVENT_HEADER*, void*);
static INT MTDCClockBitFreq_eor(int);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  TDirectory* DIR;
  TH1* vhTDCClockBitFreq[NCHANTDC];
  int NHITTOTAL[NCHANTDC] = {};
  std::string BANKS[NCHANTDC];
}

ANA_MODULE MTDCClockBitFreq_module =
{
  "MTDCClockFreq",       /* module name           */
  "John R Quirk",        /* author                */
  MTDCClockBitFreq,      /* event routine         */
  NULL,                  /* BOR routine           */
  MTDCClockBitFreq_eor,  /* EOR routine           */
  MTDCClockBitFreq_init, /* init routine          */
  NULL,                  /* exit routine          */
  NULL,                  /* parameter structure   */
  0,                     /* structure size        */
  NULL,                  /* initial parameters    */
};

INT MTDCClockBitFreq_init() {
  TDirectory* cwd = gDirectory;
  DIR = gDirectory->mkdir("TDCClockBitFreq");
  DIR->cd();
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    BANKS[ich] = bank;
    const string det = gSetup->GetDetectorName(bank);
    char name[64], title[128];
    sprintf(name, "hTDCClockFreq_%s_%s", bank, det.c_str());
    sprintf(title, "Clock Bit Frequency %s (%s);Bit", bank, det.c_str());
    vhTDCClockBitFreq[ich] = new TH1F(name, title,
				       BITSTDCCLK, 0.,
				       BITSTDCCLK);
  }
  cwd->cd();
  return SUCCESS;
}

INT MTDCClockBitFreq_eor(INT run_number) {
  for (int ich = 0; ich < NCHANTDC; ++ich)
    vhTDCClockBitFreq[ich]->Scale(1./NHITTOTAL[ich]);
  return SUCCESS;
}

INT MTDCClockBitFreq(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;

  for (int ich = 0; ich < NCHANTDC; ++ich) {
    if (!tdc_map.count(BANKS[ich])) continue;
    TH1* h = vhTDCClockBitFreq[ich];
    const vector<int64_t>& hits = tdc_map.at(BANKS[ich]);
    NHITTOTAL[ich] += hits.size();
    for (vector<int64_t>::const_iterator i = hits.begin(),
	   ei = hits.end(); i != ei; ++i) {
      const int t = *i % 2097152;
      for (int b = 0; b < BITSTDCCLK; ++b)
	if (t & 1 << b)
	  h->Fill(b);
    }
  }
  return SUCCESS;
}

/// @}
