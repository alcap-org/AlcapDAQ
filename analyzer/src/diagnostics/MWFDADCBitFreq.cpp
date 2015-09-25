////////////////////////////////////////////////////////////////////////////////
/// \defgroup MWFDADCBitFreq
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
static INT MWFDADCBitFreq_init(void);
static INT MWFDADCBitFreq(EVENT_HEADER*, void*);
static INT MWFDADCBitFreq_eor(int);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  TDirectory* DIR;
  TH1* vvhWFDADCBitFreq[NCRATE][MAXNCHANWFD];
  int NSAMPTOTAL[NCRATE][MAXNCHANWFD] = {};
  std::string BANKS[NCRATE][MAXNCHANWFD];
  int NBITS[NCRATE];
  int MAXVAL[NCRATE];
}

ANA_MODULE MWFDADCBitFreq_module =
{
  "MWFDADCFreq",       /* module name           */
  "John R Quirk",      /* author                */
  MWFDADCBitFreq,      /* event routine         */
  NULL,                /* BOR routine           */
  MWFDADCBitFreq_eor,  /* EOR routine           */
  MWFDADCBitFreq_init, /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};

INT MWFDADCBitFreq_init() {
  TDirectory* cwd = gDirectory;
  DIR = gDirectory->mkdir("WFDADCBitFreq");
  DIR->cd();

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    const int nbits = gSetup->GetNBits(icrate);
    NBITS[icrate] = nbits;
    MAXVAL[icrate] = 0;
    for (int ibit = 0; ibit < nbits; ++ibit)
      MAXVAL[icrate] |= 1<<ibit;
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      BANKS[icrate][ich] = bank;
      const string det = gSetup->GetDetectorName(bank);
      char name[64], title[128];
      sprintf(name, "hWFDADCFreq_%s_%s", bank, det.c_str());
      sprintf(title, "Bit Frequency %s (%s);Bit", bank, det.c_str());
      vvhWFDADCBitFreq[icrate][ich] = new TH1F(name, title, nbits, 0., nbits);
      vvhWFDADCBitFreq[icrate][ich]->Sumw2();
    }
  }
  cwd->cd();
  return SUCCESS;
}

INT MWFDADCBitFreq_eor(INT run_number) {
  for (int icrate = 0; icrate < NCRATE; ++icrate)
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich)
      vvhWFDADCBitFreq[icrate][ich]->Scale(1./NSAMPTOTAL[icrate][ich]);
}

INT MWFDADCBitFreq(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;

  for (int icrate = 0; icrate < NCRATE; ++icrate) {
    for (int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if (!wfd_map.count(BANKS[icrate][ich])) continue;
      TH1* h = vvhWFDADCBitFreq[icrate][ich];
      const vector<TPulseIsland*>& tpis = wfd_map.at(BANKS[icrate][ich]);
      for (vector<TPulseIsland*>::const_iterator p = tpis.begin(),
	     ep = tpis.end(); p != ep; ++p) {
	const vector<int>& samps = (*p)->GetSamples();
	NSAMPTOTAL[icrate][ich] += samps.size();
	for (vector<int>::const_iterator s = samps.begin(), es = samps.end();
	     s != es; ++s) {
	  if (*s != 0 && *s != MAXVAL[icrate]) {
	    for (int b = 0; b < NBITS[icrate]; ++b) {
	      if (*s & 1 << b)
		h->Fill(b);
	    }
	  } else {
	    --NSAMPTOTAL[icrate][ich];
	  }
	}
      }
    }
  }
  return SUCCESS;
}

/// @}
