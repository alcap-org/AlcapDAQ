////////////////////////////////////////////////////////////////////////////////
/// \defgroup MCAENPulseIslandSortAndStitch
/// \ingroup process_raw
/// \author John R Quirk
///
/// \brief
/// Sort and stitches the TPIs
///
/// \details
/// First sorts pulses based on time stamp (which is the timing that should be
/// used). Then, if of two consecutive pulses the second is shorter than
/// expected, stitch it to the first.
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

/* ROOT includes */

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"


/*-- Module declaration --------------------------------------------*/
static INT module_event_sort_and_stitch(EVENT_HEADER *pheader, void *pevent);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

ANA_MODULE MCAENPulseIslandSortAndStitch_module =
{
	"MCAENPulseIslandSortAndStitch", /* module name           */
	"Vladimir Tishchenko",           /* author                */
	module_event_sort_and_stitch,    /* event routine         */
	NULL,                            /* BOR routine           */
	NULL,                            /* EOR routine           */
	NULL,                            /* init routine          */
	NULL,                            /* exit routine          */
	NULL,                            /* parameter structure   */
	0,                               /* structure size        */
	NULL,                            /* initial parameters    */
};

static bool pulse_islands_t_comp(const TPulseIsland*, const TPulseIsland*);
static void pulse_islands_stitch(std::vector<TPulseIsland*>&);

/// \brief
/// This method sorts pulses in time and then stitches them together.
INT module_event_sort_and_stitch(EVENT_HEADER *pheader, void *pevent) {
  // Some typedefs
  typedef std::map<std::string, std::vector<TPulseIsland*> > TStringPulseIslandMap;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map = gData->fPulseIslandToChannelMap;

  for (std::map<std::string, std::vector<TPulseIsland*> >::iterator it=pulse_islands_map.begin(); it!=pulse_islands_map.end(); ++it) {
    const std::string& bank = it->first;
    if (bank[0] == 'D' && (bank[1] == '4' || bank[1] == '7')) {
      if (!pulse_islands_map.count(bank)) continue;
      std::vector<TPulseIsland*>& pulses = pulse_islands_map[bank];
      std::sort(pulses.begin(), pulses.end(), pulse_islands_t_comp);
      pulse_islands_stitch(pulses);
    }
  }

  return SUCCESS;
}


/// \brief
/// Ordering of pulse islands in time base on timestamp.
bool pulse_islands_t_comp(const TPulseIsland* a, const TPulseIsland* b) {
  return (a->GetTimeStamp() < b->GetTimeStamp()); 
}

/// \brief
/// Stitch islands together if second of two that are time ordered
/// is less than expected.
///
/// \details
/// If a pulse form the CAEN is less than expected, where what's expected is
/// set in the ODB before a run, that \e usually means that this second
/// pulse was cause by trigger pile-up of the first, and should be stitched
/// onto the end.
///
/// \param[in] pulses Vector of TPIs contained time-sorted CANE pulses to stitch
/// if necessary
/// \param[in] nSamples Number of samples we expect a pulse to be no shorter
/// than; if shorter than this, a pulse is stitched to the end of the
/// preceding pulse.
///
/// \todo Instead stitch together if the timestamps line up appropriately.
void pulse_islands_stitch(std::vector<TPulseIsland*>& pulses) {
  if (pulses.size() == 0) return;
  
  std::vector<bool> merge(pulses.size(), false);
  for (int i = 1; i < pulses.size(); ++i)
    merge[i] =
      pulses[i]->GetPulseLength() ==
      pulses[i]->GetTimeStamp() - pulses[i-1]->GetTimeStamp();

  for (int i = 0; i < pulses.size()-1; ++i) {
    if (!merge[i+1]) continue;
    const std::vector<int>& s0 = pulses[i]  ->GetSamples();
    const std::vector<int>& s1 = pulses[i+1]->GetSamples();
    std::vector<int> samps(s0);
    samps.insert(samps.end(), s1.begin(), s1.end());
    pulses[i]->SetSamples(samps.begin(), samps.end());
    delete pulses[i+1];
    pulses.erase(pulses.begin()+i+1);
    merge.erase(merge.begin()+i+1);
  }
}
/// @}
