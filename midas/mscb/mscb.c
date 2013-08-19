/********************************************************************\

  Name:         mscb.c
  Created by:   Stefan Ritt

  Contents:     Midas Slow Control Bus communication functions

  $Log: mscb.c,v $
  Revision 1.1.1.1  2005/06/20 23:37:16  mucap
  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
  (Next, I'll commit our local customizations.)

  Revision 1.74  2004/10/03 18:27:13  olchansk
  add MacOSX (aka darwin) native USB support
  make libusb support conditional on HAVE_LIBUSB (this allows one to use libusb on MacOSX)

  Revision 1.73  2004/09/10 12:27:21  midas
  Version 1.7.5

  Revision 1.72  2004/07/29 13:55:18  midas
  Version 1.7.4

  Revision 1.71  2004/07/21 14:18:29  midas
  Added more debugging info

  Revision 1.70  2004/07/08 11:14:59  midas
  Implemented reconnect for USB

  Revision 1.69  2004/05/19 15:14:47  midas
  *** empty log message ***

  Revision 1.68  2004/05/17 09:33:16  midas
  Added #ifdef _USRDLL

  Revision 1.67  2004/05/10 18:58:14  midas
  Added _STRLCPY_DEFINED

  Revision 1.66  2004/04/07 11:06:17  midas
  Version 1.7.1

  Revision 1.65  2004/04/05 10:10:27  midas
  Fixed linux warnings

  Revision 1.64  2004/03/19 12:57:59  midas
  Version 1.7.0

  Revision 1.63  2004/03/19 12:09:16  midas
  Upload with simplified CRC

  Revision 1.62  2004/03/11 09:58:10  midas
  mscb_init() does not ask for device if running under labview

  Revision 1.61  2004/03/11 08:46:07  midas
  Don't display DirectIO error if USB submaster present

  Revision 1.60  2004/03/10 13:28:25  midas
  mscb_init returns device name

  Revision 1.59  2004/03/10 12:41:07  midas
  Dedicated USB timeout

  Revision 1.58  2004/03/10 10:28:47  midas
  Implemented test block write for speed tests

  Revision 1.57  2004/03/09 15:37:09  midas
  Fixed problems with small strings

  Revision 1.56  2004/03/09 15:02:13  midas
  Reverse byte order only if size<5

  Revision 1.55  2004/03/09 14:19:32  midas
  Fixed problems with write/read of strings

  Revision 1.54  2004/03/09 11:55:50  midas
  Fixed linux usb code

  Revision 1.53  2004/03/05 19:39:52  midas
  Better usb error reporting

  Revision 1.52  2004/03/05 16:30:09  midas
  Added libusb code

  Revision 1.51  2004/03/05 14:00:19  midas
  Return if no submaster present

  Revision 1.50  2004/03/04 15:29:20  midas
  Added USB support

  Revision 1.49  2004/01/07 12:56:15  midas
  Chaned line length

  Revision 1.48  2004/01/07 12:52:23  midas
  Changed indentation

  Revision 1.47  2003/10/03 14:08:07  midas
  Added locking parameter to mscb_addr

  Revision 1.46  2003/09/30 08:03:41  midas
  Implemented multiple RPC connections

  Revision 1.45  2003/09/23 09:25:26  midas
  Added RPC call for mscb_addr

  Revision 1.44  2003/07/18 07:40:07  midas
  Version 1.4.9

  Revision 1.43  2003/07/14 10:18:27  midas
  HW RESET uses negative logic

  Revision 1.42  2003/06/27 13:51:01  midas
  Added automatic submaster reset

  Revision 1.41  2003/06/26 11:55:55  midas
  Made reset pulse longer

  Revision 1.40  2003/06/11 14:13:33  midas
  Version 1.4.5

  Revision 1.39  2003/05/12 14:28:14  midas
  Added linux include files

  Revision 1.38  2003/05/12 13:49:17  midas
  Added address check for SET_ADDR command

  Revision 1.37  2003/05/12 10:41:38  midas
  Fixed compiler warnings

  Revision 1.36  2003/05/12 10:30:45  midas
  Fixed name collisions with midas library

  Revision 1.35  2003/04/03 08:21:44  midas
  Added submaster check

  Revision 1.34  2003/03/25 11:24:17  midas
  Fixed size bug

  Revision 1.33  2003/03/25 09:42:57  midas
  Added debugging facility

  Revision 1.32  2003/03/23 11:48:41  midas
  Added mscb_link()

  Revision 1.31  2003/03/19 16:35:03  midas
  Eliminated configuration parameters

  Revision 1.30  2003/03/06 16:08:50  midas
  Protocol version 1.3 (change node name)

  Revision 1.29  2003/03/03 15:58:50  midas
  V1.2.1, fixed communication problem with slow ADuC

  Revision 1.28  2003/02/27 10:44:05  midas
  Two-cygle communication with submaster to avoid data collision

  Revision 1.27  2003/01/27 16:11:14  midas
  Switched to version 1.1.1

  Revision 1.26  2003/01/27 16:10:09  midas
  Keep port in input mode by default (not to collide with submaster)

  Revision 1.25  2002/11/29 09:01:50  midas
  Added <unistd.h>

  Revision 1.24  2002/11/28 14:44:09  midas
  Removed SIZE_XBIT

  Revision 1.23  2002/11/28 13:04:36  midas
  Implemented protocol version 1.2 (echo, read_channels)

  Revision 1.22  2002/11/28 10:32:09  midas
  Implemented locking under linux

  Revision 1.21  2002/11/27 15:40:05  midas
  Added version, fixed few bugs

  Revision 1.20  2002/11/20 12:01:26  midas
  Added host to mscb_init

  Revision 1.19  2002/11/06 16:46:11  midas
  Check parameter size

  Revision 1.18  2002/11/06 14:01:20  midas
  Fixed small bugs

  Revision 1.17  2002/10/28 14:26:30  midas
  Changes from Japan

  Revision 1.16  2002/10/16 15:25:06  midas
  xxx16 now does 32 bit exchange

  Revision 1.15  2002/10/09 15:48:13  midas
  Fixed bug with download

  Revision 1.14  2002/10/09 11:06:46  midas
  Protocol version 1.1

  Revision 1.13  2002/10/07 15:16:32  midas
  Added upgrade facility

  Revision 1.12  2002/10/03 15:30:40  midas
  Added linux support

  Revision 1.11  2002/09/27 11:10:51  midas
  Changed buffer type

  Revision 1.10  2002/09/10 13:13:21  midas
  Remove strobe on timeout

  Revision 1.9  2002/08/12 12:10:55  midas
  Added error handling (zero padding)

  Revision 1.8  2002/07/10 09:51:18  midas
  Introduced mscb_flash()

  Revision 1.7  2001/11/05 11:47:04  midas
  Fixed bug to run msc under W95

  Revision 1.6  2001/10/31 11:16:54  midas
  Added IO check function

  Revision 1.5  2001/09/04 07:33:42  midas
  Rewrote write16/read16 functions

  Revision 1.4  2001/08/31 12:23:50  midas
  Added mutex protection

  Revision 1.3  2001/08/31 11:35:20  midas
  Added "wp" command in msc.c, changed parport to device in mscb.c

  Revision 1.2  2001/08/31 11:04:39  midas
  Added write16 and read16 (for LabView)


\********************************************************************/

#define MSCB_LIBRARY_VERSION   "1.7.5"
#define MSCB_PROTOCOL_VERSION  "1.7"

#ifdef _MSC_VER                 // Windows includes

#include <windows.h>
#include <conio.h>

#elif defined(OS_LINUX)        // Linux includes

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <linux/parport.h>
#include <linux/ppdev.h>

#ifdef HAVE_LIBUSB
#include <usb.h>
#endif

#elif defined(OS_DARWIN)

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>

#include <assert.h>
#include <mach/mach.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>

#endif

#include <stdio.h>
#include "mscb.h"
#include "rpc.h"

/*------------------------------------------------------------------*/

MSCB_FD mscb_fd[MSCB_MAX_FD];

/* pin definitions parallel port */

/*

  Ofs Inv   PC     DB25     SM300   DIR   MSCB Name

   0  No    D0 ----- 2 ----- P1.0   <->    |
   0  No    D1 ----- 3 ----- P1.1   <->    |
   0  No    D2 ----- 4 ----- P1.2   <->    |  D
   0  No    D3 ----- 5 ----- P1.3   <->     \ a
   0  No    D4 ----- 6 ----- P1.4   <->     / t
   0  No    D5 ----- 7 ----- P1.5   <->    |  a
   0  No    D6 ----- 8 ----- P1.6   <->    |
   0  No    D7 ----- 9 ----- P1.7   <->    |

   2  Yes   !STR --- 1  ---- P2.3    ->    /STROBE
   1  Yes   !BSY --- 11 ---- P2.7   <-     BUSY

   1  Yes   !ACK --- 10 ---- P2.6   <-     /DATAREADY
   2  Yes   !DSL --- 17 ---- P2.5    ->    /ACK

   1  No    PAP ---- 12 ---- P2.   <-     STATUS

   2  Yes   !ALF --- 14 ---- RESET   ->    /HW RESET
   2  No    INI ---- 16 ---- P2.4    ->    BIT9

*/


/* enumeration of control lines */

#define LPT_STROBE     1
#define LPT_ACK        2
#define LPT_RESET      3
#define LPT_BIT9       4
#define LPT_READMODE   5

#define LPT_BUSY       6
#define LPT_DATAREADY  7

/*
#define LPT_NSTROBE     (1<<0)
#define LPT_NSTROBE_OFS     2

#define LPT_NBUSY       (1<<7)
#define LPT_NBUSY_OFS       1

#define LPT_NDATAREADY  (1<<6)
#define LPT_NDATAREADY_OFS  1

#define LPT_NACK        (1<<3)
#define LPT_NACK_OFS        2

#define LPT_NRESET      (1<<1)
#define LPT_NRESET_OFS      2

#define LPT_BIT9        (1<<2)
#define LPT_BIT9_OFS        2

#define LPT_DIRECTION   (1<<5)
#define LPT_DIRECTION_OFS   2
*/

/* other constants */

#define TIMEOUT_OUT      1000   /* ~1ms */

extern int _debug_flag;         /* global debug flag */
extern void debug_log(char *format, ...);

/* RS485 flags for USB submaster */
#define RS485_FLAG_BIT9      (1<<0)
#define RS485_FLAG_NO_ACK    (1<<1)
#define RS485_FLAG_SHORT_TO  (1<<2)
#define RS485_FLAG_LONG_TO   (1<<3)
#define RS485_FLAG_CMD       (1<<4)

/*------------------------------------------------------------------*/

/* function declarations */
int musb_init(int index, int *hr, int *hw);
void musb_close(int fdr, int fdw);

/*------------------------------------------------------------------*/

/* cache definitions for mscb_link */

typedef struct {
   int fd;
   unsigned short adr;
   unsigned char index;
   void *data;
   unsigned char size;
   unsigned long last;
} CACHE_ENTRY;

