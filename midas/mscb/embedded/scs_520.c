/********************************************************************\

  Name:         scs_520.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol 
                for SCS-520 analog I/O with current option

  $Log: scs_520.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:18  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.8  2004/06/09 12:27:29  midas
  Re-enabled DAC

  Revision 1.7  2004/05/12 13:10:48  midas
  Removed variables (stack overflow!)

  Revision 1.6  2004/04/07 11:06:17  midas
  Version 1.7.1

  Revision 1.5  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.4  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.3  2003/06/06 15:19:32  midas
  Set gains asynchronously

  Revision 1.2  2003/06/05 14:47:50  midas
  Added channel gain/offset/bip offset

  Revision 1.1  2003/06/05 07:26:46  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include "mscb.h"

extern bit FREEZE_MODE;
extern bit DEBUG_MODE;

char code node_name[] = "SCS-520";

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

sbit UNI_BIP = P0 ^ 3;          // Unipolar/bipolar switch
sbit SR_CLOCK = P0 ^ 4;         // Shift register clock
sbit SR_STROBE = P0 ^ 5;        // Storage register clock
sbit SR_DATA = P0 ^ 6;          // Serial data
sbit SR_DATAIN = P0 ^ 7;        // Serial data input

bit gain_flag;                  // set when new gains need to be set

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   float adc[8];
   float dac0, dac1;
   unsigned char p1;
   unsigned char adc_average;   // average 2^(adc_average+4)
   unsigned char range[8];      // see below
   char ofs[8];                 // channel offset in mV
   char gain[8];                // channel gain
//##   char bip_ofs[8];             // bipolar offset in 10mV
   unsigned char cur_mode;      // current bit for each channel
} idata user_data;

/* Usage of range:

 Bipol.     Ext.  PGA    Gain        Full Range

  Bit2      Bit1 Bit0  

   0         0    0         1         0 -   10V
   0         0    1        10         0 -    1V
   0         1    0       100         0 -  0.1V
   0         1    1      1000         0 - 0.01V

   1         0    0         1      -10V -   10V
   1         0    1        10       -1V -    1V
   1         1    0       100     -0.1V -  0.1V
   1         1    1      1000    -0.01V - 0.01V
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
   1, UNIT_BYTE, 0, 0, 0, "Range0", &user_data.range[0],
   1, UNIT_BYTE, 0, 0, 0, "Range1", &user_data.range[1],
   1, UNIT_BYTE, 0, 0, 0, "Range2", &user_data.range[2],
   1, UNIT_BYTE, 0, 0, 0, "Range3", &user_data.range[3],
   1, UNIT_BYTE, 0, 0, 0, "Range4", &user_data.range[4],
   1, UNIT_BYTE, 0, 0, 0, "Range5", &user_data.range[5],
   1, UNIT_BYTE, 0, 0, 0, "Range6", &user_data.range[6],
   1, UNIT_BYTE, 0, 0, 0, "Range7", &user_data.range[7],

   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "Ofs0", &user_data.ofs[0],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "Ofs1", &user_data.ofs[1],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "Ofs2", &user_data.ofs[2],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "Ofs3", &user_data.ofs[3],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "Ofs4", &user_data.ofs[4],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "Ofs5", &user_data.ofs[5],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "Ofs6", &user_data.ofs[6],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "Ofs7", &user_data.ofs[7],

   1, UNIT_FACTOR, 0, 0, MSCBF_SIGNED, "Gain0", &user_data.gain[0],
   1, UNIT_FACTOR, 0, 0, MSCBF_SIGNED, "Gain1", &user_data.gain[1],
   1, UNIT_FACTOR, 0, 0, MSCBF_SIGNED, "Gain2", &user_data.gain[2],
   1, UNIT_FACTOR, 0, 0, MSCBF_SIGNED, "Gain3", &user_data.gain[3],
   1, UNIT_FACTOR, 0, 0, MSCBF_SIGNED, "Gain4", &user_data.gain[4],
   1, UNIT_FACTOR, 0, 0, MSCBF_SIGNED, "Gain5", &user_data.gain[5],
   1, UNIT_FACTOR, 0, 0, MSCBF_SIGNED, "Gain6", &user_data.gain[6],
   1, UNIT_FACTOR, 0, 0, MSCBF_SIGNED, "Gain7", &user_data.gain[7],

/*##
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "BipOfs0",
   &user_data.bip_ofs[0],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "BipOfs1",
   &user_data.bip_ofs[1],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "BipOfs2",
   &user_data.bip_ofs[2],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "BipOfs3",
   &user_data.bip_ofs[3],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "BipOfs4",
   &user_data.bip_ofs[4],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "BipOfs5",
   &user_data.bip_ofs[5],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "BipOfs6",
   &user_data.bip_ofs[6],
   1, UNIT_VOLT, PRFX_MILLI, 0, MSCBF_SIGNED, "BipOfs7",
   &user_data.bip_ofs[7],
*/

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

   /* push-pull:
      P0.0    TX
      P0.1
      P0.2
      P0.3

      P0.4    SR_CLOCK
      P0.5    SR_STROBE
      P0.6    SR_DATA 
      P0.7
    */
   //PRT0CF = 0x71;
   PRT0CF = 0x41;

   /* P1 push-pull if UDN2981 driver installed, open drain if input */
   PRT1CF = 0xFF;

   /* initial EEPROM value */
   if (init) {
      user_data.adc_average = 8;
      for (i = 0; i < 8; i++) {
         user_data.range[i] = 0;
         user_data.ofs[i] = 0;
//##         user_data.bip_ofs[i] = 0;
      }
      user_data.dac0 = 0;
      user_data.dac1 = 0;
      user_data.p1 = 0xff;
   }

   /* write P1, DACs and UNI_BIP */
   user_write(8);
   user_write(9);
   user_write(10);
   user_write(12);

   /* write gains and read current mode */
   write_gain();
}

