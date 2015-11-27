/********************************************************************\

  Name:         MTCorrAutoTDC
  Created by:   John R Quirk

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using std::string;
using std::vector;
using std::map;
using std::lower_bound;

/* ROOT includes */
#include "TH1I.h"
#include "TH2I.h"

/* MIDAS includes */
#include "midas.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

/*-- Module declaration --------------------------------------------*/
static INT TCorrAutoTDC_init(void);
static INT TCorrAutoTDC(EVENT_HEADER*, void*);
static vector<double> GetTimes(const string& channel_name);

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  const int ndet = 10;
  const string dets[ndet] = { "Ge1CF",  "Ge2CF",  "Ge1LE",  "Ge2LE",
			      "ESc1Lo", "ESc1Hi", "ESc2Lo", "ESc2Hi",
			      "MuScA", "MuSc" };
  const string banks[ndet] = { "T404", "T405", "T410", "T411",
			       "T412", "T413", "T414", "T415",
			       "T409", "T402" };
  TH1* hTCorr[ndet];
}

ANA_MODULE TCorrAutoTDC_module =
{
  "TCorrAutoTDC",    /* module name           */
  "John R Quirk", /* author                */
  TCorrAutoTDC,      /* event routine         */
  NULL,           /* BOR routine           */
  NULL,           /* EOR routine           */
  TCorrAutoTDC_init, /* init routine          */
  NULL,           /* exit routine          */
  NULL,           /* parameter structure   */
  0,              /* structure size        */
  NULL,           /* initial parameters    */
};

INT TCorrAutoTDC_init() {
  for (int i = 0; i < ndet; ++i) {
    string name("hTCorrAutoTDC");
    name += dets[i];
    string title(" time since last hit;T_{i}-T_{i-1} (ns)");
    title = dets[i] + title;
    hTCorr[i] = new TH1I(name.c_str(), title.c_str(), 10000, 0., 10000.);
  }  
  return SUCCESS;
}

INT TCorrAutoTDC(EVENT_HEADER *pheader, void *pevent) {
  for (int idet = 0; idet < ndet; ++idet) {
    const vector<double> hits = GetTimes(banks[idet]);
    for (int ihit = 1; ihit < hits.size(); ++ihit)
      hTCorr[idet]->Fill(hits[ihit] - hits[ihit-1]);
  }
  return SUCCESS;
}

vector<double> GetTimes(const string& bank) {
  const double tick = 0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;

  vector<double> ts;
  if (!hit_map.count(bank)) return ts;
  const vector<int64_t>& hits = hit_map.at(bank);
  ts.reserve(hits.size());
  for (int ihit = 0; ihit < hits.size(); ++ihit)
    ts.push_back(tick*hits[ihit]);
  return ts;
}
