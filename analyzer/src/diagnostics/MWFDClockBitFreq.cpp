////////////////////////////////////////////////////////////////////////////////
/// \defgroup MWFDClockBitFreq
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
static INT MWFDClockBitFreq_init(void);
static INT MWFDClockBitFreq(EVENT_HEADER*, void*);
static INT MWFDClockBitFreq_eor(int);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  TDirectory* DIR;
  TH1* vvhWFDClockBitFreq[NCRATE][MAXNCHANWFD];
  int NPULSETOTAL[NCRATE][MAXNCHANWFD] = {};
  std::string BANKS[NCRATE][MAXNCHANWFD];
}

ANA_MODULE MWFDClockBitFreq_module =
{
  "MWFDClockFreq",       /* module name           */
  "John R Quirk",        /* author                */
  MWFDClockBitFreq,      /* event routine         */
  NULL,                  /* BOR routine           */
  MWFDClockBitFreq_eor,  /* EOR routine           */
  MWFDClockBitFreq_init, /* init routine          */
  NULL,                  /* exit routine          */
  NULL,                  /* parameter structure   */
  0,                     /* structure size        */
  NULL,                  /* initial parameters    */
};

INT MWFDClockBitFreq_init() {
  TDirectory* cwd = gDirectory;
  DIR = gDirectory->mkdir("WFDClockBitFreq");
  DIR->cd();
  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      BANKS[icrate][ich] = bank;
      const string det = gSetup->GetDetectorName(bank);
      char name[64], title[128];
      sprintf(name, "hWFDClockFreq_%s_%s", bank, det.c_str());
      sprintf(title, "Clock Bit Frequency %s (%s);Bit", bank, det.c_str());
      vvhWFDClockBitFreq[icrate][ich] = new TH1F(name, title,
						 BITSWFDCLK[icrate], 0.,
						 BITSWFDCLK[icrate]);
      vvhWFDClockBitFreq[icrate][ich]->Sumw2();
    }
  }
  cwd->cd();
  return SUCCESS;
}

INT MWFDClockBitFreq_eor(INT run_number) {
  for (int icrate = 0; icrate < NCRATE; ++icrate)
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich)
      vvhWFDClockBitFreq[icrate][ich]->Scale(1./NPULSETOTAL[icrate][ich]);
}

INT MWFDClockBitFreq(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if (!wfd_map.count(BANKS[icrate][ich])) continue;
      TH1* h = vvhWFDClockBitFreq[icrate][ich];
      const vector<TPulseIsland*>& tpis = wfd_map.at(BANKS[icrate][ich]);
      NPULSETOTAL[icrate][ich] += tpis.size();
      for (vector<TPulseIsland*>::const_iterator p = tpis.begin(),
	     ep = tpis.end(); p != ep; ++p) {
	const int t = (*p)->GetTimeStamp();
	for (int b = 0; b < BITSWFDCLK[icrate]; ++b)
	  if (t & 1 << b)
	    h->Fill(b);
      }
    }
  }
  return SUCCESS;
}

/// @}
