/********************************************************************\

  Name:         mscuser.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol

  $Log: mscbuser.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:17  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.5  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.4  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.3  2002/07/12 15:19:18  midas
  Added NOAREGS

  Revision 1.2  2002/07/10 09:52:55  midas
  Finished EEPROM routines

  Revision 1.1  2002/07/09 15:31:32  midas
  Initial Revision

\********************************************************************/

#include <stdio.h>
#include "mscb.h"

extern bit FREEZE_MODE;

char code node_name[] = "GENERIC";

/*---- Define channels and configuration parameters returned to
       the CMD_GET_INFO command                                 ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   unsigned char p1;
   unsigned int dac0;
   unsigned int dac1;
   unsigned int adc[8];
} user_data;

struct {
   unsigned char adc_average;
} user_conf;


MSCB_INFO_CHN code channel[] = {
   1, 0, 0, 0, "P1", &user_data.p1,
   2, 0, 0, 0, "DAC0", &user_data.dac0,
   2, 0, 0, 0, "DAC1", &user_data.dac1,
   2, 0, 0, 0, "ADC0", &user_data.adc[0],
   2, 0, 0, 0, "ADC1", &user_data.adc[1],
   2, 0, 0, 0, "ADC2", &user_data.adc[2],
   2, 0, 0, 0, "ADC3", &user_data.adc[3],
   2, 0, 0, 0, "ADC4", &user_data.adc[4],
   2, 0, 0, 0, "ADC5", &user_data.adc[5],
   2, 0, 0, 0, "ADC6", &user_data.adc[6],
   2, 0, 0, 0, "ADC7", &user_data.adc[7],
   0
};

MSCB_INFO_CHN code conf_param[] = {
   1, 0, 0, 0, "ADCAvrg", &user_conf.adc_average,
   0
};

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void user_write(unsigned char channel);

/*---- User init function ------------------------------------------*/

void user_init(void)
{
#ifdef CPU_ADUC812
   ADCCON1 = 0x7C;              // power up ADC, 14.5us conv+acq time
   ADCCON2 = 0;                 // select channel to convert
   DACCON = 0x1f;               // power on DACs
#endif

#ifdef CPU_C8051F000
   AMX0CF = 0x00;               // select single ended analog inputs
   ADC0CF = 0xE0;               // 16 system clocks, gain 1
   ADC0CN = 0x80;               // enable ADC 

   REF0CN = 0x03;               // enable internal reference
   DAC0CN = 0x80;               // enable DAC0
   DAC1CN = 0x80;               // enable DAC1
#endif

   /* correct initial EEPROM value */
   if (user_conf.adc_average == 0xFF)
      user_conf.adc_average = 0;

   /* write P1 and DACs */
   user_write(0);
   user_write(1);
   user_write(2);
}

/*---- User write function -----------------------------------------*/

#pragma NOAREGS

void user_write(unsigned char channel)
{
   unsigned char data *d;

   switch (channel) {
   case 0:                     // p1 
      P1 = user_data.p1;
      break;

   case 1:                     // DAC0
      d = (void *) &user_data.dac0;
#ifdef CPU_ADUC812
      DAC0H = d[1];
      DAC0L = d[0];
#endif
#ifdef CPU_C8051F000
      DAC0L = d[0];
      DAC0H = d[1];
#endif
      break;

   case 2:                     // DAC1
      d = (void *) &user_data.dac1;
#ifdef CPU_ADUC812
      DAC1H = d[1];
      DAC1L = d[0];
#endif
#ifdef CPU_C8051F000
      DAC1L = d[0];
      DAC1H = d[1];
#endif
      break;
   }
}

/*---- User read function ------------------------------------------*/

unsigned char user_read(unsigned char channel)
{
   if (channel == 0)
      user_data.p1 = P1;

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
   /* return some dummy data */
   data_out[0] = data_in[0] + 1;
   data_out[1] = data_in[1] + 1;
   return 2;
}

/*---- User loop function ------------------------------------------*/

void adc_read(channel, unsigned int *d)
{
   unsigned long value;
   unsigned int i, n;

   AMX0SL = channel & 0x0F;

   n = 1 << (user_conf.adc_average + 4);

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

   if (user_conf.adc_average)
      value >>= (user_conf.adc_average);

   DISABLE_INTERRUPTS;
   *d = value;
   ENABLE_INTERRUPTS;
}

void user_loop(void)
{
   unsigned char i;

   /* don't take data in freeze mode */
   if (!FREEZE_MODE);
   {
      for (i = 0; i < 8; i++)
         adc_read(i, &user_data.adc[i]);

      lcd_goto(0, 0);
      printf("%6.4f V", user_data.adc[0] / 65536.0 * 2.5);
   }
}
