////////////////////////////////////////////////////////////////////////////////
/// \defgroup MCAENPulseIslandStitch
/// \ingroup process_raw
/// \author John R Quirk
///
/// \brief
/// Stitches the TPIs
/// @{
////////////////////////////////////////////////////////////////////////////////

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

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::vector;
using std::map;
using std::string;

/*-- Module declaration --------------------------------------------*/
static INT MCAENPulseIslandStitch(EVENT_HEADER *pheader, void *pevent);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

ANA_MODULE MCAENPulseIslandStitch_module =
{
  "MCAENPulseIslandStitch", /* module name           */
  "John R Quirk",           /* author                */
  MCAENPulseIslandStitch,   /* event routine         */
  NULL,                     /* BOR routine           */
  NULL,                     /* EOR routine           */
  NULL,                     /* init routine          */
  NULL,                     /* exit routine          */
  NULL,                     /* parameter structure   */
  0,                        /* structure size        */
  NULL,                     /* initial parameters    */
};

static void stitch(std::vector<TPulseIsland*>&);

/// \brief
/// This method sorts pulses in time and then stitches them together.
INT MCAENPulseIslandStitch(EVENT_HEADER *pheader, void *pevent) {
  map< string, vector<TPulseIsland*> >& wfd_map =
    gData->fPulseIslandToChannelMap;

  for (map<string, vector<TPulseIsland*> >::iterator i = wfd_map.begin(),
	 e = wfd_map.end(); i != e; ++i) {
    const std::string& bank = i->first;
    if (bank[0] == 'D' &&
	(bank[1] == '4' || bank[1] == '5' ||
	 bank[1] == '7' || bank[1] == '8'))
      stitch(i->second);
  }

  return SUCCESS;
}

void stitch(vector<TPulseIsland*>& pulses) {
  std::vector<bool> merge(pulses.size(), false);
  for (int i = 1; i < pulses.size(); ++i)
    merge[i] =
      pulses[i]->GetPulseLength() ==
      pulses[i]->GetTimeStamp() - pulses[i-1]->GetTimeStamp();

  for (int i = 0; i < (int)pulses.size()-1; ++i) {
    if (!merge[i+1]) continue;
    const std::vector<int>& s0 = pulses[i]  ->GetSamples();
    const std::vector<int>& s1 = pulses[i+1]->GetSamples();
    std::vector<int> samps(s0);
    samps.insert(samps.end(), s1.begin(), s1.end());
    pulses[i]->SetSamples(samps.begin(), samps.end());
    delete pulses[i+1];
    pulses.erase(pulses.begin()+i+1);
    merge.erase(merge.begin()+i+1);
    --i;
  }
}
/// @}
