/********************************************************************\

  Name:         scs_1000.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol 
                for SCS-1000 stand alone control unit

  $Log: scs_1000.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:17  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.5  2004/09/25 01:14:54  midas
  Started implementing slave port on SCS-1000

  Revision 1.4  2004/09/10 12:27:22  midas
  Version 1.7.5

  Revision 1.3  2004/07/22 11:52:33  midas
  Fixed bugs with DOFS

  Revision 1.2  2004/07/20 16:04:40  midas
  Implemented scs-1000 code

  Revision 1.1  2004/07/09 07:48:10  midas
  Initial revision


\********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mscb.h"

extern bit FREEZE_MODE;
extern bit DEBUG_MODE;

char code node_name[] = "SCS-1000";

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

extern lcd_menu(void);

/*---- Port definitions ----*/

sbit RELAIS0  = P3 ^ 0;
sbit RELAIS1  = P3 ^ 1;
sbit RELAIS2  = P3 ^ 2;
sbit RELAIS3  = P3 ^ 3;

sbit DOUT0    = P3 ^ 7;
sbit DOUT1    = P3 ^ 6;
sbit DOUT2    = P3 ^ 5;
sbit DOUT3    = P3 ^ 4;

sbit SRCLK    = P1 ^ 7;
sbit SRIN     = P1 ^ 6;
sbit SRSTROBE = P1 ^ 5;

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   unsigned char relais[4];
   unsigned char dout[4];
   float dac[2];
   unsigned char din[4];
   float adc[8];
   float dofs[2];
   float aofs[8];

   float rem_adc, rem_dac;
} xdata user_data;

MSCB_INFO_VAR code variables[] = {

   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Relais0", &user_data.relais[0], 0, 1, 1 },
   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Relais1", &user_data.relais[1], 0, 1, 1 },
   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Relais2", &user_data.relais[2], 0, 1, 1 },
   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Relais3", &user_data.relais[3], 0, 1, 1 },

   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Dout0",   &user_data.dout[0],   0, 1, 1 },
   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Dout1",   &user_data.dout[1],   0, 1, 1 },
   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Dout2",   &user_data.dout[2],   0, 1, 1 },
   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Dout3",   &user_data.dout[3],   0, 1, 1 },

   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC0",    &user_data.dac[0],  -10, 10, 0.1 },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "DAC1",    &user_data.dac[1],  -10, 10, 0.1 },

   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Din0",    &user_data.din[0] },
   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Din1",    &user_data.din[1] },
   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Din2",    &user_data.din[2] },
   { 1, UNIT_BOOLEAN, 0, 0, 0,        "Din3",    &user_data.din[3] },

   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC0",    &user_data.adc[0] },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC1",    &user_data.adc[1] },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC2",    &user_data.adc[2] },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC3",    &user_data.adc[3] },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC4",    &user_data.adc[4] },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC5",    &user_data.adc[5] },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC6",    &user_data.adc[6] },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT, "ADC7",    &user_data.adc[7] },

   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_HIDDEN, "DOFS0",   &user_data.dofs[0], -0.1, 0.1, 0.001 },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_HIDDEN, "DOFS1",   &user_data.dofs[1], -0.1, 0.1, 0.001 },

   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_HIDDEN, "AOFS0",   &user_data.aofs[0], -0.1, 0.1, 0.001 },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_HIDDEN, "AOFS1",   &user_data.aofs[1], -0.1, 0.1, 0.001 },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_HIDDEN, "AOFS2",   &user_data.aofs[2], -0.1, 0.1, 0.001 },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_HIDDEN, "AOFS3",   &user_data.aofs[3], -0.1, 0.1, 0.001 },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_HIDDEN, "AOFS4",   &user_data.aofs[4], -0.1, 0.1, 0.001 },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_HIDDEN, "AOFS5",   &user_data.aofs[5], -0.1, 0.1, 0.001 },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_HIDDEN, "AOFS6",   &user_data.aofs[6], -0.1, 0.1, 0.001 },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_HIDDEN, "AOFS7",   &user_data.aofs[7], -0.1, 0.1, 0.001 },

   //## test
