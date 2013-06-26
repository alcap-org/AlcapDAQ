#include <time.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>

#include "midas.h"
#include "crate.h"

INT dummy_master_poll_live();
INT dummy_master_poll_dead();
INT dummy_master_start_block();

struct readout_module dummy_master_module = {
  NULL,                   // init
  NULL,                   // exit
  NULL,                   // pre_bor
  NULL,                   // bor
  NULL,                   // eor
  dummy_master_poll_live,                   // poll_live
  dummy_master_poll_dead, // poll_dead
  dummy_master_start_block, // start_block
  NULL,                   // stop_block
  NULL,                   // read
};

struct timeval block_start_time;

INT dummy_master_start_block()
{
  gettimeofday(&block_start_time, 0); 
}

INT dummy_master_poll_live()
{
  struct timeval now;
  gettimeofday(&now, 0);
  double increment = (now.tv_sec - block_start_time.tv_sec)*1e6 + 
                      (now.tv_usec - block_start_time.tv_usec);

  if(increment > 0.1e6) {
    return FE_END_BLOCK;
  } else {
    return SUCCESS;
  }
}

INT dummy_master_poll_dead()
{
    return FE_NEED_START;
}

