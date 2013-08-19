/********************************************************************\

  Name:         hvr_200.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol 
                for HVR_300 High Voltage Regulator

  $Log: hvr_200.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:17  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.8  2004/07/29 15:48:27  midas
  Disabled temp. measurement (unreliable)

  Revision 1.7  2004/07/22 13:08:10  midas
  Implemented idle command

  Revision 1.6  2004/05/14 15:10:09  midas
  Fixed bug with status bits

  Revision 1.5  2004/04/30 07:59:22  midas
  LED on when HV is on

  Revision 1.4  2004/04/08 09:23:36  midas
  Added software current limit

  Revision 1.3  2004/04/07 11:06:17  midas
  Version 1.7.1

  Revision 1.2  2004/03/04 14:33:01  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include <math.h>
#include "mscb.h"

extern bit FREEZE_MODE;
extern bit DEBUG_MODE;

char code node_name[] = "HVR-200";

/* number of HV channels */
#define N_HV_CHN 4   

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = N_HV_CHN;

/* calculate voltage divider */
#define DIVIDER ((41E6 + 33E3) / 33E3)

/* current resistor */
#define RCURR 56E3

/* current multiplier */
#define CUR_MULT 15

/* delay for opto-couplers in us */
#define OPT_DELAY 100

/* configuration jumper */
sbit JU0 = P3 ^ 4;              // negative module if forced to zero
sbit JU1 = P3 ^ 2;
sbit JU2 = P3 ^ 1;

sbit SET_ADDR = P3 ^ 3;         // push button

/* AD7718 pins */
sbit ADC_NRES = P1 ^ 7;         // !Reset
sbit ADC_SCLK = P1 ^ 5;         // Serial Clock
sbit ADC_NCS  = P1 ^ 3;         // !Chip select
sbit ADC_NRDY = P1 ^ 0;         // !Ready
sbit ADC_DOUT = P1 ^ 1;         // Data out
sbit ADC_DIN  = P1 ^ 4;         // Data in

/* LTC2600 pins */
sbit DAC_NCS  = P1 ^ 2;         // !Chip select
sbit DAC_SCK  = P1 ^ 5;         // Serial Clock
sbit DAC_CLR  = P1 ^ 6;         // Clear
sbit DAC_DIN  = P1 ^ 4;         // Data in

/* AD7718 registers */
#define REG_STATUS     0
#define REG_MODE       1
#define REG_CONTROL    2
#define REG_FILTER     3
#define REG_ADCDATA    4
#define REG_ADCOFFSET  5
#define REG_ADCGAIN    6
#define REG_IOCONTROL  7

#define ADC_SF_VALUE  82        // SF value for 50Hz rejection

unsigned char idata chn_bits[N_HV_CHN];
#define DEMAND_CHANGED     (1<<0)
#define RAMP_UP            (1<<1)
#define RAMP_DOWN          (1<<2)
#define CUR_LIMIT_CHANGED  (1<<3)

float xdata v_actual[N_HV_CHN];

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

/* CSR control bits */
#define CONTROL_HV_ON      (1<<0)
#define CONTROL_REGULATION (1<<1)
#define CONTROL_IDLE       (1<<2)

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
} xdata user_data[N_HV_CHN];

