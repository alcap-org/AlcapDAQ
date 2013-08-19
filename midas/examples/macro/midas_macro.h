/********************************************************************\

  Name:         midas_macro.h
  Created by:   Stefan Ritt

  Contents:     C macros for writing a MIDAS frontend

  $Log: midas_macro.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.4  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.3  2000/08/21 10:49:11  midas
  Added max_event_size

  Revision 1.2  1998/10/12 12:18:59  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <midas.h>
#include <cd_hv.h>
#include "mcstd.h"
#include "experim.h"

/*-- Globals -------------------------------------------------------*/

BOOL frontend_call_loop = FALSE;
INT max_event_size = 10000;
INT event_buffer_size = 10 * 10000;
HNDLE hdb;
INT _crate = 0;

#define name(x) char *frontend_name = #x; char *frontend_file_name = __FILE__;
#define display(s) INT display_period = s*1000;

#ifdef TRIGGER_SETTINGS_DEFINED
TRIGGER_SETTINGS trigger_settings;
#endif

#ifdef SCALER_SETTINGS_DEFINED
SCALER_SETTINGS scaler_settings;
#endif

#ifdef EXP_PARAM_DEFINED
EXP_PARAM exp_param;
#endif

/*-- Function declarations -----------------------------------------*/

INT frontend_init();
INT frontend_exit();
INT begin_of_run(INT run_number, char *error);
INT end_of_run(INT run_number, char *error);
INT pause_run(INT run_number, char *error);
INT resume_run(INT run_number, char *error);
INT frontend_loop();

INT read_trigger(char *pevent);
INT read_scaler(char *pevent);

/*-- Equipment list ------------------------------------------------*/

#define mask(slot) (1<<(slot-1))

#define equipment_begin EQUIPMENT equipment[] = {

#define equipment_end   { "" }};

#define equipment(name, id, type, source, readon, period, readout, cd, driver) \
  { #name, id, 0, "SYSTEM", type, source, "MIDAS", TRUE, readon, period, 0, 0, "","","", \
    readout, cd, driver },

#define equipment_trigger(id, type, s) \
  equipment(Trigger, id, EQ_##type, s, RO_RUNNING|RO_ODB, 500, \
  read_trigger, NULL, NULL)

