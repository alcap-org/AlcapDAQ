/********************************************************************\

  Name:         hvr_300.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol 
                for HVR_300 High Voltage Regulator

  $Log: hvr_300.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:17  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.8  2004/04/07 11:06:17  midas
  Version 1.7.1

  Revision 1.7  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.6  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.5  2003/11/13 12:27:11  midas
  Extent regulation region to 5V

  Revision 1.4  2003/09/23 09:20:56  midas
  Second version: new resistor values, config bits, split CSR

  Revision 1.3  2003/03/19 16:35:03  midas
  Eliminated configuration parameters

  Revision 1.2  2003/02/25 09:42:08  midas
  Fixed problem with current trip during ramping

  Revision 1.1  2003/02/21 13:42:28  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include "mscb.h"

extern bit FREEZE_MODE;
extern bit DEBUG_MODE;

char code node_name[] = "HVR-300";

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

/* calculate voltage divider */
#define DIVIDER ((41E6 + 33E3) / 33E3)

/* current resistor */
#define RCURR 112E3

/* current multiplier */
#define CUR_MULT 15

/* configuration jumper */
sbit JU1 = P0 ^ 5;              /* negative module if forced to zero */
sbit JU2 = P0 ^ 1;              /* low current version if forced to zero */

bit demand_changed, i_limit_changed, ramp_up, ramp_down;
unsigned long demand_changed_time;
float v_actual;
unsigned int i_limit_adc;

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

/* CSR control bits */
#define CONTROL_HV_ON      (1<<0)
#define CONTROL_REGULATION (1<<1)

/* CSR status bits */
#define STATUS_NEGATIVE    (1<<0)
#define STATUS_LOWCUR      (1<<1)
#define STATUS_RAMP_UP     (1<<2)
#define STATUS_RAMP_DOWN   (1<<3)
#define STATUS_VLIMIT      (1<<4)
#define STATUS_ILIMIT      (1<<5)

struct {
   unsigned char control;
   float v_demand;
   float v_meas;
   float i_meas;
   unsigned char status;
   unsigned char trip_cnt;

   unsigned int ramp_up;
   unsigned int ramp_down;
   float v_limit;
   float i_limit;
   unsigned char trip_max;

   float adc_gain;
   float adc_offset;
   float dac_gain;
   float dac_offset;
   float cur_gain;
   float cur_offset;

   float temperature;
   float v_dac;
   unsigned int debug0;
   unsigned int debug1;
} idata user_data;

MSCB_INFO_VAR code variables[] = {
   1, UNIT_BYTE, 0, 0, 0, "Control", &user_data.control,        // 0
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "Vdemand", &user_data.v_demand,     // 1
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "Vmeas", &user_data.v_meas, // 2
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "Imeas", &user_data.i_meas,      // 3
   1, UNIT_BYTE, 0, 0, 0, "Status", &user_data.status,  // 4
   1, UNIT_COUNT, 0, 0, 0, "TripCnt", &user_data.trip_cnt,      // 5

   2, UNIT_VOLT, 0, 0, 0, "RampUp", &user_data.ramp_up, // 6
   2, UNIT_VOLT, 0, 0, 0, "RampDown", &user_data.ramp_down,     // 7
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "Vlimit", &user_data.v_limit,       // 8 
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "Ilimit", &user_data.i_limit,    // 9
   1, UNIT_COUNT, 0, 0, 0, "TripMax", &user_data.trip_max,      // 10

   /* calibration constants */
   4, UNIT_FACTOR, 0, 0, MSCBF_FLOAT, "ADCgain", &user_data.adc_gain,   // 11
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADCofs", &user_data.adc_offset,    // 12
   4, UNIT_FACTOR, 0, 0, MSCBF_FLOAT, "DACgain", &user_data.dac_gain,   // 13
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DACofs", &user_data.dac_offset,    // 14
   4, UNIT_FACTOR, 0, 0, MSCBF_FLOAT, "CURgain", &user_data.cur_gain,   // 15
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "CURofs", &user_data.cur_offset, // 16

   /* debugging */
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "TempC", &user_data.temperature, // 17
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "VDAC", &user_data.v_dac,   // 18
   2, UNIT_WORD, 0, 0, 0, "Debug0", &user_data.debug0,  // 19
   2, UNIT_WORD, 0, 0, 0, "Debug1", &user_data.debug1,  // 20
   0
};

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void dac_write(float value) reentrant;
void user_write(unsigned char index) reentrant;

