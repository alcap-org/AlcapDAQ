////////////////////////////////////////////////////////////////////////////////
/// \defgroup MSIS3300ProcessRaw
/// \ingroup process_raw
/// \author Volodya Tishchenko
///
/// \brief
/// Produces TPIs from raw data read in from Struck SIS3300 digitizer.
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
#include <TDirectory.h>

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
static INT  module_bor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

/// \brief
/// Number of channels in SIS3300.
static const int sis3300_n_channels = 8;
static const int sis3300_n_boards   = 6;
static map<std::string, TH1D*> h1_Err_map;       // Errors
static std::vector<std::string> sis3300_bank_names;

ANA_MODULE MSIS3300ProcessRaw_module =
{
  "MSIS3300ProcessRaw",          /* module name           */
  "Vladimir Tishchenko",         /* author                */
  module_event,                  /* event routine         */
  module_bor,                    /* BOR routine           */
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
  // save current directory 
  //  TDirectory *dir0 = gDirectory;

  //  TDirectory *dir = dir0->mkdir("MSIS3300ProcessRaw");
  //  dir->cd();

  for (unsigned int iboard=0; iboard<sis3300_n_boards; ++iboard)
    {

      std::string bankname( Form("SIS3300_B%d",iboard+1) );
      
      TH1D *h1_Err = new TH1D(Form("h1_Errors_%s",bankname.c_str()),Form("Errors in board, %s",bankname.c_str()),1024,-0.5,1023.5);      
      h1_Err->SetXTitle("Error number");
      h1_Err_map[bankname] = h1_Err;
	  
    }

  // restore pointer of global directory
  //  dir0->cd();

  return SUCCESS;
}

INT module_bor(INT run_number) 
{
  
  for (unsigned int iboard=0; iboard<sis3300_n_boards; ++iboard)
    {

      std::string bankname( Form("SIS3300_B%d",iboard+1) );
      h1_Err_map[bankname]->Reset();
	  
    }


  return SUCCESS;
}


