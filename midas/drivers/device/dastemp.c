/********************************************************************\

  Name:         dastemp.c
  Created by:   Stefan Ritt

  Contents:     DAS-TEMP (Keithley) Device Driver

  $Log: dastemp.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  2001/08/22 13:53:02  midas
  Reorganized directio functions

  Revision 1.1  1999/12/20 10:18:19  midas
  Reorganized driver directory structure

  Revision 1.3  1999/01/14 15:57:53  midas
  Adapted to new "info" structure

  Revision 1.2  1998/10/12 12:18:56  midas
  Added Log tag in header


\********************************************************************/

#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "midas.h"

/*---- globals -----------------------------------------------------*/

typedef struct {
   unsigned short io_base;
} DASTEMP_SETTINGS;

#define DASTEMP_SETTINGS_STR "\
IO Base = WORD : 0x220\n\
"

/*---- device driver routines --------------------------------------*/

INT dastemp_init(HNDLE hKey, void **psettings, INT channels)
{
   int status, size;
   HNDLE hDB;
   DASTEMP_SETTINGS *settings;

   /* allocate info structure */
   settings = calloc(1, sizeof(DASTEMP_SETTINGS));
   *psettings = settings;

   cm_get_experiment_database(&hDB, NULL);

   /* create settings record */
   status = db_create_record(hDB, hKey, "", DASTEMP_SETTINGS_STR);
   if (status != DB_SUCCESS)
      return FE_ERR_ODB;

   size = sizeof(DASTEMP_SETTINGS);
   db_get_record(hDB, hKey, settings, &size, 0);

#ifdef OS_WINNT
   /* open IO address space */
   status = ss_directio_give_port(settings->io_base, settings->io_base + 0x0F);
   if (status != SS_SUCCESS) {
      cm_msg(MERROR, "dastemp_init", "DirectIO driver not installed");
      return FE_ERR_HW;
   }
#endif

   return FE_SUCCESS;
}

/*------------------------------------------------------------------*/

#define CT0     (unsigned short) (settings->io_base+0x00)
#define CT1     (unsigned short) (settings->io_base+0x01)
#define CT2     (unsigned short) (settings->io_base+0x02)
#define CTC     (unsigned short) (settings->io_base+0x03)
#define STATUS  (unsigned short) (settings->io_base+0x04)
#define INTR    (unsigned short) (settings->io_base+0x05)
#define LCZS    (unsigned short) (settings->io_base+0x06)

/*------------------------------------------------------------------*/

INT dastemp_get(DASTEMP_SETTINGS * settings, INT channel, float *pvalue)
{
   unsigned short int data;
   double temp;

//###
//*pvalue = (float) (channel/10.0);
//return FE_SUCCESS;

   /* set up counter 1 for gating */
   _outp(CTC, 0x72);            // HW one shot
   _outp(CT1, 0x40);            // 0x9C40 for 50 Hz rej.
   _outp(CT1, 0x9C);

   /* set up counter 0 for V-F counting */
   _outp(CTC, 0x30);            // event counting mode for cntr0
   _outp(CT0, 0xFF);            // 0xFFFF
   _outp(CT0, 0xFF);
   _outp(LCZS, 0);              // load cntr0

   /* select channel */
   _outp(STATUS, (channel & 31) | 0x80);

   /* trigger a conversion */
   data = _inp(CTC);

   /* wait until "dummy" conversion is ready */
   do {
      data = _inp(STATUS);
   } while (data & (1 << 5));

   /* set up counter 0 for V-F counting */
   _outp(CTC, 0x30);            // event counting mode for cntr0
   _outp(CT0, 0xFF);            // 0xFFFF
   _outp(CT0, 0xFF);
   _outp(LCZS, 0);              // load cntr0

   /* trigger a conversion */
   data = _inp(CTC);

   /* wait until "real" conversion is ready */
   do {
      data = _inp(STATUS);
   } while (data & (1 << 5));

   /* latch counter command */
   _outp(CTC, 0x00);

   /* read data */
   data = _inp(CT0);
   data |= (_inp(CT0) << 8);
   data = 65535 - data;

   temp = data * 0.0125 - 273.15;

   /* round temperature to 0.01 deg */
   temp = ((int) (temp * 100 + 0.5)) / 100.0;

   /* return temperature */
   *pvalue = (float) temp;

   return FE_SUCCESS;
}

/*---- device driver entry point -----------------------------------*/

INT dastemp(INT cmd, ...)
{
   va_list argptr;
   HNDLE hKey;
   INT channel, status;
   float *pvalue;
   void *info;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

   switch (cmd) {
   case CMD_INIT:
      hKey = va_arg(argptr, HNDLE);
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      status = dastemp_init(hKey, info, channel);
      break;

   case CMD_GET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = dastemp_get(info, channel, pvalue);
      break;

   default:
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
