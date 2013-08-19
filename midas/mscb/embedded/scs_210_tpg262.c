/********************************************************************\

  Name:         scs_210.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol 
                for SCS-210 RS232 node connected to a
                Pfeiffer Dual Gauge TPG262 vacuum sensor

  $Log: scs_210_tpg262.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:18  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.12  2004/09/25 01:14:54  midas
  Started implementing slave port on SCS-1000

  Revision 1.11  2004/07/30 10:22:03  midas
  Added MSCBF_DATALESS

  Revision 1.10  2004/04/07 11:06:17  midas
  Version 1.7.1

  Revision 1.9  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.8  2003/03/21 08:28:15  midas
  Fixed bug with LSB bytes

  Revision 1.7  2003/02/19 16:05:36  midas
  Added 'init' parameter to user_init

  Revision 1.6  2002/11/28 14:44:02  midas
  Removed SIZE_XBIT

  Revision 1.5  2002/11/28 13:03:41  midas
  Protocol version 1.2

  Revision 1.4  2002/11/06 16:45:42  midas
  Revised LED blinking scheme

  Revision 1.3  2002/10/09 15:48:13  midas
  Fixed bug with download

  Revision 1.2  2002/10/09 11:06:46  midas
  Protocol version 1.1

  Revision 1.1  2002/10/03 15:34:23  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include <stdlib.h>             // for atof()
#include "mscb.h"

extern bit FREEZE_MODE;
extern bit DEBUG_MODE;

char code node_name[] = "TPG262";

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

bit terminal_mode;

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   float p1;
   float p2;
   unsigned char baud;
} idata user_data;


MSCB_INFO_VAR code variables[] = {
   1, UNIT_ASCII,  0, 0, MSCBF_DATALESS, "RS232",               0,
   4, UNIT_PASCAL, 0, 0,    MSCBF_FLOAT, "P1",      &user_data.p1,
   4, UNIT_PASCAL, 0, 0,    MSCBF_FLOAT, "P2",      &user_data.p2,

   1, UNIT_BAUD,   0, 0,              0, "Baud",  &user_data.baud,
   0
};

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

void user_write(unsigned char index) reentrant;
void write_gain(void);

/*---- User init function ------------------------------------------*/

void user_init(unsigned char init)
{
   /* initialize UART1 */
   if (init)
      user_data.baud = 1;       // 9600 by default

   uart_init(1, user_data.baud);
}

/*---- User write function -----------------------------------------*/

/* buffers in mscbmain.c */
extern unsigned char xdata in_buf[300], out_buf[300];

#pragma NOAREGS

void user_write(unsigned char index) reentrant
{
   unsigned char i, n;

   if (index == 0) {
      if (in_buf[2] == 27)
         terminal_mode = 0;
      else if (in_buf[2] == 0)
         terminal_mode = 1;
      else {
         n = (in_buf[0] & 0x07) - 1;
         for (i = 0; i < n; i++)
            putchar(in_buf[i + 2]);
      }
   }

   if (index == 3)
      uart_init(1, user_data.baud);
}

/*---- User read function ------------------------------------------*/

unsigned char user_read(unsigned char index)
{
   char c;

   if (index == 0) {
      c = getchar_nowait();
      if (c != -1) {
         out_buf[1] = c;
         return 1;
      }
   }

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
   char idata str[32];
   unsigned char i;

   if (!terminal_mode) {
      /*
      i = gets_wait(str, sizeof(str), 200);

      if (i == 0) {
         // start continuous mode, 1s update
         printf("COM,1\r\n");
         flush();
      } else if (i == 27) {
         user_data.p1 = atof(str + 2);
         user_data.p2 = atof(str + 16);
      }
      */
      user_data.p1 += 1;
      if (user_data.p1 > 100)
         user_data.p1 = 0;
   }
}
