/********************************************************************\

  Name:         system.c
  Created by:   Stefan Ritt

  Contents:     All operating system dependent system services. This
                file containt routines which hide all system specific
                behaviour to higher levels. This is done by con-
                ditional compiling using the OS_xxx variable defined
                in MIDAS.H.

                Details about interprocess communication can be
                found in "UNIX distributed programming" by Chris
                Brown, Prentice Hall

  ** muCap patches **
  - Disabled ss_tape_get_blockn() since our tape drives seem to return
    garbage when asked for the block number.

  $Log: system.c,v $
  Revision 1.2  2005/06/21 00:42:24  mucap
  (Fred)
    - Disabled ss_tape_get_blockn() since our tape drives seem to return
      garbage when asked for the block number.

  Revision 1.83  2004/10/04 07:04:01  olchansk
  improve error reporting while manipulating SysV shared memory

  Revision 1.82  2004/01/28 01:22:39  pierre
  simple ss_timezone fix

  Revision 1.81  2004/01/22 22:47:11  pierre
  correct ss_timezone for VxWorks

  Revision 1.80  2004/01/19 20:19:14  midas
  Fixed compiler warnings under Windows

  Revision 1.79  2004/01/19 17:00:24  olchansk
  change ss_thread_create() and ss_thread_kill() to use midas_thread_t
  include sys/mount.h (moved from midasinc.h to avoid namespace pollution on macosx)
  update docs for ss_thread_create()
  on darwin/macosx, abort in ss_settime()
  add ss_timezone()

  Revision 1.78  2004/01/13 00:52:05  pierre
  fix dox comment for vxworks

  Revision 1.77  2004/01/08 08:40:10  midas
  Implemented standard indentation

  Revision 1.76  2004/01/08 06:36:10  pierre
  Doxygen the file

  Revision 1.75  2003/10/16 22:53:57  midas
  Changed return values for ss_thread_create to return threadId.
  Added ss_thread_kill for killing the passed threadId. (DBM - Triumf)

  Revision 1.74  2003/07/24 11:09:50  midas
  Fixed compiler warning under Windows

  Revision 1.73  2003/06/25 18:22:53  pierre
  Added pthread support for UNIX version - DBM

  Revision 1.72  2003/06/12 18:40:59  pierre
  include ss_tape_get_blockn

  Revision 1.71  2003/05/02 09:03:01  midas
  Fixed buffer overflows by strlcpy()

  Revision 1.70  2003/04/30 06:47:44  midas
  Fixed missing variable for NT

  Revision 1.69  2003/04/25 14:03:57  midas
  Fixed compiler warnings

  Revision 1.68  2003/04/25 11:47:42  midas
  Added ss_force_single_thread()

  Revision 1.67  2003/04/21 03:16:50  olchansk
  in ss_daemon_init(), if keep_stdout, keep BOTH stdout AND stderr.
  It makes no sense to keep stdout and close stderr.
  Stefan okey'ed this change ages ago, but I never commited it, until now.

  Revision 1.66  2003/04/09 13:42:53  midas
  Made file compile under C++

  Revision 1.65  2003/03/26 21:08:44  midas
  Removed tabs

  Revision 1.64  2002/10/15 19:20:24  olchansk
  add more debugging printout to send_tcp() and recv_tcp()

  Revision 1.63  2002/06/25 19:48:18  pierre
  doc++ functions

  Revision 1.62  2002/06/25 19:00:57  pierre
  doc++ functions

  Revision 1.61  2002/06/03 06:21:59  midas
  Fixed bug with stdout

  Revision 1.60  2002/06/03 06:07:15  midas
  Added extra parameter to ss_daemon_init to keep stdout

  Revision 1.59  2002/05/28 11:30:21  midas
  Made send_tcp() send always all bytes

  Revision 1.58  2001/08/22 13:51:46  midas
  Reorganized directio functions

  Revision 1.57  2001/08/07 13:07:20  midas
  Fixed typo

  Revision 1.56  2001/06/27 12:16:00  midas
  Fixed compiler warnings for IRIX

  Revision 1.55  2001/04/23 08:25:41  midas
  Use execvp instead execve so that mserver is searched in the full PATH

  Revision 1.54  2000/11/17 08:25:46  midas
  Added disable_shm_write flag for Linux cluster applications

  Revision 1.53  2000/10/20 19:05:35  pierre
  - Added ss_exec(...) for pid return.
  - Added ss_existpid(...) for pid check.
  - ss_system uses ss_exec with dummy pid.

  Revision 1.52  2000/03/08 17:38:14  midas
  Fixed BIIIIG bug in ss_mutex_wait_for which made the system under Linux
  totally unstable since mutexed didn't work at all with this bug

  Revision 1.51  2000/02/26 00:16:05  midas
  Bypass ss_kbhit and ss_getchar after becoming a daemon

  Revision 1.50  2000/02/15 11:07:51  midas
  Changed GET_xxx to bit flags

  Revision 1.49  1999/11/09 13:19:25  midas
  Fixed bug

  Revision 1.48  1999/11/09 13:17:27  midas
  Added secure ODB feature

  Revision 1.47  1999/11/09 08:36:54  midas
  Don't use console IO under Windows 95

  Revision 1.46  1999/10/27 13:37:04  midas
  Added blank line

  Revision 1.45  1999/10/12 13:42:37  midas
  Use execl in ss_system

  Revision 1.44  1999/10/11 14:45:37  midas
  Fixed bug

  Revision 1.43  1999/10/11 14:41:10  midas
  Added code to close stdin, stdout and stderr in daemon mode

  Revision 1.42  1999/10/11 14:13:47  midas
  Added ss_system which executs a program in a seperate process and closes
  all file descriptors from the parent. This is needed my mhttpd for example
  to launch programs, which should no block on the port 80 socket.

  Revision 1.41  1999/09/17 11:48:08  midas
  Alarm system half finished

  Revision 1.40  1999/09/15 08:57:10  midas
  Fixed bug im send_tcp

  Revision 1.39  1999/09/15 08:05:51  midas
  Ctrl-C handle can now be turned off by using SIG_DFL

  Revision 1.38  1999/08/26 08:50:56  midas
  Added missing status check on select() funciton

  Revision 1.37  1999/07/22 19:08:25  pierre
  - move static INT ss_in_async_routine_flag above LOCAL_ROUTINE for VxWorks

  Revision 1.36  1999/07/15 07:42:48  midas
  Added SIGTERM to ss_ctrlc_handler

  Revision 1.35  1999/07/15 07:35:00  midas
  Added ss_ctrlc_handler

  Revision 1.34  1999/07/08 13:41:37  midas
  Fixed followup error in ss_tape_read

  Revision 1.33  1999/07/06 09:02:20  midas
  Evaluater EINTR in ss_release_mutex

  Revision 1.32  1999/06/23 13:37:26  midas
  Fixed compiler warnings

  Revision 1.31  1999/06/23 09:42:28  midas
  - Changed all NT comments
  - Added ss_settime for VxWrorks

  Revision 1.30  1999/06/02 07:51:08  midas
  Fixed compiler error under RH6.0 with semun structure

  Revision 1.29  1999/05/03 10:40:41  midas
  Fixed segmentation violation on program close under Unix, when .ODB.SHM
  has been created under root, and accessed under normal user

  Revision 1.28  1999/04/30 13:19:55  midas
  Changed inter-process communication (ss_resume, bm_notify_clients, etc)
  to strings so that server process can receive it's own watchdog produced
  messages (pass buffer name insteas buffer handle)

  Revision 1.27  1999/04/30 10:58:59  midas
  Added -D debug to screen for mserver

  Revision 1.26  1999/04/29 10:48:03  midas
  Implemented "/System/Client Notify" key

  Revision 1.25  1999/04/29 07:06:40  midas
  Removed superfluous "{"

  Revision 1.24  1999/04/16 15:11:28  midas
  ss_suspends forwards more MSG_BM UPD messages (needed for JMidas seeing
  its own messages in parallel to an ODB key update like run start)

  Revision 1.23  1999/02/06 00:10:35  pierre
  -Fix timeout unit for ss_mutex_wait_for under VxWorks

  Revision 1.22  1999/02/01 11:53:45  midas
  Changed comment

  Revision 1.21  1999/01/22 09:48:16  midas
  Added return statements for MSDOS in mutex routines

  Revision 1.20  1999/01/21 23:14:05  pierre
  - Fix comments.
  - Incorporate ss_mutex_... for OS_VXWORS.
  - Rename ss_create_mutex() to ss_thread_create()
  - Incorporate ss_thread_create for OS_VXWORS.

  Revision 1.19  1999/01/20 08:55:44  midas
  - Renames ss_xxx_mutex to ss_mutex_xxx
  - Added timout flag to ss_mutex_wait_for

  Revision 1.18  1999/01/18 17:48:42  pierre
  - Correct ss_file_find().

  Revision 1.17  1998/12/10 12:47:51  midas
  Changed unexpexted tape error printf to cm_msg

  Revision 1.16  1998/12/10 10:54:52  midas
  Forwarded ss_tape_status to 'mt' under UNIX

  Revision 1.15  1998/12/10 10:45:43  midas
  Improved tape error codes under NT (now same as UNIX)

  Revision 1.14  1998/12/10 10:17:34  midas
  Improved tape status return values under UNIX

  Revision 1.13  1998/11/25 23:44:24  midas
  Removed O_RDWR... in ss_tape_open()

  Revision 1.12  1998/10/29 15:56:40  midas
  Printout note about unimplemented tape status command

  Revision 1.11  1998/10/29 15:53:52  midas
  Added end-of-tape detection under UNIX

  Revision 1.10  1998/10/29 09:37:37  midas
  Reordered IPC and server checks in ss_suspend() to avoid timeouts when
  analyze is running under full load

  Revision 1.9  1998/10/28 12:02:52  midas
  Added NO_PTY to ss_shell()

  Revision 1.8  1998/10/28 11:07:45  midas
  Added ss_shell for UNIX

  Revision 1.7  1998/10/27 10:53:48  midas
  - Added run start notification
  - Added ss_shell() for NT

  Revision 1.6  1998/10/22 12:40:34  midas
  Added "oflag" to ss_tape_open()

  Revision 1.5  1998/10/13 06:52:57  midas
  Fixed error occuring when .ODB.SHM had zero size

  Revision 1.4  1998/10/12 14:00:19  midas
  ss_shm_open returns memory size in case of failure

  Revision 1.3  1998/10/12 12:19:03  midas
  Added Log tag in header

  Revision 1.2  1998/10/12 11:59:11  midas
  Added Log tag in header

  Previous Revision history
  ------------------------------------------------------------------
  date        by    modification
  ---------   ---   ------------------------------------------------
  02-NOV-93   SR    created
  02-OCT-94   SR    unix semaphores added
  29-SEP-96   SR    tape handling added
  11-OCT-96   SR    screen functions added
  06-MAY-97   SR    removed .MTX files
  06-MAY-97   SR    added ss_syslog routine
  02-JUN-97   SR    adapted ss_syslog for ULTRIX
  15-MAY-98   PAA   replaces ftime by gettimeofday

\********************************************************************/

/**dox***************************************************************/
/** @file system.c
The Midas System file
*/

/** @defgroup msfunctionc  System Functions (ss_xxx)
 */

/**dox***************************************************************/
/** @addtogroup msystemincludecode
 *  
 *  @{  */

/**dox***************************************************************/
/** @addtogroup msfunctionc
 *  
 *  @{  */

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include "midas.h"
#include "msystem.h"
#include "strlcpy.h"

#ifdef OS_UNIX
#include <sys/mount.h>
#endif

static INT ss_in_async_routine_flag = 0;
#ifdef LOCAL_ROUTINES
#include <signal.h>

/*------------------------------------------------------------------*/
/* globals */

/* if set, don't write to *SHM file (used for Linux cluster) */
BOOL disable_shm_write = FALSE;

/*------------------------------------------------------------------*/
INT ss_set_async_flag(INT flag)
/********************************************************************\

  Routine: ss_set_async_flag

  Purpose: Sets the ss_in_async_routine_flag according to the flag
     value. This is necessary when semaphore operations under
     UNIX are called inside an asynchrounous routine (via alarm)
     because they then behave different.

  Input:
    INT  flag               May be 1 or 0

  Output:
    none

  Function value:
    INT                     Previous value of the flag

\********************************************************************/
{
   INT old_flag;

   old_flag = ss_in_async_routine_flag;
   ss_in_async_routine_flag = flag;
   return old_flag;
}

/*------------------------------------------------------------------*/
INT ss_shm_open(char *name, INT size, void **adr, HNDLE * handle)
/********************************************************************\

  Routine: ss_shm_open

  Purpose: Create a shared memory region which can be seen by several
     processes which know the name.

  Input:
    char *name              Name of the shared memory
    INT  size               Initial size of the shared memory in bytes
                            if .SHM file doesn't exist

  Output:
    void  *adr              Address of opened shared memory
    HNDLE handle            Handle or key to the shared memory

  Function value:
    SS_SUCCESS              Successful completion
    SS_CREATED              Shared memory was created
    SS_FILE_ERROR           Paging file cannot be created
    SS_NO_MEMORY            Not enough memory

\********************************************************************/
{
   INT status;
   char mem_name[256], file_name[256], path[256];

   /* Add a leading SM_ to the memory name */
   sprintf(mem_name, "SM_%s", name);

   /* Build the filename out of the path, and the name of the shared memory. */
   cm_get_path(path);
   if (path[0] == 0) {
      getcwd(path, 256);
#if defined(OS_VMS)
#elif defined(OS_UNIX)
      strcat(path, "/");
#elif defined(OS_WINNT)
      strcat(path, "\\");
#endif
   }

   strcpy(file_name, path);
#if defined (OS_UNIX)
   strcat(file_name, ".");      /* dot file under UNIX */
#endif
   strcat(file_name, name);
   strcat(file_name, ".SHM");

#ifdef OS_WINNT

   status = SS_SUCCESS;

   {
      HANDLE hFile, hMap;
      char str[256], *p;
      DWORD file_size;

      /* make the memory name unique using the pathname. This is necessary
         because NT doesn't use ftok. So if different experiments are
         running in different directories, they should not see the same
         shared memory */
      strcpy(str, path);

      /* replace special chars by '*' */
      while (strpbrk(str, "\\: "))
         *strpbrk(str, "\\: ") = '*';
      strcat(str, mem_name);

      /* convert to uppercase */
      p = str;
      while (*p)
         *p++ = (char) toupper(*p);

      hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, str);
      if (hMap == 0) {
         hFile = CreateFile(file_name, GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
         if (!hFile) {
            cm_msg(MERROR, "ss_shm_open", "CreateFile() failed");
            return SS_FILE_ERROR;
         }

         file_size = GetFileSize(hFile, NULL);
         if (file_size != 0xFFFFFFFF && file_size > 0)
            size = file_size;

         hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, size, str);

         if (!hMap) {
            status = GetLastError();
            cm_msg(MERROR, "ss_shm_open", "CreateFileMapping() failed, error %d", status);
            return SS_FILE_ERROR;
         }

         CloseHandle(hFile);
         status = SS_CREATED;
      }

      *adr = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
      *handle = (HNDLE) hMap;

      if (adr == NULL) {
         cm_msg(MERROR, "ss_shm_open", "MapViewOfFile() failed");
         return SS_NO_MEMORY;
      }

      return status;
   }

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   status = SS_SUCCESS;

   {
      int addr[2];
      $DESCRIPTOR(memname_dsc, "dummy");
      $DESCRIPTOR(filename_dsc, "dummy");
      memname_dsc.dsc$w_length = strlen(mem_name);
      memname_dsc.dsc$a_pointer = mem_name;
      filename_dsc.dsc$w_length = strlen(file_name);
      filename_dsc.dsc$a_pointer = file_name;

      addr[0] = size;
      addr[1] = 0;

      status = ppl$create_shared_memory(&memname_dsc, addr, &PPL$M_NOUNI, &filename_dsc);

      if (status == PPL$_CREATED)
         status = SS_CREATED;
      else if (status != PPL$_NORMAL)
         status = SS_FILE_ERROR;

      *adr = (void *) addr[1];
      *handle = 0;              /* not used under VMS */

      if (adr == NULL)
         return SS_NO_MEMORY;

      return status;
   }

#endif                          /* OS_VMS */
#ifdef OS_UNIX

   status = SS_SUCCESS;

   {
      int key, shmid, fh, file_size;
      struct shmid_ds buf;

      /* create a unique key from the file name */
      key = ftok(file_name, 'M');

      /* if file doesn't exist, create it */
      if (key == -1) {
         fh = open(file_name, O_CREAT | O_TRUNC | O_BINARY, 0644);
         close(fh);
         key = ftok(file_name, 'M');

         if (key == -1) {
            cm_msg(MERROR, "ss_shm_open", "ftok() failed");
            return SS_FILE_ERROR;
         }

         status = SS_CREATED;

         /* delete any previously created memory */

         shmid = shmget(key, 0, 0);
         shmctl(shmid, IPC_RMID, &buf);
      } else {
         /* if file exists, retrieve its size */
         file_size = (INT) ss_file_size(file_name);
         if (file_size > 0)
            size = file_size;
      }

      /* get the shared memory, create if not existing */
      shmid = shmget(key, size, 0);
      if (shmid == -1) {
         //cm_msg(MINFO, "ss_shm_open", "Creating shared memory segment, key: 0x%x, size: %d",key,size);
         shmid = shmget(key, size, IPC_CREAT|IPC_EXCL);
         if (shmid == -1 && errno == EEXIST)
           {
             cm_msg(MERROR, "ss_shm_open", "Shared memory segment with key 0x%x already exists, please remove it manually: ipcrm -M 0x%x",key,key);
             return SS_NO_MEMORY;
           }
         status = SS_CREATED;
      }

      if (shmid == -1) {
         cm_msg(MERROR, "ss_shm_open", "shmget(key=0x%x,size=%d) failed, errno %d (%s)", key, size, errno, strerror(errno));
         return SS_NO_MEMORY;
      }

      buf.shm_perm.uid = getuid();
      buf.shm_perm.gid = getgid();
      buf.shm_perm.mode = 0666;
      shmctl(shmid, IPC_SET, &buf);

      *adr = shmat(shmid, 0, 0);
      *handle = (HNDLE) shmid;

      if ((*adr) == (void *) (-1)) {
         cm_msg(MERROR, "ss_shm_open","shmat(shmid=%d) failed, errno %d (%s)", shmid, errno, strerror(errno));
         return SS_NO_MEMORY;
      }

      /* if shared memory was created, try to load it from file */
      if (status == SS_CREATED) {
         fh = open(file_name, O_RDONLY, 0644);
         if (fh == -1)
            fh = open(file_name, O_CREAT | O_RDWR, 0644);
         else
            read(fh, *adr, size);
         close(fh);
      }

      return status;
   }

#endif                          /* OS_UNIX */
}

