/********************************************************************\

  Name:         scs_demo.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for SCS-300 standalone node (just for demo)

  $Log: scs_demo.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:19  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.1  2003/05/08 11:55:53  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include "mscb.h"

extern bit FREEZE_MODE;
extern bit DEBUG_MODE;

char code node_name[] = "SCS-DEMO";

#define GAIN      0             // gain for internal PGA

sbit VALVE = P0 ^ 7;

/*---- Define variable parameters returned to the CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   float temp[2];
   unsigned char sw;
} idata user_data;

MSCB_INFO_VAR code variables[] = {
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "Temp0", &user_data.temp[0],
   4, UNIT_CELSIUS, 0, 0, MSCBF_FLOAT, "Temp1", &user_data.temp[1],
   1, UNIT_BOOLEAN, 0, 0, 0, "Switch", &user_data.sw,
   0
};

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void user_write(unsigned char index) reentrant;

/*---- User init function ------------------------------------------*/

void user_init(unsigned char init)
{
   AMX0CF = 0x00;               // select single ended analog inputs
   ADC0CF = 0xE0;               // 16 system clocks, gain 1
   ADC0CN = 0x80;               // enable ADC

   REF0CN = 0x03;               // enable internal reference
   DAC0CN = 0x80;               // enable DAC0
   DAC1CN = 0x80;               // enable DAC1

   PRT0CF = 0xFF;               // P0 on push-pull

   if (init) {
      user_data.temp[0] = 0;
      user_data.temp[1] = 0;
      user_data.sw = 0;
   }

   VALVE = user_data.sw;
}

/*---- User write function -----------------------------------------*/

#pragma NOAREGS

void user_write(unsigned char index) reentrant
{
   if (index == 2)
      VALVE = user_data.sw;
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
   if (data_in);
   if (data_out);
   return 0;
}

/*------------------------------------------------------------------*/

void adc_read()
{
   static unsigned char channel = 0;
   static unsigned long value;
   static unsigned char cnt = 0;
   unsigned char i;
   float t;

   if (cnt == 0) {
      /* switch between channels 0 and 4 */
      channel = channel == 0 ? 5 : 0;
      AMX0SL = channel & 0x0F;
      ADC0CF = 0xE0 | (GAIN & 0x07);    // 16 system clocks and gain

      value = 0;
   }

   /* integrate 16 times */
   for (i = 0; i < 16; i++) {
      DISABLE_INTERRUPTS;

      ADCINT = 0;
      ADBUSY = 1;
      while (!ADCINT);          // wait until conversion ready, does NOT work with ADBUSY!

      ENABLE_INTERRUPTS;

      value += (ADC0L | (ADC0H << 8));
      yield();
   }

   cnt++;

   if (cnt == 0) {              /* average 16*256 = 4096 times */
      value /= 256;

      /* convert to volts, times 100deg/Volt */
      t = value / 65536.0 * 2.5 * 100;

      DISABLE_INTERRUPTS;
      if (channel == 0)
         user_data.temp[0] = t;
      else
         user_data.temp[1] = t;
      ENABLE_INTERRUPTS;
   }
}


/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/

void user_loop(void)
{
   /* read temperature */
   adc_read();
}
