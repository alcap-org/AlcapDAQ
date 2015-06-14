/**
 * PROGRAM:     crate.cpp                                                
 * DESCRIPTION: Control and readout of CAEN V1724 digitizers.                           
 * author:      V.Tishchenko                                             
 * date:        12-Aug-2012                                              
 *
 * Modifications:
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sched.h>

#include "midas.h"
#include "msystem.h"

#include "odb_wrapper.h"
#include "crate.h"
#include "diag.h"

/* from mfe.c */
extern int run_state;
extern int frontend_index;

/* Interrupt state */
BOOL interrupts_enabled = FALSE;
void (*interrupt_handler)() = NULL;

/* The frontend name (client name) as seen by other MIDAS clients   */
char *frontend_name;

/* The frontend file name, don't change it */
char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = TRUE;

/* a frontend status page is displayed with this frequency in ms */
//INT display_period = 500;
INT display_period = 0;

/* maximum event size produced by this frontend */
#define MAX_DATA_SIZE  
INT max_event_size = MAX_EVENT_SIZE;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
INT max_event_size_frag = 0;

/* buffer size to hold events */
INT event_buffer_size = DEFAULT_EVENT_BUFFER_SIZE ;

/*-- Function declarations -----------------------------------------*/
extern "C" {
  INT frontend_early_init();
  INT frontend_init();
  INT frontend_exit();
  INT pre_begin_of_run(INT run_number, char *error);
  INT begin_of_run(INT run_number, char *error);
  INT end_of_run(INT run_number, char *error);
  INT pause_run(INT run_number, char *error);
  INT resume_run(INT run_number, char *error);
  INT frontend_loop();
  INT poll_event(INT source, INT count, BOOL test);
  INT interrupt_configure(INT cmd, INT source, PTYPE adr);
};
INT read_trigger_event(char *pevent, INT off);
INT read_periodic_event(char *pevent, INT off);

/*-- Equipment list ------------------------------------------------*/
EQUIPMENT equipment[] = {
    {"Crate ",			/* equipment name */
     {1000, TRIGGER_ALL,	/* event ID, trigger mask: 1000 for muCap */
      "BUF",			/* event buffer */
      EQ_POLLED | EQ_EB,        /* equipment type */
      LAM_SOURCE(0, 0xFFFFFF),	/* event source crate 0, all stations */
      "MIDAS",			/* format */
      TRUE,			/* enabled */
      RO_RUNNING,		/* read only when running */
      500,			/* poll for 500ms */
      0,			/* stop run after this event limit */
      0,			/* number of sub events */
      0,			/* don't log history */
      "", "", ""},
     read_trigger_event,	/* readout routine */
     },
    {""}
};

/*
 * List of modules in main fast event.
 */
extern struct readout_module rpc_master_module;
extern struct readout_module rpc_slave_module;
extern struct readout_module v1724_module;
extern struct readout_module v1720_module;
extern struct readout_module v1290_module;

struct readout_module *trigger_modules[] = { 
  &rpc_master_module,
  &v1724_module,
  &v1720_module,
  &v1290_module,
  &rpc_slave_module,  // must be last!
};
int num_trigger_modules = sizeof(trigger_modules)/sizeof(trigger_modules[0]);

struct timeval readout_finished_time;

/*
 * List of modules in periodic event.
 */
//extern struct readout_module s500_module;

struct readout_module *periodic_modules[] = { 
  //&s500_module,
};
int num_periodic_modules = sizeof(periodic_modules)/sizeof(periodic_modules[0]);

int cycle_active = 0;
bool event_avail = false;

bool between_runs = true;

int crate_number = 4;

/*
 * frontend_early_init:  Called before most MIDAS subsystems are initialized,
 *                       so we can set the name (etc.) of the frontend here.
 */
INT frontend_early_init()
{
  // Determine our crate ID by comparing against a list of hostnames
  printf("Entering frontend_early_init ... \n");

  frontend_name = new char[32];
  strcpy(frontend_name, "Crate ");
  frontend_index = crate_number;

  printf("Crate number = %d\n",crate_number);
  
  printf("Frontend early init returns success \n");
  return SUCCESS;
}

/* ********************************************************************* */
/*
 * frontend_init:  When the frontend program is started. This routine
 *                 should initialize the hardware.
 */
INT frontend_init()
{
  printf("Entering frontend_init() ... \n");
  INT run_state = odb_get_int("/Runinfo/State");
  if (run_state != STATE_STOPPED) {
    cm_msg(MERROR, "Crate",
            "Run must be stopped before starting crate program.");
    return FE_ERR_HW;
  }

  for(int i = 0; i < num_trigger_modules; i++) {
    if((*trigger_modules[i]).init != NULL) {
      int status = (*trigger_modules[i]).init();
      if(status != SUCCESS) {
        return status;
      }
    }
  }

  for(int i = 0; i < num_periodic_modules; i++) {
    if((*periodic_modules[i]).init != NULL) {
      int status = (*periodic_modules[i]).init();
      if(status != SUCCESS) {
        return status;
      }
    }
  }

  return SUCCESS;
}

