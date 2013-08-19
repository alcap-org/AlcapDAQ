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

  Revision 1.12  2004/09/30 23:35:33  midas
  Added some simple tests

  Revision 1.11  2004/09/23 19:22:43  midas
  Use new histo booking

  Revision 1.10  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.9  2003/11/14 13:43:10  midas
  Fixed type

  Revision 1.8  2003/04/28 15:33:05  midas
  Fixed compiler warnings

  Revision 1.7  2003/04/25 14:49:46  midas
  Removed HBOOK code

  Revision 1.6  2003/04/23 15:09:29  midas
  Added 'average' in ASUM bank

  Revision 1.5  2003/04/22 15:00:27  midas
  Worked on ROOT histo booking

  Revision 1.4  2003/04/21 04:02:13  olchansk
  replace MANA_LITE with HAVE_HBOOK and HAVE_ROOT
  implement ROOT-equivalents of the sample HBOOK code
  implement example of adding custom branches to the analyzer root output file

  Revision 1.3  2002/05/10 05:22:59  pierre
  add MANA_LITE #ifdef

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <math.h>

#define DEFINE_TESTS // must be defined prior to midas.h

/* midas includes */
#include "midas.h"
#include "experim.h"
#include "analyzer.h"

/* root includes */
#include <TH1F.h>

#ifndef PI
#define PI 3.14159265359
#endif

/*-- Parameters ----------------------------------------------------*/

ADC_SUMMING_PARAM adc_summing_param;

/*-- Tests ---------------------------------------------------------*/

DEF_TEST(low_sum);
DEF_TEST(high_sum);

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

static TH1F *hAdcSum, *hAdcAvg;

/*-- init routine --------------------------------------------------*/

INT adc_summing_init(void)
{
   /* book ADC sum histo */
   hAdcSum = H1_BOOK("ADCSUM", "ADC sum", 500, 0, 10000);

   /* book ADC average in separate subfolder */
   open_subfolder("Average");
   hAdcAvg = H1_BOOK("ADCAVG", "ADC average", 500, 0, 10000);
   close_subfolder();

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

   /* evaluate tests */
   SET_TEST(low_sum, asum->sum < 1000);
   SET_TEST(high_sum, asum->sum > 1000);

   /* fill sum histo */
   hAdcSum->Fill(asum->sum, 1);

   /* fill average histo */
   hAdcAvg->Fill(asum->average);

   /* close calculated bank */
   bk_close(pevent, asum + 1);

   return SUCCESS;
}