/*------------------------------------------------------------------*/
INT ss_shm_close(char *name, void *adr, HNDLE handle, INT destroy_flag)
/********************************************************************\

  Routine: ss_shm_close

  Purpose: Close a shared memory region.

  Input:
    char *name              Name of the shared memory
    void *adr               Base address of shared memory
    HNDLE handle            Handle of shared memeory
    BOOL destroy            Shared memory has to be destroyd and
          flushed to the mapping file.

  Output:
    none

  Function value:
    SS_SUCCESS              Successful completion
    SS_INVALID_ADDRESS      Invalid base address
    SS_FILE_ERROR           Cannot write shared memory file
    SS_INVALID_HANDLE       Invalid shared memory handle

\********************************************************************/
{
   char mem_name[256], file_name[256], path[256];

   /*
      append a leading SM_ to the memory name to resolve name conflicts
      with mutex or semaphore names
    */
   sprintf(mem_name, "SM_%s", name);

   /* append .SHM and preceed the path for the shared memory file name */
   cm_get_path(path);
   if (path[0] == 0) {
      getcwd(path, 256);
#if defined(OS_VMS)
#elif defined(OS_UNIX)
      strcat(path, "/");
#elif defined(OS_WINNT)
      strcat(path, "\\");
#endif
   }

   strcpy(file_name, path);
#if defined (OS_UNIX)
   strcat(file_name, ".");      /* dot file under UNIX */
#endif
   strcat(file_name, name);
   strcat(file_name, ".SHM");

#ifdef OS_WINNT

   if (!UnmapViewOfFile(adr))
      return SS_INVALID_ADDRESS;

   CloseHandle((HANDLE) handle);

   return SS_SUCCESS;

#endif                          /* OS_WINNT */
#ifdef OS_VMS
/* outcommented because ppl$delete... makes privilege violation
  {
  int addr[2], flags, status;
  char mem_name[100];
  $DESCRIPTOR(memname_dsc, mem_name);

  strcpy(mem_name, "SM_");
  strcat(mem_name, name);
  memname_dsc.dsc$w_length = strlen(mem_name);

  flags = PPL$M_FLUSH | PPL$M_NOUNI;

  addr[0] = 0;
  addr[1] = adr;

  status = ppl$delete_shared_memory( &memname_dsc, addr, &flags);

  if (status == PPL$_NORMAL)
    return SS_SUCCESS;

  return SS_INVALID_ADDRESS;
  }
*/
   return SS_INVALID_ADDRESS;

#endif                          /* OS_VMS */
#ifdef OS_UNIX

   {
      struct shmid_ds buf;
      FILE *fh;

      /* get info about shared memory */
      if (shmctl(handle, IPC_STAT, &buf) < 0) {
         cm_msg(MERROR, "ss_shm_close", "shmctl(shmid=%d,IPC_STAT) failed, errno %d (%s)",handle,errno,strerror(errno));
         return SS_INVALID_HANDLE;
      }

      /* copy to file and destroy if we are the last one */
      if (buf.shm_nattch == 1) {
         if (!disable_shm_write) {
            fh = fopen(file_name, "w");

            if (fh == NULL) {
               cm_msg(MERROR, "ss_shm_close",
                      "Cannot write to file %s, please check protection", file_name);
            } else {
               /* write shared memory to file */
               fwrite(adr, 1, buf.shm_segsz, fh);
               fclose(fh);
            }
         }

         if (shmdt(adr) < 0) {
            cm_msg(MERROR, "ss_shm_close", "shmdt(shmid=%d) failed, errno %d (%s)",handle,errno,strerror(errno));
            return SS_INVALID_ADDRESS;
         }

         if (shmctl(handle, IPC_RMID, &buf) < 0) {
            cm_msg(MERROR, "ss_shm_close", "shmctl(shmid=%d,IPC_RMID) failed, errno %d (%s)",handle,errno,strerror(errno));
            return SS_INVALID_ADDRESS;
         }
      } else
         /* only detach if we are not the last */
      if (shmdt(adr) < 0) {
         cm_msg(MERROR, "ss_shm_close", "shmdt(shmid=%d) failed, errno %d (%s)",handle,errno,strerror(errno));
         return SS_INVALID_ADDRESS;
      }

      return SS_SUCCESS;
   }

#endif                          /* OS_UNIX */
}

/*------------------------------------------------------------------*/
INT ss_shm_protect(HNDLE handle, void *adr)
/********************************************************************\

  Routine: ss_shm_protect

  Purpose: Protect a shared memory region, disallow read and write
           access to it by this process

  Input:
    HNDLE handle            Handle of shared memeory
    void  *adr              Address of shared memory

  Output:
    none

  Function value:
    SS_SUCCESS              Successful completion
    SS_INVALID_ADDRESS      Invalid base address

\********************************************************************/
{
#ifdef OS_WINNT

   if (!UnmapViewOfFile(adr))
      return SS_INVALID_ADDRESS;

#endif                          /* OS_WINNT */
#ifdef OS_UNIX

   if (shmdt(adr) < 0) {
      cm_msg(MERROR, "ss_shm_protect", "shmdt() failed");
      return SS_INVALID_ADDRESS;
   }
#endif                          /* OS_UNIX */
   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_shm_unprotect(HNDLE handle, void **adr)
/********************************************************************\

  Routine: ss_shm_unprotect

  Purpose: Unprotect a shared memory region so that it can be accessed
           by this process

  Input:
    HNDLE handle            Handle or key to the shared memory, must
                            be obtained with ss_shm_open

  Output:
    void  *adr              Address of opened shared memory

  Function value:
    SS_SUCCESS              Successful completion
    SS_NO_MEMORY            Memory mapping failed

\********************************************************************/
{
#ifdef OS_WINNT

   *adr = MapViewOfFile((HANDLE) handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

   if (*adr == NULL) {
      cm_msg(MERROR, "ss_shm_unprotect", "MapViewOfFile() failed");
      return SS_NO_MEMORY;
   }
#endif                          /* OS_WINNT */
#ifdef OS_UNIX

   *adr = shmat(handle, 0, 0);

   if ((*adr) == (void *) (-1)) {
      cm_msg(MERROR, "ss_shm_unprotect", "shmat() failed, errno = %d", errno);
      return SS_NO_MEMORY;
   }
#endif                          /* OS_UNIX */

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_shm_flush(char *name, void *adr, INT size)
/********************************************************************\

  Routine: ss_shm_flush

  Purpose: Flush a shared memory region to its disk file.

  Input:
    char *name              Name of the shared memory
    void *adr               Base address of shared memory
    INT  size               Size of shared memeory

  Output:
    none

  Function value:
    SS_SUCCESS              Successful completion
    SS_INVALID_ADDRESS      Invalid base address

\********************************************************************/
{
   char mem_name[256], file_name[256], path[256];

   /*
      append a leading SM_ to the memory name to resolve name conflicts
      with mutex or semaphore names
    */
   sprintf(mem_name, "SM_%s", name);

   /* append .SHM and preceed the path for the shared memory file name */
   cm_get_path(path);
   if (path[0] == 0) {
      getcwd(path, 256);
#if defined(OS_VMS)
#elif defined(OS_UNIX)
      strcat(path, "/");
#elif defined(OS_WINNT)
      strcat(path, "\\");
#endif
   }

   strcpy(file_name, path);
#if defined (OS_UNIX)
   strcat(file_name, ".");      /* dot file under UNIX */
#endif
   strcat(file_name, name);
   strcat(file_name, ".SHM");

#ifdef OS_WINNT

   if (!FlushViewOfFile(adr, size))
      return SS_INVALID_ADDRESS;

   return SS_SUCCESS;

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   return SS_SUCCESS;

#endif                          /* OS_VMS */
#ifdef OS_UNIX

   if (!disable_shm_write) {
      FILE *fh;

      fh = fopen(file_name, "w");

      if (fh == NULL) {
         cm_msg(MERROR, "ss_shm_flush",
                "Cannot write to file %s, please check protection", file_name);
      } else {
         /* write shared memory to file */
         fwrite(adr, 1, size, fh);
         fclose(fh);
      }
   }
   return SS_SUCCESS;

#endif                          /* OS_UNIX */
}

/*------------------------------------------------------------------*/
INT ss_getthandle(void)
/********************************************************************\

  Routine: ss_getthandle

  Purpose: Return thread handle of current thread

  Input:
    none

  Output:
    none

  Function value:
    INT thread handle

\********************************************************************/
{
#ifdef OS_WINNT
   HANDLE hThread;

   /*
      Under Windows NT, it's a little bit tricky to get a thread handle
      which can be passed to other processes. First the calling process
      has to duplicate the GetCurrentThread() handle, then the process
      which gets this handle also has to use DuplicateHandle with the
      target process as its own. Then this duplicated handle can be used
      to the SupendThread() and ResumeThread() functions.
    */

   DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
                   GetCurrentProcess(), &hThread, THREAD_ALL_ACCESS, TRUE, 0);

   return (INT)hThread;

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   return 0;

#endif                          /* OS_VMS */
#ifdef OS_UNIX

   return ss_getpid();

#endif                          /* OS_VMS */
}

#endif                          /* LOCAL_ROUTINES */

/*------------------------------------------------------------------*/
struct {
   char c;
   double d;
} test_align;

INT ss_get_struct_align()
/********************************************************************\

  Routine: ss_get_struct_align

  Purpose: Returns compiler alignment of structures. In C, structures
     can be byte aligned, word or even quadword aligned. This
     can usually be set with compiler switches. This routine
     tests this alignment during runtime and returns 1 for
     byte alignment, 2 for word alignment, 4 for dword alignment
     and 8 for quadword alignment.

  Input:
    <none>

  Output:
    <none>

  Function value:
    INT    Structure alignment

\********************************************************************/
{
   return (PTYPE) (&test_align.d) - (PTYPE) & test_align.c;
}

/*------------------------------------------------------------------*/
INT ss_getpid(void)
/********************************************************************\

  Routine: ss_getpid

  Purpose: Return process ID of current process

  Input:
    none

  Output:
    none

  Function value:
    INT              Process ID

\********************************************************************/
{
#ifdef OS_WINNT

   return (int) GetCurrentProcessId();

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   return getpid();

#endif                          /* OS_VMS */
#ifdef OS_UNIX

   return getpid();

#endif                          /* OS_UNIX */
#ifdef OS_VXWORKS

   return 0;

#endif                          /* OS_VXWORKS */
#ifdef OS_MSDOS

   return 0;

#endif                          /* OS_MSDOS */
}

/*------------------------------------------------------------------*/

static BOOL _single_thread = FALSE;

void ss_force_single_thread()
{
   _single_thread = TRUE;
}

INT ss_gettid(void)
/********************************************************************\

  Routine: ss_ggettid

  Purpose: Return thread ID of current thread

  Input:
    none

  Output:
    none

  Function value:
    INT              thread ID

\********************************************************************/
{
   /* if forced to single thread mode, simply return fake TID */
   if (_single_thread)
      return 1;

#ifdef OS_MSDOS

   return 0;

#endif                          /* OS_MSDOS */
#ifdef OS_WINNT

   return (int) GetCurrentThreadId();

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   return ss_getpid();

#endif                          /* OS_VMS */
#ifdef OS_UNIX

   return ss_getpid();

#endif                          /* OS_UNIX */
#ifdef OS_VXWORKS

   return ss_getpid();

#endif                          /* OS_VXWORKS */
}

/*------------------------------------------------------------------*/

#ifdef OS_UNIX
void catch_sigchld(int signo)
{
   int status;

   wait(&status);
   return;
}
#endif

INT ss_spawnv(INT mode, char *cmdname, char *argv[])
/********************************************************************\

  Routine: ss_spawnv

  Purpose: Spawn a subprocess or detached process

  Input:
    INT mode         One of the following modes:
           P_WAIT     Wait for the subprocess to compl.
           P_NOWAIT   Don't wait for subprocess to compl.
           P_DETACH   Create detached process.
    char cmdname     Program name to execute
    char *argv[]     Optional program arguments

  Output:
    none

  Function value:
    SS_SUCCESS       Successful completeion
    SS_INVALID_NAME  Command could not be executed;

\********************************************************************/
{
#ifdef OS_WINNT

   if (spawnvp(mode, cmdname, argv) < 0)
      return SS_INVALID_NAME;

   return SS_SUCCESS;

#endif                          /* OS_WINNT */

#ifdef OS_MSDOS

   spawnvp((int) mode, cmdname, argv);

   return SS_SUCCESS;

#endif                          /* OS_MSDOS */

#ifdef OS_VMS

   {
      char cmdstring[500], *pc;
      INT i, flags, status;
      va_list argptr;

      $DESCRIPTOR(cmdstring_dsc, "dummy");

      if (mode & P_DETACH) {
         cmdstring_dsc.dsc$w_length = strlen(cmdstring);
         cmdstring_dsc.dsc$a_pointer = cmdstring;

         status = sys$creprc(0, &cmdstring_dsc,
                             0, 0, 0, 0, 0, NULL, 4, 0, 0, PRC$M_DETACH);
      } else {
         flags = (mode & P_NOWAIT) ? 1 : 0;

         for (pc = argv[0] + strlen(argv[0]); *pc != ']' && pc != argv[0]; pc--);
         if (*pc == ']')
            pc++;

         strcpy(cmdstring, pc);

         if (strchr(cmdstring, ';'))
            *strchr(cmdstring, ';') = 0;

         strcat(cmdstring, " ");

         for (i = 1; argv[i] != NULL; i++) {
            strcat(cmdstring, argv[i]);
            strcat(cmdstring, " ");
         }

         cmdstring_dsc.dsc$w_length = strlen(cmdstring);
         cmdstring_dsc.dsc$a_pointer = cmdstring;

         status = lib$spawn(&cmdstring_dsc, 0, 0, &flags, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
      }

      return BM_SUCCESS;
   }

#endif                          /* OS_VMS */
#ifdef OS_UNIX
   pid_t child_pid;

#ifdef OS_ULTRIX
   union wait *status;
#else
   int status;
#endif

   if ((child_pid = fork()) < 0)
      return (-1);

   if (child_pid == 0) {
      /* now we are in the child process ... */
      child_pid = execvp(cmdname, argv);
      return SS_SUCCESS;
   } else {
      /* still in parent process */
      if (mode == P_WAIT)
#ifdef OS_ULTRIX
         waitpid(child_pid, status, WNOHANG);
#else
         waitpid(child_pid, &status, WNOHANG);
#endif

      else
         /* catch SIGCHLD signal to avoid <defunc> processes */
         signal(SIGCHLD, catch_sigchld);
   }

   return SS_SUCCESS;

#endif                          /* OS_UNIX */
}

/*------------------------------------------------------------------*/
INT ss_shell(int sock)
/********************************************************************\

  Routine: ss_shell

  Purpose: Execute shell via socket (like telnetd)

  Input:
    int  sock        Socket

  Output:
    none

  Function value:
    SS_SUCCESS       Successful completeion

\********************************************************************/
{
#ifdef OS_WINNT

   HANDLE hChildStdinRd, hChildStdinWr, hChildStdinWrDup,
       hChildStdoutRd, hChildStdoutWr,
       hChildStderrRd, hChildStderrWr, hSaveStdin, hSaveStdout, hSaveStderr;

   SECURITY_ATTRIBUTES saAttr;
   PROCESS_INFORMATION piProcInfo;
   STARTUPINFO siStartInfo;
   char buffer[256], cmd[256];
   DWORD dwRead, dwWritten, dwAvail, i, i_cmd;
   fd_set readfds;
   struct timeval timeout;

   /* Set the bInheritHandle flag so pipe handles are inherited. */
   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
   saAttr.bInheritHandle = TRUE;
   saAttr.lpSecurityDescriptor = NULL;

   /* Save the handle to the current STDOUT. */
   hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE);

   /* Create a pipe for the child's STDOUT. */
   if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
      return 0;

   /* Set a write handle to the pipe to be STDOUT. */
   if (!SetStdHandle(STD_OUTPUT_HANDLE, hChildStdoutWr))
      return 0;


   /* Save the handle to the current STDERR. */
   hSaveStderr = GetStdHandle(STD_ERROR_HANDLE);

   /* Create a pipe for the child's STDERR. */
   if (!CreatePipe(&hChildStderrRd, &hChildStderrWr, &saAttr, 0))
      return 0;

   /* Set a read handle to the pipe to be STDERR. */
   if (!SetStdHandle(STD_ERROR_HANDLE, hChildStderrWr))
      return 0;


   /* Save the handle to the current STDIN. */
   hSaveStdin = GetStdHandle(STD_INPUT_HANDLE);

   /* Create a pipe for the child's STDIN. */
   if (!CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0))
      return 0;

   /* Set a read handle to the pipe to be STDIN. */
   if (!SetStdHandle(STD_INPUT_HANDLE, hChildStdinRd))
      return 0;

   /* Duplicate the write handle to the pipe so it is not inherited. */
   if (!DuplicateHandle(GetCurrentProcess(), hChildStdinWr, GetCurrentProcess(), &hChildStdinWrDup, 0, FALSE,   /* not inherited */
                        DUPLICATE_SAME_ACCESS))
      return 0;

   CloseHandle(hChildStdinWr);

   /* Now create the child process. */
   memset(&siStartInfo, 0, sizeof(siStartInfo));
   siStartInfo.cb = sizeof(STARTUPINFO);
   siStartInfo.lpReserved = NULL;
   siStartInfo.lpReserved2 = NULL;
   siStartInfo.cbReserved2 = 0;
   siStartInfo.lpDesktop = NULL;
   siStartInfo.dwFlags = 0;

   if (!CreateProcess(NULL, "cmd /Q",   /* command line */
                      NULL,     /* process security attributes */
                      NULL,     /* primary thread security attributes */
                      TRUE,     /* handles are inherited */
                      0,        /* creation flags */
                      NULL,     /* use parent's environment */
                      NULL,     /* use parent's current directory */
                      &siStartInfo,     /* STARTUPINFO pointer */
                      &piProcInfo))     /* receives PROCESS_INFORMATION */
      return 0;

   /* After process creation, restore the saved STDIN and STDOUT. */
   SetStdHandle(STD_INPUT_HANDLE, hSaveStdin);
   SetStdHandle(STD_OUTPUT_HANDLE, hSaveStdout);
   SetStdHandle(STD_ERROR_HANDLE, hSaveStderr);

   i_cmd = 0;

   do {
      /* query stderr */
      do {
         if (!PeekNamedPipe(hChildStderrRd, buffer, 256, &dwRead, &dwAvail, NULL))
            break;

         if (dwRead > 0) {
            ReadFile(hChildStderrRd, buffer, 256, &dwRead, NULL);
            send(sock, buffer, dwRead, 0);
         }
      } while (dwAvail > 0);

      /* query stdout */
      do {
         if (!PeekNamedPipe(hChildStdoutRd, buffer, 256, &dwRead, &dwAvail, NULL))
            break;
         if (dwRead > 0) {
            ReadFile(hChildStdoutRd, buffer, 256, &dwRead, NULL);
            send(sock, buffer, dwRead, 0);
         }
      } while (dwAvail > 0);


      /* check if subprocess still alive */
      if (!GetExitCodeProcess(piProcInfo.hProcess, &i))
         break;
      if (i != STILL_ACTIVE)
         break;

      /* query network socket */
      FD_ZERO(&readfds);
      FD_SET(sock, &readfds);
      timeout.tv_sec = 0;
      timeout.tv_usec = 100;
      select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

      if (FD_ISSET(sock, &readfds)) {
         i = recv(sock, cmd + i_cmd, 1, 0);
         if (i <= 0)
            break;

         /* backspace */
         if (cmd[i_cmd] == 8) {
            if (i_cmd > 0) {
               send(sock, "\b \b", 3, 0);
               i_cmd -= 1;
            }
         } else if (cmd[i_cmd] >= ' ' || cmd[i_cmd] == 13 || cmd[i_cmd] == 10) {
            send(sock, cmd + i_cmd, 1, 0);
            i_cmd += i;
         }
      }

      /* linefeed triggers new command */
      if (cmd[i_cmd - 1] == 10) {
         WriteFile(hChildStdinWrDup, cmd, i_cmd, &dwWritten, NULL);
         i_cmd = 0;
      }

   } while (TRUE);

   CloseHandle(hChildStdinWrDup);
   CloseHandle(hChildStdinRd);
   CloseHandle(hChildStderrRd);
   CloseHandle(hChildStdoutRd);

   return SS_SUCCESS;

#endif                          /* OS_WINNT */

#ifdef OS_UNIX
#ifndef NO_PTY
   pid_t pid;
   int i, pipe;
   char line[32], buffer[1024], shell[32];
   fd_set readfds;

   if ((pid = forkpty(&pipe, line, NULL, NULL)) < 0)
      return 0;
   else if (pid > 0) {
      /* parent process */

      do {
         FD_ZERO(&readfds);
         FD_SET(sock, &readfds);
         FD_SET(pipe, &readfds);

         select(FD_SETSIZE, (void *) &readfds, NULL, NULL, NULL);

         if (FD_ISSET(sock, &readfds)) {
            memset(buffer, 0, sizeof(buffer));
            i = recv(sock, buffer, sizeof(buffer), 0);
            if (i <= 0)
               break;
            if (write(pipe, buffer, i) != i)
               break;
         }

         if (FD_ISSET(pipe, &readfds)) {
            memset(buffer, 0, sizeof(buffer));
            i = read(pipe, buffer, sizeof(buffer));
            if (i <= 0)
               break;
            send(sock, buffer, i, 0);
         }

      } while (1);
   } else {
      /* child process */

      if (getenv("SHELL"))
         strlcpy(shell, getenv("SHELL"), sizeof(shell));
      else
         strcpy(shell, "/bin/sh");
      execl(shell, shell, 0);
   }
#else
   send(sock, "not implemented\n", 17, 0);
#endif                          /* NO_PTY */

   return SS_SUCCESS;

#endif                          /* OS_UNIX */
}

/*------------------------------------------------------------------*/
static BOOL _daemon_flag;

INT ss_daemon_init(BOOL keep_stdout)
/********************************************************************\

  Routine: ss_daemon_init

  Purpose: Become a daemon

  Input:
    none

  Output:
    none

  Function value:
    SS_SUCCESS       Successful completeion
    SS_ABORT         fork() was not successful, or other problem

\********************************************************************/
{
#ifdef OS_UNIX

   /* only implemented for UNIX */
   int i, fd, pid;

   if ((pid = fork()) < 0)
      return SS_ABORT;
   else if (pid != 0)
      exit(0);                  /* parent finished */

   /* child continues here */

   _daemon_flag = TRUE;

   /* try and use up stdin, stdout and stderr, so other
      routines writing to stdout etc won't cause havoc. Copied from smbd */
   for (i = 0; i < 3; i++) {
      if (keep_stdout && ((i == 1) || (i == 2)))
         continue;

      close(i);
      fd = open("/dev/null", O_RDWR, 0);
      if (fd < 0)
         fd = open("/dev/null", O_WRONLY, 0);
      if (fd < 0) {
         cm_msg(MERROR, "ss_system", "Can't open /dev/null");
         return SS_ABORT;
      }
      if (fd != i) {
         cm_msg(MERROR, "ss_system", "Did not get file descriptor");
         return SS_ABORT;
      }
   }

   setsid();                    /* become session leader */
   chdir("/");                  /* change working direcotry (not on NFS!) */
   umask(0);                    /* clear our file mode createion mask */

#endif

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
BOOL ss_existpid(INT pid)
/********************************************************************\

  Routine: ss_existpid

  Purpose: Execute a Kill sig=0 which return success if pid found.

  Input:
    pid  : pid to check

  Output:
    none

  Function value:
    TRUE      PID found
    FALSE     PID not found

\********************************************************************/
{
#ifdef OS_UNIX
   /* only implemented for UNIX */
   return (kill(pid, 0) == 0 ? TRUE : FALSE);
#else
   cm_msg(MINFO, "ss_existpid", "implemented for UNIX only");
   return FALSE;
#endif
}


/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/********************************************************************/
/**
Execute command in a separate process, close all open file descriptors
invoke ss_exec() and ignore pid.
\code
{ ...
  char cmd[256];
  sprintf(cmd,"%s %s %i %s/%s %1.3lf %d",lazy.commandAfter,
     lazy.backlabel, lazyst.nfiles, lazy.path, lazyst.backfile,
     lazyst.file_size/1024.0/1024.0, blockn);
  cm_msg(MINFO,"Lazy","Exec post file write script:%s",cmd);
  ss_system(cmd);
}
...
\encode
@param command Command to execute.
@return SS_SUCCESS or ss_exec() return code
*/
INT ss_system(char *command)
{
#ifdef OS_UNIX
   INT childpid;

   return ss_exec(command, &childpid);

#else

   system(command);
   return SS_SUCCESS;

#endif
}

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/*------------------------------------------------------------------*/
INT ss_exec(char *command, INT * pid)
/********************************************************************\

  Routine: ss_exec

  Purpose: Execute command in a separate process, close all open
           file descriptors, return the pid of the child process.

  Input:
    char * command    Command to execute
    INT  * pid        Returned PID of the spawned process.
  Output:
    none

  Function value:
    SS_SUCCESS       Successful completion
    SS_ABORT         fork() was not successful, or other problem

\********************************************************************/
{
#ifdef OS_UNIX

   /* only implemented for UNIX */
   int i, fd;

   if ((*pid = fork()) < 0)
      return SS_ABORT;
   else if (*pid != 0) {
      /* avoid <defunc> parent processes */
      signal(SIGCHLD, catch_sigchld);
      return SS_SUCCESS;        /* parent returns */
   }

   /* child continues here... */

   /* close all open file descriptors */
   for (i = 0; i < 256; i++)
      close(i);

   /* try and use up stdin, stdout and stderr, so other
      routines writing to stdout etc won't cause havoc */
   for (i = 0; i < 3; i++) {
      fd = open("/dev/null", O_RDWR, 0);
      if (fd < 0)
         fd = open("/dev/null", O_WRONLY, 0);
      if (fd < 0) {
         cm_msg(MERROR, "ss_exec", "Can't open /dev/null");
         return SS_ABORT;
      }
      if (fd != i) {
         cm_msg(MERROR, "ss_exec", "Did not get file descriptor");
         return SS_ABORT;
      }
   }

   setsid();                    /* become session leader */
   /* chdir("/"); *//* change working directory (not on NFS!) */
   umask(0);                    /* clear our file mode createion mask */

   /* execute command */
   execl("/bin/sh", "sh", "-c", command, NULL);

#else

   system(command);

#endif

   return SS_SUCCESS;
}

/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/********************************************************************/
/**
Creates and returns a new thread of execution. 

Note the difference when calling from vxWorks versus Linux and Windows.
The parameter pointer for a vxWorks call is a VX_TASK_SPAWN structure, whereas
for Linux and Windows it is a void pointer.
Early versions returned SS_SUCCESS or SS_NO_THREAD instead of thread ID.

Example for VxWorks
\code
...
VX_TASK_SPAWN tsWatch = {"Watchdog", 100, 0, 2000,  (int) pDevice, 0, 0, 0, 0, 0, 0, 0, 0 ,0};
midas_thread_t thread_id = ss_thread_create((void *) taskWatch, &tsWatch);
if (thread_id == 0) {
  printf("cannot spawn taskWatch\n");
}
...
\endcode
Example for Linux
\code
...
midas_thread_t thread_id = ss_thread_create((void *) taskWatch, pDevice);
if (thread_id == 0) {
  printf("cannot spawn taskWatch\n");
}
...
\endcode
@param (*thread_func) Thread function to create.  
@param param a pointer to a VX_TASK_SPAWN structure for vxWorks and a void pointer
                for Unix and Windows
@return the new thread id or zero on error
*/
midas_thread_t ss_thread_create(INT(*thread_func) (void *), void *param)
{
#if defined(OS_WINNT)

   HANDLE status;
   DWORD thread_id;

   if (thread_func == NULL) {
      return 0;
   }

   status = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) thread_func,
                         (LPVOID) param, 0, &thread_id);

   return status == NULL ?  0 : (midas_thread_t)thread_id;

#elif defined(OS_MSDOS)

   return 0;

#elif defined(OS_VMS)

   return 0;

#elif defined(OS_VXWORKS)

/* taskSpawn which could be considered as a thread under VxWorks
   requires several argument beside the thread args
   taskSpawn (taskname, priority, option, stacksize, entry_point
              , arg1, arg2, ... , arg9, arg10)
   all the arg will have to be retrieved from the param list.
   through a structure to be simpler  */

   INT status;
   VX_TASK_SPAWN *ts;

   ts = (VX_TASK_SPAWN *) param;
   status =
       taskSpawn(ts->name, ts->priority, ts->options, ts->stackSize,
                 (FUNCPTR) thread_func, ts->arg1, ts->arg2, ts->arg3,
                 ts->arg4, ts->arg5, ts->arg6, ts->arg7, ts->arg8, ts->arg9, ts->arg10);

   return status == ERROR ? 0 : status;

#elif defined(OS_UNIX)

   INT status;
   pthread_t thread_id;

   status = pthread_create(&thread_id, NULL, (void *) thread_func, param);

   return status != 0 ? 0 : thread_id;

#endif
}

