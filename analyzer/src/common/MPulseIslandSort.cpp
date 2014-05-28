/********************************************************************\

Name:         MPulseIslandSort
Created by:   Vladimir Tishchenko

Contents:     A module to sort PulseIslands in time

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <algorithm> 
#include <iostream> 

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"


/*-- Module declaration --------------------------------------------*/
static INT module_event(EVENT_HEADER *pheader, void *pevent);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

ANA_MODULE MPulseIslandSort_module =
{
	"MPulseIslandSort",            /* module name           */
	"Vladimir Tishchenko",         /* author                */
	module_event,                  /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	NULL,                          /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

static bool  pulse_islands_t_comp(TPulseIsland *a, TPulseIsland *b);

/** This method sorts pulses in times
 */
INT module_event(EVENT_HEADER *pheader, void *pevent)
{
  // Some typedefs
  typedef std::map<std::string, std::vector<TPulseIsland*> > TStringPulseIslandMap;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map = gData->fPulseIslandToChannelMap;

  for (std::map<std::string, std::vector<TPulseIsland*> >::iterator it=pulse_islands_map.begin(); it!=pulse_islands_map.end(); ++it)
    {
      std::vector<TPulseIsland*>& v = it->second;
      std::sort(v.begin(), v.end(), pulse_islands_t_comp);
# if 0
      // check time ordering
      for (int i=1; i<v.size(); i++)
	{
	  if ( v[i]->GetPulseTime() < v[i-1]->GetPulseTime())
	    {
	      std::cout <<  it->first << " : time is out of order :" << v[i-1]->GetPulseTime() << " " <<  v[i]->GetPulseTime() << "(in v)" << std::endl;
	    }
	}
#endif
    }

  return SUCCESS;
}


/**
 *   Ordering of pulse islands in time
 */
bool  pulse_islands_t_comp(TPulseIsland *a, TPulseIsland *b) 
{ 
  return (a->GetPulseTime()<b->GetPulseTime()); 
}
