/********************************************************************\

  Name:         MTSelfCorrRollover
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
static INT TSelfCorrRollover_init(void);
static INT TSelfCorrRollover(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  TH2 *hdT;
}

ANA_MODULE TSelfCorrRollover_module =
{
  "TSelfCorrRollover",    /* module name           */
  "John R Quirk",         /* author                */
  TSelfCorrRollover,      /* event routine         */
  NULL,                   /* BOR routine           */
  NULL,                   /* EOR routine           */
  TSelfCorrRollover_init, /* init routine          */
  NULL,                   /* exit routine          */
  NULL,                   /* parameter structure   */
  0,                      /* structure size        */
  NULL,                   /* initial parameters    */
};

INT TSelfCorrRollover_init() {
  hdT = new TH2I("hTSelfCorrRollover", "Rollover dT from previous hit",
		 6000, 0., 170.e3, 1000, 0., 110.e6);
  return SUCCESS;
}

INT TSelfCorrRollover(EVENT_HEADER *pheader, void *pevent) {
  const double tick =  0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;
  const string bank("T400");
  if (!hit_map.count(bank))
    return SUCCESS;

  const vector<int64_t>& hits = hit_map.at(bank);
  
  for (int i = 1; i < hits.size(); ++i)
    hdT->Fill(tick*(hits[i]-hits[i-1]), tick*hits[i]);
  return SUCCESS;
}
