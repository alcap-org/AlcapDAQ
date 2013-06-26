#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/io.h>

#include "midas.h"

#include "crate.h"
#include "diag.h"

INT floppy_ttl_init();
INT floppy_ttl_start_block();
INT floppy_ttl_poll_live();
INT floppy_ttl_poll_dead();

struct readout_module floppy_ttl_module = {
  floppy_ttl_init,        // init
  NULL,                   // exit
  NULL,                   // pre_bor
  NULL,                   // bor
  NULL,                   // eor
  floppy_ttl_poll_live,   // poll_live
  floppy_ttl_poll_dead,   // poll_dead
  floppy_ttl_start_block, // start_block
  NULL,                   // stop_block
  NULL,                   // read
};

/*
 * floppy_ttl_init
 *
 * Called at the beginning of the run to discover SIS3600 modules
 * and initialize them.
 */
INT floppy_ttl_init()
{
  int status = iopl( 3 );

  if (status < 0) {
    diag_print(0, "Unable to get permission to access I/O ports\n");
  }

  // turn off output
  outb(0x1d, 0x3f2);

  return SUCCESS;
}

/*
 * floppy_ttl_start_block
 *
 */
INT floppy_ttl_start_block()
{
  // Produce a quick pulse on the output
  outb(0x0d, 0x3f2);
  outb(0x1d, 0x3f2);

  bool ready = false;
  DWORD start_waiting = ss_millitime();
  while(!ready && ss_millitime() < start_waiting + 1000) {
//  while(!ready) {
    int input = inb(0x3f7);
    if((input & 0x80)) {
      ready = true;
    }
  }

  if(!ready) {
    printf("\n\n\nNot ready, start times out\n");
  }

  return SUCCESS;
}

/*
 * floppy_ttl_poll_live()
 *
 * Called periodically while a block is active; performs active readout.
 *
 * Returns:
 * - ordinarily 0,
 * - a request for a "soft stop" end-of-block, or
 * - an error code
 */
INT floppy_ttl_poll_live()
{
  int input = inb(0x3f7);

  // high bit of input: 1 = TTL LOW, 0 = TTL HIGH
  if(!(input & 0x80)) {
    return FE_END_BLOCK;
  } else {
    return SUCCESS;
  }
}

INT floppy_ttl_poll_dead()
{
    return FE_NEED_START;
}
