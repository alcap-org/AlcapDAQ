////////////////////////////////////////////////////////////////////
///  \defgroup MVScTScCoincCut
///  \author Damien Alexander
///
///  \brief
///  Timing cut between the TSc and VSc.  Not to be used in
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
static INT MVScTScCoincCut_init(void);
static INT MVScTScCoincCut(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

const double TIME_LOW = -2e3, TIMEHIGH = 2e3; // in ns
ANA_MODULE MVScTScCoincCut_module = 
  {
  "MVScTScCoincCut",     /* module name           */
  "Damien Alexander",    /* author                */
  MVScTScCoincCut,       /* event routine         */
  NULL,                  /* BOR routine           */
  NULL,                  /* EOR routine           */
  MVScTScCoincCut_init,  /* init routine          */
  NULL,                  /* exit routine          */
  NULL,                  /* parameter structure   */
  0,                     /* structure size        */
  NULL,                  /* initial parameters    */
};

/*--module init routine -----------------------------------------*/
//anything necessary here?  Not building histograms
INT MVScTScCoincCut_init() {
  return SUCCESS;
}

INT MVScTScCoincCut(EVENT_HEADER *pheader, void *pevent)
{
  std::map<std::string, std::vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
  std::string target = gSetup->GetBankName("TTSc");
  if(!tdc_map.count(target)) {
    printf("MVScTScCoincCut: No reference hits TTSc!\n");
    return SUCCESS;
  }
  std::vector<int64_t>& target_hits = tdc_map.at(target);

  std::string veto = gSetup->GetBankName("TVSc");
  if(!tdc_map.count(veto)) {
    printf("MVScTScCoincCut: No Veto hits TVSc!\n");
    return SUCCESS;
  }
  std::vector<int64_t>& veto_hits = tdc_map.at(veto);

  //std::cout << target_hits.size() << " hits in TSc before cut." << std::endl;

  for(unsigned i = 0; i < target_hits.size(); i++)
    for(unsigned j=0; j< veto_hits.size(); j++) {
      static const double clock_tick = 0.025; // conversion to ns'
      const double dt = clock_tick * (target_hits[i] -  veto_hits[j]);
      if(dt < TIME_LOW)
	break;
      else if(dt < TIMEHIGH){  // a coincidence has occured
	target_hits.erase(target_hits.begin() + i);
	i--;
      }
    }

  //std::cout << target_hits.size() << " hits in TSc after cut." <<std::endl;
  return SUCCESS;
}

