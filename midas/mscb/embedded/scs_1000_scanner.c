/********************************************************************\

  Name:         scs_1000_scanner.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol 
                for PSI beamline scanner

  $Log: scs_1000_scanner.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:17  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/07/29 13:55:52  midas
  Finished code

  Revision 1.1  2004/07/27 09:30:57  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mscb.h"

extern bit FREEZE_MODE;
extern bit DEBUG_MODE;

char code node_name[] = "Scanner";

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

extern lcd_menu(void);

/*---- Port definitions ----*/

sbit RELAIS0 = P3 ^ 0;
sbit RELAIS1 = P3 ^ 1;
sbit RELAIS2 = P3 ^ 2;
sbit RELAIS3 = P3 ^ 3;

sbit DOUT0 = P3 ^ 7;
sbit DOUT1 = P3 ^ 6;
sbit DOUT2 = P3 ^ 5;
sbit DOUT3 = P3 ^ 4;

sbit SRCLK = P1 ^ 7;
sbit SRIN = P1 ^ 6;
sbit SRSTROBE = P1 ^ 5;

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   float posx, posy;
   float demandx, demandy;
   unsigned char relais[4];
   float aofs[2];
   float five_v;
} xdata user_data;

MSCB_INFO_VAR code variables[] = {

   {4, UNIT_METER, PRFX_MILLI, 0, MSCBF_FLOAT, "PosX", &user_data.posx},
   {4, UNIT_METER, PRFX_MILLI, 0, MSCBF_FLOAT, "PosY", &user_data.posy},

   {4, UNIT_METER, PRFX_MILLI, 0, MSCBF_FLOAT, "DemandX", &user_data.demandx, -1000, 1000,
    1},
   {4, UNIT_METER, PRFX_MILLI, 0, MSCBF_FLOAT, "DemandY", &user_data.demandy, -1000, 1000,
    1},

   {1, UNIT_BOOLEAN, 0, 0, 0, "Relais0", &user_data.relais[0], 0, 1, 1},
   {1, UNIT_BOOLEAN, 0, 0, 0, "Relais1", &user_data.relais[1], 0, 1, 1},
   {1, UNIT_BOOLEAN, 0, 0, 0, "Relais2", &user_data.relais[2], 0, 1, 1},
   {1, UNIT_BOOLEAN, 0, 0, 0, "Relais3", &user_data.relais[3], 0, 1, 1},

   {4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "AOFS0", &user_data.aofs[0], -10, 10, 0.1},
   {4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "AOFS1", &user_data.aofs[1], -10, 10, 0.1},

   {4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "5V", &user_data.five_v},

   {0}
};

bit moving_x = 0;
bit moving_y = 0;

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void user_write(unsigned char index) reentrant;

/*---- User init function ------------------------------------------*/

extern SYS_INFO sys_info;

void user_init(unsigned char init)
{
   unsigned char i;

   led_mode(2, 0);              // buzzer off by default

   SFRPAGE = ADC0_PAGE;
   AMX0CF = 0x00;               // select single ended analog inputs
   ADC0CF = 0xE0;               // 16 system clocks, gain 1
   ADC0CN = 0x80;               // enable ADC 
   REF0CN = 0x00;               // use external voltage reference

   SFRPAGE = LEGACY_PAGE;
   REF0CN = 0x02;               // select external voltage reference

   SFRPAGE = DAC0_PAGE;
   DAC0CN = 0x80;               // enable DAC0
   SFRPAGE = DAC1_PAGE;
   DAC1CN = 0x80;               // enable DAC1

   /* open drain(*) /push-pull: 
      P0.0 TX1      P1.0 TP6          P2.0 LED1         P3.0 RELAIS0
      P0.1*RX1      P1.1 BACKLIGHT    P2.1 LCD_E        P3.1 RELAIS1
      P0.2 TX2      P1.2 TP5          P2.2 LCD_RW       P3.2 RELAIS2
      P0.3*RX2      P1.3 TP7          P2.3 LCD_RS       P3.3 RELAIS3

      P0.4 EN1      P1.4 WATCHDOG     P2.4 LCD_DB4      P3.4 DOUT0
      P0.5 EN2      P1.5 SRSTROBE     P2.5 LCD_DB5      P3.5 DOUT1
      P0.6 LED2     P1.6*SRIN         P2.6 LCD_DB6      P3.6 DOUT2
      P0.7 BUZZER   P1.7 SRCLK        P2.7 LCD_DB7      P3.7 DOUT3
    */
   SFRPAGE = CONFIG_PAGE;
   P0MDOUT = 0xF5;
   P1MDOUT = 0xBF;
   P2MDOUT = 0xFF;
   P3MDOUT = 0xFF;

   /* initial EEPROM value */
   if (init) {
      for (i = 0; i < 4; i++)
         user_data.relais[i] = 0;

      user_data.demandx = 0;
      user_data.demandy = 0;

      user_data.aofs[0] = 0;
      user_data.aofs[1] = 0;
   }

   /* write relais */
   for (i = 4; i < 8; i++)
      user_write(i);

   /* display startup screen */
   lcd_goto(0, 0);
   for (i = 0; i < 7 - strlen(sys_info.node_name) / 2; i++)
      puts(" ");
   puts("** ");
   puts(sys_info.node_name);
   puts(" **");
   lcd_goto(0, 1);
   printf("   Address:  %04X ", sys_info.node_addr);
}

#pragma NOAREGS

/*---- User write function -----------------------------------------*/

