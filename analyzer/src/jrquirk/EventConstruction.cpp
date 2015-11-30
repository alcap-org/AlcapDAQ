/********************************************************************\

  Name:         EventConstruction
  Created by:   John R Quirk

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>

using std::string;
using std::vector;
using std::map;
using std::lower_bound;

/* ROOT includes */
#include "TH1I.h"
#include "TH2I.h"
#include "TGraph.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TFile.h"

/* MIDAS includes */
#include "midas.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

/*-- Module declaration --------------------------------------------*/
static INT EventConstruction_init(void);
static INT EventConstruction_bor(INT);
static INT EventConstruction(EVENT_HEADER*, void*);
static INT EventConstruction_eor(INT);
static vector<double> GetTDCTimes(const string&);
static vector<double> GetWFDTimes(const string&);
static vector<double> GetEnergies(const string&, const TGraph& adc2e);
static int    FirstIndexGreaterThan(const vector<double>& vec, double val);
static const double MAXDT = 10.e3; // ns
static int    IndexIfInCoincidence(const vector<double>& vec, double t0,
				   double tlo=0., double thi=MAXDT);
static double ValueIfInCoincidence(const vector<double>& vec, double t0,
				   double tlo=0., double thi=MAXDT);

extern TGlobalData* gData;
extern TSetupData*  gSetup;
namespace {
  //  const Double_t ADC1[2] = { 6.49508e+03, 7.29774e+03 }; // Before run 192
  //  const Double_t ADC2[2] = { 2.91441e+03, 3.17513e+03 }; // Before run 192
  const Double_t ADC1[2] = {7.49810e+03, 8.45279e+03}; // After and run 192
  const Double_t ADC2[2] = {7.61058e+03, 8.53723e+03}; // After run 192
  const Double_t ADC3[2] = { 0., 0. };
  const Double_t ADC4[2] = { 0., 0. };
  const Double_t E[2] = { 1173.237, 1332.501};
  const TGraph ADC2E1(2, ADC1, E);
  const TGraph ADC2E2(2, ADC2, E);
  const TGraph ADC2E3(2, E, E);
  const TGraph ADC2E4(2, E, E);
  const TGraph* ADC2E[4] = { &ADC2E1, &ADC2E2 , &ADC2E3, &ADC2E4 };
  
  // Timing windows
  string mu_bank, musca_bank;
  string ge1cf_bank, ge1le_bank, ge1s_bank, ge1f_bank;
  string ge2cf_bank, ge2le_bank, ge2s_bank, ge2f_bank;
  string veto1_bank, veto2_bank;

  const double musclo_t0 = -100, musca_t0 = -35.;
  const double ge1cf_t0  = 100., ge1le_t0 = -80.; 
  const double ge2cf_t0  = 200., ge2le_t0 = -25.;
  const double wfdtdc_tmatch[2] = { 15.e3, 18.e3 };
  const double veto_tmatch[2]   = { 0., 800. };

  const Double_t VALUENOTFOUND = std::numeric_limits<Double_t>::max();
  const int      INDEXNOTFOUND = -1;

  struct Event {
    Double_t block_t, musca_t;
    Double_t ge1cf_t, ge1le_t, ge1s_t, ge1f_t;
    Double_t ge2cf_t, ge2le_t, ge2s_t, ge2f_t;
    Double_t ge1s_e, ge1f_e;
    Double_t ge2s_e, ge2f_e;
    Double_t veto1_t, veto2_t;
    void Initialize(double t0) {
      block_t = t0;
      musca_t = VALUENOTFOUND;
      ge1cf_t = ge1le_t = ge1s_t = ge1f_t = VALUENOTFOUND;
      ge2cf_t = ge2le_t = ge2s_t = ge2f_t = VALUENOTFOUND;
      ge1s_e = ge1f_e = ge2s_e = ge2f_e = VALUENOTFOUND;
      veto1_t = veto2_t = VALUENOTFOUND;
    }
    void ApplyTimeOffset() {
      if (musca_t != VALUENOTFOUND) musca_t -= block_t;
      if (ge1cf_t != VALUENOTFOUND) ge1cf_t -= block_t;
      if (ge1le_t != VALUENOTFOUND) ge1le_t -= block_t;
      if (ge1s_t  != VALUENOTFOUND) ge1s_t  -= block_t;
      if (ge1f_t  != VALUENOTFOUND) ge1f_t  -= block_t;
      if (ge2cf_t != VALUENOTFOUND) ge2cf_t -= block_t;
      if (ge2le_t != VALUENOTFOUND) ge2le_t -= block_t;
      if (ge2s_t  != VALUENOTFOUND) ge2s_t  -= block_t;
      if (ge2f_t  != VALUENOTFOUND) ge2f_t  -= block_t;
      if (veto1_t != VALUENOTFOUND) veto1_t -= block_t;
      if (veto2_t != VALUENOTFOUND) veto2_t -= block_t;
    }
    void Print() {
      printf("block_t:\t%g\n", block_t);
      printf("musca_t:\t%g\n", musca_t);
      printf("ge1cf_t:\t%g\n", ge1cf_t);
      printf("ge1le_t:\t%g\n", ge1le_t);
      printf("ge1s_t :\t%g\n", ge1s_t);
      printf("ge1f_t :\t%g\n", ge1f_t);
      printf("ge2cf_t:\t%g\n", ge2cf_t);
      printf("ge2le_t:\t%g\n", ge2le_t);
      printf("ge2s_t :\t%g\n", ge2s_t);
      printf("ge2f_t :\t%g\n", ge2f_t);
      printf("ge1s_e :\t%g\n", ge1s_e);
      printf("ge1f_e :\t%g\n", ge1f_e);
      printf("ge2s_e :\t%g\n", ge2s_e);
      printf("ge2f_e :\t%g\n", ge2f_e);
      printf("veto1_t:\t%g\n", veto1_t);
      printf("veto1_t:\t%g\n", veto2_t);
    }
  } event;
  TTree* gEvents = NULL;
  TFile* gEventFile = NULL;
}

