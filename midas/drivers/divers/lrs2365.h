/********************************************************************\

  Name:         lrs2365.h
  Created by:   Stefan Ritt

  Cotents:      Header file for LeCroy 2365 Octal Logic Matrix

  $Log: lrs2365.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.1  1999/12/20 10:18:23  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:57  midas
  Added Log tag in header


\********************************************************************/

int lrs2365_set(int crate, int slot, WORD coeff[18]);
