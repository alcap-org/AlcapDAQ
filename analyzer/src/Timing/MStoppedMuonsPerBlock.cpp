/********************************************************************\

  Name:         MSiPulseFinder
  Created by:   Andrew Edmonds

  Contents:     A module to plot the pulse heights of the silicon detector

\********************************************************************/

/* Standard includes */
#include <stdlib.h>
#include <string>
#include <map>

/* MIDAS includes */

/* ROOT includes */
#include <TH1I.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;


/*-- Module declaration --------------------------------------------*/
INT  MStoppedMuonsPerBlock_init(void);
INT  MStoppedMuonsPerBlock(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1I* hNumberOfStoppedMuonsPerBlock;

// Time windows
static const double tw_muveto = 100.;
static const double tw_sc = 100.;
static const double tw_si = 100.;


ANA_MODULE MStoppedMuonsPerBlock_module =
{
  "MStoppedMuonsPerBlock",                    /* module name           */
  "John R Quirk",              /* author                */
  MStoppedMuonsPerBlock,                      /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MStoppedMuonsPerBlock_init,                 /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MStoppedMuonsPerBlock_init()
{
  
  hNumberOfStoppedMuonsPerBlock = new TH1I("hNumberOfStoppedMuons","Number of (Possibly) Stopped Muons",2000,0.,2000.);
  return SUCCESS;
}


INT MStoppedMuonsPerBlock(EVENT_HEADER *pheader, void *pevent)
{

  unsigned int nStopped = 0;

  // We assume the existance of all necessary TPI vectors
  std::map< std::string, std::vector<TPulseIsland*> >& tpis = gData->fPulseIslandToChannelMap;
  std::map< std::string, std::string>& dets = gSetup->fBankToDetectorMap;
  std::map< std::string, std::vector<TPulseIsland*> >::iterator iBank;
  std::vector<TPulseIsland*> *musc_pulses = NULL, *muveto_pulses = NULL, *scr_pulses = NULL, *scl_pulses = NULL;
  std::vector< std::vector<TPulseIsland*>* > sir_pulses, sil_pulses;
  std::vector<TPulseIsland*>::iterator c_musc_pulse, c_muveto_pulse, c_scr_pulse, c_scl_pulse;
  std::vector< std::vector<TPulseIsland*>::iterator > c_sir_pulse, c_sil_pulse;

  std::string cDet, cBank;
  int nSiL = 0, nSiR = 0;
  for (iBank = tpis.begin(); iBank != tpis.end(); iBank++) {
    cBank = iBank->first;
    cDet = dets[cBank];
    if (cDet == "muSc") {
      musc_pulses = &tpis.at(cBank);
      c_musc_pulse = musc_pulses->begin();
    } else if (cDet == "muVeto"){
      muveto_pulses = &tpis.at(cBank);
      c_muveto_pulse = muveto_pulses->begin();
    } else if (cDet == "ScR") {
      scr_pulses = &tpis.at(cBank);
      c_scr_pulse = scr_pulses->begin();
    } else if (cDet == "ScL") {
      scl_pulses = &tpis.at(cBank);
      c_scl_pulse = scl_pulses->begin();
    } else if (cDet.substr(0,2) == "Si" && cDet.substr(cDet.size()-5,4) == "fast") {
      if(cDet[2] == 'R') {
	sir_pulses.push_back(&tpis.at(cBank));
	c_sir_pulse.push_back(std::vector<TPulseIsland*>::iterator());
	nSiR++;
      } else {
	sil_pulses.push_back(&tpis.at(cBank));
	c_sil_pulse.push_back(std::vector<TPulseIsland*>::iterator());
	nSiL++;
      }
    }
  }


  double cTime = 0;
  for (; c_musc_pulse != musc_pulses->end(); c_musc_pulse++) {
    cTime = (*c_musc_pulse)->GetPulseTime();

    while ((*c_muveto_pulse)->GetPulseTime() < cTime)
      c_muveto_pulse++;
    while ((*c_scl_pulse)->GetPulseTime() < cTime)
      c_scl_pulse++;
    while ((*c_scr_pulse)->GetPulseTime() < cTime)
      c_scr_pulse++;
    for(int iSiR = 0; iSiR < nSiR; iSiR++)
      while ((*c_sir_pulse[iSiR])->GetPulseTime() < cTime)
	c_sir_pulse[iSiR]++;
    for (int iSiL = 0; iSiL < nSiL; iSiL++)
      while ((*c_sil_pulse[iSiL])->GetPulseTime() < cTime)
	c_sil_pulse[iSiL]++;

    if ((*c_muveto_pulse)->GetPulseTime() - cTime < tw_muveto)
      continue;
    if ((*c_scr_pulse)->GetPulseTime() - cTime < tw_sc)
      continue;
    if ((*c_scl_pulse)->GetPulseTime() - cTime < tw_sc)
      continue;
    for (int iSiR = 0; iSiR < nSiR; iSiR++)
      if ((*c_sir_pulse[iSiR])->GetPulseTime() - cTime < tw_si)
	continue;
    for (int iSiL = 0; iSiL < nSiL; iSiL++)
      if ((*c_sil_pulse[iSiL])->GetPulseTime() - cTime < tw_si)
	continue;

    nStopped++;
  }

  hNumberOfStoppedMuonsPerBlock->Fill(nStopped);

  return SUCCESS;
}
