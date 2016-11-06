////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTDCSort
/// \author John Quirk
///
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <string>
#include <vector>
#include <map>
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* AlCap includes */
#include "TGlobalData.h"

using std::string;
using std::map;
using std::vector;

/*-- Module declaration --------------------------------------------*/
static INT MTDCSort(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;

ANA_MODULE MTDCSort_module =
{
  "MTDCSort",     /* module name           */
  "John R Quirk", /* author                */
  MTDCSort,       /* event routine         */
  NULL,           /* BOR routine           */
  NULL,           /* EOR routine           */
  NULL,           /* init routine          */
  NULL,           /* exit routine          */
  NULL,           /* parameter structure   */
  0,              /* structure size        */
  NULL,           /* initial parameters    */
};

INT MTDCSort(EVENT_HEADER *pheader, void *pevent) {
  map< string, vector<int64_t> >& tdc_map =
    gData->fTDCHitsToChannelMap;
  for (map< string, vector<int64_t> >::iterator i = tdc_map.begin(),
	 e = tdc_map.end(); i != e; ++i)
    std::sort(i->second.begin(), i->second.end());

  return SUCCESS;
}

/// @}
