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
static void GetTDCTimes(const string& channel_name, vector<double>& times);
static void GetGeTDCCFTimes(vector<double>& ge1, vector<double>& ge2);
static void GetGeTDCLETimes(vector<double>& ge1, vector<double>& ge2);
static void GetGeWFDTimes  (vector<double>& ge1, vector<double>& ge2);
static void GetGeHeights   (vector<double>& ge1, vector<double>& ge2);

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  TH2 *hTCorrGe1TCFVsE;
  TH2 *hTCorrGe2TCFVsE;
  TH1 *hTCorrGe1TLEVsE;
  TH1 *hTCorrGe2TLEVsE;
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
  hTCorrGe1TCFVsE = new TH2I("hTCorrGe1TCFVsE",
			     "Ge1 WFD-TDC(CF) Timing;T_{WFD}-T_{TDC} (ns);E (ADC)",
			     10000, 10.e3, 20.e3, 4096, 0., 16384.);
  hTCorrGe2TCFVsE = new TH2I("hTCorrGe2TCFVsE",
			     "Ge2 WFD-TDC(CF) Timing;T_{WFD}-T_{TDC} (ns);E (ADC)",
			     10000, 10.e3, 20.e3, 4096, 0., 16384.);
  hTCorrGe1TLEVsE = new TH2I("hTCorrGe1TLEVsE",
			     "Ge1 WFD-TDC(LE) Timing;T_{WFD}-T_{TDC} (ns);E (ADC)",
			     10000, 10.e3, 20.e3, 4096, 0., 16384.);
  hTCorrGe2TLEVsE = new TH2I("hTCorrGe2TLEVsE",
			     "Ge2 WFD-TDC(LE) Timing;T_{WFD}-T_{TDC} (ns);E (ADC)",
			     10000, 10.e3, 20.e3, 4096, 0., 16384.);

  return SUCCESS;
}

INT TCorrGeT(EVENT_HEADER *pheader, void *pevent) {

  vector<double> tdc1_ts, tdc2_ts, wfd1_ts, wfd2_ts;
  vector<double> wfd1_es, wfd2_es;
  GetGeWFDTimes(wfd1_ts, wfd2_ts);
  GetGeHeights (wfd1_es, wfd2_es);

  GetGeTDCCFTimes(tdc1_ts, tdc2_ts);
  // Ge 1 WFD-TDC timing correlation
  for (int itdc = 0; itdc < tdc1_ts.size(); ++itdc) {
    int iwfd = lower_bound(wfd1_ts.begin(), wfd1_ts.end(),
			   tdc1_ts[itdc] - 150000.) - wfd1_ts.begin();
    for (; iwfd < wfd1_ts.size(); ++iwfd) {
      //std::cout <<"lower bound at position" << *it - *it0 << std::endl;
      const double dt = wfd1_ts[iwfd] - tdc1_ts[itdc];
      if (dt > 150000.) break;
      hTCorrGe1TCFVsE->Fill(dt, wfd1_es[iwfd]);
    }
  }
  
  // Ge 2 WFD-TDC timing correlations
  for (int itdc = 0; itdc < tdc2_ts.size(); ++itdc) {
    int iwfd = lower_bound(wfd2_ts.begin(), wfd2_ts.end(),
			   tdc2_ts[itdc] - 150000.) - wfd2_ts.begin();
    for (; iwfd < wfd2_ts.size(); ++iwfd) {
      const double dt = wfd2_ts[iwfd] - tdc2_ts[itdc];
      if (dt > 150000.) break;
      hTCorrGe2TCFVsE->Fill(dt, wfd2_es[iwfd]);
    }
  }


  GetGeTDCLETimes(tdc1_ts, tdc2_ts);
  // Ge 1 WFD-TDC timing correlation
  for (int itdc = 0; itdc < tdc1_ts.size(); ++itdc) {
    int iwfd = lower_bound(wfd1_ts.begin(), wfd1_ts.end(),
			   tdc1_ts[itdc] - 150000.) - wfd1_ts.begin();
    for (; iwfd < wfd1_ts.size(); ++iwfd) {
      //std::cout <<"lower bound at position" << *it - *it0 << std::endl;
      const double dt = wfd1_ts[iwfd] - tdc1_ts[itdc];
      if (dt > 150000.) break;
      hTCorrGe1TLEVsE->Fill(dt, wfd1_es[iwfd]);
    }
  }
  
  // Ge 2 WFD-TDC timing correlations
  for (int itdc = 0; itdc < tdc2_ts.size(); ++itdc) {
    int iwfd = lower_bound(wfd2_ts.begin(), wfd2_ts.end(),
			   tdc2_ts[itdc] - 150000.) - wfd2_ts.begin();
    for (; iwfd < wfd2_ts.size(); ++iwfd) {
      const double dt = wfd2_ts[iwfd] - tdc2_ts[itdc];
      if (dt > 150000.) break;
      hTCorrGe2TLEVsE->Fill(dt, wfd2_es[iwfd]);
    }
  } 
  return SUCCESS;
}

void GetGeTDCCFTimes(vector<double>& ge1, vector<double>& ge2) {
  const string GeT_name[2] = { "T404", "T405" };
  vector<double>* ges[2] = { &ge1, &ge2 };
  for (int ige = 0; ige < 2; ++ige)
    GetTDCTimes(GeT_name[ige], *ges[ige]);
}

void GetGeTDCLETimes(vector<double>& ge1, vector<double>& ge2) {
  const string GeT_name[2] = { "T410", "T411" };
  vector<double>* ges[2] = { &ge1, &ge2 };
  for (int ige = 0; ige < 2; ++ige)
    GetTDCTimes(GeT_name[ige], *ges[ige]);
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

void GetTDCTimes(const string& bank, vector<double>& ts) {
  const double tick = 0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;

  ts.clear();
  if (!hit_map.count(bank)) return;
  const vector<int64_t>& hits = hit_map.at(bank);
  ts.reserve(hits.size());
  for (int ihit = 0; ihit < hits.size(); ++ihit)
    ts.push_back(tick*hits[ihit]);
}
