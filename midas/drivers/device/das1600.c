/********************************************************************\

  Name:         das1600.c
  Created by:   Stefan Ritt

  Contents:     DAS-1600 (Keithley) Device Driver

  $Log: das1600.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.4  2004/05/10 19:04:45  midas
  Changed CH_IN/OUTUT to DF_IN/OUTPUT

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
   unsigned short gain[16];
} DAS1600_SETTINGS;

#define DAS1600_SETTINGS_STR "\
IO Base = WORD : 0x240\n\
Gain = WORD[16] :\n\
1\n\
1\n\
1\n\
1\n\
1\n\
1\n\
1\n\
1\n\
1\n\
1\n\
1\n\
1\n\
1\n\
1\n\
1\n\
1\n\
"

/*---- device driver routines --------------------------------------*/

INT das1600_init(HNDLE hKey, void **psettings, INT channels, DWORD type)
{
   int status, size;
   HNDLE hDB;
   DAS1600_SETTINGS *settings;

   /* allocate info structure */
   settings = calloc(1, sizeof(DAS1600_SETTINGS));
   *psettings = settings;

   cm_get_experiment_database(&hDB, NULL);

   /* create settings record */
   status = db_create_record(hDB, hKey, "", DAS1600_SETTINGS_STR);
   if (status != DB_SUCCESS)
      return FE_ERR_ODB;

   size = sizeof(DAS1600_SETTINGS);
   db_get_record(hDB, hKey, settings, &size, 0);

#ifdef OS_WINNT
   /* open IO address space */
   status = ss_directio_give_port(settings->io_base, settings->io_base + 0x0F);
   if (status != SS_SUCCESS) {
      cm_msg(MERROR, "das1600_init", "DirectIO device driver not installed");
      return FE_ERR_HW;
   }
#endif

   return FE_SUCCESS;
}

/*----------------------------------------------------------------------------*/

#define AD0     (unsigned short) (settings->io_base+0x00)
#define AD1     (unsigned short) (settings->io_base+0x01)
#define MUX     (unsigned short) (settings->io_base+0x02)
#define DIO     (unsigned short) (settings->io_base+0x03)
#define DA0LB   (unsigned short) (settings->io_base+0x04)
#define DA0HB   (unsigned short) (settings->io_base+0x05)
#define DA1LB   (unsigned short) (settings->io_base+0x06)
#define DA1HB   (unsigned short) (settings->io_base+0x07)
#define STATUS  (unsigned short) (settings->io_base+0x08)
#define CTRL    (unsigned short) (settings->io_base+0x09)
#define GAIN    (unsigned short) (settings->io_base+0x0b)

/*------------------------------------------------------------------*/

INT das1600_get_analog(DAS1600_SETTINGS * settings, INT channel, float *pvalue)
{
   unsigned short int data;
   double volts;

//###
//*pvalue = (float) (channel/10.0);
//return FE_SUCCESS;

   /* set MUX channel */
   _outp(MUX, channel | (channel << 4));

   /* diable interrupts and DMA */
   _outp(CTRL, 0);

   /* set gain */
   _outp(GAIN, settings->gain[channel] == 1 ? 0 :
         settings->gain[channel] == 10 ? 1 :
         settings->gain[channel] == 100 ? 2 : settings->gain[channel] == 500 ? 3 : 0);

   /* start conversion */
   _outp(AD0, 0);

   /* wait for conversion */
   do {
      data = _inp(STATUS);
   } while (data & 0x80);

   data = _inpw(AD0);

   /* convert to volts assuming bipolar operation */
   volts = ((data >> 4) - 2048.0) / 4096.0;


   volts = volts * (10 - (-10)) / settings->gain[channel];

   /* return volts */
   *pvalue = (float) volts;

   return FE_SUCCESS;
}

/*------------------------------------------------------------------*/

INT das1600_set_analog(DAS1600_SETTINGS * settings, INT channel, float voltage)
{
   signed short int data;

   /* assume +-10 V bipolar range */
//  data = (unsigned short int) (voltage / 10.0 * 2048 + 2048.5);

   /* assume +10 V unipolar range */
   data = (unsigned short int) (voltage / 10.0 * 4096);

   /* check if in range */
   if (data > 4095)
      data = 4095;
   if (data < 0)
      data = 0;

   if (channel == 0)
      _outpw(DA0LB, (unsigned short) (data << 4));
   else
      _outpw(DA1LB, (unsigned short) (data << 4));

   return FE_SUCCESS;
}

/*---- device driver entry points ----------------------------------*/

INT das1600_ai(INT cmd, ...)
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
      status = das1600_init(hKey, info, channel, DF_INPUT);
      break;

   case CMD_GET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      pvalue = va_arg(argptr, float *);
      status = das1600_get_analog(info, channel, pvalue);
      break;

   default:
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/

INT das1600_ao(INT cmd, ...)
{
   va_list argptr;
   HNDLE hKey;
   INT channel, status;
   float value;
   void *info;

   va_start(argptr, cmd);
   status = FE_SUCCESS;

   switch (cmd) {
   case CMD_INIT:
      hKey = va_arg(argptr, HNDLE);
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      status = das1600_init(hKey, info, channel, DF_OUTPUT);
      break;

   case CMD_SET:
      info = va_arg(argptr, void *);
      channel = va_arg(argptr, INT);
      value = (float) va_arg(argptr, double);
      status = das1600_set_analog(info, channel, value);
      break;

   default:
      break;
   }

   va_end(argptr);

   return status;
}

/*------------------------------------------------------------------*/