/*---- User init function ------------------------------------------*/

extern SYS_INFO sys_info;

void user_init(unsigned char init)
{
   /* initial nonzero EEPROM values */
   if (init) {
      user_data.v_limit = 1200;
      user_data.i_limit = 3000;

      user_data.adc_gain = 1;
      user_data.adc_offset = 0;
      user_data.dac_gain = 1;
      user_data.dac_offset = 0;
      user_data.cur_gain = 1;
      user_data.cur_offset = 0;
   }

   /* default control register */
   user_data.control = CONTROL_REGULATION;
   user_data.status = 0;
   JU1 = 1;
   JU2 = 1;

   /* enable ADC */
   ADCCON1 = 0x7C;              // power up ADC, 14.5us conv+acq time

   /* power up or down DACs */
   user_write(0);

   /* force update */
   demand_changed = 1;

   /* forc update i_limit_adc */
   i_limit_adc = 4096;
   i_limit_changed = 1;
}

/*---- User write function -----------------------------------------*/

#pragma NOAREGS

void user_write(unsigned char index) reentrant
{
   if (index == 0) {
      if (user_data.control & CONTROL_HV_ON) {
         /* power up DACs */
         DACCON = 0x1F;
         v_actual = user_data.v_demand;
         dac_write(user_data.v_demand);
      } else {
         /* power down DACs */
         DACCON = 0;
      }
   }

   if (index == 1 || index == 4) {
      /* reset trip */
      user_data.status &= ~STATUS_ILIMIT;

      /* indicate new demand voltage */
      demand_changed = 1;
      demand_changed_time = time();
   }

   /* re-check voltage limit */
   if (index == 7) {
      demand_changed = 1;
      demand_changed_time = time();
   }

   /* recalculate trip current in ADC units */
   if (index == 8)
      i_limit_changed = 1;
}

/*---- User read function ------------------------------------------*/

unsigned char user_read(unsigned char index)
{
   if (index);
   return 0;
}

/*---- User function called vid CMD_USER command -------------------*/

unsigned char user_func(unsigned char *data_in, unsigned char *data_out)
{
   /* echo input data */
   data_out[0] = data_in[0];
   data_out[1] = data_in[1];
   return 2;
}

/*------------------------------------------------------------------*/

unsigned int adc_read(unsigned char channel)
{
   /* set MUX */
   ADCCON2 = (channel & 0x0F);

   DISABLE_INTERRUPTS;

   SCONV = 1;
   while (SCONV == 1);

   ENABLE_INTERRUPTS;

   return ADCDATAL + (ADCDATAH & 0x0F) * 256;
}

/*------------------------------------------------------------------*/

void dac_write(float value) reentrant
{
   unsigned int d;

   /* check for limit */
   if (value > user_data.v_limit) {
      value = user_data.v_limit;
      user_data.status |= STATUS_VLIMIT;
   } else
      user_data.status &= ~STATUS_VLIMIT;

   /* apply correction */
   value = value * user_data.dac_gain + user_data.dac_offset;
   if (value < 0)
      value = 0;

   /* convert HV to voltage */
   value = value / DIVIDER;

   /* convert to DAC units */
   value = value / 2.5 * 4096;

   /* output integer part to DAC0 */
   d = ((int) value) & 0x0FFF;
   //user_data.debug0 = d;

   DAC0H = d >> 8;
   DAC0L = d & 0xFF;

   /* output fractional part to DAC1 */
   value = value - d;
   value *= 56;                 /* assume 10k/560k mixer */

   /* offset by 16 to get out of the rail */
   d = (int) (value + 16.5);
   //user_data.debug1 = d;

   DAC1H = 0;
   DAC1L = d;
}

