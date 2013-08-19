/********************************************************************\

  Name:         calib_520.c
  Created by:   Stefan Ritt

  Contents:     Calibration program for SCS-520

  $Log: calib_520.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:15  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.4  2004/03/10 13:28:25  midas
  mscb_init returns device name

  Revision 1.3  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.2  2004/01/07 12:52:23  midas
  Changed indentation

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
   int i, fd, adr, size, d;
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

   printf("Enter address of SCS-520 device: ");
   fgets(str, sizeof(str), stdin);
   adr = atoi(str);

   if (mscb_ping(fd, adr) != MSCB_SUCCESS) {
      printf("No device at address %d found.\n", adr);
      return 0;
   }

   mscb_info_variable(fd, adr, 44, &info);
   memset(str, 0, sizeof(str));
   memcpy(str, info.name, 8);
   if (strcmp(str, "GlblGain") != 0) {
      printf("Incorrect software versionon SCS-520. Expect \"GlblGain\" on var #44.\n");
      return 0;
   }

   printf("\nDisconnect all inputs and press ENTER\n");
   fgets(str, sizeof(str), stdin);

   d = 0;
   v = 0;
   for (i = 0; i < 8; i++) {
      /* set all ranges to 0-10V */
      mscb_write(fd, adr, (unsigned char) (i + 12), &d, 1);

      /* set all offset to 0 */
      mscb_write(fd, adr, (unsigned char) (i + 20), &d, 1);

      /* set all gain correction to 0 */
      mscb_write(fd, adr, (unsigned char) (i + 28), &d, 1);

      /* set all bip offset 0 */
      mscb_write(fd, adr, (unsigned char) (i + 36), &d, 1);
   }

   /* set global gain to 1 */
   v = 1;
   mscb_write(fd, adr, 44, &v, 4);

   /* wait for voltages to settle */
   Sleep(1000);

   /* read voltages, calculate corrections */
   for (i = 0; i < 8; i++) {
      size = sizeof(v);
      mscb_read(fd, adr, (unsigned char) i, &v, &size);

      v = -(v * 1000);

      d = (int) floor(v + 0.5);
      mscb_write(fd, adr, (unsigned char) (i + 20), &d, 1);
      printf("Ofs%d = %d\n", i, d);
      if (d < -127 || d > 128)
         printf("ERROR: offset out of range\n");
   }

   /* switch to bipolar mode */
   for (i = 0; i < 8; i++) {
      /* set all ranges to 0-10V */
      d = 4;
      mscb_write(fd, adr, (unsigned char) (i + 12), &d, 1);
   }

   /* wait for voltages to settle */
   Sleep(1000);

   /* read voltages, calculate corrections */
   for (i = 0; i < 8; i++) {
      size = sizeof(v);
      mscb_read(fd, adr, (unsigned char) i, &v, &size);

      v = -(v * 100);

      d = (int) floor(v + 0.5);
      mscb_write(fd, adr, (unsigned char) (i + 36), &d, 1);
      printf("BipOfs%d = %d\n", i, d);
      if (d < -127 || d > 128)
         printf("ERROR: offset out of range\n");
   }

   /* switch to unipolar mode */
   for (i = 0; i < 8; i++) {
      /* set all ranges to 0-10V */
      d = 0;
      mscb_write(fd, adr, (unsigned char) (i + 12), &d, 1);
   }

   printf("\nApply +10V to all inputs and press ENTER\n");
   fgets(str, sizeof(str), stdin);

   /* read voltages, calculate corrections */
   for (i = 0; i < 8; i++) {
      size = sizeof(v);
      mscb_read(fd, adr, (unsigned char) i, &v, &size);

      v = (10 / v - 1) * 10000;

      d = (int) floor(v + 0.5);
      mscb_write(fd, adr, (unsigned char) (i + 28), &d, 1);
      printf("Gain%d = %d\n", i, d);
      if (d < -127 || d > 128)
         printf("ERROR: gain out of range\n");
   }

   /* write constants to EEPROM */
   mscb_flash(fd, adr);

   printf("\nCalibration finished.\n");

   mscb_exit(fd);

   return 0;
}
