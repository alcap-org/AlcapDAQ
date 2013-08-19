/********************************************************************\

  Name:         scs_500.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol 
                for SCS-500 analog I/O

  $Log: scs_500.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:18  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.20  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.19  2003/07/14 10:19:14  midas
  Version 1.4.8

  Revision 1.18  2003/05/14 15:13:43  midas
  Limit ADC_average to 8

  Revision 1.17  2003/05/14 15:08:04  midas
  Switched P1 back to open drain

  Revision 1.16  2003/05/14 14:56:44  midas
  Switched P1 to push-pull

  Revision 1.15  2003/03/19 16:35:03  midas
  Eliminated configuration parameters

  Revision 1.14  2003/03/06 16:08:50  midas
  Protocol version 1.3 (change node name)

  Revision 1.13  2003/02/19 16:05:36  midas
  Added 'init' parameter to user_init

  Revision 1.12  2002/11/22 15:43:03  midas
  Made user_write reentrant

  Revision 1.11  2002/11/20 12:02:59  midas
  Added yield()

  Revision 1.10  2002/10/28 14:26:30  midas
  Changes from Japan

  Revision 1.9  2002/10/23 05:46:04  midas
  Fixed bug with wrong gain bits

  Revision 1.8  2002/10/22 15:05:36  midas
  Added gain and offset calibrations

  Revision 1.7  2002/10/16 15:24:38  midas
  Added units in descriptor

  Revision 1.6  2002/10/09 11:06:46  midas
  Protocol version 1.1

  Revision 1.5  2002/10/03 15:31:53  midas
  Various modifications

  Revision 1.4  2002/08/12 12:11:49  midas
  No voltage output in debug mode

  Revision 1.3  2002/08/08 06:47:27  midas
  Fixed typo

  Revision 1.2  2002/07/12 15:19:53  midas
  Added NOAREGS

  Revision 1.1  2002/07/12 08:38:13  midas
  Fixed LCD recognition

  Revision 1.2  2002/07/10 09:52:55  midas
  Finished EEPROM routines

  Revision 1.1  2002/07/09 15:31:32  midas
  Initial Revision

\********************************************************************/

#include <stdio.h>
#include "mscb.h"

extern bit FREEZE_MODE;
extern bit DEBUG_MODE;

char code node_name[] = "SCS-500";

sbit SR_CLOCK = P0 ^ 4;         // Shift register clock
sbit SR_STROBE = P0 ^ 5;        // Storage register clock
sbit SR_DATA = P0 ^ 6;          // Serial data

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   float adc[8];
   float dac0, dac1;
   unsigned char p1;
   unsigned char adc_average;
   unsigned char gain[8];       // PGA bits
   float gain_cal;              // gain calibration
   float bip_cal;               // bipolar zero offset
} idata user_data;

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


MSCB_INFO_VAR code variables[] = {
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC0", &user_data.adc[0],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC1", &user_data.adc[1],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC2", &user_data.adc[2],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC3", &user_data.adc[3],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC4", &user_data.adc[4],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC5", &user_data.adc[5],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC6", &user_data.adc[6],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC7", &user_data.adc[7],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC0", &user_data.dac0,
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC1", &user_data.dac1,
   1, UNIT_BYTE, 0, 0, 0, "P1", &user_data.p1,

   1, UNIT_COUNT, 0, 0, 0, "ADCAvrg", &user_data.adc_average,
   1, UNIT_BYTE, 0, 0, 0, "Gain0", &user_data.gain[0],
   1, UNIT_BYTE, 0, 0, 0, "Gain1", &user_data.gain[1],
   1, UNIT_BYTE, 0, 0, 0, "Gain2", &user_data.gain[2],
   1, UNIT_BYTE, 0, 0, 0, "Gain3", &user_data.gain[3],
   1, UNIT_BYTE, 0, 0, 0, "Gain4", &user_data.gain[4],
   1, UNIT_BYTE, 0, 0, 0, "Gain5", &user_data.gain[5],
   1, UNIT_BYTE, 0, 0, 0, "Gain6", &user_data.gain[6],
   1, UNIT_BYTE, 0, 0, 0, "Gain7", &user_data.gain[7],
   4, UNIT_FACTOR, 0, 0, MSCBF_FLOAT, "GainCal", &user_data.gain_cal,
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "BipCal", &user_data.bip_cal,
   0
};

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void user_write(unsigned char index) reentrant;
void write_gain(void) reentrant;

/*---- User init function ------------------------------------------*/

extern SYS_INFO sys_info;