/* ********************************************************************* */
/*
 * frontend_exit:  When the frontend program is shut down. Can be used
 *                 to release any locked resources like memory, commu-
 *                 nications ports etc.
 */
INT frontend_exit()
{
  for(int i = 0; i < num_trigger_modules; i++) {
    if((*trigger_modules[i]).exit != NULL) {
      (*trigger_modules[i]).exit();
    }
  }

  for(int i = 0; i < num_periodic_modules; i++) {
    if((*periodic_modules[i]).exit != NULL) {
      (*periodic_modules[i]).exit();
    }
  }

  return SUCCESS;
}

/*
 * pre_begin_of_run:   Called before a new run is started. 
 */
INT pre_begin_of_run(INT run_number, char *error)
{
  diag_print(1, "*** Before begin of run %d ***\n", run_number);

  for(int i = 0; i < num_trigger_modules; i++) {
    if((*trigger_modules[i]).pre_bor != NULL) {
      int status = (*trigger_modules[i]).pre_bor();
      if(status != SUCCESS) {
        return status;
      }
    }
  }

  for(int i = 0; i < num_periodic_modules; i++) {
    if((*periodic_modules[i]).pre_bor != NULL) {
      int status = (*periodic_modules[i]).pre_bor();
      if(status != SUCCESS) {
        return status;
      }
    }
  }


  return SUCCESS;
}

/* ********************************************************************* */
/*
 * begin_of_run:   When a new run is started. Clear scalers, open
 *                 rungates, etc.
 */
INT begin_of_run(INT run_number, char *error)
{
  diag_print(1, "*** Begin of run %d ***\n", run_number);

  for(int i = 0; i < num_trigger_modules; i++) {
    if((*trigger_modules[i]).bor != NULL) {
      int status = (*trigger_modules[i]).bor();
      if(status != SUCCESS) {
        return status;
      }
    }
  }

  for(int i = 0; i < num_periodic_modules; i++) {
    if((*periodic_modules[i]).bor != NULL) {
      int status = (*periodic_modules[i]).bor();
      if(status != SUCCESS) {
        return status;
      }
    }
  }

  cycle_active = 0;
  event_avail = false;

  between_runs = false;

  return SUCCESS;
}

/* ********************************************************************* */
/*
 * end_of_run:     Called on a request to stop a run. Can send
 *                 end-of-run event and close run gates.
 */
INT end_of_run(INT run_number, char *error)
{
  diag_print(1, "*** End of run %d ***\n", run_number);

  between_runs = true;

  for(int i = 0; i < num_trigger_modules; i++) {
    if((*trigger_modules[i]).eor != NULL) {
      int status = (*trigger_modules[i]).eor();
      if(status != SUCCESS) {
        return status;
      }
    }
  }

  for(int i = 0; i < num_periodic_modules; i++) {
    if((*periodic_modules[i]).eor != NULL) {
      int status = (*periodic_modules[i]).eor();
      if(status != SUCCESS) {
        return status;
      }
    }
  }


  return SUCCESS;
}

/* ********************************************************************* */
/*
 * pause_run:      When a run is paused. Should disable trigger events.
 */
INT pause_run(INT run_number, char *error)
{
    return SUCCESS;
}

/* ********************************************************************* */
/*
 * resume_run:     When a run is resumed. Should enable trigger events.
 */
INT resume_run(INT run_number, char *error)
{
    return SUCCESS;
}

/* ********************************************************************* */
/*
 * start_cycle
 */
void start_cycle()
{
  printf("starting new cycle in crate.cpp\n");
  struct timeval restart_time;
  gettimeofday(&restart_time, NULL);

  for(int i = 0; i < num_trigger_modules; i++) {
    if((*trigger_modules[i]).start_cycle != NULL) {
      (*trigger_modules[i]).start_cycle();
    }
  }

  diag_print(2, "Waited %f microseconds for restart.\n", 
    (restart_time.tv_sec-readout_finished_time.tv_sec)*1e6 +
      (restart_time.tv_usec-readout_finished_time.tv_usec));

  cycle_active = 1;
}

/*
 * consider_start
 */
INT consider_start()
{
    for(int i = 0; i < num_trigger_modules; i++) {
      if((*trigger_modules[i]).poll_dead != NULL) {
        int status = (*trigger_modules[i]).poll_dead();

	if(status == FE_NEED_START && !event_avail) {
          start_cycle();
	} else if(status != SUCCESS) {
          return status;
	}
      }
    }
    return SUCCESS;
}

/*
 * stop_cycle
 */
void stop_cycle()
{
  for(int i = 0; i < num_trigger_modules; i++) {
    if((*trigger_modules[i]).stop_cycle != NULL) {
      (*trigger_modules[i]).stop_cycle();
    }
  }
}

/* ********************************************************************* */
/* 
 * If frontend_call_loop is true, this routine gets called when
 * the frontend is idle or once between every event 
 */ 
