/********************************************************************\

  Name:         GESpectra
  Created by:   Elisa

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

/* ROOT includes */
#include "TH1I.h"
#include "TH2I.h"

/* MIDAS includes */
#include "midas.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

/*-- Module declaration --------------------------------------------*/
static INT GESpectra_init(void);
static INT GESpectra(EVENT_HEADER*, void*);
static vector<double> GetGe1TTimes();
static vector<double> GetGe2TTimes();
static vector<double> GetGe1ETimes();
static vector<double> GetGe2ETimes();
static vector<double> GetGe1EHeights();
static vector<double> GetGe2EHeights();

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  TH1 *hGe1;
  TH1 *hGe1E;
  TH1 *hGe2;
  TH1 *hGe2E;
  TH1 *hGe1t;
  TH1 *hGe2t;
  TH1 *hTdiffGe1Ge2;
  TH1 *hGe1TDCt;
  TH1 *hGe2TDCt;
  TH1 *hTdiffTDC1;
  TH1 *hTdiffTDC2;
}

ANA_MODULE GESpectra_module =
{
  "GeSpectra",     /* module name           */
  "Elisa",     /* author                */
  GESpectra,       /* event routine         */
  NULL,           /* BOR routine           */
  NULL,           /* EOR routine           */
  GESpectra_init,  /* init routine          */
  NULL,           /* exit routine          */
  NULL,           /* parameter structure   */
  0,              /* structure size        */
  NULL,           /* initial parameters    */
};

INT GESpectra_init() {
  
  hGe1 = new TH1I("gamma1", "Germanium 1 Single Spectrum",
		       20000, 0, 20000);
  hGe1E = new TH1I("gamma1e", "Calibrated Germanium 1 Single Spectrum",
		       3000, 0, 3000);
  hGe2 = new TH1I("gamma2", "Germanium 2 Single Spectrum",
		       20000, 0, 20000);
  hGe2E = new TH1I("gamma2e", "Calibrated Germanium 2 Single Spectrum",
		       3000, 0, 3000);
  hGe1t = new TH1I("time1ADC", "Time (ns) of Ge1 from ADC",
		       10000, 0, 100E+6);
  hGe2t = new TH1I("time2ADC", "Time (ns) of Ge2 from ADC",
		       100000, 0, 100E+6);
  hTdiffGe1Ge2 = new TH1I("Tdiff", "Tdiff (ns) Ge1-Ge2 in 100ms",
		       100000, -100E+6, 100E+6);
  hGe1TDCt = new TH1I("time1TDC", "Time (ns) of Ge1 from TDC",
		       10000, 0, 100E+6);
  hGe2TDCt = new TH1I("time2TDC", "Time (ns) of Ge2 from TDC",
		       10000, 0, 100E+6);
  hTdiffTDC1 = new TH1I("tdTDC1", "Time diff (ns) between consecutive hits in TDC 1",
		       1000000, 0, 100E+6);
  hTdiffTDC2 = new TH1I("tdTDC2", "Time diff (ns) between consecutive hits in TDC 2",
		       1000000, 0, 100E+6);

  return SUCCESS;
}

INT GESpectra(EVENT_HEADER *pheader, void *pevent) {

  //  printf("timeMuons %d \n",t0s.size());
  const vector<double> ts1  = GetGe1ETimes();
  const vector<double> ts2  = GetGe2ETimes();
  const vector<double> es1  = GetGe1EHeights();
  const vector<double> es2  = GetGe2EHeights();
  // all the above vector have same dimension
  const vector<double> tdcs1 = GetGe1TTimes();
  const vector<double> tdcs2 = GetGe2TTimes();

// tdcs1 and tdcs2 do no have the same dimension, tdcs2 has dimension almost double than tdcs1 
//  Example from run 226: timeTDCGe1 175  timeTDCGe2 329   

    vector<double> energiesGe1;  // vector is created with 0 size
    vector<double> energiesGe2;  // vector is created with 0 size
    for (int ii = 0; ii < es1.size(); ++ii){ 
      //  printf("Energy %lf \n",es1[ii]);
      //      energiesGe1.push_back(0.198564*es1[ii]-0.802289);  // cal Ge1 for Pb
      //      energiesGe1.push_back(0.166586*es1[ii]-80.0795);  // cal Ge1 for Zn (made witrh run 290)
      energiesGe1.push_back(1.0*es1[ii]);  // cal Ge1 for W (made witrh run 392)
    hGe1->Fill(es1[ii]);
    hGe1t->Fill(ts1[ii]);
    hGe1E->Fill(energiesGe1[ii]); 
// energies.push_back(0.613113*es2[ii]-6.25095); //cal Ge2 for Pb
//    energiesGe2.push_back(0.17187*es2[ii]-134.93); //cal Ge2 for Zn (made with run 290)
    energiesGe2.push_back(1.0*es2[ii]); //cal Ge2 for W (made with run 392)
    hGe2->Fill(es2[ii]);
    hGe2E->Fill(energiesGe2[ii]);
    hGe2t->Fill(ts2[ii]);
    hTdiffGe1Ge2->Fill(ts1[ii]-ts2[ii]);
  }
 
      for (int ii = 0; ii < tdcs1.size(); ++ii){ 
	hGe1TDCt->Fill(tdcs1[ii]);
        hTdiffTDC1->Fill(tdcs1[ii+1]-tdcs1[ii]);      
      }
      
      for (int ii = 0; ii < tdcs2.size(); ++ii){ 
       hGe2TDCt->Fill(tdcs2[ii]);
       hTdiffTDC2->Fill(tdcs2[ii+1]-tdcs2[ii]);
      }
     
     
      for (int itdc = 0; itdc < tdcs1.size(); ++itdc) {   // loop of Ge1 TDC hits
      vector<double>::const_iterator ilower =
      lower_bound(tdcs2.begin(), tdcs2.end(),                 // search a hit in Ge2 TDC
		  tdcs1[itdc]);
      vector<double>::const_iterator iupper =
      lower_bound(tdcs2.begin(), tdcs2.end(),
		  tdcs1[itdc]+100);  // TDCGe1 + 100ns
      // std::cout << *iupper << std::endl;
      // no idea how to print the ilower, iupper: they are iterators
      // if (iupper - ilower != 1) {
      //hGe2TUnmatched->Fill(tdc2_ts[itdc]);
    //continue;
    }

  return SUCCESS;
}