/*------------------------------------------------------------------*/

unsigned char check_current_trip(unsigned int adc)
{
   /* read current ADC if not given */
   if (adc == 0)
      adc = adc_read(0);

   user_data.debug0 = i_limit_adc;
   user_data.debug1 = adc;

   if (adc > i_limit_adc) {
      /* turn off HV */
      DAC0H = 0;
      DAC0L = 0;

      /* stop possible ramping */
      demand_changed = 0;
      v_actual = 0;
      user_data.v_dac = 0;
      ramp_up = 0;
      ramp_down = 0;
      user_data.status &= ~(STATUS_RAMP_UP | STATUS_RAMP_DOWN);

      /* raise trip flag */
      user_data.status |= STATUS_ILIMIT;
      user_data.trip_cnt++;
      user_data.v_dac = 0;
      v_actual = 0;

      /* check for trip recovery */
      if (user_data.trip_cnt < user_data.trip_max) {
         /* clear trip */
         user_data.status &= ~STATUS_ILIMIT;

         /* force ramp up */
         demand_changed = 1;
      }

      return 1;
   }

   return 0;
}

/*------------------------------------------------------------------*/

void read_hv(void)
{
   unsigned int i, n;
   float hv;
   unsigned long v_average;

   n = 10000;

   /* reduced averaging during ramping and after demand change */
   if (ramp_up || ramp_down || time() < demand_changed_time + 100)
      n = 100;

   v_average = 0;
   for (i = 0; i < n;) {
      v_average += adc_read(1);
      i++;

      if (check_current_trip(0))
         break;

      if (demand_changed)
         break;

      yield();
   }

   /* convert to Volt */
   hv = (float) v_average / i / 4096.0 * 2.5;

   /* convert to HV */
   hv *= DIVIDER;

   /* apply calibration */
   hv = hv * user_data.adc_gain + user_data.adc_offset;

   DISABLE_INTERRUPTS;
   user_data.v_meas = hv;
   ENABLE_INTERRUPTS;
}

/*------------------------------------------------------------------*/

void read_current(void)
{
   unsigned int i, adc;
   float current;
   unsigned long c_average;

   if (i_limit_changed) {
      if (user_data.cur_gain > 0)
         i_limit_adc =
             (user_data.i_limit -
              user_data.cur_offset) / user_data.cur_gain / 1E6 * RCURR /
             DIVIDER * CUR_MULT / 2.5 * 4096;
      i_limit_changed = 0;
   }

   c_average = 0;

   /* average 100 readings */
   for (i = 0; i < 100; i++) {
      adc = adc_read(0);
      check_current_trip(adc);

      c_average += adc;

      yield();

      if (demand_changed) {
         i++;
         break;
      }
   }

   /* convert to Volt, opamp gain, divider & curr. resist, microamp */
   current = (float) c_average / i / 4096.0 * 2.5 / CUR_MULT * DIVIDER / RCURR * 1E6;

   /* calibrate */
   current = current * user_data.cur_gain + user_data.cur_offset;

   DISABLE_INTERRUPTS;
   user_data.i_meas = current;
   ENABLE_INTERRUPTS;
}

/*------------------------------------------------------------------*/

