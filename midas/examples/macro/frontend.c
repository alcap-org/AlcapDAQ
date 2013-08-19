#include "midas_macro.h"

name(Sample Frontend)
    display(3)

/**************************************/
hv_driver_begin
driver(lrs2415, 1)
hv_driver_end
equipment_begin
equipment_trigger(1, POLLED, CRATE(0)) equipment_periodic(scaler, 2, 10) equipment_hv(3, 60) equipment_end
/**************************************/
/* define CAMAC slots */
constant(IO506, 10) constant(ADC, 12) constant(SCALER, 13)

/**************************************/
init_begin
    camac_init
    camac_inhibit_clear
    camac_crate_zinit control(IO506, 1, 26) /* enable auto mode      */ control(IO506, 2,
                                                                                16)
/* NIM level, int. clock */
control(IO506, 0, 26) /* enable LAM Flip-Flop  */ control(IO506, 0, 10)

                               /* clear LAM Flip-Flop   */
 /* reset LAM logic */ write(IO506, 1, 16, 2)
write(IO506, 1, 16, 3) write(IO506, 1, 16, 2) write(IO506, 1, 16, 0) init_end
exit_begin
    exit_end
    begin_run
    begin_run_end end_run end_run_end pause_run pause_run_end resume_run resume_run_end
/**************************************/
trigger_update_begin message(TALK, "Received trigger update") trigger_update_end
/**************************************/
readout_begin(trigger) variable(a) bank(ADC0) loop(a, 0, 1) read(ADC, a, 0) bank_end
  /* clear LAM Flip-Flop */
camac_lam_clear(IO506)

  /* reset LAM logic */
write(IO506, 1, 16, 2)
write(IO506, 1, 16, 3) write(IO506, 1, 16, 2) write(IO506, 1, 16, 0) readout_end
/**************************************/
readout_begin(scaler) variable(a) bank32(SCLR) loop(a, 0, 1) read32(SCALER, a, 0)
bank_end readout_end
