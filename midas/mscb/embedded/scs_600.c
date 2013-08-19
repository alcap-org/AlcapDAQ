/********************************************************************\

  Name:         scd_600.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol 
                for SCS-600 Digital I/O

  $Log: scs_600.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:19  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.19  2004/04/07 11:06:17  midas
  Version 1.7.1

  Revision 1.18  2004/03/04 14:33:22  midas
  *** empty log message ***

  Revision 1.17  2004/01/08 09:41:42  midas
  Made single shot capability conditional

  Revision 1.16  2004/01/07 13:12:39  midas
  Added single shot mode

  Revision 1.15  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.14  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.13  2003/07/17 14:23:24  midas
  Turn output off immediately if button pressed

  Revision 1.12  2003/06/11 14:12:18  midas
  Removed LCD output

  Revision 1.11  2003/03/19 16:35:03  midas
  Eliminated configuration parameters

  Revision 1.10  2003/03/06 10:59:54  midas
  Added P1 output and 16-bit input

  Revision 1.9  2003/02/19 16:05:36  midas
  Added 'init' parameter to user_init

  Revision 1.8  2002/11/28 13:03:41  midas
  Protocol version 1.2

  Revision 1.7  2002/11/22 15:43:03  midas
  Made user_write reentrant

  Revision 1.6  2002/10/28 14:26:30  midas
  Changes from Japan

  Revision 1.5  2002/10/22 15:06:18  midas
  Removed temporary OE test

  Revision 1.4  2002/10/16 15:24:38  midas
  Added units in descriptor

  Revision 1.3  2002/10/09 11:06:46  midas
  Protocol version 1.1

  Revision 1.2  2002/10/03 15:31:53  midas
  Various modifications

  Revision 1.1  2002/07/12 15:20:08  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include "mscb.h"

extern bit DEBUG_MODE;

char code node_name[] = "SCS-600";

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

#undef SINGLE_SHOT               /* #define/#undef for single shot capability */

sbit SR_CLOCK  = P0 ^ 4;         // Shift register clock
sbit SR_STROBE = P0 ^ 5;         // Storage register clock
sbit SR_DATAO  = P0 ^ 6;         // Serial data out
sbit SR_DATAI  = P0 ^ 7;         // Serial data in
sbit SR_OE     = P0 ^ 3;         // Output enable
sbit SR_READB  = P0 ^ 2;         // Serial data readback

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   unsigned char out[8];
   unsigned char button;
   unsigned char p1;
#ifdef SINGLE_SHOT
   unsigned char single;
#endif
   unsigned char input[2];
   float power[8];
} idata user_data;

MSCB_INFO_VAR code variables[] = {
   1, UNIT_BOOLEAN, 0, 0, 0, "Out0", &user_data.out[0],
   1, UNIT_BOOLEAN, 0, 0, 0, "Out1", &user_data.out[1],
   1, UNIT_BOOLEAN, 0, 0, 0, "Out2", &user_data.out[2],
   1, UNIT_BOOLEAN, 0, 0, 0, "Out3", &user_data.out[3],
   1, UNIT_BOOLEAN, 0, 0, 0, "Out4", &user_data.out[4],
   1, UNIT_BOOLEAN, 0, 0, 0, "Out5", &user_data.out[5],
   1, UNIT_BOOLEAN, 0, 0, 0, "Out6", &user_data.out[6],
   1, UNIT_BOOLEAN, 0, 0, 0, "Out7", &user_data.out[7],
   1, UNIT_BYTE, 0, 0, 0, "Button", &user_data.button,
   1, UNIT_BYTE, 0, 0, 0, "P1", &user_data.p1,
#ifdef SINGLE_SHOT
   1, UNIT_BYTE, 0, 0, 0, "Single", &user_data.single,
#endif
   1, UNIT_BYTE, 0, 0, 0, "Input1", &user_data.input[0],
   1, UNIT_BYTE, 0, 0, 0, "Input2", &user_data.input[1],
   4, UNIT_PERCENT, 0, 0, MSCBF_FLOAT, "Power0", &user_data.power[0],
   4, UNIT_PERCENT, 0, 0, MSCBF_FLOAT, "Power1", &user_data.power[1],
   4, UNIT_PERCENT, 0, 0, MSCBF_FLOAT, "Power2", &user_data.power[2],
   4, UNIT_PERCENT, 0, 0, MSCBF_FLOAT, "Power3", &user_data.power[3],
   4, UNIT_PERCENT, 0, 0, MSCBF_FLOAT, "Power4", &user_data.power[4],
   4, UNIT_PERCENT, 0, 0, MSCBF_FLOAT, "Power5", &user_data.power[5],
   4, UNIT_PERCENT, 0, 0, MSCBF_FLOAT, "Power6", &user_data.power[6],
   4, UNIT_PERCENT, 0, 0, MSCBF_FLOAT, "Power7", &user_data.power[7],
   0
};

unsigned char output;

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

#pragma NOAREGS

void user_write(unsigned char channel) reentrant;
void ser_clock(void);

/*---- User init function ------------------------------------------*/

