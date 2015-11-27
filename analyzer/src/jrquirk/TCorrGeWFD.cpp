/********************************************************************\

  Name:         MTCorrGeWFD
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
static INT TCorrGeWFD_init(void);
static INT TCorrGeWFD(EVENT_HEADER*, void*);
static void GetGeTimes(vector<double>& hi, vector<double>& lo);

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  TH1 *hdT;
}

ANA_MODULE TCorrGeWFD_module =
{
  "TCorrGeWFD",    /* module name           */
  "John R Quirk",  /* author                */
  TCorrGeWFD,      /* event routine         */
  NULL,            /* BOR routine           */
  NULL,            /* EOR routine           */
  TCorrGeWFD_init, /* init routine          */
  NULL,            /* exit routine          */
  NULL,            /* parameter structure   */
  0,               /* structure size        */
  NULL,            /* initial parameters    */
};

INT TCorrGeWFD_init() {
  hdT = new TH1I("hTCorrGeHiLo", "GeHi Gain TCorr GeLoGain;Hi-Lo (ns)",
		 1000, -500, 500);
  return SUCCESS;
}

INT TCorrGeWFD(EVENT_HEADER *pheader, void *pevent) {

  vector<double> t0s, ts;
  GetGeTimes(t0s, ts);

  const vector<double>::const_iterator et0 = t0s.end();
  for (vector<double>::const_iterator it0 = t0s.begin(); it0 < et0; ++it0) {
    const vector<double>::const_iterator bt = ts.begin(), et = ts.end();
    for (vector<double>::const_iterator it = lower_bound(bt, et, *it0 - 500.);
	 it < et; ++it) {
      const double dt = *it - *it0;
      if (dt > 500.)
	break;
      hdT->Fill(dt);
    }
  }
  return SUCCESS;
}

void GetGeTimes(vector<double>& hi, vector<double>& lo) {
  const string name[2] = { "D401", "D402" };
  vector<double>* ts[2] = { &hi, &lo };
  const double tick =62 ; // ns
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  for (int i = 0; i < 2; ++i) {
    if (!tpi_map.count(name[i]))
      continue;
    const vector<TPulseIsland*>& tpis = tpi_map.at(name[i]);
    ts[i]->reserve(tpis.size());
    for (vector<TPulseIsland*>::const_iterator i = tpis.begin(), e = tpis.end();
	 i < e; ++i)
      ts[i]->push_back(tick*(*i)->GetCFTime(0.2));
  }
}

vector<double> GetGeEHeights() {
  const string geE_name("D401");
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  vector<double> es;
  if (!tpi_map.count(geE_name))
    return es;
  const vector<TPulseIsland*>& tpis = tpi_map.at(geE_name);
  es.reserve(tpis.size());
  for (vector<TPulseIsland*>::const_iterator i = tpis.begin(), e = tpis.end();
       i < e; ++i)
    es.push_back((*i)->GetPulseHeight());
  return es;
}