CACHE_ENTRY *cache;
int n_cache;

#define CACHE_PERIOD 500        // update cache every 500 ms

/*------------------------------------------------------------------*/

/* missing linux functions */

#if defined(OS_UNIX)

int kbhit()
{
   int n;

   ioctl(0, FIONREAD, &n);
   return (n > 0);
}

#endif

/*---- strlcpy and strlcat to avoid buffer overflow ----------------*/

#ifndef HAVE_STRLCPY

/*
* Copy src to string dst of size siz.  At most siz-1 characters
* will be copied.  Always NUL terminates (unless size == 0).
* Returns strlen(src); if retval >= siz, truncation occurred.
*/
size_t strlcpy(char *dst, const char *src, size_t size)
{
   char *d = dst;
   const char *s = src;
   size_t n = size;

   /* Copy as many bytes as will fit */
   if (n != 0 && --n != 0) {
      do {
         if ((*d++ = *s++) == 0)
            break;
      } while (--n != 0);
   }

   /* Not enough room in dst, add NUL and traverse rest of src */
   if (n == 0) {
      if (size != 0)
         *d = '\0';             /* NUL-terminate dst */
      while (*s++);
   }

   return (s - src - 1);        /* count does not include NUL */
}

/*
* Appends src to string dst of size siz (unlike strncat, siz is the
* full size of dst, not space left).  At most siz-1 characters
* will be copied.  Always NUL terminates (unless size <= strlen(dst)).
* Returns strlen(src) + MIN(size, strlen(initial dst)).
* If retval >= size, truncation occurred.
*/
size_t strlcat(char *dst, const char *src, size_t size)
{
   char *d = dst;
   const char *s = src;
   size_t n = size;
   size_t dlen;

   /* Find the end of dst and adjust bytes left but don't go past end */
   while (n-- != 0 && *d != '\0')
      d++;
   dlen = d - dst;
   n = size - dlen;

   if (n == 0)
      return (dlen + strlen(s));
   while (*s != '\0') {
      if (n != 1) {
         *d++ = *s;
         n--;
      }
      s++;
   }
   *d = '\0';

   return (dlen + (s - src));   /* count does not include NUL */
}

#endif

/*------------------------------------------------------------------*/

