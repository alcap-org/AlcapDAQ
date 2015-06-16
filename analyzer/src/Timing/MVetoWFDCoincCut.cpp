////////////////////////////////////////////////////////////////////
///  \defgroup MVetoWFDCoincCut
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
static INT MVetoWFDCoincCut_init(void);
static INT MVetoWFDCoincCut(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

const double TIME_LOW = -2e3, TIME_HIGH = 2e3; // in ns
ANA_MODULE MVetoWFDCoincCut_module = 
  {
  "MVetoWFDCoincCut",     /* module name           */
  "Damien Alexander",    /* author                */
  MVetoWFDCoincCut,       /* event routine         */
  NULL,                  /* BOR routine           */
  NULL,                  /* EOR routine           */
  MVetoWFDCoincCut_init,  /* init routine          */
  NULL,                  /* exit routine          */
  NULL,                  /* parameter structure   */
  0,                     /* structure size        */
  NULL,                  /* initial parameters    */
};

/*--module init routine -----------------------------------------*/
//anything necessary here?  Not building histograms
INT MVetoWFDCoincCut_init() {
  return SUCCESS;
}

INT MVetoWFDCoincCut(EVENT_HEADER *pheader, void *pevent)
{
  std::map<std::string, std::vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
  std::map< std::string, std::vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;
  for(std::map<std::string, std::vector<int64_t> >::iterator mIter = wfd_map.begin(); mIter != wfd_map.end(); mIter++){
    std::string detname = gSetup->GetDetectorName(mIter->first);
    if(!wfd_map.count(mIter->first))
      continue;
    std::string vetoname = "T" + detname + "V";
    std::string vetobank = gSetup->GetBankName(vetoname);
    if( detname == "GeCHT")
      vetoname = "TGeV";
    if(!tdc_map.count(vetobank)){// detector has no veto
      //printf("MVetoTDCCoincCut: No veto found for %s\n", detname.c_str());
      continue;                    // or is veto.  skip bank
    }
    std::vector<int64_t>& det_hits = mIter->second;
    std::vector<int64_t>& veto_hits = tdc_map.at(vetobank);

    //std::cout << detname << " has veto counter " << vetoname << std::endl;
    //std::cout << detname << " has " << det_hits.size() << " hits before the coincidence cut" << std::endl;

    for(unsigned i = 0; i < det_hits.size(); i++)
      for(unsigned j=0; j< veto_hits.size(); j++) {
	static const double clock_tick = 0.025; // conversion to ns
	const double dt = clock_tick * (det_hits[i] -  veto_hits[j]);
	if(dt < TIME_LOW)
	  break;
	else if(dt < TIME_HIGH){  // a coincidence has occured
	  det_hits.erase(det_hits.begin() + i);
	  i--;
	}
      } // end j loop
    //std::cout << detname << " has " << det_hits.size() << " hits after the coincidence cut" << std::endl;


  }

  return SUCCESS;
}

