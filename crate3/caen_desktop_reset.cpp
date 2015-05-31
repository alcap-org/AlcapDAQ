#include "midas.h"

#include "crate.h"
#include "diag.h"
#include "../CAEN_DesktopDigi_utils/CAENdtDigiUtils.h"

INT caen_desktop_reset_bor();

struct readout_module caen_desktop_reset_module = {
  NULL,                   // init
  NULL,                   // exit
  NULL,                   // pre_bor
  caen_desktop_reset_bor, // bor
  NULL,                   // eor
  NULL,                   // poll_live
  NULL,                   // poll_dead
  NULL,                   // start_block
  NULL,                   // stop_block
  NULL,                   // read
};

// Since using the CAEN DPP firmware on a trial basis,
// we must reset the digitizers at the beginning of
// run.
INT caen_desktop_reset_bor() {

  // Argument to  power cycles the CAEN desktop digitizers
  // in the order they were plugged in. Since this
  // is difficult to determine, we just do it for
  // all of the ones we might have.
  const int NDigitizers = 2;
  for (int i = 0; i < NDigitizers; ++i)
    simulate_power_cycle(i);
  ss_sleep(10*1000); // Give units time to restart.

  return SUCCESS;
}