unsigned char crc8_data[] = {
   0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
   0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
   0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
   0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
   0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
   0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
   0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
   0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
   0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5,
   0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
   0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
   0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
   0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6,
   0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
   0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
   0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
   0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f,
   0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
   0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
   0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
   0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c,
   0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
   0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1,
   0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
   0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
   0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
   0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
   0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
   0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a,
   0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
   0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
   0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

unsigned char crc8(unsigned char *data, int len)
/********************************************************************\

  Routine: crc8

  Purpose: Calculate 8-bit cyclic redundancy checksum

  Input:
    unsigned char *data     data buffer
    int len                 data length in bytes


  Function value:
    unsighend char          CRC-8 code

\********************************************************************/
{
   int i;
   unsigned char crc8_code, index;

   crc8_code = 0;
   for (i = 0; i < len; i++) {
      index = data[i] ^ crc8_code;
      crc8_code = crc8_data[index];
   }

   return crc8_code;
}

/*------------------------------------------------------------------*/

int strieq(const char *str1, const char *str2)
{
   if (str1 == NULL && str2 != NULL)
      return 0;
   if (str1 != NULL && str2 == NULL)
      return 0;
   if (str1 == NULL && str2 == NULL)
      return 1;

   while (*str1)
      if (toupper(*str1++) != toupper(*str2++))
         return 0;

   if (*str2)
      return 0;

   return 1;
}

/*------------------------------------------------------------------*/

#ifdef _MSC_VER
static HANDLE mutex_handle = 0;
#endif

int mscb_lock(int fd)
{
#ifdef _MSC_VER
   int status;
   if (mutex_handle == 0)
      mutex_handle = CreateMutex(NULL, FALSE, "mscb");

   if (mutex_handle == 0)
      return 0;

   status = WaitForSingleObject(mutex_handle, 1000);

   if (status == WAIT_FAILED)
      return 0;
   if (status == WAIT_TIMEOUT)
      return 0;

#elif defined(OS_LINUX)

   if (mscb_fd[fd - 1].type == MSCB_TYPE_LPT) {
      if (ioctl(mscb_fd[fd - 1].fd, PPCLAIM))
         return 0;
   } else if (mscb_fd[fd - 1].type == MSCB_TYPE_USB) {
      if (usb_claim_interface((usb_dev_handle *) mscb_fd[fd - 1].hr, 0) < 0)
         return 0;
   }
#endif
   return MSCB_SUCCESS;
}

int mscb_release(int fd)
{
#ifdef _MSC_VER
   int status;

   status = ReleaseMutex(mutex_handle);
   if (status == FALSE)
      return 0;

#elif defined(OS_LINUX)

   if (mscb_fd[fd - 1].type == MSCB_TYPE_LPT) {
      if (ioctl(mscb_fd[fd - 1].fd, PPRELEASE))
         return 0;
   } else if (mscb_fd[fd - 1].type == MSCB_TYPE_USB) {
      if (usb_release_interface((usb_dev_handle *) mscb_fd[fd - 1].hr, 0) < 0)
         return 0;
   }
#endif
   return MSCB_SUCCESS;
}

/*---- low level functions for direct port access ------------------*/

void pp_wcontrol(int fd, int signal, int flag)
/* write control signal */
{
   static unsigned int mask = 0;

   switch (signal) {
   case LPT_STROBE:            // negative port, negative MSCB usage
      mask = flag ? mask | (1 << 0) : mask & ~(1 << 0);
      break;
   case LPT_RESET:             // negative port, negative MSCB usage
      mask = flag ? mask | (1 << 1) : mask & ~(1 << 1);
      break;
   case LPT_BIT9:              // positive
      mask = flag ? mask | (1 << 2) : mask & ~(1 << 2);
      break;
   case LPT_ACK:               // negative port, negative MSCB usage
      mask = flag ? mask | (1 << 3) : mask & ~(1 << 3);
      break;
   case LPT_READMODE:          // positive
      mask = flag ? mask | (1 << 5) : mask & ~(1 << 5);
      break;
   }

#if defined(_MSC_VER)
   _outp((unsigned short) (mscb_fd[fd - 1].fd + 2), mask);
#elif defined(OS_LINUX)
   ioctl(mscb_fd[fd - 1].fd, PPWCONTROL, &mask);
#else
   /* no-op unsupported operation */
#endif
}

/*------------------------------------------------------------------*/

int pp_rstatus(int fd, int signal)
{
/* read status signal */
   unsigned int mask = 0;

#if defined(_MSC_VER)
   mask = _inp((unsigned short) (mscb_fd[fd - 1].fd + 1));
#elif defined(OS_LINUX)
   ioctl(mscb_fd[fd - 1].fd, PPRSTATUS, &mask);
#else
   /* no-op unsupported operation */
#endif

   switch (signal) {
   case LPT_BUSY:
      return (mask & (1 << 7)) == 0;
   case LPT_DATAREADY:
      return (mask & (1 << 6)) == 0;
   }

   return 0;
}

/*------------------------------------------------------------------*/

void pp_setdir(int fd, unsigned int r)
{
/* set port direction: r==0:write mode;  r==1:read mode */
#if defined(_MSC_VER)
   pp_wcontrol(fd, LPT_READMODE, r);
#elif defined(OS_LINUX)
   if (ioctl(mscb_fd[fd - 1].fd, PPDATADIR, &r))
      perror("PPDATADIR");
#else
   /* no-op unsupported operation */
#endif
}

/*------------------------------------------------------------------*/

void pp_wdata(int fd, unsigned int data)
/* output data byte */
{
#if defined(_MSC_VER)
   _outp((unsigned short) mscb_fd[fd - 1].fd, data);
#elif defined(OS_LINUX)
   ioctl(mscb_fd[fd - 1].fd, PPWDATA, &data);
#else
   /* no-op unsupported operation */
#endif
}

/*------------------------------------------------------------------*/

unsigned char pp_rdata(int fd)
/* intput data byte */
{
#if defined(_MSC_VER)
   return _inp((unsigned short) mscb_fd[fd - 1].fd);
#elif defined(OS_LINUX)
   unsigned char data;
   if (ioctl(mscb_fd[fd - 1].fd, PPRDATA, &data))
      perror("PPRDATA");
   return data;
#else
   /* no-op unsupported operation */
#endif
}

/*------------------------------------------------------------------*/

int msend_usb(int fd, void *buf, int size)
{
   int n_written;

#if defined(_MSC_VER)
   WriteFile((HANDLE) fd, buf, size, &n_written, NULL);
#elif defined(OS_LINUX)
   n_written = usb_bulk_write((usb_dev_handle *) fd, 2, buf, size, 100);
   usleep(0); // needed for linux not to crash !!!!
#elif defined(OS_DARWIN)
   IOReturn status;
   IOUSBInterfaceInterface** device = (IOUSBInterfaceInterface**)fd;
   status = (*device)->WritePipe(device,2,buf,size);
   if (status != 0) printf("msend_usb: WritePipe() status %d 0x%x\n",status,status);
   n_written = size;
#endif
   return n_written;
}

/*------------------------------------------------------------------*/

int mrecv_usb(int fd, void *buf, int size)
{
   int n_read;

#if defined(_MSC_VER)
   OVERLAPPED overlapped;
   int status;

   memset(&overlapped, 0, sizeof(overlapped));
   overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   n_read = 0;

   status = ReadFile((HANDLE) fd, buf, size, &n_read, &overlapped);
   if (!status) {

      status = GetLastError();
      if (status != ERROR_IO_PENDING)
         return 0;

      /* wait for completion, 1s timeout */
      status = WaitForSingleObject(overlapped.hEvent, 1000);
      if (status == WAIT_TIMEOUT)
         CancelIo((HANDLE) fd);
      else
         GetOverlappedResult((HANDLE) fd, &overlapped, &n_read, FALSE);
   }

   CloseHandle(overlapped.hEvent);
#elif defined(HAVE_LIBUSB)
   usb_dev_handle *dev;
   int i;

   dev = (usb_dev_handle *) fd;

   for (i = 0; i < 10; i++) {
      n_read = usb_bulk_read(dev, 1, buf, size, 1000);
      //printf("##mrecv_usb(%X): %d\n", dev, n_read);

      if (n_read < 0)
         usleep(10000);
      else
         break;
   }
#elif defined(OS_DARWIN)
   IOReturn status;
   IOUSBInterfaceInterface** device = (IOUSBInterfaceInterface**)fd;
   n_read = size;
   status = (*device)->ReadPipe(device,1,buf,&n_read);
   if (status != kIOReturnSuccess) {
      printf("mrecv_usb: size %d, read %d, ReadPipe() status %d 0x%x\n",size,n_read,status,status);
      return -1;
   }
#endif
   return n_read;
}

/*------------------------------------------------------------------*/

int mscb_out(int index, unsigned char *buffer, int len, int flags)
/********************************************************************\

  Routine: mscb_out

  Purpose: Write number of bytes to SM via parallel port or USB

  Input:
    int  index              index to file descriptor table
    char *buffer            data buffer
    int  len                number of bytes in buffer
    int  bit9               set bit9 in communication, used for
                            node addressing

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout

\********************************************************************/
{
   int i, timeout;
   unsigned char usb_buf[65];

   if (index > MSCB_MAX_FD || index < 1 || !mscb_fd[index - 1].type)
      return MSCB_INVAL_PARAM;

   /*---- USB code ----*/
   if (mscb_fd[index - 1].type == MSCB_TYPE_USB) {

      if (len >= 64 || len < 1)
         return MSCB_INVAL_PARAM;

      /* add flags in first byte of USB buffer */
      usb_buf[0] = flags;
      memcpy(usb_buf + 1, buffer, len);

      /* send on OUT pipe */
      i = msend_usb(mscb_fd[index - 1].hw, usb_buf, len + 1);

      if (i == 0) {
         /* USB submaster might have been dis- and reconnected, so reinit */
         musb_close(mscb_fd[index - 1].hr, mscb_fd[index - 1].hw);

         i = musb_init(atoi(mscb_fd[index - 1].device+3), &mscb_fd[index - 1].hr, &mscb_fd[index - 1].hw);
         if (i < 0)
            return MSCB_TIMEOUT;

         i = msend_usb(mscb_fd[index - 1].hw, usb_buf, len + 1);
      }

      if (i != len + 1) {
         return MSCB_TIMEOUT;
      }

   } else {

   /*---- LPT code ----*/

      /* remove accidental strobe */
      pp_wcontrol(index, LPT_STROBE, 0);

      for (i = 0; i < len; i++) {
         /* wait for SM ready */
         for (timeout = 0; timeout < TIMEOUT_OUT; timeout++) {
            if (!pp_rstatus(index, LPT_BUSY))
               break;
         }
         if (timeout == TIMEOUT_OUT) {
#ifndef _USRDLL
            printf("Automatic submaster reset.\n");
#endif

            mscb_release(index);
            mscb_reset(index);
            mscb_lock(index);

            Sleep(100);

            /* wait for SM ready */
            for (timeout = 0; timeout < TIMEOUT_OUT; timeout++) {
               if (!pp_rstatus(index, LPT_BUSY))
                  break;
            }

            if (timeout == TIMEOUT_OUT)
               return MSCB_TIMEOUT;
         }

         /* output data byte */
         pp_wdata(index, buffer[i]);

         /* set bit 9 */
         if (flags & RS485_FLAG_BIT9)
            pp_wcontrol(index, LPT_BIT9, 1);
         else
            pp_wcontrol(index, LPT_BIT9, 0);

         /* set strobe */
         pp_wcontrol(index, LPT_STROBE, 1);

         /* wait for busy to become active */
         for (timeout = 0; timeout < TIMEOUT_OUT; timeout++) {
            if (pp_rstatus(index, LPT_BUSY))
               break;
         }

         if (timeout == TIMEOUT_OUT) {
#ifndef _USRDLL
            printf("Automatic submaster reset.\n");
#endif

            mscb_release(index);
            mscb_reset(index);
            mscb_lock(index);

            Sleep(100);

            /* try again */
            return mscb_out(index, buffer, len, flags);
         }

         if (timeout == TIMEOUT_OUT) {
            pp_wdata(index, 0xFF);
            pp_wcontrol(index, LPT_STROBE, 0);
            return MSCB_TIMEOUT;
         }

         /* remove data, make port available for input */
         pp_wdata(index, 0xFF);

         /* remove strobe */
         pp_wcontrol(index, LPT_STROBE, 0);
      }
   }

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_in1(int fd, unsigned char *c, int timeout)
/********************************************************************\

  Routine: mscb_in1

  Purpose: Read one byte from SM via parallel port

  Input:
    int  fd                 file descriptor for connection
    int  timeout            timeout in microseconds

  Output:
    char *c                 data bytes

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout

\********************************************************************/
{
   int i;

   /* wait for DATAREADY, each port access takes roughly 1us */
   timeout = (int) (timeout / 1.3);

   for (i = 0; i < timeout; i++) {
      if (pp_rstatus(fd, LPT_DATAREADY))
         break;
   }
   if (i == timeout)
      return MSCB_TIMEOUT;

   /* switch to input mode */
   pp_setdir(fd, 1);

   /* signal switch to input */
   pp_wcontrol(fd, LPT_ACK, 1);

   /* wait for data ready */
   for (i = 0; i < 1000; i++) {
      if (!pp_rstatus(fd, LPT_DATAREADY))
         break;
   }

   /* read data */
   *c = pp_rdata(fd);

   /* signal data read */
   pp_wcontrol(fd, LPT_ACK, 0);

   /* wait for mode switch */
   for (i = 0; i < 1000; i++) {
      if (pp_rstatus(fd, LPT_DATAREADY))
         break;
   }

   /* switch to output mode */
   pp_setdir(fd, 0);

   /* indicate mode switch */
   pp_wcontrol(fd, LPT_ACK, 1);

   /* wait for end of cycle */
   for (i = 0; i < 1000; i++) {
      if (!pp_rstatus(fd, LPT_DATAREADY))
         break;
   }

   /* remove acknowlege */
   pp_wcontrol(fd, LPT_ACK, 0);

   if (i == 1000)
      return MSCB_TIMEOUT;

   for (i = 0; i < 50; i++)
      pp_rdata(fd);

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_in(int index, char *buffer, int size, int timeout)
/********************************************************************\

  Routine: mscb_in

  Purpose: Read number of bytes from SM via parallel port

  Input:
    int  index              index to file descriptor table
    int  size               size of data buffer
    int  timeout            timeout in microseconds

  Output:
    char *buffer            data buffer

  Function value:
    int                     number of received bytes, -1 for timeout

\********************************************************************/
{
   int i, n, len, status;

   n = 0;
   len = -1;

   if (index > MSCB_MAX_FD || index < 1 || !mscb_fd[index - 1].type)
      return MSCB_INVAL_PARAM;

   /*---- USB code ----*/
   if (mscb_fd[index - 1].type == MSCB_TYPE_USB) {

      /* receive result on IN pipe */
      n = mrecv_usb(mscb_fd[index - 1].hr, buffer, size);

   } else {

   /*---- LPT code ----*/

      /* wait for MCMD_ACK command */
      do {
         status = mscb_in1(index, buffer, timeout);

         /* only read single byte */
         if (size == 1)
            return status;

         /* return in case of timeout */
         if (status != MSCB_SUCCESS)
            return -1;

         /* check for Acknowledge */
         if ((buffer[0] & 0xF8) == MCMD_ACK) {
            len = buffer[n++] & 0x7;

            /* check for variable length data block */
            if (len == 7) {
               status = mscb_in1(index, buffer + n, timeout);

               /* return in case of timeout */
               if (status != MSCB_SUCCESS)
                  return -1;

               len = buffer[n++];
            }
         }

      } while (len == -1);

      /* receive remaining bytes and CRC code */
      for (i = 0; i < len + 1; i++) {
         status = mscb_in1(index, buffer + n + i, timeout);

         /* return in case of timeout */
         if (status != MSCB_SUCCESS)
            return -1;
      }

      n += i;
   }

   return n;
}

/*------------------------------------------------------------------*/

int lpt_init(char *device, int index)
/********************************************************************\

  Routine: lpt_init

  Purpose: Initialize MSCB submaster on parallel port

  Input:
    char *device            Device name "lptx"/"/dev/parportx"
    int  index              Index to mscb_fd[] array

  Output:
    int  mscb_fd[index].fd  File descriptor for device or
                            DirectIO address
  Function value:
    0                       Successful completion
    -1                      Invalid parameter or other error
    -2                      Submaster does not respond
    -3                      No DirectIO driver
    -4                      MSCB system locked by other user

\********************************************************************/
{
   int status;
   char c;

#ifdef _MSC_VER

   /* under NT, user directio driver */

   OSVERSIONINFO vi;
   DWORD buffer[4];
   DWORD size;
   HANDLE hdio;


   /* derive base address from device name */
   if (atoi(device + 3) == 1)
      mscb_fd[index].fd = 0x378;
   else if (atoi(device + 3) == 2)
      mscb_fd[index].fd = 0x278;
   else
      return -1;

   buffer[0] = 6;               /* give IO */
   buffer[1] = mscb_fd[index].fd;
   buffer[2] = buffer[1] + 4;
   buffer[3] = 0;

   vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&vi);

   /* use DirectIO driver under NT to gain port access */
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
      hdio = CreateFile("\\\\.\\directio", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
      if (hdio == INVALID_HANDLE_VALUE) {
         //printf("mscb.c: Cannot access parallel port (No DirectIO driver installed)\n");
         return -3;
      }

      if (!DeviceIoControl(hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer), NULL, 0, &size, NULL))
         return -1;
   }
#elif defined(OS_LINUX)

   int i;

   mscb_fd[index].fd = open(device, O_RDWR);
   if (mscb_fd[index].fd < 0) {
#ifndef _USRDLL
      perror("open");
      printf("Please make sure that device \"%s\" is world readable/writable\n", device);
#endif
      return -1;
   }

   if (ioctl(mscb_fd[index].fd, PPCLAIM)) {
#ifndef _USRDLL
      perror("PPCLAIM");
      printf("Please load driver via \"modprobe parport_pc\" as root\n");
#endif
      return -1;
   }

   if (ioctl(mscb_fd[index].fd, PPRELEASE)) {
#ifndef _USRDLL
      perror("PPRELEASE");
#endif
      return -1;
   }

   i = IEEE1284_MODE_BYTE;
   if (ioctl(mscb_fd[index].fd, PPSETMODE, &i)) {
#ifndef _USRDLL
      perror("PPSETMODE");
#endif
      return -1;
   }
#endif

   status = mscb_lock(index + 1);
   if (status != MSCB_SUCCESS)
      return -4;

   /* set initial state of handshake lines */
   pp_wcontrol(index + 1, LPT_RESET, 0);
   pp_wcontrol(index + 1, LPT_STROBE, 0);

   /* switch to output mode */
   pp_setdir(index + 1, 0);

   /* wait for submaster irritated by a stuck LPT_STROBE */
   Sleep(100);

   /* check if SM alive */
   if (pp_rstatus(index + 1, LPT_BUSY)) {
      //printf("mscb.c: No SM present on parallel port\n");
      mscb_release(index + 1);
      return -2;
   }

   /* empty RBuffer of SM */
   do {
      status = mscb_in1(index + 1, &c, 1000);
      if (status == MSCB_SUCCESS)
         printf("%02X ", c);
   } while (status == MSCB_SUCCESS);

   mscb_release(index + 1);

   return 0;
}

/*------------------------------------------------------------------*/

int lpt_close(int fd)
/********************************************************************\

  Routine: lpt_close

  Purpose: Close handle to MSCB submaster

  Input:
    int  index              Index to mscb_fd[] array

  Function value:
    MSCB_SUCCES             Successful completion

\********************************************************************/
{
#ifdef _MSC_VER

   /* under NT, user directio driver */

   OSVERSIONINFO vi;
   DWORD buffer[4];
   DWORD size;
   HANDLE hdio;

   buffer[0] = 7;               /* lock port */
   buffer[1] = fd;
   buffer[2] = buffer[1] + 4;
   buffer[3] = 0;

   vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&vi);

   /* use DirectIO driver under NT to gain port access */
   if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
      hdio = CreateFile("\\\\.\\directio", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
      if (hdio == INVALID_HANDLE_VALUE) {
#ifndef _USRDLL
         printf("mscb.c: Cannot access parallel port (No DirectIO driver installed)\n");
#endif
         return -1;
      }

      if (!DeviceIoControl(hdio, (DWORD) 0x9c406000, &buffer, sizeof(buffer), NULL, 0, &size, NULL))
         return -1;
   }
#elif defined(OS_LINUX)

   close(fd);

#endif

   return MSCB_SUCCESS;
}

