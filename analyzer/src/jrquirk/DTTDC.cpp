/********************************************************************\

  Name:         MDTTDC
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
using std::sort;

/* ROOT includes */
#include "TH1I.h"
#include "TH2I.h"

/* MIDAS includes */
#include "midas.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

/*-- Module declaration --------------------------------------------*/
static INT DTTDC_init(void);
static INT DTTDC(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  TH2 *hdT;
}

ANA_MODULE DTTDC_module =
{
  "DTTDC",        /* module name           */
  "John R Quirk", /* author                */
  DTTDC,          /* event routine         */
  NULL,           /* BOR routine           */
  NULL,           /* EOR routine           */
  DTTDC_init,     /* init routine          */
  NULL,           /* exit routine          */
  NULL,           /* parameter structure   */
  0,              /* structure size        */
  NULL,           /* initial parameters    */
};

INT DTTDC_init() {
  hdT = new TH2I("hDTTDC", "dT from previous hit for all TDC channels;dT (ns); T",
		 52000, 0, 52000, 1000, 0., 100.e6);
  return SUCCESS;
}

INT DTTDC(EVENT_HEADER *pheader, void *pevent) {
  const double tick =  0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;
  vector<double> ts;
  vector<int64_t> ts_raw;
  for (map<string, vector<int64_t> >::const_iterator i = hit_map.begin();
       i != hit_map.end(); ++i) {
    const vector<int64_t>& hits = i->second;
    ts_raw.insert(ts_raw.end(), hits.begin(), hits.end());
    ts.reserve(ts.size() + hits.size());
    for (int j = 0; j < hits.size(); ++j)
      ts.push_back(tick*hits[j]);
  }
  sort(ts.begin(), ts.end());
  sort(ts_raw.begin(), ts_raw.end());

  for (int i = 1; i < ts.size(); ++i) {
    hdT->Fill(ts[i]-ts[i-1], ts[i]);
    if (ts[i]-ts[i-1] > 50.e3)
      printf("%ld %ld %ld\n", ts_raw[i-1] % 2097152, ts_raw[i] % 2097152, (ts_raw[i-1] % 2097152 - ts_raw[i] % 2097152) + 2097152);
  }

  return SUCCESS;
}
