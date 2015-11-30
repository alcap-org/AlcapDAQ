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
static void GetTDCTimes(const string& channel_name, vector<double>& times);
static void GetMuScTimes (vector<double>& musc);
static void GetGeTDCTimes(vector<double>& ge1, vector<double>& ge2);
static void GetGeWFDTimes(vector<double>& ge1, vector<double>& ge2);
static void GetGeEnergies(vector<double>& ge1,   vector<double>& ge2);
static void GetVetoTimes (vector<double>& veto1, vector<double>& veto2);
static bool IsCPProtected(const vector<double>& vs, double t, int ige);
static void GetFilters(vector<bool>& filter1, vector<bool>& filter2);

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
 //  const Double_t ADC1[2] = { 6.49508e+03, 7.29774e+03 };
 // const Double_t ADC2[2] = { 2.91441e+03, 3.17513e+03 }; // Re measurement
  const Double_t ADC1[2] = {7.49812e+03, 8.45277e+03};
  const Double_t ADC2[2] = {7.61058e+03 , 8.53722e+03 }; //Zn measurement
  const Double_t E[2] = { 1173.237, 1332.501};
  const TGraph ADC2E1(2, ADC1, E);
  const TGraph ADC2E2(2, ADC2, E);
  const TGraph* ADC2E[2] = { &ADC2E1, &ADC2E2 };
  const Double_t TDCMatchWindow[2][2] = { { 15500., 17000. },
					  { 16400., 17000. } };
  const Double_t MuonLookTime[2] = { -10000., -10000. };

  TH2 *hGe1BlockTVsE;
  TH2 *hGe2BlockTVsE;
  TH1 *hGe1TUnmatched;
  TH1 *hGe2TUnmatched;
  TH2 *hGe1MuTVsE;
  TH2 *hGe2MuTVsE;
  TH2 *hGe1MuTVsE_CPP;
  TH2 *hGe2MuTVsE_CPP;
  TH2 *hGeEVsE;
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
  hGe1BlockTVsE  = new TH2D("h2Ge1SpectraBlock",
			    "Ge1 Spectra;Time in block muon (ns);E (keV)",
			    1000, 0., 100.e6,
			    4096, ADC2E[0]->Eval(0), ADC2E[0]->Eval(16384));
  hGe2BlockTVsE  = new TH2D("h2Ge2SpectraBlock",
			    "Ge2 Spectra;Time in block muon (ns);E (keV)",
			    1000, 0., 100.e6,
			    4096, ADC2E[1]->Eval(0), ADC2E[1]->Eval(16384));
  hGe1TUnmatched = new TH1D("h1Ge1TDCUnmatched",
			    "Ge1 Unmatched TDC hits;Block time (ns)",
			    1000, 0., 100.e6);
  hGe2TUnmatched = new TH1D("h1Ge2TDCUnmatched",
			    "Ge2 Unmatched TDC hits;Block time (ns)",
			    1000, 0., 100.e6);
  hGe1MuTVsE     = new TH2D("h2Ge1SpectraMuCentered",
			    "Ge1 Spectra;Time since muon (ns);E (keV)",
			    1000, 0., 10.e3,
			    4096, ADC2E[0]->Eval(0), ADC2E[0]->Eval(16384));
  hGe2MuTVsE     = new TH2D("h2Ge2SpectraMuCentered",
			    "Ge2 Spectra;Time since muon (ns);E (keV)",
			    1000, 0., 10.e3,
			    4096, ADC2E[1]->Eval(0), ADC2E[1]->Eval(16384));
  hGe1MuTVsE_CPP = new TH2D("h2Ge1SpectraMuCenteredCPProtected",
			    "Ge1 Spectra;Time since muon (ns);E (keV)",
			    1000, 0., 10.e3,
			    4096, ADC2E[0]->Eval(0), ADC2E[0]->Eval(16384));
  hGe2MuTVsE_CPP = new TH2D("h2Ge2SpectraMuCenteredCPProtected",
			    "Ge2 Spectra;Time since muon (ns);E (keV)",
			    1000, 0., 10.e3,
			    4096, ADC2E[1]->Eval(0), ADC2E[1]->Eval(16384));
  hGeEVsE        = new TH2D("hGeEVsE",
			    "Coincident Ge hits;Ge1 E (keV);Ge2 E (keV)",
			    4096, ADC2E[0]->Eval(0.), ADC2E[0]->Eval(16384.),
			    4096, ADC2E[0]->Eval(0.), ADC2E[0]->Eval(16384.));
  return SUCCESS;
}