/*---- USB access functions ----------------------------------------*/

#ifdef _MSC_VER

#include <setupapi.h>
#include <initguid.h>           /* Required for GUID definition */

// link with SetupAPI.Lib.
#pragma comment (lib, "setupapi.lib")

// {CBEB3FB1-AE9F-471c-9016-9B6AC6DCD323}
DEFINE_GUID(GUID_CLASS_MSCB_BULK, 0xcbeb3fb1, 0xae9f, 0x471c, 0x90, 0x16, 0x9b, 0x6a, 0xc6, 0xdc, 0xd3, 0x23);

int musb_init(int index, int *hUSBRead, int *hUSBWrite)
{
   GUID guid;
   HDEVINFO hDevInfoList;
   SP_DEVICE_INTERFACE_DATA deviceInfoData;
   PSP_DEVICE_INTERFACE_DETAIL_DATA functionClassDeviceData;
   ULONG predictedLength, requiredLength;
   int status;
   char device_name[256], str[256];

   // Establish USB connection
   if (hUSBRead && hUSBWrite) {
      *hUSBRead = 0;
      *hUSBWrite = 0;
   }
   guid = GUID_CLASS_MSCB_BULK;

   // Retrieve device list for GUID that has been specified.
   hDevInfoList = SetupDiGetClassDevs(&guid, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

   status = FALSE;
   if (hDevInfoList != NULL) {

      // Clear data structure
      memset(&deviceInfoData, 0, sizeof(deviceInfoData));
      deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

      // retrieves a context structure for a device interface of a device information set.
      if (SetupDiEnumDeviceInterfaces(hDevInfoList, 0, &guid, index, &deviceInfoData)) {
         // Must get the detailed information in two steps
         // First get the length of the detailed information and allocate the buffer
         // retrieves detailed information about a specified device interface.
         functionClassDeviceData = NULL;

         predictedLength = requiredLength = 0;

         SetupDiGetDeviceInterfaceDetail(hDevInfoList, &deviceInfoData, NULL,   // Not yet allocated
                                         0,     // Set output buffer length to zero
                                         &requiredLength,       // Find out memory requirement
                                         NULL);

         predictedLength = requiredLength;
         functionClassDeviceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(predictedLength);
         functionClassDeviceData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

         // Second, get the detailed information
         if (SetupDiGetDeviceInterfaceDetail(hDevInfoList,
                                             &deviceInfoData, functionClassDeviceData,
                                             predictedLength, &requiredLength, NULL)) {

            // Save the device name for subsequent pipe open calls
            strcpy(device_name, functionClassDeviceData->DevicePath);
            free(functionClassDeviceData);

            // Signal device found
            status = TRUE;
         } else
            free(functionClassDeviceData);
      }
   }
   // SetupDiDestroyDeviceInfoList() destroys a device information set
   // and frees all associated memory.
   SetupDiDestroyDeviceInfoList(hDevInfoList);

   if (status) {

      if (hUSBRead) {
         // Get the read handle
         sprintf(str, "%s\\PIPE00", device_name);
         *hUSBRead = (int) CreateFile(str,
                                      GENERIC_WRITE | GENERIC_READ,
                                      FILE_SHARE_WRITE | FILE_SHARE_READ, NULL,
                                      OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

         if (*hUSBRead == (int) INVALID_HANDLE_VALUE)
            return -1;
      }

      if (hUSBWrite) {
         // Get the write handle
         sprintf(str, "%s\\PIPE01", device_name);
         *hUSBWrite = (int) CreateFile(str,
                                       GENERIC_WRITE | GENERIC_READ,
                                       FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

         if (*hUSBWrite == (int) INVALID_HANDLE_VALUE)
            return -1;
      }

   } else
      return -1;

   return 0;
}

#elif defined(HAVE_LIBUSB)

int musb_init(int index, int *fdr, int *fdw)
{
   struct usb_bus *bus;
   struct usb_device *dev;
   usb_dev_handle *udev;
   int found = 0;

   usb_init();
   usb_find_busses();
   usb_find_devices();

   for (bus = usb_busses; bus; bus = bus->next) {
      for (dev = bus->devices; dev; dev = dev->next) {

         if (dev->descriptor.idVendor == 0x10C4 && dev->descriptor.idProduct == 0x1175) {

            found++;
            if (found == index + 1) {

               if (fdr && fdw) {
                  udev = usb_open(dev);
                  if (!udev)
                     return -1;

                  if (usb_set_configuration(udev, 1) < 0)
                     return -5;

                  /* see if we have write access */
                  if (usb_claim_interface(udev, 0) < 0)
                     return -5;
                  usb_release_interface(udev, 0);

                  *fdr = (int) udev;
                  *fdw = (int) udev;
               }

               return 0;
            }
         }
      }
   }

   return -1;
}

#elif defined(OS_DARWIN)

int musb_init(int index, int *fdr, int *fdw)
{
	kern_return_t status;
	io_iterator_t iter;
	io_service_t  service;
	IOCFPlugInInterface** plugin;
	SInt32 score;
	IOUSBDeviceInterface** device;
	IOUSBInterfaceInterface** interface;
	UInt16 vendor, product;
	UInt8 numend;
	int found = 0;
	
	printf("musb_init, index=%d!\n",index);

	status = IORegistryCreateIterator(kIOMasterPortDefault,kIOUSBPlane,kIORegistryIterateRecursively,&iter);
	assert(status==kIOReturnSuccess);

	while ((service = IOIteratorNext(iter)))
	{
	status = IOCreatePlugInInterfaceForService(service, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugin, &score);
	assert(status==kIOReturnSuccess);

	status = IOObjectRelease(service);
	assert(status==kIOReturnSuccess);

	status = (*plugin)->QueryInterface(plugin, CFUUIDGetUUIDBytes (kIOUSBDeviceInterfaceID), (void*)&device);
	assert(status==kIOReturnSuccess);

	status = (*plugin)->Release(plugin);

	status = (*device)->GetDeviceVendor(device, &vendor);
	assert(status==kIOReturnSuccess);
        status = (*device)->GetDeviceProduct(device, &product);
	assert(status==kIOReturnSuccess);

	printf("Found USB device: vendor 0x%04x, product 0x%04x\n",vendor,product);

	if (vendor==0x10C4 && product==0x1175)
		{
		found++;
		if (found==index+1)
		{
printf("Here0\n");
			if (fdr && fdw) {

		  	status = (*device)->USBDeviceOpen(device);
			assert(status==kIOReturnSuccess);

		  	status = (*device)->SetConfiguration(device,1);
			assert(status==kIOReturnSuccess);

    IOUSBFindInterfaceRequest request;

    request.bInterfaceClass = kIOUSBFindInterfaceDontCare;
    request.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
    request.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
    request.bAlternateSetting = kIOUSBFindInterfaceDontCare;

	status = (*device)->CreateInterfaceIterator(device,&request,&iter);
	assert(status==kIOReturnSuccess);

printf("HereA\n");

	while ((service=IOIteratorNext(iter)))
	{
printf("HereB\n");
	status = IOCreatePlugInInterfaceForService(service, kIOUSBInterfaceUserClientTypeID, kIOCFPlugInInterfaceID, &plugin, &score);
	assert(status==kIOReturnSuccess);

	status = (*plugin)->QueryInterface(plugin, CFUUIDGetUUIDBytes (kIOUSBInterfaceInterfaceID), (void*)&interface);
	assert(status==kIOReturnSuccess);


		  	status = (*interface)->USBInterfaceOpen(interface);
			printf("status 0x%x\n",status);
			assert(status==kIOReturnSuccess);

		  	status = (*interface)->GetNumEndpoints(interface,&numend);
			assert(status==kIOReturnSuccess);

			printf("endpoints: %d\n",numend);

			printf("pipe 1 status: 0x%x\n",(*interface)->GetPipeStatus(interface,1));
			printf("pipe 2 status: 0x%x\n",(*interface)->GetPipeStatus(interface,2));


		          *fdr = (int) interface;
		          *fdw = (int) interface;
			return 0;
	}
	}
	return 0;
		}
printf("Here1\n");
		}
	(*device)->Release(device);
	}

        return -1;
}

#endif

/*------------------------------------------------------------------*/

void musb_close(int fdr, int fdw)
{
#if defined(_MSC_VER)
   CloseHandle((HANDLE) fdr);
   CloseHandle((HANDLE) fdw);
#elif defined(HAVE_LIBUSB)
   usb_close((usb_dev_handle *) fdr);
#else
   /* FIXME */
#endif
}

/*------------------------------------------------------------------*/

void mscb_get_version(char *lib_version, char *prot_version)
{
   strcpy(lib_version, MSCB_LIBRARY_VERSION);
   strcpy(prot_version, MSCB_PROTOCOL_VERSION);
}

/*------------------------------------------------------------------*/

int mrpc_connected(int fd)
{
   return mscb_fd[fd - 1].type == MSCB_TYPE_RPC;
}

/*------------------------------------------------------------------*/

int mscb_init(char *device, int bufsize, int debug)
/********************************************************************\

  Routine: mscb_init

  Purpose: Initialize and open MSCB

  Input:
    char *device            Under NT: lpt1 or lpt2
                            Under Linux: /dev/parport0 or /dev/parport1
                            "<host>:device" for RPC connection
                            usbx for USB connection

                            If device equals "", the function 
                            mscb_select_device is called which selects
                            the first available device or asks the user
                            which one to use.
    int bufsize             Size of "device" string, in case no device
                            is specified from the caller and this function
                            returns the chosen device
    int debug               Debug flag. If "1" debugging information is
                            written to "mscb_debug.log" in the current
                            directory. If "-1", do not ask for device
                            (useb by LabView DLL)

  Function value:
    int fd                  device descriptor for connection, -1 if
                            error

\********************************************************************/
{
   int index, i, n;
   int status;
   char host[256], port[256], dev3[256], buf[10], remote_device[256];

   /* search for new file descriptor */
   for (index = 0; index < MSCB_MAX_FD; index++)
      if (mscb_fd[index].fd == 0)
         break;

   if (index == MSCB_MAX_FD)
      return -1;

   /* set global debug flag */
   _debug_flag = (debug == 1);

   debug_log("mscb_init( %s %d %d * %d)\n", device, bufsize, debug);

   /* clear cache */
   for (i = 0; i < n_cache; i++)
      free(cache[i].data);
   free(cache);
   n_cache = 0;

   if (!device[0]) {
      if (debug == -1)
         mscb_select_device(device, bufsize, 0); /* for LabView */
      else
         mscb_select_device(device, bufsize, 1); /* interactively ask for device */
   }

   /* check for RPC connection */
   if (strchr(device, ':')) {
      strlcpy(mscb_fd[index].device, device, sizeof(mscb_fd[index].device));
      mscb_fd[index].type = MSCB_TYPE_RPC;

      strcpy(remote_device, strchr(device, ':') + 1);
      strcpy(host, device);
      *strchr(host, ':') = 0;

      mscb_fd[index].fd = mrpc_connect(host);

      if (mscb_fd[index].fd < 0) {
         mscb_fd[index].fd = 0;
         return -1;
      }

      mscb_fd[index].remote_fd = mrpc_call(mscb_fd[index].fd, RPC_MSCB_INIT, remote_device, bufsize, debug);
      if (mscb_fd[index].remote_fd < 0) {
         mrpc_disconnect(mscb_fd[index].fd);
         mscb_fd[index].fd = 0;
         return -1;
      }

      sprintf(device, "%s:%s", host, remote_device);

      return index + 1;
   }

   /* check which device type */
   strcpy(dev3, device);
   dev3[3] = 0;
   strcpy(mscb_fd[index].device, device);
   mscb_fd[index].type = 0;

   /* LPT port with direct address */
   if (device[1] == 'x') {
      mscb_fd[index].type = MSCB_TYPE_LPT;
      sscanf(port + 2, "%x", &mscb_fd[index].fd);
   }

   /* LPTx */
   if (strieq(dev3, "LPT")) {
      mscb_fd[index].type = MSCB_TYPE_LPT;
   }

   /* /dev/parportx */
   if (strstr(device, "parport")) {
      mscb_fd[index].type = MSCB_TYPE_LPT;
   }

   /* USBx */
   if (strieq(dev3, "usb"))
      mscb_fd[index].type = MSCB_TYPE_USB;

   if (mscb_fd[index].type == 0)
      return -1;

   /*---- initialize submaster ----*/

   if (mscb_fd[index].type == MSCB_TYPE_LPT) {

      status = lpt_init(device, index);
      if (status < 0)
         return status;
   }

   if (mscb_fd[index].type == MSCB_TYPE_USB) {

      status = musb_init(atoi(device + 3), &mscb_fd[index].hr, &mscb_fd[index].hw);
      if (status < 0)
         return status;

      /* linux needs some time to start-up ...??? */
      for (i = 0; i < 10; i++) {
         mscb_lock(index + 1);

         /* check if submaster alive */
         buf[0] = MCMD_ECHO;
         mscb_out(index + 1, buf, 1, RS485_FLAG_CMD);

         n = mscb_in(index + 1, buf, 2, 10000);
         mscb_release(index + 1);

         if (n == 2 && buf[0] == MCMD_ACK)
            break;
      }

      if (n != 2 || buf[0] != MCMD_ACK)
         return -4;
   }

   return index + 1;
}

/*------------------------------------------------------------------*/

int mscb_exit(int fd)
/********************************************************************\

  Routine: mscb_exit

  Purpose: Close a MSCB interface

  Input:
    int  fd                 file descriptor for connection

  Function value:
    MSCB_SUCCESS            Successful completion

\********************************************************************/
{
   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd)) {
      mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_EXIT, mscb_fd[fd - 1].remote_fd);
      mrpc_disconnect(mscb_fd[fd - 1].fd);
   }

   if (mscb_fd[fd - 1].type == MSCB_TYPE_USB)
      musb_close(mscb_fd[fd - 1].hr, mscb_fd[fd - 1].hw);

   /* outcommented, other client (like Labview) could still use LPT ...
   if (mscb_fd[fd - 1].type == MSCB_TYPE_LPT)
      lpt_close(mscb_fd[fd - 1].fd);
   */

   memset(&mscb_fd[fd - 1], 0, sizeof(MSCB_FD));

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

void mscb_cleanup(int sock)
/* Called by mrpc_server_loop to remove stale fd's on broken connection */
{
   int i;

   for (i = 0; i < MSCB_MAX_FD; i++)
      if (mscb_fd[i].fd && mscb_fd[i].remote_fd == sock)
         memset(&mscb_fd[i], 0, sizeof(MSCB_FD));
}

/*------------------------------------------------------------------*/

void mscb_get_device(int fd, char *device, int bufsize)
/********************************************************************\

  Routine: mscb_get_device

  Purpose: Return device name for fd

  Input:
    int fd                  File descriptor obtained wiv mscb_init()
    int bufsize             Size of device string
    char *device            device name, "" if invalid fd
                            
\********************************************************************/
{
   char str[256];

   if (!device)
      return;

   *device = 0;
   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return;

   if (mrpc_connected(fd)) {
      mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_GET_DEVICE, 
                mscb_fd[fd - 1].remote_fd, str, sizeof(str));
   }

   strcpy(device, mscb_fd[fd-1].device);
}