/*-- module event routine -----------------------------------------*/
INT module_event(EVENT_HEADER *pheader, void *pevent)
{
  //printf("===================> %s MIDAS event %i\n",__FILE__,pheader->serial_number);

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  std::map< std::string, std::vector<TPulseIsland*> >& pulse_islands_map = gData->fPulseIslandToChannelMap;

  // Clear out any previous events
  for (std::map< std::string, std::vector<TPulseIsland*> >::iterator iter = pulse_islands_map.begin(); iter != pulse_islands_map.end(); ++iter) 
    {
      if ( iter->first[0] == 'S' &&
	   iter->first[1] == 'I' && 
	   iter->first[2] == 'S' && 
	   iter->first[3] == '3' && 
	   iter->first[4] == '3' && 
	   iter->first[5] == '0' && 
	   (iter->first[6] == '0' || iter->first[6] == '1') ) 
	{
	  std::vector<TPulseIsland*>& islands = iter->second;
	  for (int i = 0; i < islands.size(); ++i) 
	    {
	      if (islands[i]) 
		{
		  delete islands[i];
		  islands[i] = NULL;
		}
	    }
	  islands.clear();
	}
    }

  // Get the event number
  int midas_event_number = pheader->serial_number;

  BYTE *pdata;

  for (int iboard=0; iboard<sis3300_n_boards; iboard++)
    {

      char bank_name[32];
      sprintf(bank_name,"S30%i", iboard+1);

      unsigned int bank_len = bk_locate(pevent, bank_name, &pdata);
      
      if ( pdata == NULL ) continue;
      
      uint32_t *p32   = (uint32_t*)pdata;
      uint32_t *p32_0 = (uint32_t*)pdata;

      if ( bank_len < 4 ) continue;

      // module ID
      uint32_t board_ID = *p32++;
      //      printf("board ID: 0x%08x\n", board_ID);
      
      uint32_t board_type = board_ID>>16;
      //      printf("board type: 0x%08x\n", board_type);
      uint32_t adc_mask = 0xFFF;
      uint32_t ovfw_bit = (1<<12);
      if ( board_type == 0x3301 )
	{
	  adc_mask = 0x3FFF;
	  ovfw_bit = (1<<14);
	}

      std::string bankname( Form("SIS3300_B%d",iboard+1) );
      TH1D *h1_Err = h1_Err_map.find(bankname)->second;

      
      // wf length (samples)
      int wf_len = *p32++;
      //      printf("SIS3300 waveform length: %i\n", wf_len);

      // number of events (triggers)
      int n_events =  *p32++;
      //      printf("SIS3300 number of triggers (events): %i\n", n_events);

      // Errors
      uint32_t errors = *p32++;
      //      printf("Errors: 0x%08x\n", errors);
      if ( errors )
	{	  
	  for (int bit=0; bit<8*sizeof(errors); bit++)
	    {
	      u_int32_t mask = 1<<bit;
	      if ( (errors&mask) == mask )
		h1_Err->Fill( bit );
	    }
	}


      // time stamps
      u_int32_t *p32_timestamps = p32;
      
      // trigger event directory
      u_int32_t *p32_triggers = p32+n_events;

      // ADC data
      u_int32_t adc_data_bytes = sizeof(u_int32_t)*n_events*wf_len;
      //      printf("SIS3300 ADC data len: %i (bytes)\n", adc_data_bytes);

      u_int32_t adc_data_lwords = n_events*wf_len; 
      //      printf("SIS3300 ADC data len: %i (32-bit-long words)\n", adc_data_lwords);

      u_int16_t *p16_ADC[sis3300_n_channels];
      
      // ADC 2 
      p16_ADC[1] = (u_int16_t*)(p32+n_events*2);
      // ADC 1
      p16_ADC[0] = p16_ADC[1]+1;

      // ADC 4 
      p16_ADC[3] = (u_int16_t*)(p32+n_events*2+adc_data_lwords);
      // ADC 3
      p16_ADC[2] = p16_ADC[3]+1;

      // ADC 6 
      p16_ADC[5] = (u_int16_t*)(p32+n_events*2+adc_data_lwords*2);
      // ADC 5
      p16_ADC[4] = p16_ADC[5]+1;

      // ADC 8 
      p16_ADC[7] = (u_int16_t*)(p32+n_events*2+adc_data_lwords*3);
      // ADC 7
      p16_ADC[6] = p16_ADC[7]+1;


      /*
      u_int16_t *p16_ADC2 = p32+n_events*2;
      u_int16_t *p16_ADC1 = p16_ADC2+1;

      u_int16_t *p16_ADC2 = p32+n_events*2;
      u_int16_t *p16_ADC1 = p16_ADC2+1;
      */
      
      for (unsigned int i=0; i<n_events; i++)
	{
	  
	  u_int32_t time = (*p32_timestamps++)&0xFFFFFF;
	  u_int32_t trigger = *p32_triggers++;
	  u_int32_t trigger_mask = trigger>>24;
	  u_int32_t trigger_sample = (trigger & 0x1FFFF) - i*wf_len;
	  if ( trigger_sample > wf_len )
	    {
	      printf("*** ERROR! invalid sample end address: 0x%04x\n",trigger_sample);
	      trigger_sample = 0;
	      h1_Err->Fill( 100 );
	    }
	  //printf("event %i time: 0x%08x trigger 0x%08x mask 0x%x\n",i, time,trigger, trigger_mask);	  

	  bool wraparound = ((trigger>>19)&1);
	  if ( ! wraparound ) time += wf_len - trigger_sample + 1;

	  int this_wf_len = wf_len;
	  if ( ! wraparound ) this_wf_len = trigger_sample;

	  int start_sample = 0;
	  if ( wraparound ) start_sample = trigger_sample;

	  for (unsigned int ich=0; ich<sis3300_n_channels; ich++)
	    {
	      std::vector<int> sample_vector;
	      for (unsigned int k=0; k<this_wf_len; ++k)
		{
		  int sample_nr = start_sample + k;
		  if ( sample_nr >= wf_len ) sample_nr -= wf_len;
		  //sample_nr = k;
		  //if ( i == 2 && ich == 0) printf("sample nr: %i\n",sample_nr);
		  //u_int16_t w16 = (p16_ADC[ich])[(i*wf_len+k)*2];
		  u_int16_t w16 = (p16_ADC[ich])[(i*wf_len+sample_nr)*2];
		  int adc  = w16 & adc_mask;
		  int ovfw = w16 & ovfw_bit;
		  //if ( i == 2 && ich == 0) printf("sample nr: %i adc = %i\n",sample_nr, adc);

		  sample_vector.push_back(adc);
		}
	      char bankname[32];
	      if (board_type == 0x3301) 
		{
		  sprintf(bankname, "SIS3301_B%dC%d", iboard+1, ich+1);
		}
	      else 
		{
		  sprintf(bankname, "SIS3300_B%dC%d", iboard+1, ich+1);
		}
	      if (midas_event_number == 1 && i==0) {
		sis3300_bank_names.push_back(bankname);
	      }
	      std::vector<TPulseIsland*>& pulse_islands = pulse_islands_map[bankname];
	      //if ( i == 2 )
	      //if ( trigger_mask == 0x80 && wraparound )
	      //if ( trigger_mask == 0x80 && wraparound )
	      //if ( ! wraparound )
	      //if ( i == 1 )
	      //if ( trigger_mask == 0x80 && wraparound && n_events == 3 )
	      // Don't record WF unless it triggered
	      if ( trigger_mask & (1<<(7-ich)) )
		pulse_islands.push_back(new TPulseIsland(time, sample_vector, bankname));
	    }

#if 0
	  // only 1st N events
	  if ( i>=5 )
	    break;
#endif


	  /*
	  for (int k=0; k<wf_len; k++)
	    {
	      u_int16_t adc_1 = (p16_ADC[0])[2*k];
	      u_int16_t adc_2 = (p16_ADC[1])[2*k];
	      u_int16_t adc_3 = (p16_ADC[2])[2*k];
	      u_int16_t adc_4 = (p16_ADC[3])[2*k];
	      u_int16_t adc_5 = (p16_ADC[4])[2*k];
	      u_int16_t adc_6 = (p16_ADC[5])[2*k];
	      u_int16_t adc_7 = (p16_ADC[6])[2*k];
	      u_int16_t adc_8 = (p16_ADC[7])[2*k];
	      //printf("Sample %5i: %8i\n",k, adc_1&0xFFF);
	      printf("Sample %5i: %8i %8i %8i %8i %8i %8i %8i %8i\n",k,adc_1&0xFFF,adc_2&0xFFF,adc_3&0xFFF,adc_4&0xFFF,adc_5&0xFFF,adc_6&0xFFF,adc_7&0xFFF,adc_8&0xFFF );
	      }
	      */
	}
    }



#if 1
  // print for testing
  if(midas_event_number == 1) {
    // Loop through all the banks and print an output (because this ProcessRaw loops through pulses then banks, it has been put here)
    for (std::vector<std::string>::iterator bankNameIter = sis3300_bank_names.begin();
      	 bankNameIter != sis3300_bank_names.end(); bankNameIter++) {

      vector<TPulseIsland*>& pulse_islands = pulse_islands_map[*(bankNameIter)];
      printf("TEST MESSAGE: Read %d events into bank %s in event %d\n",
      	     pulse_islands.size(),
      	     bankNameIter->c_str(),
      	     midas_event_number);
    }
  }
#endif

  return SUCCESS;
}

/// @}
