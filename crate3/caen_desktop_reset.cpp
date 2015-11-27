#include "midas.h"

#include "crate.h"
#include "diag.h"
#include "../CAEN_DesktopDigi_utils/CAENdtDigiUtils.h"

INT caen_desktop_reset_bor();

struct readout_module caen_desktop_reset_module = {
  NULL,                   // init
  NULL,                   // exit
  caen_desktop_reset_bor, // pre_bor
  NULL,                   // bor
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
  const int NDigitizers = 0;
  for (int i = 0; i < NDigitizers; ++i)
      simulate_power_cycle(i);

  // *** modified by VT on 2015-11-07 ***
  // We run the DT5720 with a Standard firmware, so we don't have
  // power cycle it. 
  // DT5720 has to be power ON the first.
  // Than it will stay the first (USB link Nr. 0)
  // 
  //simulate_power_cycle(1);

  //ss_sleep(10*1000); // Give units time to restart.

  return SUCCESS;
}
