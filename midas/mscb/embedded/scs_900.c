/********************************************************************\

  Name:         scs_900.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol 
                for SCS-900 analog high precision I/O 

  $Log: scs_900.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:19  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.5  2004/09/10 12:27:23  midas
  Version 1.7.5

  Revision 1.4  2004/04/07 11:06:17  midas
  Version 1.7.1

  Revision 1.3  2004/02/24 13:29:35  midas
  Made bipolar mode working

  Revision 1.2  2004/02/06 12:03:31  midas
  ADC works as well

  Revision 1.1  2004/02/05 16:30:54  midas
  DAC works

\********************************************************************/

#include <stdio.h>
#include <math.h>
#include "mscb.h"

extern bit FREEZE_MODE;
extern bit DEBUG_MODE;

char code node_name[] = "SCS-900";

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

/* AD7718 pins */
sbit ADC_P1   = P1 ^ 1;         // P1
sbit ADC_NRES = P1 ^ 2;         // !Reset
sbit ADC_SCLK = P1 ^ 3;         // Serial Clock
sbit ADC_NCS  = P1 ^ 4;         // !Chip select
sbit ADC_NRDY = P1 ^ 5;         // !Ready
sbit ADC_DOUT = P1 ^ 6;         // Data out
sbit ADC_DIN  = P1 ^ 7;         // Data in

/* LTC2600 pins */
sbit DAC_NCS  = P0 ^ 3;         // !Chip select
sbit DAC_SCK  = P0 ^ 4;         // Serial Clock
sbit DAC_CLR  = P0 ^ 5;         // Clear
sbit DAC_DOUT = P0 ^ 6;         // Data out
sbit DAC_DIN  = P0 ^ 7;         // Data in

/* analog switch */
sbit UNI_DAC = P1 ^ 0;          // DAC bipolar/unipolar
sbit UNI_ADC = P0 ^ 2;          // ADC bipolar/unipolar

/* AD7718 registers */
#define REG_STATUS     0
#define REG_MODE       1
#define REG_CONTROL    2
#define REG_FILTER     3
#define REG_ADCDATA    4
#define REG_ADCOFFSET  5
#define REG_ADCGAIN    6
#define REG_IOCONTROL  7

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   float adc[8];
   float dac[8];
   char  uni_dac;
   char  uni_adc;
   char  adc_25;
} idata user_data;

MSCB_INFO_VAR code variables[] = {

   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC0", &user_data.adc[0],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC1", &user_data.adc[1],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC2", &user_data.adc[2],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC3", &user_data.adc[3],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC4", &user_data.adc[4],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC5", &user_data.adc[5],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC6", &user_data.adc[6],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC7", &user_data.adc[7],

   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC0", &user_data.dac[0],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC1", &user_data.dac[1],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC2", &user_data.dac[2],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC3", &user_data.dac[3],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC4", &user_data.dac[4],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC5", &user_data.dac[5],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC6", &user_data.dac[6],
   4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC7", &user_data.dac[7],

   1, UNIT_BOOLEAN, 0, 0, 0, "Uni DAC", &user_data.uni_dac,
   1, UNIT_BOOLEAN, 0, 0, 0, "Uni ADC", &user_data.uni_adc,
   1, UNIT_BOOLEAN, 0, 0, 0, "2.5V ADC", &user_data.adc_25,

   0
};

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void user_write(unsigned char index) reentrant;
void write_adc(unsigned char a, unsigned char d);

unsigned char adc_chn = 0;

/*---- User init function ------------------------------------------*/

extern SYS_INFO sys_info;