void user_init(unsigned char init)
{
   unsigned char i;

   AMX0CF = 0x00;               // select single ended analog inputs
   ADC0CF = 0xE0;               // 16 system clocks, gain 1
   ADC0CN = 0x80;               // enable ADC 

   REF0CN = 0x03;               // enable internal reference
   DAC0CN = 0x80;               // enable DAC0
   DAC1CN = 0x80;               // enable DAC1

   // uncomment following line for P1 push-pull mode
   //PRT1CF = 0xFF;

   /* initial EEPROM value */
   if (init) {
      user_data.adc_average = 8;
      for (i = 0; i < 8; i++)
         user_data.gain[i] = 0;
      user_data.gain_cal = 1;
      user_data.bip_cal = 0;

      user_data.dac0 = 0;
      user_data.dac1 = 0;
      user_data.p1 = 0xff;
   }

   if (user_data.gain_cal < 0.1 || user_data.gain_cal > 10)
      user_data.gain_cal = 1;

   if (user_data.bip_cal < -1 || user_data.bip_cal > 1)
      user_data.bip_cal = 0;

   /* write P1 and DACs */
   user_write(8);
   user_write(9);
   user_write(10);

   /* write gains */
   write_gain();

   // DEBUG_MODE = 1;
}

/*---- User write function -----------------------------------------*/

#pragma NOAREGS

void write_gain(void) reentrant
{
   unsigned char i;

   SR_STROBE = 0;
   SR_CLOCK = 0;

   for (i = 0; i < 4; i++) {
      SR_DATA = ((user_data.gain[3 - i] & 0x02) > 0);   // first bit ext. PGA
      SR_CLOCK = 1;
      SR_CLOCK = 0;

      SR_DATA = ((user_data.gain[3 - i] & 0x01) > 0);   // second bit ext. PGA
      SR_CLOCK = 1;
      SR_CLOCK = 0;
   }

   for (i = 0; i < 4; i++) {
      SR_DATA = ((user_data.gain[7 - i] & 0x02) > 0);   // first bit ext. PGA
      SR_CLOCK = 1;
      SR_CLOCK = 0;

      SR_DATA = ((user_data.gain[7 - i] & 0x01) > 0);   // second bit ext. PGA
      SR_CLOCK = 1;
      SR_CLOCK = 0;
   }

   SR_DATA = 0;
   SR_STROBE = 1;
   SR_STROBE = 0;
}

void user_write(unsigned char index) reentrant
{
   unsigned short d;

   switch (index) {
   case 8:                     // DAC0
      /* assume -10V..+10V range */
      d = ((user_data.dac0 + 10) / 20) * 0x1000;
      if (d >= 0x1000)
         d = 0x0FFF;

#ifdef CPU_ADUC812
      DAC0H = d >> 8;
      DAC0L = d & 0xFF;
#endif
#ifdef CPU_C8051F000
      DAC0L = d & 0xFF;
      DAC0H = d >> 8;
#endif
      break;

   case 9:                     // DAC1
      /* assume -10V..+10V range */
      d = ((user_data.dac1 + 10) / 20) * 0x1000;
      if (d >= 0x1000)
         d = 0x0FFF;

#ifdef CPU_ADUC812
      DAC1H = d >> 8;
      DAC1L = d & 0xFF;
#endif
#ifdef CPU_C8051F000
      DAC1L = d & 0xFF;
      DAC1H = d >> 8;
#endif
      break;

   case 10:                    // p1 
      P1 = user_data.p1;
      break;

   case 11:                    // adc_average
      if (user_data.adc_average > 8)
         user_data.adc_average = 8;
      break;
   }

   if (index > 10)
      write_gain();
}

/*---- User read function ------------------------------------------*/

unsigned char user_read(unsigned char index)
{
   if (index == 10)
      user_data.p1 = P1;

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

/*---- User loop function ------------------------------------------*/

void adc_read(channel, float *d)
{
   unsigned long value;
   unsigned int i, n;
   float gvalue;

   AMX0SL = channel & 0x0F;
   ADC0CF = 0xE0;               // 16 system clocks, gain 1

   n = 1 << (user_data.adc_average + 4);

   value = 0;
   for (i = 0; i < n; i++) {
      DISABLE_INTERRUPTS;

      ADCINT = 0;
      ADBUSY = 1;
      while (!ADCINT);          // wait until conversion ready, does NOT work with ADBUSY!

      ENABLE_INTERRUPTS;

      value += (ADC0L | (ADC0H << 8));
      yield();
   }

   if (user_data.adc_average)
      value >>= (user_data.adc_average);

   /* convert to volts */
   gvalue = value / 65536.0 * 2.5;

   /* subtract 1V for bipolar mode */
   if (user_data.gain[channel] & 0x04)
      gvalue = gvalue - 1;

   /* external voltage divider */
   gvalue *= 10 * user_data.gain_cal;

   /* correct for bipolar offset */
   if (user_data.gain[channel] & 0x04)
      gvalue += user_data.bip_cal;

   /* external PGA */
   switch (user_data.gain[channel] & 0x03) {
   case 0x00:
      gvalue /= 1.0;
      break;
   case 0x01:
      gvalue /= 10.0;
      break;
   case 0x02:
      gvalue /= 100.0;
      break;
   case 0x03:
      gvalue /= 1000.0;
      break;
   }

   DISABLE_INTERRUPTS;
   *d = gvalue;
   ENABLE_INTERRUPTS;
}

void user_loop(void)
{
   static unsigned char i = 0;

   adc_read(i, &user_data.adc[i]);
   i = (i + 1) % 8;
}
