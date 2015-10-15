////////////////////////////////////////////////////////////////////
///  \defgroup MVetoTDCCoincCut
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
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"

/*--Module declaration ---------------------------------------*/
static INT MPileupCut_init(void);
static INT MPileupCut(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace{
  const double TIME_LOW = -2000, TIME_HIGH = 2000; // in ns
  const double DOUBLEPULSE_LOW = -50, DOUBLEPULSE_HIGH = 50;
}

ANA_MODULE MPileupCut_module = 
  {
  "MPileupCut",          /* module name           */
  "Damien Alexander",    /* author                */
  MPileupCut,            /* event routine         */
  NULL,                  /* BOR routine           */
  NULL,                  /* EOR routine           */
  MPileupCut_init,       /* init routine          */
  NULL,                  /* exit routine          */
  NULL,                  /* parameter structure   */
  0,                     /* structure size        */
  NULL,                  /* initial parameters    */
};

/*--module init routine -----------------------------------------*/
//anything necessary here?  Not building histograms
INT MPileupCut_init() {
  return SUCCESS;
}

INT MPileupCut(EVENT_HEADER *pheader, void *pevent)
{
  std::map<std::string, std::vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
  for(std::map<std::string, std::vector<int64_t> >::iterator mIter = tdc_map.begin(); mIter != tdc_map.end(); mIter++){
    std::string detname = gSetup->GetDetectorName(mIter->first);
    if(!tdc_map.count(mIter->first))
      continue;
    
    std::vector<int64_t>& det_hits = mIter->second;

    double CUT_LOW = DOUBLEPULSE_LOW, CUT_HIGH = DOUBLEPULSE_HIGH;
    if(detname == "TTSc") { CUT_LOW = TIME_LOW; CUT_HIGH = TIME_HIGH; }

    for(unsigned i = 0; i < det_hits.size(); i++){
      
      //check for double pulse counts
      for(unsigned j=i+1; j< det_hits.size(); j++) {
	if(j == i) j=i+1;
	static const double clock_tick = TICKTDC; // conversion to ns
	const double dt = clock_tick * (det_hits[i] -  det_hits[j]);
	if(dt < DOUBLEPULSE_LOW) break;
	else if(dt < DOUBLEPULSE_HIGH){  // a coincidence has occured
	  det_hits.erase(det_hits.begin() + j);
	  j--;
	}
      } // end j loop
      
      if(detname != "TTSc") continue;

      for(unsigned j=i+1; j< det_hits.size(); j++) {
	if(j == i) j=i+1;
	static const double clock_tick = TICKTDC; // conversion to ns
	const double dt = clock_tick * (det_hits[i] -  det_hits[j]);
	if(dt < TIME_LOW) break;
	else if(dt < TIME_HIGH){  // a coincidence has occured
	  det_hits.erase(det_hits.begin() + j);
	  j--;
	  if(i > 0){
	    det_hits.erase(det_hits.begin()+i);
	    i--;
	  }
	}
      } // end j loop
    
    }//end i loop

  }

  return SUCCESS;
}

