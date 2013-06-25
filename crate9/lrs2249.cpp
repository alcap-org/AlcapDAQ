#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <time.h>

#include "midas.h"

#include "crate.h"
#include "mcstd.h"
#include "odb_wrapper.h"
#include "camac_wrapper.h"
#include "diag.h"

INT lrs2249_bor();
INT lrs2249_read(char *pevent);

struct readout_module lrs2249_module = {
  NULL,                 // init
  NULL,                 // exit
  NULL,                 // pre_bor
  lrs2249_bor,          // bor
  NULL,                 // eor
  NULL,                 // poll_live
  NULL,                 // poll_dead
  NULL,                 // start_cycle
  NULL,                 // stop_cycle
  lrs2249_read,         // read
};

struct lrs2249 {
  BOOL enabled;
  int crate;
  int slot;
  char odb_name[20];
  char bank_name[20];
};

#define MAX_LRS2249 8
struct lrs2249 lrs2249[MAX_LRS2249];

#define N_CH_ADC 12

/*
 * lrs2249_bor1
 *
 * Initialize one LRS2249 module.
 */
INT lrs2249_bor1(struct lrs2249 *adc)
{
  // Get the address of the module
  adc->crate = odb_get_int("/Equipment/Crate %d/Settings/%s/Crate",
                    crate_number, adc->odb_name);
  adc->slot = odb_get_int("/Equipment/Crate %d/Settings/%s/Slot",
                    crate_number, adc->odb_name);

  cam_init_once();

  camc(adc->crate, adc->slot, 0, 9);

  return SUCCESS;
}
                                                                                
/*
 * lrs2249_bor
 *
 * Called at the beginning of the run to discover LRS2249 modules
 * and initialize them.
 */
INT lrs2249_bor()
{
  // Use the ODB to find any LRS2249 modules
  for(int j = 0; j < MAX_LRS2249; j++) {
 
    bool enabled = false;
                                                                                
    if (odb_find_key("/Equipment/Crate %d/Settings/LRS2249 %d", crate_number, j)) {
      diag_print(1, "ODB says LRS2249 %d is present, ", j);
      enabled =
        odb_get_bool("/Equipment/Crate %d/Settings/LRS2249 %d/enabled status",
            crate_number, j);
      if (enabled) {
        diag_print(1, "and is enabled.  Initializing...\n");
      } else {
        diag_print(1, "but is disabled.\n");
      }
    }
                                                                                
    lrs2249[j].enabled = enabled;
                                                                                
    // Set up the name of the MIDAS bank associated with the module
    sprintf(lrs2249[j].bank_name, "LAD%d", j);
    sprintf(lrs2249[j].odb_name, "LRS2249 %d", j);
                                                                                
    if(enabled) {
      lrs2249_bor1(&lrs2249[j]);
    }
  }
 
  return SUCCESS;
}

/*
 * lrs2249_read1
 *
 * Constructs the MIDAS bank for a single LRS2249.
 */
INT lrs2249_read1(struct lrs2249 *adc, char *pevent)
{
  // Check whether the ADC has converted; if not, skip it
  INT q;
  camc_q(adc->crate, adc->slot, 0, 8, &q);
  if(!q) return SUCCESS;
   
  // Create the MIDAS bank
  WORD *pdata;
  bk_create(pevent, adc->bank_name, TID_WORD, &pdata);

  // Read ADC
  for(int i = 0; i < N_CH_ADC; i++) {
    cami(adc->crate, adc->slot, i, 0, pdata++);
  }

  // Clear ADC
  camc(adc->crate, adc->slot, 0, 9);

  // Close the bank
  bk_close(pevent, pdata);

  return SUCCESS;
}

/*
 * lrs2249_read
 *
 * Called at the end of a block to assemble data from that block into a 
 * MIDAS event.
 */
INT lrs2249_read(char *pevent)
{
  for(int i = 0; i < MAX_LRS2249; i++) {
    if(lrs2249[i].enabled) {
      int status = lrs2249_read1(&lrs2249[i], pevent);
       if(status != SUCCESS) {
         return status;
       }
    }
  }

  return SUCCESS;
}
