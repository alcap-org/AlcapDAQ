/********************************************************************\

  Name:         scs_800.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol
                for SCS-800 capacitance meter

  $Log: scs_800.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:19  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.4  2004/04/07 11:06:17  midas
  Version 1.7.1

  Revision 1.3  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.2  2003/09/23 09:23:05  midas
  Removed tabs

  Revision 1.1  2003/09/10 13:46:34  midas
  Initial revision


\********************************************************************/

#include <stdio.h>
#include "mscb.h"

extern bit FREEZE_MODE;
extern bit DEBUG_MODE;

char code node_name[] = "SCS-800";

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

sbit SR_CLOCK = P0 ^ 4;         // Shift register clock
sbit SR_STROBE = P0 ^ 5;        // Storage register clock
sbit SR_DATAO = P0 ^ 2;         // Serial data out

sbit NE555_RESET = P0 ^ 7;
sbit NE555_TRIGGER = P0 ^ 6;

/*---- Define variable parameters returned to the CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   float cap[8];
   signed short ofs[8];
   float gain;
} idata user_data;

unsigned char range[8];

MSCB_INFO_VAR code variables[] = {
   4, UNIT_FARAD, PRFX_NANO, 0, MSCBF_FLOAT, "Cap0", &user_data.cap[0],
   4, UNIT_FARAD, PRFX_NANO, 0, MSCBF_FLOAT, "Cap1", &user_data.cap[1],
   4, UNIT_FARAD, PRFX_NANO, 0, MSCBF_FLOAT, "Cap2", &user_data.cap[2],
   4, UNIT_FARAD, PRFX_NANO, 0, MSCBF_FLOAT, "Cap3", &user_data.cap[3],
   4, UNIT_FARAD, PRFX_NANO, 0, MSCBF_FLOAT, "Cap4", &user_data.cap[4],
   4, UNIT_FARAD, PRFX_NANO, 0, MSCBF_FLOAT, "Cap5", &user_data.cap[5],
   4, UNIT_FARAD, PRFX_NANO, 0, MSCBF_FLOAT, "Cap6", &user_data.cap[6],
   4, UNIT_FARAD, PRFX_NANO, 0, MSCBF_FLOAT, "Cap7", &user_data.cap[7],
   2, UNIT_FARAD, PRFX_PICO, 0, MSCBF_SIGNED, "Ofs0", &user_data.ofs[0],
   2, UNIT_FARAD, PRFX_PICO, 0, MSCBF_SIGNED, "Ofs1", &user_data.ofs[1],
   2, UNIT_FARAD, PRFX_PICO, 0, MSCBF_SIGNED, "Ofs2", &user_data.ofs[2],
   2, UNIT_FARAD, PRFX_PICO, 0, MSCBF_SIGNED, "Ofs3", &user_data.ofs[3],
   2, UNIT_FARAD, PRFX_PICO, 0, MSCBF_SIGNED, "Ofs4", &user_data.ofs[4],
   2, UNIT_FARAD, PRFX_PICO, 0, MSCBF_SIGNED, "Ofs5", &user_data.ofs[5],
   2, UNIT_FARAD, PRFX_PICO, 0, MSCBF_SIGNED, "Ofs6", &user_data.ofs[6],
   2, UNIT_FARAD, PRFX_PICO, 0, MSCBF_SIGNED, "Ofs7", &user_data.ofs[7],
   4, UNIT_FACTOR, 0, 0, MSCBF_FLOAT, "Gain", &user_data.gain,
   0
};

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void user_write(unsigned char index) reentrant;
void set_range() reentrant;

/*---- User init function ------------------------------------------*/

void user_init(unsigned char init)
{
   unsigned char i;

   PRT0CF = 0xFD;               // P0.0: TX = Push Pull  
   // P0.2,3,4,5,6,7: Shift Reg & NE555 = Push Pull

   PRT1CF = 0x00;               // P1 input
   P1 = 0xFF;

   NE555_RESET = 1;             // Remove reset
   NE555_TRIGGER = 1;           // Remove trigger

   if (init) {
      for (i = 0; i < 8; i++) {
         user_data.cap[i] = 0;
         user_data.ofs[i] = 0;
         range[i] = 0;
         user_data.gain = 1;
      }
   }

   set_range();
}

/*---- User write function -----------------------------------------*/

#pragma NOAREGS

void user_write(unsigned char index) reentrant
{
   if (index);
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

void set_range() reentrant
{
   unsigned char i;

   SR_STROBE = 0;
   SR_CLOCK = 0;

   /* first shift register (chn 0-3) */
   for (i = 0; i < 4; i++) {
      /* first bit (5M1 Ohm) */
      SR_DATAO = 1;
      SR_CLOCK = 1;
      SR_CLOCK = 0;

      /* second bit (5k6 Ohm) */
      SR_DATAO = 1;
      SR_CLOCK = 1;
      SR_CLOCK = 0;
   }

   /* second shift register (chn 0-3) */
   for (i = 0; i < 4; i++) {
      /* first bit (5M1 Ohm) */
      SR_DATAO = 1;
      SR_CLOCK = 1;
      SR_CLOCK = 0;

      /* second bit (5k6 Ohm) */
      SR_DATAO = 1;
      SR_CLOCK = 1;
      SR_CLOCK = 0;
   }

   /* strobe output */
   SR_DATAO = 0;
   SR_STROBE = 1;
   SR_STROBE = 0;
}

/*------------------------------------------------------------------*/

void cap_read()
{
   static unsigned char channel = 0;
   unsigned char m, i;
   unsigned long v;
   float c;

   /* average 10 times */
   for (i = v = 0; i < 10; i++) {
      yield();

      m = (1 << channel);

      /* init timer 3 */
      TMR3L = 0;
      TMR3H = 0;
      TMR3RLL = 0xFF;
      TMR3RLH = 0xFF;

      TMR3CN = (1 << 2);        /* clock/12, Timer 3 enabled */

      /* trigger NE555 */
      NE555_TRIGGER = 0;
      delay_us(1);
      NE555_TRIGGER = 1;

      /* wait until NE555 output goes low again */
      while ((P1 & m) != 0 && (TMR3CN & 0x80) == 0);

      if ((TMR3CN & 0x80) != 0) {
         /* overflow */
         c = 999;

         /* stop timer 3 */
         TMR3CN = 0;

         goto overflow;
      } else {
         /* stop timer 3 */
         TMR3CN = 0;

         /* time in us */
         v += TMR3H * 256 + TMR3L;
      }
   }

   c = (float) v / i / 11.052 * 12;

   /* time in s */
   c /= 1E6;

   /* convert to nF via t = 1.1 * R * C */
   c = c / 1.1 / 5.6E6 * 1E9;

   /* apply overall gain */
   c *= user_data.gain;

   /* general offset 18pF */
   c -= 0.018;

   /* channel offset */
   c -= user_data.ofs[channel] / 1000.0;

   /* stip off unsignificant digits */
   c = ((int) (c * 1000 + 0.5)) / 1000.0;

 overflow:

   DISABLE_INTERRUPTS;
   user_data.cap[channel] = c;
   ENABLE_INTERRUPTS;

   channel = (channel + 1) % 8;
}

/*------------------------------------------------------------------*/

void user_loop(void)
{
   static long t = 0;

   if (time() - t > 10) {
      cap_read();
      t = time();
   }
}
