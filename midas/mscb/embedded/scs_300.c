/********************************************************************\

  Name:         scd_300.c
  Created by:   Stefan Ritt


  Contents:     Application specific (user) part of
                Midas Slow Control Bus protocol 
                for SCS-300 Parallel Port Interface

  $Log: scs_300.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:18  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.10  2004/06/16 11:40:25  midas
  Fixed temporarily linker error

  Revision 1.9  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.8  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.7  2003/03/31 08:15:54  midas
  Added data to idata

  Revision 1.6  2003/03/19 16:35:03  midas
  Eliminated configuration parameters

  Revision 1.5  2003/02/19 16:05:36  midas
  Added 'init' parameter to user_init

  Revision 1.4  2003/01/16 16:29:49  midas
  Removed SIZE_32

  Revision 1.3  2002/11/28 13:03:41  midas
  Protocol version 1.2

  Revision 1.2  2002/10/09 11:06:46  midas
  Protocol version 1.1

  Revision 1.1  2002/10/04 09:02:05  midas
  Initial revision

  Revision 1.2  2002/10/03 15:31:53  midas
  Various modifications

  Revision 1.1  2002/07/12 15:20:08  midas
  Initial revision

\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mscb.h"

char code node_name[] = "SCS-300";

/* declare number of sub-addresses to framework */
unsigned char idata _n_sub_addr = 1;

/*---- Define variable parameters returned to CMD_GET_INFO command ----*/

/* data buffer (mirrored in EEPROM) */

struct {
   float value[4];
} idata user_data;

MSCB_INFO_VAR code variables[] = {
   4, UNIT_UNDEFINED, 0, 0, MSCBF_FLOAT, "Data0", &user_data.value[0],
   4, UNIT_UNDEFINED, 0, 0, MSCBF_FLOAT, "Data1", &user_data.value[1],
   4, UNIT_UNDEFINED, 0, 0, MSCBF_FLOAT, "Data2", &user_data.value[2],
   4, UNIT_UNDEFINED, 0, 0, MSCBF_FLOAT, "Data3", &user_data.value[3],
   0
};

/********************************************************************\

  Application specific init and inout/output routines

\********************************************************************/

/* 8 data bits to LPT */
#define LPT_DATA         P1

/* control/status bits to LPT       DB25   */
sbit LPT_STROBE = P2 ^ 3;       // Pin 1
sbit LPT_INIT = P2 ^ 4;         // Pin 16
sbit LPT_SELECT = P2 ^ 5;       // Pin 17
sbit LPT_ACK = P2 ^ 6;          // Pin 10
sbit LPT_BUSY = P2 ^ 7;         // Pin 11

#pragma NOAREGS

void user_write(unsigned char channel) reentrant;

/*---- User init function ------------------------------------------*/

void user_init(unsigned char init)
{
   /* set initial state of lines */
   LPT_DATA = 0xFF;
   LPT_STROBE = 1;
   LPT_INIT = 1;
   LPT_SELECT = 1;
   LPT_ACK = 1;
   LPT_BUSY = 1;

   if (init);
}

/*---- User write function -----------------------------------------*/

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

/*---- Functions for parallel port ---------------------------------*/

char putchar(char c)
{
   /* wait for acknowledge to go low */
   while (LPT_ACK);

   /* output data */
   LPT_DATA = c;

   /* negative strobe */
   LPT_STROBE = 0;

   /* wati for acknowledge to go high */
   while (!LPT_ACK);

   /* remove strobe */
   LPT_STROBE = 1;

   /* clear data for next input */
   LPT_DATA = 0xFF;

   return c;
}

char getchar_nowait()
{
   char c;

   /* if busy signal high -> no data */
   if (LPT_BUSY == 1)
      return -1;

   /* read byte */
   c = LPT_DATA;

   /* singal received data */
   LPT_SELECT = 0;

   /* wait for busy go high */
   while (LPT_BUSY == 0);

   /* remove select */
   LPT_SELECT = 1;
}

unsigned char gets_wait(char *str, unsigned char size, unsigned char timeout)
{
   unsigned long idata start;
   unsigned char i;
   char c;

   start = time();
   i = 0;
   do {
      c = getchar_nowait();
      if (c != -1 && c != '\n') {
         if (c == '\r') {
            str[i] = 0;
            return i;
         }
         str[i++] = c;
         if (i == size)
            return i;
      }

      yield();

   } while (time() < start + timeout);

   return 0;
}

/*---- User loop function ------------------------------------------*/

void user_loop(void)
{
   char idata str[32];
   unsigned char i, n;

   return;                      // do not execute example code

   /* this could be some example code to demonstrate a 4 chn readout */

   for (i = 0; i < 4; i++) {
      /* send data request */
      printf("READ %d\r\n", i); // goes through putchar()

      /* wait for reply for 2 seconds */
      n = gets_wait(str, sizeof(str), 200);

      /* if correct response, interprete data */
 //     if (n == 10)
 //        user_data.value[i] = atof(str);

      yield();
   }

}
