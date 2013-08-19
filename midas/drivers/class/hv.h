/********************************************************************\

  Name:         hv.h
  Created by:   Stefan Ritt

  Contents:     High Voltage Class Driver

  $Log: hv.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:08  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/01/08 08:40:08  midas
  Implemented standard indentation

  Revision 1.2  2000/03/02 21:54:02  midas
  Added offset in readout routines, added cmd_set_label

  Revision 1.1  1999/12/20 10:18:16  midas
  Reorganized driver directory structure

  Revision 1.2  1998/10/12 12:18:55  midas
  Added Log tag in header


\********************************************************************/

/* class driver routines */
INT cd_hv(INT cmd, PEQUIPMENT pequipment);
INT cd_hv_read(char *pevent, int);
