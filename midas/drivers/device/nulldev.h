/********************************************************************\

  Name:         nulldev.h
  Created by:   Stefan Ritt

  Contents:     Device driver function declarations for NULL device

  $Log: nulldev.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.1  2002/03/14 13:12:17  midas
  Split null driver in bus and device drivers

  Revision 1.1  1999/12/20 10:18:20  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:58  midas
  Added Log tag in header


\********************************************************************/

INT nulldev(INT cmd, ...);