void user_write(unsigned char index) reentrant
{
   switch (index) {

   case 2:
      if (user_data.demandx < user_data.posx)
         RELAIS0 = 1;
      else if (user_data.demandx > user_data.posx)
         RELAIS1 = 1;
      moving_x = 1;
      break;
   case 3:
      if (user_data.demandy < user_data.posy)
         RELAIS2 = 1;
      else if (user_data.demandy > user_data.posy)
         RELAIS3 = 1;
      moving_y = 1;
      break;
   case 4:
      RELAIS0 = user_data.relais[0];
      break;
   case 5:
      RELAIS1 = user_data.relais[1];
      break;
   case 6:
      RELAIS2 = user_data.relais[2];
      break;
   case 7:
      RELAIS3 = user_data.relais[3];
      break;

   case 10:
      RELAIS0 = RELAIS1 = RELAIS2 = RELAIS3 = 0;
      moving_x = moving_y = 0;
      break;
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

/*---- ADC read function -------------------------------------------*/

void adc_read(channel, float *d)
{
   unsigned long value;
   unsigned int i, n;
   float gvalue;

   SFRPAGE = ADC0_PAGE;
   AMX0SL = channel & 0x0F;
   ADC0CF = 0xE0;               // 16 system clocks, gain 1

   n = 1 << 10;                 // average 1024 times

   value = 0;
   for (i = 0; i < n; i++) {
      DISABLE_INTERRUPTS;

      AD0INT = 0;
      AD0BUSY = 1;
      while (!AD0INT);          // wait until conversion ready, does NOT work with ADBUSY!

      ENABLE_INTERRUPTS;

      value += (ADC0L | (ADC0H << 8));
      yield();
   }

   value >>= 6;

   if (channel < 2)
      gvalue = value / 65536.0 * 20 - 10;       // +- 10V range
   else
      gvalue = value / 65536.0 * 10;    // 0...10V range

   if (channel == 2 || channel == 3) {
      gvalue *= 100;            // 0..5V -> 0..500 mm range
      gvalue += user_data.aofs[channel - 2];
   }

   DISABLE_INTERRUPTS;
   *d = gvalue;
   ENABLE_INTERRUPTS;
}

/*---- Shift register read function --------------------------------*/

bit b0, b1, b2, b3;

#define SHIFT SRCLK = 1; delay_us(1); SRCLK = 0; delay_us(1)

unsigned char din[4];

void sr_read()
{
   SRCLK = 0;
   SRIN = 1;
   SRSTROBE = 1;
   delay_us(1);
   SRSTROBE = 0;

   din[2] = SRIN;
   SHIFT;
   din[3] = SRIN;
   SHIFT;
   din[0] = SRIN;
   SHIFT;
   din[1] = SRIN;

   SHIFT;
   b3 = SRIN;
   SHIFT;
   b2 = SRIN;
   SHIFT;
   b1 = SRIN;
   SHIFT;
   b0 = SRIN;
}

/*---- Application display -----------------------------------------*/

unsigned char application_display(bit init)
{
   static bit b0_old = 0, b1_old = 0, b2_old = 0, b3_old = 0;

   if (init)
      lcd_clear();

   /* display first two ADCs */
   lcd_goto(0, 0);
   printf("X:%5.1fmm", user_data.posx);
   lcd_goto(10, 0);
   printf("Y:%5.1fmm", user_data.posy);

   /* show state of relais 0&1 */
   if (moving_x || moving_y) {

      lcd_goto(0, 1);
      printf("       abort        ");

      if ((!b0_old && b0) || (!b1_old && b1) || (!b2_old && b2) || (!b3_old && b3)) {
         RELAIS0 = 0;
         RELAIS1 = 0;
         RELAIS2 = 0;
         RELAIS3 = 0;
         moving_x = moving_y = 0;
      }

      if (user_data.demandx >= user_data.posx && RELAIS0 == 1) {
         RELAIS0 = 0;
         moving_x = 0;
      }

      if (user_data.demandx <= user_data.posx && RELAIS1 == 1) {
         RELAIS1 = 0;
         moving_x = 0;
      }

      if (user_data.demandy >= user_data.posy && RELAIS2 == 1) {
         RELAIS2 = 0;
         moving_y = 0;
      }

      if (user_data.demandy <= user_data.posy && RELAIS3 == 1) {
         RELAIS3 = 0;
         moving_y = 0;
      }

   } else {
      lcd_goto(0, 1);
      printf("  -   +   ");
      lcd_goto(10, 1);
      printf("  -   +   ");

      /* switch relais 0 with button 0 */
      user_data.relais[0] = b0;
      user_write(4);

      /* switch relais 1 with button 1 */
      user_data.relais[1] = b1;
      user_write(5);

      /* switch relais 2 with button 2 */
      user_data.relais[2] = b2;
      user_write(6);

      /* switch relais 3 with button 3 */
      user_data.relais[3] = b3;
      user_write(7);
   }

   /* enter menu on button 0 and 1 */
   if (!init)
      if (b0 && b1) {
         RELAIS0 = 0;
         RELAIS1 = 0;
         RELAIS2 = 0;
         RELAIS3 = 0;
         moving_x = moving_y = 0;
         return 1;
      }

   b0_old = b0;
   b1_old = b1;
   b2_old = b2;
   b3_old = b3;

   return 0;
}

/*---- User loop function ------------------------------------------*/

void user_loop(void)
{
   /* read scanne position */
   adc_read(2, &user_data.posx);
   adc_read(3, &user_data.posy);
   adc_read(4, &user_data.five_v);

   /* read buttons and digital input */
   sr_read();

   /* mangae menu on LCD display */
   lcd_menu();
}
