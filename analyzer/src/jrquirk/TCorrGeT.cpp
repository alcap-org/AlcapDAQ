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
static void GetGeTDCCFTimes(vector<double>& ge1, vector<double>& ge2);
static void GetGeTDCLETimes(vector<double>& ge1, vector<double>& ge2);
static void GetGeWFDTimes(vector<double>& ge1, vector<double>& ge2);
static void GetGeHeights (vector<double>& ge1, vector<double>& ge2);

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  TH2 *hTCorrGe1TVsE;
  TH2 *hTCorrGe2TVsE;
  TH2 *hTCorrGeVsT;
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
  hTCorrGe1TVsE  = new TH2I("hTCorrGe1TVsE",
			    "Ge1 TCorr;WFD-TDC dt (ns);E (ADC)",
			    30000, -150000., 150000., 4096, 0., 16384.);
  hTCorrGe2TVsE  = new TH2I("hTCorrGe2TVsE",
			    "Ge2 TCorr;WFD-TDC dt (ns);E (ADC)",
			    30000, -150000., 150000., 4096, 0., 16384.);
  return SUCCESS;
}

INT TCorrGeT(EVENT_HEADER *pheader, void *pevent) {

  vector<double> tdc1_ts, tdc2_ts, wfd1_ts, wfd2_ts;
  vector<double> wfd1_es, wfd2_es;
  GetGeTDCCFTimes(tdc1_ts, tdc2_ts);
  //GetGeTDCLETimes(tdc1_ts, tdc2_ts);
  GetGeWFDTimes(wfd1_ts, wfd2_ts);
  GetGeHeights (wfd1_es, wfd2_es);
  //  printf("timeMuons %d \n",t0s.size());

  // Ge 1 loop
  for (int itdc = 0; itdc < tdc1_ts.size(); ++itdc) {
    int iwfd = lower_bound(wfd1_ts.begin(), wfd1_ts.end(),
			   tdc1_ts[itdc] - 150000.) - wfd1_ts.begin();
    for (; iwfd < wfd1_ts.size(); ++iwfd) {
      //std::cout <<"lower bound at position" << *it - *it0 << std::endl;
      const double dt = wfd1_ts[iwfd] - tdc1_ts[itdc];
      if (dt > 150000.)
	break;
      hTCorrGe1TVsE->Fill(dt, wfd1_es[iwfd]);
    }
  }
  
  // Ge 2 loop
  for (int itdc = 0; itdc < tdc2_ts.size(); ++itdc) {
    int iwfd = lower_bound(wfd2_ts.begin(), wfd2_ts.end(),
			   tdc2_ts[itdc] - 150000.) - wfd2_ts.begin();
    for (; iwfd < wfd2_ts.size(); ++iwfd) {
      const double dt = wfd2_ts[iwfd] - tdc2_ts[itdc];
      if (dt > 150000.)
	break;
      hTCorrGe2TVsE->Fill(dt, wfd2_es[iwfd]);
    }
  }
  return SUCCESS;
}

void GetGeTDCCFTimes(vector<double>& ge1, vector<double>& ge2) {
  const string GeT_name[2] = { "T404", "T405" };
  const double tick = 0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;

  vector<double>* ges[2] = { &ge1, &ge2 };
  for (int ige = 0; ige < 2; ++ige) {
    if (!hit_map.count(GeT_name[ige])) continue;
    vector<double>& ts = *ges[ige];
    const vector<int64_t>& hits = hit_map.at(GeT_name[ige]);
    ts.reserve(hits.size());
    for (int ihit = 0; ihit < hits.size(); ++ihit)
      ts.push_back(tick*hits[ihit]);
  }
}

void GetGeTDCLETimes(vector<double>& ge1, vector<double>& ge2) {
  const string GeT_name[2] = { "T410", "T411" };
  const double tick = 0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;

  vector<double>* ges[2] = { &ge1, &ge2 };
  for (int ige = 0; ige < 2; ++ige) {
    if (!hit_map.count(GeT_name[ige])) continue;
    vector<double>& ts = *ges[ige];
    const vector<int64_t>& hits = hit_map.at(GeT_name[ige]);
    ts.reserve(hits.size());
    for (int ihit = 0; ihit < hits.size(); ++ihit)
      ts.push_back(tick*hits[ihit]);
  }
}

void GetGeWFDTimes(vector<double>& ge1, vector<double>& ge2) {
  const string GeE_name[2] = { "D401", "D402" };
  const double tick = 62.; // ns
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  vector<double>* ges[2] = { &ge1, &ge2 };
  for (int ige = 0; ige < 2; ++ige) {
    if (!tpi_map.count(GeE_name[ige])) continue;
    vector<double>& ts = *ges[ige];
    const vector<TPulseIsland*>& tpis = tpi_map.at(GeE_name[ige]);
    ts.reserve(tpis.size());
    for (int itpi = 0; itpi < tpis.size(); ++itpi)
      ts.push_back(tick*tpis[itpi]->GetCFTime(0.2));
  }
}

void GetGeHeights(vector<double>& ge1, vector<double>& ge2) {
  const string GeE_name[2] =  { "D401", "D402" };
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  vector<double>* ges[2] = { &ge1, &ge2 };
  for (int ige = 0; ige < 2; ++ige) {
    if (!tpi_map.count(GeE_name[ige])) continue;
    vector<double>& es = *ges[ige];
    const vector<TPulseIsland*>& tpis = tpi_map.at(GeE_name[ige]);
    es.reserve(tpis.size());
    for (int itpi = 0; itpi < tpis.size(); ++itpi)
      es.push_back(tpis[itpi]->GetPulseHeight());
  }
}