void user_init(unsigned char init)
{
   unsigned char i;

   PRT0CF = 0x7B;               // push-pull for P0.0,1,3,4,5,6
   PRT1CF = 0xFF;               // push-pull for P1

   /* init shift register lines */
   SR_OE = 1;
   SR_CLOCK = 0;
   SR_DATAO = 0;
   SR_DATAI = 1;                // prepare for input

   /* initialize configuration data */
   if (init) {
      for (i = 0; i < 8; i++) {
         user_data.out[i] = 0;
         user_data.p1 = 0;
         user_data.power[i] = 100;
      }
   }

   /* set outputs according to main switch */
   for (i = 0; i < 8; i++) {
      if (user_data.out[i])
         output |= (1 << i);
      else
         output &= ~(1 << i);
   }

   /* output P1 value from EEPROM */
   user_write(9);

   /* output initial values */
   ser_clock();
}


/*---- User write function -----------------------------------------*/

void user_write(unsigned char index) reentrant
{
   if (index == 9)
      P1 = user_data.p1;
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

/*---- Clock external shift registers to read and output data ------*/

void ser_clock()
{
   unsigned char d, i;

   /* first shift register (buttons) */
   for (i = d = 0; i < 8; i++) {
      if (SR_DATAI)
         d |= (0x80 >> i);

      delay_us(10);
      SR_CLOCK = 1;
      delay_us(10);
      SR_CLOCK = 0;
      delay_us(10);
   }

   user_data.button = d;

   /* second shift register (LED & input2) */
   for (i = d = 0; i < 8; i++) {
      if (SR_DATAI)
         d |= (0x01 << i);

      SR_DATAO = ((output & (0x01 << i)) == 0);
      delay_us(10);
      SR_CLOCK = 1;
      delay_us(10);
      SR_CLOCK = 0;
      delay_us(10);
   }

   user_data.input[1] = d;

   /* third shift register (VR & input1) */
   for (i = d = 0; i < 8; i++) {
      if (SR_DATAI)
         d |= (0x01 << i);

      SR_DATAO = ((output & (0x80 >> i)) == 0);
      delay_us(10);
      SR_CLOCK = 1;
      delay_us(10);
      SR_CLOCK = 0;
      delay_us(10);
   }

   user_data.input[0] = d;

   /* strobe for output and next input */
   SR_STROBE = 1;
   delay_us(10);
   SR_STROBE = 0;

   /* after first cycle, enable outputs */
   SR_OE = 0;
}

/*---- Apply power settings as a fraction of a second --------------*/

unsigned char cycle;
unsigned char ca[8];

void set_power(void)
{
   unsigned char i;
   static unsigned long on_time;
   float frac;
   unsigned long expired;

   /* turn output off after on-time expired */
   for (i = 0; i < 8; i++)
      if (user_data.power[i] < 100) {
         expired = time() - on_time;
         if (expired >= (unsigned long) (user_data.power[i])) {

#ifdef SINGLE_SHOT
            /* check for single shot */
            if ((user_data.single & (1 << i)) && (output & (1 << i))) {
               output &= ~(1 << i);
               user_data.out[i] = 0;
            } else
#endif
            {

               frac = user_data.power[i] - (unsigned long) (user_data.power[i]);

               if (frac == 0 || expired >= (unsigned long) (user_data.power[i]) + 1) {
                  output &= ~(1 << i);
               } else if (cycle > 0) {
                  if ((float) ca[i] / cycle > frac)
                     output &= ~(1 << i);
                  else
                     ca[i]++;
               } else {
                  ca[i]++;
               }
            }
         }
      }

   /* turn all outputs on every second */
   if (time() - on_time >= 100) {
      on_time = time();
      for (i = 0; i < 8; i++)
         if (user_data.power[i] > 0 && user_data.out[i])
            output |= (1 << i);

      cycle = (cycle + 1) % 10;

      if (cycle == 0)
         for (i = 0; i < 8; i++)
            ca[i] = 0;
   }

   /* turn output off according to main switch */
   for (i = 0; i < 8; i++) {
      if (!user_data.out[i])
         output &= ~(1 << i);
   }

   /* set outputs according to main switch */
   for (i = 0; i < 8; i++)
      if (user_data.power[i] >= 100) {
         if (user_data.out[i])
            output |= (1 << i);
         else
            output &= ~(1 << i);
      }
}

/*---- User loop function ------------------------------------------*/

void user_loop(void)
{
   unsigned char i, old_button;
/*
static unsigned long last = 300;
static bit first = 1;
*/

   /* read inputs */
   old_button = user_data.button;
   ser_clock();
   for (i = 0; i < 8; i++) {
      if ((user_data.button & (1 << i)) > 0 && (old_button & (1 << i)) == 0)
         user_data.out[i] = !user_data.out[i];
   }

   /* set output according to power */
   set_power();

   /*
      if (!DEBUG_MODE && time() > last+30)
      {
      if (first)
      {
      lcd_clear();
      first = 0;
      }

      last = time();

      lcd_goto(0, 0);
      printf("OUT: ");
      for (i=0 ; i<8 ; i++)
      printf("%c", user_data.out[i] ? '1' : '0');

      lcd_goto(0, 1);
      printf("IN:  ");
      for (i=0 ; i<8 ; i++)
      printf("%c", user_data.button & (1<<i) ? '1' : '0');
      }
    */
}