/*------------------------------------------------------------------*/

void mscb_check(char *device, int size)
/********************************************************************\

  Routine: mscb_check

  Purpose: Check IO pins of port

  Input:
    char *device            Under NT: lpt1 or lpt2
                            Under Linux: /dev/parport0 or /dev/parport1

  Function value:
    int fd                  device descriptor for connection, -1 if
                            error

\********************************************************************/
{
   int i, fd, d;

   mscb_init(device, size, 0);
   fd = 1;

   mscb_lock(fd);

   printf("Toggling %s output pins, hit ENTER to stop.\n", device);
   printf("GND = 19-25, toggling 2-9, 1, 14, 16 and 17\n\n");
   do {
      printf("\r11111111 1111");
      fflush(stdout);
      pp_wdata(fd, 0xFF);
      pp_wcontrol(fd, LPT_STROBE, 1);
      pp_wcontrol(fd, LPT_ACK, 1);
      pp_wcontrol(fd, LPT_RESET, 1);
      pp_wcontrol(fd, LPT_BIT9, 1);

      Sleep(1000);

      printf("\r00000000 0000");
      fflush(stdout);
      pp_wdata(fd, 0);
      pp_wcontrol(fd, LPT_STROBE, 0);
      pp_wcontrol(fd, LPT_ACK, 0);
      pp_wcontrol(fd, LPT_RESET, 0);
      pp_wcontrol(fd, LPT_BIT9, 0);

      Sleep(1000);

   } while (!kbhit());

   while (kbhit())
      getch();

   printf("\n\n\nInput display, hit ENTER to stop.\n");
   printf("Pins 2-9, 10 and 11\n\n");

   do {
      d = pp_rdata(fd);
      for (i = 0; i < 8; i++) {
         printf("%d", (d & 1) > 0);
         d >>= 1;
      }

      printf(" %d%d\r", !pp_rstatus(fd, LPT_DATAREADY), pp_rstatus(fd, LPT_BUSY));
      fflush(stdout);

      Sleep(100);
   } while (!kbhit());

   while (kbhit())
      getch();

   mscb_release(fd);
}

/*------------------------------------------------------------------*/

int mscb_addr(int fd, int cmd, int adr, int retry, int lock)
/********************************************************************\

  Routine: mscb_addr

  Purpose: Address node or nodes, only used internall from read and
           write routines. A MSCB lock has to be obtained outside
           of this routine!

  Input:
    int fd                  File descriptor for connection
    int cmd                 Addressing mode, one of
                              MCMD_ADDR_BC
                              MCMD_ADDR_NODE8
                              MCMD_ADDR_GRP8
                              MCMD_PING8
                              MCMD_ADDR_NODE16
                              MCMD_ADDR_GRP16
                              MCMD_PING16

    int adr                 Node or group address
    int retry               Number of retries
    int lock                Lock MSCB if TRUE

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving ping acknowledge
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   unsigned char buf[10];
   int i, n, status;

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_ADDR, mscb_fd[fd - 1].remote_fd, cmd, adr, retry, lock);

   if (lock) {
      if (mscb_lock(fd) != MSCB_SUCCESS)
         return MSCB_MUTEX;
   }

   for (n = 0; n < retry; n++) {
      buf[0] = cmd;

      if (cmd == MCMD_ADDR_NODE8 || cmd == MCMD_ADDR_GRP8) {
         buf[1] = (unsigned char) adr;
         buf[2] = crc8(buf, 2);
         status = mscb_out(fd, buf, 3, RS485_FLAG_BIT9 | RS485_FLAG_SHORT_TO | RS485_FLAG_NO_ACK);
      } else if (cmd == MCMD_PING8) {
         buf[1] = (unsigned char) adr;
         buf[2] = crc8(buf, 2);
         status = mscb_out(fd, buf, 3, RS485_FLAG_BIT9 | RS485_FLAG_SHORT_TO);
      } else if (cmd == MCMD_ADDR_NODE16 || cmd == MCMD_ADDR_GRP16) {
         buf[1] = (unsigned char) (adr >> 8);
         buf[2] = (unsigned char) (adr & 0xFF);
         buf[3] = crc8(buf, 3);
         status = mscb_out(fd, buf, 4, RS485_FLAG_BIT9 | RS485_FLAG_SHORT_TO | RS485_FLAG_NO_ACK);
      } else if (cmd == MCMD_PING16) {
         buf[1] = (unsigned char) (adr >> 8);
         buf[2] = (unsigned char) (adr & 0xFF);
         buf[3] = crc8(buf, 3);
         status = mscb_out(fd, buf, 4, RS485_FLAG_BIT9 | RS485_FLAG_SHORT_TO);
      } else {
         buf[1] = crc8(buf, 1);
         status = mscb_out(fd, buf, 2, RS485_FLAG_BIT9 | RS485_FLAG_SHORT_TO | RS485_FLAG_NO_ACK);
      }

      if (status != MSCB_SUCCESS) {
         if (lock)
            mscb_release(fd);
         return MSCB_SUBM_ERROR;
      }

      if (cmd == MCMD_PING8 || cmd == MCMD_PING16) {
         /* read back ping reply, 4ms timeout (for USB!) */
         i = mscb_in(fd, buf, 1, 4000);

         if (i == MSCB_SUCCESS && buf[0] == MCMD_ACK) {
            if (lock)
               mscb_release(fd);
            return MSCB_SUCCESS;
         }

         if (retry > 1) {
            /* send 0's to overflow partially filled node receive buffer */
            memset(buf, 0, sizeof(buf));
            mscb_out(fd, buf, 10, RS485_FLAG_BIT9 | RS485_FLAG_NO_ACK);

            /* wait some time */
            Sleep(10);
         }

         /* try again.... */
      } else {
         if (lock)
            mscb_release(fd);
         return MSCB_SUCCESS;
      }
   }

   if (lock)
      mscb_release(fd);

   return MSCB_TIMEOUT;
}