INT frontend_loop()
{
  // If we're going to be pre-empted, get it over with during livetime
  // rather than deadtime.
  if (cycle_active || run_state != STATE_RUNNING || between_runs == TRUE) { 
    dm_area_flush();
    sched_yield();
  }

  // Exit immediately if no run in progress
  if (run_state != STATE_RUNNING || between_runs == TRUE) {
    return SUCCESS;
  }

  // Call appropriate poll functions, depending on whether we're live or dead
  //printf("fe loop: cycle_active %i\n",cycle_active);
  if (cycle_active) { 

    for(int i = 0; i < num_trigger_modules; i++) {
      if((*trigger_modules[i]).poll_live != NULL) {

        // There are conditions that can lead to changes in run state between
        // modules...
        if (run_state != STATE_RUNNING || between_runs == TRUE) {
          return SUCCESS;
        }
        int status = (*trigger_modules[i]).poll_live();

        if(status == FE_END_BLOCK) {
          cycle_active = 0;
	  event_avail = true;
	} else if(status == FE_NEED_STOP) {
          stop_cycle(); 
	} else if(status != SUCCESS) {
          return status;
	}
      }
    }
  } else {
    consider_start();
  }

  return SUCCESS;
}

/* ********************************************************************* */
/* 
 * Polling routine for events. Returns TRUE if event
 * is available. If test equals TRUE, don't return. The test
 * flag is used to time the polling.
 */
INT poll_event(INT source, INT count, BOOL test)
{
    INT retval = false;

    if(!test) {
      count = 1;
    }

    for (int i = 0; i < count; i++) {
        frontend_loop();
	if(event_avail) {
          retval = true;
	}
    }

    return retval;
}

/* ********************************************************************* */
INT interrupt_configure(INT cmd, INT source, PTYPE adr)
{
    switch (cmd) {
    case CMD_INTERRUPT_ENABLE:
	interrupts_enabled = TRUE;
	break;
    case CMD_INTERRUPT_DISABLE:
	interrupts_enabled = FALSE;
	break;
    case CMD_INTERRUPT_ATTACH:
	interrupt_handler = (void (*)(void)) adr;
	break;
    case CMD_INTERRUPT_DETACH:
	interrupts_enabled = FALSE;
	interrupt_handler = NULL;
	break;
    }
    return SUCCESS;
}

void print_diag_output(char *pevent)
{
  printf("--------\n"); 

  printf("Event number: %d\n", (((EVENT_HEADER *)pevent)-1)->serial_number);

  BANK32 *pbk = NULL; 
  do {
    DWORD *pdata; 
    int size = bk_iterate32(pevent, &pbk, &pdata);
    if(pbk != NULL) {
      printf("%c%c%c%c\t%d\t\t", pbk->name[0], pbk->name[1], pbk->name[2],
        pbk->name[3], size);
      for(int i = 0; i < MIN(4, size/4); i++) {
         printf("0x%08x ", pdata[i]); 
      }
      printf("\n");
    }
  } while(pbk != NULL);

  printf("--------\n"); 
}

/* ********************************************************************* */
INT read_trigger_event(char *pevent, INT off)
{
  diag_print(2, "Event has ended.  Executing read_trigger_event.\n");

  // Remember that we have read out this event
  event_avail = false;

  // Initialize the event
  bk_init32(pevent);

  // Protection against readout after run has ended.
  if (run_state != STATE_RUNNING) {
    diag_print(1, "Readout after end of run!!!\n");
    return bk_size(pevent);
  }

  struct timeval readout_start_time;
  gettimeofday(&readout_start_time, NULL);
  // Ask trigger_modules to read out
  for(int i = 0; i < num_trigger_modules; i++) {
     if((*trigger_modules[i]).read != NULL) {
       int status = (*trigger_modules[i]).read(pevent);
       if(status != SUCCESS) {
         return status;
       }
    }
  }
  gettimeofday(&readout_finished_time, NULL);
  diag_print(2, "Spent %f microseconds for readout.\n", 
    (readout_finished_time.tv_sec-readout_start_time.tv_sec)*1e6 +
      (readout_finished_time.tv_usec-readout_start_time.tv_usec));

  // Consider starting up the next block
  consider_start();

  // Print a diagnostic
  if(diag_print_threshold >= 2) {
    print_diag_output(pevent);
  }

  return bk_size(pevent);
}

/* ********************************************************************* */
INT read_periodic_event(char *pevent, INT off)
{
  diag_print(2, "Periodic event readout:\n");

  // Initialize the event
  bk_init32(pevent);

  // Ask periodic_modules to read out
  for(int i = 0; i < num_periodic_modules; i++) {
     if((*periodic_modules[i]).read != NULL) {
       int status = (*periodic_modules[i]).read(pevent);
       if(status != SUCCESS) {
         return status;
       }
    }
  }

  // Print a diagnostic
  if(diag_print_threshold >= 2) {
    print_diag_output(pevent);
  }


  return bk_size(pevent);
}
