#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/io.h>
#include <sys/time.h>

#include "midas.h"

#include "crate.h"
#include "diag.h"
#include "odb_wrapper.h"

#include "rpc_mucap.h"

INT rpc_end_of_cycle(INT index, void *prpc_param[]);
INT rpc_slave_init();
INT rpc_slave_bor();
INT rpc_slave_eor();
INT rpc_slave_poll_live();
INT rpc_slave_poll_dead();
INT rpc_slave_stop_block();
INT rpc_slave_read(char *pevent);

INT send_ready_for_cycle();

static HNDLE rpc_conn_master_crate;
static BOOL enable_rpc_slave;

static BOOL event_ended = FALSE;
static INT last_cycle = 0;
static INT last_event_number = 0;
static INT sent_stop_request = -1;

struct readout_module rpc_slave_module = {
  rpc_slave_init,        // init
  NULL,                  // exit
  NULL,                  // pre_bor
  rpc_slave_bor,         // bor
  rpc_slave_eor,         // eor
  rpc_slave_poll_live,   // poll_live
  rpc_slave_poll_dead,   // poll_dead
  NULL,                  // start_block
  rpc_slave_stop_block,  // stop_block
  rpc_slave_read,        // read
};

/* ********************************************************************* */
INT rpc_end_of_cycle(INT index, void *prpc_param[])
{
  INT cycle_number = CINT(0);
  INT event_number = CINT(1);

  diag_print(2, "Cycle ended: RAM %d, event %d\n", cycle_number, event_number);

  // first send out "fake" zero-length events for those cycles
  // where we didn't participate
  for(INT fake_evnum=last_event_number+1; fake_evnum < event_number; 
      fake_evnum++) 
  {
    diag_print(2, "Sending fake catch-up event for %d\n", fake_evnum);
    char fake_event[sizeof(EVENT_HEADER) + sizeof(BANK_HEADER)];
    char *pevent = fake_event + sizeof(EVENT_HEADER);

    rpc_flush_event();
    bk_init32(pevent);
    bm_compose_event((EVENT_HEADER *) fake_event, equipment[0].info.event_id, 0,
                     bk_size(pevent), fake_evnum);
    bm_send_event(equipment[0].buffer_handle, fake_event,
                    sizeof(fake_event), SYNC);
    bm_flush_cache(equipment[0].buffer_handle, SYNC);
  }
 
  event_ended = true;
  last_cycle = cycle_number;
  last_event_number = event_number;

  return SUCCESS;
}

/* ********************************************************************* */
INT rpc_slave_init()
{
  // register the RPC function that we provide
  rpc_register_functions(rpc_list_mucap, NULL);
  rpc_register_function(RPC_END_OF_CYCLE, rpc_end_of_cycle);

  return SUCCESS;
}

/* ********************************************************************* */
INT rpc_slave_bor()
{
  event_ended = FALSE;
  last_event_number = 0;
  sent_stop_request = -1;
  last_cycle = 0;

  // find the master crate
  int master_crate = -1;
  for(int i = 0; i < MAX_CRATES; i++) {
    if(odb_find_key("/Equipment/Crate %d", i) != NULL) {
      BOOL master = odb_get_bool("/Equipment/Crate %d/Settings/Master", i);
      if(master) master_crate = i;
    }
  }

  // maybe we're the master...in that case, disable slave function
  if(master_crate == crate_number) {
    enable_rpc_slave = FALSE;
    diag_print(1, "Disabling RPC slave function\n");
    return SUCCESS;
  } else {
    enable_rpc_slave = TRUE;
    diag_print(1, "Enabling RPC slave function\n");
  }

  // return error if unable to find the master crate
	printf("looking for master crate ...\n");
  if(master_crate < 0) {
		printf("no master found\n");
    diag_print(0, "Unable to locate master crate\n");
    return FE_ERR_HW;
  }
  diag_print(1, "Found master crate %d\n", master_crate);
	printf("Master :%d\n", master_crate);

  // establish RPC connections to the master crate
  char master_crate_label[10];
  sprintf(master_crate_label, "Crate %d", master_crate);

  cm_connect_client(master_crate_label, &rpc_conn_master_crate);
  rpc_set_option(rpc_conn_master_crate, RPC_OTRANSPORT, RPC_FTCP);
  rpc_set_option(rpc_conn_master_crate, RPC_NODELAY, TRUE);

  send_ready_for_cycle();

  return SUCCESS;
}

/* ********************************************************************* */
INT rpc_slave_eor()
{
  if(enable_rpc_slave) {
    cm_disconnect_client(rpc_conn_master_crate, 0);
  }
  return SUCCESS;
}

/* ********************************************************************* */
INT rpc_slave_poll_live()
{
  if(!enable_rpc_slave) {
    return SUCCESS;
  }

  // Yield to receive any pending RPCs
  cm_yield(1000);

  // Have we received an end-of-event notice?
  if(event_ended) {
    event_ended = FALSE;
    diag_print(2, "event ended\n");
    return FE_END_BLOCK;
  } else {
    return SUCCESS;
  }
}

/* ********************************************************************* */
INT rpc_slave_poll_dead()
{
  if(!enable_rpc_slave) {
    return SUCCESS;
  }

  return FE_NEED_START;
}

/* ********************************************************************* */
INT rpc_slave_stop_block()
{
  if(!enable_rpc_slave) {
    return SUCCESS;
  }

  int event_number = last_event_number + 1;

  // Send a request to the master crate for the block to end.
  if(sent_stop_request < event_number) {
    rpc_client_call(rpc_conn_master_crate, RPC_REQUEST_STOP, crate_number, 
      event_number);
    sent_stop_request = event_number;
  }

  return SUCCESS;
}

/* ********************************************************************* */
INT rpc_slave_read(char *pevent)
{
  if(!enable_rpc_slave) {
    return SUCCESS;
  }

  // Fill in event number
  (((EVENT_HEADER *)pevent)-1)->serial_number = last_event_number;

  // Announce that we're ready for the next cycle

  send_ready_for_cycle();

  return SUCCESS;
}

/* ********************************************************************* */
INT send_ready_for_cycle()
{
  // Send "ready to go"
  struct timeval tv1, tv2;
  gettimeofday(&tv1, NULL);
  rpc_client_call(rpc_conn_master_crate, RPC_READY_FOR_CYCLE, crate_number, 0); 
  gettimeofday(&tv2, NULL);

  diag_print(2, "Waited %f microseconds for RPC_READY_FOR_CYCLE.\n",
    (tv2.tv_sec-tv1.tv_sec)*1e6 + (tv2.tv_usec-tv1.tv_usec));
   
  return SUCCESS;
}

/* ********************************************************************* */
