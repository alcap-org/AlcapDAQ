/********************************************************************\

  Name:         MV1724ProcessRaw
  Created by:   V.Tishchenko, J.Grange hoping to extend it

  Contents:     Module to decode CAEN v1724 digitizer data.

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>
#include "TRandom3.h"

//JG: added alcap includes
/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
static INT  module_init(void);
static INT  module_event(EVENT_HEADER*, void*);
/*-- Other functions -----------------------------------------------*/
static bool pulse_islands_sort(TPulseIsland*, TPulseIsland*);
static void pulse_islands_stitch(std::vector<TPulseIsland*>&);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

vector<string> caen_houston_bank_names;
static unsigned int nSamples;    // For stitching
static unsigned int nPreSamples; // For time adjustment

ANA_MODULE MV1724ProcessRaw_module =
{
  "MV1724ProcessRaw",            /* module name           */
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

/*-- Number of channels in V1724 -----------------------------------*/
static const int NCHAN = 8;

/*-- Histogram declaration -----------------------------------------*/
static TH2* hNV1724IslandsReadPerBlock;

/*--module init routine --------------------------------------------*/
INT module_init()
{

  hNV1724IslandsReadPerBlock = new TH2I(
    "hNV1724IslandsReadPerBlock",
    "Number of CAEN Islands read by block",
    1,0,1, 3000,0,3000);
  hNV1724IslandsReadPerBlock->SetBit(TH1::kCanRebin);

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 
    
    std::string bankname = mapIter->first;
    
    // We only want the CAEN banks here
    if (TSetupData::IsHoustonCAEN(bankname))
      caen_houston_bank_names.push_back(bankname);
  }
  
   /*** Get necessary data from ODB ***/
  char key[80];
  int size;
  unsigned int post_trigger_percentage;

  // Get Trigger Time Tag info
  // Timestamp will be shifted by number of presamples
  sprintf(key, "/Equipment/Crate 4/Settings/CAEN0/waveform length");
  size = sizeof(nSamples);
  db_get_value(hDB, 0, key, &nSamples, &size, TID_DWORD, 1);
  post_trigger_percentage = 80; // This is hardcoded in the frontend
  nPreSamples = (int) (0.01 * ((100 - post_trigger_percentage) * nSamples));
  
  return SUCCESS;
}

