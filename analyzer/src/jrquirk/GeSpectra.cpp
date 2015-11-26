/********************************************************************\

  Name:         MGeSpectra
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
#include "TGraph.h"

/* MIDAS includes */
#include "midas.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

/*-- Module declaration --------------------------------------------*/
static INT GeSpectra_init(void);
static INT GeSpectra(EVENT_HEADER*, void*);
static void GetGeTDCTimes(vector<double>& ge1, vector<double>& ge2);
static void GetGeWFDTimes(vector<double>& ge1, vector<double>& ge2);
static void GetGeEnergies(vector<double>& ge1, vector<double>& ge2);

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  const Double_t ADC1[2] = { 9.35545e+03, 1.06251e+04 };
  const Double_t ADC2[2] = { 1.92472e+03, 2.18558e+03 };
  const Double_t E[2] = { 1173.237, 1332.501};
  const TGraph ADC2E1(2, ADC1, E);
  const TGraph ADC2E2(2, ADC2, E);
  const TGraph* ADC2E[2] = { &ADC2E1, &ADC2E2 };
  const Double_t TDCMatchWindow[2][2] = { { 15500., 17000. },
					  { 16400., 17000. } };

  TH2 *hGe1TVsE;
  TH2 *hGe2TVsE;
  TH1 *hGe1TUnmatched;
  TH1 *hGe2TUnmatched;
}

ANA_MODULE GeSpectra_module =
{
  "GeSpectra",    /* module name           */
  "John R Quirk", /* author                */
  GeSpectra,      /* event routine         */
  NULL,           /* BOR routine           */
  NULL,           /* EOR routine           */
  GeSpectra_init, /* init routine          */
  NULL,           /* exit routine          */
  NULL,           /* parameter structure   */
  0,              /* structure size        */
  NULL,           /* initial parameters    */
};

INT GeSpectra_init() {
  hGe1TVsE  = new TH2I("h2Ge1Spectra",
		       "Ge1 Spectra;Time in block muon (ns);E (keV)",
		       1000, 0., 100.e6,
		       4096, ADC2E[0]->Eval(0), ADC2E[0]->Eval(16384));
  hGe2TVsE  = new TH2I("h2Ge2Spectra",
		       "Ge2 Spectra;Time in block muon (ns);E (keV)",
		       1000, 0., 100.e6,
		       4096, ADC2E[1]->Eval(0), ADC2E[1]->Eval(16384));
  hGe1TUnmatched = new TH1I("h1Ge1TDCUnmatched",
			    "Ge1 Unmatched TDC hits;Block time (ns)",
			    1000, 0., 100.e6);
  hGe2TUnmatched = new TH1I("h1Ge2TDCUnmatched",
			    "Ge2 Unmatched TDC hits;Block time (ns)",
			    1000, 0., 100.e6);

  return SUCCESS;
}

INT GeSpectra(EVENT_HEADER *pheader, void *pevent) {

  vector<double> tdc1_ts, tdc2_ts, wfd1_ts, wfd2_ts;
  vector<double> wfd1_es, wfd2_es;
  GetGeTDCTimes(tdc1_ts, tdc2_ts);
  GetGeWFDTimes(wfd1_ts, wfd2_ts);
  GetGeEnergies(wfd1_es, wfd2_es);

  // Ge 1 loop
  for (int itdc = 0; itdc < tdc1_ts.size(); ++itdc) {
     vector<double>::const_iterator ilower =
       lower_bound(wfd1_ts.begin(), wfd1_ts.end(),
		   tdc1_ts[itdc] + TDCMatchWindow[0][0]);
     vector<double>::const_iterator iupper =
       lower_bound(wfd1_ts.begin(), wfd1_ts.end(),
		   tdc1_ts[itdc] + TDCMatchWindow[0][1]);
    if (iupper - ilower != 1) {
      hGe1TUnmatched->Fill(tdc1_ts[itdc]);
      continue;
    }
    int iwfd = ilower - wfd1_ts.begin();
    hGe1TVsE->Fill(tdc1_ts[itdc], wfd1_es[iwfd]);
  }
  // Ge 2 loop
  for (int itdc = 0; itdc < tdc2_ts.size(); ++itdc) {
    vector<double>::const_iterator ilower =
      lower_bound(wfd2_ts.begin(), wfd2_ts.end(),
		  tdc2_ts[itdc] + TDCMatchWindow[1][0]);
    vector<double>::const_iterator iupper =
      lower_bound(wfd2_ts.begin(), wfd2_ts.end(),
		  tdc2_ts[itdc] + TDCMatchWindow[1][1]);
    if (iupper - ilower != 1) {
      hGe2TUnmatched->Fill(tdc2_ts[itdc]);
      continue;
    }
    int iwfd = ilower - wfd2_ts.begin();
    hGe1TVsE->Fill(tdc2_ts[itdc], wfd2_es[iwfd]);
  }

  return SUCCESS;
}

void GetGeTDCTimes(vector<double>& ge1, vector<double>& ge2) {
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

void GetGeEnergies(vector<double>& ge1, vector<double>& ge2) {
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
      es.push_back(ADC2E[ige]->Eval(tpis[itpi]->GetPulseHeight()));
  }
}
