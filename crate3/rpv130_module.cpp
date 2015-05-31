#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/io.h>
#include <sys/time.h> 

#include "midas.h"

#include "crate.h"
#include "diag.h"

#include "rpv130.h"

#define RPV130_BASE 0x8000
INT rpv130_init();
INT rpv130_start_block();
INT rpv130_poll_live();
INT rpv130_poll_dead();
struct timespec timer_start;
MVME_INTERFACE *myvme;

struct readout_module rpv130_module = {
  rpv130_init,           // init
  NULL,                   // exit
  NULL,                   // pre_bor
  NULL,                   // bor
  NULL,                   // eor
  rpv130_poll_live,      // poll_live
  rpv130_poll_dead,      // poll_dead
  rpv130_start_block,    // start_block
  NULL,                   // stop_block
  NULL,                   // read
};

/*
 * rpv130_init
 *
 * Called at the beginning of the run 
 */

INT rpv130_init()
{
	printf("VME crate init...\n");
	mvme_open(&myvme,0);
  return SUCCESS;
}

/*
 * rpv130_start_block
 *
 */
INT rpv130_start_block()
{
	rpv130_Pulse(myvme, RPV130_BASE, 2);
	rpv130_ClearBusy3(myvme, RPV130_BASE);
	clock_gettime(CLOCK_REALTIME,&timer_start);
  return SUCCESS;
}

/*
 * rpv130_poll_live()
 *
 * Called periodically while a block is active; performs active readout.
 *
 * Returns:
 * - ordinarily 0,
 * - a request for a "soft stop" end-of-block, or
 * - an error code
 */

INT rpv130_poll_live()
{
	struct timespec timer_now;
	clock_gettime(CLOCK_REALTIME, &timer_now);  
	unsigned int time_elapse =(timer_now.tv_sec - timer_start.tv_sec)*1000000000 +
		(timer_now.tv_nsec - timer_start.tv_nsec);
	printf("time elapse: %ld\n", time_elapse);

	if (rpv130_IsBusy3(myvme, RPV130_BASE))
		return FE_END_BLOCK;

	//if (time_elapse > 2*1000*1000)
	//{
		//printf("end of block\n");
		//return FE_END_BLOCK;
	//}
  return SUCCESS;
} 

INT rpv130_poll_dead()
{
  return 0;
}