/*------------------------------------------------------------------*/

int mscb_reboot(int fd, int adr)
/********************************************************************\

  Routine: mscb_reboot

  Purpose: Reboot node by sending MCMD_INIT

  Input:
    int fd                  File descriptor for connection
    int adr                 Node address

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_MUTEX              Cannot obtain mutex for mscb
    MSCB_TIMEOUT            Timeout receiving ping acknowledge

\********************************************************************/
{
   unsigned char buf[10];
   int status;

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_REBOOT, mscb_fd[fd - 1].remote_fd, adr);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   buf[0] = MCMD_INIT;
   buf[1] = crc8(buf, 1);
   mscb_out(fd, buf, 2, RS485_FLAG_NO_ACK);

   mscb_release(fd);

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_reset(int fd)
/********************************************************************\

  Routine: mscb_reset

  Purpose: Reset submaster via hardware reset

  Input:
    int fd                  File descriptor for connection

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_RESET, mscb_fd[fd - 1].remote_fd);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   if (mscb_fd[fd - 1].type == MSCB_TYPE_LPT) {
      /* toggle reset */
      pp_wcontrol(fd, LPT_RESET, 1);
      Sleep(100);               // for elko
      pp_wcontrol(fd, LPT_RESET, 0);
   } else if (mscb_fd[fd - 1].type == MSCB_TYPE_USB) {

      char buf[10];

      buf[0] = MCMD_INIT;
      mscb_out(fd, buf, 1, RS485_FLAG_CMD);
      musb_close(mscb_fd[fd - 1].hr, mscb_fd[fd - 1].hw);
      Sleep(1000);
      musb_init(atoi(mscb_fd[fd - 1].device + 3), &mscb_fd[fd - 1].hr, &mscb_fd[fd - 1].hw);
   }

   mscb_release(fd);

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_ping(int fd, int adr)
/********************************************************************\

  Routine: mscb_ping

  Purpose: Ping node to see if it's alive

  Input:
    int fd                  File descriptor for connection
    int adr                 Node address

  Output:
    none

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving data
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   int status;

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_PING, mscb_fd[fd - 1].remote_fd, adr);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 1, 0);

   mscb_release(fd);
   return status;
}

/*------------------------------------------------------------------*/

int mscb_info(int fd, int adr, MSCB_INFO * info)
/********************************************************************\

  Routine: mscb_info

  Purpose: Retrieve info on addressd node

  Input:
    int fd                  File descriptor for connection
    int adr                 Node address

  Output:
    MSCB_INFO *info         Info structure defined in mscb.h

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving data
    MSCB_CRC_ERROR          CRC error
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   int i, status;
   unsigned char buf[256];

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_INFO, mscb_fd[fd - 1].remote_fd, adr, info);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   buf[0] = MCMD_GET_INFO;
   buf[1] = crc8(buf, 1);
   mscb_out(fd, buf, 2, RS485_FLAG_LONG_TO);

   i = mscb_in(fd, buf, sizeof(buf), 10000);
   mscb_release(fd);

   if (i < (int) sizeof(MSCB_INFO) + 2)
      return MSCB_TIMEOUT;

   memcpy(info, buf + 2, sizeof(MSCB_INFO));

   /* do CRC check */
   if (crc8(buf, sizeof(MSCB_INFO) + 2) != buf[sizeof(MSCB_INFO) + 2])
      return MSCB_CRC_ERROR;

   WORD_SWAP(&info->node_address);
   WORD_SWAP(&info->group_address);
   WORD_SWAP(&info->watchdog_resets);

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_info_variable(int fd, int adr, int index, MSCB_INFO_VAR * info)
/********************************************************************\

  Routine: mscb_info_variable

  Purpose: Retrieve info on a specific node variable

  Input:
    int fd                  File descriptor for connection
    int adr                 Node address
    int index               Variable index 0..255

  Output:
    MSCB_INFO_VAR *info     Info structure defined in mscb.h

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving data
    MSCB_CRC_ERROR          CRC error

    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   int i, status;
   unsigned char buf[80];

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_INFO_VARIABLE,
                       mscb_fd[fd - 1].remote_fd, adr, index, info);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   buf[0] = MCMD_GET_INFO + 1;
   buf[1] = index;
   buf[2] = crc8(buf, 2);
   mscb_out(fd, buf, 3, RS485_FLAG_LONG_TO);

   i = mscb_in(fd, buf, sizeof(buf), 10000);
   mscb_release(fd);

   if (i < (int) sizeof(MSCB_INFO_VAR) + 3)
      return MSCB_TIMEOUT;

   memcpy(info, buf + 2, sizeof(MSCB_INFO_VAR));

   /* do CRC check */
   if (crc8(buf, sizeof(MSCB_INFO_VAR) + 2) != buf[sizeof(MSCB_INFO_VAR) + 2])
      return MSCB_CRC_ERROR;

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_set_addr(int fd, int adr, int node, int group)
/********************************************************************\

  Routine: mscb_set_addr

  Purpose: Set node and group address of an node

  Input:
    int fd                  File descriptor for connection
    int adr                 Node address
    int node                16-bit node address
    int group               16-bit group address

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   unsigned char buf[8];
   int status;

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_SET_ADDR, mscb_fd[fd - 1].remote_fd, adr, node, group);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   /* check if destination address is alive */
   status = mscb_addr(fd, MCMD_PING16, node, 10, 0);
   if (status == MSCB_SUCCESS) {
      mscb_release(fd);
      return MSCB_ADDR_EXISTS;
   }

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   buf[0] = MCMD_SET_ADDR;
   buf[1] = (unsigned char) (node >> 8);
   buf[2] = (unsigned char) (node & 0xFF);
   buf[3] = (unsigned char) (group >> 8);
   buf[4] = (unsigned char) (group & 0xFF);
   buf[5] = crc8(buf, 5);
   mscb_out(fd, buf, 6, RS485_FLAG_NO_ACK);

   mscb_release(fd);

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_set_name(int fd, int adr, char *name)
/********************************************************************\

  Routine: mscb_set_name

  Purpose: Set node name

  Input:
    int fd                  File descriptor for connection
    int adr                 Node address
    char *name              New node name, up to 16 characters

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   unsigned char buf[256];
   int status, i;

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_SET_NAME, mscb_fd[fd - 1].remote_fd, adr, name);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {

      mscb_release(fd);
      return status;
   }

   buf[0] = MCMD_SET_ADDR | 0x07;
   for (i = 0; i < 16 && name[i]; i++)
      buf[2 + i] = name[i];

   if (i < 16)
      buf[2 + (i++)] = 0;

   buf[1] = i;                  /* varibale buffer length */

   buf[2 + i] = crc8(buf, 2 + i);
   mscb_out(fd, buf, 3 + i, RS485_FLAG_NO_ACK);

   mscb_release(fd);

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_write_group(int fd, int adr, unsigned char index, void *data, int size)
/********************************************************************\

  Routine: mscb_write_na

  Purpose: Write data to channels on group of nodes

  Input:
    int fd                  File descriptor for connection
    int adr                 Group address
    unsigned char index     Variable index 0..255
    unsigned int  data      Data to send
    int size                Data size in bytes 1..4 for byte, word,
                            and dword

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   int i, status;
   unsigned char *d;
   unsigned char buf[256];

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_WRITE_GROUP,
                       mscb_fd[fd - 1].remote_fd, adr, index, data, size);

   if (size > 4 || size < 1)
      return MSCB_INVAL_PARAM;

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_ADDR_GRP16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   buf[0] = MCMD_WRITE_NA + size + 1;
   buf[1] = index;

   for (i = 0, d = data; i < size; i++)
      buf[2 + size - 1 - i] = *d++;

   buf[2 + i] = crc8(buf, 2 + i);
   mscb_out(fd, buf, 3 + i, RS485_FLAG_NO_ACK);

   mscb_release(fd);

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_write(int fd, int adr, unsigned char index, void *data, int size)
/********************************************************************\

  Routine: mscb_write

  Purpose: Write data to variable on single node

  Input:
    int fd                  File descriptor for connection
    int adr                 Node address
    unsigned char index     Variable index 0..255
    void *data              Data to send
    int size                Data size in bytes 1..4 for byte, word,
                            and dword

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving acknowledge
    MSCB_CRC_ERROR          CRC error
    MSCB_INVAL_PARAM        Parameter "size" has invalid value
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   int i, status;
   unsigned char buf[256], crc, ack[2];
   unsigned char *d;

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_WRITE, mscb_fd[fd - 1].remote_fd, adr, index, data, size);

   if (size < 1)
      return MSCB_INVAL_PARAM;

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   if (size < 6) {
      buf[0] = MCMD_WRITE_ACK + size + 1;
      buf[1] = index;

      /* reverse order for WORD & DWORD */
      if (size < 5)
         for (i = 0, d = data; i < size; i++)
            buf[2 + size - 1 - i] = *d++;
      else
         for (i = 0, d = data; i < size; i++)
            buf[2 + i] = *d++;

      crc = crc8(buf, 2 + i);
      buf[2 + i] = crc;
      mscb_out(fd, buf, 3 + i, 0);
   } else {
      buf[0] = MCMD_WRITE_ACK + 7;
      buf[1] = size + 1;
      buf[2] = index;

      for (i = 0, d = data; i < size; i++)
         buf[3 + i] = *d++;

      crc = crc8(buf, 3 + i);
      buf[3 + i] = crc;
      mscb_out(fd, buf, 4 + i, 0);
   }

   /* read acknowledge */
   i = mscb_in(fd, ack, 2, 10000);
   mscb_release(fd);
   if (i < 2)
      return MSCB_TIMEOUT;

   if (ack[0] != MCMD_ACK || ack[1] != crc)
      return MSCB_CRC_ERROR;

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_write_block(int fd, int adr, unsigned char index, void *data, int size)
/********************************************************************\

  Routine: mscb_write_block

  Purpose: Write block of data to single node

  Input:
    int fd                  File descriptor for connection
    int adr                 Node address
    unsigned char index     Variable index 0..255
    void *data              Data to send
    int size                Data size in bytes

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving acknowledge
    MSCB_CRC_ERROR          CRC error
    MSCB_INVAL_PARAM        Parameter "size" has invalid value
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   int i, n, status;
   unsigned char buf[1024];

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_WRITE, mscb_fd[fd - 1].remote_fd, adr, index, data, size);

   if (size < 1)
      return MSCB_INVAL_PARAM;

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   /*
   Bulk test gave 128kb/sec, 10.3.04, SR

   for (i=0 ; i<1024 ; i++)
      buf[i] = RS485_FLAG_CMD;
   
   i = msend_usb(mscb_fd[fd - 1].hw, buf, 1024);
   mscb_release(fd);
   return MSCB_SUCCESS;
   */

   /* slice data in 60 byte blocks */
   for (i=0 ; i<=size/60 ; i++) {
      buf[0] = MCMD_WRITE_BLOCK;

      n = 60;
      if (size < (i+1)*60)
         n = size - i*60;

      if (n == 0)
         break;

      memcpy(buf+1, (char *)data+i*60, n);
      mscb_out(fd, buf, n+1, RS485_FLAG_CMD);
   }

   mscb_release(fd);

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_flash(int fd, int adr)
/********************************************************************\

  Routine: mscb_flash

  Purpose: Flash node variables to EEPROM


  Input:
    int fd                  File descriptor for connection
    int adr                 Node address
                            and dword

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving acknowledge
    MSCB_CRC_ERROR          CRC error
    MSCB_INVAL_PARAM        Parameter "size" has invalid value
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   int i, status;
   unsigned char buf[10], crc, ack[2];

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_FLASH, mscb_fd[fd - 1].remote_fd, adr);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   buf[0] = MCMD_FLASH;
   crc = crc8(buf, 1);
   buf[1] = crc;
   mscb_out(fd, buf, 2, RS485_FLAG_LONG_TO);

   /* read acknowledge, 100ms timeout */
   i = mscb_in(fd, ack, 2, 100000);
   mscb_release(fd);

   if (i < 2)
      return MSCB_TIMEOUT;

   if (ack[0] != MCMD_ACK || ack[1] != crc)
      return MSCB_CRC_ERROR;

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_upload(int fd, int adr, char *buffer, int size, int debug)
/********************************************************************\

  Routine: mscb_upload

  Purpose: Upload new firmware to node


  Input:
    int fd                  File descriptor for connection
    int adr                 Node address
    char *filename          File name for Intel HEX file
    int debug               If true, produce detailed debugging output

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving acknowledge
    MSCB_CRC_ERROR          CRC error
    MSCB_INVAL_PARAM        Parameter "size" has invalid value
    MSCB_MUTEX              Cannot obtain mutex for mscb
    MSCB_FORMAT_ERROR       Error in HEX file format

\********************************************************************/
{
   unsigned char buf[512], crc, ack[2], image[0x10000], *line;
   unsigned int len, ofh, ofl, type, d;
   int i, status, page, flash_size, n_page, last_page, retry;
   unsigned short ofs;

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_UPLOAD, mscb_fd[fd - 1].remote_fd, adr, buffer, size);

   /* interprete HEX file */
   memset(image, 0xFF, sizeof(image));
   line = buffer;
   flash_size = 0;
   do {
      if (line[0] == ':') {
         sscanf(line + 1, "%02x%02x%02x%02x", &len, &ofh, &ofl, &type);
         ofs = (ofh << 8) | ofl;

         for (i = 0; i < (int) len; i++) {
            sscanf(line + 9 + i * 2, "%02x", &d);
            image[ofs + i] = d;
         }
         
         flash_size += len;
         line = strchr(line, '\r') + 1;
         if (line && *line == '\n')
            line++;
      } else
         return MSCB_FORMAT_ERROR;

   } while (*line);

   if (debug) {
      /* count pages and byes */
      n_page = 0;
      last_page = -1;
      for (page = 0; page < 128; page++) {
         /* check if page contains data */
         for (i = 0; i < 512; i++) {
            if (image[page * 512 + i] != 0xFF) {
               if (page != last_page) {
                  last_page = page;
                  n_page++;
               }
            }
         }
      }

      printf("Found %d valid pages (%d bytes) in HEX file\n", n_page, flash_size);
   }

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   /* send upgrade command */
   buf[0] = MCMD_UPGRADE;
   crc = crc8(buf, 1);
   buf[1] = crc;
   mscb_out(fd, buf, 2, RS485_FLAG_LONG_TO);

   /* wait for ready, 1 sec timeout */
   if (mscb_in(fd, ack, 2, 1000000) != 2) {
      printf("Error: timeout receiving upgrade acknowledge from remote node\n");

      /* send exit upgrade command, in case node gets to upgrade routine later */
      buf[0] = 6;
      mscb_out(fd, buf, 1, RS485_FLAG_LONG_TO);

      mscb_release(fd);
      return MSCB_TIMEOUT;
   }

   if (debug)
      printf("Received acknowledge for upgrade command\n");

   /* program pages up to 64k */
   for (page = 0; page < 128; page++) {
      /* check if page contains data */
      for (i = 0; i < 512; i++)
         if (image[page * 512 + i] != 0xFF)
            break;

      retry = 0;

      if (i < 512) {
         do {
            if (debug)
               printf("Erase page   0x%04X - ", page * 512);
            else
               printf("E");
            fflush(stdout);

            /* erase page */
            buf[0] = 2;
            buf[1] = page;
            mscb_out(fd, buf, 2, RS485_FLAG_LONG_TO);

            if (mscb_in(fd, ack, 2, 100000) != 2) {
               printf("\nError: timeout from remote node for erase page 0x%04X\n", page * 512);
               goto prog_error;
            }

            if (ack[1] == 0xFF) {
               /* protected page, so finish */
               if (debug)
                  printf("found protected page, exit\n");
               else
                  printf("\bX");
               fflush(stdout);
               goto prog_error;
            }

            if (ack[0] != MCMD_ACK) {
               printf("\nError: received wrong acknowledge for erase page 0x%04X\n", page * 512);
               goto prog_error;
            }

            if (debug)
               printf("ok\n");

            if (debug)
               printf("Program page 0x%04X - ", page * 512);
            else
               printf("\bP");
            fflush(stdout);

            /* program page */
            buf[0] = 3;
            buf[1] = page;
            mscb_out(fd, buf, 2, RS485_FLAG_NO_ACK);


            for (i = crc = 0; i < 512; i++) {
               buf[i] = image[page * 512 + i];
               crc += buf[i];
            }

            /* chop down page in 60 byte segments (->USB) */
            for (i = 0; i < 8; i++) {
               mscb_out(fd, buf + i * 60, 60, RS485_FLAG_LONG_TO);

               /* read acknowledge */
               if (mscb_in(fd, ack, 2, 100000) != 2) {
                  printf("\nError: timeout from remote node for program page 0x%04X\n", page * 512);
                  goto prog_error;
               }
            }

            /* send remaining 32 bytes */
            mscb_out(fd, buf + i * 60, 32, RS485_FLAG_LONG_TO);

            /* read acknowledge */
            if (mscb_in(fd, ack, 2, 100000) != 2) {
               printf("\nError: timeout from remote node for program page 0x%04X\n", page * 512);
               goto prog_error;
            }

            if (ack[0] != MCMD_ACK) {
               printf("\nError: received wrong acknowledge for program page 0x%04X\n", page * 512);
               goto prog_error;
            }

            if (debug)
               printf("ok\n");

            /* verify page */
            if (debug)
               printf("Verify page  0x%04X - ", page * 512);
            else
               printf("\bV");
            fflush(stdout);

            /* verify page */
            buf[0] = 4;
            buf[1] = page;
            mscb_out(fd, buf, 2, RS485_FLAG_LONG_TO);

            if (mscb_in(fd, ack, 2, 100000) != 2) {
               printf("\nError: timeout from remote node for verify page 0x%04X\n", page * 512);
               goto prog_error;
            }

            /* compare CRCs */
            if (ack[1] == crc) {
               if (debug)
                  printf("ok (CRC = 0x%02X)\n", crc);
               break;
            }

            if (debug)
               printf("CRC error (0x%02X != 0x%02X)\n", crc, ack[1]);

            /* reprogram page until verified */
            retry++;
            if (retry == 10) {
               printf("\nToo many retries, aborting\n");
               goto prog_error;
               break;
            }

         } while (1);

         if (!debug)
            printf("\b=");
         fflush(stdout);
      }
   }

 prog_error:
   printf("\n");

   /* reboot node */
   buf[0] = 5;
   mscb_out(fd, buf, 1, RS485_FLAG_NO_ACK);

   mscb_release(fd);
   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_read(int fd, int adr, unsigned char index, void *data, int *size)
