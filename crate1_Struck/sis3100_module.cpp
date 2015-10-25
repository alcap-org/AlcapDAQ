/**
 * PROGRAM:     sis3100_module.cpp
 * DESCRIPTION: handles the VME interface.
 * author:      V.Tishchenko
 * date:        27-Sept-2015
 *
 *
 * Modifications:
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <stdint.h>
#include <sys/io.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Struck includes
#include <dev/pci/sis1100_var.h>  
#include <sis3100_calls/sis3100_vme_calls.h>

#include "midas.h"

#include "crate.h"
#include "diag.h"

static INT  module_init(void);
static void module_exit(void);
//static INT  module_bor(void);
//static INT  module_eor(void);
//static INT  module_poll_live(void);
//static INT  module_read(char *pevent); // MIDAS readout routine

int fd_vme;

struct readout_module sis3100_module = {
  module_init,             // init
  module_exit,             // exit
  NULL,                    // pre_bor
  NULL,                    // bor
  NULL,                    // eor
  NULL,                    // poll_live
  NULL,                    // poll_dead
  NULL,                    // start_block
  NULL,                    // stop_block
  NULL                     // read
};

extern HNDLE hDB;
#define SIS3100_DEVNAME      "/dev/sis1100_00remote"

INT module_init() 
{
  int status = SUCCESS;

  printf("%s module_init()\n", __FILE__ );

  fd_vme = -1;

  printf("Opening sus3100 VME interface %s ... ",SIS3100_DEVNAME);

  fflush(stdout);
  if ( (fd_vme = open(SIS3100_DEVNAME, O_RDWR, 0)) < 0 ) 
    {
      printf("ERROR opening device [%s]\n", SIS3100_DEVNAME);
      status = FE_ERR_HW;
    }
  printf("fd = %i [done]\n", fd_vme);

  return status;
}

void module_exit() 
{
  
  if ( fd_vme >= 0 )
    {
      printf("Closing sis3100 VME interface %i ...", fd_vme);
      
      if( close(fd_vme) < 0 ) 
	{
	  printf("error closing the Struck interface!");
	}
      fd_vme = -1;
      printf(" done\n");
    }

}


