/********************************************************************

  Name:         dio.c
  Created by:   Pierre Amaudruz

  Contents:     ioperm() wrapper for frontends which access CAMAC
                directly under Linux. Compile dio.c, change owner
                to root and set setuid-bit (chmod a+s dio). Then
                start the frontend with

                dio frontend

  $Log: dio.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:38  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.4  2004/01/08 08:40:11  midas
  Implemented standard indentation

  Revision 1.3  2003/04/23 23:08:57  pierre
  Fixed compiler warning

  Revision 1.2  2001/08/14 10:04:56  midas
  Use iopl() instead of ioperm() to access PCI ports > 0x3FF

  Revision 1.1  1999/12/20 09:19:11  midas
  Moved dio.c from drivers to utils direcotry

  Revision 1.2  1999/05/06 18:44:16  pierre
  - Added ioperm to port 0x80 for slow IO access (linux asm/io.h)

  Revision 1.1  1998/11/25 15:57:37  midas
  Added dio.c program as ioperm() wrapper for frontends


\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/io.h>

int main(int argc, char **argv)
{
   int status;

   /* Grant access to the device's ioports */
   status = iopl(3);
   if (status < 0) {
      perror("iopl()");
      exit(2);
   }

   /* Surrender root privileges - the exec()ed program will keep IO
      access to the IO ports (see "man 2 iopl") */
   if (setuid(getuid()) < 0) {
      perror("setuid()");
      exit(2);
   }

   /* Check command arguments */
   if (argc < 2) {
      fprintf(stderr, "Usage: %s program [arguments]\n", argv[0]);
      exit(1);
   }

   /* Execute the program (with any supplied command line args) */
   if (execvp(argv[1], &argv[1]) < 0) {
      perror(argv[1]);
      exit(2);
   }
   return 0;
}
