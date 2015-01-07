/********************************************************************\

  Name:         MPileUpPerBlock
  Created by:   John R Quirk

  Contents:     A module to plot the number of muSC events per MIDAS block

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>

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
INT  MPileUpPerBlock_init(void);
INT  MPileUpPerBlock(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static TH1I* hPileUpPerBlock;

// Time windows
static const double pileup_tw = 10000.; // ns


ANA_MODULE MPileUpPerBlock_module =
{
  "MPileUpPerBlock",                    /* module name           */
  "John R Quirk",              /* author                */
  MPileUpPerBlock,                      /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MPileUpPerBlock_init,                 /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/** This method initializes histograms.
  */
INT MPileUpPerBlock_init()
{
  
  hPileUpPerBlock = new TH1I("hPileUpPerBlock","Number of Pile Up; Percent; Counts",100,0.,1);
  return SUCCESS;
}


INT MPileUpPerBlock(EVENT_HEADER *pheader, void *pevent)
{

  unsigned int nPileUp = 0;

  // We assume the existance of all necessary TPI vectors
  std::map< std::string, std::vector<TPulseIsland*> >& tpis = gData->fPulseIslandToChannelMap;
  std::map< std::string, std::string >& dets = gSetup->fBankToDetectorMap;
  std::map< std::string, std::vector<TPulseIsland*> >::iterator iBank;
  std::vector<TPulseIsland*> *musc_pulses = NULL;
  std::vector<TPulseIsland*>::iterator curr_musc_pulse, next_musc_pulse;

  for (iBank = tpis.begin(); iBank != tpis.end(); iBank++) {
    if (dets.at(iBank->first) == "muSC") {
      musc_pulses = &tpis.at(iBank->first);
      curr_musc_pulse = musc_pulses->begin();
      next_musc_pulse = curr_musc_pulse + 1;
    }
  }

  double cTime = 0;
  for (; curr_musc_pulse != musc_pulses->end();) {
    if ((*next_musc_pulse)->GetPulseTime() - (*curr_musc_pulse)->GetPulseTime() < pileup_tw)
      nPileUp++;
    curr_musc_pulse++;
    next_musc_pulse++;
  }

  hPileUpPerBlock->Fill(nPileUp);

  return SUCCESS;
}