/********************************************************************/
/** 
Destroys the thread identified by the passed thread id. 
The thread id is returned by ss_thread_create() on creation.

\code
...
midas_thread_t thread_id = ss_thread_create((void *) taskWatch, pDevice);
if (thread_id == 0) {
  printf("cannot spawn taskWatch\n");
}
...
ss_thread_kill(thread_id);
...
\endcode
@param thread_id the thread id of the thread to be killed.
@return SS_SUCCESS if no error, else SS_NO_THREAD
*/
INT ss_thread_kill(midas_thread_t thread_id)
{
#if defined(OS_WINNT)

   DWORD status;

   status = TerminateThread(thread_id, 0);

   return status != 0 ? SS_SUCCESS : SS_NO_THREAD;

#elif defined(OS_MSDOS)

   return 0;

#elif defined(OS_VMS)

   return 0;

#elif defined(OS_VXWORKS)

   INT status;
   status = taskDelete(thread_id);
   return status == OK ? 0 : ERROR;

#elif defined(OS_UNIX)

   INT status;
   status = pthread_kill(thread_id, SIGKILL);
   return status == 0 ? SS_SUCCESS : SS_NO_THREAD;

#endif
}

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/*------------------------------------------------------------------*/
static INT skip_mutex_handle = -1;

INT ss_mutex_create(char *name, HNDLE * mutex_handle)
/********************************************************************\

  Routine: ss_mutex_create

  Purpose: Create a mutex with a specific name

    Remark: Under VxWorks the specific semaphore handling is
            different than other OS. But VxWorks provides
            the POSIX-compatible semaphore interface.
            Under POSIX, no timeout is supported.
            So for the time being, we keep the pure VxWorks
            The semaphore type is a Binary instead of mutex
            as the binary is an optimized mutex.

  Input:
    char   *name            Name of the mutex to create

  Output:
    HNDLE  *mutex_handle    Handle of the created mutex

  Function value:
    SS_CREATED              Mutex was created
    SS_SUCCESS              Mutex existed already and was attached
    SS_NO_MUTEX             Cannot create mutex

\********************************************************************/
{
   char mutex_name[256], path[256], file_name[256];

   /* Add a leading SM_ to the mutex name */
   sprintf(mutex_name, "MX_%s", name);

#ifdef OS_VXWORKS

   /* semBCreate is a Binary semaphore which is under VxWorks a optimized mutex
      refering to the programmer's Guide 5.3.1 */
   if ((*((SEM_ID *) mutex_handle) = semBCreate(SEM_Q_FIFO, SEM_EMPTY)) == NULL)
      return SS_NO_MUTEX;
   return SS_CREATED;

#endif                          /* OS_VXWORKS */

   /* Build the filename out of the path and the name of the mutex */
   cm_get_path(path);
   if (path[0] == 0) {
      getcwd(path, 256);
#if defined(OS_VMS)
#elif defined(OS_UNIX)
      strcat(path, "/");
#elif defined(OS_WINNT)
      strcat(path, "\\");
#endif
   }

   strcpy(file_name, path);
#if defined (OS_UNIX)
   strcat(file_name, ".");      /* dot file under UNIX */
#endif
   strcat(file_name, name);
   strcat(file_name, ".SHM");

#ifdef OS_WINNT

   *mutex_handle = (HNDLE) CreateMutex(NULL, FALSE, mutex_name);

   if (*mutex_handle == 0)
      return SS_NO_MUTEX;

   return SS_CREATED;

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   /* VMS has to use lock manager... */

   {
      INT status;
      $DESCRIPTOR(mutexname_dsc, "dummy");
      mutexname_dsc.dsc$w_length = strlen(mutex_name);
      mutexname_dsc.dsc$a_pointer = mutex_name;

      *mutex_handle = (HNDLE) malloc(8);

      status = sys$enqw(0, LCK$K_NLMODE, *mutex_handle, 0, &mutexname_dsc,
                        0, 0, 0, 0, 0, 0);

      if (status != SS$_NORMAL) {
         free((void *) *mutex_handle);
         *mutex_handle = 0;
      }

      if (*mutex_handle == 0)
         return SS_NO_MUTEX;

      return SS_CREATED;
   }

#endif                          /* OS_VMS */
#ifdef OS_UNIX

   {
      INT key, status, fh;
      struct semid_ds buf;

#if (defined(OS_LINUX) && !defined(_SEM_SEMUN_UNDEFINED)) || defined(OS_FREEBSD)
      union semun arg;
#else
      union semun {
         INT val;
         struct semid_ds *buf;
         ushort *array;
      } arg;
#endif

      status = SS_SUCCESS;

      /* create a unique key from the file name */
      key = ftok(file_name, 'M');
      if (key < 0) {
         fh = open(file_name, O_CREAT, 0644);
         close(fh);
         key = ftok(file_name, 'M');
         status = SS_CREATED;
      }

      /* create or get semaphore */
      *mutex_handle = (HNDLE) semget(key, 1, 0);
      if (*mutex_handle < 0) {
         *mutex_handle = (HNDLE) semget(key, 1, IPC_CREAT);
         status = SS_CREATED;
      }

      if (*mutex_handle < 0) {
         cm_msg(MERROR, "ss_mutex_mutex", "semget() failed, errno = %d", errno);
         return SS_NO_MUTEX;
      }

      buf.sem_perm.uid = getuid();
      buf.sem_perm.gid = getgid();
      buf.sem_perm.mode = 0666;
      arg.buf = &buf;

      semctl(*mutex_handle, 0, IPC_SET, arg);

      /* if semaphore was created, set value to one */
      if (status == SS_CREATED) {
         arg.val = 1;
         if (semctl(*mutex_handle, 0, SETVAL, arg) < 0)
            return SS_NO_MUTEX;
      }

      return SS_SUCCESS;
   }
#endif                          /* OS_UNIX */

#ifdef OS_MSDOS
   return SS_NO_MUTEX;
#endif
}

/*------------------------------------------------------------------*/
INT ss_mutex_wait_for(HNDLE mutex_handle, INT timeout)
/********************************************************************\

  Routine: ss_mutex_wait_for

  Purpose: Wait for a mutex to get owned

  Input:
    HNDLE  *mutex_handle    Handle of the created mutex
    INT    timeout          Timeout in ms, zero for no timeout

  Output:
    none

  Function value:
    SS_SUCCESS              Successful completion
    SS_NO_MUTEX             Invalid mutex handle
    SS_TIMEOUT              Timeout

\********************************************************************/
{
   INT status;

#ifdef OS_WINNT

   status = WaitForSingleObject((HANDLE) mutex_handle, timeout == 0 ? INFINITE : timeout);
   if (status == WAIT_FAILED)
      return SS_NO_MUTEX;
   if (status == WAIT_TIMEOUT)
      return SS_TIMEOUT;

   return SS_SUCCESS;
#endif                          /* OS_WINNT */
#ifdef OS_VMS
   status = sys$enqw(0, LCK$K_EXMODE, mutex_handle, LCK$M_CONVERT, 0, 0, 0, 0, 0, 0, 0);
   if (status != SS$_NORMAL)
      return SS_NO_MUTEX;
   return SS_SUCCESS;

#endif                          /* OS_VMS */
#ifdef OS_VXWORKS
   /* convert timeout in ticks (1/60) = 1000/60 ~ 1/16 = >>4 */
   status = semTake((SEM_ID) mutex_handle, timeout == 0 ? WAIT_FOREVER : timeout >> 4);
   if (status == ERROR)
      return SS_NO_MUTEX;
   return SS_SUCCESS;

#endif                          /* OS_VXWORKS */
#ifdef OS_UNIX
   {
      DWORD start_time;
      struct sembuf sb;

#if (defined(OS_LINUX) && !defined(_SEM_SEMUN_UNDEFINED)) || defined(OS_FREEBSD)
      union semun arg;
#else
      union semun {
         INT val;
         struct semid_ds *buf;
         ushort *array;
      } arg;
#endif

      sb.sem_num = 0;
      sb.sem_op = -1;           /* decrement semaphore */
      sb.sem_flg = SEM_UNDO;

      memset(&arg, 0, sizeof(arg));

      /* don't request the mutex when in asynchronous state
         and mutex was locked already by foreground process */
      if (ss_in_async_routine_flag)
         if (semctl(mutex_handle, 0, GETPID, arg) == getpid())
            if (semctl(mutex_handle, 0, GETVAL, arg) == 0) {
               skip_mutex_handle = mutex_handle;
               return SS_SUCCESS;
            }

      skip_mutex_handle = -1;

      start_time = ss_millitime();

      do {
         status = semop(mutex_handle, &sb, 1);

         /* return on success */
         if (status == 0)
            break;

         /* retry if interrupted by a ss_wake signal */
         if (errno == EINTR) {
            /* return if timeout expired */
            if (timeout > 0 && ss_millitime() - start_time > timeout)
               return SS_TIMEOUT;

            continue;
         }

         return SS_NO_MUTEX;
      } while (1);

      return SS_SUCCESS;
   }
#endif                          /* OS_UNIX */

#ifdef OS_MSDOS
   return SS_NO_MUTEX;
#endif
}

