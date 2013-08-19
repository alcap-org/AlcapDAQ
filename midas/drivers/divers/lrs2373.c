/*********************************************************************

  Name:         lrs2373.c
  Created by:   Stefan Ritt

  Cotents:      Routines for accessing LeCroy 2372 MLU
                
  $Log: lrs2373.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2004/01/08 07:51:21  midas
  Changed indentation

  Revision 1.1  1999/12/20 10:18:23  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:57  midas
  Added Log tag in header


*********************************************************************/

#include <stdio.h>
#include <dos.h>
#include <string.h>

#include "midas.h"

#define CAMAC_BASE_DEFINED
#define BYTE_DEFINED
#include "hyt1331.h"
#include "lrs2373.h"

/*------------------------------------------------------------------*/

int lrs2373_set(int camac_slot, char *filename)
/**********************************************************************\
   Description:

   Loads the memory of a LeCroy 2373 memory lookup unit.  Data to
   be loaded is taken from the file <filename>, in the format:
	     <input1 (HEX)>  <output1 (HEX)>
	     <input2 (HEX)>  <output2 (HEX)>
	     <input3 (HEX)>  <output3 (HEX)>
	     .
	     .
	     .
   giving the MLU output to be associated with each possible MLU input
   (0x0000 to 0xFFFF).  Each output is verified after loading.  The
   parameter <camac_slot> gives the number of the CAMAC slot in which the
   MLU resides.

   Alternatively, following filenames are reserved. The data
   is generated directly and stored in the MLU, which is faster
   than reading in a file:

   "1TO1"   Output equals input
   "OR1"    Output one is the or of all inputs
   "ORALL"  All outputs are the or of all inputs

\**********************************************************************/
{
   int q, x;
   WORD i, value, mode;
   WORD data;
   FILE *fd;

   mode = 0;
   if (strcmp(filename, "1TO1") == 0)
      mode = 1;
   else if (strcmp(filename, "OR_1") == 0)
      mode = 2;
   else if (strcmp(filename, "OR_ALL") == 0)
      mode = 3;

   /* Open input file */
   if (mode == 0) {
      fd = fopen(filename, "r");
      if (fd == NULL) {
         printf("lrs2373_set: Cannot find file \"%s\".\n", filename);
         return -1;
      }
   }
   printf("Loading %s into MLU in slot %d...\n", filename, camac_slot);

   /* Crate setup is done in InitUserDown */

   /* Set MLU inhibit and 2373 mode */
   value = SET_INHIBIT | SET_2373MODE;
   CAMO(camac_slot, CCR, WRITE, value);

   /* Verify MLU mode */
   data = 0;
   CAMI(camac_slot, CCR, READ, &data);
   if (data != value) {
      printf("lrs2373_set: Error setting inhibit/2373 mode. CCR is %X, should be %X.\n",
             data, SET_INHIBIT | SET_2373MODE);
      return -1;
   }

   /* set CAR to zero and verify */
   CAMO(camac_slot, CAR, WRITE, 0);
   CAMI(camac_slot, CAR, READ, &value);
   if (value != 0) {
      printf("lrs2373_set: Error setting CAR register. CAR is %X, should be %X.\n",
             value, 0);
      return -1;
   }

   /* Loop through input permutations, setting and verifying
      output for each */

   for (i = 0;; i++) {
      if (mode == 0)
         fscanf(fd, "%X", &value);
      else if (mode == 1)
         value = i;
      else if (mode == 2)
         value = i == 0 ? 0 : 1;
      else if (mode == 3)
         value = i == 0 ? 0 : 0xFFFF;

      if ((i & 0x0FFF) == 0xFFF)
         printf(".");

      /* Write output data */
      if (i == 0) {
         CAMO_RSTART(camac_slot, DATA, WRITE, value);
      } else {
         CAMO_REPEAT(value);
      }

      if (i == 0xFFFF)
         break;
   }

   /* Verify data */
   if (mode == 0)
      rewind(fd);

   CAMO(camac_slot, CAR, WRITE, 0);
   for (i = 0;; i++) {
      if (mode == 0)
         fscanf(fd, "%X", &value);
      else if (mode == 1)
         value = i;
      else if (mode == 2)
         value = i == 0 ? 0 : 1;
      else if (mode == 3)
         value = i == 0 ? 0 : 0xFFFF;

      if ((i & 0x0FFF) == 0xFFF)
         printf("=");

      /* Read data and compare */
      data = 0;
      CAMI(camac_slot, DATA, READ, &data);

      if (data != value) {
         printf("Error verifying data: Was %X, should be %X\n", data, value);
//      return -1;
      }

      if (i == 0xFFFF)
         break;
   }

   /* Set Transparent and 2373 mode */
   value = SET_TRANSPARENT | SET_2373MODE;
   CAMO(camac_slot, CCR, WRITE, value);

   /* Verify MLU mode */
   data = 0;
   CAMI(camac_slot, CCR, READ, &data);
   if (data != value) {
      printf("Error setting transparent/2373 mode.  CCR was %X, should be %X.\n",
             data, value);
      return -1;
   }

   if (mode == 0)
      fclose(fd);

   printf("\n");

   return (0);
}
