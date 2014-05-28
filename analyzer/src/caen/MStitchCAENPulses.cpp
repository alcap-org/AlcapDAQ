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
static INT module_init();

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static unsigned int nUHPreSamples;
static unsigned int nBUPreSamples;
static unsigned int nUHSamples;
static unsigned int nBUSamples;

ANA_MODULE MStitchCAENPulses_module =
{
	"MPulseIslandSort",            /* module name           */
	"Vladimir Tishchenko",         /* author                */
	module_event,                  /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	module_init,                   /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

static bool  pulse_islands_t_comp(TPulseIsland *a, TPulseIsland *b);

INT module_init() {

  /*** Get necessary data from ODB ***/
  char key[80];
  int size;
  int post_trigger_percentage;

  // Get UH CAEN info
  sprintf(key, "/Equipment/Crate 4/Settings/CAEN0/waveform length");
  size = sizeof(nUHSamples);
  db_get_value(hDB, 0, key, &nBUSamples, &size, TID_DWORD, 1);
  post_trigger_percentage = 80;
  nUHPreSamples = (int) (0.01 * ((100 - post_trigger_percentage) * nUHSamples));

  // Get BU info
  sprintf(key, "/Equipment/Crate 5/Settings/CAEN/waveform length");
  size = sizeof(nBUSamples);
  db_get_value(hDB, 0, key, &nBUSamples, &size, TID_DWORD, 1);
  sprintf(key, "/Equipment/Crate 5/Settings/CAEN/post_trigger_size");
  size = sizeof(post_trigger_percentage);
  //db_get_value(hDB, 0, key, &post_trigger_percentage, &size, TID_BYTE, 1);
  //nBUPreSamples = (int) (0.01 * ((100 - post_trigger_percentage) * nBUSamples));
  nBUPreSamples = 20; // From the Golden Data, it looks like there are 20 presamples.

  // Let's make sure we got it right
  printf("nUHSamples: %d\n nBUSamples: %d\n", nUHSamples, nBUSamples);

  return SUCCESS;
}

INT module_event(EVENT_HEADER *pheader, void *pevent)
{
  // Some typedefs
  typedef std::map<std::string, std::vector<TPulseIsland*> > TStringPulseIslandMap;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map = gData->fPulseIslandToChannelMap;

  for (std::map<std::string, std::vector<TPulseIsland*> >::iterator it=pulse_islands_map.begin(); it!=pulse_islands_map.end(); ++it)
    {
      unsigned int nsamples = 0;
      if (TSetupData::IsHoustonCAEN(it->first))
	nsamples = nUHSamples;
      else if (TSetupData::IsBostonCAEN(it->first))
	nsamples = nBUSamples;
      if (nsamples != 0) {
	std::vector<TPulseIsland*>& pulses = it->second;
	unsigned int npulses = pulses.size();
	std::vector<int> next_samples, current_samples;
	TPulseIsland* temp_pulse;
	for (unsigned int ipulse = 0; ipulse < npulses - 1; ++ipulse) {
	  next_samples = pulses[ipulse + 1]->GetSamples(); 
	  while (next_samples.size() < nsamples) {
	    current_samples = pulses[ipulse]->GetSamples();
	    for (int i = 0; i < next_samples.size(); ++i)
	      current_samples.push_back(next_samples[i]);
	    temp_pulse = pulses[ipulse];
	    pulses[ipulse] = new TPulseIsland(temp_pulse->GetTimeStamp(), current_samples, temp_pulse->GetBankName());
	    delete temp_pulse;
	    delete pulses[ipulse + 1];
	    pulses.erase(pulses.begin() + ipulse + 1);
	    if(!(ipulse < --npulses - 1)) break;
	    next_samples = pulses[ipulse + 1]->GetSamples();
	  }
	}
      }
    }

  return SUCCESS;
}  
