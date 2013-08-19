/********************************************************************\

  Name:         rs232test.c
  Created by:   Stefan Ritt

  Contents:     Simple RS232 test program, needs  
                midas/drivers/bus/rs232.c

  $Log: rs232test.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:38  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:11  midas
  Implemented standard indentation

  Revision 1.2  2001/01/05 15:25:43  midas
  Added variable port name

  Revision 1.1  2001/01/05 15:08:50  midas
  Initial revision

\********************************************************************/

#include <midas.h>
#include "rs232.c"

int main()
{
   RS232_INFO info;
   char str[10000];

   printf("Enter port [/dev/ttyS0]: ");
   fgets(str, sizeof(str), stdin);
   if (strchr(str, '\n'))
      *strchr(str, '\n') = 0;

   if (!str[0])
      strcpy(str, "/dev/ttyS0");

   info.fd = rs232_open(str, 9600, 'N', 8, 1, 0);

   if (info.fd < 0) {
      printf("Cannot open ttyS0\n");
      return 0;
   }

   /* turn on debugging, will go to rs232.log */
   rs232(CMD_DEBUG, TRUE);

   printf("Connected to ttyS0, exit with <ESC>\n");

   do {
      memset(str, 0, sizeof(str));
      str[0] = ss_getchar(0);
      if (str[0] == 27)
         break;

      if (str[0] > 0)
         rs232_puts(&info, str);

      rs232_gets(&info, str, sizeof(str), "", 10);
      printf(str);
      fflush(stdout);

   } while (1);

   ss_getchar(TRUE);
   rs232_exit(&info);

   return 1;
}
