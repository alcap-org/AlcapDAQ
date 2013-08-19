/********************************************************************\

  Name:         adccalib.c
  Created by:   Stefan Ritt

  Contents:     Example analyzer module for ADC calibration. Looks
                for ADC0 bank, subtracts pedestals and applies gain
                calibration. The resulting values are appended to 
                the event as an CADC bank ("calibrated ADC"). The
                pedestal values and software gains are stored in
                adccalib_param structure which was defined in the ODB
                and transferred to experim.h.

  $Log: adccalib.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.10  2004/09/23 19:22:49  midas
  Use new histo booking

  Revision 1.9  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.8  2003/04/25 14:49:46  midas
  Removed HBOOK code

  Revision 1.7  2003/04/23 15:09:47  midas
  Removed user branch

  Revision 1.6  2003/04/22 15:00:27  midas
  Worked on ROOT histo booking

  Revision 1.5  2003/04/21 04:02:13  olchansk
  replace MANA_LITE with HAVE_HBOOK and HAVE_ROOT
  implement ROOT-equivalents of the sample HBOOK code
  implement example of adding custom branches to the analyzer root output file

  Revision 1.4  2003/04/21 03:51:41  olchansk
  kludge misunderstanding of the ADC0 bank size.

  Revision 1.3  2002/05/10 05:22:47  pierre
  add MANA_LITE #ifdef

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <time.h>

/* midas includes */
#include "midas.h"
#include "experim.h"
#include "analyzer.h"

/* root includes */
#include <TH1F.h>
#include <TTree.h>

/*-- Parameters ----------------------------------------------------*/

ADC_CALIBRATION_PARAM adccalib_param;
extern EXP_PARAM exp_param;
extern RUNINFO runinfo;

/*-- Module declaration --------------------------------------------*/

INT adc_calib(EVENT_HEADER *, void *);
INT adc_calib_init(void);
INT adc_calib_bor(INT run_number);
INT adc_calib_eor(INT run_number);

ADC_CALIBRATION_PARAM_STR(adc_calibration_param_str);

ANA_MODULE adc_calib_module = {
   "ADC calibration",           /* module name           */
   "Stefan Ritt",               /* author                */
   adc_calib,                   /* event routine         */
   adc_calib_bor,               /* BOR routine           */
   adc_calib_eor,               /* EOR routine           */
   adc_calib_init,              /* init routine          */
   NULL,                        /* exit routine          */
   &adccalib_param,             /* parameter structure   */
   sizeof(adccalib_param),      /* structure size        */
   adc_calibration_param_str,   /* initial parameters    */
};

/*-- module-local variables ----------------------------------------*/

static TH1F *hAdcHists[N_ADC];

/*-- init routine --------------------------------------------------*/

#define ADC_N_BINS   500
#define ADC_X_LOW      0
#define ADC_X_HIGH  4000

INT adc_calib_init(void)
{
   char name[256];
   int i;

   /* book CADC histos */

   for (i = 0; i < N_ADC; i++) {
      char title[256];

      sprintf(name, "CADC%02d", i);
      sprintf(title, "ADC %d", i);

      hAdcHists[i] = H1_BOOK(name, title, ADC_N_BINS, ADC_X_LOW, ADC_X_HIGH);
   }

   return SUCCESS;
}

/*-- BOR routine ---------------------------------------------------*/

INT adc_calib_bor(INT run_number)
{
   return SUCCESS;
}

/*-- eor routine ---------------------------------------------------*/

INT adc_calib_eor(INT run_number)
{
   return SUCCESS;
}

/*-- event routine -------------------------------------------------*/

INT adc_calib(EVENT_HEADER * pheader, void *pevent)
{
   INT i;
   WORD *pdata;
   float *cadc;

   /* look for ADC0 bank, return if not present */
   if (!bk_locate(pevent, "ADC0", &pdata))
      return 1;

   /* create calibrated ADC bank */
   bk_create(pevent, "CADC", TID_FLOAT, &cadc);

   /* zero cadc bank */
   for (i = 0; i < N_ADC; i++)
      cadc[i] = 0.f;

   /* subtract pedestal */
   for (i = 0; i < N_ADC; i++)
      cadc[i] = (float) ((double) pdata[i] - adccalib_param.pedestal[i] + 0.5);

   /* apply software gain calibration */
   for (i = 0; i < N_ADC; i++)
      cadc[i] *= adccalib_param.software_gain[i];

   /* fill ADC histos if above threshold */
   for (i = 0; i < N_ADC; i++)
      if (cadc[i] > (float) adccalib_param.histo_threshold)
         hAdcHists[i]->Fill(cadc[i], 1);

   /* close calculated bank */
   bk_close(pevent, cadc + N_ADC);

   return SUCCESS;
}