/*---- User write function -----------------------------------------*/

#pragma NOAREGS

void write_gain(void) reentrant
{
   unsigned char i, d;

   SR_STROBE = 0;
   SR_CLOCK = 0;
   SR_DATAIN = 1;
   d = 0;

   for (i = 0; i < 4; i++) {
      if (!SR_DATAIN)
         d |= (1 << (i * 2));

      SR_DATA = ((user_data.range[3 - i] & 0x02) > 0);  // first bit ext. PGA
      SR_CLOCK = 1;
      delay_us(10);
      SR_CLOCK = 0;
      delay_us(10);

      if (!SR_DATAIN)
         d |= (1 << (i * 2 + 1));

      SR_DATA = ((user_data.range[3 - i] & 0x01) > 0);  // second bit ext. PGA
      SR_CLOCK = 1;
      delay_us(10);
      SR_CLOCK = 0;
      delay_us(10);
   }

   user_data.cur_mode = d;

   for (i = 0; i < 4; i++) {
      SR_DATA = ((user_data.range[7 - i] & 0x02) > 0);  // first bit ext. PGA
      SR_CLOCK = 1;
      delay_us(10);
      SR_CLOCK = 0;
      delay_us(10);

      SR_DATA = ((user_data.range[7 - i] & 0x01) > 0);  // second bit ext. PGA
      SR_CLOCK = 1;
      delay_us(10);
      SR_CLOCK = 0;
      delay_us(10);
   }

   /* strobe for output and next input */
   SR_DATA = 0;
   SR_STROBE = 1;
   delay_us(10);
   SR_STROBE = 0;
   delay_us(10);
}

void user_write(unsigned char index) reentrant
{
   unsigned char i;
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

   case 12:                    // set bipolar/unipoar
      UNI_BIP = (user_data.range[0] & 0x04) > 0;

      for (i = 1; i < 8; i++) {
         if (UNI_BIP)
            user_data.range[i] |= 0x04;
         else
            user_data.range[i] &= ~0x04;
      }

      break;
   }

   if (index >= 12)
      gain_flag = 1;
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

   /* correct for channel offset in mV for a gain of 10 */
   gvalue += user_data.ofs[channel] / 10000.0;

   /* correct for channel gain up to +- 1.28 % */
   gvalue *= (1 + user_data.gain[channel] / 10000.0);

   /* subtract 1V for bipolar mode */
   if (user_data.range[channel] & 0x04)
      gvalue = gvalue - 1;

   /* correct for bipolar offset */
//##   if (user_data.range[channel] & 0x04)
//##      gvalue += user_data.bip_ofs[channel] / 1000.0;

   /* external voltage divider */
   gvalue *= 10;

   /* external PGA */
   switch (user_data.range[channel] & 0x03) {
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

   if (gain_flag) {
      gain_flag = 0;
      write_gain();
   }
}
