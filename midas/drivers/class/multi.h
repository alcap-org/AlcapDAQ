/********************************************************************\

  Name:         multi.h
  Created by:   Stefan Ritt

  Contents:     Multimeter Class Driver Header File

  $Log: multi.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:08  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.2  2000/03/02 21:54:02  midas
  Added offset in readout routines, added cmd_set_label

  Revision 1.1  1999/12/20 10:18:16  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:56  midas
  Added Log tag in header


\********************************************************************/

/* class driver routines */
INT cd_multi(INT cmd, PEQUIPMENT pequipment);
INT cd_multi_read(char *pevent, int);
