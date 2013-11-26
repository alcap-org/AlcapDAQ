/********************************************************************\

  Name:         MHitTime
  Created by:   John R Quirk

  Contents:     A module that plots the time of hits relative to muSC

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

/* MIDAS includes */

/* ROOT includes */
#include <TH1I.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MHitTime_init(void);
INT  MHitTime(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1I* hTime_MuSC;
static std::map< std::string, TH1I* > hEventTimes;
static std::map< std::string, TH1I* > hBlockTimes;

const static double BLOCKLENGTH = 10000000.; // ns
const static double CUTWINDOW = 10000.; // ns

ANA_MODULE MHitTime_module =
{
  "MHitTime",                    /* module name           */
  "John R Quirk",              /* author                */
  MHitTime,                      /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MHitTime_init,                 /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};


INT MHitTime_init()
{

  std::map< std::string, std::vector<TPulseIsland*> >& tpis = gData->fPulseIslandToChannelMap;
  std::map< std::string, std::vector<TPulseIsland*> >::iterator iBank;
  std::map< std::string, std::string> dets = gSetup->fBankToDetectorMap;
  
  std::string cBank, cDet;
  hBlockTimes.clear();
  hEventTimes.clear();
  hTime_MuSC = new TH1I("hTime_MuSC", "Hit Times of MuSC;Block Hit Time (ns);Counts",
				100, 0., BLOCKLENGTH);
  for (iBank = tpis.begin(); iBank != tpis.end(); iBank++) {
    cBank = iBank->first;
    cDet = dets[cBank];
    if (cDet == "muSC")
      continue;
    hBlockTimes[cDet] = new TH1I(("hBlockTime_" + cDet).c_str(), ("Block Hit Times of " + cDet + ";Time (ns); Counts").c_str(),
				 100, 0., BLOCKLENGTH);
    hEventTimes[cDet] = new TH1I(("hEventTime_" + cDet).c_str(), ("Event Hit Times of " + cDet +";Time (ns); Counts").c_str(),
				 100, 0., CUTWINDOW);
  }
  return SUCCESS;
}


INT MHitTime(EVENT_HEADER *pheader, void *pevent)
{

  // We assume the existance of all necessary TPI vectors
  std::map< std::string, std::vector<TPulseIsland*> >& tpis = gData->fPulseIslandToChannelMap;
  std::map< std::string, std::string>& dets = gSetup->fBankToDetectorMap;
  std::map< std::string, std::string>& banks = gSetup->fDetectorToBankMap;

  std::vector<TPulseIsland*> muons = tpis.at(banks.at("muSC"));

  std::map< std::string, std::vector<TPulseIsland*> >::iterator iBank;
  std::map< std::string, std::string>::iterator iDet;
  std::vector<TPulseIsland*>::iterator cMuon;
  std::map< std::string, std::vector<TPulseIsland*>::iterator > pulses;
  for (iBank = tpis.begin(); iBank != tpis.end(); iBank++)
    if(iBank->first != banks.at("muSC"))
      pulses[iBank->first] = iBank->second.begin();
      

  double t_mu;
  double t[2];
  std::string cDet, cBank;
  for (cMuon = muons.begin(); cMuon != muons.end(); cMuon++) {
    t_mu = (*cMuon)->GetPulseTime();
    hTime_MuSC->Fill(t_mu);
    for (iDet = dets.begin(); iDet != dets.end(); iDet++) {
      cBank = iDet->first;
      cDet = iDet->second;
      std::vector<TPulseIsland*>::iterator& cPulse = pulses.at(cBank);
      if (cPulse == tpis.at(cBank).end())
	continue;
      t[0] = (*cPulse)->GetPulseTime();
      t[1] = t[0] - t_mu;
      hBlockTimes.at(cDet)->Fill(t[0]);
      while (t[1] < 0.) {
	cPulse++;
	t[0] = (*cPulse)->GetPulseTime();
	t[1] = t[0] - t_mu;
	hBlockTimes.at(cDet)->Fill(t[0]);
      }
      while (t[1] < CUTWINDOW) {
	hEventTimes.at(cDet)->Fill(t[1]);
	cPulse++;
	t[0] = (*cPulse)->GetPulseTime();
	t[1] = t[0] - t_mu;
	hBlockTimes.at(cDet)->Fill(t[0]);
      }
    }
  }

  // Are there any pulses left?
  for (iDet = dets.begin(); iDet != dets.end(); iDet++) {
    cBank = iDet->first;
    cDet = iDet->second;
    std::vector<TPulseIsland*>::iterator cPulse = pulses.at(cBank);
    while(cPulse != tpis.at(cBank).end()) {
      t[0] = (*cPulse)->GetPulseTime();
      hBlockTimes[cDet]->Fill(t[0]);
      cPulse++;
    }
  }

  return SUCCESS;
}