void user_init(unsigned char init)
{
   unsigned char i;

   ADC0CN = 0x00;               // disable ADC 
   DAC0CN = 0x00;               // disable DAC0
   DAC1CN = 0x00;               // disable DAC1
   REF0CN = 0x00;               // disenable internal reference

   /* push-pull:
      P0.0    TX
      P0.1
      P0.2    SW_ADC
      P0.3    DAC_NCS

      P0.4    DAC_SCK
      P0.5    DAC_DIN
      P0.6 
      P0.7    DAC_CLR
    */
   PRT0CF = 0xBD;
   P0 = 0xFF;

   /* push-pull:
      P1.0    SW_DAC
      P1.1
      P1.2    ADC_NRES
      P1.3    ADC_SCLK

      P1.4    ADC_NCS
      P1.5    
      P1.6 
      P1.7    ADC_DIN
    */
   PRT1CF = 0x9D;
   P1 = 0xFF;

   /* initial EEPROM value */
   if (init) {

      for (i = 0; i < 8; i++)
         user_data.dac[i] = 0;
   }

   /* set-up DAC & ADC */
   DAC_CLR = 1;
   ADC_NRES = 1;
   write_adc(REG_FILTER, 82);                   // SF value for 50Hz rejection
   write_adc(REG_MODE, 3);                      // continuous conversion
   write_adc(REG_CONTROL, adc_chn << 4 | 0x0F); // Chn. 1, +2.56V range

   /* write DACs and UNI_BIP */
   for (i=0 ; i<8 ; i++)
      user_write(i+8);

   user_write(16);

   /* swich unipolar/bipolar */
   user_data.uni_dac = 0;
   user_data.uni_adc = 0;
   user_data.adc_25 = 0;
   
   UNI_DAC = user_data.uni_dac;
   UNI_ADC = !user_data.uni_adc;
}

#pragma NOAREGS

/*---- DAC functions -----------------------------------------------*/

void write_dac_cmd(unsigned char cmd, unsigned char adr, unsigned short d)
{
unsigned char i, m, b;

   DAC_NCS = 0; // chip select

   for (i=0,m=8 ; i<4 ; i++) {
      DAC_SCK = 0;
      DAC_DIN = (cmd & m) > 0;
      DAC_SCK = 1;
      m >>= 1;
   }
   
   for (i=0,m=8 ; i<4 ; i++) {
      DAC_SCK = 0;
      DAC_DIN = (adr & m) > 0;
      DAC_SCK = 1;
      m >>= 1;
   }

   b = d >> 8;
   for (i=0,m=0x80 ; i<8 ; i++) {
      DAC_SCK = 0;
      DAC_DIN = (b & m) > 0;
      DAC_SCK = 1;
      m >>= 1;
   }
   b = d & 0xFF;
   for (i=0,m=0x80 ; i<8 ; i++) {
      DAC_SCK = 0;
      DAC_DIN = (b & m) > 0;
      DAC_SCK = 1;
      m >>= 1;
   }

   DAC_NCS = 1; // remove chip select
}

unsigned char code dac_index[8] = {4, 5, 2, 3, 1, 0, 7, 6};

void write_dac(unsigned char index) reentrant
{
   unsigned short d;

   if (user_data.uni_dac) {
      if (user_data.dac[index] < 0)
         user_data.dac[index] = 0;
  
      if (user_data.dac[index] > 10)
         user_data.dac[index] = 10;

      d = user_data.dac[index] / 10 * 65535 + 0.5;
   } else {
      if (user_data.dac[index] < -10)
         user_data.dac[index] = -10;
  
      if (user_data.dac[index] > 10)
         user_data.dac[index] = 10;

      d = (user_data.dac[index] + 10) / 20 * 65535 + 0.5;
   }

   /* do mapping */
   index = dac_index[index % 8];
   write_dac_cmd(0x3, index, d);
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
      ADC_DIN  = 0;
      ADC_SCLK = 1;
   }

   /* register address */
   for (i=0,m=8 ; i<4 ; i++) {
      ADC_SCLK = 0;
      ADC_DIN  = (a & m) > 0;
      ADC_SCLK = 1;
      m >>= 1;
   }

   ADC_NCS = 1;

   /* write to selected data register */

   ADC_NCS = 0;

   for (i=0,m=0x80 ; i<8 ; i++) {
      ADC_SCLK = 0;
      ADC_DIN  = (d & m) > 0;
      ADC_SCLK = 1;
      m >>= 1;
   }

   ADC_NCS = 1;
}