#define equipment_periodic(name, id, period)  \
  equipment(name, id, EQ_PERIODIC, 0,\
  RO_RUNNING|RO_TRANSITIONS|RO_ODB, period*1000,\
  read_##name, NULL, NULL)

#define equipment_hv(id, period) \
  equipment(HV, id, EQ_SLOW, 0,\
  RO_RUNNING|RO_TRANSITIONS, period*1000,\
  cd_hv_read, cd_hv, hv_driver)

#define hv_driver_begin DEVICE_DRIVER hv_driver[] = {

#define driver(name, count) { #name, name, count },

#define hv_driver_end { "" }};

/*-- Polling routine -----------------------------------------------*/

INT poll_event(INT source, INT count, BOOL test)
/* Polling routine for events. Returns "source" if event
   is available. If test equals TRUE, don't return. The test
   flag is used to time the polling */
{
   INT i, lam;

   for (i = 0; i < count; i++) {
      cam_lam_read(source >> 24, &lam);
      if (lam)
         if (!test)
            return TRUE;
   }

   return FALSE;
}

/*-- Interrupt configuration for trigger event ---------------------*/

INT interrupt_configure(INT cmd, INT source, PTYPE adr)
{
   switch (cmd) {
   case CMD_INTERRUPT_ENABLE:
      cam_interrupt_enable();
      break;
   case CMD_INTERRUPT_DISABLE:
      cam_interrupt_disable();
      break;
   case CMD_INTERRUPT_ATTACH:
      cam_interrupt_attach((void (*)()) adr);
      break;
   case CMD_INTERRUPT_DETACH:
      cam_interrupt_detach();
      break;
   }
   return SUCCESS;
}

/*-- trigger settings callback -------------------------------------*/

#ifdef TRIGGER_SETTINGS_DEFINED
void trigger_update(void);

void trigger_upd(INT hDB, INT hKey)
{
   trigger_update();
}
#endif

#define trigger_update_begin  void trigger_update(void) {
#define trigger_update_end }

/*-- Frontend init function ----------------------------------------*/

void user_init(void);
void user_exit(void);

INT frontend_init()
{
   HNDLE hkey;
#ifdef EXP_PARAM_DEFINED
   EXP_PARAM_STR(exp_param_str);
#endif
#ifdef TRIGGER_SETTINGS_DEFINED
   TRIGGER_SETTINGS_STR(trigger_settings_str);
#endif
#ifdef SCALER_SETTINGS_STR
   SCALER_SETTINGS_STR(scaler_settings_str);
#endif

   cm_get_experiment_database(&hdb, &hkey);

#ifdef EXP_PARAM_DEFINED
   db_create_record(hdb, 0, "/Experiment/Run Parameters", strcomb(exp_param_str));
   db_find_key(hdb, 0, "/Experiment/Run Parameters", &hkey);
   if (db_open_record(hdb, hkey, &exp_param,
                      sizeof(exp_param), MODE_READ, NULL) != DB_SUCCESS) {
      cm_msg(MERROR, "frontend_init", "Cannot open Run Parameters in ODB");
      return -1;
   }
#endif
#ifdef TRIGGER_SETTINGS_DEFINED
   db_create_record(hdb, 0, "/Equipment/Trigger/Settings", strcomb(trigger_settings_str));
   db_find_key(hdb, 0, "/Equipment/Trigger/Settings", &hkey);
   if (db_open_record(hdb, hkey, &trigger_settings,
                      sizeof(trigger_settings), MODE_READ, trigger_upd) != DB_SUCCESS) {
      cm_msg(MERROR, "frontend_init", "Cannot open Trigger Settings in ODB");
      return -1;
   }
#endif
#ifdef SCALER_SETTINGS_DEFINED
   db_create_record(hdb, 0, "/Equipment/Scaler/Settings", strcomb(scaler_settings_str));
   db_find_key(hdb, 0, "/Equipment/Scaler/Settings", &hkey);
   if (db_open_record(hdb, hkey, &scaler_settings,
                      sizeof(scaler_settings), MODE_READ, NULL) != DB_SUCCESS) {
      cm_msg(MERROR, "frontend_init", "Cannot open Scaler Settings in ODB");
      return -1;
   }
#endif

   user_init();
   return SUCCESS;
}

INT frontend_exit()
{
   user_exit();
   return SUCCESS;
}

#define init_begin   void user_init() {
#define init_end     }
#define exit_begin   void user_exit() {
#define exit_end     }

/*-- Callback functions --------------------------------------------*/

#define begin_run        INT begin_of_run(INT run_number, char *error) {
#define begin_run_end    return SUCCESS; }
#define end_run          INT end_of_run(INT run_number, char *error) {
#define end_run_end      return SUCCESS; }
#define pause_run        INT pause_run(INT run_number, char *error) {
#define pause_run_end    return SUCCESS; }
#define resume_run       INT resume_run(INT run_number, char *error) {
#define resume_run_end   return SUCCESS; }

/*-- Frontend Loop -------------------------------------------------*/

INT frontend_loop()
{
   return SUCCESS;
}

/*-- Event readout -------------------------------------------------*/

#define readout_begin(name) INT read_##name(char *pevent) \
{                                                         \
WORD *pdata; INT source;                                  \
                                                          \
  source = *((INT *)pevent);                              \
  bk_init(pevent); {

#define readout_end } return bk_size(pevent); }

#define readout_abort return 0;

#define trigger_mask     TRIGGER_MASK(pevent)
#define event_id         EVENT_ID(pevent)
#define serial_number    SERIAL_NUMBER(pevent)
#define time_stamp       TIME_STAMP(pevent)

#define bank(name) bk_create(pevent, #name, TID_WORD, &pdata);
#define bank32(name) bk_create(pevent, #name, TID_DWORD, &pdata);
#define bank_end  bk_close(pevent, pdata);

#define read(n,a,f)         cam16i(_crate,n,a,f,pdata++);
#define read_into(n,a,f,d)  cam16i(_crate,n,a,f,&d);
#define read32(n,a,f)     { cam24i(_crate,n,a,f,(DWORD*)pdata); ((DWORD*)pdata)++; }
#define write(n,a,f,d)      cam24o(_crate,n,a,f,d);
#define control(n,a,f)      camc(_crate,n,a,f);
#define test_q(n,a,f,q)     camc_q(_crate,n,a,f,&q);
#define wait_q(n,a,f)     { int _q; do { camc_q(_crate,n,a,f,&_q); } while(!_q); }
#define last_read           *(pdata-1)
#define last_read32         *(((DWORD *)pdata)-1)

#define camac_init cam_init();
#define camac_set_crate(c) _crate = c;
#define camac_crate_clear cam_crate_clear(_crate);
#define camac_crate_zinit cam_crate_zinit(_crate);
#define camac_inhibit_clear cam_inhibit_clear(_crate);
#define camac_inhibit_set cam_inhibit_set(_crate);
#define camac_lam_enable(n) cam_lam_enable(_crate,n);

#define camac_lam_clear(n) cam_lam_clear(_crate,n);

#define message(type, str)  cm_msg(M##type, "user code", str);

/*-- Library functions ---------------------------------------------*/

#define odb_set(path, value, type) \
  db_set_value(hdb, 0, #path, &value, sizeof(value), 1, TID_##type);

#define sleep(ms) ss_sleep(ms);

/*-- Language elements ---------------------------------------------*/

#define variable(name) int name;
#define constant(name, value) int name=value;

#define increment(var) var++;
#define decrement(var) var--;
#define set(var, value) var = value;

#define loop(var,c1,c2) for (var=c1 ; var<=c2 ; var++)
#define call(sub) sub();
#define begin {
#define end }

#define equal ==
#define or    ||
#define and   &&
