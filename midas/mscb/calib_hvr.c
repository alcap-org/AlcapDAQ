/********************************************************************\

  Name:         calib_hvr.c
  Created by:   Stefan Ritt

  Contents:     Calibration program for HVR-200

  $Log: calib_hvr.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:15  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.9  2004/07/29 15:47:38  midas
  Fixed wrong warning with current offset

  Revision 1.8  2004/05/14 15:09:46  midas
  Increased wait time

  Revision 1.7  2004/03/10 13:28:25  midas
  mscb_init returns device name

  Revision 1.6  2004/03/04 15:28:57  midas
  Changed HVR-500 to HVR-200

  Revision 1.5  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.4  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.3  2003/11/13 12:26:24  midas
  Remove HV before plugging in 1MOhm resistor

  Revision 1.2  2003/09/23 09:24:07  midas
  Added current calibration

  Revision 1.1  2003/09/12 10:07:00  midas
  Initial revision

\********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mscb.h"

/*------------------------------------------------------------------*/

#define CH_CONTROL      0
#define CH_VDEMAND      1
#define CH_VMEAS        2
#define CH_IMEAS        3
#define CH_STATUS       4
#define CH_TRIPCNT      5
#define CH_RAMPUP       6
#define CH_RAMPDOWN     7
#define CH_VLIMIT       8
#define CH_ILIMIT       9
#define CH_TRIPMAX     10
#define CH_ADCGAIN     11
#define CH_ADCOFS      12
#define CH_DACGAIN     13
#define CH_DACOFS      14
#define CH_CURGAIN     15
#define CH_CUROFS      16
#define CH_TEMP        17

