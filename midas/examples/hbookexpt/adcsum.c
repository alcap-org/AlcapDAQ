/********************************************************************\

  Name:         adcsum.c
  Created by:   Stefan Ritt

  Contents:     Example analyzer module for ADC summing. This module
                looks for a bank named CADC and produces an ASUM
                bank which contains the sum of all ADC values. The
                ASUM bank is a "structured" bank. It has been defined
                in the ODB and transferred to experim.h.

  $Log: adcsum.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.2  2003/12/10 13:56:08  midas
  Fixed HBOOK compiler warning

  Revision 1.1  2003/04/25 13:20:07  midas
  Initial revision


\********************************************************************/

/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <math.h>

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

#ifndef PI
#define PI 3.14159265359
#endif

/*-- Parameters ----------------------------------------------------*/

ADC_SUMMING_PARAM adc_summing_param;

/*-- Module declaration --------------------------------------------*/

INT adc_summing(EVENT_HEADER *, void *);
INT adc_summing_init(void);
INT adc_summing_bor(INT run_number);

ADC_SUMMING_PARAM_STR(adc_summing_param_str);

ANA_MODULE adc_summing_module = {
   "ADC summing",               /* module name           */
   "Stefan Ritt",               /* author                */
   adc_summing,                 /* event routine         */
   NULL,                        /* BOR routine           */
   NULL,                        /* EOR routine           */
   adc_summing_init,            /* init routine          */
   NULL,                        /* exit routine          */
   &adc_summing_param,          /* parameter structure   */
   sizeof(adc_summing_param),   /* structure size        */
   adc_summing_param_str,       /* initial parameters    */
};

/*-- Module-local variables-----------------------------------------*/

#ifdef HAVE_ROOT
extern TDirectory *gManaHistsDir;

static TH1F *gAdcSumHist;
#endif

/*-- init routine --------------------------------------------------*/

INT adc_summing_init(void)
{
   char name[80];

   /* book sum histo */
   strcpy(name, "ADC sum");
   HBOOK1(ADCSUM_ID_BASE, name, 500, 0.f, 10000.f, 0.f);

   return SUCCESS;
}

/*-- event routine -------------------------------------------------*/

INT adc_summing(EVENT_HEADER * pheader, void *pevent)
{
   INT i, j, n_adc;
   float *cadc;
   ASUM_BANK *asum;

   /* look for CADC bank, return if not present */
   n_adc = bk_locate(pevent, "CADC", &cadc);
   if (n_adc == 0)
      return 1;

   /* create ADC sum bank */
   bk_create(pevent, "ASUM", TID_STRUCT, &asum);

   /* sum all channels above threashold */
   asum->sum = 0.f;
   for (i = j = 0; i < n_adc; i++)
      if (cadc[i] > adc_summing_param.adc_threshold) {
         asum->sum += cadc[i];
         j++;
      }

   /* calculate ADC average */
   asum->average = j > 0 ? asum->sum / j : 0;

   /* fill sum histo */
   HF1(ADCSUM_ID_BASE, asum->sum, 1.f);

   /* close calculated bank */
   bk_close(pevent, asum + 1);

   return SUCCESS;
}
