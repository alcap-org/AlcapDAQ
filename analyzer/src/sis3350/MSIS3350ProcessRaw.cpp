////////////////////////////////////////////////////////////////////////////////
/// \defgroup MSIS3350ProcessRaw
/// \ingroup process_raw
/// \author Volodya Tishchenko
///
/// \brief
/// Produces TPIs from raw data read in from Struck SIS3350 digitizer.
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
/// Number of channels in SIS3350.
static const int sis3350_n_channels = 4;
static const int sis3350_n_boards   = 2;
static map<std::string, TH1D*> h1_Err_map;       // Errors


ANA_MODULE MSIS3350ProcessRaw_module =
{
  "MSIS3350ProcessRaw",          /* module name           */
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

  //  TDirectory *dir = dir0->mkdir("MSIS3350ProcessRaw");
  //  dir->cd();

  for (unsigned int iboard=0; iboard<sis3350_n_boards; ++iboard)
    {
      for (unsigned int i_ch=0; i_ch<sis3350_n_channels; ++i_ch)
	{
	  std::string bankname( Form("SIS3350_B%dC%d",iboard+1, i_ch+1) );
	  
	  TH1D *h1_Err = new TH1D(Form("h1_Errors_%s",bankname.c_str()),Form("Errors in board, %s",bankname.c_str()),1024,-0.5,1023.5);      
	  h1_Err->SetXTitle("Error number");
	  h1_Err_map[bankname] = h1_Err;
	}
    }

  // restore pointer of global directory
  //  dir0->cd();

  return SUCCESS;
}

INT module_bor(INT run_number) 
{
  // Clear histograms at the start of each run
  for (unsigned int iboard=0; iboard<sis3350_n_boards; ++iboard)
    {
      for (unsigned int i_ch=0; i_ch<sis3350_n_channels; ++i_ch)
	{
	  std::string bankname( Form("SIS3350_B%dC%d",iboard+1, i_ch+1) );
	  
	  h1_Err_map[bankname]->Reset();
	}
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
	   iter->first[5] == '5' && 
	   iter->first[6] == '0' ) 
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

  for (int iboard=0; iboard<sis3350_n_boards; iboard++)
    {      
      for (int i_ch=0; i_ch<sis3350_n_channels; i_ch++)
	{
	  
	  std::string bankname( Form("SIS3350_B%dC%d",iboard+1, i_ch+1) );
	  TH1D *h1_Err = h1_Err_map.find(bankname)->second;

	  char bank_name[32];
	  sprintf(bank_name,"S5%i%i", iboard+1, i_ch+1); 
	  
	  unsigned int bank_len = bk_locate(pevent, bank_name, &pdata);
	  //	  printf("bank [%s] length %i\n",bank_name, bank_len);
	  
	  if ( pdata == NULL ) continue;

	  
	  uint32_t *p32   = (uint32_t*)pdata;
	  uint32_t *p32_0 = (uint32_t*)pdata;
	  
	  if ( bank_len < 4 ) continue;	    
 
	  // data size
	  u_int32_t data_size = *p32++;
	  //	  printf("SIS3350 Bank [%s] adc_data_size: %i 16-bit words\n", bank_name, data_size);

	  // Error mask
	  u_int32_t errors = *p32++;
	  //	  printf("                  errors: 0x%08x\n", errors);

	  if ( errors )
	    {	      
	      for (int bit=0; bit<8*sizeof(errors); bit++)
		{
		  u_int32_t mask = 1<<bit;
		  if ( (errors&mask) == mask )
		      h1_Err->Fill( bit );
		}
	    }

	  // ADC data start here
	  // check data size for consistency 
	  if ( data_size/2 != (bank_len - (p32 - p32_0)) )
	    {
	      printf("***ERROR! Corrupted ADC data in bank [%s]\n",bank_name);
	      h1_Err->Fill( 100 );
	      continue;
	    }
	  
	  while ( (p32 + 4 - p32_0) < bank_len )
	    {	  
	      // time stamp
	      u_int64_t time_1 = *p32++;
	      u_int64_t time_2 = *p32++;
	      u_int64_t time = (time_2 & 0x00000FFF) + ((time_2 & 0x0FFF0000)>>4) + 
		((time_1 & 0x00000FFF)<<24) + ((time_1 & 0x0FFF0000)<<20);
	      //printf("     -> time %Li\n", time);
	      
	      // wf length
	      u_int64_t wf_len_1 = *p32++;
	      u_int64_t wf_len_2 = *p32++;
	      u_int64_t wf_len = (wf_len_2 & 0x00000FFF) + ((wf_len_2 & 0x0FFF0000)>>4) + 
		((wf_len_1 & 0x00000007)<<24);
	      //printf("     -> wf_len %Li\n", wf_len);
	      
	      // Information word
	      u_int32_t info =  (wf_len_1>>16) & 0x0FFF;
	      u_int32_t extra_header = info & 0xF;
	      u_int32_t trigger_counter = (info>>4) & 0xF;
	      u_int32_t stop_delay_counter = (info>>8) & 0x3;
	      u_int32_t w_reserved = (info>>9) & 0x1;
	      u_int32_t wrap = (info>>11) & 0x1;
#if 0
	      printf("     -> info 0x%08x\n", info);
	      printf("     -> extra_header 0x%08x\n", extra_header);
	      printf("     -> trigger_counter 0x%08x\n", trigger_counter);
	      printf("     -> stop_delay_counter 0x%08x\n", stop_delay_counter);
	      printf("     -> w_reserved 0x%08x\n", w_reserved);
	      printf("     -> wrap 0x%08x\n", wrap);
#endif	  
	      
	      if ( wf_len < 8 || (wf_len%2) )
		{
		  printf("***ERROR! Bad SIS3350 ADC data. Bank [%s]\n",bank_name);
		  h1_Err->Fill( 101 );
		  break;
		}
	      
	      
	      // record samples
	      std::vector<int> sample_vector;
	      for (unsigned int k=0; k<wf_len; k+=2)
		{
		  int adc;
		  u_int32_t d32 = *p32++;
		  
		  adc = (d32&0xFFFF);
		  sample_vector.push_back(adc);
		  
		  adc = ((d32>>16)&0xFFFF);
		  sample_vector.push_back(adc);
		}
	      char bankname[32];
	      sprintf(bankname, "SIS3350_B%dC%d", iboard+1, i_ch+1);
	      std::vector<TPulseIsland*>& pulse_islands = pulse_islands_map[bankname];
	      pulse_islands.push_back(new TPulseIsland(time*2.0, sample_vector, bankname));
	    }
	  
	  // print for testing
	  if(midas_event_number == 1) {
	    // Loop through all the banks and print an output (because this ProcessRaw loops through pulses then banks, it has been put here)
	    char bankname[32];
	    sprintf(bankname, "SIS3350_B%dC%d", iboard+1, i_ch+1);
	    vector<TPulseIsland*>& pulse_islands = pulse_islands_map[bankname];
	    printf("TEST MESSAGE: Read %d events from bank %s to %s in event %d\n",
		   pulse_islands.size(),
		   bank_name,
		   bankname,
		   midas_event_number);
	  }
	}

    }

  return SUCCESS;
}

/// @}