//   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_REMIN,  "RemAdc",  &user_data.rem_adc, 0, 0, 0, 1, 1 },
   { 4, UNIT_VOLT, 0, 0, MSCBF_FLOAT | MSCBF_REMOUT, "RemDac",  &user_data.rem_dac, 0, 0, 0, 1, 2 },

   { 0 }
};

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
      for (i=0 ; i<4 ; i++)
         user_data.relais[i] = 0;

      for (i=0 ; i<4 ; i++)
         user_data.dout[i] = 0;

      user_data.dac[0] = 0;
      user_data.dac[1] = 0;

      for (i=0 ; i<8 ; i++)
         user_data.aofs[i] = 0;
   }

   /* write digital outputs */
   for (i=0 ; i<8 ; i++)
      user_write(i);

   /* write DACs */
   user_write(8);
   user_write(9);

   /* display startup screen */
   lcd_goto(0, 0);
   for (i=0 ; i<7-strlen(sys_info.node_name)/2 ; i++)
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
   unsigned short d;

   switch (index) {

   case 0: RELAIS0 = user_data.relais[0]; break;
   case 1: RELAIS1 = user_data.relais[1]; break;
   case 2: RELAIS2 = user_data.relais[2]; break;
   case 3: RELAIS3 = user_data.relais[3]; break;

   case 4: DOUT0 = user_data.dout[0]; break;
   case 5: DOUT1 = user_data.dout[1]; break;
   case 6: DOUT2 = user_data.dout[2]; break;
   case 7: DOUT3 = user_data.dout[3]; break;

   case 8:                     // DAC0
   case 22:                    // DOFS0
      /* assume -10V..+10V range */
      d = ((user_data.dac[0] + user_data.dofs[0] + 10) / 20) * 0x1000;
      if (d >= 0x1000)
         d = 0x0FFF;

      SFRPAGE = DAC0_PAGE;
      DAC0L = d & 0xFF;
      DAC0H = d >> 8;
      break;

   case 9:                     // DAC1
   case 23:                    // DOFS1
      /* assume -10V..+10V range */
      d = ((user_data.dac[1] + user_data.dofs[1] + 10) / 20) * 0x1000;
      if (d >= 0x1000)
         d = 0x0FFF;

      SFRPAGE = DAC1_PAGE;
      DAC1L = d & 0xFF;
      DAC1H = d >> 8;
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
      gvalue = value / 65536.0 * 20 - 10; // +- 10V range
   else
      gvalue = value / 65536.0 * 10; // 0...10V range

   gvalue += user_data.aofs[channel];

   DISABLE_INTERRUPTS;
   *d = gvalue;
   ENABLE_INTERRUPTS;
}

/*---- Shift register read function --------------------------------*/

bit b0, b1, b2, b3;

#define SHIFT SRCLK = 1; delay_us(1); SRCLK = 0; delay_us(1)

void sr_read()
{
   SRCLK = 0;
   SRIN = 1;
   SRSTROBE = 1;
   delay_us(1);
   SRSTROBE = 0;

   user_data.din[2] = SRIN;
   SHIFT;
   user_data.din[3] = SRIN;
   SHIFT;
   user_data.din[0] = SRIN;
   SHIFT;
   user_data.din[1] = SRIN;

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
   printf("0:%6.3fV", user_data.adc[0]);
   lcd_goto(10, 0);
   printf("1:%6.3fV", user_data.adc[1]);

   /* show state of relais 0&1 */
   lcd_goto(0, 1);
   printf("Rel0: %s", user_data.relais[0] ? "ON " : "OFF");
   lcd_goto(10, 1);
   printf("Rel1: %s", user_data.relais[1] ? "ON " : "OFF");

   /* toggle relais 0 with button 1 */
   if (b1 && !b1_old) {
      user_data.relais[0] = !user_data.relais[0];
      user_write(0);
   }

   /* toggle relais 1 with button 3 */
   if (b3 && !b3_old) {
      user_data.relais[1] = !user_data.relais[1];
      user_write(1);
   }

   /* enter menu on button 0 or 2 */
   if (!init)
      if ((b0 && !b0_old) || (b2 && !b2_old))
         return 1;

   b0_old = b0;
   b1_old = b1;
   b2_old = b2;
   b3_old = b3;

   return 0;
}

/*---- User loop function ------------------------------------------*/

unsigned char uart1_send(char *buffer, int size);

void user_loop(void)
{
   static unsigned char adc_chn = 0;
 
   /* read one ADC channel */
   adc_read(adc_chn, &user_data.adc[adc_chn]);
   adc_chn = (adc_chn + 1) % 8;

   /* read buttons and digital input */
   sr_read();

   /* mangae menu on LCD display */
   lcd_menu();
}
