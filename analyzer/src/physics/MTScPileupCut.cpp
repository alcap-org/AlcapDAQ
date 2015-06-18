////////////////////////////////////////////////////////////////////
///  \defgroup MTScPileupCut
///  \author Damien Alexander
///
///  \brief
///  Timing cut of two TSc hits in 2 us.  Not to be used in
///  production of trees.
/// @{
///////////////////////////////////////////////////////////////////


/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <map>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include "TH1D.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

/*--Module declaration ---------------------------------------*/
static INT MTScPileupCut_init(void);
static INT MTScPileupCut(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;
TH1D* TScPileupPreCut, *TScPileupPostCut, *TScPileupCut;

const double TIME_LOW = -2e3, TIMEHIGH = 2e3; // in ns
ANA_MODULE MTScPileupCut_module = 
  {
  "MTScPileupCut",       /* module name           */
  "Damien Alexander",    /* author                */
  MTScPileupCut,         /* event routine         */
  NULL,                  /* BOR routine           */
  NULL,                  /* EOR routine           */
  MTScPileupCut_init,    /* init routine          */
  NULL,                  /* exit routine          */
  NULL,                  /* parameter structure   */
  0,                     /* structure size        */
  NULL,                  /* initial parameters    */
};

/*--module init routine -----------------------------------------*/
//anything necessary here?  Not building histograms
INT MTScPileupCut_init() {
  std::string histname = "TScPileupPreCut";
  std::string histtitle = "TSc correlation before pileup protection";
  TScPileupPreCut = new TH1D(histname.c_str(), histtitle.c_str(), 2000, -10e3, 10e3);
  TScPileupPreCut->GetXaxis()->SetTitle("TSc time difference (ns)");
  histname = "TScPileupPostCut";
  histtitle = "TSc correlation after pileup protection";
  TScPileupPostCut = new TH1D(histname.c_str(), histtitle.c_str(), 2000, -10e3, 10e3);
  TScPileupPostCut->GetXaxis()->SetTitle("TSc time difference (ns)");
  histname = "TScPileupCut";
  histtitle = "TSc correlation of pileup protected muons";
  TScPileupCut = new TH1D(histname.c_str(), histtitle.c_str(), 2000, -10e3, 10e3);
  TScPileupCut->GetXaxis()->SetTitle("TSc time difference (ns)");

  return SUCCESS;
}

INT MTScPileupCut(EVENT_HEADER *pheader, void *pevent)
{
  std::map<std::string, std::vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  std::string target = gSetup->GetBankName("TSc");
  if(!wfd_map.count(target)) {
    printf("MTScPileupCut: No reference hits TSc!\n");
    return SUCCESS;
  }
  std::vector<TPulseIsland*>& target_hits = wfd_map.at(target);

  //std::cout << target_hits.size() << " hits in TSc before cut." << std::endl;
  const double clock_tick = gSetup->GetClockTick(target);
  for(unsigned i = 0; i < target_hits.size(); i++)
    for(unsigned j=i+1; j< target_hits.size(); j++) {
      if(i<0) i = 0;
      if(j<i+1) j=i+1;
      const double dt = clock_tick * (target_hits[j]->GetTimeStamp() - target_hits[i]->GetTimeStamp());
      TScPileupPreCut->Fill(dt);

      if(dt < TIME_LOW)
	break;
      else if(dt < TIMEHIGH && i > 1){  // a coincidence has occured
	target_hits.erase(target_hits.begin() + j);
	target_hits.erase(target_hits.begin() + i);
	j -= 2;
	i--;
	TScPileupCut->Fill(dt);
      }
      else
	TScPileupPostCut->Fill(dt);
    }

  //std::cout << target_hits.size() << " hits in TSc after cut." <<std::endl;
  return SUCCESS;
}

