#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/io.h>

#include "midas.h"
#include "crate.h"

INT dummy_master_poll_dead();

struct readout_module dummy_master_module = {
  NULL,                   // init
  NULL,                   // exit
  NULL,                   // pre_bor
  NULL,                   // bor
  NULL,                   // eor
  NULL,                   // poll_live
  dummy_master_poll_dead, // poll_dead
  NULL,                   // start_block
  NULL,                   // stop_block
  NULL,                   // read
};

INT dummy_master_poll_dead()
{
    return FE_NEED_START;
}

