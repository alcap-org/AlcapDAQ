////////////////////////////////////////////////////////////////////////////////
/// \defgroup MDT5720ProcessRaw
/// \ingroup process_raw
/// \author Volodya Tishchenko
/// \author Joe Grange
///
/// \brief
/// Produces TPIs from raw data read in from BU CAEN.
/// @{
////////////////////////////////////////////////////////////////////////////////

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
static INT  module_event_caen(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// List of BU CAEN bank names for the event loop.
static vector<string> caen_boston_bank_names;
/// \brief
/// Number of samples to record before a trigger.
/// \details
/// At the moment this is \b hard \b coded into the ::module_init function
/// instead of grabbed from the ODB. The way the CAEN should work is that
/// a percentage of the total sampling window is set to be the "after trigger".
/// However, it does \e not seem like this was set correctly in the front end.
/// \todo
/// Find out the problem and characterize.
static unsigned int nPreSamples;
/// \brief
/// Number of channels in DT5720.
static const int NCHAN = 4;

ANA_MODULE MDT5720ProcessRaw_module =
{
  "MDT5720ProcessRaw",            /* module name           */
  "Vladimir Tishchenko",         /* author                */
  module_event_caen,                  /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  module_init,                   /* init routine          */
  NULL,                          /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

/*--module init routine --------------------------------------------*/
INT module_init()
{

  std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); 
      mapIter != bank_to_detector_map.end(); mapIter++) { 
    
    std::string bankname = mapIter->first;
    
    // We only want the CAEN banks here
    if (TSetupData::IsBostonCAEN(bankname))
      caen_boston_bank_names.push_back(bankname);
  }
  
  /*** Get necessary data from ODB ***/
  char key[80];
  int size;
  unsigned int post_trigger_percentage, nSamples;


  // Get Trigger Time Tag info
  // Timestamp will be shifted by number of presamples

  sprintf(key, "/Equipment/Crate 5/Settings/CAEN/waveform length");
  size = sizeof(nSamples);
  db_get_value(hDB, 0, key, &nSamples, &size, TID_DWORD, 1);
  sprintf(key, "/Equipment/Crate 5/Settings/CAEN/post_trigger_size");
  size = sizeof(post_trigger_percentage);
  db_get_value(hDB, 0, key, &post_trigger_percentage, &size, TID_BYTE, 1);
  //nPreSamples = (int) (0.01 * ((100 - post_trigger_percentage) * nSamples));
  nPreSamples = 20; // From the Golden Data, it looks like there are 20 presamples.
                    // The frontend does not seem to correctly load post_trigger_size
                    // onto the CAEN.
  
  return SUCCESS;
}

/*-- module event routine -----------------------------------------*/
INT module_event_caen(EVENT_HEADER *pheader, void *pevent)
{
  // Some typedefs
  
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

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
  sprintf(bank_name,"CND%i",0); // one MIDAS bank per board
  unsigned int bank_len = bk_locate(pevent, bank_name, &pdata);

  //  printf("MIDAS bank [%s] size %d ----------------------------------------\n",bank_name,bank_len);

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
      //      printf("CW: %08x\n",caen_event_cw);
      if ( caen_event_cw != 0xA )
	{
	  printf("***ERROR! Wrong data format it dt5720: incorrect control word 0x%08x\n", caen_event_cw);
	  return SUCCESS;
	}
      
      uint32_t caen_event_size = p32[0] & 0x0FFFFFFF;
      //      printf("caen event size: %i\n",caen_event_size);
      
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
      
      // PW: There seems to be a factor of 2 missing between the timestamp and real timestamp
      uint32_t caen_trigger_time = 2*p32[3];
      //      printf("caen trigger time: %i\n",caen_trigger_time);// = clock ticks?
      
      // number of samples per channel
      unsigned int nsamples = ((caen_event_size-4)*2) / nchannels;
      //      printf("waveform length: %i\n",nsamples);

      // Loop through the channels (i.e. banks)
      for (std::vector<std::string>::iterator bankNameIter = caen_boston_bank_names.begin();
	   bankNameIter != caen_boston_bank_names.end(); bankNameIter++) {
        
	vector<TPulseIsland*>& pulse_islands = pulse_islands_map[*(bankNameIter)];
	std::vector<int> sample_vector;
	int ichannel = bankNameIter - caen_boston_bank_names.begin();

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
		      
		    //printf("CAEN DT5720 channel %d: adc[%i] = %i\n", ichannel, isample, adc);
		    //		    h2_v1724_pulses[ichannel]->Fill(isample,adc);
		    isample++;
		      
		    sample_vector.push_back(adc);

		  }
	      }
              
	    pulse_islands.push_back(new TPulseIsland(caen_trigger_time - nPreSamples, sample_vector, *bankNameIter));
	  }
      }

      // *** updated by VT: align data by 32bit 
      p32 += caen_event_size + (caen_event_size%2);
      //      printf("offset: %i bank size: %i\n", (int)(p32-p32_0), bank_len);

    }
    
  // print for testing
  if(midas_event_number == 1) {
    // Loop through all the banks and print an output (because this ProcessRaw loops through pulses then banks, it has been put here)
    for (std::vector<std::string>::iterator bankNameIter = caen_boston_bank_names.begin();
	 bankNameIter != caen_boston_bank_names.end(); bankNameIter++) {
      
      vector<TPulseIsland*>& pulse_islands = pulse_islands_map[*(bankNameIter)];
      printf("TEST MESSAGE: Read %d events from bank %s in event %d\n",
	     pulse_islands.size(),
	     (*(bankNameIter)).c_str(),
	     midas_event_number);
    }
  }

  return SUCCESS;
}

/// @}
