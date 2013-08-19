/********************************************************************\

  Name:         scs_500_pt100.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol 
                for SCS-500 unit modified for PT100 sensors

  $Log: scs_500_pt100.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:18  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.2  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.1  2003/05/09 11:31:34  midas
  Added labview files


\********************************************************************/

#include <stdio.h>
#include "mscb.h"

extern bit FREEZE_MODE;
extern bit DEBUG_MODE;

char code node_name[] = "SCS-500-PT100";

/*---- Define channels and configuration parameters returned to
       the CMD_GET_INFO command                                 ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   float temp[8];
} idata user_data;

struct {
   float gain_cal;              // gain calibration
   float ofs_cal;               // offset calibration
} idata user_conf;

float idata gain[8];            // gain resulting from PGA bits

MSCB_INFO_CHN code channel[] = {
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "Temp0", &user_data.temp[0],
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "Temp1", &user_data.temp[1],
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "Temp2", &user_data.temp[2],
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "Temp3", &user_data.temp[3],
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "Temp4", &user_data.temp[4],
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "Temp5", &user_data.temp[5],
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "Temp6", &user_data.temp[6],
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "Temp7", &user_data.temp[7],
   0
};

/* Usage of gain:

 Bipol.     Ext.  PGA          Int. PGA       Gain

  Bit5      Bit4 Bit3       Bit2 Bit1 Bit0
   0         0    0          0    0    0       1
   0         0    0          0    0    1       2
   0         0    0          0    1    0       4
   0         0    0          0    1    1       8
   0         0    0          1    0    0      16
   0         0    0          1    1    0       0.5

   0         0    1          0    0    0      10
   0         1    0          0    0    0     100
   0         1    1          0    0    0    1000

   1  bipolar, represents external jumper setting
*/

MSCB_INFO_CHN code conf_param[] = {
   4, UNIT_FACTOR, 0, 0, MSCBF_FLOAT, "GainCal", &user_conf.gain_cal,
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "OfsCal", &user_conf.ofs_cal,
   0
};

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void user_write(unsigned char channel);

/*---- User init function ------------------------------------------*/

extern SYS_INFO sys_info;

void user_init(void)
{
   AMX0CF = 0x00;               // select single ended analog inputs
   ADC0CF = 0xE0;               // 16 system clocks, gain 1
   ADC0CN = 0x80;               // enable ADC 

   REF0CN = 0x03;               // enable internal reference
   DAC0CN = 0x80;               // enable DAC0
   DAC1CN = 0x80;               // enable DAC1

   /* correct initial EEPROM value */
   if (user_conf.gain_cal < 0.1 || user_conf.gain_cal > 10)
      user_conf.gain_cal = 1;

   if (user_conf.ofs_cal < -1 || user_conf.ofs_cal > 1)
      user_conf.ofs_cal = 0;

   // DEBUG_MODE = 1;
}

/*---- User write function -----------------------------------------*/

#pragma NOAREGS

void user_write(unsigned char channel)
{
   if (channel);
}

/*---- User read function ------------------------------------------*/

unsigned char user_read(unsigned char channel)
{
   if (channel == 0);
   return 0;
}

/*---- User write config function ----------------------------------*/

void user_write_conf(unsigned char channel)
{
   if (channel);
}

/*---- User read config function -----------------------------------*/

void user_read_conf(unsigned char channel)
{
   if (channel);
}

/*---- User function called vid CMD_USER command -------------------*/

unsigned char user_func(unsigned char idata * data_in, unsigned char idata * data_out)
{
   /* echo input data */
   data_out[0] = data_in[0];
   data_out[1] = data_in[1];
   return 2;
}

/*---- User loop function ------------------------------------------*/

void adc_read(channel, float *d)
{
   unsigned long value;
   unsigned int i, n;
   float gvalue;

   AMX0SL = channel & 0x0F;
   ADC0CF = 0xE0;               // 16 system clocks, gain 1

   n = 1 << (8 + 4);

   value = 0;
   for (i = 0; i < n; i++) {
      DISABLE_INTERRUPTS;

      ADCINT = 0;
      ADBUSY = 1;
      while (!ADCINT);          // wait until conversion ready, does NOT work with ADBUSY!

      ENABLE_INTERRUPTS;

      value += (ADC0L | (ADC0H << 8));
      watchdog_refresh();
   }

   value >>= 8;

   /* convert to volts */
   gvalue = value / 65536.0 * 2.5;

   /* external voltage divider */
   gvalue *= 0.5 * user_conf.gain_cal;

   /* convert to Ohms (15V supply, 2kOhm resistor) */
   gvalue = (2000 * gvalue) / (15 - gvalue);

   /* convert Ohms to Celsius, valid -100.100 deg., error <0.04K */
   gvalue -= 100;
   gvalue = 0.0011 * gvalue * gvalue + 2.5558 * gvalue;

   /* correct for offset */
   gvalue += user_conf.ofs_cal;

   DISABLE_INTERRUPTS;
   *d = gvalue;
   ENABLE_INTERRUPTS;
}

void user_loop(void)
{
   static unsigned char i = 0;
   static unsigned long last = 300;
   static bit first = 1;

   adc_read(i, &user_data.temp[i]);
   i = (i + 1) % 8;

   if (!DEBUG_MODE && time() > last + 30) {
      if (first) {
         lcd_clear();
         first = 0;
      }

      last = time();

      lcd_goto(0, 0);
      printf("CH1: %6.1f C   ", user_data.temp[0]);

      lcd_goto(0, 1);
      printf("CH2: %6.1f C   ", user_data.temp[1]);
   }
}