/********************************************************************\

  Routine: mscb_read

  Purpose: Read data from variable on node

  Input:
    int fd                  File descriptor for connection
    int adr                 Node address
    unsigned char index     Variable index 0..255
    int size                Buffer size for data

  Output:
    void *data              Received data
    int  *size              Number of received bytes

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving acknowledge
    MSCB_CRC_ERROR          CRC error
    MSCB_INVAL_PARAM        Parameter "size" has invalid value
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   int i, n, status;
   unsigned char buf[256], crc;

   memset(data, 0, *size);

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_READ, mscb_fd[fd - 1].remote_fd, adr, index, data, size);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   /* try ten times */
   for (n = 0; n < 10; n++) {
      /* after five times, reset submaster */
      if (n == 5) {
#ifndef _USRDLL
         printf("Automatic submaster reset.\n");
#endif

         mscb_reset(fd);
         Sleep(100);
      }

      buf[0] = MCMD_READ + 1;
      buf[1] = index;
      buf[2] = crc8(buf, 2);
      mscb_out(fd, buf, 3, 0);

      /* read data */
      i = mscb_in(fd, buf, sizeof(buf), 10000);

      if (i == 1 && buf[0] == 0xFF) {
#ifndef _USRDLL
         printf("Timeout from RS485 bus.\n");
#endif
         return MSCB_TIMEOUT;
      }

      if (i < 2)
         continue;

      crc = crc8(buf, i - 1);

      if ((buf[0] != MCMD_ACK + i - 2 && buf[0] != MCMD_ACK + 7)
          || buf[i - 1] != crc)
         continue;

      if (buf[0] == MCMD_ACK + 7) {
         if (i - 3 > *size) {
            mscb_release(fd);
            *size = 0;
            return MSCB_NO_MEM;
         }

         memcpy(data, buf + 2, i - 3);  // variable length
         *size = i - 3;
      } else {
         if (i - 2 > *size) {
            mscb_release(fd);
            *size = 0;
            return MSCB_NO_MEM;
         }

         memcpy(data, buf + 1, i - 2);
         *size = i - 2;
      }

      if (i - 2 == 2)
         WORD_SWAP(data);
      if (i - 2 == 4)
         DWORD_SWAP(data);

      mscb_release(fd);
      return MSCB_SUCCESS;
   }

   mscb_release(fd);

   if (i < 2)
      return MSCB_TIMEOUT;

   return MSCB_CRC_ERROR;
}

/*------------------------------------------------------------------*/

