/********************************************************************\

  Name:         MTCorrSiTGe
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
static INT TCorrSiTGe_init(void);
static INT TCorrSiTGe(EVENT_HEADER*, void*);
static vector<double> GetSiTTimes();
static vector<double> GetGeTTimes();
static vector<double> GetGeETimes();

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  TH1 *hTCorrSiTGeT;
  TH1 *hTCorrSiTGeE;
}

ANA_MODULE TCorrSiTGe_module =
{
  "TCorrSiTGe",    /* module name           */
  "John R Quirk",  /* author                */
  TCorrSiTGe,      /* event routine         */
  NULL,            /* BOR routine           */
  NULL,            /* EOR routine           */
  TCorrSiTGe_init, /* init routine          */
  NULL,            /* exit routine          */
  NULL,            /* parameter structure   */
  0,               /* structure size        */
  NULL,            /* initial parameters    */
};

INT TCorrSiTGe_init() {
  hTCorrSiTGeT = new TH1I("hTCorrSiTGeT", "GeT TCorr with SiT;GeT - SiT (ns)",
		       1000, -500., 500.);
  hTCorrSiTGeE = new TH1I("hTCorrSiTGeLo", "GeLo TCorr with SiT;GeLo - SiT(ns)",
		       1000, 15000., 16000.);
  return SUCCESS;
}

INT TCorrSiTGe(EVENT_HEADER *pheader, void *pevent) {

  const vector<double> t0s    = GetSiTTimes();
  const vector<double> tgees  = GetGeETimes();
  const vector<double> tgets  = GetGeTTimes();

  const vector<double>::const_iterator et0 = t0s.end();
  for (vector<double>::const_iterator it0 = t0s.begin(); it0 < et0; ++it0) {
    vector<double>::const_iterator bt = tgees.begin(), et = tgees.end();
    for (vector<double>::const_iterator it = lower_bound(bt, et, *it0 + 15000.);
	 it < et; ++it) {
      const double dt = *it - *it0;
      if (dt > 16000.)
	break;
      hTCorrSiTGeE->Fill(dt);
    }

    bt = tgets.begin(), et = tgets.end();
    for (vector<double>::const_iterator it = lower_bound(bt, et, *it0 - 500.);
	 it < et; ++it) {
      const double dt = *it - *it0;
      if (dt > 500.)
	break;
      hTCorrSiTGeT->Fill(dt);
    }
  }
  return SUCCESS;
}

vector<double> GetSiTTimes() {
  const string sit_name[4] = { "D702", "D703", "D704", "D705" };
  const double tick = 2.; // ns
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  vector<double> ts;
  for (int i = 0; i < 4; ++i) {
    if (tpi_map.count(sit_name[i])) {
      const vector<TPulseIsland*>& tpis = tpi_map.at(sit_name[i]);
      ts.reserve(ts.size()+tpis.size());
      const vector<TPulseIsland*>::const_iterator e = tpis.end();
      for (vector<TPulseIsland*>::const_iterator j = tpis.begin(); j < e; ++j)
	ts.push_back(tick*(*j)->GetCFTime(0.2));
    }
  }
  sort(ts.begin(), ts.end());
  return ts;
}

vector<double> GetGeTTimes() {
  const string geT_name("T404");
  const double tick = 0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;

  vector<double> ts;
  if (!hit_map.count(geT_name))
    return ts;
  const vector<int64_t>& hits = hit_map.at(geT_name);
  ts.reserve(hits.size());
  for (vector<int64_t>::const_iterator i = hits.begin(), e = hits.end();
       i < e; ++i)
    ts.push_back(tick*(*i));
  return ts;
}

vector<double> GetGeETimes() {
  const string geE_name("D401");
  const double tick = 62.; // ns
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  vector<double> ts;
  if (!tpi_map.count(geE_name))
    return ts;
  const vector<TPulseIsland*>& tpis = tpi_map.at(geE_name);
  ts.reserve(tpis.size());
  for (vector<TPulseIsland*>::const_iterator i = tpis.begin(), e = tpis.end();
       i < e; ++i)
    ts.push_back(tick*(*i)->GetCFTime(0.2));
  return ts;
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