ANA_MODULE EventConstruction_module =
{
  "EventConstruction",    /* module name           */
  "John R Quirk",         /* author                */
  EventConstruction,      /* event routine         */
  EventConstruction_bor,  /* BOR routine           */
  EventConstruction_eor,  /* EOR routine           */
  EventConstruction_init, /* init routine          */
  NULL,                   /* exit routine          */
  NULL,                   /* parameter structure   */
  0,                      /* structure size        */
  NULL,                   /* initial parameters    */
};

INT EventConstruction_init() {
  mu_bank    = gSetup->GetBankName("TMuScHi");
  musca_bank = gSetup->GetBankName("TMuScA");
  ge1cf_bank = gSetup->GetBankName("TGe1CF");
  ge1le_bank = gSetup->GetBankName("TGe1LE");
  ge1s_bank  = gSetup->GetBankName("Ge1Energy");
  ge1f_bank  = gSetup->GetBankName("Ge1Time");
  ge2cf_bank = gSetup->GetBankName("TGe2CF");
  ge2le_bank = gSetup->GetBankName("TGe2LE");
  ge2s_bank  = gSetup->GetBankName("Ge2Energy");
  ge2f_bank  = gSetup->GetBankName("Ge2Time");
  veto1_bank = gSetup->GetBankName("TeSc1Lo");
  veto2_bank = gSetup->GetBankName("TeSc2Lo");

  return SUCCESS;
}

INT EventConstruction_bor(INT run) {
  char fname[16]; sprintf(fname, "evt%05d.root", run);
  gEventFile = new TFile(fname, "RECREATE");
  
  gEvents = new TTree("events", "Muon Centered Hits");
  gEvents->SetDirectory(gEventFile);
  gEvents->Branch("block_t", &event.block_t);
  gEvents->Branch("musca_t", &event.musca_t);
  gEvents->Branch("ge1cf_t", &event.ge1cf_t);
  gEvents->Branch("ge1le_t", &event.ge1le_t);
  gEvents->Branch("ge1s_t",  &event.ge1s_t);
  gEvents->Branch("ge1f_t",  &event.ge1f_t);
  gEvents->Branch("ge2cf_t", &event.ge2cf_t);
  gEvents->Branch("ge2le_t", &event.ge2le_t);
  gEvents->Branch("ge2s_t",  &event.ge2s_t);
  gEvents->Branch("ge2f_t",  &event.ge2f_t);
  gEvents->Branch("ge1s_e",  &event.ge1s_e);
  gEvents->Branch("ge1f_e",  &event.ge1f_e);
  gEvents->Branch("ge2s_e",  &event.ge2s_e);
  gEvents->Branch("ge2f_e",  &event.ge2f_e);
  gEvents->Branch("veto2_t", &event.veto1_t);
  gEvents->Branch("veto1_t", &event.veto2_t);
  
  return SUCCESS;
}

