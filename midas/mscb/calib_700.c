/********************************************************************\

  Name:         calib_700.c
  Created by:   Stefan Ritt

  Contents:     Calibration program for SCS-700

  $Log: calib_700.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:15  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/03/10 13:28:25  midas
  mscb_init returns device name

  Revision 1.2  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.1  2003/11/18 14:22:52  midas
  Initial revision

  Revision 1.1  2003/06/11 14:13:33  midas
  Version 1.4.5

\********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mscb.h"

/*------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
   float v;
   int i, n, fd, adr, size;
   short s;
   char str[80];
   MSCB_INFO_VAR info;

   /* open port */
   *str = 0;
   fd = mscb_init(str, sizeof(str), 0);
   if (fd < 0) {
      if (str[0])
         printf("Cannot connect to device \"%s\".\n", str);
      else
         printf("No MSCB submaster found.\n");

      return 0;
   }

   printf("Enter address of SCS-700 device: ");
   fgets(str, sizeof(str), stdin);
   adr = atoi(str);

   if (mscb_ping(fd, adr) != MSCB_SUCCESS) {
      printf("No device at address %d found.\n", adr);
      return 0;
   }

   mscb_info_variable(fd, adr, 14, &info);
   memset(str, 0, sizeof(str));
   memcpy(str, info.name, 6);
   if (strcmp(str, "Period") != 0 && strcmp(str, "Temp6") != 0) {
      printf
          ("Incorrect software versionon SCS-700. Expect \"Period\" or \"Temp6\" on var #14.\n");
      return 0;
   }

   if (strcmp(str, "Temp6") == 0)
      n = 8;
   else
      n = 2;

   printf("\nConnect 100 Ohm resistors to all inputs and press ENTER\n");
   fgets(str, sizeof(str), stdin);

   if (n == 8) {
      /* 8-channel firmware */

      /* read temperatures, calculate corrections */
      for (i = 0; i < 8; i++) {
         size = sizeof(v);
         mscb_read(fd, adr, (unsigned char) (i + 8), &v, &size);

         if (v < -50 || v > 50) {
            printf
                ("Error: Temperature %1.2lf seems totally wrong, skipping channel %d\n",
                 v, i);
            continue;
         }

         v = -(v * 100);

         /* read old offset */
         size = sizeof(s);
         mscb_read(fd, adr, (unsigned char) (i + 16), &s, &size);

         /* calculate new offset */
         s += (int) floor(v + 0.5);

         /* write new offset */
         mscb_write(fd, adr, (unsigned char) (i + 16), &s, sizeof(short));
         printf("Ofs%d = %d\n", i, s);
      }
   } else {
      /* 2-channel firmware */

      /* read temperatures, calculate corrections */
      for (i = 0; i < 2; i++) {
         size = sizeof(v);
         mscb_read(fd, adr, (unsigned char) (i + 2), &v, &size);

         if (v < -50 || v > 50) {
            printf
                ("Error: Temperature %1.2lf seems totally wrong, skipping channel %d\n",
                 v, i);
            continue;
         }

         v = -(v * 100);

         /* read old offset */
         size = sizeof(s);
         mscb_read(fd, adr, (unsigned char) (i + 12), &s, &size);

         /* calculate new offset */
         s += (int) floor(v + 0.5);

         /* write new offset */
         mscb_write(fd, adr, (unsigned char) (i + 12), &s, sizeof(short));
         printf("Ofs%d = %d\n", i, s);
      }
   }

   /* write constants to EEPROM */
   mscb_flash(fd, adr);

   printf("\nCalibration finished.\n");

   mscb_exit(fd);

   return 0;
}
