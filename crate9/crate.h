#define FE_END_BLOCK    10001
#define FE_NEED_STOP    10002
#define FE_NEED_START   10003

#ifndef MIN
#define MIN(a,b) ( ((a)<(b)) ? (a) : (b) )
#endif

#ifndef MAX
#define MAX(a,b) ( ((a)>(b)) ? (a) : (b) )
#endif

struct readout_module
{
  // initialization
  INT (*init)();

  // exit
  void (*exit)();

  // begin run
  INT (*pre_bor)();

  // begin run
  INT (*bor)();

  // end run
  INT (*eor)();

  // called periodically while running and the DAQ is live (during a block)
  INT (*poll_live)();

  // called periodically while running and the DAQ is dead (between blocks)
  INT (*poll_dead)();

  // called when we intend to start the next block
  INT (*start_cycle)();

  // called when we intend to stop the current block
  INT (*stop_cycle)();

  // reads data from modules into MIDAS event
  INT (*read)(char *pevent);
};

extern char *frontend_name;
extern int crate_number;

#define MAX_CRATES 9

extern BOOL crate_is_participating(INT crate_number);

extern EQUIPMENT equipment[];
