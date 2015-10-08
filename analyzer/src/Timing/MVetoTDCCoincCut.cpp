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
static INT MVetoTDCCoincCut_init(void);
static INT MVetoTDCCoincCut(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace{
  const double TIME_LOW = -300, TIME_HIGH = 300; // in ns
 const double DOUBLEPULSE_LOW = -50, DOUBLEPULSE_HIGH = 50;
}

ANA_MODULE MVetoTDCCoincCut_module = 
  {
  "MVetoTDCCoincCut",     /* module name           */
  "Damien Alexander",    /* author                */
  MVetoTDCCoincCut,       /* event routine         */
  NULL,                  /* BOR routine           */
  NULL,                  /* EOR routine           */
  MVetoTDCCoincCut_init,  /* init routine          */
  NULL,                  /* exit routine          */
  NULL,                  /* parameter structure   */
  0,                     /* structure size        */
  NULL,                  /* initial parameters    */
};

/*--module init routine -----------------------------------------*/
//anything necessary here?  Not building histograms
INT MVetoTDCCoincCut_init() {
  return SUCCESS;
}

INT MVetoTDCCoincCut(EVENT_HEADER *pheader, void *pevent)
{
  std::map<std::string, std::vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
  for(std::map<std::string, std::vector<int64_t> >::iterator mIter = tdc_map.begin(); mIter != tdc_map.end(); mIter++){
    std::string detname = gSetup->GetDetectorName(mIter->first);
    if(!tdc_map.count(mIter->first))
      continue;
    
    std::string vetoname = detname + "V";
    if( detname == "TGeCHT") vetoname = "TGeV";
    if(detname == "TTSc") vetoname = "TVSc";
    std::string vetobank = gSetup->GetBankName(vetoname);
    if(!tdc_map.count(vetobank)){// detector has no veto or is veto, skip
      //printf("MVetoTDCCoincCut: No veto found for %s\n", detname.c_str());
      continue; 
    }
    
    std::vector<int64_t>& det_hits = mIter->second;
    std::vector<int64_t>& veto_hits = tdc_map.at(vetobank);

    //std::cout << detname << " has veto counter " << vetoname << std::endl;
    //std::cout << detname << " has " << det_hits.size() << " hits before the coincidence cut" << std::endl;

    for(unsigned i = 0; i < det_hits.size(); i++){
      
      //check for double pulse counts
      for(unsigned j=i+1; j< det_hits.size(); j++) {
	static const double clock_tick = TICKTDC; // conversion to ns
	const double dt = clock_tick * (det_hits[i] -  det_hits[j]);
	if(dt < DOUBLEPULSE_LOW) break;
	else if(dt < DOUBLEPULSE_HIGH){  // a coincidence has occured
	  det_hits.erase(det_hits.begin() + j);
	  j--;
	}
      } // end j loop
      
      ///check for vetos in time window
      for(unsigned j=0; j< veto_hits.size(); j++) {
	static const double clock_tick = TICKTDC; // conversion to ns
	const double dt = clock_tick * (det_hits[i] -  veto_hits[j]);
	if(dt < TIME_LOW) break;
	else if(dt < TIME_HIGH){  // a coincidence has occured
	  det_hits.erase(det_hits.begin() + i);
	  i--;
	}
      } // end j loop
    //std::cout << detname << " has " << det_hits.size() << " hits after the coincidence cut" << std::endl;
    
    }//end i loop

  }

  return SUCCESS;
}

