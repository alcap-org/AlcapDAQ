/********************************************************************\

  Name:         lcwp950.c
  Created by:   Stefan Ritt

  Contents:     LeCroy WavePro 950 Digital Storage Oscilloscope 
                header file

  $Log: lcwp950.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:09  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.3  2004/07/16 08:20:34  schneebeli
  Added timeout to scope_wait()

  Revision 1.1  2002/01/14 16:49:53  midas
  Initial revisioin


\********************************************************************/

#define SCOPE_MAX_CHN   16
#define SCOPE_MEM_SIZE 500      /* # of points to read out */

#define SCOPE_PORT    1861      /* TCP/IP port of scope */

int scope_open(char *addr, int channel);
void scope_close(int fh);
void scope_arm();
int scope_wait(int fh, unsigned int timeout);
int scope_read(int fh, double x[SCOPE_MEM_SIZE], double y[SCOPE_MEM_SIZE]);
