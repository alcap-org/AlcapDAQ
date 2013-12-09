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
//double GetClockTickForChannel(string bank_name);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

vector<string> caen_houston_bank_names;
//static vector<TV1724BankReader*> caen_bank_readers;

struct caen_event_t
{
  uint32_t time;
  uint16_t samples[4096];
};

caen_event_t xxx;//JG: should be implemented in event routine?

extern HNDLE hDB;

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
  
  //  printf("caen init!\n");
  
  //for (int i=0; i<bank_names.size(); i++)  printf(" name for bank %d is %s \n",i,bank_names[i].data());
  
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

  //printf("MIDAS bank [%s] size %d ----------------------------------------\n",bank_name,bank_len);

  /* uncomment when have real data
  if ( bank_len == 0 )
    {
      return SUCCESS;
    }
  */
  

  uint32_t *p32 = (uint32_t*)pdata;
  uint32_t *p32_0 = (uint32_t*)pdata;

  
  while ( (p32 - p32_0)*4 < bank_len )
    {

      uint32_t caen_event_cw = p32[0]>>28;
      //printf("CW: %08x\n",caen_event_cw);
      if ( caen_event_cw != 0xA )
	{
	  printf("***ERROR! Wrong data format: incorrect control word 0x%08x\n", caen_event_cw);
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
      //      printf("caen channel mask: 0x%08x (%i)\n",caen_channel_mask,nchannels);
      
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
              
	    
	    pulse_islands.push_back(new TPulseIsland(
						       caen_trigger_time, sample_vector,
						       gSetup->GetClockTick(*bankNameIter), gSetup->GetADCValue(*bankNameIter),  *bankNameIter));
	  }
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

  //  hNV1724IslandsReadPerBlock->Fill(bank_name,midas_event_number,pulse_islands.size());
  
  /* fake data! just to show how to push to a tree: */

  /*  vector<TPulseIsland*> pulse_islands[bank_names.size()];

  TRandom3 *rndm = new TRandom3(0);

  std::string bank_name2;

  int whichChan = 0;

  for (int j=0; j<bank_names.size(); j++){
   
    int adc = 0;
    int tstamp = 0;
    double clock_tick = 0;
    
    std::vector<int> sample_vector[5];
   
    for (int i=0; i<5; i++){
      adc = rndm->Gaus(5+2*j,2);
      sample_vector[0].push_back(adc);
    }
   
    tstamp = 10;
   
    pulse_islands[j].push_back(new TPulseIsland(
    tstamp, sample_vector[0],
    clock_tick, bank_names[j]));
   
    for (int i=0; i<5; i++){
      adc = rndm->Gaus(9+2*j,6);
      sample_vector[1].push_back(adc);
    }
   
    tstamp = 20;
   
    pulse_islands[j].push_back(new TPulseIsland(
    tstamp, sample_vector[1],
    clock_tick, bank_names[j]));
   
    for (int i=0; i<5; i++){
      adc = rndm->Gaus(10+2*j,3);
      sample_vector[2].push_back(adc);
    }
   
    tstamp = 30;
   
    pulse_islands[j].push_back(new TPulseIsland(
    tstamp, sample_vector[2],
    clock_tick, bank_names[j]));

    //print for testing
    if(midas_event_number == 1) {

      //for (int i=0; i<bank_names.size(); i++)  printf(" name for bank %d is %s \n",i,bank_names[i].data());

      printf(" channel %d, sample vector 0 size %d, first element %d\n",j,(int)sample_vector[0].size(),sample_vector[0][0]);
      printf(" channel %d, sample vector 0 size %d, first element %d\n",j,(int)sample_vector[1].size(),sample_vector[1][0]);
      printf(" channel %d, sample vector 0 size %d, first element %d\n",j,(int)sample_vector[2].size(),sample_vector[2][0]);
      //printf(" channel %d, sample vector 1 size %d\n",j,(int)sample_vector[1].size());
      //printf(" channel %d, sample vector 2 size %d\n",j,(int)sample_vector[2].size());
      printf(" channel %d, pulse_islands size %d\n",j,(int)pulse_islands[j].size());
  
    }
    
    bank_name2 = bank_names[j];
    //sprintf(bank_name2.c_str,bank_names[j].data());
    //scanf(" bankname2 is %s\n",bank_name2);

    //if (j==5) 
    pulse_islands_map.insert(TStringPulseIslandPair(bank_name2, pulse_islands[j]));

  }
  */
  //printf("bank_names size %d\n",(int)bank_names.size());

  /* --- */
  
  return SUCCESS;
}