/*------------------------------------------------------------------*/
INT ss_mutex_release(HNDLE mutex_handle)
/********************************************************************\

  Routine: ss_release_mutex

  Purpose: Release ownership of a mutex

  Input:
    HNDLE  *mutex_handle    Handle of the created mutex

  Output:
    none

  Function value:
    SS_SUCCESS              Successful completion
    SS_NO_MUTEX             Invalid mutex handle

\********************************************************************/
{
   INT status;

#ifdef OS_WINNT

   status = ReleaseMutex((HANDLE) mutex_handle);

   if (status == FALSE)
      return SS_NO_MUTEX;

   return SS_SUCCESS;

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   status = sys$enqw(0, LCK$K_NLMODE, mutex_handle, LCK$M_CONVERT, 0, 0, 0, 0, 0, 0, 0);

   if (status != SS$_NORMAL)
      return SS_NO_MUTEX;

   return SS_SUCCESS;

#endif                          /* OS_VMS */

#ifdef OS_VXWORKS

   if (semGive((SEM_ID) mutex_handle) == ERROR)
      return SS_NO_MUTEX;
   return SS_SUCCESS;
#endif                          /* OS_VXWORKS */

#ifdef OS_UNIX
   {
      struct sembuf sb;

      sb.sem_num = 0;
      sb.sem_op = 1;            /* increment semaphore */
      sb.sem_flg = SEM_UNDO;

      if (mutex_handle == skip_mutex_handle) {
         skip_mutex_handle = -1;
         return SS_SUCCESS;
      }

      do {
         status = semop(mutex_handle, &sb, 1);

         /* return on success */
         if (status == 0)
            break;

         /* retry if interrupted by a ss_wake signal */
         if (errno == EINTR)
            continue;

         return SS_NO_MUTEX;
      } while (1);

      return SS_SUCCESS;
   }
#endif                          /* OS_UNIX */

#ifdef OS_MSDOS
   return SS_NO_MUTEX;
#endif
}

/*------------------------------------------------------------------*/
INT ss_mutex_delete(HNDLE mutex_handle, INT destroy_flag)
/********************************************************************\

  Routine: ss_mutex_delete

  Purpose: Delete a mutex

  Input:
    HNDLE  *mutex_handle    Handle of the created mutex

  Output:
    none

  Function value:
    SS_SUCCESS              Successful completion
    SS_NO_MUTEX             Invalid mutex handle

\********************************************************************/
{
#ifdef OS_WINNT

   if (CloseHandle((HANDLE) mutex_handle) == FALSE)
      return SS_NO_MUTEX;

   return SS_SUCCESS;

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   free((void *) mutex_handle);
   return SS_SUCCESS;

#endif                          /* OS_VMS */

#ifdef OS_VXWORKS
   /* no code for VxWorks destroy yet */
   if (semDelete((SEM_ID) mutex_handle) == ERROR)
      return SS_NO_MUTEX;
   return SS_SUCCESS;
#endif                          /* OS_VXWORKS */

#ifdef OS_UNIX
#if (defined(OS_LINUX) && !defined(_SEM_SEMUN_UNDEFINED)) || defined(OS_FREEBSD)
   union semun arg;
#else
   union semun {
      INT val;
      struct semid_ds *buf;
      ushort *array;
   } arg;
#endif

   memset(&arg, 0, sizeof(arg));

   if (destroy_flag)
      if (semctl(mutex_handle, 0, IPC_RMID, arg) < 0)
         return SS_NO_MUTEX;

   return SS_SUCCESS;

#endif                          /* OS_UNIX */

#ifdef OS_MSDOS
   return SS_NO_MUTEX;
#endif
}

/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/********************************************************************/
/**
Returns the actual time in milliseconds with an arbitrary
origin. This time may only be used to calculate relative times.

Overruns in the 32 bit value don't hurt since in a subtraction calculated
with 32 bit accuracy this overrun cancels (you may think about!)..
\code
...
DWORD start, stop:
start = ss_millitime();
  < do operations >
stop = ss_millitime();
printf("Operation took %1.3lf seconds\n",(stop-start)/1000.0);
...
\endcode
@return millisecond time stamp.
*/
DWORD ss_millitime()
{
#ifdef OS_WINNT

   return (int) GetTickCount();

#endif                          /* OS_WINNT */
#ifdef OS_MSDOS

   return clock() * 55;

#endif                          /* OS_MSDOS */
#ifdef OS_VMS

   {
      char time[8];
      DWORD lo, hi;

      sys$gettim(time);

      lo = *((DWORD *) time);
      hi = *((DWORD *) (time + 4));

/*  return *lo / 10000; */

      return lo / 10000 + hi * 429496.7296;

   }

#endif                          /* OS_VMS */
#ifdef OS_UNIX
   {
      struct timeval tv;

      gettimeofday(&tv, NULL);

      return tv.tv_sec * 1000 + tv.tv_usec / 1000;
   }

#endif                          /* OS_UNIX */
#ifdef OS_VXWORKS
   {
      int count;
      static int ticks_per_msec = 0;

      if (ticks_per_msec == 0)
         ticks_per_msec = 1000 / sysClkRateGet();

      return tickGet() * ticks_per_msec;
   }
#endif                          /* OS_VXWORKS */
}

/********************************************************************/
/**
Returns the actual time in seconds since 1.1.1970 UTC.
\code
...
DWORD start, stop:
start = ss_time();
  ss_sleep(12000);
stop = ss_time();
printf("Operation took %1.3lf seconds\n",stop-start);
...
\endcode
@return Time in seconds
*/
DWORD ss_time()
{
#if !defined(OS_VXWORKS)
#if !defined(OS_VMS)
   tzset();
#endif
#endif
   return (DWORD) time(NULL);
}

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/*------------------------------------------------------------------*/
DWORD ss_settime(DWORD seconds)
/********************************************************************\

  Routine: ss_settime

  Purpose: Set local time. Used to synchronize different computers

   Input:
    INT    Time in seconds since 1.1.1970 UTC.

  Output:
    none

  Function value:

\********************************************************************/
{
#if defined(OS_WINNT)
   SYSTEMTIME st;
   struct tm *ltm;

   tzset();
   ltm = localtime((time_t *) & seconds);

   st.wYear = ltm->tm_year + 1900;
   st.wMonth = ltm->tm_mon + 1;
   st.wDay = ltm->tm_mday;
   st.wHour = ltm->tm_hour;
   st.wMinute = ltm->tm_min;
   st.wSecond = ltm->tm_sec;
   st.wMilliseconds = 0;

   SetLocalTime(&st);

#elif defined(OS_DARWIN)

   assert(!"ss_settime() is not supported");
   /* not reached */
   return SS_NO_DRIVER;

#elif defined(OS_UNIX)

   stime((time_t *) & seconds);

#elif defined(OS_VXWORKS)

   struct timespec ltm;

   ltm.tv_sec = seconds;
   ltm.tv_nsec = 0;
   clock_settime(CLOCK_REALTIME, &ltm);

#endif
   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
char *ss_asctime()
/********************************************************************\

  Routine: ss_asctime

  Purpose: Returns the local actual time as a string

  Input:
    none

  Output:
    none

  Function value:
    char   *     Time string

\********************************************************************/
{
   static char str[32];
   time_t seconds;

   seconds = (time_t) ss_time();

#if !defined(OS_VXWORKS)
#if !defined(OS_VMS)
   tzset();
#endif
#endif
   strcpy(str, asctime(localtime(&seconds)));

   /* strip new line */
   str[24] = 0;

   return str;
}

/*------------------------------------------------------------------*/
INT ss_timezone()
/********************************************************************\

  Routine: ss_timezone

  Purpose: Returns difference in seconds between coordinated universal
           time and local time.

  Input:
    none

  Output:
    none

  Function value:
    INT    Time difference in seconds

\********************************************************************/
{
#if defined(OS_DARWIN) || defined(OS_VXWORKS)
  return 0;
#else
  return timezone; /* on Linux, comes from "#include <time.h>". */
#endif
}


/*------------------------------------------------------------------*/

#ifdef OS_UNIX
/* dummy function for signal() call */
void ss_cont()
{
}
#endif

/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/********************************************************************/
/**
Suspend the calling process for a certain time.

The function is similar to the sleep() function,
but has a resolution of one milliseconds. Under VxWorks the resolution
is 1/60 of a second. It uses the socket select() function with a time-out.
See examples in ss_time()
@param millisec Time in milliseconds to sleep. Zero means
                infinite (until another process calls ss_wake)
@return SS_SUCCESS
*/
INT ss_sleep(INT millisec)
{
   fd_set readfds;
   struct timeval timeout;
   int status;
   static int sock = 0;

   if (millisec == 0) {
#ifdef OS_WINNT
      SuspendThread(GetCurrentThread());
#endif
#ifdef OS_VMS
      sys$hiber();
#endif
#ifdef OS_UNIX
      signal(SIGCONT, ss_cont);
      pause();
#endif
      return SS_SUCCESS;
   }
#ifdef OS_WINNT
   {
      WSADATA WSAData;

      /* Start windows sockets */
      if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
         return SS_SOCKET_ERROR;
   }
#endif

   timeout.tv_sec = millisec / 1000;
   timeout.tv_usec = (millisec % 1000) * 1000;

   if (!sock)
      sock = socket(AF_INET, SOCK_DGRAM, 0);

   FD_ZERO(&readfds);
   FD_SET(sock, &readfds);
   do {
      status = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

      /* if an alarm signal was cought, restart select with reduced timeout */
      if (status == -1 && timeout.tv_sec >= WATCHDOG_INTERVAL / 1000)
         timeout.tv_sec -= WATCHDOG_INTERVAL / 1000;

   } while (status == -1);      /* dont return if an alarm signal was cought */

   return SS_SUCCESS;
}

/**dox***************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/*------------------------------------------------------------------*/
BOOL ss_kbhit()
/********************************************************************\

  Routine: ss_kbhit

  Purpose: Returns TRUE if a key is pressed

  Input:
    none

  Output:
    none

  Function value:
    FALSE                 No key has been pressed
    TRUE                  Key has been pressed

\********************************************************************/
{
#ifdef OS_MSDOS

   return kbhit();

#endif                          /* OS_MSDOS */
#ifdef OS_WINNT

   return kbhit();

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   return FALSE;

#endif                          /* OS_VMS */
#ifdef OS_UNIX

   int n;

   if (_daemon_flag)
      return 0;

   ioctl(0, FIONREAD, &n);
   return (n > 0);

#endif                          /* OS_UNIX */
#ifdef OS_VXWORKS

   int n;
   ioctl(0, FIONREAD, (long) &n);
   return (n > 0);

#endif                          /* OS_UNIX */
}


/*------------------------------------------------------------------*/
#ifdef LOCAL_ROUTINES

INT ss_wake(INT pid, INT tid, INT thandle)
/********************************************************************\

  Routine: ss_wake

  Purpose: Wake a process with a specific ID and optional with a
     specific thread ID (on OS with support threads).

  Input:
    INT    pid              Process ID
    INT    tid              Thread ID
    INT    thandle          Thread handle (used under Windows NT)


  Output:
    none

  Function value:
    SS_SUCCESS              Successful completion
    SS_NO_PROCESS           Nonexisting process

\********************************************************************/
{
#ifdef OS_WINNT
   HANDLE process_handle;
   HANDLE dup_thread_handle;

   /*
      Under Windows NT, it's a little bit tricky to get a thread handle
      which can be passed to other processes. First the calling process
      has to duplicate the GetCurrentThread() handle, then the process
      which gets this handle also has to use DuplicateHandle with the
      target process as its own. Then this duplicated handle can be used
      to the SupendThread() and ResumeThread() functions.
    */

   process_handle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);

   if (process_handle == 0)
      return SS_NO_PROCESS;

   DuplicateHandle(process_handle, (HANDLE) thandle, GetCurrentProcess(),
                   &dup_thread_handle, THREAD_ALL_ACCESS, TRUE, 0);

   /* close handles not to waste resources */
   CloseHandle(process_handle);

   if (dup_thread_handle == 0)
      return SS_NO_PROCESS;

   ResumeThread(dup_thread_handle);

   /* close handles not to waste resources */
   CloseHandle(dup_thread_handle);

   return SS_SUCCESS;

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   if (sys$wake(&pid, 0) == SS$_NONEXPR)
      return SS_NO_PROCESS;

   return SS_SUCCESS;

#endif                          /* OS_VMS */
#ifdef OS_UNIX

   if (kill(pid, SIGCONT) < 0)
      return SS_NO_PROCESS;

   return SS_SUCCESS;

   /*
      Bryan: kill has to get the target process out of pause(). Some
      UNIX documentation say that pause() terminates by receiving any
      signal which would be ok because the buffer manager is designed
      in a way where an additional wake doesn't hurt. If this is not
      true, one has to setup a signal(SIGCONT, dummy) in the ss_sleep
      routine or so. Please check this.
    */
#endif                          /* OS_UNIX */
}

/*------------------------------------------------------------------*/
#ifdef OS_WINNT

static void (*UserCallback) (int);
static UINT _timer_id = 0;

VOID CALLBACK _timeCallback(UINT idEvent, UINT uReserved, DWORD dwUser,
                            DWORD dwReserved1, DWORD dwReserved2)
{
   _timer_id = 0;
   if (UserCallback != NULL)
      UserCallback(0);
}

#endif                          /* OS_WINNT */

INT ss_alarm(INT millitime, void (*func) (int))
/********************************************************************\

  Routine: ss_alarm

  Purpose: Schedules an alarm. Call function referenced by *func
     after the specified seconds.

  Input:
    INT    millitime        Time in milliseconds
    void   (*func)()        Function to be called after the spe-
          cified time.

  Output:
    none

  Function value:
    SS_SUCCESS              Successful completion

\********************************************************************/
{
#ifdef OS_WINNT

   UserCallback = func;
   if (millitime > 0)
      _timer_id =
          timeSetEvent(millitime, 100, (LPTIMECALLBACK) _timeCallback, 0, TIME_ONESHOT);
   else {
      if (_timer_id)
         timeKillEvent(_timer_id);
      _timer_id = 0;
   }

   return SS_SUCCESS;

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   signal(SIGALRM, func);
   alarm(millitime / 1000);
   return SS_SUCCESS;

#endif                          /* OS_VMS */
#ifdef OS_UNIX

   signal(SIGALRM, func);
   alarm(millitime / 1000);
   return SS_SUCCESS;

#endif                          /* OS_UNIX */
}

/*------------------------------------------------------------------*/
void (*MidasExceptionHandler) ();

#ifdef OS_WINNT

LONG MidasExceptionFilter(LPEXCEPTION_POINTERS pexcep)
{
   if (MidasExceptionHandler != NULL)
      MidasExceptionHandler();

   return EXCEPTION_CONTINUE_SEARCH;
}

INT MidasExceptionSignal(INT sig)
{
   if (MidasExceptionHandler != NULL)
      MidasExceptionHandler();

   raise(sig);

   return 0;
}

/*
INT _matherr(struct _exception *except)
{
  if (MidasExceptionHandler != NULL)
    MidasExceptionHandler();

  return 0;
}
*/

#endif                          /* OS_WINNT */

#ifdef OS_VMS

INT MidasExceptionFilter(INT * sigargs, INT * mechargs)
{
   if (MidasExceptionHandler != NULL)
      MidasExceptionHandler();

   return (SS$_RESIGNAL);
}

void MidasExceptionSignal(INT sig)
{
   if (MidasExceptionHandler != NULL)
      MidasExceptionHandler();

   kill(getpid(), sig);
}

#endif                          /* OS_VMS */

/*------------------------------------------------------------------*/
INT ss_exception_handler(void (*func) ())
/********************************************************************\

  Routine: ss_exception_handler

  Purpose: Establish new exception handler which is called before
     the program is aborted due to a Ctrl-Break or an access
     violation. This handler may clean up things which may
     otherwise left in an undefined state.

  Input:
    void  (*func)()     Address of handler function
  Output:
    none

  Function value:
    BM_SUCCESS          Successful completion

\********************************************************************/
{
#ifdef OS_WINNT

   MidasExceptionHandler = func;
/*  SetUnhandledExceptionFilter(
    (LPTOP_LEVEL_EXCEPTION_FILTER) MidasExceptionFilter);

  signal(SIGINT, MidasExceptionSignal);
  signal(SIGILL, MidasExceptionSignal);
  signal(SIGFPE, MidasExceptionSignal);
  signal(SIGSEGV, MidasExceptionSignal);
  signal(SIGTERM, MidasExceptionSignal);
  signal(SIGBREAK, MidasExceptionSignal);
  signal(SIGABRT, MidasExceptionSignal); */

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   MidasExceptionHandler = func;
   lib$establish(MidasExceptionFilter);

   signal(SIGINT, MidasExceptionSignal);
   signal(SIGILL, MidasExceptionSignal);
   signal(SIGQUIT, MidasExceptionSignal);
   signal(SIGFPE, MidasExceptionSignal);
   signal(SIGSEGV, MidasExceptionSignal);
   signal(SIGTERM, MidasExceptionSignal);

#endif                          /* OS_WINNT */

   return SS_SUCCESS;
}

#endif                          /* LOCAL_ROUTINES */

/*------------------------------------------------------------------*/
void *ss_ctrlc_handler(void (*func) (int))
/********************************************************************\

  Routine: ss_ctrlc_handler

  Purpose: Establish new exception handler which is called before
     the program is aborted due to a Ctrl-Break. This handler may
     clean up things which may otherwise left in an undefined state.

  Input:
    void  (*func)(int)     Address of handler function, if NULL
                           install default handler

  Output:
    none

  Function value:
    same as signal()

\********************************************************************/
{
#ifdef OS_WINNT

   if (func == NULL) {
      signal(SIGBREAK, SIG_DFL);
      return signal(SIGINT, SIG_DFL);
   } else {
      signal(SIGBREAK, func);
      return signal(SIGINT, func);
   }
   return NULL;

#endif                          /* OS_WINNT */
#ifdef OS_VMS

   return signal(SIGINT, func);

#endif                          /* OS_WINNT */

#ifdef OS_UNIX

   if (func == NULL) {
      signal(SIGTERM, SIG_DFL);
      return (void *) signal(SIGINT, SIG_DFL);
   } else {
      signal(SIGTERM, func);
      return (void *) signal(SIGINT, func);
   }

#endif                          /* OS_UNIX */
}

/*------------------------------------------------------------------*/
/********************************************************************\
*                                                                    *
*                  Suspend/resume functions                          *
*                                                                    *
\********************************************************************/

/*------------------------------------------------------------------*/
/* globals */

/*
   The suspend structure is used in a multithread environment
   (multi thread server) where each thread may resume another thread.
   Since all threads share the same global memory, the ports and
   sockets for suspending and resuming must be stored in a array
   which keeps one entry for each thread.
*/

typedef struct {
   BOOL in_use;
   INT thread_id;
   INT ipc_port;
   INT ipc_recv_socket;
   INT ipc_send_socket;
    INT(*ipc_dispatch) (char *, INT);
   INT listen_socket;
    INT(*listen_dispatch) (INT);
   RPC_SERVER_CONNECTION *server_connection;
    INT(*client_dispatch) (INT);
   RPC_SERVER_ACCEPTION *server_acception;
    INT(*server_dispatch) (INT, int, BOOL);
   struct sockaddr_in bind_addr;
} SUSPEND_STRUCT;

SUSPEND_STRUCT *_suspend_struct = NULL;
INT _suspend_entries;

/*------------------------------------------------------------------*/
INT ss_suspend_init_ipc(INT index)
/********************************************************************\

  Routine: ss_suspend_init_ipc

  Purpose: Create sockets used in the suspend/resume mechanism.

  Input:
    INT    index            Index to the _suspend_struct array for
          the calling thread.
  Output:
    <indirect>              Set entry in _suspend_struct

  Function value:
    SS_SUCCESS              Successful completion
    SS_SOCKET_ERROR         Error in socket routines
    SS_NO_MEMORY            Not enough memory

\********************************************************************/
{
   INT status, sock;
   int i;
   struct sockaddr_in bind_addr;
   char local_host_name[HOST_NAME_LENGTH];
   struct hostent *phe;

#ifdef OS_WINNT
   {
      WSADATA WSAData;

      /* Start windows sockets */
      if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0)
         return SS_SOCKET_ERROR;
   }
