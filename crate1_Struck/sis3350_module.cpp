/**
 * PROGRAM:     sis3350_module.cpp
 * DESCRIPTION: Control and readout of Struck SIS3350 digitizer.
 * author:      V.Tishchenko
 * date:        17-Oct-2015
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

#include "sis3350.h"
#include "sis3100_module.h"
#include "dev/pci/sis1100_var.h"  
#include "sis3100_calls/sis3100_vme_calls.h"

#include "sis3350_odb.cpp"
#include "sis3350_tools.cpp"

// Struck includes

static INT  module_init(void);
static void module_exit(void);
static INT  module_bor(void);
static INT  module_eor(void);
static INT  module_poll_live(void);
static INT  module_read(char *pevent); // MIDAS readout routine

struct readout_module sis3350_module = {
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
  if ( sis3350_odb_init() != SUCCESS ) return FE_ERR_HW;

  // Open SIS3350 digitizer(s)
  if ( sis3350_open() != SUCCESS ) return FE_ERR_HW;

  return status;
}

void module_exit() 
{
  //free(data_buffer);
}

INT module_bor() 
{
  
  INT status = SUCCESS;
  
  if ( sis3350_bor() != SUCCESS )
    {
      return FE_ERR_HW;
    }

  return status;
}

INT module_eor() 
{
  
  sis3350_eor();

  return SUCCESS;
}

INT module_read(char *pevent) 
{

  sis3350_readout_eob() ;

  // ===========================================================================
  // Fill MIDAS event
  // ===========================================================================

  for (unsigned int i=0; i<sis3350_n_boards; i++)
    {

      if ( sis3350_odb[i].enabled == 0 ) continue;
      
      //printf("Board %i event counter: %i\n",i, sis3350_event_counter[i]);
      
      for ( unsigned int j=0; j<sis3350_n_channels; j++)
	{
	  if ( ! sis3350_odb[i].ch[j].enabled  ) continue;

	  char bk_name[32];
	  DWORD *pdata;

	  sprintf(bk_name, "S5%i%i", i+1, j+1);
	  bk_create(pevent, bk_name, TID_DWORD, &pdata);

	  // record the data size
	  *pdata++ = sis3350_ADC_data_size[i][j];

	  // record the error mask
	  *pdata++ = sis3350_err[i][j];
	  sis3350_err[i][j] = 0;
	  
	  // record ADC data
	  memcpy(pdata, sis3350_ADC_data[i][j], sizeof(DWORD)*sis3350_ADC_data_size[i][j]);
	  pdata += sis3350_ADC_data_size[i][j]/2;

	  printf("SIS3350 board %i channel %i data size %i\n",i,j,sis3350_ADC_data_size[i][j]);
	  
	  // reset the data counter
	  sis3350_ADC_data_size[i][j] = 0;

	  bk_close(pevent, pdata);
	}
    }
    
  return SUCCESS;
}
  

INT module_poll_live() 
{
  if ( sis3350_poll() != SUCCESS ) return FE_ERR_HW;

  return SUCCESS;
}