MSCB_INFO_VAR code variables[] = {

   1, UNIT_BYTE,            0, 0,           0, "Control", &user_data[0].control,     // 0
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "Vdemand", &user_data[0].v_demand,    // 1
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "Vmeas",   &user_data[0].v_meas,      // 2
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "Imeas",   &user_data[0].i_meas,      // 3
   1, UNIT_BYTE,            0, 0,           0, "Status",  &user_data[0].status,      // 4
   1, UNIT_COUNT,           0, 0,           0, "TripCnt", &user_data[0].trip_cnt,    // 5

   2, UNIT_VOLT,            0, 0,           0, "RampUp",  &user_data[0].ramp_up,     // 6
   2, UNIT_VOLT,            0, 0,           0, "RampDown",&user_data[0].ramp_down,   // 7
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "Vlimit",  &user_data[0].v_limit,     // 8 
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "Ilimit",  &user_data[0].i_limit,     // 9
   1, UNIT_COUNT,           0, 0,           0, "TripMax", &user_data[0].trip_max,    // 10

   /* calibration constants */
   4, UNIT_FACTOR,          0, 0, MSCBF_FLOAT, "ADCgain", &user_data[0].adc_gain,    // 11
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "ADCofs",  &user_data[0].adc_offset,  // 12
   4, UNIT_FACTOR,          0, 0, MSCBF_FLOAT, "DACgain", &user_data[0].dac_gain,    // 13
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "DACofs",  &user_data[0].dac_offset,  // 14
   4, UNIT_FACTOR,          0, 0, MSCBF_FLOAT, "CURgain", &user_data[0].cur_gain,    // 15
   4, UNIT_AMPERE, PRFX_MICRO, 0, MSCBF_FLOAT, "CURofs",  &user_data[0].cur_offset,  // 16

   4, UNIT_CELSIUS,         0, 0, MSCBF_FLOAT, "Temp",    &user_data[0].temperature, // 17
   4, UNIT_VOLT,            0, 0, MSCBF_FLOAT, "VDAC",    &user_data[0].v_dac,       // 18
   0
};

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void write_dac(unsigned char channel, unsigned short value) reentrant;
void write_adc(unsigned char a, unsigned char d);
void user_write(unsigned char index) reentrant;
void ramp_hv(unsigned char channel);

/*---- User init function ------------------------------------------*/

extern SYS_INFO sys_info;

void user_init(unsigned char init)
{
   unsigned char i;

   P2MDOUT = 0xF0; // P2.4-7: enable Push/Pull for LEDs

   SET_ADDR = 1;   // use button as input

   /* initial nonzero EEPROM values */
   if (init) {
      for (i=0 ; i<N_HV_CHN ; i++) {
         user_data[i].v_demand = 0;
         user_data[i].trip_cnt = 0;
         user_data[i].ramp_up = 0;
         user_data[i].ramp_down = 0;
         user_data[i].v_limit = 3000;
         user_data[i].i_limit = 3000;
         user_data[i].trip_max = 0;
   
         user_data[i].adc_gain = 1;
         user_data[i].adc_offset = 0;
         user_data[i].dac_gain = 1;
         user_data[i].dac_offset = 0;
         user_data[i].cur_gain = 1;
         user_data[i].cur_offset = 0;
      }
   }

   /* default control register */
   for (i=0 ; i<N_HV_CHN ; i++) {
      user_data[i].control = CONTROL_REGULATION;
      user_data[i].status = 0;
      user_data[i].temperature = 0;
   }

   JU1 = 1;
   JU2 = 1;

   /* set-up DAC & ADC */
   DAC_CLR = 1;
   ADC_NRES = 1;
   ADC_NRDY = 1; // input
   ADC_DIN = 1;  // input

   write_adc(REG_FILTER, ADC_SF_VALUE);

   /* force update */
   for (i=0 ; i<N_HV_CHN ; i++)
      chn_bits[i] = DEMAND_CHANGED | CUR_LIMIT_CHANGED;

   /* LED normally off (non-inverted) */
   for (i=0 ; i<N_HV_CHN ; i++)
      led_mode(i, 0);
}

/*---- User write function -----------------------------------------*/

#pragma NOAREGS