vector<double> GetGe1TTimes() {
  const string GeT_name("T404");
  const double tick = 0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;

  vector<double> tdcs1;
  if (!hit_map.count(GeT_name))
    return tdcs1;
  const vector<int64_t>& hits = hit_map.at(GeT_name);
  tdcs1.reserve(hits.size());
  for (vector<int64_t>::const_iterator i = hits.begin(), e = hits.end();
       i < e; ++i)
    tdcs1.push_back(tick*(*i));
  return tdcs1;
}

vector<double> GetGe2TTimes() {
  const string GeT_name("T405");
  const double tick = 0.0244140625; // ns
  const map<string, vector<int64_t> >& hit_map =
    gData->fTDCHitsToChannelMap;

  vector<double> tdcs2;
  if (!hit_map.count(GeT_name))
    return tdcs2;
  const vector<int64_t>& hits = hit_map.at(GeT_name);
  tdcs2.reserve(hits.size());
  for (vector<int64_t>::const_iterator i = hits.begin(), e = hits.end();
       i < e; ++i)
    tdcs2.push_back(tick*(*i));
  return tdcs2;
}



vector<double> GetGe1ETimes() {
  const string geE_name("D401");
  const double tick = 62.; // ns
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  vector<double> ts1;
  if (!tpi_map.count(geE_name))
    return ts1;
  const vector<TPulseIsland*>& tpis = tpi_map.at(geE_name);
  ts1.reserve(tpis.size());
  for (vector<TPulseIsland*>::const_iterator i = tpis.begin(), e = tpis.end();
       i < e; ++i)
    ts1.push_back(tick*(*i)->GetCFTime(0.2)); 
  return ts1;
}
vector<double> GetGe2ETimes() {
  const string geE_name("D402");
  const double tick = 62.; // ns
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  vector<double> ts2;
  if (!tpi_map.count(geE_name))
    return ts2;
  const vector<TPulseIsland*>& tpis = tpi_map.at(geE_name);
  ts2.reserve(tpis.size());
  for (vector<TPulseIsland*>::const_iterator i = tpis.begin(), e = tpis.end();
       i < e; ++i)
    ts2.push_back(tick*(*i)->GetCFTime(0.2)); 
  return ts2;
}

vector<double> GetGe1EHeights() {
  const string geE_name("D401");
  
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;
  vector<double> es1;

  if (!tpi_map.count(geE_name))
    return es1;

  const vector<TPulseIsland*>& tpis = tpi_map.at(geE_name);
  es1.reserve(tpis.size());
  for (vector<TPulseIsland*>::const_iterator i = tpis.begin(), e = tpis.end();
       i < e; ++i)
    es1.push_back((*i)->GetPulseHeight());
    return es1; 
}
vector<double> GetGe2EHeights() {
  const string geE_name("D402");
  
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;
  vector<double> es2;

  if (!tpi_map.count(geE_name))
    return es2;

  const vector<TPulseIsland*>& tpis = tpi_map.at(geE_name);
  es2.reserve(tpis.size());
  for (vector<TPulseIsland*>::const_iterator i = tpis.begin(), e = tpis.end();
       i < e; ++i)
    es2.push_back((*i)->GetPulseHeight());
    return es2; 
}