int mscb_read_range(int fd, int adr, unsigned char index1, unsigned char index2, void *data, int *size)
/********************************************************************\

  Routine: mscb_read

  Purpose: Read data from channel on node

  Input:
    int fd                  File descriptor for connection
    int adr                 Node address
    unsigned char index1    First index to read
    unsigned char index2    Last index to read
    int size                Buffer size for data

  Output:
    void *data              Received data
    int  *size              Number of received bytes

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving acknowledge
    MSCB_CRC_ERROR          CRC error
    MSCB_INVAL_PARAM        Parameter "size" has invalid value
    MSCB_MUTEX              Cannot obtain mutex for mscb

\********************************************************************/
{
   int i, n, status;
   unsigned char buf[256], crc;

   memset(data, 0, *size);

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_READ_RANGE,
                       mscb_fd[fd - 1].remote_fd, adr, index1, index2, data, size);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   /* try ten times */
   for (n = 0; n < 10; n++) {
      /* after five times, reset submaster */
      if (n == 5) {
#ifndef _USRDLL
         printf("Automatic submaster reset.\n");
#endif

         mscb_reset(fd);
         Sleep(100);
      }

      buf[0] = MCMD_READ + 2;
      buf[1] = index1;
      buf[2] = index2;
      buf[3] = crc8(buf, 3);
      mscb_out(fd, buf, 4, 0);

      /* read data */
      i = mscb_in(fd, buf, 256, 10000);

      if (i < 2)
         continue;

      crc = crc8(buf, i - 1);

      if (buf[0] != MCMD_ACK + 7 || buf[i - 1] != crc)
         continue;

      memcpy(data, buf + 2, i - 3);
      *size = i - 3;

      mscb_release(fd);
      return MSCB_SUCCESS;
   }

   mscb_release(fd);

   if (i < 2)
      return MSCB_TIMEOUT;

   return MSCB_CRC_ERROR;
}

/*------------------------------------------------------------------*/

int mscb_user(int fd, int adr, void *param, int size, void *result, int *rsize)
/********************************************************************\

  Routine: mscb_user

  Purpose: Call user function on node

  Input:
    int  fd                 File descriptor for connection
    int adr                 Node address
    char *param             Parameters passed to user function, no CRC code
    int  size               Size of parameters in bytes
    int  *rsize             Size of result buffer

  Output:
    char *result            Optional return parameters
    int  *rsize             Number of returned size

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving data
    MSCB_CRC_ERROR          CRC error
    MSCB_MUTEX              Cannot obtain mutex for mscb
    MSCB_FORMAT_ERROR       "size" parameter too large

\********************************************************************/
{
   int i, n, status;
   unsigned char buf[80];

   memset(result, 0, *rsize);

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_USER,
                       mscb_fd[fd - 1].remote_fd, adr, param, size, result, rsize);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   status = mscb_addr(fd, MCMD_PING16, adr, 10, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   if (size > 4 || size < 0)
      return MSCB_FORMAT_ERROR;

   buf[0] = MCMD_USER + size;

   for (i = 0; i < size; i++)
      buf[1 + i] = ((char *) param)[i];

   /* add CRC code and send data */
   buf[1 + i] = crc8(buf, 1 + i);
   status = mscb_out(fd, buf, 2 + i, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   if (result == NULL) {
      mscb_release(fd);
      return MSCB_SUCCESS;
   }

   /* read result */
   n = mscb_in(fd, buf, sizeof(buf), 10000);
   mscb_release(fd);

   if (n < 0)
      return MSCB_TIMEOUT;

   if (rsize)
      *rsize = n - 2;
   for (i = 0; i < n - 2; i++)
      ((char *) result)[i] = buf[1 + i];

   if (buf[n - 1] != crc8(buf, n - 1))
      return MSCB_CRC_ERROR;

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

int mscb_echo(int fd, int adr, unsigned char d1, unsigned char *d2)
/********************************************************************\

  Routine: mscb_echo

  Purpose: Send byte and receive echo, useful for testing

  Input:
    int  fd                 File descriptor for connection
    int  adr                Node address
    unsigned char d1        Byte to send

  Output:
    unsigned char *d2       Received byte

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving data
    MSCB_CRC_ERROR          CRC error
    MSCB_MUTEX              Cannot obtain mutex for mscb
    MSCB_FORMAT_ERROR       "size" parameter too large

\********************************************************************/
{
   int n, status;
   unsigned char buf[80];

   *d2 = 0xFF;

   if (fd > MSCB_MAX_FD || fd < 1 || !mscb_fd[fd - 1].type)
      return MSCB_INVAL_PARAM;

   if (mrpc_connected(fd))
      return mrpc_call(mscb_fd[fd - 1].fd, RPC_MSCB_ECHO, mscb_fd[fd - 1].remote_fd, adr, d1, d2);

   if (mscb_lock(fd) != MSCB_SUCCESS)
      return MSCB_MUTEX;

   if (adr) {
      status = mscb_addr(fd, MCMD_PING16, adr, 1, 0);
      if (status != MSCB_SUCCESS) {
         mscb_release(fd);
         return status;
      }
   }

   buf[0] = MCMD_ECHO;
   buf[1] = d1;

   /* add CRC code and send data */
   buf[2] = crc8(buf, 2);
   status = mscb_out(fd, buf, 3, 0);
   if (status != MSCB_SUCCESS) {
      mscb_release(fd);
      return status;
   }

   /* read result */
   n = mscb_in(fd, buf, sizeof(buf), 10000);
   mscb_release(fd);

   if (n < 0)
      return MSCB_TIMEOUT;

   *d2 = buf[1];

   if (buf[n - 1] != crc8(buf, n - 1))
      return MSCB_CRC_ERROR;

   return MSCB_SUCCESS;
}

/*------------------------------------------------------------------*/

unsigned long millitime()
{
#ifdef _MSC_VER
   return (int) GetTickCount();
#else
   {
      struct timeval tv;

      gettimeofday(&tv, NULL);

      return tv.tv_sec * 1000 + tv.tv_usec / 1000;
   }
#endif
}

/*------------------------------------------------------------------*/

int mscb_link(int fd, int adr, unsigned char index, void *data, int size)
/********************************************************************\

  Routine: mscb_link

  Purpose: Used by LabView to link controls to MSCB variables

  Input:
    int  fd                 File descriptor for connection
    int  adr                Node address
    unsigned char index     Variable index
    void *data              Pointer to data
    int size                Size of data

  Output:
    void *data              Readback data

  Function value:
    MSCB_SUCCESS            Successful completion
    MSCB_TIMEOUT            Timeout receiving data
    MSCB_CRC_ERROR          CRC error
    MSCB_MUTEX              Cannot obtain mutex for mscb
    MSCB_FORMAT_ERROR       "size" parameter too large
    MSCB_NO_MEM             Out of memory

\********************************************************************/
{
   int i, s, status;
   MSCB_INFO_VAR info;

   debug_log("mscb_link( %d %d %d * %d)\n", fd, adr, index, size);

   /* check if variable in cache */
   for (i = 0; i < n_cache; i++)
      if (cache[i].fd == fd && cache[i].adr == adr && cache[i].index == index)
         break;

   if (i < n_cache) {
      if (memcmp(data, cache[i].data, cache[i].size) != 0) {
         /* data has changed, send update */
         memcpy(cache[i].data, data, cache[i].size);
         mscb_write(fd, adr, index, data, cache[i].size);
      } else {
         /* retrieve data from node */
         if (millitime() > cache[i].last + CACHE_PERIOD) {
            cache[i].last = millitime();

            s = cache[i].size;
            status = mscb_read(fd, adr, index, cache[i].data, &s);
            if (status != MSCB_SUCCESS)
               return status;

            memcpy(data, cache[i].data, size);
         }
      }
   } else {
      /* add new entry in cache */
      if (n_cache == 0)
         cache = (CACHE_ENTRY *) malloc(sizeof(CACHE_ENTRY));
      else
         cache = (CACHE_ENTRY *) realloc(cache, sizeof(CACHE_ENTRY) * (n_cache + 1));

      if (cache == NULL)
         return MSCB_NO_MEM;

      /* get variable size from node */
      status = mscb_info_variable(fd, adr, index, &info);
      if (status != MSCB_SUCCESS)
         return status;

      /* setup cache entry */
      i = n_cache;
      n_cache++;

      cache[i].fd = fd;
      cache[i].adr = adr;
      cache[i].index = index;
      cache[i].last = 0;
      cache[i].size = info.width;

      /* allocate at least 4 bytes */
      s = info.width;
      if (s < 4)
         s = 4;
      cache[i].data = malloc(s);
      if (cache[i].data == NULL)
         return MSCB_NO_MEM;
      memset(cache[i].data, 0, s);

      /* read initial value */
      s = cache[i].size;
      status = mscb_read(fd, adr, index, cache[i].data, &s);
      if (status != MSCB_SUCCESS)
         return status;

      memcpy(data, cache[i].data, size);
   }

   return MSCB_SUCCESS;
}


/*------------------------------------------------------------------*/

int mscb_select_device(char *device, int size, int select)
/********************************************************************\

  Routine: mscb_select_device

  Purpose: Select MSCB submaster device. Show dialog if sevral
           possibilities

  Input:
    none

  Output:
    char *device            Device name
    int  size               Size of device string buffer in bytes
    int  select             If 1, ask user which device to select,
                            if zero, use first device (for Labview)

  Function value:
    MSCB_SUCCESS            Successful completion
    0                       No submaster found

\********************************************************************/
{
   char list[10][256], str[256];
   int status, i, n, index, error_code;

   n = 0;
   *device = 0;
   error_code = 0;

   /* search for temporary file descriptor */
   for (index = 0; index < MSCB_MAX_FD; index++)
      if (mscb_fd[index].fd == 0)
         break;

   if (index == MSCB_MAX_FD)
      return -1;

   /* check USB devices */
   for (i = 0; i < 127; i++) {
      status = musb_init(i, NULL, NULL);
      if (status != -1)
         sprintf(list[n++], "usb%d", i);
      else
         break;
   }

   /* check LPT devices */
   for (i = 0; i < 1; i++) {
#ifdef _MSC_VER
      sprintf(str, "lpt%d", i + 1);
#else
      sprintf(str, "/dev/parport%d", i);
#endif

      mscb_fd[index].type = MSCB_TYPE_LPT;

      status = lpt_init(str, index);
      if (status < 0)
         error_code = status;
      else
         lpt_close(mscb_fd[index].fd);
      memset(&mscb_fd[index], 0, sizeof(MSCB_FD));
      if (status == 0)
         sprintf(list[n++], str);
      else
         break;
   }

   if (n == 0) {
      if (error_code == -3)
         printf("mscb.c: Cannot access parallel port (No DirectIO driver installed)\n");
      if (error_code == -4)
         printf("mscb.c: Cannot access parallel port, locked by other application\n");
      return 0;
   }

   /* if only one device, return it */
   if (n == 1 || select == 0) {
      strlcpy(device, list[0], size);
      return MSCB_SUCCESS;
   }

   do {
      printf("Found several submasters, please select one:\n\n");
      for (i = 0; i < n; i++)
         printf("%d: %s\n", i + 1, list[i]);

      printf("\n");
      fgets(str, sizeof(str), stdin);
      i = atoi(str);
      if (i > 0 && i <= n) {
         strlcpy(device, list[i - 1], size);
         return MSCB_SUCCESS;
      }

      printf("Invalid selection, please enter again\n");
   } while (1);

   return MSCB_SUCCESS;
}
