/********************************************************************\

  Name:         MIDASINC.H
  Created by:   Stefan Ritt

  Purpose:      Centralized file with all #includes
  Contents:     Includes all necessary include files

  $Log: midasinc.h,v $
  Revision 1.1.1.1  2005/06/20 23:37:15  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.12  2004/01/19 16:54:56  olchansk
  add assert.h
  add OS_DARWIN specific
  remove sys/mount.h (moved to system.c to avoid namespace pollution on macosx)

  Revision 1.11  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.10  2003/06/25 18:22:53  pierre
  Added pthread support for UNIX version - DBM

  Revision 1.9  2003/04/25 14:38:02  midas
  Fixed compiler warnings

  Revision 1.8  2003/04/25 13:54:19  midas
  Added include file for inet_ntoa

  Revision 1.7  2003/04/15 21:47:35  pierre
  include ctype.h in all OS

  Revision 1.6  2001/06/27 12:18:16  midas
  Added code for OS_IRIX

  Revision 1.5  1999/01/21 22:58:18  pierre
  - include DM_DUAL_THREAD lib as default for OS_VXWORKS

  Revision 1.4  1999/01/18 17:25:36  pierre
  - Add extra lib for dm_() for OS_VxWorks

  Revision 1.3  1998/10/28 11:06:42  midas
  Added NO_PTY for solaris and ultrix

  Revision 1.2  1998/10/12 12:19:01  midas
  Added Log tag in header


\********************************************************************/


/* OS independent files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>

/* OS dependent files */

#ifdef OS_VMS

#include <descrip.h>
#include <errno.h>
#include <in.h>
#include <inet.h>
#include <lckdef.h>
#include <lkidef.h>
#include <netdb.h>
#include <ppl$def.h>
#include <ppl$routines.h>
#include <ppldef.h>
#include <prcdef.h>
#include <socket.h>
#include <starlet.h>
#include <stdarg.h>
#include <ssdef.h>
#include <ucx$inetdef.h>
#include <file.h>
#include <stat.h>
#include <ctype.h>

#ifdef __ALPHA
#include <unixio.h>
#include <unixlib.h>
#include <lib$routines.h>
#endif

#endif

#ifdef OS_WINNT

#include <windows.h>
#include <conio.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/timeb.h>

#endif

#ifdef OS_MSDOS

#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <process.h>
#include <dos.h>
#include <alloc.h>
#include <errno.h>
#include <conio.h>
#include <ctype.h>

#endif

#ifdef OS_VXWORKS

#include <vxWorks.h>
#include <sockLib.h>
#include <selectLib.h>
#include <hostLib.h>
#include <in.h>
#include <ioLib.h>
#include <netinet/tcp.h>
#include <sys/stat.h>
/*-PAA- permanent as semphore moved in ss_mutex_...() */
#include <semLib.h>
#include <taskLib.h>
#include <symLib.h>
#include <errnoLib.h>

#endif

#ifdef OS_UNIX

#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <netdb.h>
#include <pwd.h>
#include <fcntl.h>
#include <syslog.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/termios.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/mtio.h>
#include <dirent.h>
#include <pthread.h>

/* special code for Linux and FreeBSD */
#if defined(OS_LINUX) || defined(OS_FREEBSD)
#include <sys/time.h>
#ifdef OS_DARWIN
#else
#include <sys/vfs.h>
#endif
#include <arpa/inet.h>
#include <fnmatch.h>
#ifdef OS_DARWIN
#include <util.h>
#else
#include <pty.h>
#endif
#undef LITTLE_ENDIAN
#endif

/* special code for OSF1 */
#ifdef OS_OSF1
#include <sys/time.h>
#include <sys/timers.h>

/*-PAA- Special for Ultrix */
#ifndef OS_ULTRIX
#include <fnmatch.h>
#define NO_PTY
#endif

extern void *malloc();          /* patches for wrong gcc include files */
extern void *realloc();
#endif

#ifdef OS_IRIX
#include <sys/statfs.h>
#include <fnmatch.h>
#define NO_PTY
#endif

#include <sys/wait.h>

#endif

/* special code for Solaris */
#ifdef OS_SOLARIS
#include <sys/filio.h>
#include <sys/statvfs.h>
#define NO_PTY
#endif

/*------------------------------------------------------------------*/
