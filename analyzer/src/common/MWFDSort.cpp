////////////////////////////////////////////////////////////////////////////////
/// \defgroup MWFDSort
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
static INT MWFDSort(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;

ANA_MODULE MWFDSort_module =
{
  "MWFDSort",     /* module name           */
  "John R Quirk", /* author                */
  MWFDSort,       /* event routine         */
  NULL,           /* BOR routine           */
  NULL,           /* EOR routine           */
  NULL,           /* init routine          */
  NULL,           /* exit routine          */
  NULL,           /* parameter structure   */
  0,              /* structure size        */
  NULL,           /* initial parameters    */
};

bool tpi_less(const TPulseIsland* a, const TPulseIsland* b) {
  return a->GetTimeStamp() < b->GetTimeStamp();
}

INT MWFDSort(EVENT_HEADER *pheader, void *pevent) {
  map< string, vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;

  for (map< string, vector<TPulseIsland*> >::iterator i = wfd_map.begin(),
	 e = wfd_map.end(); i != e; ++i)
    std::sort(i->second.begin(), i->second.end(), tpi_less);

  return SUCCESS;
}

/// @}
