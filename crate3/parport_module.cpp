//#define PARALLEL_PORT0 0x4450
//#define PARALLEL_PORT1 0x4451
//#define PARALLEL_PORT1 0x44c0
//#define PARALLEL_PORT1 0x4440  // always reads 4
//#define PARALLEL_PORT0 0x1250
//#define PARALLEL_PORT1 0x1251
//#define PARALLEL_PORT0 0x1150
//#define PARALLEL_PORT1 0x1151

//#define PARALLEL_PORT0 0x378
// PSI R15a
//#define PARALLEL_PORT0 0xe050
#define PARALLEL_PORT0 0xd050
#define PARALLEL_PORT1 (PARALLEL_PORT0+1)

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/io.h>
#include <sys/time.h> 

#include "midas.h"

#include "crate.h"
#include "diag.h"
INT setPP(char, int);
INT parport_init();
INT parport_start_block();
INT parport_poll_live();
INT parport_poll_dead();
bool block_sig;
#if 0
struct timespec timer_start;
static timeval t_0;
#endif
static unsigned int block_nr;

struct readout_module parport_module = {
  parport_init,           // init
  NULL,                   // exit
  NULL,                   // pre_bor
  NULL,                   // bor
  NULL,                   // eor
  parport_poll_live,      // poll_live
  parport_poll_dead,      // poll_dead
  parport_start_block,    // start_block
  NULL,                   // stop_block
  NULL,                   // read
};


INT setPP(char v, int usec) { 

  outb(v,PARALLEL_PORT0);
  //outb(v,PARALLEL_PORT1);

  return 0;

}

/*
 * parport_init
 *
 * Called at the beginning of the run 
 */

INT parport_init()
{

  printf("Opening parallel port %x\t\n", PARALLEL_PORT0);
  if (ioperm(PARALLEL_PORT0,3,1))
    {
      perror("PArallel POrt opening error\n");
      return 1;
    }
  /*
  int status = iopl( 3 );

  if (status < 0) {
    diag_print(0, "Unable to get permission to access I/O ports\n");
  }

  // turn off output
  outb(0x1d, 0x3f2);
  */

  block_nr = 0;

#if 0
  gettimeofday(&t_0,NULL);  
#endif
 
	printf("Dummy master crate init\n");
  return SUCCESS;
}

/*
 * parport_start_block
 *
 */
INT parport_start_block()
{
  
  block_nr++;
#if 0
  printf("start block %i\n",block_nr);
#endif

  block_sig = true;
#if 0
  clock_gettime(CLOCK_REALTIME,&timer_start);
#endif
  //setPP(255, 1);

  // Send reset 
  setPP(1<<1, 1);
  setPP(0, 1);

  // start new segment
  setPP(1, 1);
  setPP(0, 1);

  // make sure that the segment has started 
  unsigned char p = inb(PARALLEL_PORT1);
#if 0
  printf("parport status: 0x%08x\n",p);
#endif
  if ( !(p & 0x40) )
    {
      cm_msg(MERROR, "parport_start_block()",
	     " The new segment did not start");
      return FE_ERR_HW;
    }

  return SUCCESS;
}

/*
 * parport_poll_live()
 *
 * Called periodically while a block is active; performs active readout.
 *
 * Returns:
 * - ordinarily 0,
 * - a request for a "soft stop" end-of-block, or
 * - an error code
 */

INT parport_poll_live()
{
  
  static unsigned int block=1;

#if 0
  struct timespec timer_now;
  clock_gettime(CLOCK_REALTIME, &timer_now);  
  unsigned int time_elapse = (timer_now.tv_sec - timer_start.tv_sec) * 1000000000 +
    (timer_now.tv_nsec - timer_start.tv_nsec);
  printf("time elapse: %ld\n", time_elapse);
  if (time_elapse > 2*1000*1000)
    {
#if 0
      printf("end of block\n");
#endif
      return FE_END_BLOCK;
    }
#endif 

  unsigned char p = inb(PARALLEL_PORT1);

  //printf("p=%i, 0x%x\n",p, p);
  //return FE_END_BLOCK;
  if ( !(p & 0x40) )
    {
#if 0
      //printf("p=%i, 0x%x\n",p, p);
      printf("end of block %i\n",block);
#endif
      block++;
      return FE_END_BLOCK;
    }
  

  return SUCCESS;
} 

INT parport_poll_dead()
{
  //printf("Send to start block\n");
  //return FE_NEED_START;
  return 0;
}
