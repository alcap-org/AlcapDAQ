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

  Revision 1.2  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.1  2003/04/25 13:20:07  midas
  Initial revision

  
\********************************************************************/

/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <time.h>

/* midas includes */
#include "midas.h"
#include "experim.h"
#include "analyzer.h"

/* cernlib includes */
#ifdef OS_WINNT
#define VISUAL_CPLUSPLUS
#endif

#ifdef __linux__
#define f2cFortran
#endif

#include <cfortran.h>
#include <hbook.h>

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

#ifdef HAVE_ROOT
extern TDirectory *gManaHistsDir;

static TH1F *gAdcHists[N_ADC];
#endif

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
      sprintf(name, "CADC%02d", i);
      HBOOK1(ADCCALIB_ID_BASE + i, name, ADC_N_BINS,
             (float) ADC_X_LOW, (float) ADC_X_HIGH, 0.f);
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
         HF1(ADCCALIB_ID_BASE + i, cadc[i], 1.f);

   /* close calculated bank */
   bk_close(pevent, cadc + N_ADC);

   return SUCCESS;
}
