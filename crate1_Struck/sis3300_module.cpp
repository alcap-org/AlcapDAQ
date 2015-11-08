/**
 * PROGRAM:     sis3300_module.cpp
 * DESCRIPTION: Control and readout of Struck SIS3300 digitizer.
 * author:      V.Tishchenko
 * date:        23-Sept-2015
 *
 *
 * Modifications:
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <stdint.h>
#include <sys/io.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "midas.h"

#include "crate.h"
#include "diag.h"

#include "sis3300.h"
#include "sis3100_module.h"
#include "dev/pci/sis1100_var.h"  
#include "sis3100_calls/sis3100_vme_calls.h"

#include "sis3300_odb.cpp"
#include "sis3300_tools.cpp"
#ifdef SINGLE_BANK_MODE_DEF
#include "sis3300_tools_onebank.cpp"
//#include "sis3300_tools_onebank_gatechaining.cpp"
#endif
#ifdef AUTOBANK_SWITCH_MODE_DEF
#include "sis3300_tools_twobanks.cpp"
#endif

// Struck includes

static INT  module_init(void);
static void module_exit(void);
static INT  module_bor(void);
static INT  module_eor(void);
static INT  module_poll_live(void);
static INT  module_read(char *pevent); // MIDAS readout routine

struct readout_module sis3300_module = {
  module_init,             // init
  module_exit,             // exit
  NULL,                    // pre_bor
  module_bor,              // bor
  module_eor,              // eor
  module_poll_live,        // poll_live
  NULL,                    // poll_dead
  NULL,                    // start_block
  NULL,                    // stop_block
  module_read              // read
};

INT module_init() 
{
  INT status = SUCCESS;
  printf("%s module_init()\n", __FILE__ );

  // ODB Link
  if ( sis3300_odb_init() != SUCCESS ) return FE_ERR_HW;

  // Open SIS3300 digitizer(s)
  if ( sis3300_open() != SUCCESS ) return FE_ERR_HW;

  return status;
}

void module_exit() 
{
  //free(data_buffer);
}

INT module_bor() 
{
  
  INT status = SUCCESS;
  
  if ( sis3300_bor() != SUCCESS )
    {
      return FE_ERR_HW;
    }

  return status;
}

INT module_eor() 
{
  
  sis3300_eor();

  return SUCCESS;
}

INT module_read(char *pevent) 
{

  sis3300_readout_eob() ;

  // ===========================================================================
  // Fill MIDAS event
  // ===========================================================================
  char bk_name[32];
  char *pdata;

  for (unsigned int i=0; i<sis3300_n_boards; i++)
    {

      if ( sis3300_odb[i].enabled == 0 ) continue;

      printf("SIS3300 Board %i event counter: %i\n",i, sis3300_event_counter[i]);

#if 0
      bk_close(pevent, pdata);
      sis3300_event_counter[i] = 0;
      continue;
#endif

      sprintf(bk_name, "S30%i", i+1);
      bk_create(pevent, bk_name, TID_BYTE, &pdata);

      // record the module ID
      memcpy(pdata, sis3300_ID+i, sizeof(sis3300_ID[0]));
      pdata += sizeof(sis3300_ID[0]);

      // record the number os samples per event
      memcpy(pdata, &sis3300_event_size, sizeof(sis3300_event_size));
      pdata += sizeof(sis3300_event_size);
      
      // record the number of events
      memcpy(pdata, &(sis3300_event_counter[i]), sizeof(sis3300_event_counter[i]));
      pdata += sizeof(sis3300_event_counter[i]);

      // record the error mask
      memcpy(pdata, sis3300_err+i, sizeof(sis3300_err[0]));
      pdata += sizeof(sis3300_err[0]);
      sis3300_err[i] = 0x0;
     
      u_int32_t len;

      // record time stamps
      len = sizeof(u_int32_t)*sis3300_event_counter[i];
      memcpy(pdata, sis3300_TimeStamps[i], len);
      pdata += len;

      // record Triggers
      len = sizeof(u_int32_t)*sis3300_event_counter[i];
      memcpy(pdata, sis3300_Trigger_data[i], len);
      pdata += len;

      // record ADC data
      len = sizeof(u_int32_t)*sis3300_event_counter[i]*sis3300_event_size; 
      printf("SIS3300 board %i data size: %i bytes\n",i,len);
      memcpy(pdata, sis3300_ADC12_data[i], len); 
      pdata += len;

      memcpy(pdata, sis3300_ADC34_data[i], len); 
      pdata += len;
            
      memcpy(pdata, sis3300_ADC56_data[i], len); 
      pdata += len;
      
      memcpy(pdata, sis3300_ADC78_data[i], len); 
      pdata += len;

      bk_close(pevent, pdata);
      sis3300_event_counter[i] = 0;


    }

  //if (data_size > MAX_EVENT_SIZE) 
  //  {
  //    cm_msg(MERROR, "sis3300_read","Event size is too large. Truncating data...\n");
  //    data_size = MAX_EVENT_SIZE;
  //  }
  //memcpy(pdata, data_buffer, data_size);
  //pdata += data_size;
  printf("data size: %i bytes\n",bk_size(pevent));


  return SUCCESS;
}


INT module_poll_live() 
{
  if ( sis3300_poll() != SUCCESS ) return FE_ERR_HW;

  return SUCCESS;
}

