/********************************************************************\

  Name:         MTCorrGeT
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
static INT TCorrGeT_init(void);
static INT TCorrGeT(EVENT_HEADER*, void*);
static vector<double> GetGeTTimes();
static vector<double> GetGeETimes();
static vector<double> GetGeEHeights();

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  TH2 *hTCorrGe;
  TH1 *hNTCorrGe;
}

ANA_MODULE TCorrGeT_module =
{
  "TCorrGeT",     /* module name           */
  "John R Quirk", /* author                */
  TCorrGeT,       /* event routine         */
  NULL,           /* BOR routine           */
  NULL,           /* EOR routine           */
  TCorrGeT_init,  /* init routine          */
  NULL,           /* exit routine          */
  NULL,           /* parameter structure   */
  0,              /* structure size        */
  NULL,           /* initial parameters    */
};

INT TCorrGeT_init() {
  hTCorrGe  = new TH2I("hTCorrGeTGeLoE", "GeLo TCorr with GeT;dt (ns)",
		       10000, -50000, 50000, 4096, 0., 16384.);
  hNTCorrGe = new TH1I("hNTCorrGeTGeLoE", "N Matches GeLo TCorr with GeT",
		       20, 0, 20);
  return SUCCESS;
}

INT TCorrGeT(EVENT_HEADER *pheader, void *pevent) {

  const vector<double> t0s = GetGeTTimes();
  //  printf("timeMuons %d \n",t0s.size());
  const vector<double> ts  = GetGeETimes();
  const vector<double> es  = GetGeEHeights();

  const vector<double>::const_iterator et0 = t0s.end();
  for (vector<double>::const_iterator it0 = t0s.begin(); it0 < et0; ++it0) {
    const vector<double>::const_iterator bt = ts.begin(), et = ts.end();
    int n = 0;
    for (vector<double>::const_iterator it = lower_bound(bt, et, *it0);
	 it < et; ++it) {
      std::cout <<"lower bound at position" << *it - *it0 << std::endl;
      const double dt = *it - *it0;
      //      printf("time Difference mu-Ge %d \n",ts.at(100));
      if (dt > 36000.)
	break;
      ++n;
       
      hTCorrGe->Fill(dt, es[it-bt]);
    }
    hNTCorrGe->Fill(n);
  }
  return SUCCESS;
}

vector<double> GetGeTTimes() {
  const string GeT_name("T404");
  const double tick = 0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;

  vector<double> ts;
  if (!hit_map.count(GeT_name))
    return ts;
  const vector<int64_t>& hits = hit_map.at(GeT_name);
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
    ts.push_back(tick*(*i)->GetCFTime(0.2)); // ER 23.11 it was  GetCFTime(0.2)
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
