#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <time.h>

#include "midas.h"

#include "crate.h"
#include "mcstd.h"
#include "odb_wrapper.h"
#include "diag.h"

INT s500_bor();
INT s500_read(char *pevent);

struct readout_module s500_module = {
  NULL,                 // init
  NULL,                 // exit
  NULL,                 // pre_bor
  s500_bor,             // bor
  NULL,                 // eor
  NULL,                 // poll_live
  NULL,                 // poll_dead
  NULL,                 // start_cycle
  NULL,                 // stop_cycle
  s500_read,         // read
};

#define N_CH_S500 6

struct s500 {
  BOOL enabled;
  int crate;
  int slot;
  char odb_name[20];
  DWORD acum[N_CH_S500];
};

#define MAX_S500 8
struct s500 s500[MAX_S500];

/*
 * s500_bor1
 *
 * Initialize one S500 module.
 */
INT s500_bor1(struct s500 *s500)
{
  // Get the VME base address of the module
  s500->crate = odb_get_int("/Equipment/Crate %d/Settings/%s/Crate",
                    crate_number, s500->odb_name);
  s500->slot = odb_get_int("/Equipment/Crate %d/Settings/%s/Slot",
                    crate_number, s500->odb_name);

  // clear all channels
  for(int i = 0; i < N_CH_S500; i++) {
    camo(s500->crate, s500->slot, i, 9, 0);
  }

  // clear the accumulated values
  for(int i = 0; i < N_CH_S500; i++) {
    s500->acum[i] = 0;
  }
                                                                                
  return SUCCESS;
}
                                                                                
/*
 * s500_bor
 *
 * Called at the beginning of the run to discover S500 modules
 * and initialize them.
 */
INT s500_bor()
{
  // Use the ODB to find any S500 modules
  for(int j = 0; j < MAX_S500; j++) {
 
    bool enabled = false;
                                                                                
    if (odb_find_key("/Equipment/Crate %d/Settings/S500 %d", crate_number, j)) {
      diag_print(1, "ODB says S500 %d is present, ", j);
      enabled =
        odb_get_bool("/Equipment/Crate %d/Settings/S500 %d/enabled status",
            crate_number, j);
      if (enabled) {
        diag_print(1, "and is enabled.  Initializing...\n");
      } else {
        diag_print(1, "but is disabled.\n");
      }
    }
                                                                                
    s500[j].enabled = enabled;
                                                                                
    // Set up the name of the MIDAS bank associated with the module
    sprintf(s500[j].odb_name, "S500 %d", j);
                                                                                
    if(enabled) {
      s500_bor1(&s500[j]);
    }
  }

  return SUCCESS;
}

/*
 * s500_read
 *
 * Called at the end of a block to assemble data from that block into a 
 * MIDAS event.
 */
INT s500_read(char *pevent)
{
  DWORD *pdata;

  // first fill in the raw values
  bk_create(pevent, "S500", TID_DWORD, &pdata);
  for(int i = 0; i < MAX_S500; i++) {
    if(s500[i].enabled) {
      for(int j = 0; j < N_CH_S500; j++) {
        DWORD val;
        cam24i(s500[i].crate, s500[i].slot, j, 2, &val);
        *pdata++ = val;
        s500[i].acum[j] += val; 
      }
    }
  }
  bk_close(pevent, pdata);

  // now the accumulated values
  bk_create(pevent, "ACUM", TID_DWORD, &pdata);
  for(int i = 0; i < MAX_S500; i++) {
    if(s500[i].enabled) {
      for(int j = 0; j < N_CH_S500; j++) {
        *pdata++ = s500[i].acum[j];
      }
    }
  }
  bk_close(pevent, pdata);

  return SUCCESS;
}