INT GeSpectra(EVENT_HEADER *pheader, void *pevent) {

  vector<double> tdc1_ts, tdc2_ts, wfd1_ts, wfd2_ts;
  vector<double> wfd1_es, wfd2_es;
  vector<double> mu_ts;
  vector<double> veto1_ts, veto2_ts;
  GetGeTDCTimes(tdc1_ts, tdc2_ts);
  GetGeWFDTimes(wfd1_ts, wfd2_ts);
  GetGeEnergies(wfd1_es, wfd2_es);
  GetMuScTimes (mu_ts);
  GetVetoTimes (veto1_ts, veto2_ts);

  vector<bool> filter1, filter2;
  GetFilters(filter1, filter2);
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
    if (!filter1[iwfd]) continue;
    hGe1BlockTVsE->Fill(tdc1_ts[itdc], wfd1_es[iwfd]);

    vector<double>::const_iterator mut =
      lower_bound(mu_ts.begin(), mu_ts.end(), tdc1_ts[itdc] + MuonLookTime[0]);
    const bool cpprotected = IsCPProtected(veto1_ts, tdc1_ts[itdc], 1);
    if (mut == mu_ts.end()) {
      hGe1MuTVsE->Fill(1.e10, wfd1_es[iwfd]);
      if (cpprotected) hGe1MuTVsE_CPP->Fill(1.e10, wfd1_es[iwfd]);
    } else {
      hGe1MuTVsE->Fill(tdc1_ts[itdc] - *mut, wfd1_es[iwfd]);
      if (cpprotected) hGe1MuTVsE_CPP->Fill(tdc1_ts[itdc] - *mut, wfd1_es[iwfd]);
    }
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
    if (!filter2[iwfd]) continue;
    hGe2BlockTVsE->Fill(tdc2_ts[itdc], wfd2_es[iwfd]);

    vector<double>::const_iterator mut =
      lower_bound(mu_ts.begin(), mu_ts.end(), tdc2_ts[itdc] + MuonLookTime[0]);
    const bool cpprotected = IsCPProtected(veto2_ts, tdc2_ts[itdc], 2);
    if (mut == mu_ts.end()) {
      hGe2MuTVsE->Fill(1.e10, wfd2_es[iwfd]);
      if (cpprotected) hGe2MuTVsE_CPP->Fill(1.e10, wfd2_es[iwfd]);
    } else {
      hGe2MuTVsE->Fill(tdc2_ts[itdc] - *mut, wfd2_es[iwfd]);
      if (cpprotected) hGe2MuTVsE_CPP->Fill(tdc2_ts[itdc] - *mut, wfd2_es[iwfd]);
    }
  }
  // Ge1 vs Ge2
  // Number of TPIs will be the same in each vector, number of actual pulses
  // will be different. The pulse height of not an actual pulse will be near
  // zero.
  for (int ige; ige < wfd1_es.size(); ++ige)
    hGeEVsE->Fill(wfd1_es[ige], wfd2_es[ige]);

  return SUCCESS;
}

void GetMuScTimes(vector<double>& musc) {
  const string bank("T402");
  GetTDCTimes(bank, musc);
}
  
void GetGeTDCTimes(vector<double>& ge1, vector<double>& ge2) {
  const string GeT_name[2] = { "T404", "T405" };
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

void GetTDCTimes(const string& bank, vector<double>& ts) {
  const double tick = 0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;

  if (!hit_map.count(bank)) return;
  const vector<int64_t>& hits = hit_map.at(bank);
  ts.reserve(hits.size());
  for (int ihit = 0; ihit < hits.size(); ++ihit)
    ts.push_back(tick*hits[ihit]);
}

void GetVetoTimes(vector<double>& veto1, vector<double>& veto2) {
  const string Veto_name[2] = { "T412", "T414" };
  vector<double>* vetos[2] = { &veto1, &veto2 };
  for (int iveto = 0; iveto < 2; ++iveto)
    GetTDCTimes(Veto_name[iveto], *vetos[iveto]);
}

bool IsCPProtected(const vector<double>& vs, double t, int ige) {
  const vector<double>::const_iterator b = vs.begin(), e = vs.end();
  return ige == 1 ?
    lower_bound(b, e, t - 200.) - lower_bound(b, e, t - 280.) :
    lower_bound(b, e, t - 300.) - lower_bound(b, e, t - 380.);
}

void GetFilters(vector<bool>& filter1, vector<bool>& filter2) {
  const string names[2] = { "D401", "D402" };
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  vector<bool>* filters[2] = { &filter1, &filter2 };
  for (int i = 0; i < 2; ++i) {
    if (tpi_map.count(names[i])) {
      const vector<TPulseIsland*>& tpis = tpi_map.at(names[i]);
      *(filters[i]) = vector<bool>(tpis.size(), true); continue;
      
      filters[i]->reserve(tpis.size());
      for (int j = 0; j < tpis.size(); ++j)
	filters[i]->push_back(tpis[j]->GetPulseLength() == 300 &&
			      tpis[j]->GetPeakSample()   > 50 &&
			      tpis[j]->GetPeakSample()   < 250);
    }
  }
}
	  