/*------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
   float f;
   int fd, adr, size, d;
   char str[80];
   float v_adc1, v_adc2, v_multi1, v_multi2,
       adc_gain, adc_ofs, dac_gain, dac_ofs, i1, i2, i_gain, i_ofs;
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

   printf("Enter address of HVR-200 device: ");
   fgets(str, sizeof(str), stdin);
   adr = atoi(str);

   if (mscb_ping(fd, adr) != MSCB_SUCCESS) {
      printf("No device at address %d found.\n", adr);
      return 0;
   }

   mscb_info_variable(fd, adr, CH_TEMP, &info);
   memset(str, 0, sizeof(str));
   memcpy(str, info.name, 8);
   if (strcmp(str, "Temp") != 0) {
      printf("Incorrect software versionon HVR-200. Expect \"Temp\" on var #%d.\n",
             CH_TEMP);
      return 0;
   }

   /* check temperature */
   size = sizeof(float);
   mscb_read(fd, adr, CH_TEMP, &f, &size);
   if (f < 0 || f > 50) {
      printf("Incorrect temperature reading %1.1lf. Check reference voltage and AGND.\n");
      return 0;
   }

   printf("Calibrate voltage ([y]/n) ? ");
   fgets(str, sizeof(str), stdin);

   if (str[0] != 'n') {

      printf("\nPlease connect multimeter with 1000V range to output,\n");
      printf("connect 1300V to input and press ENTER\n");
      fgets(str, sizeof(str), stdin);

      /* init variables */
      f = 0;
      d = 0;
      mscb_write(fd, adr, CH_VDEMAND, &f, sizeof(float));
      mscb_write(fd, adr, CH_ADCOFS, &f, sizeof(float));
      mscb_write(fd, adr, CH_DACOFS, &f, sizeof(float));
      mscb_write(fd, adr, CH_RAMPUP, &d, sizeof(short));
      mscb_write(fd, adr, CH_RAMPDOWN, &d, sizeof(short));

      f = 1;
      mscb_write(fd, adr, CH_ADCGAIN, &f, sizeof(float));
      mscb_write(fd, adr, CH_DACGAIN, &f, sizeof(float));

      /* set current limit to 3mA */
      f = 3000;
      mscb_write(fd, adr, CH_ILIMIT, &f, sizeof(float));

      /* set demand to 100V */
      f = 100;
      mscb_write(fd, adr, CH_VDEMAND, &f, sizeof(float));

      /* set CSR to HV on, no regulation */
      d = 1;
      mscb_write(fd, adr, CH_CONTROL, &d, 1);

      printf("Please enter voltage from multimeter: ");
      fgets(str, sizeof(str), stdin);
      v_multi1 = (float) fabs(atof(str));

      /* read ADC */
      size = sizeof(float);
      mscb_read(fd, adr, CH_VMEAS, &v_adc1, &size);

      printf("HVR-200 ADC reads %1.1lf Volt\n", v_adc1);

      if (v_adc1 < 50) {
         printf("HVR-200 ADC voltage too low, aborting.\n");
         return 0;
      }

      /* set demand to 900V */
      f = 900;
      mscb_write(fd, adr, CH_VDEMAND, &f, sizeof(float));

      printf("Please enter voltage from multimeter: ");
      fgets(str, sizeof(str), stdin);
      v_multi2 = (float) fabs(atof(str));

      /* read ADC */
      size = sizeof(float);
      mscb_read(fd, adr, CH_VMEAS, &v_adc2, &size);

      printf("HVR-200 ADC reads %1.1lf\n", v_adc2);

      if (v_adc2 < 500) {
         printf("HVR-200 ADC voltage too low, aborting.\n");
         return 0;
      }

      /* calculate corrections */
      dac_gain = (float) ((900.0 - 100.0) / (v_multi2 - v_multi1));
      dac_ofs = (float) (100 - dac_gain * v_multi1);

      adc_gain = (float) ((v_multi2 - v_multi1) / (v_adc2 - v_adc1));
      adc_ofs = (float) (v_multi1 - adc_gain * v_adc1);

      printf("\n\nCalculated calibration constants:\n");
      printf("DAC gain:   %1.5lf\n", dac_gain);
      printf("DAC offset: %1.5lf\n", dac_ofs);
      printf("ADC gain:   %1.5lf\n", adc_gain);
      printf("ADC offset: %1.5lf\n", adc_ofs);

      printf("\nWrite values to flash? ([y]/n) ");
      fgets(str, sizeof(str), stdin);
      if (str[0] == 'n') {
         printf("Calibration aborted.\n");
         return 0;
      }

      mscb_write(fd, adr, CH_ADCGAIN, &adc_gain, sizeof(float));
      mscb_write(fd, adr, CH_ADCOFS, &adc_ofs, sizeof(float));
      mscb_write(fd, adr, CH_DACGAIN, &dac_gain, sizeof(float));
      mscb_write(fd, adr, CH_DACOFS, &dac_ofs, sizeof(float));

      /* remove voltage */
      f = 0;
      mscb_write(fd, adr, CH_VDEMAND, &f, sizeof(float));

      /* write constants to EEPROM */
      mscb_flash(fd, adr);
   }

   printf("Calibrate current ([y]/n) ? ");
   fgets(str, sizeof(str), stdin);

   if (str[0] != 'n') {

      /* init variables */
      f = 0;
      d = 0;
      mscb_write(fd, adr, CH_RAMPUP, &d, sizeof(short));
      mscb_write(fd, adr, CH_RAMPDOWN, &d, sizeof(short));
      mscb_write(fd, adr, CH_CUROFS, &f, sizeof(float));        /* CURofs  */
      mscb_write(fd, adr, CH_VDEMAND, &f, sizeof(float));

      f = 1;
      mscb_write(fd, adr, CH_CURGAIN, &f, sizeof(float));       /* CURgain */

      /* set current limit to 3mA */
      f = 3000;
      mscb_write(fd, adr, CH_ILIMIT, &f, sizeof(float));

      /* set CSR to HV on, no regulation */
      d = 1;
      mscb_write(fd, adr, CH_CONTROL, &d, 1);

      printf("\nPlease connect 1MOhm resistor to output,\n");
      printf("connect 1300V to input and press ENTER\n");
      fgets(str, sizeof(str), stdin);

      /* set demand to 100V */
      f = 100;
      mscb_write(fd, adr, CH_VDEMAND, &f, sizeof(float));

      /* wait voltage to settle */
      Sleep(3000);

      /* read current */
      size = sizeof(float);
      mscb_read(fd, adr, CH_IMEAS, &i1, &size);
      printf("Current at 100V: %1.1lf uA\n", i1);

      do {
         /* set demand to 900V */
         f = 900;
         mscb_write(fd, adr, CH_VDEMAND, &f, sizeof(float));

         /* wait voltage to settle */
         Sleep(3000);

         /* read voltage */
         size = sizeof(float);
         mscb_read(fd, adr, CH_VMEAS, &v_adc1, &size);

         if (v_adc1 < 890) {
            v_adc2 = 900 - v_adc1 + 100;
            v_adc2 = (float) ((int) (v_adc2 / 100.0) * 100);

            printf("Only %1.1lf V can be reached on output,\n", v_adc1);
            printf("please increase input voltage by %1.0lf V and press ENTER.\n",
                   v_adc2);
            fgets(str, sizeof(str), stdin);
         }

      } while (v_adc1 < 890);

      /* read current */
      size = sizeof(float);
      mscb_read(fd, adr, CH_IMEAS, &i2, &size);
      printf("Current at 900V: %1.1lf uA\n", i2);

      /* calculate corrections */
      i_gain = (float) ((900 - 100) / (i2 - i1));
      i_ofs = (float) (100 - i_gain * i1);

      printf("\n\nCalculated calibration constants:\n");
      printf("I gain:   %1.5lf\n", i_gain);
      printf("I offset: %1.5lf\n", i_ofs);

      if (i_ofs > 0) {
         printf
             ("\nWARNING: The current offset is positive, which means that a current\n");
         printf
             ("below %1.0lfuA cannot be measured. A compensation resistor to Vref is missing.\n\n",
              i_ofs);
      }

      printf("\nWrite values to flash? ([y]/n) ");
      fgets(str, sizeof(str), stdin);
      if (str[0] == 'n') {
         printf("Calibration aborted.\n");
         return 0;
      }

      mscb_write(fd, adr, CH_CURGAIN, &i_gain, sizeof(float));
      mscb_write(fd, adr, CH_CUROFS, &i_ofs, sizeof(float));

      /* remove voltage */
      f = 0;
      mscb_write(fd, adr, CH_VDEMAND, &f, sizeof(float));
      d = 2;
      mscb_write(fd, adr, CH_CONTROL, &d, 1);

      /* write constants to EEPROM */
      mscb_flash(fd, adr);
   }

   printf("\nCalibration finished.\n");

   mscb_exit(fd);

   return 0;
}
