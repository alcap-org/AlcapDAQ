/********************************************************************\

  Name:         MTCorrMuSc
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
static INT TCorrMuSc_init(void);
static INT TCorrMuSc(EVENT_HEADER*, void*);
static vector<double> GetTimes(const string& channel_name);

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  const int ndet = 9;
  const string dets[ndet] = { "Ge1CF",  "Ge2CF",  "Ge1LE",  "Ge2LE",
			      "ESc1Lo", "ESc1Hi", "ESc2Lo", "ESc2Hi",
			      "MuScA" };
  const string banks[ndet] = { "T404", "T405", "T410", "T411",
			       "T412", "T413", "T414", "T415",
			       "T409" };
  TH1* hTCorr[ndet];
}

ANA_MODULE TCorrMuSc_module =
{
  "TCorrMuSc",    /* module name           */
  "John R Quirk", /* author                */
  TCorrMuSc,      /* event routine         */
  NULL,           /* BOR routine           */
  NULL,           /* EOR routine           */
  TCorrMuSc_init, /* init routine          */
  NULL,           /* exit routine          */
  NULL,           /* parameter structure   */
  0,              /* structure size        */
  NULL,           /* initial parameters    */
};

INT TCorrMuSc_init() {
  for (int i = 0; i < ndet; ++i) {
    string name("hTCorrMuSc");
    name += dets[i];
    string title(" time correlations with MuSc (All TDC);T-T_{Mu} (ns)");
    title = dets[i] + title;
    hTCorr[i] = new TH1I(name.c_str(), title.c_str(), 2000, -1000., 1000.);
  }  
  return SUCCESS;
}

INT TCorrMuSc(EVENT_HEADER *pheader, void *pevent) {
  const string musc_name("T402");
  const vector<double> mus = GetTimes(musc_name);
  for (int idet = 0; idet < ndet; ++idet) {
    const vector<double> hits = GetTimes(banks[idet]);
    for (int ihit = 0; ihit < hits.size(); ++ihit) {
      int imu = lower_bound(mus.begin(), mus.end(),
			    hits[ihit] - 1000.) - mus.begin();
      for (; imu < mus.size(); ++imu) {
	const double dt = hits[ihit] - mus[imu];
	if (dt < -1000.) break;
	hTCorr[idet]->Fill(dt);
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