void ramp_hv(void)
{
   static unsigned long t;
   unsigned char delta;

   /* only process ramping when HV is on and not tripped */
   if ((user_data.control & CONTROL_HV_ON) && !(user_data.status & STATUS_ILIMIT)) {

      if (demand_changed) {
         /* start ramping */

         if (user_data.v_demand > v_actual && user_data.ramp_up > 0) {
            /* ramp up */
            ramp_up = 1;
            ramp_down = 0;
            user_data.status |= STATUS_RAMP_UP;
            user_data.status &= ~STATUS_RAMP_DOWN;
            demand_changed = 0;
         }

         if (user_data.v_demand < v_actual && user_data.ramp_down > 0) {
            /* ramp down */
            ramp_up = 0;
            ramp_down = 1;
            user_data.status &= ~STATUS_RAMP_UP;
            user_data.status |= STATUS_RAMP_DOWN;
            demand_changed = 0;
         }

         /* remember start time */
         t = time();
      }

      /* ramp up */
      if (ramp_up) {
         delta = time() - t;
         if (delta) {
            v_actual += (float) user_data.ramp_up * delta / 100.0;

            if (v_actual >= user_data.v_demand) {
               /* finish ramping */

               v_actual = user_data.v_demand;
               user_data.v_dac = v_actual;
               ramp_up = 0;
               user_data.status &= ~STATUS_RAMP_UP;

               /* make regulation loop faster */
               demand_changed_time = time();
            }

            dac_write(v_actual);
            t = time();
         }
      }

      /* ramp down */
      if (ramp_down) {
         delta = time() - t;
         if (delta) {
            v_actual -= (float) user_data.ramp_down * delta / 100.0;

            if (v_actual <= user_data.v_demand) {
               /* finish ramping */

               v_actual = user_data.v_demand;
               user_data.v_dac = v_actual;
               ramp_down = 0;
               user_data.status &= ~STATUS_RAMP_DOWN;

               /* make regulation loop faster */
               demand_changed_time = time();
            }

            dac_write(v_actual);
            t = time();
         }
      }
   }
}

/*------------------------------------------------------------------*/

void regulation(void)
{
   /* only if HV on and not ramping */
   if ((user_data.control & CONTROL_HV_ON) && !ramp_up && !ramp_down &&
       !(user_data.status & STATUS_ILIMIT)) {
      if (user_data.control & CONTROL_REGULATION) {
         /* apply correction if outside +-0.05V window */
         if (user_data.v_meas > user_data.v_demand + 0.05 ||
             user_data.v_meas < user_data.v_demand - 0.05) {
            v_actual = user_data.v_demand;
            user_data.v_dac += user_data.v_demand - user_data.v_meas;

            /* only allow +-5V fine regulation range */
            if (user_data.v_dac < user_data.v_demand - 5)
               user_data.v_dac = user_data.v_demand - 5;

            if (user_data.v_dac > user_data.v_demand + 5)
               user_data.v_dac = user_data.v_demand + 5;

            demand_changed = 0;
            dac_write(user_data.v_dac);
         }
      } else {
         /* set voltage directly */
         if (demand_changed) {
            v_actual = user_data.v_demand;
            dac_write(v_actual);
            demand_changed = 0;
         }
      }
   }

   if (!(user_data.control & CONTROL_HV_ON))
      demand_changed = 0;
}

/*------------------------------------------------------------------*/

void read_temperature(void)
{
   unsigned int i;
   float temperature;
   unsigned long t_average;

   if (!ramp_up && !ramp_down) {
      t_average = 0;
      for (i = 0; i < 100; i++) {
         t_average += adc_read(8);

         check_current_trip(0);
         yield();
      }

      /* convert to V */
      temperature = (float) t_average / 100 / 4096.0 * 2.5;

      /* convert to deg. C */
      temperature = (temperature - 0.6) / -0.003 + 25;

      /* offset measured with prototype */
      temperature += 7.0;

      /* strip to 0.1 digits */
      temperature = ((int) (temperature * 10 + 0.5)) / 10.0;

      DISABLE_INTERRUPTS;
      user_data.temperature = temperature;
      ENABLE_INTERRUPTS;

      /* read node configuration */
      if (JU1 == 0)
         user_data.status |= STATUS_NEGATIVE;
      if (JU2 == 0)
         user_data.status |= STATUS_LOWCUR;
   }
}

/*---- User loop function ------------------------------------------*/

void user_loop(void)
{
   read_hv();
   ramp_hv();
   regulation();
   read_current();
   read_temperature();
}