INT EventConstruction(EVENT_HEADER *pheader, void *pevent) {

  const vector<double> mu_ts    = GetTDCTimes(mu_bank);
  const vector<double> musca_ts = GetTDCTimes(musca_bank);
  const vector<double> ge1cf_ts = GetTDCTimes(ge1cf_bank);
  const vector<double> ge1le_ts = GetTDCTimes(ge1le_bank);
  const vector<double> ge1s_ts  = GetWFDTimes(ge1s_bank);
  const vector<double> ge1f_ts  = GetWFDTimes(ge1f_bank);
  const vector<double> ge2cf_ts = GetTDCTimes(ge2cf_bank);
  const vector<double> ge2le_ts = GetTDCTimes(ge2le_bank);
  const vector<double> ge2s_ts  = GetWFDTimes(ge2s_bank);
  const vector<double> ge2f_ts  = GetWFDTimes(ge2f_bank);
  const vector<double> ge1s_es  = GetEnergies(ge1s_bank, ADC2E1);
  const vector<double> ge1f_es  = GetEnergies(ge1f_bank, ADC2E3);
  const vector<double> ge2s_es  = GetEnergies(ge2s_bank, ADC2E2);
  const vector<double> ge2f_es  = GetEnergies(ge2f_bank, ADC2E4);
  const vector<double> veto1_ts = GetTDCTimes(veto1_bank);
  const vector<double> veto2_ts = GetTDCTimes(veto2_bank);

  for (int imu = 0; imu < mu_ts.size(); ++imu) {
    const Double_t t0 = mu_ts[imu];
    
    event.Initialize(t0);
    event.musca_t = ValueIfInCoincidence(musca_ts, t0, musca_t0);
    event.ge1cf_t = ValueIfInCoincidence(ge1cf_ts, t0, ge1cf_t0);
    event.ge1le_t = ValueIfInCoincidence(ge1le_ts, t0, ge1le_t0);
    event.veto1_t = ValueIfInCoincidence(veto1_ts, event.ge1cf_t,
					 veto_tmatch[0], veto_tmatch[1]);
    event.ge2cf_t = ValueIfInCoincidence(ge2cf_ts, t0, ge2cf_t0);
    event.ge2le_t = ValueIfInCoincidence(ge2le_ts, t0, ge2le_t0);
    event.veto2_t = ValueIfInCoincidence(veto2_ts, event.ge2cf_t,
					 veto_tmatch[0], veto_tmatch[1]);
    const int iwfd1 = IndexIfInCoincidence(ge1s_ts, event.ge1cf_t,
					   wfdtdc_tmatch[0], wfdtdc_tmatch[1]);
    if (iwfd1 != INDEXNOTFOUND) {
      event.ge1s_t = ge1s_ts[iwfd1];
      event.ge1f_t = ge1f_ts[iwfd1];
      event.ge1s_e = ge1s_es[iwfd1];
      event.ge1f_e = ge1f_es[iwfd1];
    }
    const int iwfd2 = IndexIfInCoincidence(ge2s_ts, event.ge2cf_t,
					   wfdtdc_tmatch[0], wfdtdc_tmatch[1]);
    if (iwfd2 != INDEXNOTFOUND) {
      event.ge2s_t = ge2s_ts[iwfd2];
      event.ge2f_t = ge2f_ts[iwfd2];
      event.ge2s_e = ge2s_es[iwfd2];
      event.ge2f_e = ge2f_es[iwfd2];
    }
    //event.Print();
    event.ApplyTimeOffset();
    gEvents->Fill();
  }
  return SUCCESS;
}

INT EventConstruction_eor(INT run_number) {
  gEventFile->Write();
  gEventFile->Close();
  delete gEventFile;
  gEvents = NULL;
  gEventFile = NULL;
  return SUCCESS;
}

vector<double> GetWFDTimes(const string& name) {
  const double tick = 62.; // ns
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  vector<double> ts;
  if (!tpi_map.count(name)) return ts;
  const vector<TPulseIsland*>& tpis = tpi_map.at(name);
  ts.reserve(tpis.size());
  for (int itpi = 0; itpi < tpis.size(); ++itpi)
    ts.push_back(tick*tpis[itpi]->GetCFTime(0.2));
  return ts;
}

vector<double> GetEnergies(const string& name, const TGraph& adc2e) {
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  vector<double> es;
  if (!tpi_map.count(name)) return es;
  const vector<TPulseIsland*>& tpis = tpi_map.at(name);
  es.reserve(tpis.size());
  for (int itpi = 0; itpi < tpis.size(); ++itpi)
    es.push_back(adc2e.Eval(tpis[itpi]->GetPulseHeight()));
  return es;
}

vector<double> GetTDCTimes(const string& name) {
  const double tick = 0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;

  vector<double> ts;
  if (!hit_map.count(name)) return ts;
  const vector<int64_t>& hits = hit_map.at(name);
  ts.reserve(hits.size());
  for (int ihit = 0; ihit < hits.size(); ++ihit)
    ts.push_back(tick*hits[ihit]);
  return ts;
}

int FirstIndexGreaterThan(const vector<double>& vec, double val) {
  vector<double>::const_iterator i = lower_bound(vec.begin(), vec.end(), val);
  return i == vec.end() ? INDEXNOTFOUND : i - vec.begin();
}

double ValueIfInCoincidence(const vector<double>& vec, double t0,
			    double tlo, double thi) {
  int i = IndexIfInCoincidence(vec, t0, tlo, thi);
  return i == INDEXNOTFOUND ? VALUENOTFOUND : vec[i];
}

int IndexIfInCoincidence(const vector<double>& vec, double t0,
			 double tlo, double thi) {
  if ( t0 == VALUENOTFOUND) return INDEXNOTFOUND;
  const int i1 = FirstIndexGreaterThan(vec, t0+tlo);
  const int i2 = FirstIndexGreaterThan(vec, t0+thi);
  return i1 == i2 ? INDEXNOTFOUND : i1;
}