#endif

  /*--------------- create UDP receive socket -------------------*/
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock == -1)
      return SS_SOCKET_ERROR;

   /* let OS choose port for socket */
   memset(&bind_addr, 0, sizeof(bind_addr));
   bind_addr.sin_family = AF_INET;
   bind_addr.sin_addr.s_addr = 0;
   bind_addr.sin_port = 0;

   gethostname(local_host_name, sizeof(local_host_name));

#ifdef OS_VXWORKS
   {
      INT host_addr;

      host_addr = hostGetByName(local_host_name);
      memcpy((char *) &(bind_addr.sin_addr), &host_addr, 4);
   }
#else
   phe = gethostbyname(local_host_name);
   if (phe == NULL) {
      cm_msg(MERROR, "ss_suspend_init_ipc", "cannot get host name");
      return SS_SOCKET_ERROR;
   }
   memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);
#endif

   status = bind(sock, (struct sockaddr *) &bind_addr, sizeof(bind_addr));
   if (status < 0)
      return SS_SOCKET_ERROR;

   /* find out which port OS has chosen */
   i = sizeof(bind_addr);
   getsockname(sock, (struct sockaddr *) &bind_addr, (int *) &i);

   _suspend_struct[index].ipc_recv_socket = sock;
   _suspend_struct[index].ipc_port = ntohs(bind_addr.sin_port);

  /*--------------- create UDP send socket ----------------------*/
   sock = socket(AF_INET, SOCK_DGRAM, 0);

   if (sock == -1)
      return SS_SOCKET_ERROR;

   /* fill out bind struct pointing to local host */
   memset(&bind_addr, 0, sizeof(bind_addr));
   bind_addr.sin_family = AF_INET;
   bind_addr.sin_addr.s_addr = 0;

#ifdef OS_VXWORKS
   {
      INT host_addr;

      host_addr = hostGetByName(local_host_name);
      memcpy((char *) &(bind_addr.sin_addr), &host_addr, 4);
   }
#else
   memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);
