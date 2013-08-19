/*********************************************************************

  Name:         ps7106.c
  Created by:   Stefan Ritt

  Cotents:      Routines for accessing Phillips Scientific
                7106 discriminators


  Revision history
  ------------------------------------------------------------------
  date        by    modification
  ---------   ---   ------------------------------------------------
  13-NOV-96   SR    created

*********************************************************************/

#include <stdio.h>
#include <dos.h>
#include <math.h>

#include "midas.h"
#include "mcstd.h"
#include "ps7106.h"

/*------------------------------------------------------------------*/

#define READ_MASK_7106          0
#define READ_LATCH_7106         0
#define READ_THRESH_7106        1
#define READ_STATUS_7106        1
#define WRITE_MASK_7106         16
#define WRITE_THRESH_7106       17
#define START_ADC_7106          17
#define SET_LOCAL_7106          24
#define TEST_DISC_7106          25
#define SET_REMOTE_7106         26
#define TEST_REMOTE_7106        27

/*------------------------------------------------------------------*/

int ps7106_set(int crate, int slot, double thresh_value)
/********************************************************************\
  Sets the threshold of a Phillips 7106 discriminator to the given
  <thresh_value> in millivolts.
\********************************************************************/
{
   INT q;
   WORD data, threshold;

   // Convert threshold value to register value
   if (thresh_value > 1033)
      thresh_value = 1033;
   if (thresh_value < 10)
      thresh_value = 10;
   threshold = (unsigned int) (thresh_value - 10);

   /* Set remote mode */
   camo(crate, slot, 0, SET_REMOTE_7106, 0);

   /* Verify Remote mode */
   camc_q(crate, slot, 0, TEST_REMOTE_7106, &q);
   if (!q) {
      cm_msg(MERROR, "ps7106_set", "Error setting remote mode on crate %d, slot %d.\n",
             crate, slot);
//    printf("Error setting remote mode on crate %d, slot %d.\n", crate, slot);
      return -1;
   }

   /* unmask all channels */
   camo(crate, slot, 0, 16, 0xFFFF);

   /* Write threshold */
   camo(crate, slot, 0, WRITE_THRESH_7106, threshold);
   ss_sleep(10);

   /* Verify threshold */
   cami(crate, slot, 0, READ_THRESH_7106, &data);
   data = data & 0x3FF;
   if (data != threshold) {
      cm_msg(MERROR, "ps7106_set", "Error setting threshold:\
 requested %d (%4.0f mV), read %d", threshold, thresh_value, data);
//    printf("Error setting threshold:\
// requested %d (%4.0f mV), read %d", threshold, thresh_value, data);
      return -1;
   }

   /* Measure threshold */
   camo(crate, slot, 1, START_ADC_7106, threshold);
   ss_sleep(10);

   cami(crate, slot, 1, READ_THRESH_7106, &data);
   data = data & 0x3FF;

   if (fabs((double) data - thresh_value) > 2.0) {
      cm_msg(MERROR, "ps7106_set", "Error setting threshold on crate %d, slot %d:\
 requested %1.0lfmV, measured %dmV", crate, slot, thresh_value, data);
//    printf("Error setting threshold on crate %d, slot %d:\
// requested %1.0lfmV, measured %dmV", crate, slot, thresh_value, data);
   }

   return 0;
}
