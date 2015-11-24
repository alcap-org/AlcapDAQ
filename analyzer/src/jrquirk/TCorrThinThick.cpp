/********************************************************************\

  Name:         MTCorrThinThick
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

/* MIDAS includes */
#include "midas.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

/*-- Module declaration --------------------------------------------*/
static INT TCorrThinThick_init(void);
static INT TCorrThinThick(EVENT_HEADER*, void*);
static vector<double> CalculateTimes(const vector<TPulseIsland*>&);

extern HNDLE hDB;
extern TGlobalData* gData;

namespace {
  const string sil1_name[16] = { "SIS3300_B4C1", "SIS3300_B4C2",
				 "SIS3300_B4C3", "SIS3300_B4C4",
				 "SIS3300_B4C5", "SIS3300_B4C6",
				 "SIS3300_B4C7", "SIS3300_B4C8",
				 "SIS3300_B5C1", "SIS3300_B5C2",
				 "SIS3300_B5C3", "SIS3300_B5C4",
				 "SIS3300_B5C5", "SIS3300_B5C6",
				 "SIS3300_B5C7", "SIS3300_B5C8" };
  const string sil3_name     = "SIS3300_B1C6";
  const string sir1_name[4]  = { "SIS3300_B3C1", "SIS3300_B3C2",
				 "SIS3300_B3C1", "SIS3300_B3C2" };
  const string sir2_name     = "SIS3300_B1C7";
  TH1 *hSiL[16], *hSiR[4];
}

ANA_MODULE TCorrThinThick_module =
{
  "TCorrThinThick",    /* module name           */
  "John R Quirk",      /* author                */
  TCorrThinThick,      /* event routine         */
  NULL,                /* BOR routine           */
  NULL,                /* EOR routine           */
  TCorrThinThick_init, /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};

INT TCorrThinThick_init() {
  for (int i = 0; i < 16; ++i) {
    char name[32], title[128];
    sprintf(name, "hSiLTCorr_%d", i+1);
    sprintf(title, "SiL1-%d TCorr with SiL3;Time (ns)", i+1);
    hSiL[i] = new TH1I(name, title, 1000, -500., 500);
  }
    for (int i = 0; i < 4; ++i) {
    char name[32], title[128];
    sprintf(name, "hSiRTCorr_%d", i+1);
    sprintf(title, "SiR1-%d TCorr with SiR2;Time (ns)", i+1);
    hSiR[i] = new TH1I(name, title, 1000, -500., 500);
  }
  return SUCCESS;
}

INT TCorrThinThick(EVENT_HEADER *pheader, void *pevent) {
  const map<string, vector<TPulseIsland*> >& tpi_map =
    gData->fPulseIslandToChannelMap;

  if (tpi_map.count(sil3_name)) {
    const vector<double> sil3 = CalculateTimes(tpi_map.at(sil3_name));
    for (int i = 0; i < 16; ++i) {
      if (!tpi_map.count(sil1_name[i])) continue;
      const vector<double> sil1 = CalculateTimes(tpi_map.at(sil1_name[i]));
      for (vector<double>::const_iterator it = sil3.begin(); it < sil3.end(); ++it) {
	for (vector<double>::const_iterator jt = lower_bound(sil1.begin(), sil1.end(), *it-500.);
	     jt < sil1.end(); ++jt) {
	  double dt = *it - *jt;
	  if (dt > 500.)
	    break;
	  hSiL[i]->Fill(dt);
	}
      }
    }
  }
  if (tpi_map.count(sir2_name)) {
    const vector<double> sir2 = CalculateTimes(tpi_map.at(sir2_name));
    for (int i = 0; i < 4; ++i) {
      if (!tpi_map.count(sir1_name[i])) continue;
      const vector<double> sir1 = CalculateTimes(tpi_map.at(sir1_name[i]));
      for (vector<double>::const_iterator it = sir2.begin(); it < sir2.end(); ++it) {
	for (vector<double>::const_iterator jt = lower_bound(sir1.begin(), sir1.end(), *it-500.);
	     jt < sir1.end(); ++jt) {
	  double dt = *it - *jt;
	  if (dt > 500.)
	    break;
	  hSiR[i]->Fill(dt);
	}
      }
    }
  }
  return SUCCESS;
}

vector<double> CalculateTimes(const vector<TPulseIsland*>& tpis) {
  const double tick = 10.; // ns, all digitizers in 100 MHz digitizer
  vector<double> ts;
  ts.reserve(tpis.size());
  for (int i = 0; i < tpis.size(); ++i)
    ts.push_back(tick*tpis[i]->GetTimeStamp());
  return ts;
}
