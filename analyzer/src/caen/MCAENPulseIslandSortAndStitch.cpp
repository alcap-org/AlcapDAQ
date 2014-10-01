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
static INT module_init_sort_and_stitch();

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// Number of samples we expect a UH CAEN pulse to be no shorter than
/// (from ODB).
static unsigned int nUHSamples;
/// \brief
/// Number of samples we expect a BU CAEN pulse to be no shorter than
/// (from ODB).
static unsigned int nBUSamples;
/// \brief
/// Number of samples saved before a trigger on UH CAEN
/// (from ODB).
static unsigned int nUHPreSamples;
/// \brief
/// Number of samples saved before a trigger on BU CAEN
/// (from ODB).
static unsigned int nBUPreSamples;

ANA_MODULE MCAENPulseIslandSortAndStitch_module =
{
	"MCAENPulseIslandSortAndStitch",   /* module name           */
	"Vladimir Tishchenko",             /* author                */
	module_event_sort_and_stitch,      /* event routine         */
	NULL,                              /* BOR routine           */
	NULL,                              /* EOR routine           */
	module_init_sort_and_stitch,       /* init routine          */
	NULL,                              /* exit routine          */
	NULL,                              /* parameter structure   */
	0,                                 /* structure size        */
	NULL,                              /* initial parameters    */
};

static bool pulse_islands_t_comp(TPulseIsland *a, TPulseIsland *b);
static void pulse_islands_stitch(std::vector<TPulseIsland*>& v, unsigned int nSamples);


INT module_init_sort_and_stitch() {

   /*** Get necessary data from ODB ***/
  char key[80];
  int size;
  unsigned int post_trigger_percentage;

  sprintf(key, "/Equipment/Crate 4/Settings/CAEN0/waveform length");
  size = sizeof(nUHSamples);
  db_get_value(hDB, 0, key, &nUHSamples, &size, TID_DWORD, 1);
  post_trigger_percentage = 80; // This is hardcoded in the frontend
  nUHPreSamples = (int) (0.01 * ((100 - post_trigger_percentage) * nUHSamples));
  
  return SUCCESS;

  sprintf(key, "/Equipment/Crate 5/Settings/CAEN/waveform length");
  size = sizeof(nBUSamples);
  db_get_value(hDB, 0, key, &nBUSamples, &size, TID_DWORD, 1);
  sprintf(key, "/Equipment/Crate 5/Settings/CAEN/post_trigger_size");
  size = sizeof(post_trigger_percentage);
  db_get_value(hDB, 0, key, &post_trigger_percentage, &size, TID_BYTE, 1);
  //nBUPreSamples = (int) (0.01 * ((100 - post_trigger_percentage) * nBUSamples));
  nBUPreSamples = 20; // From the Golden Data, it looks like there are 20 presamples.
                    // The frontend does not seem to correctly load post_trigger_size
                    // onto the CAEN.
  
  return SUCCESS;
}

/// \brief
/// This method sorts pulses in time and then stitches them together.
INT module_event_sort_and_stitch(EVENT_HEADER *pheader, void *pevent)
{
  // Some typedefs
  typedef std::map<std::string, std::vector<TPulseIsland*> > TStringPulseIslandMap;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map = gData->fPulseIslandToChannelMap;

  for (std::map<std::string, std::vector<TPulseIsland*> >::iterator it=pulse_islands_map.begin(); it!=pulse_islands_map.end(); ++it)
    {
      unsigned int nSamples = 0;
      if (TSetupData::IsBostonCAEN(it->first))
	nSamples = nBUSamples;
      else if (TSetupData::IsHoustonCAEN(it->first))
	nSamples = nUHSamples;
      if (nSamples != 0) {
	std::vector<TPulseIsland*> &v = it->second;
	std::sort(v.begin(), v.end(), pulse_islands_t_comp);
	pulse_islands_stitch(v, nSamples);
      }
    }

  return SUCCESS;
}


/// \brief
/// Ordering of pulse islands in time base on timestamp.
bool pulse_islands_t_comp(TPulseIsland *a, TPulseIsland *b) 
{ 
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
void pulse_islands_stitch(std::vector<TPulseIsland*>& pulses, unsigned int nSamples) {
  unsigned int nPulses = pulses.size();
  if (nPulses == 0) return;
  std::vector<int> next_samples, current_samples;
  TPulseIsland* temp_pulse;
  for (unsigned int iPulse = 0; iPulse < nPulses - 1; ++iPulse) {
    next_samples = pulses[iPulse + 1]->GetSamples();
    // If the next pulse is less than the set number of samples,
    // it's a continuation of this pulse
    while (next_samples.size() < nSamples) {
      current_samples = pulses[iPulse]->GetSamples();
      for (unsigned int i = 0; i < next_samples.size(); ++i)
	current_samples.push_back(next_samples[i]);
      temp_pulse = pulses[iPulse];
      pulses[iPulse] = new TPulseIsland(temp_pulse->GetTimeStamp(), current_samples, temp_pulse->GetBankName());
      delete temp_pulse;
      delete pulses[iPulse + 1];
      pulses.erase(pulses.begin() + iPulse + 1);
      if (!(iPulse < --nPulses - 1)) break;
      next_samples = pulses[iPulse + 1]->GetSamples();
    }
  }
}

/// @}
