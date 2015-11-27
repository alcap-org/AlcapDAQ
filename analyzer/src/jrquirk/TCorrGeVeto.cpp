/********************************************************************\

  Name:         MTCorrGeVeto
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
static INT TCorrGeVeto_init(void);
static INT TCorrGeVeto(EVENT_HEADER*, void*);
static vector<double> GetTimes(const string& channel_name);

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  TH1* hTCorrGeESc[2];
}

ANA_MODULE TCorrGeVeto_module =
{
  "TCorrGeVeto",    /* module name           */
  "John R Quirk",   /* author                */
  TCorrGeVeto,      /* event routine         */
  NULL,             /* BOR routine           */
  NULL,             /* EOR routine           */
  TCorrGeVeto_init, /* init routine          */
  NULL,             /* exit routine          */
  NULL,             /* parameter structure   */
  0,                /* structure size        */
  NULL,             /* initial parameters    */
};

INT TCorrGeVeto_init() {
  hTCorrGeESc[0] = new TH1I("hTCorrGe1ESc1",
			    "Ge1 TCorr with Veto (TDC);T_{Ge}-T_{Veto} (ns)",
			    2000, -1000., 1000.);
  hTCorrGeESc[1] = new TH1I("hTCorrGe2ESc2",
			    "Ge2 TCorr with Veto (TDC);T_{Ge}-T_{Veto} (ns)",
			    2000, -1000., 1000.);
  return SUCCESS;
}

INT TCorrGeVeto(EVENT_HEADER *pheader, void *pevent) {
  const string ge_banks[2]   = { "T404", "T405" };
  const string veto_banks[2] = { "T412", "T414" };

  const vector<double> ge1       = GetTimes(ge_banks[0]);
  const vector<double> ge2       = GetTimes(ge_banks[1]);
  const vector<double> veto1     = GetTimes(veto_banks[0]);
  const vector<double> veto2     = GetTimes(veto_banks[1]);
  const vector<double>* ges[2]   = { &ge1, &ge2 };
  const vector<double>* vetos[2] = { &veto1, &veto2 };

  for (int ige = 0; ige < 2; ++ige) {
    for (int ihit = 0; ihit < ges[ige]->size(); ++ihit) {
      int jhit = lower_bound(vetos[ige]->begin(), vetos[ige]->end(),
			     ges[ige]->at(ihit) - 1000.) - vetos[ige]->begin();
      for (; jhit < vetos[ige]->size(); ++jhit) {
	const double dt = ges[ige]->at(ihit) - vetos[ige]->at(jhit);
	if (dt < -1000.) break;
	hTCorrGeESc[ige]->Fill(dt);
      }
    }
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
