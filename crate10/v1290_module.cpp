#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/io.h>
#include <sys/time.h> 

#include "midas.h"

#include "crate.h"
#include "diag.h"

#include "v1290.h"

#define V1290N_BASE 0x40000000
INT v1290_init();
INT v1290_bor();
INT v1290_eor();
INT v1290_poll_live();
INT v1290_read(char *pevent);

bool block_sig;
struct timespec timer_start;
MVME_INTERFACE *myvme;

struct readout_module v1290_module = {
  v1290_init(),           // init
  NULL,                   // exit
  NULL,                   // pre_bor
  v1290_bor,                   // bor
  v1290_eor,                   // eor
  v1290_poll_live,      // poll_live
  NULL,      // poll_dead
  NULL,    // start_block
  NULL,                   // stop_block
  v1290_read,                   // read
};

INT v1290_init()
{
	printf("VME crate init...\n");
	mvme_open(&myvme,0);
  return SUCCESS;
}

INT v1290_bor()
{
	return SUCCESS;
}
INT v1290_eor()
{
	return SUCCESS;
}
INT v1290_read(char *pevent)
{
	return SUCCESS;
}