void read_adc8(unsigned char a, unsigned char *d)
{
   unsigned char i, m;

   /* write to communication register */

   ADC_NCS = 0;
   
   /* write zero to !WEN and one to R/!W */
   for (i=0 ; i<4 ; i++) {
      ADC_SCLK = 0;
      ADC_DIN  = (i == 1);
      ADC_SCLK = 1;
   }

   /* register address */
   for (i=0,m=8 ; i<4 ; i++) {
      ADC_SCLK = 0;
      ADC_DIN  = (a & m) > 0;
      ADC_SCLK = 1;
      m >>= 1;
   }

   ADC_NCS = 1;

   /* read from selected data register */

   ADC_NCS = 0;

   for (i=0,m=0x80,*d=0 ; i<8 ; i++) {
      ADC_SCLK = 0;
      if (ADC_DOUT)
         *d |= m;
      ADC_SCLK = 1;
      m >>= 1;
   }

   ADC_NCS = 1;
}

void read_adc24(unsigned char a, unsigned long *d)
{
   unsigned char i, m;

   /* write to communication register */

   ADC_NCS = 0;
   
   /* write zero to !WEN and one to R/!W */
   for (i=0 ; i<4 ; i++) {
      ADC_SCLK = 0;
      ADC_DIN  = (i == 1);
      ADC_SCLK = 1;
   }

   /* register address */
   for (i=0,m=8 ; i<4 ; i++) {
      ADC_SCLK = 0;
      ADC_DIN  = (a & m) > 0;
      ADC_SCLK = 1;
      m >>= 1;
   }

   ADC_NCS = 1;

   /* read from selected data register */

   ADC_NCS = 0;

   for (i=0,*d=0 ; i<24 ; i++) {
      *d <<= 1;
      ADC_SCLK = 0;
      *d |= ADC_DOUT;
      ADC_SCLK = 1;
   }

   ADC_NCS = 1;
}

unsigned char code adc_index[8] = {6, 7, 0, 1, 2, 3, 4, 5};

void adc_read()
{
   unsigned char i;
   unsigned long value;
   float gvalue;

   if (ADC_NRDY)
       return;

   read_adc24(REG_ADCDATA, &value);

   /* convert to volts */
   gvalue = ((float)value / (1l<<24)) * 2.56;

   /* round result to 5 digits */
   gvalue = floor(gvalue*1E5+0.5)/1E5;

   /* apply range */
   if (user_data.adc_25 == 0) {
      if (user_data.uni_adc)
         gvalue *= 10.0/2.56;
      else
         gvalue = gvalue/2.56*20.0 - 10;
   }

   DISABLE_INTERRUPTS;
   user_data.adc[adc_chn] = gvalue;
   ENABLE_INTERRUPTS;

   /* start next conversion */
   adc_chn = (adc_chn + 1) % 8;
   i = adc_index[adc_chn];
   write_adc(REG_CONTROL, i << 4 | 0x0F); // adc_chn, +2.56V range
}

void adc_calib()
{
   unsigned char i;

   for (i=0 ; i<8 ; i++) {

      write_adc(REG_CONTROL, i << 4 | 0x0F);

      /* zero calibration */
      write_adc(REG_MODE, 4);
      while (ADC_NRDY) led_blink(1, 1, 100);

      /* full scale calibration */
      write_adc(REG_MODE, 5);
      while (ADC_NRDY) led_blink(1, 1, 100);
   }

   /* restart continuous conversion */
   write_adc(REG_MODE, 3);
}

/*---- User write function -----------------------------------------*/

void user_write(unsigned char index) reentrant
{
   if (index > 7 && index < 16)
      write_dac(index-8);
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

/*---- User loop function ------------------------------------------*/

void user_loop(void)
{
   adc_read();
}