/*-- module event routine -----------------------------------------*/
INT module_event(EVENT_HEADER *pheader, void *pevent)
{
  // Some typedefs
  
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  //printf("===================> %s MIDAS event %i\n",__FILE__,pheader->serial_number);
  
  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map =
    gData->fPulseIslandToChannelMap;
    
  // Delete the islands found in the previous block. Don't clear
  // the map of (bank_name, vector) pairs. Once we use a bank name
  // once, it will have a (possibly empty) entry in this map for the
  // whole run. JG: the islands.clear() line causes seg fault for > 1 TPulseIslandPair
  /*for(map_iterator iter = pulse_islands_map.begin();
      iter != pulse_islands_map.end(); iter++){
    vector<TPulseIsland*>& islands = iter->second;
    // Delete the pointers to TPulseIslands, then clear the vector
    for(unsigned int j=0; j<islands.size(); j++) {
      if(islands[j]) { delete islands[j]; islands[j] = NULL; }
    }
    islands.clear();
    pulse_islands_map.erase(iter); // AE: need to erase the key so that blocks after the first will be recorded
  }*/    
    
  // Get the event number
  int midas_event_number = pheader->serial_number;

  BYTE *pdata;

  //  printf("In caen ER!\n");

  char bank_name[8];
  sprintf(bank_name,"CDG%i",0); // one MIDAS bank per board
  unsigned int bank_len = bk_locate(pevent, bank_name, &pdata);
  

  uint32_t *p32 = (uint32_t*)pdata;
  uint32_t *p32_0 = (uint32_t*)pdata;

  
  while ( (p32 - p32_0)*4 < bank_len )
    {

      uint32_t caen_event_cw = p32[0]>>28;
      //printf("CW: %08x\n",caen_event_cw);
      if ( caen_event_cw != 0xA )
	{
	  printf("***ERROR UH CAEN! Wrong data format: incorrect control word 0x%08x\n", caen_event_cw);
	  return SUCCESS;
	}
      
      uint32_t caen_event_size = p32[0] & 0x0FFFFFFF;
      //printf("caen event size: %i\n",caen_event_size);
      
      uint32_t caen_channel_mask = p32[1] & 0x000000FF;
      // count the number of channels in the event
      int nchannels = 0;
      for (int ichannel=0; ichannel<NCHAN; ichannel++ )
	{
	  if ( caen_channel_mask & (1<<ichannel) ) nchannels++; 
	}
      
      uint32_t caen_event_counter = p32[2] & 0x00FFFFFF;
      //      printf("caen event counter: %i\n",caen_event_counter);
      
      uint32_t caen_trigger_time = p32[3];
      //      printf("caen trigger time: %i\n",caen_trigger_time);// = clock ticks?
      
      // number of samples per channel
      unsigned int nsamples = ((caen_event_size-4)*2) / nchannels;
      //      printf("waveform length: %i\n",nsamples);

      // Loop through the channels (i.e. banks)
      for (std::vector<std::string>::iterator bankNameIter = caen_houston_bank_names.begin();
	   bankNameIter != caen_houston_bank_names.end(); bankNameIter++) {
        
	vector<TPulseIsland*>& pulse_islands = pulse_islands_map[*(bankNameIter)];
	std::vector<int> sample_vector;
	int ichannel = bankNameIter - caen_houston_bank_names.begin();

	//	printf("TGlobalData bank [%s] ----------------------------------------\n", (*bankNameIter).c_str());
	//	printf("Number of TPulseIslands already existing = %d\n", pulse_islands.size());
	//	printf("*-- channel %i -------------------------------------------------------------*\n",ichannel);

	if ( caen_channel_mask & (1<<ichannel) )
	  {
	    // channel enabled
	    unsigned int isample = 0;
	    unsigned int nwords = nsamples/2;

	    for (int iword=0; iword<nwords; iword++)
	      {
		
		for (int isubword=0; isubword<2; isubword++)
		  {
		    uint32_t adc;
		    
		    if (isubword == 0 )
		      adc = (p32[4+iword+ichannel*nwords] & 0x3fff);
		    else 
		      adc = ((p32[4+iword+ichannel*nwords] >> 16) & 0x3fff);
		      
		    //printf("CAEN V1724 channel %d: adc[%i] = %i\n", ichannel, isample, adc);
		    //		    h2_v1724_pulses[ichannel]->Fill(isample,adc);
		    isample++;
		      
		    sample_vector.push_back(adc);

		  }
	      }
              
	    pulse_islands.push_back(new TPulseIsland(caen_trigger_time - nPreSamples, sample_vector, *bankNameIter));
	  }
      }
    
      // Sort and Stitch Islands
      for (std::vector<std::string>::iterator bankNameIter = caen_houston_bank_names.begin();
	   bankNameIter != caen_houston_bank_names.end(); bankNameIter++) {
	vector<TPulseIsland*>& pulse_islands = pulse_islands_map[*(bankNameIter)];
	std::sort(pulse_islands.begin(), pulse_islands.end(), pulse_islands_sort);
	pulse_islands_stitch(pulse_islands);  
      }

      // align the event by two bytes.
      p32 += caen_event_size + (caen_event_size%2);
      //      printf("offset: %i bank size: %i\n", (int)(p32-p32_0), bank_len);

    }
    
  // print for testing
  if(midas_event_number == 1) {
    // Loop through all the banks and print an output (because this ProcessRaw loops through pulses then banks, it has been put here)
    for (std::vector<std::string>::iterator bankNameIter = caen_houston_bank_names.begin();
	 bankNameIter != caen_houston_bank_names.end(); bankNameIter++) {
      
      vector<TPulseIsland*>& pulse_islands = pulse_islands_map[*(bankNameIter)];
      printf("TEST MESSAGE: Read %d events from bank %s in event %d\n",
	     pulse_islands.size(),
	     (*(bankNameIter)).c_str(),
	     midas_event_number);
    }
  }
  
  return SUCCESS;
}

bool pulse_islands_sort(TPulseIsland *a, TPulseIsland *b) {
  return (a->GetTimeStamp() < b->GetTimeStamp());
}

void pulse_islands_stitch(std::vector<TPulseIsland*>& v) {
  unsigned int nPulses = v.size();
  std::vector<int> next_samples, current_samples;
  TPulseIsland* temp_pulse;
  for (unsigned int iPulse = 0; iPulse < nPulses - 1; ++iPulse) {
    next_samples = v[iPulse + 1]->GetSamples();
    // If the next pulse is less than the set number of samples,
    // it's a continuation of this pulse
    while (next_samples.size() < nSamples) {
      current_samples = v[iPulse]->GetSamples();
      for (unsigned int i = 0; i < next_samples.size(); ++i)
	current_samples.push_back(next_samples[i]);
      temp_pulse = v[iPulse];
      v[iPulse] = new TPulseIsland(temp_pulse->GetTimeStamp(), current_samples, temp_pulse->GetBankName());
      delete temp_pulse;
      delete v[iPulse + 1];
      v.erase(v.begin() + iPulse + 1);
      if (!(iPulse < --nPulses - 1)) break;
      next_samples = v[iPulse + 1]->GetSamples();
    }
  }
}
