/*********************************************************************

  Name:         mvmestd.h
  Created by:   Stefan Ritt

  Cotents:      Midas VME standard routines (MVMESTD) supplying an
                abstract layer to all supported VME interfaces. So far,
                bt617.c and sis3100.c are supported

  *** muCap patches ***
  - choose DWORD type appropriately for x86-64.
                
  $Log: mvmestd.h,v $
  Revision 1.2  2005/06/21 00:14:14  mucap
  (Fred)
  Choose DWORD type appropriately for x86-64.

  Revision 1.8  2004/09/10 12:33:47  midas
  Implemented SIS3100/1100

  Revision 1.7  2004/01/08 08:40:09  midas
  Implemented standard indentation

  Revision 1.6  2003/11/24 08:22:45  midas
  Changed timeouts from INT to DWORD, added ignore_timeout to cm_cleanup, adde '-f' flag to ODBEdit 'cleanup'

  Revision 1.5  2001/06/27 12:16:30  midas
  Added OS_IRIX

  Revision 1.4  2001/04/05 05:51:36  midas
  Added VME_LM

  Revision 1.3  2000/09/28 11:12:15  midas
  Added DMA flag to vme_read/vme_write

  Revision 1.2  2000/09/26 07:45:19  midas
  Added vme_write

  Revision 1.1  2000/09/26 07:26:56  midas
  Added file


*********************************************************************/

/*---- replacements if not running under MIDAS ---------------------*/

#ifndef MIDAS_TYPE_DEFINED
#define MIDAS_TYPE_DEFINED

typedef unsigned short int WORD;

#if defined(__alpha) || defined(__x86_64__)
typedef unsigned int DWORD;
#else
typedef unsigned long int DWORD;
#endif

#define SUCCESS  1

#endif                          /* MIDAS_TYPE_DEFINED */

/* make functions under WinNT dll exportable */
#if defined(_MSC_VER) && defined(MIDAS_DLL)
#define EXPRT __declspec(dllexport)
#else
#define EXPRT
#endif

/*---- status codes ------------------------------------------------*/

#define MVME_SUCCESS                  1
#define MVME_NO_INTERFACE             2
#define MVME_NO_CRATE                 3
#define MVME_UNSUPPORTED              4

/*---- types -------------------------------------------------------*/

typedef unsigned long mvme_addr_t;
typedef unsigned long mvme_size_t;

/*---- constants ---------------------------------------------------*/

/* data modes */
#define MVME_A16D16                   1
#define MVME_A16D32                   2
#define MVME_A24D16                   3
#define MVME_A24D32                   4
#define MVME_A32D16                   5
#define MVME_A32D32                   6
#define MVME_RAMD16                   7   /* RAM memory of VME adapter */
#define MVME_RAND32                   8
#define MVME_LM                       9   /* local memory mapped to VME */

/* vme_ioctl commands */
#define MVME_IOCTL_CRATE_SET          0
#define MVME_IOCTL_CRATE_GET          1
#define MVME_IOCTL_AMOD_SET           2
#define MVME_IOCTL_AMOD_GET           3
#define MVME_IOCTL_DMA_SET            4
#define MVME_IOCTL_DMA_GET            5
#define MVME_IOCTL_FIFO_SET           6
#define MVME_IOCTL_FIFO_GET           7

/* vme bus address modifiers */
#define MVME_AMOD_A32_SB     (0x0F)      /* A32 Extended Supervisory Block */
#define MVME_AMOD_A32_SP     (0x0E)      /* A32 Extended Supervisory Program */
#define MVME_AMOD_A32_SD     (0x0D)      /* A32 Extended Supervisory Data */
#define MVME_AMOD_A32_NB     (0x0B)      /* A32 Extended Non-Privileged Block */
#define MVME_AMOD_A32_NP     (0x0A)      /* A32 Extended Non-Privileged Program */
#define MVME_AMOD_A32_ND     (0x09)      /* A32 Extended Non-Privileged Data */
#define MVME_AMOD_A32_SMBLT  (0x0C)      /* A32 Multiplexed Block Transfer (D64) */
#define MVME_AMOD_A32_NMBLT  (0x08)      /* A32 Multiplexed Block Transfer (D64) */

#define MVME_AMOD_A32     MVME_AMOD_A32_SD
#define MVME_AMOD_A32_D64 MVME_AMOD_A32_SMBLT

#define MVME_AMOD_A24_SB     (0x3F)      /* A24 Standard Supervisory Block Transfer      */
#define MVME_AMOD_A24_SP     (0x3E)      /* A24 Standard Supervisory Program Access      */
#define MVME_AMOD_A24_SD     (0x3D)      /* A24 Standard Supervisory Data Access         */
#define MVME_AMOD_A24_NB     (0x3B)      /* A24 Standard Non-Privileged Block Transfer   */
#define MVME_AMOD_A24_NP     (0x3A)      /* A24 Standard Non-Privileged Program Access   */
#define MVME_AMOD_A24_ND     (0x39)      /* A24 Standard Non-Privileged Data Access      */
#define MVME_AMOD_A24_SMBLT  (0x3C)      /* A24 Multiplexed Block Transfer (D64) */
#define MVME_AMOD_A24_NMBLT  (0x38)      /* A24 Multiplexed Block Transfer (D64) */

#define MVME_AMOD_A24     MVME_AMOD_A24_SD
#define MVME_AMOD_A24_D64 MVME_AMOD_A24_SMBLT

#define MVME_AMOD_A16_SD  (0x2D) /* A16 Short Supervisory Data Access            */
#define MVME_AMOD_A16_ND  (0x29) /* A16 Short Non-Privileged Data Access         */

#define MVME_AMOD_A16     MVME_AMOD_A16_SD

/*---- function declarations ---------------------------------------*/

/* make functions callable from a C++ program */
#ifdef __cplusplus
extern "C" {
#endif

   int EXPRT mvme_init();
   int EXPRT mvme_ioctl(int req, int *parm);
   int EXPRT mvme_exit();
   int EXPRT mvme_read(void *dst, mvme_addr_t vme_addr, mvme_size_t size);
   int EXPRT mvme_write(mvme_addr_t vme_addr, void *src, mvme_size_t n_bytes);
   int EXPRT mvme_mmap(void **ptr, mvme_addr_t vme_addr, mvme_size_t size);
   int EXPRT mvme_unmap(void *ptr, mvme_size_t size);

#ifdef __cplusplus
}
#endif