void user_write(unsigned char index) reentrant
{
   if (index == 0 || index == 1 || index == 4) {
      /* reset trip */
      user_data[cur_sub_addr()].status &= ~STATUS_ILIMIT;

      /* indicate new demand voltage */
      chn_bits[cur_sub_addr()] |= DEMAND_CHANGED;
   }

   /* re-check voltage limit */
   if (index == 8) {
      chn_bits[cur_sub_addr()] |= DEMAND_CHANGED;
   }
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

/*---- DAC functions -----------------------------------------------*/

unsigned char code dac_index[] = {3, 2, 1, 0 };

void write_dac(unsigned char channel, unsigned short value) reentrant
{
unsigned char i, m, b;

   /* do mapping */
   channel = dac_index[channel % 4];

   DAC_NCS = 0; // chip select

   // command 3: write and update
   for (i=0,m=8 ; i<4 ; i++) {
      DAC_SCK = 0;
      DAC_DIN = (3 & m) > 0;
      delay_us(OPT_DELAY);
      DAC_SCK = 1;
      delay_us(OPT_DELAY);
      m >>= 1;
   }

   // channel address
   for (i=0,m=8 ; i<4 ; i++) {
      DAC_SCK = 0;
      DAC_DIN = (channel & m) > 0;
      delay_us(OPT_DELAY);
      DAC_SCK = 1;
      delay_us(OPT_DELAY);
      m >>= 1;
   }

   // MSB
   b = value >> 8;
   for (i=0,m=0x80 ; i<8 ; i++) {
      DAC_SCK = 0;
      DAC_DIN = (b & m) > 0;
      delay_us(OPT_DELAY);
      DAC_SCK = 1;
      delay_us(OPT_DELAY);
      m >>= 1;
   }

   // LSB
   b = value & 0xFF;
   for (i=0,m=0x80 ; i<8 ; i++) {
      DAC_SCK = 0;
      DAC_DIN = (b & m) > 0;
      delay_us(OPT_DELAY);
      DAC_SCK = 1;
      delay_us(OPT_DELAY);
      m >>= 1;
   }

   DAC_NCS = 1; // remove chip select
   delay_us(OPT_DELAY);
}

/*---- ADC functions -----------------------------------------------*/

void write_adc(unsigned char a, unsigned char d)
{
   unsigned char i, m;

   /* write to communication register */

   ADC_NCS = 0;
   
   /* write zeros to !WEN and R/!W */
   for (i=0 ; i<4 ; i++) {
      ADC_SCLK = 0;
      delay_us(OPT_DELAY);
      ADC_DIN  = 0;
      delay_us(OPT_DELAY);
      ADC_SCLK = 1;
      delay_us(OPT_DELAY);
   }

   /* register address */
   for (i=0,m=8 ; i<4 ; i++) {
      ADC_SCLK = 0;
      delay_us(OPT_DELAY);
      ADC_DIN  = (a & m) > 0;
      delay_us(OPT_DELAY);
      ADC_SCLK = 1;
      delay_us(OPT_DELAY);
      m >>= 1;
   }

   ADC_NCS = 1;
   delay_us(OPT_DELAY);

   /* write to selected data register */

   ADC_NCS = 0;

   for (i=0,m=0x80 ; i<8 ; i++) {
      ADC_SCLK = 0;
      delay_us(OPT_DELAY);
      ADC_DIN  = (d & m) > 0;
      delay_us(OPT_DELAY);
      ADC_SCLK = 1;
      delay_us(OPT_DELAY);
      m >>= 1;
   }

   ADC_NCS = 1;
   delay_us(OPT_DELAY);
}

void read_adc8(unsigned char a, unsigned char *d)
{
   unsigned char i, m;

   /* write to communication register */

   ADC_NCS = 0;
   
   /* write zero to !WEN and one to R/!W */
   for (i=0 ; i<4 ; i++) {
      ADC_SCLK = 0;
      delay_us(OPT_DELAY);
      ADC_DIN  = (i == 1);
      delay_us(OPT_DELAY);
      ADC_SCLK = 1;
      delay_us(OPT_DELAY);
   }

   /* register address */
   for (i=0,m=8 ; i<4 ; i++) {
      ADC_SCLK = 0;
      delay_us(OPT_DELAY);
      ADC_DIN  = (a & m) > 0;
      delay_us(OPT_DELAY);
      ADC_SCLK = 1;
      delay_us(OPT_DELAY);
      m >>= 1;
   }

   ADC_NCS = 1;
   delay_us(OPT_DELAY);

   /* read from selected data register */

   ADC_NCS = 0;

   for (i=0,m=0x80,*d=0 ; i<8 ; i++) {
      ADC_SCLK = 0;
      delay_us(OPT_DELAY);
      if (ADC_DOUT)
         *d |= m;
      ADC_SCLK = 1;
      delay_us(OPT_DELAY);
      m >>= 1;
   }

   ADC_NCS = 1;
   delay_us(OPT_DELAY);
}

void read_adc24(unsigned char a, unsigned long *d)
{
   unsigned char i, m;

   /* write to communication register */

   ADC_NCS = 0;
   
   /* write zero to !WEN and one to R/!W */
   for (i=0 ; i<4 ; i++) {
      ADC_SCLK = 0;
      delay_us(OPT_DELAY);
      ADC_DIN  = (i == 1);
      delay_us(OPT_DELAY);
      ADC_SCLK = 1;
      delay_us(OPT_DELAY);
   }

   /* register address */
   for (i=0,m=8 ; i<4 ; i++) {
      ADC_SCLK = 0;
      delay_us(OPT_DELAY);
      ADC_DIN  = (a & m) > 0;
      delay_us(OPT_DELAY);
      ADC_SCLK = 1;
      delay_us(OPT_DELAY);
      m >>= 1;
   }

   ADC_NCS = 1;
   delay_us(OPT_DELAY);

   /* read from selected data register */

   ADC_NCS = 0;

   for (i=0,*d=0 ; i<24 ; i++) {
      *d <<= 1;
      ADC_SCLK = 0;
      delay_us(OPT_DELAY);
      delay_us(OPT_DELAY);
      *d |= ADC_DOUT;
      ADC_SCLK = 1;
      delay_us(OPT_DELAY);
   }

   ADC_NCS = 1;
   delay_us(OPT_DELAY);
}

unsigned char code adc_index[8] = {7, 6, 5, 4, 3, 2, 1, 0 };

void adc_read(unsigned char channel, float *value)
{
   unsigned long d, start_time;
   unsigned char i;

   /* start conversion */
   channel = adc_index[channel % 8];
   write_adc(REG_CONTROL, channel << 4 | 0x0F); // adc_chn, +2.56V range
   write_adc(REG_MODE, 2);                      // single conversion

   start_time = time();

   while (ADC_NRDY) {
      yield();
      for (i=0 ; i<N_HV_CHN ; i++)
         ramp_hv(i);      // do ramping while reading ADC

      /* abort if no ADC ready after 300ms */
      if (time() - start_time > 30) {
         
         /* reset ADC */
         ADC_NRES = 0;
         delay_ms(100);
         ADC_NRES = 1;
         delay_ms(300);

         write_adc(REG_FILTER, ADC_SF_VALUE);

         *value = 0;
         return;
      }
   }

   read_adc24(REG_ADCDATA, &d);

   /* convert to volts */
   *value = ((float)d / (1l<<24)) * 2.56;

   /* round result to 6 digits */
   *value = floor(*value*1E6+0.5)/1E6;
}

/*------------------------------------------------------------------*/

void set_hv(unsigned char channel, float value) reentrant
{
   unsigned short d;

   /* check for limit */
   if (value > user_data[channel].v_limit) {
      value = user_data[channel].v_limit;
      user_data[channel].status |= STATUS_VLIMIT;
   } else
      user_data[channel].status &= ~STATUS_VLIMIT;

   led_mode(channel, value > 10);

   /* apply correction */
   value = value * user_data[channel].dac_gain + user_data[channel].dac_offset;
   if (value < 0)
      value = 0;

   /* convert HV to voltage */
   value = value / DIVIDER;

   /* convert to DAC units */
   d = (unsigned short) ((value / 2.5 * 65536) + 0.5);

   /* write dac */
   write_dac(channel, d);
}

/*------------------------------------------------------------------*/

void read_hv(unsigned char channel)
{
   float hv;

   /* read voltage channel */
   adc_read(channel*2, &hv);

   /* convert to HV */
   hv *= DIVIDER;

   /* apply calibration */
   hv = hv * user_data[channel].adc_gain + user_data[channel].adc_offset;

   DISABLE_INTERRUPTS;
   user_data[channel].v_meas = hv;
   ENABLE_INTERRUPTS;
}

/*------------------------------------------------------------------*/

void set_current_limit(float value) reentrant
{
   unsigned short d;

   /* convert current to voltage */
   value = value / DIVIDER * CUR_MULT * RCURR / 1E6;

   /* convert to DAC units */
   d = (unsigned short) ((value / 2.5 * 65536) + 0.5);

   /* write current dac */
   write_dac(7, d);
}

/*------------------------------------------------------------------*/

void check_current(unsigned char channel)
{
   if (user_data[channel].i_meas > user_data[channel].i_limit) {

      /* zero output voltage */
      set_hv(channel, 0);
      v_actual[channel] = 0;
      user_data[channel].v_dac = 0;

      /* stop possible ramping */
      chn_bits[channel] &= ~DEMAND_CHANGED;
      chn_bits[channel] &= ~RAMP_UP;
      chn_bits[channel] &= ~RAMP_DOWN;
      user_data[channel].status &= ~(STATUS_RAMP_UP | STATUS_RAMP_DOWN);

      /* raise trip flag */
      user_data[channel].status |= STATUS_ILIMIT;
      user_data[channel].trip_cnt++;

      /* check for trip recovery */
      if (user_data[channel].trip_cnt < user_data[channel].trip_max) {
         /* clear trip */
         user_data[channel].status &= ~STATUS_ILIMIT;

         /* force ramp up */
         chn_bits[channel] |= DEMAND_CHANGED;
      }
   }

   if (user_data[channel].status & STATUS_ILIMIT)
      led_blink(channel, 1, 500);
}

/*------------------------------------------------------------------*/

void read_current(unsigned char channel)
{
   float current;

   /* read current channel */
   adc_read(channel*2+1, &current);

   /* correct opamp gain, divider & curr. resist, microamp */
   current = current / CUR_MULT * DIVIDER / RCURR * 1E6;

   /* calibrate */
   current = current * user_data[channel].cur_gain + user_data[channel].cur_offset;

   DISABLE_INTERRUPTS;
   user_data[channel].i_meas = current;
   ENABLE_INTERRUPTS;
}

/*------------------------------------------------------------------*/

void ramp_hv(unsigned char channel)
{
   static unsigned long xdata t[N_HV_CHN];
   unsigned char delta;

   /* only process ramping when HV is on and not tripped */
   if ((user_data[channel].control & CONTROL_HV_ON) && 
       !(user_data[channel].status & STATUS_ILIMIT)) {

      if (chn_bits[channel] & DEMAND_CHANGED) {
         /* start ramping */

         if (user_data[channel].v_demand > v_actual[channel] && 
             user_data[channel].ramp_up > 0) {
            /* ramp up */
            chn_bits[channel] |= RAMP_UP;
            chn_bits[channel] &= ~RAMP_DOWN;
            user_data[channel].status |= STATUS_RAMP_UP;
            user_data[channel].status &= ~STATUS_RAMP_DOWN;
            chn_bits[channel] &= ~DEMAND_CHANGED;
         }

         if (user_data[channel].v_demand < v_actual[channel] && 
             user_data[channel].ramp_down > 0) {
            /* ramp down */
            chn_bits[channel] &= ~RAMP_UP;
            chn_bits[channel] |= RAMP_DOWN;
            user_data[channel].status &= ~STATUS_RAMP_UP;
            user_data[channel].status |= STATUS_RAMP_DOWN;
            chn_bits[channel] &= ~DEMAND_CHANGED;
         }

         /* remember start time */
         t[channel] = time();
      }

      /* ramp up */
      if (chn_bits[channel] & RAMP_UP) {
         delta = time() - t[channel];
         if (delta) {
            v_actual[channel] += (float) user_data[channel].ramp_up * delta / 100.0;

            if (v_actual[channel] >= user_data[channel].v_demand) {
               /* finish ramping */

               v_actual[channel] = user_data[channel].v_demand;
               user_data[channel].v_dac = v_actual[channel];
               chn_bits[channel] &= ~RAMP_UP;
               user_data[channel].status &= ~STATUS_RAMP_UP;

            }

            set_hv(channel, v_actual[channel]);
            t[channel] = time();
         }
      }

      /* ramp down */
      if (chn_bits[channel] & RAMP_DOWN) {
         delta = time() - t[channel];
         if (delta) {
            v_actual[channel] -= (float) user_data[channel].ramp_down * delta / 100.0;

            if (v_actual[channel] <= user_data[channel].v_demand) {
               /* finish ramping */

               v_actual[channel] = user_data[channel].v_demand;
               user_data[channel].v_dac = v_actual[channel];
               chn_bits[channel] &= ~RAMP_DOWN;
               user_data[channel].status &= ~STATUS_RAMP_DOWN;

            }

            set_hv(channel, v_actual[channel]);
            t[channel] = time();
         }
      }
   }
}

/*------------------------------------------------------------------*/

void regulation(unsigned char channel)
{
   /* only if HV on and not ramping */
   if ((user_data[channel].control & CONTROL_HV_ON) && 
       (chn_bits[channel] & (RAMP_UP | RAMP_DOWN)) == 0 &&
       !(user_data[channel].status & STATUS_ILIMIT)) {
      if (user_data[channel].control & CONTROL_REGULATION) {

         v_actual[channel] = user_data[channel].v_demand;

         /* correct if difference is at least half a LSB */
         if (fabs(user_data[channel].v_demand - user_data[channel].v_meas) / DIVIDER / 2.5 * 65536 > 0.5) {
   
            user_data[channel].v_dac += user_data[channel].v_demand - user_data[channel].v_meas;
   
            /* only allow +-2V fine regulation range */
            if (user_data[channel].v_dac < user_data[channel].v_demand - 2)
               user_data[channel].v_dac = user_data[channel].v_demand - 2;
   
            if (user_data[channel].v_dac > user_data[channel].v_demand + 2)
               user_data[channel].v_dac = user_data[channel].v_demand + 2;
   
            chn_bits[channel] &= ~DEMAND_CHANGED;
            set_hv(channel, user_data[channel].v_dac);
         }

      } else {
         /* set voltage directly */
         if (chn_bits[channel] & DEMAND_CHANGED) {
            v_actual[channel] = user_data[channel].v_demand;
            user_data[channel].v_dac = user_data[channel].v_demand;
            set_hv(channel, user_data[channel].v_demand);

            chn_bits[channel] &= ~DEMAND_CHANGED;
         }
      }
   }

   /* if HV switched off, set DAC to zero */
   if (!(user_data[channel].control & CONTROL_HV_ON) && 
       (chn_bits[channel] & DEMAND_CHANGED)) {

      user_data[channel].v_dac = 0;
      set_hv(channel, 0);
      chn_bits[channel] &= ~DEMAND_CHANGED;
   }
}

/*------------------------------------------------------------------*/

#ifdef NOT_RELIABLE

void read_temperature(void)
{
   float temperature;
   unsigned char i;

   REF0CN = 0x0E; // temperature sensor on
   AD0EN = 1;     // turn on ADC

   /* set MUX */
   AMX0P = 0x1E; // Temp sensor
   AMX0N = 0x1F; // GND

   DISABLE_INTERRUPTS;

   AD0INT = 0;
   AD0BUSY = 1;
   while (AD0INT == 0);

   ENABLE_INTERRUPTS;

   temperature = ADC0L + (ADC0H & 0x03) * 256;

   /* convert to mV */
   temperature = temperature / 1023.0 * 3000;

   /* convert to deg. C */
   temperature = (temperature - 897) / 3.35;

   /* offset correction using prototype */
   temperature -= 27;

   /* strip fractional part */
   temperature = (int) (temperature + 0.5);
   
   for (i=0 ; i<N_HV_CHN ; i++) {
      DISABLE_INTERRUPTS;
      user_data[i].temperature = temperature;
      ENABLE_INTERRUPTS;

      /* read node configuration */
      if (JU1 == 0)
         user_data[i].status |= STATUS_NEGATIVE;
      else
         user_data[i].status &= ~STATUS_NEGATIVE;

      if (JU2 == 0)
         user_data[i].status |= STATUS_LOWCUR;
      else
         user_data[i].status &= ~STATUS_LOWCUR;
   }
}

#endif

/*---- User loop function ------------------------------------------*/

void user_loop(void)
{
   unsigned char channel;

   /* loop over all HV channels */
   for (channel=0 ; channel<N_HV_CHN ; channel++) {

      if ((user_data[0].control & CONTROL_IDLE) == 0) {

         /* outcommented until implemented in HW
         if (chn_bits[channel] & CUR_LIMIT_CHANGED) {
            set_current_limit(user_data[channel].i_limit);
            chn_bits[channel] &= ~CUR_LIMIT_CHANGED;
         }
         */

         /* instead, use software limit */
         check_current(channel);
   
         read_hv(channel);
         ramp_hv(channel);
         regulation(channel);
         read_current(channel);
      }
   }

   // read_temperature();
}