#endif

   memcpy(&_suspend_struct[index].bind_addr, &bind_addr, sizeof(bind_addr));
   _suspend_struct[index].ipc_send_socket = sock;

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_suspend_get_index(INT * pindex)
/********************************************************************\

  Routine: ss_suspend_init

  Purpose: Return the index for the suspend structure for this
     thread.

  Input:
    none

  Output:
    INT    *pindex          Index to the _suspend_struct array for
          the calling thread.

  Function value:
    SS_SUCCESS              Successful completion
    SS_NO_MEMORY            Not enough memory

\********************************************************************/
{
   INT index;

   if (_suspend_struct == NULL) {
      /* create a new entry for this thread */
      _suspend_struct = (SUSPEND_STRUCT *) malloc(sizeof(SUSPEND_STRUCT));
      memset(_suspend_struct, 0, sizeof(SUSPEND_STRUCT));
      if (_suspend_struct == NULL)
         return SS_NO_MEMORY;

      _suspend_entries = 1;
      *pindex = 0;
      _suspend_struct[0].thread_id = ss_gettid();
      _suspend_struct[0].in_use = TRUE;
   } else {
      /* check for an existing entry for this thread */
      for (index = 0; index < _suspend_entries; index++)
         if (_suspend_struct[index].thread_id == ss_gettid()) {
            if (pindex != NULL)
               *pindex = index;

            return SS_SUCCESS;
         }

      /* check for a deleted entry */
      for (index = 0; index < _suspend_entries; index++)
         if (!_suspend_struct[index].in_use)
            break;

      if (index == _suspend_entries) {
         /* if not found, create new one */
         _suspend_struct = (SUSPEND_STRUCT *) realloc(_suspend_struct,
                                                      sizeof
                                                      (SUSPEND_STRUCT) *
                                                      (_suspend_entries + 1));
         memset(&_suspend_struct[_suspend_entries], 0, sizeof(SUSPEND_STRUCT));

         _suspend_entries++;
         if (_suspend_struct == NULL) {
            _suspend_entries--;
            return SS_NO_MEMORY;
         }
      }
      *pindex = index;
      _suspend_struct[index].thread_id = ss_gettid();
      _suspend_struct[index].in_use = TRUE;
   }

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_suspend_exit()
/********************************************************************\

  Routine: ss_suspend_exit

  Purpose: Closes the sockets used in the suspend/resume mechanism.
     Should be called before a thread exits.

  Input:
    none

  Output:
    none

  Function value:
    SS_SUCCESS              Successful completion

\********************************************************************/
{
   INT i, status;

   status = ss_suspend_get_index(&i);

   if (status != SS_SUCCESS)
      return status;

   if (_suspend_struct[i].ipc_recv_socket) {
      closesocket(_suspend_struct[i].ipc_recv_socket);
      closesocket(_suspend_struct[i].ipc_send_socket);
   }

   memset(&_suspend_struct[i], 0, sizeof(SUSPEND_STRUCT));

   /* calculate new _suspend_entries value */
   for (i = _suspend_entries - 1; i >= 0; i--)
      if (_suspend_struct[i].in_use)
         break;

   _suspend_entries = i + 1;

   if (_suspend_entries == 0) {
      free(_suspend_struct);
      _suspend_struct = NULL;
   }

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_suspend_set_dispatch(INT channel, void *connection, INT(*dispatch) ())
/********************************************************************\

  Routine: ss_suspend_set_dispatch

  Purpose: Set dispatch functions which get called whenever new data
     on various sockets arrive inside the ss_suspend function.

     Beside the Inter Process Communication socket several other
     sockets can simultanously watched: A "listen" socket for
     a server main thread, server sockets which receive new
     RPC requests from remote clients (given by the
     server_acception array) and client sockets which may
     get notification data from remote servers (such as
     database updates).

  Input:
    INT    channel               One of CH_IPC, CH_CLIENT,
         CH_SERVER, CH_MSERVER

    INT    (*dispatch())         Function being called

  Output:
    none

  Function value:
    SS_SUCCESS              Successful completion

\********************************************************************/
{
   INT i, status;

   status = ss_suspend_get_index(&i);

   if (status != SS_SUCCESS)
      return status;

   if (channel == CH_IPC) {
      _suspend_struct[i].ipc_dispatch = (INT(*)(char *, INT)) dispatch;

      if (!_suspend_struct[i].ipc_recv_socket)
         ss_suspend_init_ipc(i);
   }

   if (channel == CH_LISTEN) {
      _suspend_struct[i].listen_socket = *((INT *) connection);
      _suspend_struct[i].listen_dispatch = (INT(*)(INT)) dispatch;
   }

   if (channel == CH_CLIENT) {
      _suspend_struct[i].server_connection = (RPC_SERVER_CONNECTION *) connection;
      _suspend_struct[i].client_dispatch = (INT(*)(INT)) dispatch;
   }

   if (channel == CH_SERVER) {
      _suspend_struct[i].server_acception = (RPC_SERVER_ACCEPTION *) connection;
      _suspend_struct[i].server_dispatch = (INT(*)(INT, int, BOOL)) dispatch;
   }

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_suspend_get_port(INT * port)
/********************************************************************\

  Routine: ss_suspend_get_port

  Purpose: Return the UDP port number which can be used to resume
     the calling thread inside a ss_suspend function. The port
     number can then be used by another process as a para-
     meter to the ss_resume function to resume the thread
     which called ss_suspend.

  Input:
    none

  Output:
    INT    *port            UDP port number

  Function value:
    SS_SUCCESS              Successful completion

\********************************************************************/
{
   INT index, status;

   status = ss_suspend_get_index(&index);

   if (status != SS_SUCCESS)
      return status;

   if (!_suspend_struct[index].ipc_port)
      ss_suspend_init_ipc(index);

   *port = _suspend_struct[index].ipc_port;

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_suspend(INT millisec, INT msg)
/********************************************************************\

  Routine: ss_suspend

  Purpose: Suspend the calling thread for a speficic time. If
     timeout (in millisec.) is negative, the thead is suspended
     indefinitely. It can only be resumed from another thread
     or process which calls ss_resume or by some data which
     arrives on the client or server sockets.

     If msg equals to one of MSG_BM, MSG_ODB, the function
     return whenever such a message is received.

  Input:
    INT    millisec         Timeout in milliseconds
    INT    msg              Return from ss_suspend when msg
          (MSG_BM, MSG_ODB) is received.

  Output:
    none

  Function value:
    SS_SUCCESS              Requested message was received
    SS_TIMEOUT              Timeout expired
    SS_SERVER_RECV          Server channel got data
    SS_CLIENT_RECV          Client channel got data
    SS_ABORT (RPC_ABORT)    Connection lost
    SS_EXIT                 Connection closed

\********************************************************************/
{
   fd_set readfds;
   struct timeval timeout;
   INT sock, server_socket;
   INT index, status, i, return_status;
   int size;
   struct sockaddr from_addr;
   char str[100], buffer[80], buffer_tmp[80];

   /* get index to _suspend_struct for this thread */
   status = ss_suspend_get_index(&index);

   if (status != SS_SUCCESS)
      return status;

   return_status = SS_TIMEOUT;

   do {
      FD_ZERO(&readfds);

      /* check listen socket */
      if (_suspend_struct[index].listen_socket)
         FD_SET(_suspend_struct[index].listen_socket, &readfds);

      /* check server channels */
      if (_suspend_struct[index].server_acception)
         for (i = 0; i < MAX_RPC_CONNECTION; i++) {
            /* RPC channel */
            sock = _suspend_struct[index].server_acception[i].recv_sock;

            /* only watch the event tcp connection belonging to this thread */
            if (!sock || _suspend_struct[index].server_acception[i].tid != ss_gettid())
               continue;

            /* watch server socket if no data in cache */
            if (recv_tcp_check(sock) == 0)
               FD_SET(sock, &readfds);
            /* set timeout to zero if data in cache (-> just quick check IPC)
               and not called from inside bm_send_event (-> wait for IPC) */
            else if (msg == 0)
               millisec = 0;

            /* event channel */
            sock = _suspend_struct[index].server_acception[i].event_sock;

            if (!sock)
               continue;

            /* watch server socket if no data in cache */
            if (recv_event_check(sock) == 0)
               FD_SET(sock, &readfds);
            /* set timeout to zero if data in cache (-> just quick check IPC)
               and not called from inside bm_send_event (-> wait for IPC) */
            else if (msg == 0)
               millisec = 0;
         }

      /* watch client recv connections */
      if (_suspend_struct[index].server_connection) {
         sock = _suspend_struct[index].server_connection->recv_sock;
         if (sock)
            FD_SET(sock, &readfds);
      }

      /* check IPC socket */
      if (_suspend_struct[index].ipc_recv_socket)
         FD_SET(_suspend_struct[index].ipc_recv_socket, &readfds);

      timeout.tv_sec = millisec / 1000;
      timeout.tv_usec = (millisec % 1000) * 1000;

      do {
         if (millisec < 0)
            status = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);    /* blocking */
         else
            status = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

         /* if an alarm signal was cought, restart select with reduced timeout */
         if (status == -1 && timeout.tv_sec >= WATCHDOG_INTERVAL / 1000)
            timeout.tv_sec -= WATCHDOG_INTERVAL / 1000;

      } while (status == -1);   /* dont return if an alarm signal was cought */

      /* if listen socket got data, call dispatcher with socket */
      if (_suspend_struct[index].listen_socket &&
          FD_ISSET(_suspend_struct[index].listen_socket, &readfds)) {
         sock = _suspend_struct[index].listen_socket;

         if (_suspend_struct[index].listen_dispatch) {
            status = _suspend_struct[index].listen_dispatch(sock);
            if (status == RPC_SHUTDOWN)
               return status;
         }
      }

      /* check server channels */
      if (_suspend_struct[index].server_acception)
         for (i = 0; i < MAX_RPC_CONNECTION; i++) {
            /* rpc channel */
            sock = _suspend_struct[index].server_acception[i].recv_sock;

            /* only watch the event tcp connection belonging to this thread */
            if (!sock || _suspend_struct[index].server_acception[i].tid != ss_gettid())
               continue;

            if (recv_tcp_check(sock) || FD_ISSET(sock, &readfds)) {
               if (_suspend_struct[index].server_dispatch) {
                  status = _suspend_struct[index].server_dispatch(i, sock, msg != 0);
                  _suspend_struct[index].server_acception[i].
                      last_activity = ss_millitime();

                  if (status == SS_ABORT || status == SS_EXIT || status == RPC_SHUTDOWN)
                     return status;

                  return_status = SS_SERVER_RECV;
               }
            }

            /* event channel */
            sock = _suspend_struct[index].server_acception[i].event_sock;
            if (!sock)
               continue;

            if (recv_event_check(sock) || FD_ISSET(sock, &readfds)) {
               if (_suspend_struct[index].server_dispatch) {
                  status = _suspend_struct[index].server_dispatch(i, sock, msg != 0);
                  _suspend_struct[index].server_acception[i].
                      last_activity = ss_millitime();

                  if (status == SS_ABORT || status == SS_EXIT || status == RPC_SHUTDOWN)
                     return status;

                  return_status = SS_SERVER_RECV;
               }
            }
         }

      /* check server message channels */
      if (_suspend_struct[index].server_connection) {
         sock = _suspend_struct[index].server_connection->recv_sock;

         if (sock && FD_ISSET(sock, &readfds)) {
            if (_suspend_struct[index].client_dispatch)
               status = _suspend_struct[index].client_dispatch(sock);
            else {
               status = SS_SUCCESS;
               size = recv_tcp(sock, buffer, sizeof(buffer), 0);

               if (size <= 0)
                  status = SS_ABORT;
            }

            if (status == SS_ABORT) {
               sprintf(str, "Server connection broken to %s",
                       _suspend_struct[index].server_connection->host_name);
               cm_msg(MINFO, "ss_suspend", str);

               /* close client connection if link broken */
               closesocket(_suspend_struct[index].server_connection->send_sock);
               closesocket(_suspend_struct[index].server_connection->recv_sock);
               closesocket(_suspend_struct[index].server_connection->event_sock);

               memset(_suspend_struct[index].server_connection,
                      0, sizeof(RPC_CLIENT_CONNECTION));

               /* exit program after broken connection to MIDAS server */
               return SS_ABORT;
            }

            return_status = SS_CLIENT_RECV;
         }
      }

      /* check IPC socket */
      if (_suspend_struct[index].ipc_recv_socket &&
          FD_ISSET(_suspend_struct[index].ipc_recv_socket, &readfds)) {
         /* receive IPC message */
         size = sizeof(struct sockaddr);
         size = recvfrom(_suspend_struct[index].ipc_recv_socket,
                         buffer, sizeof(buffer), 0, &from_addr, (int *) &size);

         /* find out if this thread is connected as a server */
         server_socket = 0;
         if (_suspend_struct[index].server_acception &&
             rpc_get_server_option(RPC_OSERVER_TYPE) != ST_REMOTE)
            for (i = 0; i < MAX_RPC_CONNECTION; i++) {
               sock = _suspend_struct[index].server_acception[i].send_sock;
               if (sock && _suspend_struct[index].server_acception[i].tid == ss_gettid())
                  server_socket = sock;
            }

         /* receive further messages to empty UDP queue */
         do {
            FD_ZERO(&readfds);
            FD_SET(_suspend_struct[index].ipc_recv_socket, &readfds);

            timeout.tv_sec = 0;
            timeout.tv_usec = 0;

            status = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

            if (status != -1
                && FD_ISSET(_suspend_struct[index].ipc_recv_socket, &readfds)) {
               size = sizeof(struct sockaddr);
               size =
                   recvfrom(_suspend_struct[index].ipc_recv_socket,
                            buffer_tmp, sizeof(buffer_tmp), 0, &from_addr, &size);

               /* don't forward same MSG_BM as above */
               if (buffer_tmp[0] != 'B' || strcmp(buffer_tmp, buffer) != 0)
                  if (_suspend_struct[index].ipc_dispatch)
                     _suspend_struct[index].ipc_dispatch(buffer_tmp, server_socket);
            }

         } while (FD_ISSET(_suspend_struct[index].ipc_recv_socket, &readfds));

         /* return if received requested message */
         if (msg == MSG_BM && buffer[0] == 'B')
            return SS_SUCCESS;
         if (msg == MSG_ODB && buffer[0] == 'O')
            return SS_SUCCESS;

         /* call dispatcher */
         if (_suspend_struct[index].ipc_dispatch)
            _suspend_struct[index].ipc_dispatch(buffer, server_socket);

         return_status = SS_SUCCESS;
      }

   } while (millisec < 0);

   return return_status;
}

/*------------------------------------------------------------------*/
INT ss_resume(INT port, char *message)
/********************************************************************\

  Routine: ss_resume

  Purpose: Resume another thread or process which called ss_suspend.
     The port has to be transfered (shared memory or so) from
     the thread or process which should be resumed. In that
     process it can be obtained via ss_suspend_get_port.

  Input:
    INT    port             UDP port number
    INT    msg              Mesage id & parameter transferred to
    INT    param              target process

  Output:
    none

  Function value:
    SS_SUCCESS              Successful completion
    SS_SOCKET_ERROR         Socket error

\********************************************************************/
{
   INT status, index;

   if (ss_in_async_routine_flag) {
      /* if called from watchdog, tid is different under NT! */
      index = 0;
   } else {
      status = ss_suspend_get_index(&index);

      if (status != SS_SUCCESS)
         return status;
   }

   _suspend_struct[index].bind_addr.sin_port = htons((short) port);

   status = sendto(_suspend_struct[index].ipc_send_socket, message,
                   strlen(message) + 1, 0,
                   (struct sockaddr *) &_suspend_struct[index].bind_addr,
                   sizeof(struct sockaddr_in));

   if (status != (INT) strlen(message) + 1)
      return SS_SOCKET_ERROR;

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
/********************************************************************\
*                                                                    *
*                     Network functions                              *
*                                                                    *
\********************************************************************/

/*------------------------------------------------------------------*/
INT send_tcp(int sock, char *buffer, DWORD buffer_size, INT flags)
/********************************************************************\

  Routine: send_tcp

  Purpose: Send network data over TCP port. Break buffer in smaller
           parts if larger than maximum TCP buffer size (usually 64k).

  Input:
    INT   sock               Socket which was previosly opened.
    DWORD buffer_size        Size of the buffer in bytes.
    INT   flags              Flags passed to send()

  Output:
    char  *buffer            Network receive buffer.

  Function value:
    INT                     Same as send()

\********************************************************************/
{
   DWORD count;
   INT status;

   /* transfer fragments until complete buffer is transferred */

   for (count = 0; (INT) count < (INT) buffer_size - NET_TCP_SIZE;) {
      status = send(sock, buffer + count, NET_TCP_SIZE, flags);
      if (status != -1)
         count += status;
      else {
         cm_msg(MERROR, "send_tcp",
                "send(socket=%d,size=%d) returned %d, errno: %d (%s)",
                sock, NET_TCP_SIZE, status, errno, strerror(errno));
         return status;
      }
   }

   while (count < buffer_size) {
      status = send(sock, buffer + count, buffer_size - count, flags);
      if (status != -1)
         count += status;
      else {
         cm_msg(MERROR, "send_tcp",
                "send(socket=%d,size=%d) returned %d, errno: %d (%s)",
                sock, (int) (buffer_size - count), status, errno, strerror(errno));
         return status;
      }
   }

   return count;
}

/*------------------------------------------------------------------*/
INT recv_string(int sock, char *buffer, DWORD buffer_size, INT millisec)
/********************************************************************\

  Routine: recv_string

  Purpose: Receive network data over TCP port. Since sockets are
     operated in stream mode, a single transmission via send
     may not transfer the full data. Therefore, one has to check
     at the receiver side if the full data is received. If not,
     one has to issue several recv() commands.

     The length of the data is determined by a trailing zero.

  Input:
    INT   sock               Socket which was previosly opened.
    DWORD buffer_size        Size of the buffer in bytes.
    INT   millisec           Timeout in ms

  Output:
    char  *buffer            Network receive buffer.

  Function value:
    INT                      String length

\********************************************************************/
{
   INT i, status;
   DWORD n;
   fd_set readfds;
   struct timeval timeout;

   n = 0;
   memset(buffer, 0, buffer_size);

   do {
      if (millisec > 0) {
         FD_ZERO(&readfds);
         FD_SET(sock, &readfds);

         timeout.tv_sec = millisec / 1000;
         timeout.tv_usec = (millisec % 1000) * 1000;

         do {
            status = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

            /* if an alarm signal was cought, restart select with reduced timeout */
            if (status == -1 && timeout.tv_sec >= WATCHDOG_INTERVAL / 1000)
               timeout.tv_sec -= WATCHDOG_INTERVAL / 1000;

         } while (status == -1);        /* dont return if an alarm signal was cought */

         if (!FD_ISSET(sock, &readfds))
            break;
      }

      i = recv(sock, buffer + n, 1, 0);

      if (i <= 0)
         break;

      n++;

      if (n >= buffer_size)
         break;

   } while (buffer[n - 1] && buffer[n - 1] != 10);

   return n - 1;
}

/*------------------------------------------------------------------*/
INT recv_tcp(int sock, char *net_buffer, DWORD buffer_size, INT flags)
/********************************************************************\

  Routine: recv_tcp

  Purpose: Receive network data over TCP port. Since sockets are
     operated in stream mode, a single transmission via send
     may not transfer the full data. Therefore, one has to check
     at the receiver side if the full data is received. If not,
     one has to issue several recv() commands.

     The length of the data is determined by the data header,
     which consists of two DWORDs. The first is the command code
     (or function id), the second is the size of the following
     parameters in bytes. From that size recv_tcp() determines
     how much data to receive.

  Input:
    INT   sock               Socket which was previosly opened.
    char  *net_buffer        Buffer to store data to
    DWORD buffer_size        Size of the buffer in bytes.
    INT   flags              Flags passed to recv()

  Output:
    char  *buffer            Network receive buffer.

  Function value:
    INT                      Same as recv()

\********************************************************************/
{
   INT param_size, n_received, n;
   NET_COMMAND *nc;

   if (buffer_size < sizeof(NET_COMMAND_HEADER)) {
      cm_msg(MERROR, "recv_tcp", "parameters too large for network buffer");
      return -1;
   }

   /* first receive header */
   n_received = 0;
   do {
#ifdef OS_UNIX
      do {
         n = recv(sock, net_buffer + n_received, sizeof(NET_COMMAND_HEADER), flags);

         /* don't return if an alarm signal was cought */
      } while (n == -1 && errno == EINTR);
#else
      n = recv(sock, net_buffer + n_received, sizeof(NET_COMMAND_HEADER), flags);
#endif

      if (n <= 0) {
         cm_msg(MERROR, "recv_tcp",
                "header: recv returned %d, n_received = %d, errno: %d (%s)",
                n, n_received, errno, strerror(errno));
         return n;
      }

      n_received += n;

   } while (n_received < sizeof(NET_COMMAND_HEADER));

   /* now receive parameters */

   nc = (NET_COMMAND *) net_buffer;
   param_size = nc->header.param_size;
   n_received = 0;

   if (param_size == 0)
      return sizeof(NET_COMMAND_HEADER);

   do {
#ifdef OS_UNIX
      do {
         n = recv(sock,
                  net_buffer + sizeof(NET_COMMAND_HEADER) + n_received,
                  param_size - n_received, flags);

         /* don't return if an alarm signal was cought */
      } while (n == -1 && errno == EINTR);
#else
      n = recv(sock, net_buffer + sizeof(NET_COMMAND_HEADER) + n_received,
               param_size - n_received, flags);
#endif

      if (n <= 0) {
         cm_msg(MERROR, "recv_tcp",
                "param: recv returned %d, n_received = %d, errno: %d (%s)",
                n, n_received, errno, strerror(errno));
         return n;
      }

      n_received += n;
   } while (n_received < param_size);

   return sizeof(NET_COMMAND_HEADER) + param_size;
}

/*------------------------------------------------------------------*/
INT send_udp(int sock, char *buffer, DWORD buffer_size, INT flags)
/********************************************************************\

  Routine: send_udp

  Purpose: Send network data over UDP port. If buffer_size is small,
     collect several events and send them together. If
     buffer_size is larger than largest datagram size
     NET_UDP_SIZE, split event in several udp buffers and
     send them separately with serial number protection.

  Input:
    INT   sock               Socket which was previosly opened.
    DWORD buffer_size        Size of the buffer in bytes.
    INT   flags              Flags passed to send()

  Output:
    char  *buffer            Network receive buffer.

  Function value:
    INT                     Same as send()

\********************************************************************/
{
   INT status;
   UDP_HEADER *udp_header;
   static char udp_buffer[NET_UDP_SIZE];
   static INT serial_number = 0, n_received = 0;
   DWORD i, data_size;

   udp_header = (UDP_HEADER *) udp_buffer;
   data_size = NET_UDP_SIZE - sizeof(UDP_HEADER);

   /*
      If buffer size is between half the UPD size and full UDP size,
      send immediately a single packet.
    */
   if (buffer_size >= NET_UDP_SIZE / 2 && buffer_size <= data_size) {
      /*
         If there is any data already in the buffer, send it first.
       */
      if (n_received) {
         udp_header->serial_number = UDP_FIRST | n_received;
         udp_header->sequence_number = ++serial_number;

         send(sock, udp_buffer, n_received + sizeof(UDP_HEADER), flags);
         n_received = 0;
      }

      udp_header->serial_number = UDP_FIRST | buffer_size;
      udp_header->sequence_number = ++serial_number;

      memcpy(udp_header + 1, buffer, buffer_size);
      status = send(sock, udp_buffer, buffer_size + sizeof(UDP_HEADER), flags);
      if (status == (INT) buffer_size + (int) sizeof(UDP_HEADER))
         status -= sizeof(UDP_HEADER);

      return status;
   }

   /*
      If buffer size is smaller than half the UDP size, collect events
      until UDP buffer is optimal filled.
    */
   if (buffer_size <= data_size) {
      /* If udp_buffer has space, just copy it there */
      if (buffer_size + n_received < data_size) {
         memcpy(udp_buffer + sizeof(UDP_HEADER) + n_received, buffer, buffer_size);

         n_received += buffer_size;
         return buffer_size;
      }

      /* If udp_buffer has not enough space, send it */
      udp_header->serial_number = UDP_FIRST | n_received;
      udp_header->sequence_number = ++serial_number;

      status = send(sock, udp_buffer, n_received + sizeof(UDP_HEADER), flags);

      n_received = 0;

      memcpy(udp_header + 1, buffer, buffer_size);
      n_received = buffer_size;

      return buffer_size;
   }

   /*
      If buffer size is larger than UDP size, split event in several
      buffers.
    */

   /* If there is any data already in the buffer, send it first */
   if (n_received) {
      udp_header->serial_number = UDP_FIRST | n_received;
      udp_header->sequence_number = ++serial_number;

      send(sock, udp_buffer, n_received + sizeof(UDP_HEADER), flags);
      n_received = 0;
   }

   for (i = 0; i < ((buffer_size - 1) / data_size); i++) {
      if (i == 0) {
         udp_header->serial_number = UDP_FIRST | buffer_size;
         udp_header->sequence_number = ++serial_number;
      } else {
         udp_header->serial_number = serial_number;
         udp_header->sequence_number = i;
      }

      memcpy(udp_header + 1, buffer + i * data_size, data_size);
      send(sock, udp_buffer, NET_UDP_SIZE, flags);
   }

   /* Send remaining bytes */
   udp_header->serial_number = serial_number;
   udp_header->sequence_number = i;
   memcpy(udp_header + 1, buffer + i * data_size, buffer_size - i * data_size);
   status =
       send(sock, udp_buffer, sizeof(UDP_HEADER) + buffer_size - i * data_size, flags);
   if ((DWORD) status == sizeof(UDP_HEADER) + buffer_size - i * data_size)
      return buffer_size;

   return status;
}

/*------------------------------------------------------------------*/
INT recv_udp(int sock, char *buffer, DWORD buffer_size, INT flags)
/********************************************************************\

  Routine: recv_udp

  Purpose: Receive network data over UDP port. If received event
     is splitted into several buffers, recombine them checking
     the serial number. If one buffer is missing in a splitted
     event, throw away the whole event.

  Input:
    INT   sock               Socket which was previosly opened.
    DWORD buffer_size        Size of the buffer in bytes.
    INT   flags              Flags passed to recv()

  Output:
    char  *buffer            Network receive buffer.

  Function value:
    INT                     Same as recv()

\********************************************************************/
{
   INT i, status;
   UDP_HEADER *udp_header;
   char udp_buffer[NET_UDP_SIZE];
   DWORD serial_number, sequence_number, total_buffer_size;
   DWORD data_size, n_received;
   fd_set readfds;
   struct timeval timeout;

   udp_header = (UDP_HEADER *) udp_buffer;
   data_size = NET_UDP_SIZE - sizeof(UDP_HEADER);

   /* Receive the first buffer */
#ifdef OS_UNIX
   do {
      i = recv(sock, udp_buffer, NET_UDP_SIZE, flags);

      /* dont return if an alarm signal was cought */
   } while (i == -1 && errno == EINTR);
#else
   i = recv(sock, udp_buffer, NET_UDP_SIZE, flags);
#endif

 start:

   /* Receive buffers until we get a sequence start */
   while (!(udp_header->serial_number & UDP_FIRST)) {
      /* wait for data with timeout */
      FD_ZERO(&readfds);
      FD_SET(sock, &readfds);

      timeout.tv_sec = 0;
      timeout.tv_usec = 100000; /* 0.1 s */

      do {
         status = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
      } while (status == -1);

      /*
         If we got nothing, return zero so that calling program can do
         other things like checking TCP port for example.
       */
      if (!FD_ISSET(sock, &readfds))
         return 0;

#ifdef OS_UNIX
      do {
         i = recv(sock, udp_buffer, NET_UDP_SIZE, flags);

         /* dont return if an alarm signal was caught */
      } while (i == -1 && errno == EINTR);
#else
      i = recv(sock, udp_buffer, NET_UDP_SIZE, flags);
#endif
   }

   /* if no others are following, return */
   total_buffer_size = udp_header->serial_number & ~UDP_FIRST;
   serial_number = udp_header->sequence_number;
   sequence_number = 0;

   if (total_buffer_size <= data_size) {
      if (buffer_size < total_buffer_size) {
         memcpy(buffer, udp_header + 1, buffer_size);
         return buffer_size;
      } else {
         memcpy(buffer, udp_header + 1, total_buffer_size);
         return total_buffer_size;
      }
   }

   /* if others are following, collect them */
   n_received = data_size;

   if (buffer_size < data_size) {
      memcpy(buffer, udp_header + 1, buffer_size);
      return buffer_size;
   }

   memcpy(buffer, udp_header + 1, data_size);


   do {
      /* wait for new data with timeout */
      FD_ZERO(&readfds);
      FD_SET(sock, &readfds);

      timeout.tv_sec = 0;
      timeout.tv_usec = 100000; /* 0.1 s */

      do {
         status = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
      } while (status == -1);

      /*
         If we got nothing, return zero so that calling program can do
         other things like checking TCP port for example.
       */
      if (!FD_ISSET(sock, &readfds))
         return 0;

#ifdef OS_UNIX
      do {
         i = recv(sock, udp_buffer, NET_UDP_SIZE, flags);

         /* dont return if an alarm signal was caught */
      } while (i == -1 && errno == EINTR);
#else
      i = recv(sock, udp_buffer, NET_UDP_SIZE, flags);
#endif

      sequence_number++;

      /* check sequence and serial numbers */
      if (udp_header->serial_number != serial_number ||
          udp_header->sequence_number != sequence_number)
         /* lost one, so start again */
         goto start;

      /* copy what we got */
      memcpy(buffer + n_received, udp_header + 1, i - sizeof(UDP_HEADER));

      n_received += (i - sizeof(UDP_HEADER));

   } while (n_received < total_buffer_size);

   return n_received;
}

/*------------------------------------------------------------------*/

#ifdef OS_MSDOS
#ifdef sopen
/********************************************************************\
   under Turbo-C, sopen is defined as a macro instead a function.
   Since the PCTCP library uses sopen as a function call, we supply
   it here.
\********************************************************************/

#undef sopen

int sopen(const char *path, int access, int shflag, int mode)
{
   return open(path, (access) | (shflag), mode);
}

#endif
#endif

/*------------------------------------------------------------------*/
/********************************************************************\
*                                                                    *
*                     Tape functions                                 *
*                                                                    *
\********************************************************************/

/*------------------------------------------------------------------*/
INT ss_tape_open(char *path, INT oflag, INT * channel)
/********************************************************************\

  Routine: ss_tape_open

  Purpose: Open tape channel

  Input:
    char  *path             Name of tape
                            Under Windows NT, usually \\.\tape0
                            Under UNIX, usually /dev/tape
    INT   oflag             Open flags, same as open()

  Output:
    INT   *channel          Channel identifier

  Function value:
    SS_SUCCESS              Successful completion
    SS_NO_TAPE              No tape in device
    SS_DEV_BUSY             Device is used by someone else

\********************************************************************/
{
#ifdef OS_UNIX
   struct mtop arg;

   cm_enable_watchdog(FALSE);

   *channel = open(path, oflag, 0644);

   cm_enable_watchdog(TRUE);

   if (*channel < 0)
      cm_msg(MERROR, "ss_tape_open", strerror(errno));

   if (*channel < 0) {
      if (errno == EIO)
         return SS_NO_TAPE;
      if (errno == EBUSY)
         return SS_DEV_BUSY;
      return errno;
   }
#ifdef MTSETBLK
   /* set variable block size */
   arg.mt_op = MTSETBLK;
   arg.mt_count = 0;

   ioctl(*channel, MTIOCTOP, &arg);
#endif                          /* MTSETBLK */

#endif                          /* OS_UNIX */

#ifdef OS_WINNT
   INT status;
   TAPE_GET_MEDIA_PARAMETERS m;

   *channel = (INT) CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0,
                               0, OPEN_EXISTING, 0, NULL);

   if (*channel == (INT) INVALID_HANDLE_VALUE) {
      status = GetLastError();
      if (status == ERROR_SHARING_VIOLATION) {
         cm_msg(MERROR, "ss_tape_open", "tape is used by other process");
         return SS_DEV_BUSY;
      }
      if (status == ERROR_FILE_NOT_FOUND) {
         cm_msg(MERROR, "ss_tape_open", "tape device \"%s\" doesn't exist", path);
         return SS_NO_TAPE;
      }

      cm_msg(MERROR, "ss_tape_open", "unknown error %d", status);
      return status;
   }

   status = GetTapeStatus((HANDLE) (*channel));
   if (status == ERROR_NO_MEDIA_IN_DRIVE || status == ERROR_BUS_RESET) {
      cm_msg(MERROR, "ss_tape_open", "no media in drive");
      return SS_NO_TAPE;
   }

   /* set block size */
   memset(&m, 0, sizeof(m));
   m.BlockSize = TAPE_BUFFER_SIZE;
   SetTapeParameters((HANDLE) (*channel), SET_TAPE_MEDIA_INFORMATION, &m);

#endif

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_tape_close(INT channel)
/********************************************************************\

  Routine: ss_tape_close

  Purpose: Close tape channel

  Input:
    INT   channel           Channel identifier

  Output:
    <none>

  Function value:
    SS_SUCCESS              Successful completion
    errno                   Low level error number

\********************************************************************/
{
   INT status;

#ifdef OS_UNIX

   status = close(channel);

   if (status < 0) {
      cm_msg(MERROR, "ss_tape_close", strerror(errno));
      return errno;
   }
#endif                          /* OS_UNIX */

#ifdef OS_WINNT

   if (!CloseHandle((HANDLE) channel)) {
      status = GetLastError();
      cm_msg(MERROR, "ss_tape_close", "unknown error %d", status);
      return status;
   }
#endif                          /* OS_WINNT */

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_tape_status(char *path)
/********************************************************************\

  Routine: ss_tape_status

  Purpose: Print status information about tape

  Input:
    char  *path             Name of tape

  Output:
    <print>                 Tape information

  Function value:
    SS_SUCCESS              Successful completion

\********************************************************************/
{
#ifdef OS_UNIX
   char str[256];
   /* let 'mt' do the job */
   sprintf(str, "mt -f %s status", path);
   system(str);
#endif                          /* OS_UNIX */

#ifdef OS_WINNT
   INT status, channel;
   DWORD size;
   TAPE_GET_MEDIA_PARAMETERS m;
   TAPE_GET_DRIVE_PARAMETERS d;
   double x;

   channel = (INT) CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0,
                              0, OPEN_EXISTING, 0, NULL);

   if (channel == (INT) INVALID_HANDLE_VALUE) {
      status = GetLastError();
      if (status == ERROR_SHARING_VIOLATION) {
         cm_msg(MINFO, "ss_tape_status", "tape is used by other process");
         return SS_SUCCESS;
      }
      if (status == ERROR_FILE_NOT_FOUND) {
         cm_msg(MINFO, "ss_tape_status", "tape device \"%s\" doesn't exist", path);
         return SS_SUCCESS;
      }

      cm_msg(MINFO, "ss_tape_status", "unknown error %d", status);
      return status;
   }

   /* poll media changed messages */
   GetTapeParameters((HANDLE) channel, GET_TAPE_DRIVE_INFORMATION, &size, &d);
   GetTapeParameters((HANDLE) channel, GET_TAPE_DRIVE_INFORMATION, &size, &d);

   status = GetTapeStatus((HANDLE) channel);
   if (status == ERROR_NO_MEDIA_IN_DRIVE || status == ERROR_BUS_RESET) {
      cm_msg(MINFO, "ss_tape_status", "no media in drive");
      CloseHandle((HANDLE) channel);
      return SS_SUCCESS;
   }

   GetTapeParameters((HANDLE) channel, GET_TAPE_DRIVE_INFORMATION, &size, &d);
   GetTapeParameters((HANDLE) channel, GET_TAPE_MEDIA_INFORMATION, &size, &m);

   printf("Hardware error correction is %s\n", d.ECC ? "on" : "off");
   printf("Hardware compression is %s\n", d.Compression ? "on" : "off");
   printf("Tape %s write protected\n", m.WriteProtected ? "is" : "is not");

   if (d.FeaturesLow & TAPE_DRIVE_TAPE_REMAINING) {
      x = ((double) m.Remaining.LowPart + (double) m.Remaining.HighPart * 4.294967295E9)
          / 1024.0 / 1000.0;
      printf("Tape capacity remaining is %d MB\n", (int) x);
   } else
      printf("Tape capacity is not reported by tape\n");

   CloseHandle((HANDLE) channel);

#endif

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_tape_write(INT channel, void *pdata, INT count)
/********************************************************************\

  Routine: ss_tape_write

  Purpose: Write count bytes to tape channel

  Input:
    INT   channel           Channel identifier
    void  *pdata            Address of data to write
    INT   count             number of bytes

  Output:
    <none>

  Function value:
    SS_SUCCESS              Successful completion
    SS_IO_ERROR             Physical IO error
    SS_TAPE_ERROR           Unknown tape error

\********************************************************************/
{
#ifdef OS_UNIX
   INT status;

   do {
      status = write(channel, pdata, count);
/*
    if (status != count)
      printf("count: %d - %d\n", count, status);
*/
   } while (status == -1 && errno == EINTR);

   if (status != count) {
      cm_msg(MERROR, "ss_tape_write", strerror(errno));

      if (errno == EIO)
         return SS_IO_ERROR;
      else
         return SS_TAPE_ERROR;
   }
#endif                          /* OS_UNIX */

#ifdef OS_WINNT
   INT status;
   DWORD written;

   WriteFile((HANDLE) channel, pdata, count, &written, NULL);
   if (written != (DWORD) count) {
      status = GetLastError();
      cm_msg(MERROR, "ss_tape_write", "error %d", status);

      return SS_IO_ERROR;
   }
#endif                          /* OS_WINNT */

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_tape_read(INT channel, void *pdata, INT * count)
/********************************************************************\

  Routine: ss_tape_write

  Purpose: Read count bytes to tape channel

  Input:
    INT   channel           Channel identifier
    void  *pdata            Address of data
    INT   *count            Number of bytes to read

  Output:
    INT   *count            Number of read

  Function value:
    SS_SUCCESS              Successful operation
    <errno>                 Error code

\********************************************************************/
{
#ifdef OS_UNIX
   INT n, status;

   do {
      n = read(channel, pdata, *count);
   } while (n == -1 && errno == EINTR);

   if (n == -1) {
      if (errno == ENOSPC || errno == EIO)
         status = SS_END_OF_TAPE;
      else {
         if (n == 0 && errno == 0)
            status = SS_END_OF_FILE;
         else {
            cm_msg(MERROR, "ss_tape_read",
                   "unexpected tape error: n=%d, errno=%d\n", n, errno);
            status = errno;
         }
      }
   } else
      status = SS_SUCCESS;
   *count = n;

   return status;

#elif defined(OS_WINNT)         /* OS_UNIX */

   INT status;
   DWORD read;

   if (!ReadFile((HANDLE) channel, pdata, *count, &read, NULL)) {
      status = GetLastError();
      if (status == ERROR_NO_DATA_DETECTED)
         status = SS_END_OF_TAPE;
      else if (status == ERROR_FILEMARK_DETECTED)
         status = SS_END_OF_FILE;
      else if (status == ERROR_MORE_DATA)
         status = SS_SUCCESS;
      else
         cm_msg(MERROR, "ss_tape_read",
                "unexpected tape error: n=%d, errno=%d\n", read, status);
   } else
      status = SS_SUCCESS;

   *count = read;
   return status;

#else                           /* OS_WINNT */

   return SS_SUCCESS;

#endif
}

/*------------------------------------------------------------------*/
INT ss_tape_write_eof(INT channel)
/********************************************************************\

  Routine: ss_tape_write_eof

  Purpose: Write end-of-file to tape channel

  Input:
    INT   *channel          Channel identifier

  Output:
    <none>

  Function value:
    SS_SUCCESS              Successful completion
    errno                   Error number

\********************************************************************/
{
   INT status;

#ifdef OS_UNIX
   struct mtop arg;

   arg.mt_op = MTWEOF;
   arg.mt_count = 1;

   cm_enable_watchdog(FALSE);

   status = ioctl(channel, MTIOCTOP, &arg);

   cm_enable_watchdog(TRUE);

   if (status < 0) {
      cm_msg(MERROR, "ss_tape_write_eof", strerror(errno));
      return errno;
   }
#endif                          /* OS_UNIX */

#ifdef OS_WINNT

   TAPE_GET_DRIVE_PARAMETERS d;
   DWORD size;

   size = sizeof(TAPE_GET_DRIVE_PARAMETERS);
   GetTapeParameters((HANDLE) channel, GET_TAPE_DRIVE_INFORMATION, &size, &d);

   if (d.FeaturesHigh & TAPE_DRIVE_WRITE_FILEMARKS)
      status = WriteTapemark((HANDLE) channel, TAPE_FILEMARKS, 1, FALSE);
   else if (d.FeaturesHigh & TAPE_DRIVE_WRITE_LONG_FMKS)
      status = WriteTapemark((HANDLE) channel, TAPE_LONG_FILEMARKS, 1, FALSE);
   else if (d.FeaturesHigh & TAPE_DRIVE_WRITE_SHORT_FMKS)
      status = WriteTapemark((HANDLE) channel, TAPE_SHORT_FILEMARKS, 1, FALSE);
   else
      cm_msg(MERROR, "ss_tape_write_eof", "tape doesn't support writing of filemarks");

   if (status != NO_ERROR) {
      cm_msg(MERROR, "ss_tape_write_eof", "unknown error %d", status);
      return status;
   }
#endif                          /* OS_WINNT */

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_tape_fskip(INT channel, INT count)
/********************************************************************\

  Routine: ss_tape_fskip

  Purpose: Skip count number of files on a tape

  Input:
    INT   *channel          Channel identifier
    INT   count             Number of files to skip

  Output:
    <none>

  Function value:
    SS_SUCCESS              Successful completion
    errno                   Error number

\********************************************************************/
{
   INT status;

#ifdef OS_UNIX
   struct mtop arg;

   if (count > 0)
      arg.mt_op = MTFSF;
   else
      arg.mt_op = MTBSF;
   arg.mt_count = abs(count);

   cm_enable_watchdog(FALSE);

   status = ioctl(channel, MTIOCTOP, &arg);

   cm_enable_watchdog(TRUE);

   if (status < 0) {
      cm_msg(MERROR, "ss_tape_fskip", strerror(errno));
      return errno;
   }
#endif                          /* OS_UNIX */

#ifdef OS_WINNT

   status = SetTapePosition((HANDLE) channel, TAPE_SPACE_FILEMARKS, 0,
                            (DWORD) count, 0, FALSE);

   if (status == ERROR_END_OF_MEDIA)
      return SS_END_OF_TAPE;

   if (status != NO_ERROR) {
      cm_msg(MERROR, "ss_tape_fskip", "error %d", status);
      return status;
   }
#endif                          /* OS_WINNT */

   return SS_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_tape_rskip(INT channel, INT count)
/********************************************************************\

  Routine: ss_tape_rskip

  Purpose: Skip count number of records on a tape

  Input:
    INT   *channel          Channel identifier
    INT   count             Number of records to skip

  Output:
    <none>

  Function value:
    SS_SUCCESS              Successful completion
    errno                   Error number

\********************************************************************/
{
   INT status;

#ifdef OS_UNIX
   struct mtop arg;

   if (count > 0)
      arg.mt_op = MTFSR;
   else
      arg.mt_op = MTBSR;
   arg.mt_count = abs(count);

   cm_enable_watchdog(FALSE);

   status = ioctl(channel, MTIOCTOP, &arg);

   cm_enable_watchdog(TRUE);

   if (status < 0) {
      cm_msg(MERROR, "ss_tape_rskip", strerror(errno));
      return errno;
   }
#endif                          /* OS_UNIX */

#ifdef OS_WINNT

   status =
       SetTapePosition((HANDLE) channel, TAPE_SPACE_RELATIVE_BLOCKS, 0,
                       (DWORD) count, 0, FALSE);
   if (status != NO_ERROR) {
      cm_msg(MERROR, "ss_tape_rskip", "error %d", status);
      return status;
   }
#endif                          /* OS_WINNT */

   return CM_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_tape_rewind(INT channel)
/********************************************************************\

  Routine: ss_tape_rewind

  Purpose: Rewind tape

  Input:
    INT   channel           Channel identifier

  Output:
    <none>

  Function value:
    SS_SUCCESS              Successful completion
    errno                   Error number

\********************************************************************/
{
   INT status;

#ifdef OS_UNIX
   struct mtop arg;

   arg.mt_op = MTREW;
   arg.mt_count = 0;

   cm_enable_watchdog(FALSE);

   status = ioctl(channel, MTIOCTOP, &arg);

   cm_enable_watchdog(TRUE);

   if (status < 0) {
      cm_msg(MERROR, "ss_tape_rewind", strerror(errno));
      return errno;
   }
#endif                          /* OS_UNIX */

#ifdef OS_WINNT

   status = SetTapePosition((HANDLE) channel, TAPE_REWIND, 0, 0, 0, FALSE);
   if (status != NO_ERROR) {
      cm_msg(MERROR, "ss_tape_rewind", "error %d", status);
      return status;
   }
#endif                          /* OS_WINNT */

   return CM_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_tape_spool(INT channel)
/********************************************************************\

  Routine: ss_tape_spool

  Purpose: Spool tape forward to end of recorded data

  Input:
    INT   channel           Channel identifier

  Output:
    <none>

  Function value:
    SS_SUCCESS              Successful completion
    errno                   Error number

\********************************************************************/
{
   INT status;

#ifdef OS_UNIX
   struct mtop arg;

#ifdef MTEOM
   arg.mt_op = MTEOM;
#else
   arg.mt_op = MTSEOD;
#endif
   arg.mt_count = 0;

   cm_enable_watchdog(FALSE);

   status = ioctl(channel, MTIOCTOP, &arg);

   cm_enable_watchdog(TRUE);

   if (status < 0) {
      cm_msg(MERROR, "ss_tape_rewind", strerror(errno));
      return errno;
   }
#endif                          /* OS_UNIX */

#ifdef OS_WINNT

   status = SetTapePosition((HANDLE) channel, TAPE_SPACE_END_OF_DATA, 0, 0, 0, FALSE);
   if (status != NO_ERROR) {
      cm_msg(MERROR, "ss_tape_spool", "error %d", status);
      return status;
   }
#endif                          /* OS_WINNT */

   return CM_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_tape_mount(INT channel)
/********************************************************************\

  Routine: ss_tape_mount

  Purpose: Mount tape

  Input:
    INT   channel           Channel identifier

  Output:
    <none>

  Function value:
    SS_SUCCESS              Successful completion
    errno                   Error number

\********************************************************************/
{
   INT status;

#ifdef OS_UNIX
   struct mtop arg;

#ifdef MTLOAD
   arg.mt_op = MTLOAD;
#else
   arg.mt_op = MTNOP;
#endif
   arg.mt_count = 0;

   cm_enable_watchdog(FALSE);

   status = ioctl(channel, MTIOCTOP, &arg);

   cm_enable_watchdog(TRUE);

   if (status < 0) {
      cm_msg(MERROR, "ss_tape_mount", strerror(errno));
      return errno;
   }
#endif                          /* OS_UNIX */

#ifdef OS_WINNT

   status = PrepareTape((HANDLE) channel, TAPE_LOAD, FALSE);
   if (status != NO_ERROR) {
      cm_msg(MERROR, "ss_tape_mount", "error %d", status);
      return status;
   }
#endif                          /* OS_WINNT */

   return CM_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_tape_unmount(INT channel)
/********************************************************************\

  Routine: ss_tape_unmount

  Purpose: Unmount tape

  Input:
    INT   channel           Channel identifier

  Output:
    <none>

  Function value:
    SS_SUCCESS              Successful completion
    errno                   Error number

\********************************************************************/
{
   INT status;

#ifdef OS_UNIX
   struct mtop arg;

#ifdef MTOFFL
   arg.mt_op = MTOFFL;
#else
   arg.mt_op = MTUNLOAD;
#endif
   arg.mt_count = 0;

   cm_enable_watchdog(FALSE);

   status = ioctl(channel, MTIOCTOP, &arg);

   cm_enable_watchdog(TRUE);

   if (status < 0) {
      cm_msg(MERROR, "ss_tape_unmount", strerror(errno));
      return errno;
   }
#endif                          /* OS_UNIX */

#ifdef OS_WINNT

   status = PrepareTape((HANDLE) channel, TAPE_UNLOAD, FALSE);
   if (status != NO_ERROR) {
      cm_msg(MERROR, "ss_tape_unmount", "error %d", status);
      return status;
   }
#endif                          /* OS_WINNT */

   return CM_SUCCESS;
}

/*------------------------------------------------------------------*/
INT ss_tape_get_blockn(INT channel)
/********************************************************************\
Routine: ss_tape_get_blockn
Purpose: Ask the tape channel for the present block number
Input:
INT   *channel          Channel identifier
Function value:
blockn:  >0 = block number, =0 option not available, <0 errno
\********************************************************************/
{
  return 0;

#if defined(OS_DARWIN)

   return 0;

#elif defined(OS_UNIX)

   INT status;
   struct mtpos arg;

   cm_enable_watchdog(FALSE);
   status = ioctl(channel, MTIOCPOS, &arg);
   cm_enable_watchdog(TRUE);
   if (status < 0) {
      if (errno == EIO)
         return 0;
      else {
         cm_msg(MERROR, "ss_tape_get_blockn", strerror(errno));
         return -errno;
      }
   }
   return (arg.mt_blkno);

#elif defined(OS_WINNT)

   INT status;
   TAPE_GET_MEDIA_PARAMETERS media;
   unsigned long size;
   /* I'm not sure the partition count corresponds to the block count */
   status =
       GetTapeParameters((HANDLE) channel, GET_TAPE_MEDIA_INFORMATION, &size, &media);
   return (media.PartitionCount);

#endif
}


/*------------------------------------------------------------------*/
/********************************************************************\
*                                                                    *
*                     Disk functions                                 *
*                                                                    *
\********************************************************************/

/*------------------------------------------------------------------*/
double ss_disk_free(char *path)
/********************************************************************\

  Routine: ss_disk_free

  Purpose: Return free disk space

  Input:
    char  *path             Name of a file in file system to check

  Output:

  Function value:
    doube                   Number of bytes free on disk

\********************************************************************/
{
#ifdef OS_UNIX
#if defined(OS_OSF1)
   struct statfs st;
   statfs(path, &st, sizeof(st));
   return (double) st.f_bavail * st.f_bsize;
#elif defined(OS_LINUX)
   struct statfs st;
   statfs(path, &st);
   return (double) st.f_bavail * st.f_bsize;
#elif defined(OS_SOLARIS)
   struct statvfs st;
   statvfs(path, &st);
   return (double) st.f_bavail * st.f_bsize;
#elif defined(OS_IRIX)
   struct statfs st;
   statfs(path, &st, sizeof(struct statfs), 0);
   return (double) st.f_bfree * st.f_bsize;
#else
   struct fs_data st;
   statfs(path, &st);
   return (double) st.fd_otsize * st.fd_bfree;
#endif

#elif defined(OS_WINNT)         /* OS_UNIX */
   DWORD SectorsPerCluster;
   DWORD BytesPerSector;
   DWORD NumberOfFreeClusters;
   DWORD TotalNumberOfClusters;
   char str[80];

   strcpy(str, path);
   if (strchr(str, ':') != NULL) {
      *(strchr(str, ':') + 1) = 0;
      strcat(str, DIR_SEPARATOR_STR);
      GetDiskFreeSpace(str, &SectorsPerCluster, &BytesPerSector,
                       &NumberOfFreeClusters, &TotalNumberOfClusters);
   } else
      GetDiskFreeSpace(NULL, &SectorsPerCluster, &BytesPerSector,
                       &NumberOfFreeClusters, &TotalNumberOfClusters);

   return (double) NumberOfFreeClusters *SectorsPerCluster * BytesPerSector;
#else                           /* OS_WINNT */

   return 1e9;

#endif
}

#if defined(OS_ULTRIX) || defined(OS_WINNT)
int fnmatch(const char *pat, const char *str, const int flag)
{
   while (*str != '\0') {
      if (*pat == '*') {
         pat++;
         if ((str = strchr(str, *pat)) == NULL)
            return -1;
      }
      if (*pat == *str) {
         pat++;
         str++;
      } else
         return -1;
   }
   if (*pat == '\0')
      return 0;
   else
      return -1;
}
#endif

#ifdef OS_WINNT
HANDLE pffile;
LPWIN32_FIND_DATA lpfdata;
#endif
INT ss_file_find(char *path, char *pattern, char **plist)
/********************************************************************\

  Routine: ss_file_find

  Purpose: Return list of files matching 'pattern' from the 'path' location

  Input:
    char  *path             Name of a file in file system to check
    char  *pattern          pattern string (wildcard allowed)

  Output:
    char  **plist           pointer to the lfile list

  Function value:
    int                     Number of files matching request

\********************************************************************/
{
   int i;
#ifdef OS_UNIX
   DIR *dir_pointer;
   struct dirent *dp;

   if ((dir_pointer = opendir(path)) == NULL)
      return 0;
   *plist = (char *) malloc(MAX_STRING_LENGTH);
   i = 0;
   for (dp = readdir(dir_pointer); dp != NULL; dp = readdir(dir_pointer)) {
      if (fnmatch(pattern, dp->d_name, 0) == 0) {
         *plist = (char *) realloc(*plist, (i + 1) * MAX_STRING_LENGTH);
         strncpy(*plist + (i * MAX_STRING_LENGTH), dp->d_name, strlen(dp->d_name));
         *(*plist + (i * MAX_STRING_LENGTH) + strlen(dp->d_name)) = '\0';
         i++;
         seekdir(dir_pointer, telldir(dir_pointer));
      }
   }
   closedir(dir_pointer);
#endif
#ifdef OS_WINNT
   char str[255];
   int first;

   strcpy(str, path);
   strcat(str, "\\");
   strcat(str, pattern);
   first = 1;
   i = 0;
   lpfdata = (WIN32_FIND_DATA *) malloc(sizeof(WIN32_FIND_DATA));
   *plist = (char *) malloc(MAX_STRING_LENGTH);
   pffile = FindFirstFile(str, lpfdata);
   if (pffile == INVALID_HANDLE_VALUE)
      return 0;
   first = 0;
   *plist = (char *) realloc(*plist, (i + 1) * MAX_STRING_LENGTH);
   strncpy(*plist + (i * MAX_STRING_LENGTH), lpfdata->cFileName,
           strlen(lpfdata->cFileName));
   *(*plist + (i * MAX_STRING_LENGTH) + strlen(lpfdata->cFileName)) = '\0';
   i++;
   while (FindNextFile(pffile, lpfdata)) {
      *plist = (char *) realloc(*plist, (i + 1) * MAX_STRING_LENGTH);
      strncpy(*plist + (i * MAX_STRING_LENGTH), lpfdata->cFileName,
              strlen(lpfdata->cFileName));
      *(*plist + (i * MAX_STRING_LENGTH) + strlen(lpfdata->cFileName)) = '\0';
      i++;
   }
   free(lpfdata);
#endif
   return i;
}

INT ss_file_remove(char *path)
/********************************************************************\

  Routine: ss_file_remove

  Purpose: remove (delete) file given through the path

  Input:
    char  *path             Name of a file in file system to check

  Output:

  Function value:
    int                     function error 0= ok, -1 check errno

\********************************************************************/
{
   return remove(path);
}

double ss_file_size(char *path)
/********************************************************************\

  Routine: ss_file_size

  Purpose: Return file size in bytes for the given path

  Input:
    char  *path             Name of a file in file system to check

  Output:

  Function value:
    double                     File size

\********************************************************************/
{
   struct stat stat_buf;

   /* allocate buffer with file size */
   stat(path, &stat_buf);
   return (double) stat_buf.st_size;
}

double ss_disk_size(char *path)
/********************************************************************\

  Routine: ss_disk_size

  Purpose: Return full disk space

  Input:
    char  *path             Name of a file in file system to check

  Output:

  Function value:
    doube                   Number of bytes free on disk

\********************************************************************/
{
#ifdef OS_UNIX
#if defined(OS_OSF1)
   struct statfs st;
   statfs(path, &st, sizeof(st));
   return (double) st.f_blocks * st.f_fsize;
#elif defined(OS_LINUX)
   struct statfs st;
   statfs(path, &st);
   return (double) st.f_blocks * st.f_bsize;
#elif defined(OS_SOLARIS)
   struct statvfs st;
   statvfs(path, &st);
   if (st.f_frsize > 0)
      return (double) st.f_blocks * st.f_frsize;
   else
      return (double) st.f_blocks * st.f_bsize;
#elif defined(OS_ULTRIX)
   struct fs_data st;
   statfs(path, &st);
   return (double) st.fd_btot * 1024;
#elif defined(OS_IRIX)
   struct statfs st;
   statfs(path, &st, sizeof(struct statfs), 0);
   return (double) st.f_blocks * st.f_bsize;
#else
#error ss_disk_size not defined for this OS
#endif
#endif                          /* OS_UNIX */

#ifdef OS_WINNT
   DWORD SectorsPerCluster;
   DWORD BytesPerSector;
   DWORD NumberOfFreeClusters;
   DWORD TotalNumberOfClusters;
   char str[80];

   strcpy(str, path);
   if (strchr(str, ':') != NULL) {
      *(strchr(str, ':') + 1) = 0;
      strcat(str, DIR_SEPARATOR_STR);
      GetDiskFreeSpace(str, &SectorsPerCluster, &BytesPerSector,
                       &NumberOfFreeClusters, &TotalNumberOfClusters);
   } else
      GetDiskFreeSpace(NULL, &SectorsPerCluster, &BytesPerSector,
                       &NumberOfFreeClusters, &TotalNumberOfClusters);

   return (double) TotalNumberOfClusters *SectorsPerCluster * BytesPerSector;
#endif                          /* OS_WINNT */

   return 1e9;
}

/*------------------------------------------------------------------*/
/********************************************************************\
*                                                                    *
*                  Screen  functions                                 *
*                                                                    *
\********************************************************************/

/*------------------------------------------------------------------*/
void ss_clear_screen()
/********************************************************************\

  Routine: ss_clear_screen

  Purpose: Clear the screen

  Input:
    <none>

  Output:
    <none>

  Function value:
    <none>

\********************************************************************/
{
#ifdef OS_WINNT

   HANDLE hConsole;
   COORD coordScreen = { 0, 0 };        /* here's where we'll home the cursor */
   BOOL bSuccess;
   DWORD cCharsWritten;
   CONSOLE_SCREEN_BUFFER_INFO csbi;     /* to get buffer info */
   DWORD dwConSize;             /* number of character cells in the current buffer */

   hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

   /* get the number of character cells in the current buffer */
   bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
   dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

   /* fill the entire screen with blanks */
   bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',
                                         dwConSize, coordScreen, &cCharsWritten);

   /* put the cursor at (0, 0) */
   bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
   return;

#endif                          /* OS_WINNT */
#if defined(OS_UNIX) || defined(OS_VXWORKS) || defined(OS_VMS)
   printf("\033[2J");
#endif
#ifdef OS_MSDOS
   clrscr();
#endif
}

/*------------------------------------------------------------------*/
void ss_set_screen_size(int x, int y)
/********************************************************************\

  Routine: ss_set_screen_size

  Purpose: Set the screen size in character cells

  Input:
    <none>

  Output:
    <none>

  Function value:
    <none>

\********************************************************************/
{
#ifdef OS_WINNT

   HANDLE hConsole;
   COORD coordSize;

   coordSize.X = (short) x;
   coordSize.Y = (short) y;
   hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
   SetConsoleScreenBufferSize(hConsole, coordSize);

#endif                          /* OS_WINNT */
}

/*------------------------------------------------------------------*/
void ss_printf(INT x, INT y, const char *format, ...)
/********************************************************************\

  Routine: ss_printf

  Purpose: Print string at given cursor position

  Input:
    INT   x,y               Cursor position, starting from zero,
          x=0 and y=0 left upper corner

    char  *format           Format string for printf
    ...                     Arguments for printf

  Output:
    <none>

  Function value:
    <none>

\********************************************************************/
{
   char str[256];
   va_list argptr;

   va_start(argptr, format);
   vsprintf(str, (char *) format, argptr);
   va_end(argptr);

#ifdef OS_WINNT
   {
      HANDLE hConsole;
      COORD dwWriteCoord;
      DWORD cCharsWritten;

      hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

      dwWriteCoord.X = (short) x;
      dwWriteCoord.Y = (short) y;

      WriteConsoleOutputCharacter(hConsole, str, strlen(str),
                                  dwWriteCoord, &cCharsWritten);
   }

#endif                          /* OS_WINNT */

#if defined(OS_UNIX) || defined(OS_VXWORKS) || defined(OS_VMS)
   printf("\033[%1d;%1d;H", y + 1, x + 1);
   printf(str);
   fflush(stdout);
#endif

#ifdef OS_MSDOS
   gotoxy(x + 1, y + 1);
   cputs(str);
#endif
}

/*------------------------------------------------------------------*/
char *ss_getpass(char *prompt)
/********************************************************************\

  Routine: ss_getpass

  Purpose: Read password without echoing it at the screen

  Input:
    char   *prompt    Prompt string

  Output:
    <none>

  Function value:
    char*             Pointer to password

\********************************************************************/
{
   static char password[32];

   printf(prompt);
   memset(password, 0, sizeof(password));

#ifdef OS_UNIX
   return (char *) getpass("");
#elif defined(OS_WINNT)
   {
      HANDLE hConsole;
      DWORD nCharsRead;

      hConsole = GetStdHandle(STD_INPUT_HANDLE);
      SetConsoleMode(hConsole, ENABLE_LINE_INPUT);
      ReadConsole(hConsole, password, sizeof(password), &nCharsRead, NULL);
      SetConsoleMode(hConsole, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT |
                     ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
      printf("\n");

      if (password[strlen(password) - 1] == '\r')
         password[strlen(password) - 1] = 0;

      return password;
   }
#elif defined(OS_MSDOS)
   {
      char c, *ptr;

      ptr = password;
      while ((c = getchar()) != EOF && c != '\n')
         *ptr++ = c;
      *ptr = 0;

      printf("\n");
      return password;
   }
#else
   {
      ss_gets(password, 32);
      return password;
   }
#endif
}

/*------------------------------------------------------------------*/
INT ss_getchar(BOOL reset)
/********************************************************************\

  Routine: ss_getchar

  Purpose: Read a single character

  Input:
    BOOL   reset            Reset terminal to standard mode

  Output:
    <none>

  Function value:
    int             0       for no character available
                    CH_xxs  for special character
                    n       ASCII code for normal character
                    -1      function not available on this OS

\********************************************************************/
{
#ifdef OS_UNIX

   static BOOL init = FALSE;
   static struct termios save_termios;
   struct termios buf;
   int i, fd;
   char c[3];

   if (_daemon_flag)
      return 0;

   fd = fileno(stdin);

   if (reset) {
      if (init)
         tcsetattr(fd, TCSAFLUSH, &save_termios);
      init = FALSE;
      return 0;
   }

   if (!init) {
      tcgetattr(fd, &save_termios);
      memcpy(&buf, &save_termios, sizeof(buf));

      buf.c_lflag &= ~(ECHO | ICANON | IEXTEN);

      buf.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON);

      buf.c_cflag &= ~(CSIZE | PARENB);
      buf.c_cflag |= CS8;
      /* buf.c_oflag &= ~(OPOST); */
      buf.c_cc[VMIN] = 0;
      buf.c_cc[VTIME] = 0;

      tcsetattr(fd, TCSAFLUSH, &buf);
      init = TRUE;
   }

   memset(c, 0, 3);
   i = read(fd, c, 1);

   if (i == 0)
      return 0;

   /* check if ESC */
   if (c[0] == 27) {
      i = read(fd, c, 2);
      if (i == 0)               /* return if only ESC */
         return 27;

      /* cursor keys return 2 chars, others 3 chars */
      if (c[1] < 65)
         read(fd, c, 1);

      /* convert ESC sequence to CH_xxx */
      switch (c[1]) {
      case 49:
         return CH_HOME;
      case 50:
         return CH_INSERT;
      case 51:
         return CH_DELETE;
      case 52:
         return CH_END;
      case 53:
         return CH_PUP;
      case 54:
         return CH_PDOWN;
      case 65:
         return CH_UP;
      case 66:
         return CH_DOWN;
      case 67:
         return CH_RIGHT;
      case 68:
         return CH_LEFT;
      }
   }

   /* BS/DEL -> BS */
   if (c[0] == 127)
      return CH_BS;

   return c[0];

#elif defined(OS_WINNT)

   static BOOL init = FALSE;
   static INT repeat_count = 0;
   static INT repeat_char;
   HANDLE hConsole;
   DWORD nCharsRead;
   INPUT_RECORD ir;
   OSVERSIONINFO vi;

   /* find out if we are under W95 */
   vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&vi);

   if (vi.dwPlatformId != VER_PLATFORM_WIN32_NT) {
      /* under W95, console doesn't work properly */
      int c;

      if (!kbhit())
         return 0;

      c = getch();
      if (c == 224) {
         c = getch();
         switch (c) {
         case 71:
            return CH_HOME;
         case 72:
            return CH_UP;
         case 73:
            return CH_PUP;
         case 75:
            return CH_LEFT;
         case 77:
            return CH_RIGHT;
         case 79:
            return CH_END;
         case 80:
            return CH_DOWN;
         case 81:
            return CH_PDOWN;
         case 82:
            return CH_INSERT;
         case 83:
            return CH_DELETE;
         }
      }
      return c;
   }

   hConsole = GetStdHandle(STD_INPUT_HANDLE);

   if (reset) {
      SetConsoleMode(hConsole, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT |
                     ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
      init = FALSE;
      return 0;
   }

   if (!init) {
      SetConsoleMode(hConsole, ENABLE_PROCESSED_INPUT);
      init = TRUE;
   }

   if (repeat_count) {
      repeat_count--;
      return repeat_char;
   }

   PeekConsoleInput(hConsole, &ir, 1, &nCharsRead);

   if (nCharsRead == 0)
      return 0;

   ReadConsoleInput(hConsole, &ir, 1, &nCharsRead);

   if (ir.EventType != KEY_EVENT)
      return ss_getchar(0);

   if (!ir.Event.KeyEvent.bKeyDown)
      return ss_getchar(0);

   if (ir.Event.KeyEvent.wRepeatCount > 1) {
      repeat_count = ir.Event.KeyEvent.wRepeatCount - 1;
      repeat_char = ir.Event.KeyEvent.uChar.AsciiChar;
      return repeat_char;
   }

   if (ir.Event.KeyEvent.uChar.AsciiChar)
      return ir.Event.KeyEvent.uChar.AsciiChar;

   if (ir.Event.KeyEvent.dwControlKeyState & (ENHANCED_KEY)) {
      switch (ir.Event.KeyEvent.wVirtualKeyCode) {
      case 33:
         return CH_PUP;
      case 34:
         return CH_PDOWN;
      case 35:
         return CH_END;
      case 36:
         return CH_HOME;
      case 37:
         return CH_LEFT;
      case 38:
         return CH_UP;
      case 39:
         return CH_RIGHT;
      case 40:
         return CH_DOWN;
      case 45:
         return CH_INSERT;
      case 46:
         return CH_DELETE;
      }

      return ir.Event.KeyEvent.wVirtualKeyCode;
   }

   return ss_getchar(0);

#elif defined(OS_MSDOS)

   int c;

   if (!kbhit())
      return 0;

   c = getch();
   if (!c) {
      c = getch();
      switch (c) {
      case 71:
         return CH_HOME;
      case 72:
         return CH_UP;
      case 73:
         return CH_PUP;
      case 75:
         return CH_LEFT;
      case 77:
         return CH_RIGHT;
      case 79:
         return CH_END;
      case 80:
         return CH_DOWN;
      case 81:
         return CH_PDOWN;
      case 82:
         return CH_INSERT;
      case 83:
         return CH_DELETE;
      }
   }
   return c;

#else
   return -1;
#endif
}

/*------------------------------------------------------------------*/
char *ss_gets(char *string, int size)
/********************************************************************\

  Routine: ss_gets

  Purpose: Read a line from standard input. Strip trailing new line
           character. Return in a loop so that it cannot be interrupted
           by an alarm() signal (like under Sun Solaris)

  Input:
    INT    size             Size of string

  Output:
    BOOL   string           Return string

  Function value:
    char                    Return string

\********************************************************************/
{
   char *p;

   do {
      p = fgets(string, size, stdin);
   } while (p == NULL);


   if (strlen(p) > 0 && p[strlen(p) - 1] == '\n')
      p[strlen(p) - 1] = 0;

   return p;
}

/*------------------------------------------------------------------*/
/********************************************************************\
*                                                                    *
*                  Direct IO functions                               *
*                                                                    *
\********************************************************************/

/*------------------------------------------------------------------*/
INT ss_directio_give_port(INT start, INT end)
{
#ifdef OS_WINNT

   /* under Windows NT, use DirectIO driver to open ports */

   OSVERSIONINFO vi;
   HANDLE hdio = 0;
   DWORD buffer[] = { 6, 0, 0, 0 };
   DWORD size;

   vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&vi);

   /* use DirectIO driver under NT to gain port access */
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
      hdio =
          CreateFile("\\\\.\\directio", GENERIC_READ, FILE_SHARE_READ,
                     NULL, OPEN_EXISTING, 0, NULL);
      if (hdio == INVALID_HANDLE_VALUE) {
         printf("hyt1331.c: Cannot access IO ports (No DirectIO driver installed)\n");
         return -1;
      }

      /* open ports */
      buffer[1] = start;
      buffer[2] = end;
      if (!DeviceIoControl
          (hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer), NULL, 0, &size, NULL))
         return -1;
   }

   return SS_SUCCESS;
#else
   return SS_SUCCESS;
#endif
}

/*------------------------------------------------------------------*/
INT ss_directio_lock_port(INT start, INT end)
{
#ifdef OS_WINNT

   /* under Windows NT, use DirectIO driver to lock ports */

   OSVERSIONINFO vi;
   HANDLE hdio;
   DWORD buffer[] = { 7, 0, 0, 0 };
   DWORD size;

   vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&vi);

   /* use DirectIO driver under NT to gain port access */
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
      hdio =
          CreateFile("\\\\.\\directio", GENERIC_READ, FILE_SHARE_READ,
                     NULL, OPEN_EXISTING, 0, NULL);
      if (hdio == INVALID_HANDLE_VALUE) {
         printf("hyt1331.c: Cannot access IO ports (No DirectIO driver installed)\n");
         return -1;
      }

      /* lock ports */
      buffer[1] = start;
      buffer[2] = end;
      if (!DeviceIoControl
          (hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer), NULL, 0, &size, NULL))
         return -1;
   }

   return SS_SUCCESS;
#else
   return SS_SUCCESS;
#endif
}

/*------------------------------------------------------------------*/
/********************************************************************\
*                                                                    *
*                  System logging                                    *
*                                                                    *
\********************************************************************/

/*------------------------------------------------------------------*/
INT ss_syslog(const char *message)
/********************************************************************\

  Routine: ss_syslog

  Purpose: Write a message to the system logging facility

  Input:
    char   format  Same as for printf

  Output:
    <none>

  Function value:
    SS_SUCCESS     Successful completion

\********************************************************************/
{
#ifdef OS_UNIX
   static BOOL init = FALSE;

   if (!init) {
#ifdef OS_ULTRIX
      openlog("MIDAS", LOG_PID);
#else
      openlog("MIDAS", LOG_PID, LOG_USER);
#endif
      init = TRUE;
   }

   syslog(LOG_DEBUG, message);
   return SS_SUCCESS;
#elif defined(OS_WINNT)         /* OS_UNIX */
/*
HANDLE hlog = 0;
const char *pstr[2];

  if (!hlog)
    {
    HKEY  hk;
    DWORD d;
    char  str[80];

    RegCreateKey(HKEY_LOCAL_MACHINE,
      "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\Midas", &hk);

    strcpy(str, (char *) rpc_get_server_option(RPC_OSERVER_NAME));
    RegSetValueEx(hk, "EventMessageFile", 0, REG_EXPAND_SZ, (LPBYTE) str, strlen(str) + 1);

    d = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE |
        EVENTLOG_INFORMATION_TYPE;
    RegSetValueEx(hk, "TypesSupported", 0, REG_DWORD, (LPBYTE) &d, sizeof(DWORD));
    RegCloseKey(hk);

    hlog = RegisterEventSource(NULL, "Midas");
    }

  pstr[0] = message;
  pstr[1] = NULL;

  if (hlog)
    ReportEvent(hlog, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, pstr, NULL);
*/
   return SS_SUCCESS;

#else                           /* OS_WINNT */

   return SS_SUCCESS;

#endif
}

/*------------------------------------------------------------------*/
/********************************************************************\
*                                                                    *
*                  Encryption                                        *
*                                                                    *
\********************************************************************/

#define bin_to_ascii(c) ((c)>=38?((c)-38+'a'):(c)>=12?((c)-12+'A'):(c)+'.')

char *ss_crypt(char *buf, char *salt)
/********************************************************************\

  Routine: ss_crypt

  Purpose: Simple fake of UNIX crypt(3) function, until we get
           a better one

  Input:
    char   *buf             Plain password
    char   *slalt           Two random characters
                            events. Can be used to skip events

  Output:
    <none>

  Function value:
    char*                   Encrypted password

\********************************************************************/
{
   int i, seed;
   static char enc_pw[13];

   memset(enc_pw, 0, sizeof(enc_pw));
   enc_pw[0] = salt[0];
   enc_pw[1] = salt[1];

   for (i = 0; i < 8 && buf[i]; i++)
      enc_pw[i + 2] = buf[i];
   for (; i < 8; i++)
      enc_pw[i + 2] = 0;

   seed = 123;
   for (i = 2; i < 13; i++) {
      seed = 5 * seed + 27 + enc_pw[i];
      enc_pw[i] = (char) bin_to_ascii(seed & 0x3F);
   }

   return enc_pw;
}

/**dox***************************************************************/
#endif                          /* DOXYGEN_SHOULD_SKIP_THIS */

/** @} */ /* end of msfunctionc */
/** @} */ /* end of msystemincludecode */

